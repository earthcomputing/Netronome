/* Copyright (C) 2015,  Netronome Systems, Inc.  All rights reserved. */

#include <stdint.h>
#include <nfp.h>
#include <app_counters.h>

#include <nfp/me.h>
#include <nfp/mem_bulk.h>
#include <nfp/mem_ring.h>
#include <nfp/mem_atomic.h>
#include <nfp6000/nfp_me.h>

#ifndef PIF_GLOBAL_NFD_DISABLED
#include <vnic/shared/nfd_cfg.h>
#endif /* PIF_GLOBAL_NFD_DISABLED */

#include <vnic/pci_in.h>
#include <vnic/pci_out.h>

#ifdef PIF_USE_GRO
#include <gro_cli.h>
#endif

#include <system_init_c.h>
#include <pkt_ops.h>

/***************************************************************************
 * Globals                                                                 *
 ***************************************************************************/

PIF_PKT_INFO_TYPE struct pif_pkt_info pif_pkt_info_global;

/* make sure only half the CTM is used for packet data */
/* TODO move the code below */
__asm .init_csr xpb:CTMXpbMap.MuPacketReg.MUPEMemConfig 1
__asm .alloc_mem PKT_RSVD ctm+0x0 island 0x20000 reserved


#ifndef PIF_GLOBAL_NFD_DISABLED
NFD_CFG_BASE_DECLARE(PCIE_ISL);
#endif /* PIF_GLOBAL_NFD_DISABLED */

/* XXX: parrep is a hack so the pif runtime goodies can find the app MEs */
__volatile __lmem uint32_t parrep[1] = {0};

/* entry point for managedc function */
extern int mc_main(void);

/* optional initialization calls for managedc */
#ifdef MC_INIT
extern int mc_init(void);
extern int mc_init_master(void);
#endif

/***************************************************************************
 * Helpers                                                                 *
 ***************************************************************************/

#define PACKET_ERROR_SUCCESS   0
#define PACKET_ERROR_PKTMETA   1
#define PACKET_ERROR_PARREP    2
#define PACKET_ERROR_FLOWCACHE 3
#define PACKET_ERROR_INGRESS   4
#define PACKET_ERROR_EGRESS    5
#define PACKET_ERROR_DEPARSE   6
void handle_error_packet(int reason)
{
    __nnr struct pif_pkt_info *pkt_state = &pif_pkt_info_global;

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
    return;
}

/***************************************************************************
 * Main                                                                    *
 ***************************************************************************/

void main(void)
{
    __volatile __shared __gpr unsigned int ready_ctxs;

    if (ctx() == 0) {
        ready_ctxs = __nctx_mode();

#ifndef PIF_GLOBAL_NFD_DISABLED
        nfd_in_recv_init();
        nfd_out_send_init();
#endif /* PIF_GLOBAL_NFD_DISABLED */
#ifdef PIF_USE_GRO
        gro_cli_init();
#endif
#ifdef MC_INIT
        mc_init();
        if (__ME() == MC_MASTER_ME)
            mc_init_master();
#endif
    }
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

    parrep[0] = local_csr_read(local_csr_mailbox0);
    local_csr_write(local_csr_mailbox0, 0);

    while (1) {
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
        if (mc_main() != 0) {
            pkt_ops_tx_drop();
            PIF_COUNT(DROP);
            continue;
        }

        {
            __gpr uint32_t byte_offset;

            /* Set packet offset depending on the ingress port type */
            if(pif_pkt_info_global.ig_port.type == PIF_PKT_SRC_NBI)
                byte_offset = PKT_NBI_OFFSET + PKT_MAC_PREPEND_BYTES;
            else
                byte_offset = NFD_IN_DATA_OFFSET;

            if (pif_pkt_info_global.eg_port.type == PIF_PKT_SRC_NBI) {
                pkt_ops_tx(TO_WIRE, byte_offset);
                PIF_COUNT(TX);
            } else {
                if (pkt_ops_tx(TO_HOST, byte_offset) == 0) {
                    PIF_COUNT(TX_NFD);
                } else {
                    PIF_COUNT(DROP_NFD_NO_CREDITS);
                }
            }
        }
    }
}
