/* Copyright (C) 2015,  Netronome Systems, Inc.  All rights reserved. */

/* PIF packet ops */

#include <stdint.h>
#include <nfp.h>

/* Flowenv includes */
#include <assert.h>
#include <nfp/me.h>
#include <nfp/mem_bulk.h>
#include <nfp6000/nfp_me.h>
#include <pktdma/pktdma.h>
#include <std/reg_utils.h>
#include <pkt/pkt.h>

/* NFD includes */
#ifndef PIF_GLOBAL_NFD_DISABLED
#include <vnic/shared/nfd_cfg.h>
#include <vnic/pci_in.h>
#include <vnic/pci_out.h>
#include <vnic/utils/ordering.h>
#include <shared/nfd_common.h>
#endif /* PIF_GLOBAL_NFD_DISABLED */

#include <pif_pkt_mod_script.h>

#include <blm.h>

#ifdef PIF_USE_GRO
#include <gro_cli.h>
#endif

#include <pif_pkt.h>
#include <pkt_ops.h>
#include <mac_flush.h>

/****************************************************************
 * Defines                                                      *
 ****************************************************************/

#define MIN(x, y)   (x > y) ? y : x

/* Only support single NFD work queue */
#define NFD_IN_WQ           0

/* CTM credit management, required for RX from host. Use all of CTM RX */
#define MAX_CREDITS         255

/* XXX assuming that isl 0 indicates that there is no CTM component
 *     (as NFD does)  */
#define PKT_HAS_CTM(pkt) (pkt.isl != 0)

/* Must be 256/512/1024/2048 ONLY */
#ifndef NFD_PKT_CTM_SIZE
#define PIF_CTM_TYPE PKT_CTM_SIZE_256
#else
#define PIF_CTM_TYPE NFD_PKT_CTM_SIZE
#endif
#define PIF_CTM_SIZE        (256 << PIF_CTM_TYPE)

#define CTM_ALLOC_ERR 0xffffffff

__export __shared __cls struct ctm_pkt_credits ctm_credits =
    {MAX_CREDITS, MAX_CREDITS};

/***************************************************************************
 * Rx                                                                      *
 ***************************************************************************/

/* Convert NBI RX descriptor to PIF packet descriptor */
__intrinsic static int
nbirxd_to_pif_pkt_info(__xread struct PIF_NBI_META_STRUCT *nbi_rxd)
{
    int ret = 0;
    __xread pkt_status_t pstatus;
#if 0
    /* FIXME: this would hopefully not be needed */
    /* Ensure pkt_rx_desc is zero before populating it */
    reg_zero(&pif_pkt_info_global, sizeof(struct pif_pkt_info));
#endif

    /* Check the metadata and meta data valid bit, if clear, return error
     * this should catch bad pico loads
     */
    if (((nbi_rxd->meta_type << 1) | nbi_rxd->meta_valid) == 0)
        ret = -1;

    /* Extract the isl, pkt_num, bls and pkt_len in one shot */
    pif_pkt_info_global.__raw[0] = nbi_rxd->pkt_info.__raw[0];

    /* Extract the split + MU pointer */
    pif_pkt_info_global.__raw[1] = nbi_rxd->pkt_info.__raw[1];


    /* Set bits instead of bit fields ... performance enhancement
     * pif_pkt_info_global.seqr = nbi_rxd->seqr;
     * pif_pkt_info_global.seq = nbi_rxd->seq;
     * pif_pkt_info_global.unseq = 1 or 0;
     * pif_pkt_info_global.ctm_isl = nbi_rxd->pkt_info.isl;
     * pif_pkt_info_global.ctm_size = pstatus.size
     */
    pkt_status_read(0, nbi_rxd->pkt_info.pnum, &pstatus); // Find out how big this CTM buffer is
    if (nbi_rxd->seqr == 0) {
        /* Set the un-sequenced bit. */
        pif_pkt_info_global.__raw[PIF_PKT_INFO_CTM_INFO_wrd] =  PIF_PKT_INFO_SET_CTM_INFO(pstatus.size,
                                                                                 nbi_rxd->pkt_info.isl,
                                                                                 nbi_rxd->seq,
                                                                                 1,
                                                                                 0);
    } else {
        /* Catamaran pico code uses sequencers 1-4 */
        /* Clear the un-sequenced bit. */
        pif_pkt_info_global.__raw[PIF_PKT_INFO_CTM_INFO_wrd] =  PIF_PKT_INFO_SET_CTM_INFO(pstatus.size,
                                                                                 nbi_rxd->pkt_info.isl,
                                                                                 nbi_rxd->seq,
                                                                                 0,
                                                                                 nbi_rxd->seqr);
    }

    /*
     * Ingress
     * pif_pkt_info_global.ig_port.type = PIF_PKT_SRC_NBI;
     * pif_pkt_info_global.ig_port.nbi = nbi_rxd->meta_type & 0x01;
     * pif_pkt_info_global.ig_port.port = nbi_rxd->port;
     */
    pif_pkt_info_global.__raw[PIF_PKT_INFO_INGRESS_PORT_wrd] = PIF_PKT_INFO_SET_PORT(PIF_PKT_SRC_NBI,
                                                                            nbi_rxd->meta_type & 0x01,
                                                                            nbi_rxd->port);

    /* tweak the pkt_len */
    pif_pkt_info_global.pkt_len = (nbi_rxd->pkt_info.len - PKT_MAC_PREPEND_BYTES);

    pif_pkt_info_global.pkt_buf = pkt_ctm_ptr40(0,
        pif_pkt_info_global.pkt_num, PKT_NBI_OFFSET + PKT_MAC_PREPEND_BYTES);



    /* set the truncate length to 0, indicate no truncate has happened */
    pif_pkt_info_global.trunc_len = 0;

#ifdef EN_MAC_PREPEND
    /* Set the MAC prepend checksum info. */
    if (nbi_rxd->mac_meta.l3_info == MAC_L3_INFO_IPV4_SUMOK)
        pif_pkt_info_global.ipv4_sum_ok = 1;
    if (nbi_rxd->mac_meta.csum_status == MAC_CSUM_STATUS_TCP_SUMOK) {
        pif_pkt_info_global.tcp_sum_ok = 1;
    } else if (nbi_rxd->mac_meta.csum_status == MAC_CSUM_STATUS_UDP_SUMOK) {
        pif_pkt_info_global.udp_sum_ok = 1;
    }
#endif

    return ret;
}


/* Convert NFD RX descriptor to PIF packet descriptor */
#ifndef PIF_GLOBAL_NFD_DISABLED
__intrinsic static int
nfdrxd_to_pif_pkt_info(__xread const struct nfd_in_pkt_desc *nfd_rxd)
{
    int ret = 0;

    if (nfd_rxd->invalid)
        ret = -1;

    /* Ensure pkt_rx_desc is zero before populating it */
    reg_zero(&pif_pkt_info_global, sizeof(struct pif_pkt_info));
    pif_pkt_info_global.muptr = nfd_rxd->buf_addr;

    /* Sequence number and Sequencer */
#ifdef PIF_USE_GRO
    pif_pkt_info_global.__raw[PIF_PKT_INFO_CTM_INFO_wrd] = PIF_PKT_INFO_SET_CTM_INFO(
        PIF_CTM_TYPE,
        __ISLAND,
        nfd_in_get_seqn((__xread struct nfd_in_pkt_desc *)nfd_rxd),
        0,
        GRO_CTX_NFD_BASE);
#else
    /* Set the un-sequenced bit. */
    pif_pkt_info_global.__raw[PIF_PKT_INFO_CTM_INFO_wrd] = PIF_PKT_INFO_SET_CTM_INFO(
        PIF_CTM_TYPE,
        __ISLAND,
        nfd_in_get_seqn((__xread struct nfd_in_pkt_desc *)nfd_rxd),
        1,
        0);
#endif

    /* Ingress */
    pif_pkt_info_global.__raw[PIF_PKT_INFO_INGRESS_PORT_wrd] = PIF_PKT_INFO_SET_PORT(PIF_PKT_SRC_PCIE, nfd_rxd->intf, nfd_rxd->q_num);

    /** @note Use nfd_in_map_queue(vnic,queue,q_num) if vnic & queue required. */

    /* CTM allocation */
    {
        __gpr uint32_t ctm_pnum;
        __gpr uint16_t i;
        __addr40 void *mu_ptr;
        __addr40 void *ctm_ptr;
        __xread uint64_t buf_xr[8];
        __xwrite uint64_t buf_xw[8];

        /* Poll for a CTM packet until one is returned. */
        for (ctm_pnum = CTM_ALLOC_ERR; ctm_pnum == CTM_ALLOC_ERR;) {
            ctm_pnum = pkt_ctm_alloc(&ctm_credits, __ISLAND, PIF_CTM_TYPE, 0, 0);
        }

        pif_pkt_info_global.__raw[0] = PIF_PKT_INFO_SET_META_0_INFO(__ISLAND,
                                                                    (ctm_pnum & 0x1FF),
                                                                    NFD_IN_BLM_REG_BLS,
                                                                    (nfd_rxd->data_len - nfd_rxd->offset));

        pif_pkt_info_global.pkt_buf = pkt_ctm_ptr40(__ISLAND, ctm_pnum, NFD_IN_DATA_OFFSET);
        pif_pkt_info_global.split =
            ((nfd_rxd->data_len - nfd_rxd->offset) > (PIF_CTM_SIZE - NFD_IN_DATA_OFFSET)) ? 1 : 0;

        for (i = 0; i < PIF_CTM_SIZE; i += sizeof(buf_xr)) {
            /* get a handle to both the mu and ctm pkt pointers */
            mu_ptr = (__addr40 void *)(((uint64_t)pif_pkt_info_global.muptr << 11) | i);
            ctm_ptr = pkt_ctm_ptr40(__ISLAND, ctm_pnum, i);

            mem_read64(buf_xr, mu_ptr, sizeof(buf_xr));
            reg_cp(buf_xw, buf_xr, sizeof(buf_xw));
            mem_write64(buf_xw, ctm_ptr, sizeof(buf_xw));
        }

        /* Write two 32-bit words of metadata to start of CTM buffer. */
        {
            __xwrite uint32_t meta_xw[2];

            ctm_ptr = pkt_ctm_ptr40(__ISLAND, ctm_pnum, 0);
            meta_xw[0] = pif_pkt_info_global.__raw[0];
            meta_xw[1] = pif_pkt_info_global.__raw[1];
            mem_write32(meta_xw, ctm_ptr, sizeof(meta_xw));
        }
    }

    return ret;
}
#endif /* PIF_GLOBAL_NFD_DISABLED */

int pkt_ops_rx(enum infra_src src)
{
    __xread struct PIF_NBI_META_STRUCT nbi_rxd;
    __gpr int err;
#ifndef PIF_GLOBAL_NFD_DISABLED
    SIGNAL nfd_rx_sig;
    __xread struct nfd_in_pkt_desc nfd_rxd;
    uint32_t nfd_q = NFD_IN_WQ;
#endif /* PIF_GLOBAL_NFD_DISABLED */

    switch (src) {
    case FROM_WIRE:
        pkt_nbi_recv_with_hdrs(&nbi_rxd, sizeof(nbi_rxd), PKT_NBI_OFFSET);
        err = nbirxd_to_pif_pkt_info(&nbi_rxd);
        break;

#ifndef PIF_GLOBAL_NFD_DISABLED
    case FROM_HOST:
        __nfd_in_recv(PCIE_ISL, nfd_q, &nfd_rxd, sig_done, &nfd_rx_sig);
        wait_for_all(&nfd_rx_sig);
        err = nfdrxd_to_pif_pkt_info(&nfd_rxd);
        break;
#endif /* PIF_GLOBAL_NFD_DISABLED */

    /* unsupported modes */
    default:
        break;
        // halt();
    }
    return err;
}


/***************************************************************************
 * Tx                                                                      *
 ***************************************************************************/

/* Check whether the NBI egress port is paused (i.e. has received a pause frame
 * from the remote port it is linked to). */
__intrinsic int is_port_paused()
{
    __gpr uint8_t is_paused;
    __xread uint64_t xfr_port_bitmap;
    __gpr uint32_t count = sizeof(xfr_port_bitmap)>>2;
    __gpr uint32_t isl = __ISLAND;
    __gpr uint32_t addr_hi;
    __gpr uint32_t addr_lo;
    __gpr uint32_t port;
    __gpr uint64_t port_bit;
    __cls __addr40 void *port_bitmap_addr = (__cls __addr40 void*) __link_sym("MAC_PORT_PAUSED_BITMAP");
    SIGNAL sig;

    is_paused = 0;
    port = pif_pkt_info_global.eg_port.nbi * 24 + pif_pkt_info_global.eg_port.port;
    port_bit = (uint64_t) 1 << port;

    addr_hi = ((uint64_t)port_bitmap_addr>>8) & 0xffffffff;
    addr_lo = (uint64_t)port_bitmap_addr & 0xff;
    __asm cls[read, xfr_port_bitmap, addr_hi, <<8, addr_lo, __ct_const_val(count)], ctx_swap[sig]

    if (xfr_port_bitmap & port_bit)
        is_paused = 1;

    return is_paused;
}

/* Convert PIF packet descriptor to NFD TX descriptor. */
#ifndef PIF_GLOBAL_NFD_DISABLED
__intrinsic static void
pif_pkt_info_to_nfdtxd(__gpr struct nfd_out_input *nfd_txd, __gpr uint32_t pkt_offset)
{
    /* Ensure pkt_rx_desc is zero before populating it. */
    reg_zero(nfd_txd, sizeof(struct nfd_out_input));

    /* Address details */
    nfd_txd->cpp.isl = (pif_pkt_info_global.ctm_isl & 0x3f);
    nfd_txd->cpp.pktnum = pif_pkt_info_global.pkt_num;
    nfd_txd->cpp.mu_addr = pif_pkt_info_global.muptr;
    nfd_txd->cpp.split = pif_pkt_info_global.ctm_size;
    /*nfd_txd->cpp.nbi = 0; /** @note only use for NBI packet. */
    nfd_txd->cpp.bls = pif_pkt_info_global.bls;

    /* Length and offset details. */
    nfd_txd->rxd.data_len = pif_pkt_info_global.pkt_len; /* + meta_len);*/
    /* nfd_txd->rxd.meta_len = 0; */
    nfd_txd->cpp.offset = pkt_offset;
    /* nfd_txd->cpp.offset = pkt_start - meta_len; */

    nfd_out_check_ctm_only(nfd_txd);

    nfd_txd->rxd.queue = pif_pkt_info_global.eg_port.port; /** @note Unnecessary? */
}
#endif /* PIF_GLOBAL_NFD_DISABLED */

#ifdef PIF_USE_GRO
#ifndef PIF_GLOBAL_NFD_DISABLED
/* Convert PIF packet descriptor to a GRO NFD TX descriptor. */
__intrinsic static void
pif_pkt_info_to_grotxd_nfd(__xwrite struct gro_meta_nfd3 *gro_meta, __gpr uint32_t pkt_offset)
{
    __gpr struct nfd_out_input nfd_txd;

    /* Ensure pkt_rx_desc is zero before populating it. */
    reg_zero(&nfd_txd, sizeof(struct nfd_out_input));

    /* Address details */
    nfd_txd.cpp.isl = (pif_pkt_info_global.ctm_isl & 0x3f);
    nfd_txd.cpp.pktnum = pif_pkt_info_global.pkt_num;
    nfd_txd.cpp.mu_addr = pif_pkt_info_global.muptr;
    nfd_txd.cpp.split = pif_pkt_info_global.ctm_size;
    /* nfd_txd->cpp.nbi = 0; /** @note only use for NBI packet. */
    nfd_txd.cpp.bls = pif_pkt_info_global.bls;

    /* Length and offset details */
    nfd_txd.rxd.data_len = pif_pkt_info_global.pkt_len; /* + meta_len); */
    /* nfd_txd.rxd.meta_len = 0; */
    nfd_txd.cpp.offset = pkt_offset; /** @note Depends on source - NBI/PCIe */
    /* nfd_txd.cpp.offset = pkt_start - meta_len; */

    nfd_out_check_ctm_only(&nfd_txd);

    nfd_txd.rxd.queue = pif_pkt_info_global.eg_port.port; /** @note Unnecessary? */

    nfd_txd.rxd.dd = 1;

    /* Build GRO meta out of the NFD OUT meta */
    gro_cli_nfd_desc2meta(gro_meta, (void *)&nfd_txd, pif_pkt_info_global.eg_port.nbi, pif_pkt_info_global.eg_port.port); /** @note nfd_q = pif_pkt_info_global.eg_port.port */
}
#endif /* PIF_GLOBAL_NFD_DISABLED */
#endif

int pkt_ops_tx(enum infra_dst dst, int pkt_byteoffset)
{
#ifndef PIF_GLOBAL_NFD_DISABLED
    SIGNAL nfd_tx_sig;
    __xwrite struct nfd_out_input nfd_txd_xfr;
    __gpr struct nfd_out_input nfd_txd;
    __gpr uint32_t nfd_q;
#endif /* PIF_GLOBAL_NFD_DISABLED */
#ifdef PIF_USE_GRO
    __xwrite struct gro_meta_nbi gro_nbi_meta;
    __xwrite struct gro_meta_nfd3 gro_nfd_meta;
    __xwrite struct gro_meta_drop drop_meta;
#endif
    __gpr struct nbi_meta_pkt_info pkt_info;
    __gpr struct pkt_ms_info msi;
    __addr40 void *mu_ptr;
    __gpr uint32_t credit;


    switch (dst) {
    case TO_WIRE:
        /* Do not transmit; instead drop packets when pause frame(s) received.
         * from remote port. */
        if (is_port_paused()) {
            pkt_ops_tx_drop();
            return -1;
        }

        /* Write modification script */
        msi = pif_pkt_msd_write(pkt_ctm_ptr40(pif_pkt_info_global.ctm_isl,
                                pif_pkt_info_global.pkt_num, 0),
                                pkt_byteoffset);
        /* Send the packet */
#ifdef PIF_USE_GRO
        if (!pif_pkt_info_global.unseq) {
            gro_cli_build_nbi_meta(&gro_nbi_meta,
                                   pif_pkt_info_global.ctm_isl,
                                   pif_pkt_info_global.pkt_num,
                                   pif_pkt_info_global.ctm_size,
                                   ((msi.off_enc + 1) << 3),
                                   (pkt_byteoffset + pif_pkt_info_global.pkt_len),
                                   pif_pkt_info_global.eg_port.nbi,
                                   (pif_pkt_info_global.eg_port.port * 8));
            gro_cli_send((void *)&gro_nbi_meta, pif_pkt_info_global.seqr, pif_pkt_info_global.seq);
        } else {
#endif
        pkt_nbi_send(pif_pkt_info_global.ctm_isl,
                     pif_pkt_info_global.pkt_num,
                     &msi,
                     pif_pkt_info_global.pkt_len,
                     pif_pkt_info_global.eg_port.nbi,
                     pif_pkt_info_global.eg_port.port * 8, /* TM Q 8:1 fan in */
                     pif_pkt_info_global.seqr,
                     pif_pkt_info_global.seq,
                     pif_pkt_info_global.ctm_size);
#ifdef PIF_USE_GRO
        }
#endif
        break;
#ifndef PIF_GLOBAL_NFD_DISABLED
    case TO_HOST:
        nfd_q = pif_pkt_info_global.eg_port.port;
        /** @note Can use nfd_q = nfd_out_map_queue(vnic, queue); */

        credit = nfd_out_get_credit(PCIE_ISL, nfd_q, 1);

        if (credit) {
#ifdef PIF_USE_GRO
            if (!pif_pkt_info_global.unseq) {
                pif_pkt_info_to_grotxd_nfd(&gro_nfd_meta, pkt_byteoffset);
                gro_cli_send((void *)&gro_nfd_meta, pif_pkt_info_global.seqr, pif_pkt_info_global.seq);
            } else {
#endif
                pif_pkt_info_to_nfdtxd(&nfd_txd, pkt_byteoffset);
                __nfd_out_send(PCIE_ISL,
                            nfd_q,
                            &nfd_txd_xfr,
                            &nfd_txd,
                            sig_done,
                            &nfd_tx_sig);
                wait_for_all(&nfd_tx_sig);
#ifdef PIF_USE_GRO
            }
#endif
        } else {
            /*  We don't have a NFD credit, drop the packet. */
            pkt_ops_tx_drop();
            return -1;
        }
        break;
#endif /* PIF_GLOBAL_NFD_DISABLED */

    /* unsupported modes */
    default:
        return -1;
        break;
    }

    return 0;
}

int pkt_ops_tx_drop()
{
#ifndef PIF_GLOBAL_NFD_DISABLED
    SIGNAL nfd_tx_sig;
#endif /* PIF_GLOBAL_NFD_DISABLED */
#ifdef PIF_USE_GRO
    __xwrite struct gro_meta_drop drop_meta;
#endif
    __gpr struct pkt_ms_info msi;
    __addr40 void *mu_ptr;


    if (pif_pkt_info_global.ig_port.type == PIF_PKT_SRC_NBI) {
#ifdef PIF_USE_GRO
        if (!pif_pkt_info_global.unseq) {
            /* Tell GRO to skip the packet's sequence number and free CTM buffer */
            gro_cli_build_drop_ctm_buf_meta(&drop_meta,
                                            pif_pkt_info_global.ctm_isl,
                                            pif_pkt_info_global.pkt_num);
            gro_cli_send((void *)&drop_meta,
                         pif_pkt_info_global.seqr,
                         pif_pkt_info_global.seq);
        } else {
#endif
            /* Notify the NBI to ignore the packets sequence number */
            pkt_nbi_drop_seq(pif_pkt_info_global.ctm_isl,
                             pif_pkt_info_global.pkt_num,
                             &msi,
                             pif_pkt_info_global.pkt_len,
                             0, 0,
                             pif_pkt_info_global.seqr, pif_pkt_info_global.seq,
                             pif_pkt_info_global.ctm_size);
            /* Free CTM buffer. */
            pkt_ctm_free(pif_pkt_info_global.ctm_isl, pif_pkt_info_global.pkt_num);
#ifdef PIF_USE_GRO
        }
#endif
        /* Free MU buffer. */
        mu_ptr = (__addr40 void *)((uint64_t)pif_pkt_info_global.muptr << 11);
        blm_buf_free(blm_buf_ptr2handle(mu_ptr), pif_pkt_info_global.bls);
    } else {

#ifdef PIF_USE_GRO
        if (!pif_pkt_info_global.unseq) {
            /* Tell GRO to skip the packet's sequence number and free CTM buffer */
            gro_cli_build_drop_ctm_buf_meta(&drop_meta,
                                            pif_pkt_info_global.ctm_isl,
                                            pif_pkt_info_global.pkt_num);
            gro_cli_send((void *)&drop_meta, pif_pkt_info_global.seqr, pif_pkt_info_global.seq);
        } else {
#endif
            /* Free CTM buffer. */
            pkt_ctm_free(pif_pkt_info_global.ctm_isl, pif_pkt_info_global.pkt_num);
#ifdef PIF_USE_GRO
        }
#endif
        /* Free MU buffer. */
        mu_ptr = (__addr40 void *)((uint64_t)pif_pkt_info_global.muptr << 11);
        blm_buf_free(blm_buf_ptr2handle(mu_ptr), pif_pkt_info_global.bls);

    }

    return 0;
}
