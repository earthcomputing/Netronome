/* Copyright (C) 2015,  Netronome Systems, Inc.  All rights reserved. */

#include <stdint.h>
#include <nfp.h>
#include <pif_common.h>
#include <pif_counters.h>

#include <nfp/me.h>
#include <nfp/mem_bulk.h>
#include <nfp/mem_ring.h>
#include <nfp/mem_atomic.h>
#include <nfp6000/nfp_me.h>

#ifndef PIF_GLOBAL_NFD_DISABLED
#include <vnic/shared/nfd_cfg.h>
#include <vnic/pci_in.h>
#include <vnic/pci_out.h>
#endif /* PIF_GLOBAL_NFD_DISABLED */

#include <blm.h>

#ifdef PIF_USE_GRO
#include <gro_cli.h>
#endif

#ifndef PIF_GLOBAL_FLOWCACHE_DISABLED
#include <flow_cache_c.h>
#include <flow_cache_lock_c.h>
#include <fc.h>
#endif /* PIF_GLOBAL_FLOWCACHE_DISABLED */

#include <hashmap.h>

#include <pif_pkt_duplication.h>

#include "system_init_c.h"
#include "pkt_ops.h"
#include "pkt_clone.h"
#include "mcast.h"

/***************************************************************************
 * Defines                                                                 *
 ***************************************************************************/

struct fc_control {
    union {
        struct {
            int16_t ingress_len;
            int16_t egress_len;
        };
        uint32_t _raw[0];
    };
};

/* special encoding for control lengths */
#define PIF_FC_CACHE_INITIAL -1
#define PIF_FC_CACHE_BYPASS  -2

/* always 1LW of control info */
#define PIF_FC_PAYLOAD_OFF 1


/* the BLM EMU ring to use (0 - 3) for MU buffer allocation  */
#define BLM_BLQ_0         0

/* drop packet */
#define PIF_EGRESS_DROP  0xffff

#ifdef FC_PAYLOAD_SIZE
#define ACTION_BUFFER_SIZE FC_PAYLOAD_SIZE
#else
#define ACTION_BUFFER_SIZE 256
#endif


/***************************************************************************
 * Globals                                                                 *
 ***************************************************************************/

#ifdef DEBUG
volatile __export __emem uint32_t pif_debug_enabled = 1;
#endif

__cls struct pif_pkt_info               pif_pkt_info_copy;
PIF_PKT_INFO_TYPE struct pif_pkt_info               pif_pkt_info_global;

volatile __lmem uint32_t                parrep[PIF_PARREP_LEN_LW + PIF_DEBUG_EXTRA_PARREP] = {0};
__cls  uint32_t                         parrep_copy[PIF_PARREP_LEN_LW];

#ifndef PIF_GLOBAL_FLOWCACHE_DISABLED
__lmem uint32_t                         flowkey[FC_MAX_KEY_LEN_LW];
#else
__lmem uint32_t                         flowkey[0];
#endif /* PIF_GLOBAL_FLOWCACHE_DISABLED */

__nnr struct pif_pkt_duplication_type   pkt_dup_state;

__nnr uint32_t pif_flowcache_bypass;

/* make sure only half the CTM is used for packet data */
/* TODO move the code below */
__asm .init_csr xpb:CTMXpbMap.MuPacketReg.MUPEMemConfig 1
__asm .alloc_mem PKT_RSVD ctm+0x0 island 0x20000 reserved

#ifndef PIF_GLOBAL_NFD_DISABLED
NFD_CFG_BASE_DECLARE(PCIE_ISL);
#endif /* PIF_GLOBAL_NFD_DISABLED */

/***************************************************************************
 * Forwarding                                                              *
 ***************************************************************************/

/* The #NBI and #port are encoded in P4 standard_metadata.egress_port */
__forceinline int forward(__lmem uint32_t *parrep)
{
    unsigned int port_spec = 0;

    //port_spec = pif_global_get_stdmd_egress_port(parrep);

    /* egress_port == -1 : drop */
    if (port_spec == PIF_EGRESS_DROP)
        return 1;

    /* egress_port[7] == dst */
#ifdef PIF_NO_NFD
    pif_pkt_info_global.__raw[PIF_PKT_INFO_EGRESS_PORT_wrd] = PIF_PKT_INFO_SET_PORT(PIF_PKT_SRC_NBI,
                                                                           PIF_PKT_PSPEC_NBI_of(port_spec),
                                                                           PIF_PKT_PSPEC_CHAN_of(port_spec));
#else

    if (PIF_PORT_SPEC_TYPE_of(port_spec) == PIF_PORT_SPEC_TYPE_PHYSICAL) {
         /* egress_spec[6;0] == port + channel */
        pif_pkt_info_global.__raw[PIF_PKT_INFO_EGRESS_PORT_wrd] = PIF_PKT_INFO_SET_PORT(PIF_PKT_SRC_NBI,
                                                                               PIF_PKT_PSPEC_NBI_of(port_spec),
                                                                               PIF_PKT_PSPEC_CHAN_of(port_spec));
        return 0;
    } else if (PIF_PORT_SPEC_TYPE_of(port_spec) == PIF_PORT_SPEC_TYPE_HOST) {

        pif_pkt_info_global.__raw[PIF_PKT_INFO_EGRESS_PORT_wrd] = PIF_PKT_INFO_SET_PORT(PIF_PKT_SRC_PCIE,
                                                                               PIF_PKT_PSPEC_NBI_of(port_spec),
                                                                               PIF_PORT_SPEC_PORT_of(port_spec));
        return 0;
    } else {

        /* logical port - for future use, drop packet for now */
        /* multicast port - we dont support recursive mcast groups (mcast in mcast)- drop packet */
        //pif_global_set_stdmd_egress_port(parrep, PIF_EGRESS_DROP);
        return 1;
    }
#endif /* ! PIF_NO_NFD */

    return 0;
}


/***************************************************************************
 * Helpers                                                                 *
 ***************************************************************************/

/* load control info into flow cache */
__forceinline void load_fc_control(__mem uint32_t *buf, int ig_len, int eg_len)
{
    struct fc_control control_buf;
    __xwrite struct fc_control control;

    control_buf.ingress_len = ig_len;
    control_buf.egress_len = eg_len;
    control = control_buf;

    mem_write32(&control, buf, sizeof(control));
}

#define PACKET_ERROR_SUCCESS   0
#define PACKET_ERROR_PKTMETA   1
#define PACKET_ERROR_PARREP    2
#define PACKET_ERROR_FLOWCACHE 3
#define PACKET_ERROR_INGRESS   4
#define PACKET_ERROR_EGRESS    5
#define PACKET_ERROR_DEPARSE   6
void handle_error_packet(int reason)
{

#if 0
    local_csr_write(local_csr_mailbox0, 0xffff0000 | reason);
#ifdef DEBUG
    /*
     * Break the simulation here
     */
    __asm ctx_arb[bpt]
#endif
#endif

    pkt_ops_tx_drop();
    PIF_COUNT(DROP);

    return;
}

/***************************************************************************
 * Main                                                                    *
 ***************************************************************************/

volatile __export __mem uint32_t flowkey_copy[256];

void main(void)
{
    /* action buffer in the case where no flow cache lookup is performed */
    __mem uint32_t action_buffer[ACTION_BUFFER_SIZE];
    __volatile __shared __gpr unsigned int ready_ctxs;
    __emem uint32_t *act_buf;

    if (ctx() == 0)
        ready_ctxs = __nctx_mode();

    if (ctx() == 0) {
#ifndef PIF_GLOBAL_FLOWCACHE_DISABLED
        fc_me_init();
#endif /* PIF_GLOBAL_FLOWCACHE_DISABLED */
        camp_hash_init();
#ifndef PIF_GLOBAL_NFD_DISABLED
        nfd_in_recv_init();
        nfd_out_send_init();
#endif /* PIF_GLOBAL_NFD_DISABLED */
#ifndef PIF_GLOBAL_GRO_DISABLED
        gro_cli_init();
#endif
    }

    /* All other contexts need to wait for sys init */
    system_init_wait_done();

    while (1) {
        unsigned int mb0;
        mb0 = local_csr_read(local_csr_mailbox0);
        if (mb0 != 0x0) {
            ready_ctxs--;
#if (PIF_APP_NUM_NBI_CTX > 0)
            if (ctx() == 0)
                break;
#endif
#if (PIF_APP_NUM_PCIE_CTX > 0)
            if (ctx() == PIF_APP_NUM_NBI_CTX)
                break;
#endif
            /* mailbox0 == 2 means use at most a single thread for packet
             * from NBI and/or PCIe/NFD. */
            if (mb0 == 2)
                __asm ctx_arb[kill];

            break;
        }
        __asm ctx_arb[voluntary]
    }

    while (ready_ctxs)
        __asm ctx_arb[voluntary];

    local_csr_write(local_csr_mailbox0, 0);

    pkt_dup_state.__raw = PIF_PKT_INSTANCE_TYPE_NORMAL;

    while (1) {
        __gpr unsigned int flowkey_len = 0;
        __gpr int exit_node;
#ifndef PIF_GLOBAL_FLOWCACHE_DISABLED
        __gpr fc_lookup_result_t result;
#endif /* PIF_GLOBAL_FLOWCACHE_DISABLED */
        __gpr int ingress_cached, egress_cached;
        __gpr struct replication_information rep_info;

        pif_flowcache_bypass = 0;

        /*
         * For a multicast packet and clone to egress, jump to egress processing.
         * For a resubmit/recirculate/clone to ingress packet, jump to parsing.
         */

        if (!PIF_DUP_EGRESS_ACTION(pkt_dup_state)) {
            if (pkt_dup_state.__raw == PIF_PKT_INSTANCE_TYPE_NORMAL) {

                __critical_path();
                PIF_DEBUG_SET_STATE(PIF_DEBUG_STATE_WAITING, 0x999);

                /*
                * Receive next packet.
                */
                if (ctx() < PIF_APP_NUM_NBI_CTX) {
                    if (pkt_ops_rx(FROM_WIRE) != 0) {
                        handle_error_packet(PACKET_ERROR_PKTMETA);
                        PIF_COUNT(ERROR_PKTMETA);
                        continue;
                    }
                    PIF_COUNT(RX);
                } else {
                    if (pkt_ops_rx(FROM_HOST) != 0) {
                        handle_error_packet(PACKET_ERROR_PKTMETA);
                        PIF_COUNT(ERROR_NFDMETA);
                        continue;
                    }
                    PIF_COUNT(RX_NFD);
                }

                //pif_global_metadata_init((__lmem uint32_t *)parrep);
                rep_info.rid = 0;

            } else {

                /*
                * If resubmit/recirculate/clone ingress of packet; do ingress parsing again.
                */
                pkt_dup_state.__raw = PIF_PKT_INSTANCE_TYPE_NORMAL;          // clear duplication flag
            }

            //exit_node = pif_parrep_extract((__lmem uint32_t *)parrep,
            //                               pif_pkt_info_global.pkt_buf,
            //                               pif_pkt_info_global.pkt_len,
            //                               flowkey,
            //                               (__gpr unsigned int *)&flowkey_len);

            /* Verify calculated fields if required. Returns 0 if verification
             * successful and -PIF_PARSE_ERROR_CHECKSUM otherwise. */
            if (exit_node >= 0) {
                __gpr int flcalc_verify_result;
                //flcalc_verify_result = pif_flcalc_verify((__lmem uint32_t *)parrep);
                if (flcalc_verify_result)
                    exit_node = flcalc_verify_result;
            }

            if (exit_node < 0) {
                /* handle parser error, run P4 parser exception code */
                if (-exit_node >= PIF_PARSE_ERROR_LAST) {
                    PIF_COUNT(ERROR_PARREP_CUSTOM);
                } else {
                    PIF_COUNT_RANGE(ERROR_PARREP, (-exit_node));
                }

                //exit_node = pif_parrep_exceptions((__lmem uint32_t *)parrep,
                //                                  exit_node);
                if (exit_node < 0) {
                    handle_error_packet(PACKET_ERROR_PARREP);
                    continue;
                }
                flowkey_len = 0; /* dont cache the lookup */
            }

            /* Pipeline with flow key */
#ifndef PIF_GLOBAL_FLOWCACHE_DISABLED
            if (flowkey_len > 0) {
                result = fc_do_lookup(
                            flowkey,
                            flowkey_len,
                            pif_pkt_info_global.pkt_len,
                            fc_get_version(),
                            30 /* TODO: timeout */ );
                act_buf = (__mem uint32_t *)result.payload_address;

                if (result.result_code == FC_LOOKUP_EXACT_MATCH) {
                    __xread struct fc_control control;

                    mem_read32(&control, act_buf, sizeof(control));
                    ingress_cached = control.ingress_len;
                    egress_cached = control.egress_len;
                    PIF_COUNT(FLOWCACHE_HIT);

                    if (ingress_cached == PIF_FC_CACHE_BYPASS) {
                        /* if ingress is not cacheable, use the local action buffer
                        * and release the lock */
                        act_buf = action_buffer;
                        fc_do_release(result.lock);
                    }
                } else if (result.result_code == FC_LOOKUP_ENTRY_ADDED) {
                    ingress_cached = PIF_FC_CACHE_INITIAL;
                    egress_cached = PIF_FC_CACHE_INITIAL;
                    PIF_COUNT(FLOWCACHE_MISS);
                } else {
                    /* unexpected flow cache response */
                    handle_error_packet(PACKET_ERROR_FLOWCACHE);
                    PIF_COUNT(ERROR_FLOWCACHE);
                    continue;
                }
            } else {
                /* flowkey_len == 0 indicates no lookup */
                act_buf = action_buffer;
                ingress_cached = PIF_FC_CACHE_BYPASS;
                egress_cached = PIF_FC_CACHE_BYPASS;
                PIF_COUNT(FLOWCACHE_BYPASS);
            }
#else
            act_buf = action_buffer;
            ingress_cached = PIF_FC_CACHE_BYPASS;
            egress_cached = PIF_FC_CACHE_BYPASS;
#endif /* PIF_GLOBAL_FLOWCACHE_DISABLED */

            /*
            * Ingress processing
            */
#ifdef PIF_GLOBAL_FLOWCACHE_DISABLED
            {
                int ret;
                ret = pif_ctlflow_ingress_flow((int *)&exit_node,
                                               (__lmem uint32_t *)parrep,
                                               act_buf + PIF_FC_PAYLOAD_OFF);
                if (ret < 0) { /* drop */
                    pkt_ops_tx_drop();
                    PIF_COUNT(DROP);
                    continue;
                }

                ingress_cached = ret;
            }
#else
            if (ingress_cached < 0) { /* uncached */
                int ret;

                //ret = pif_ctlflow_ingress_flow((int *)&exit_node,
                //                               (__lmem uint32_t *)parrep,
                //                               act_buf + PIF_FC_PAYLOAD_OFF);
                if (ret < 0) { /* drop */
                    /* store the action buffer length so actions up to and
                    * including drop will be done each time
                    */

                    /* release the lock if needed */
                    if (ingress_cached == PIF_FC_CACHE_INITIAL) {
                        if (pif_flowcache_bypass)
                            ret = PIF_FC_CACHE_BYPASS;
                        else
                            ret = -ret;
                        load_fc_control(act_buf, ret, 0);
                        fc_do_release(result.lock);
                    }

                    pkt_ops_tx_drop();
                    PIF_COUNT(DROP);
                    continue;
                }

                if (pif_flowcache_bypass) {
                    ingress_cached = PIF_FC_CACHE_BYPASS;
                } else
                    ingress_cached = ret;
            } else { /* cached */
                int ret;

                __critical_path();
                ret = pif_action_execute((__lmem uint32_t *)parrep,
                                         act_buf + PIF_FC_PAYLOAD_OFF,
                                         ingress_cached);
                if (ret < 0) { /* drop */
                    fc_do_release(result.lock);
                    pkt_ops_tx_drop();
                    PIF_COUNT(DROP);
                    continue;
                }

                if (egress_cached == PIF_FC_CACHE_BYPASS) {
                    /* if egress is not cacheable, use the local action buffer
                    * for the second part and release the lock
                    */
                    act_buf = action_buffer;
                    fc_do_release(result.lock);
                }
            }
#endif /* !PIF_GLOBAL_FLOWCACHE_DISABLED */


            /*
            * Check for packet cloning (after ingress processing) unless packet
            * has to be dropped.
            */
            {
                __gpr uint32_t port_spec ; //= pif_global_get_stdmd_egress_spec((__lmem uint32_t *)parrep);

#if (MCAST_GROUP_MAX > 0)
                if (PIF_PORT_SPEC_TYPE_of(port_spec) == PIF_PORT_SPEC_TYPE_MULTICAST) {

                    __gpr int32_t port = -1;

                    /*
                    * First time lookup for multicast, save egress length as 0
                    */
                    if (egress_cached == PIF_FC_CACHE_INITIAL)
                        load_fc_control(act_buf, ingress_cached, 0);

                    /*
                    * We can release the exclusive lock even if we are still sending out mcast packets
                    * as we won't be accessing the payload for egress. We have all the information we need.
                    */
#ifndef PIF_GLOBAL_FLOWCACHE_DISABLED
                    if (flowkey_len != 0)
                        fc_do_release(result.lock);
#endif /* PIF_GLOBAL_FLOWCACHE_DISABLED */

                    /*
                    * Do multicast processing
                    */
                    pkt_dup_state.__raw = PIF_PKT_INSTANCE_TYPE_REPLICATION;
                    rep_info.__raw = PIF_PORT_SPEC_PORT_of(port_spec) << 16;

                    if (rep_info.group < MCAST_GROUP_MAX)
                    {
                        port = process_multicast(rep_info);
                        PIF_COUNT(MULTICAST);
                    }

                    /*
                    * Completed multicast processing
                    */
                    if (port == -1) {
                        pkt_dup_state.__raw = PIF_PKT_INSTANCE_TYPE_NORMAL;
                        continue;
                    }

                    rep_info.rid++;

                    /*
                    * Set to always do ingress match/action.
                    * Egress actions are not saved to payload as these can be different between elements in the multicast group.
                    */
                    egress_cached = PIF_FC_CACHE_BYPASS;
                    act_buf = action_buffer;

                }
                else
#endif
                {
                    /*
                    * Set egress port for non-mcast packets.
                    * Mcast will set egress port for each element in the mcast group.
                    */
                    __critical_path();

                    //pif_global_set_stdmd_egress_port((__lmem uint32_t *)parrep, port_spec);

                    if (pkt_dup_state.action && port_spec != PIF_EGRESS_DROP) {

                        /*
                        * First time lookup for clone, save egress length as initial
                        */
                        if (egress_cached == PIF_FC_CACHE_INITIAL)
                            load_fc_control(act_buf, ingress_cached, egress_cached);

                        /*
                         * Process any ingress duplication actions
                         */
#ifdef PIF_RECURSE_PACKET
                        if (pkt_dup_state.type == PIF_PKT_INSTANCE_TYPE_RESUBMIT) {

                            /*
                            * Because the packet is resubmitted, another fc lookup will be done,
                            * release the lock on the current entry.
                            */
#ifndef PIF_GLOBAL_FLOWCACHE_DISABLED
                            if (flowkey_len != 0)
                                fc_do_release(result.lock);
#endif /* PIF_GLOBAL_FLOWCACHE_DISABLED */

                            pif_resubmit_post_ingress((__lmem uint32_t *)parrep);

                            continue; // cycle to top of while loop - parsing

                        }
#endif
#ifdef PIF_CLONE_PACKET
#ifdef PIF_RECURSE_PACKET
                        else
#endif
                            if (pkt_dup_state.type == PIF_PKT_INSTANCE_TYPE_I2I_CLONE) {

                            /*
                            * Do not release fc lock because the lock is released when the normal packet does egress processing
                            */

                            pif_clone_packet((__lmem uint32_t *)parrep, rep_info.rid);

                            /*
                            * original packet proceeds as normal
                            */

                        } else if (pkt_dup_state.type == PIF_PKT_INSTANCE_TYPE_I2E_CLONE) {

                            /*
                            * Do not release fc lock because the lock is released when the normal packet does egress processing
                            */
                            pif_clone_i2e((__lmem uint32_t *)parrep, rep_info.rid);

                            /*
                            * original packet proceeds as normal
                            */
                        }
#endif
                    }
                }
            }
        } else {
            /*
             * Egress processing
             */
            __gpr uint32_t type = pkt_dup_state.type;

#if (MCAST_GROUP_MAX > 0)
            if (type == PIF_PKT_INSTANCE_TYPE_REPLICATION) {
                /*
                * If to do multicast processing
                */
                __gpr int32_t port = process_multicast(rep_info);

                /*
                * Drop the original packet if we have processed all multicast elements
                */
                if (port == -1) {
                    pkt_dup_state.__raw = PIF_PKT_INSTANCE_TYPE_NORMAL;          // finished multicast processing

                    continue;
                }

                rep_info.rid++;

                /*
                * Set to always do ingress match/action.
                * Egress actions are not saved to payload as these can be different between elements in the multicast group.
                */
                egress_cached = PIF_FC_CACHE_BYPASS;
                act_buf = action_buffer;

            }
#ifdef PIF_CLONE_PACKET
            else
#endif
#endif

#ifdef PIF_CLONE_PACKET
                if (type == PIF_PKT_INSTANCE_TYPE_I2E_CLONE
                    || type == PIF_PKT_INSTANCE_TYPE_E2E_CLONE) {

                /*
                * If clone ingress to egress, we do not have flow key, no fc lookup and no egress cached.
                */
                pkt_dup_state.__raw = PIF_PKT_INSTANCE_TYPE_NORMAL;          // clear clone flag
                egress_cached = PIF_FC_CACHE_BYPASS;
                act_buf = action_buffer;
            }
#endif
        }



        /*
         * Egress processing
         */
#ifdef PIF_GLOBAL_FLOWCACHE_DISABLED
        {
            int ret;
            ret = pif_ctlflow_egress_flow((int *)&exit_node,
                                           (__lmem uint32_t *)parrep,
                                           act_buf + PIF_FC_PAYLOAD_OFF + ingress_cached);

            if (ret < 0) { /* drop */
                pkt_ops_tx_drop();
                PIF_COUNT(DROP);
                continue;
            }
        }
#else
        if (egress_cached < 0) {
            int ret = 0;

            //ret = pif_ctlflow_egress_flow((int *)&exit_node,
            //                               (__lmem uint32_t *)parrep,
            //                               act_buf + PIF_FC_PAYLOAD_OFF + ingress_cached);

            if (ret < 0) { /* drop */
                /* store the action buffer length so actions up to and
                 * including drop will be done each time
                 */
                if (pif_flowcache_bypass)
                    ret = PIF_FC_CACHE_BYPASS;
                else
                    ret = -ret;
                load_fc_control(act_buf, ingress_cached, ret);

                if (egress_cached == PIF_FC_CACHE_INITIAL)
                    fc_do_release(result.lock);     // only release if not multicast processing

                pkt_ops_tx_drop();
                PIF_COUNT(DROP);
                continue;
            }

            if (egress_cached == PIF_FC_CACHE_INITIAL) {
                /*
                 * save the egress length and release lock if not mcast
                 */

                if (pif_flowcache_bypass)
                    ret = PIF_FC_CACHE_BYPASS;

                load_fc_control(act_buf, ingress_cached, ret);
                fc_do_release(result.lock);
            }

        } else {
            int ret;

            __critical_path();

            ret = pif_action_execute((__lmem uint32_t *)parrep,
                                     act_buf + PIF_FC_PAYLOAD_OFF + ingress_cached,
                                     egress_cached);

            /* always release the lock */
            fc_do_release(result.lock);

            if (ret < 0) {
                pkt_ops_tx_drop();
                PIF_COUNT(DROP);
                continue;
            }
        }
#endif /* !PIF_GLOBAL_FLOWCACHE_DISABLED */

        /* Do P4 forwarding */
        if (forward((__lmem uint32_t *)parrep)) {
            pkt_ops_tx_drop();
            PIF_COUNT(DROP_META);
            continue;
        }

        /* Update calculated fields if required. */
        //pif_flcalc_update((__lmem uint32_t *)parrep);


        PIF_DEBUG_SET_STATE(PIF_DEBUG_STATE_DEPARSING, 0);
        {
            __gpr int pkt_byteoffset = 0 ; //pif_deparse((__lmem uint32_t *)parrep, &pif_pkt_info_global);

            if (pkt_byteoffset < 0) {
                handle_error_packet(PACKET_ERROR_DEPARSE);
                PIF_COUNT(ERROR_DEPARSE);
                continue;
            }

            /* Check if a pkt duplication action was set during egress processing */
            if (pkt_dup_state.action)
            {
#ifdef PIF_RECURSE_PACKET
                if (pkt_dup_state.type == PIF_PKT_INSTANCE_TYPE_INGRESS_RECIRC) {
                    /*
                    * Recirculate
                    */
                    pif_resubmit_post_ingress((__lmem uint32_t *)parrep);
                    continue;  //cycle to parsing

                }
#endif
#ifdef PIF_CLONE_PACKET
#ifdef PIF_RECURSE_PACKET
                else
#endif
                    if (pkt_dup_state.type == PIF_PKT_INSTANCE_TYPE_E2I_CLONE) {
                    /*
                    * Clone egress to ingress
                    */
                    pif_clone_packet((__lmem uint32_t *)parrep, rep_info.rid);

                    /*
                    * original packet proceeds as normal
                    */
                } else if (pkt_dup_state.type == PIF_PKT_INSTANCE_TYPE_E2E_CLONE) {
                    /*
                    * Clone egress to ingress
                    */
                    pif_clone_packet((__lmem uint32_t *)parrep, rep_info.rid);

                    /*
                    * original packet proceeds as normal
                    */
                }
#endif
            }


            PIF_DEBUG_SET_STATE(PIF_DEBUG_STATE_SENDING, 0);
            if (pif_pkt_info_global.eg_port.type == PIF_PKT_SRC_NBI) {
                if (pkt_ops_tx(TO_WIRE, pkt_byteoffset) == 0) {
                    PIF_COUNT(TX);
                } else {
                    PIF_COUNT(DROP_PORT_PAUSED);
                }
            } else {
                if (pkt_ops_tx(TO_HOST, pkt_byteoffset) == 0) {
                    PIF_COUNT(TX_NFD);
                } else {
                    PIF_COUNT(DROP_NFD_NO_CREDITS);
                }
            }
        }

        /*
         * Go to top of while-loop for normal pkts (performance sake)
         */
        if (pkt_dup_state.__raw == PIF_PKT_INSTANCE_TYPE_NORMAL
            || pkt_dup_state.__raw == PIF_PKT_INSTANCE_TYPE_REPLICATION)
            continue;

#ifdef PIF_CLONE_PACKET
        /*
        * Setup cloned packet as next packet to process
        * if packet_type is set to:
        * PIF_PKT_INSTANCE_TYPE_I2I_CLONE
        * PIF_PKT_INSTANCE_TYPE_I2E_CLONE
        * PIF_PKT_INSTANCE_TYPE_E2I_CLONE
        * PIF_PKT_INSTANCE_TYPE_E2E_CLONE
        */
        rep_info.rid = pif_clone_packet_submit((__lmem uint32_t *)parrep, rep_info.rid);
#endif
    } // while (1)
}
