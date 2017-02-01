/* Copyright (C) 2016,  Netronome Systems, Inc.  All rights reserved. */

#include <nfp.h>
#include <stdint.h>
#include <nfp/me.h>
#include <nfp/cls.h>
#include "mac_flush.h"

/* NBI port configuration.
 * NBI0 ports[0:23] correspond to  bits[0:23] of word pif_cfg...0
 * NBI1 ports[24:47] correspond to bits[0:23] of word pif_cfg...1 */
__asm {
    .alloc_mem pif_cfg_active_ports_nbi0 cls island 0x4
    .alloc_mem pif_cfg_active_ports_nbi1 cls island 0x4
    .init pif_cfg_active_ports_nbi0 0
    .init pif_cfg_active_ports_nbi1 0
}

#define MAC_NBI0                    0
#define MAC_NBI1                    1

/* XPB addressing constants. */
#define GLOBAL_LOCAL_shf    30
#define ISLAND_ID_shf       24
#define SLAVE_IDX_shf       22
#define TGT_DEVICE_shf      16
/* The CSR's are 0x400 or (2**10) apart. */
#define CSR_shf             10

/* MAC CSR constants. */
#define ETH_TX_FLUSH_shf            22
#define ETH_RX_PAUSE_STATUS_msk     0xff
/* xpb:Nbi0IsldXpbMap.NbiTopXpbMap.MacGlbAdrMap.MacEthernet0.MacEthSeg0.EthRxPauseStatus */
#define ETH_RX_PAUSE_STATUS_BASE    0x0074
/* xpb:Nbi0IsldXpbMap.NbiTopXpbMap.MacGlbAdrMap.MacEthernet0.MacEthSeg0.EthCmdConfig */
#define ETH_CMD_CONFIG_BASE         0x0008
/* xpb:Nbi0IsldXpbMap.NbiTopXpbMap.MacGlbAdrMap.MacEthernet0.MacEthChPcsSeg0 */
#define MAC_ETH_CH_PCS_shf          14 /* 0x4000 */
/* xpb:Nbi0IsldXpbMap.NbiTopXpbMap.MacGlbAdrMap.MacEthernet0.MacEthChPcsSeg0.EthChPcsStatus1 */
#define MAC_ETH_CH_PCS_STATUS_1     0x0004
/* xpb:Nbi0IsldXpbMap.NbiTopXpbMap.MacGlbAdrMap.MacEthernet0.MacEthChPcsSeg0.EthChPcsBaseRStatus1 */
#define MAC_ETH_CH_PCS_BASER_STA_1  0x0080

/* Within EthChPcsStatus1 */
#define NFP_MAC_ETH_MACETHCHPCSSEG_STATUS1_RCVLINKSTATUS_msk        0x4
/* Within EthChPcsBaseRStatus1 */
#define NFP_MAC_ETH_MACETHCHPCSSEG_BASERSTATUS1_RCVLINKSTATUS_msk   0x1000
/* Within EthChPcsBaseRStatus1 */
#define NFP_MAC_ETH_MACETHCHPCSSEG_BASERSTATUS1_BLOCKLOCKED_msk     0x1
/* Within EthCmdConfig */
#define NFP_MAC_ETH_MACETHSEG_ETHCMDCONFIG_ETHTXENA_msk             0x1
/* Within EthCmdConfig */
#define NFP_MAC_ETH_MACETHSEG_ETHCMDCONFIG_ETHRXENA_msk             0x2

/*
 * Helper macro to calculate xpb addresses for CSR's
 * See:
 * Flow Processor Core Programmer's Reference Manual, section 2.2.38.1
 * Netronome Network Flow Processor - NFP-6xxx-xC Databook Table 3.71. XPB Island and Device IDs
 *
 * @param out_addr          The resultant address.
 * @param in_global_local   1= Global, 0=Local
 * @param in_island         Island number
 * @param in_slave_idx      XPB slave index
 * @param in_tgt_dev        XPB Device ID within Island
 * @param in_csr_offset     Optional offset
 */
#define _xpb_csr_addr(out_addr, in_global_local, in_island, in_slave_idx, in_tgt_dev) \
    do { \
        __asm { alu[out_addr, --, B, in_global_local, <<GLOBAL_LOCAL_shf] } \
        __asm { alu[out_addr, out_addr, OR, in_island, <<ISLAND_ID_shf] } \
        __asm { alu[out_addr, out_addr, OR, in_slave_idx, <<SLAVE_IDX_shf] } \
        __asm { alu[out_addr, out_addr, OR, in_tgt_dev, <<TGT_DEVICE_shf] } \
    } while (0)

/*1 LW for each possible port (48 max), counting mac_eth_pause_poll() iterations
 * since paused. */
__lmem uint32_t pause_counts[48] = {0};
/* PAUSE_COUNT_THRES: Threshold of number of sequential paused 
 * mac_eth_pause_poll() iterations after which flushing is enabled. */
#define PAUSE_COUNT_THRES 100

/*
 * Indicate that port is paused so workers drop any packets set to egress via it.
 * Bitmap is written to CLS in all worker ME islands.
 * See "mes" entry for "pif_app_nfd" in me/apps/...app.json for worker islands.
 */
__intrinsic void mac_set_port_paused(__gpr uint32_t port)
{
    __gpr uint32_t addr_hi_base;
    __gpr uint32_t addr_hi;
    __gpr uint32_t addr_lo;
    __cls __addr40 void *port_bitmap_addr = (__cls __addr40 void*) __link_sym("MAC_PORT_PAUSED_BITMAP");
    __xwrite uint64_t xfr_port_bit = (uint64_t) 1 << port;
    __gpr uint32_t count = sizeof(xfr_port_bit)>>2;
    SIGNAL sig0;
    SIGNAL sig1;
    SIGNAL sig2;
    SIGNAL sig3;
    SIGNAL sig4;
#if (PLATFORM_PORT_SELECTION == SF_PLATFORM) || (PLATFORM_PORT_SELECTION == CDP_PLATFORM)
    SIGNAL sig5;
    SIGNAL sig6;
#endif

    /* Table 3.15 of DB - bits [39:34] are island. */
    addr_hi_base = ((uint64_t)port_bitmap_addr>>8) & 0x03000000;
    addr_lo = (uint64_t)port_bitmap_addr & 0xffffffff;

    addr_hi = addr_hi_base | (32 << (40 - 8 - 6));
    __asm cls[set, xfr_port_bit, addr_hi, <<8, addr_lo, __ct_const_val(count)], sig_done[sig0]
    addr_hi = addr_hi_base | (33 << (40 - 8 - 6));
    __asm cls[set, xfr_port_bit, addr_hi, <<8, addr_lo, __ct_const_val(count)], sig_done[sig1]
    addr_hi = addr_hi_base | (34 << (40 - 8 - 6));
    __asm cls[set, xfr_port_bit, addr_hi, <<8, addr_lo, __ct_const_val(count)], sig_done[sig2]
    addr_hi = addr_hi_base | (35 << (40 - 8 - 6));
    __asm cls[set, xfr_port_bit, addr_hi, <<8, addr_lo, __ct_const_val(count)], sig_done[sig3]
    addr_hi = addr_hi_base | (36 << (40 - 8 - 6));
    __asm cls[set, xfr_port_bit, addr_hi, <<8, addr_lo, __ct_const_val(count)], sig_done[sig4]
#if (PLATFORM_PORT_SELECTION == SF_PLATFORM) || (PLATFORM_PORT_SELECTION == CDP_PLATFORM)
    addr_hi = addr_hi_base | (37 << (40 - 8 - 6));
    __asm cls[set, xfr_port_bit, addr_hi, <<8, addr_lo, __ct_const_val(count)], sig_done[sig5]
    addr_hi = addr_hi_base | (38 << (40 - 8 - 6));
    __asm cls[set, xfr_port_bit, addr_hi, <<8, addr_lo, __ct_const_val(count)], sig_done[sig6]
#endif

#if (PLATFORM_PORT_SELECTION == SF_PLATFORM) || (PLATFORM_PORT_SELECTION == CDP_PLATFORM)
    __asm ctx_arb[sig0, sig1, sig2, sig3, sig4, sig5, sig6]
#else
    __asm ctx_arb[sig0, sig1, sig2, sig3, sig4]
#endif
}

/*
 * Indicate that port is unpaused so workers allow egress via it.
 * Bitmap is written to CLS in all worker ME islands.
 * See "mes" entry for "pif_app_nfd" in me/apps/...app.json
 */
__intrinsic void mac_clr_port_paused(__gpr uint32_t port)
{
    __gpr uint32_t addr_hi_base;
    __gpr uint32_t addr_hi;
    __gpr uint32_t addr_lo;
    __cls __addr40 void *port_bitmap_addr = (__cls __addr40 void*) __link_sym("MAC_PORT_PAUSED_BITMAP");
    __xwrite uint64_t xfr_port_bit = (uint64_t) 1 << port;
    __gpr uint32_t count = sizeof(xfr_port_bit)>>2;
    SIGNAL sig0;
    SIGNAL sig1;
    SIGNAL sig2;
    SIGNAL sig3;
    SIGNAL sig4;
#if (PLATFORM_PORT_SELECTION == SF_PLATFORM) || (PLATFORM_PORT_SELECTION == CDP_PLATFORM)
    SIGNAL sig5;
    SIGNAL sig6;
#endif

    /* Table 3.15 of DB - bits [39:34] are island. */
    addr_hi_base = ((uint64_t)port_bitmap_addr>>8) & 0x03000000;
    addr_lo = (uint64_t)port_bitmap_addr & 0xffffffff;

    addr_hi = addr_hi_base | (32 << (40 - 8 - 6));
    __asm cls[clr, xfr_port_bit, addr_hi, <<8, addr_lo, __ct_const_val(count)], sig_done[sig0]
    addr_hi = addr_hi_base | (33 << (40 - 8 - 6));
    __asm cls[clr, xfr_port_bit, addr_hi, <<8, addr_lo, __ct_const_val(count)], sig_done[sig1]
    addr_hi = addr_hi_base | (34 << (40 - 8 - 6));
    __asm cls[clr, xfr_port_bit, addr_hi, <<8, addr_lo, __ct_const_val(count)], sig_done[sig2]
    addr_hi = addr_hi_base | (35 << (40 - 8 - 6));
    __asm cls[clr, xfr_port_bit, addr_hi, <<8, addr_lo, __ct_const_val(count)], sig_done[sig3]
    addr_hi = addr_hi_base | (36 << (40 - 8 - 6));
    __asm cls[clr, xfr_port_bit, addr_hi, <<8, addr_lo, __ct_const_val(count)], sig_done[sig4]
#if (PLATFORM_PORT_SELECTION == SF_PLATFORM) || (PLATFORM_PORT_SELECTION == CDP_PLATFORM)
    addr_hi = addr_hi_base | (37 << (40 - 8 - 6));
    __asm cls[clr, xfr_port_bit, addr_hi, <<8, addr_lo, __ct_const_val(count)], sig_done[sig5]
    addr_hi = addr_hi_base | (38 << (40 - 8 - 6));
    __asm cls[clr, xfr_port_bit, addr_hi, <<8, addr_lo, __ct_const_val(count)], sig_done[sig6]
#endif

#if (PLATFORM_PORT_SELECTION == SF_PLATFORM) || (PLATFORM_PORT_SELECTION == CDP_PLATFORM)
    __asm ctx_arb[sig0, sig1, sig2, sig3, sig4, sig5, sig6]
#else
    __asm ctx_arb[sig0, sig1, sig2, sig3, sig4]
#endif
}

/*
 * @brief Gate port if paused for less than PAUSE_COUNT_THRES iterations; flush
 *        MAC if paused for longer or link status is down.
 *
 * If the link is paused(detected by
 * NbiXIsldXpbMap.NbiTopXpbMap.MacGlbAdrMap.MacEthernetY.MacEthSegZ.EthRxPauseStatus
 * being nonzero) or the link is down (assuming the port has been initialised
 * (i.e. assume EthActCtlSeg is set and that MacMuxCtrl is 0), and given that 1G
 * links are not used, only need to check
 * NbiXIsldXpbMap.NbiTopXpbMap.MacGlbAdrMap.MacEthernetY.MacEthChPcsSegZ.EthChPcsStatus1 
 * and
 * NbiXIsldXpbMap.NbiTopXpbMap.MacGlbAdrMap.MacEthernetY.MacEthChPcsSegZ.EthChPcsBaseRStatus1),
 * enable
 * xpb:NbiXIsldXpbMap.NbiTopXpbMap.MacGlbAdrMap.MacEthernetY.MacEthSegZ.EthCmdConfig.EthTxFlush
 * for a very short period of time.
 *
 * @param in_nbi    NBI number.
 * @param in_core   MAC core within NBI number.
 * @param in_seg    Segment within the MAC core.
 */
void mac_flush_port_if_paused(__gpr uint8_t nbi, __gpr uint8_t core,
                              __gpr uint8_t seg)
{
    __gpr uint32_t csr_addr;
    __gpr uint32_t pause_status;
    __gpr uint32_t nbi_island;
    __gpr uint32_t tgt_dev;
    __gpr uint32_t offset;
    __gpr uint32_t eth_cmd_config;
    __gpr uint32_t csr_offset;
    __gpr uint32_t csr_offset_chpcs;
    __gpr uint32_t nbicoreseg;
    __gpr uint8_t flush = 0;
    __lmem uint32_t *pause_count_ptr;
    __xread uint32_t xfr_eth_cmd_config;
    __xread uint32_t xfr_eth_rx_pause_status;
    __xread uint32_t xfer_pcsstatus1_0, xfer_pcsstatus1_1;
    __xread uint32_t xfer_pcsbaserstatus1;
    __xwrite uint32_t xfr_flush;
    SIGNAL sig1, sig2, sig_pcsstatus1_pre, sig_pcsstatus1, sig_pcsbaserstatus1;

    /* Build up CSR address for 
     * NbiXIsldXpbMap.NbiTopXpbMap.MacGlbAdrMap.MacEthernetY.MacEthSegZ */
    nbi_island = nbi + 8;
    /* NFP-6xxx-xC Databook Table 3.71 MAC Ethernet0 is 4, MAC Ethernet1 is 6 */
    tgt_dev = (core << 1) + 4;
    /* Build the top bits of the xpb address. */
    _xpb_csr_addr(csr_addr, 0, nbi_island, 1, tgt_dev);
    /* The CSR's are 0x400 apart. CSR offset based on the segment number. */
    csr_offset = seg << CSR_shf;

    /* Read EthCmdConfig */
    offset = csr_offset + ETH_CMD_CONFIG_BASE;
    __asm ct[xpb_read, xfr_eth_cmd_config, csr_addr, offset, 1], sig_done[sig1]

    /*Read the RX pause status */
    offset = csr_offset + ETH_RX_PAUSE_STATUS_BASE;
    __asm ct[xpb_read, xfr_eth_rx_pause_status, csr_addr, offset, 1], sig_done[sig2]

    /* Build up CSR offset for 
     * NbiXIsldXpbMap.NbiTopXpbMap.MacGlbAdrMap.MacEthernetY.MacEthChPcsSegZ */
     csr_offset_chpcs = csr_offset + (1 << MAC_ETH_CH_PCS_shf);
    
    /* Read EthChPcsStatus1 - Double read to clear latch low on link down. */
    offset = csr_offset_chpcs + MAC_ETH_CH_PCS_STATUS_1;
    __asm ct[xpb_read, xfer_pcsstatus1_0, csr_addr, offset, 1], sig_done[sig_pcsstatus1_pre]
    __asm ct[xpb_read, xfer_pcsstatus1_1, csr_addr, offset, 1], sig_done[sig_pcsstatus1]

    /* Read EthChPcsBaseRStatus1 */
    offset = csr_offset_chpcs + MAC_ETH_CH_PCS_BASER_STA_1;
    __asm ct[xpb_read, xfer_pcsbaserstatus1, csr_addr, offset, 1], sig_done[sig_pcsbaserstatus1]

    __asm ctx_arb[sig1, sig2, sig_pcsstatus1_pre, sig_pcsstatus1, sig_pcsbaserstatus1]
    __implicit_read(&xfer_pcsstatus1_0);

    /* Separate pause poll counts for each port identified by (nbi,core,seg).*/
    nbicoreseg = nbi * 24 + core * 12 + seg;
    pause_count_ptr = pause_counts + nbicoreseg;

    /* Update pause poll count and port paused bit map. */
    eth_cmd_config = xfr_eth_cmd_config;
    pause_status = xfr_eth_rx_pause_status & ETH_RX_PAUSE_STATUS_msk;
    if (pause_status > 0) {
        if (*pause_count_ptr == 0) {
            mac_set_port_paused(nbicoreseg);
        }
        /* Increment and saturate at largest positive integer. */
        *pause_count_ptr = (*pause_count_ptr + 1) & 0x7fffffff;

        if (*pause_count_ptr >= PAUSE_COUNT_THRES) {
            flush = 1;
        }
    } else {
        if (*pause_count_ptr != 0) {
            mac_clr_port_paused(nbicoreseg);
            *pause_count_ptr = 0;
        }
    }

    /* If receive link status is down. */
    if (flush == 0 && ((xfer_pcsstatus1_1 & 
            NFP_MAC_ETH_MACETHCHPCSSEG_STATUS1_RCVLINKSTATUS_msk) == 0)) {
        flush = 1;
    }
    /* If receive link status is down. */
    if (flush == 0 && ((xfer_pcsbaserstatus1 & 
            NFP_MAC_ETH_MACETHCHPCSSEG_BASERSTATUS1_RCVLINKSTATUS_msk) == 0)) {
        flush = 1;
    }
    /* block_lock is asserted when PCS establishes link - DB 10.2.5.1 */
    if (flush == 0 && ((xfer_pcsbaserstatus1 & 
            NFP_MAC_ETH_MACETHCHPCSSEG_BASERSTATUS1_BLOCKLOCKED_msk) == 0)) {
        flush = 1;
    }
    /* For purpose of flushing transmission, ignore EthRxEna and only check if 
     * MAC Transmit Path disabled. */
    if (flush == 0 && ((xfr_eth_cmd_config & 
            NFP_MAC_ETH_MACETHSEG_ETHCMDCONFIG_ETHTXENA_msk) == 0)) {
        flush = 1;
    }

    /* Set or clear the TX flush bit. */
    if (flush == 1) {
        xfr_flush = eth_cmd_config | (1<<ETH_TX_FLUSH_shf);
    } else {
        xfr_flush = eth_cmd_config & ~(1<<ETH_TX_FLUSH_shf);
    }
    offset = csr_offset + ETH_CMD_CONFIG_BASE;
    __asm ct[xpb_write, xfr_flush, csr_addr, offset, 1], ctx_swap[sig1]
}

/*
 * @param nbi                   NBI status to check.
 * @param active_ports_bitmap   Bitmap of active ports in NBI.
 */
void mac_poll_nbi_ports(__gpr uint8_t nbi, __gpr uint32_t active_ports_bitmap)
{
    __gpr uint32_t port;
    __gpr uint8_t core;
    __gpr uint8_t seg;

    while (active_ports_bitmap)
    {
        __asm ffs[port, active_ports_bitmap]
        active_ports_bitmap &= ~(1 << port);

        if (port >= 12) {
            core = 1;
            seg = port - 12;
        } else {
            core = 0;
            seg = port;
        }

        mac_flush_port_if_paused(nbi, core, seg);
    }
}

/* Continuously check whether the pause status has been asserted for any
 * MAC ports. Gate those ports and flush them if they have been paused for
 * PAUSE_COUNT_THRES polling iterations. */
void mac_poll()
{
    while (1) {
        __gpr uint32_t active_ports = 0;
        __xread uint32_t xfr_active_ports_nbi0;
        __xread uint32_t xfr_active_ports_nbi1;
        SIGNAL sig_nbi0;
        SIGNAL sig_nbi1;

        __cls_read(&xfr_active_ports_nbi0,
                  (__cls void *) __link_sym("pif_cfg_active_ports_nbi0"),
                  sizeof(xfr_active_ports_nbi0),
                  sizeof(xfr_active_ports_nbi0),
                  sig_done,
                  &sig_nbi0);
        __cls_read(&xfr_active_ports_nbi1,
                  (__cls void *) __link_sym("pif_cfg_active_ports_nbi1"),
                  sizeof(xfr_active_ports_nbi1),
                  sizeof(xfr_active_ports_nbi1),
                  sig_done,
                  &sig_nbi1);
        __asm ctx_arb[sig_nbi0, sig_nbi1]

        /* NBI0 */
        if (xfr_active_ports_nbi0 != 0x0) {
            active_ports = xfr_active_ports_nbi0;
            mac_poll_nbi_ports(MAC_NBI0, active_ports);
        }
        /* NBI1 */
        if (xfr_active_ports_nbi1 != 0x0) {
            active_ports = xfr_active_ports_nbi1;
            mac_poll_nbi_ports(MAC_NBI1, active_ports);
        }
        /* Wait a while before polling again if no ports active. */
        if (active_ports == 0x0) {
            ctx_wait(voluntary);
            sleep(BACKOFF_SLEEP);
        }
    }
}

void main()
{
    if (ctx() == 0) {
        mac_poll();
    } else {
        ctx_wait(kill);
    }
}
