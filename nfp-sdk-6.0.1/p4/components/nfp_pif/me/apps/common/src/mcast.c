/* Copyright (C) 2016,  Netronome Systems, Inc.  All rights reserved. */

#include <nfp.h>
#include <nfp_chipres.h>
#include <nfp/me.h>
#include <nfp/mem_bulk.h>
#include <nfp/cls.h>

#include <nfp6000/nfp_me.h>

#include <std/reg_utils.h>

#include <blm.h>

#include <pif_counters.h>

#include "nfd_user_cfg.h"
#include "pkt_clone.h"
#include "mcast.h"

#if (MCAST_GROUP_MAX > 0)
/***************************************************************************
 * Global multicast table indexed on mcast group id                        *
 ***************************************************************************/
__volatile __export __mem int16_t pif_mcast_table[MCAST_GROUP_MAX][MCAST_PORTS_MAX];

/***************************************************************************
 * Multicast table for a group                                             *
 ***************************************************************************/
__cls int16_t mcast_group[MCAST_PORTS_MAX];

/***************************************************************************
 * Multicast group                                                         *
 ***************************************************************************/
#endif

void mcast_table_init(void)
{
#if (MCAST_GROUP_MAX > 0)
    __xwrite int32_t     wr_buffer = -1;
    __gpr uint32_t      i;

    for ( i = 0 ; i < MCAST_GROUP_MAX; i++)
    {
        mem_write8(&wr_buffer, (__mem __addr40 void *)pif_mcast_table[i], 2);
    }
#endif
}

extern __forceinline int32_t mcast_grp_load(uint32_t grp_id)
{
#if (MCAST_GROUP_MAX > 0)
    __xread int16_t     rd_buffer[16];
    __xwrite int16_t    wr_buffer[16];
    __gpr int           i;
    __gpr int32_t       first_val;

    for (i = 0 ; i < MCAST_PORTS_MAX; i += sizeof(rd_buffer)/sizeof(uint16_t))
    {
        mem_read32(rd_buffer,
                (__mem __addr40 void *)(pif_mcast_table[grp_id] + i),
                sizeof(rd_buffer));

        if (i == 0)
            first_val = ((rd_buffer[0] << 16) | (rd_buffer[1] & 0xffff));

        reg_cp(wr_buffer, rd_buffer, sizeof(rd_buffer));
        cls_write(wr_buffer, mcast_group + i, sizeof(wr_buffer));
    }

    return first_val;
#else
    return -1;
#endif
}

extern __forceinline int32_t mcast_grp_retrieve_element(uint32_t grp_id, uint32_t index)
{
#if (MCAST_GROUP_MAX > 0)
    if (index >= MCAST_PORTS_MAX)
    {
        PIF_COUNT(DROP_MCAST_PORT_OUT_BOUNDS);
        return -1;
    }

    if (index + 1 >= MCAST_PORTS_MAX)
        return ((mcast_group[index] << 16) | 0xffff);
    else
        return ((mcast_group[index] << 16) | (mcast_group[index+1] & 0xffff));
#else
    return -1;
#endif
}

#if (MCAST_GROUP_MAX > 0)
int16_t process_multicast(struct replication_information mcast)
{
    __gpr int32_t                               egress_ports;
    __gpr int32_t                               egress_port;
    __gpr struct pkt_clone_destination          destination;

    /*
     * Copy parsed metadata and packet info to CLS
     * Do this only once per multicast group.
     */
    if (mcast.rid > 0)
    {
        /*
         * retrieve original packet info before we possibly drop the packet
         */
        egress_ports = mcast_grp_retrieve_element(mcast.group, mcast.rid);
        retrieve_pkt_info((__cls void*)&pif_pkt_info_copy);

        if (egress_ports == -1)
        {
            return -1;
        }

        /*
        * For each port in the multicast group, retrieve the original parsed header
        */
        retrieve_parrep((__lmem uint32_t *)parrep, (__cls uint32_t *)parrep_copy, PIF_PARREP_LEN_LW);

    }
    else
    {
        egress_ports = mcast_grp_load(mcast.group);

        if (egress_ports == -1)
        {
            return -1;
        }

        /*
        * Setup multicast group and return the first egress_port
        * Save the packet info and parsed header of the original packet.
        */
        save_parrep((__lmem uint32_t *)parrep, (__cls uint32_t *)parrep_copy, PIF_PARREP_LEN_LW);
        save_pkt_info((__cls void*)&pif_pkt_info_copy);
    }


    /*
    * Setup multicast element information
    */
    egress_port = (egress_ports >> 16); // extract current port
    //pif_global_set_stdmd_egress_port((__lmem uint32_t *)parrep, egress_port);
    //pif_global_set_stdmd_egress_instance((__lmem uint32_t *)parrep, mcast.rid);
    //pif_global_set_stdmd_instance_type((__lmem uint32_t *)parrep, PIF_PKT_INSTANCE_TYPE_NORMAL);

    if ((egress_ports & 0xffff) != 0xffff)
    {
        /*
        * Clone the CTM packet and linked MU buffer if we have valid port, but only
        * if the next port is non-zero, otherwise we can tx the current buffer
        */
        destination = pkt_clone(
            pif_pkt_info_global.pkt_num,
            pif_pkt_info_global.ctm_size,
            (__addr40 __mem uint32_t *) blm_buf_handle2ptr(pif_pkt_info_global.muptr),
            pif_pkt_info_global.bls, // BLM_BLQ_0 ??
            PIF_PKT_SOP(pif_pkt_info_global.pkt_buf, pif_pkt_info_global.pkt_num),
            pif_pkt_info_global.pkt_len,
            pif_pkt_info_global.split
        );

        {
            __xwrite uint32_t buf_xw[2];
            __addr40 void *ctm_ptr;

            /*
            * Setup packet info to reflect new pkt and buffer
            */
            pif_pkt_info_global.muptr = blm_buf_ptr2handle(destination.mu_ptr);
            pif_pkt_info_global.pkt_num = destination.ctm_pkt_num;

            if (pif_pkt_info_global.ig_port.type == PIF_PKT_SRC_NBI) {
                pif_pkt_info_global.pkt_buf = pkt_ctm_ptr40(
                    __ISLAND,
                    pif_pkt_info_global.pkt_num,
                    PKT_NBI_OFFSET + PKT_MAC_PREPEND_BYTES  // this is ok as the packet has not been deparsed yet, hence SOP not moved yet
                );
            } else {
                pif_pkt_info_global.pkt_buf = pkt_ctm_ptr40(
                    __ISLAND,
                    pif_pkt_info_global.pkt_num,
                    NFD_IN_DATA_OFFSET
                );
            }

            pif_pkt_info_global.__raw[PIF_PKT_INFO_CTM_INFO_wrd] =  PIF_PKT_INFO_SET_CTM_INFO(pif_pkt_info_global.ctm_size,
                                                                                 __ISLAND,
                                                                                 0,
                                                                                 1,
                                                                                 0);
            /*
            * Write metadata to start of CTM buffer.
            */
            ctm_ptr = pkt_ctm_ptr40(__ISLAND, pif_pkt_info_global.pkt_num, 0);
            buf_xw[0] = pif_pkt_info_global.__raw[0];
            buf_xw[1] = pif_pkt_info_global.__raw[1];
            mem_write32(buf_xw, ctm_ptr, sizeof(buf_xw));
        }
    }

    return egress_port;

} // process_multicast
#endif

