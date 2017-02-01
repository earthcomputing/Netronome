/* Copyright (C) 2015,  Netronome Systems, Inc.  All rights reserved. */

#ifndef __PIF_PKT_H__
#define __PIF_PKT_H__

#include <pif_port_spec.h>
#include <pkt/pkt.h>

#ifndef PIF_PKT_INFO_TYPE
#define PIF_PKT_INFO_TYPE __nnr
#endif

#define PIF_PKT_SRC_NBI 0
#define PIF_PKT_SRC_PCIE 1

struct pif_pkt_info {
    union {
        struct {
            /* first word mirrors nbi_meta_pkt_info[0] */
            unsigned int isl:6;
            unsigned int pkt_num:10;
            unsigned int bls:2;
            unsigned int pkt_len:14;

            /* second word mirrors nbi_meta_pkt_info[1] */
            unsigned int split:1;
            unsigned int resv0:2;
            unsigned int muptr:29;

            /* third word is ... */
            unsigned int resv2:1;
            unsigned int ctm_size:2;
            unsigned int ctm_isl:7;
            unsigned int seqr:5;
            unsigned int unseq:1;
            unsigned int seq:16;

            /* fourth word is ingress info */
            struct {
                unsigned int type:2;
                unsigned int nbi:2;
                unsigned int port:8;

                unsigned int resv0:20;
            } ig_port;

            /* fifth word is egress info */
            struct {
                unsigned int type:2;
                unsigned int nbi:2;
                unsigned int port:8;

                unsigned int resv0:20;
            } eg_port;

            /* 6th + 7th word is the ctm buffer address */
            __addr40 void *pkt_buf;

            /* 8th word */
            unsigned int trunc_len:14;
            unsigned int pkt_pl_off:10; /* payload offset after parsing */
            unsigned int resv1:5;
            /* Checksum status reported by NBI */
            unsigned int ipv4_sum_ok:1;
            unsigned int udp_sum_ok:1;
            unsigned int tcp_sum_ok:1;

            /* 9th word - bit mask for toggling incremental calculated field
             * updates */
            unsigned int calc_fld_bmsk;
        };
        uint32_t __raw[9];
    };
};

#define PIF_PKT_INFO_META_0_wrd         0
#define PIF_PKT_INFO_ISLAND_lsb         26
#define PIF_PKT_INFO_PKT_NUM_lsb        16
#define PIF_PKT_INFO_BLS_lsb            14
#define PIF_PKT_INFO_PKT_LEN_lsb        0

#define PIF_PKT_INFO_CTM_INFO_wrd       2
#define PIF_PKT_INFO_CTM_SIZE_lsb       29
#define PIF_PKT_INFO_CTM_ISLAND_lsb     22
#define PIF_PKT_INFO_SEQR_lsb           17
#define PIF_PKT_INFO_UNSEQ_lsb          16
#define PIF_PKT_SEQ_lsb                 0

#define PIF_PKT_INFO_INGRESS_PORT_wrd   3
#define PIF_PKT_INFO_EGRESS_PORT_wrd    4


/* set the ingress or egress port */
#define PIF_PKT_INFO_SET_PORT(type, nbi, port) \
    ((type) << 30) | ((nbi) << 28) | ((port) << 20)

#define PIF_PKT_INFO_SET_META_0_INFO(isl, pnum, bls, plen) \
    ((isl << PIF_PKT_INFO_ISLAND_lsb) \
    | (pnum << PIF_PKT_INFO_PKT_NUM_lsb) \
    | (bls << PIF_PKT_INFO_BLS_lsb) \
    | (plen << PIF_PKT_INFO_PKT_LEN_lsb))

#define PIF_PKT_INFO_SET_CTM_INFO(size, isl, seq, unseq, seqr) \
    ((size << PIF_PKT_INFO_CTM_SIZE_lsb) \
    | (isl << PIF_PKT_INFO_CTM_ISLAND_lsb) \
    | (seq << PIF_PKT_SEQ_lsb) \
    | (unseq << PIF_PKT_INFO_UNSEQ_lsb) \
    | (seqr << PIF_PKT_INFO_SEQR_lsb))

/*
 * returned port is :
 * type: 2 .. pkt_port_type
 * port: 8
 */
#define PIF_PKT_P4PORT(_pktmd) \
    (((_pktmd)->ig_port.type) == PIF_PKT_SRC_PCIE) ? \
    PIF_PORT_SPEC((_pktmd)->ig_port.port, PIF_PORT_SPEC_TYPE_HOST, 0) : \
    PIF_PORT_SPEC(((_pktmd)->ig_port.port >> 2) | ((_pktmd)->ig_port.nbi << 6), PIF_PORT_SPEC_TYPE_PHYSICAL, 0)

/* get the nbi number out of the port spec */
#define PIF_PKT_PSPEC_NBI_of(pspec) ((PIF_PORT_SPEC_PORT_of(pspec) >> 6) & 0x1)

/* get the nbi mac channel out of the port spec */
/* note we mulitply be 4 due to the channel->port fan in inside the mac
 * this fan in is configuration dependent, so care must be taken to get it right
 */
#define PIF_PKT_PSPEC_CHAN_of(pspec) ((PIF_PORT_SPEC_PORT_of(pspec) & 0x3f) << 2)


/*
 * Calculate the start of the packet providing the offset in the packet and packet number.
 */
#define PIF_PKT_SOP(pktbuf, pktnum) \
    ((uint32_t)((uint64_t)pktbuf - (uint64_t)pkt_ctm_ptr40(__ISLAND, pktnum, 0)))

extern PIF_PKT_INFO_TYPE struct pif_pkt_info pif_pkt_info_global;

#ifdef EN_MAC_PREPEND
/*
 * MAC prepend
 * 8 bytes prepended by NBI on ingress.
 * Adapted from implementation in SDN project (me/pktutil.h).
 */
struct pif_mac_prepend {
    union {
        struct {
            unsigned int resv0:32;
            unsigned int csum_status:3; /* layer 4 checksum status */
            unsigned int resv1:7;
            unsigned int l3_info:2; /* layer 3 info */
            unsigned int resv2:20;
        };
        uint32_t __raw[2];
    };
};

/* layer 4 checksum status */
enum mac_csum_status {
    MAC_CSUM_STATUS_UNKNOWN,
    MAC_CSUM_STATUS_ESP,
    MAC_CSUM_STATUS_TCP_SUMOK,
    MAC_CSUM_STATUS_TCP_SUMFAIL,
    MAC_CSUM_STATUS_UDP_SUMOK,
    MAC_CSUM_STATUS_UDP_SUMFAIL,
    MAC_CSUM_STATUS_AH,
    MAC_CSUM_STATUS_FRAG
};

/* layer 3 info */
enum mac_l3_info {
    MAC_L3_INFO_UNKNOWN,
    MAC_L3_INFO_IPV6,
    MAC_L3_INFO_IPV4_SUMFAIL,
    MAC_L3_INFO_IPV4_SUMOK
};

/*
 * NBI metadata struct comprised of 24 bytes of catamaran metadata as well
 * as 8 bytes of prepended MAC metadata (pkt/pkt.h).
 */
struct pif_nbi_meta {
    union {
        struct {
            struct nbi_meta_pkt_info pkt_info;
            unsigned int seq:16;               /**< Packet sequence number */
            unsigned int nfp:2;                /**< Packet NFP number */
            unsigned int __reserved_12:3;      /**< Reserved */
            unsigned int seqr:3;               /**< Packet sequencer */
            unsigned int prot_err:1;           /**< Packet protocol error */
            unsigned int meta_type:3;          /**< Packet meta type nbi_id */
            unsigned int meta_valid:1;         /**< Packet meta valid */
            unsigned int buffer_pool:3;        /**< Buffer pool */

            unsigned int __reserved_19:13;     /**< Reserved */
            unsigned int e_term:1;             /**< Early termination of pkt */
            unsigned int tunnel_depth:2;       /**< Packet tunnel layer depth */
            unsigned int outer_l4_prot_type:4; /**< Packet outer L4 protocol */
            unsigned int outer_l3_prot_type:4; /**< Packet outer L3 protocol */
            unsigned int outer_l2_prot_type:5; /**< Packet outer L2 protocol */
            unsigned int __reserved_1:3;       /**< Reserved */

            unsigned int port:8;               /**< Port number */
            unsigned int hp_off1:8;            /**< Packet header parse offset
                                                * 1 Start of L4 payload
                                                * relative to PKT_NBI_OFFSET
                                                * */
            unsigned int hp_off0:8;            /**< Packet header parse offset
                                                * 2 Start of L4 relative to
                                                * PKT_NBI_OFFSET */
            unsigned int misc:8;               /**< Miscellaneous packet flags */

            unsigned int pe:1;                 /**< Packet error */
            unsigned int ie:1;                 /**< Interface error */
            unsigned int pw:1;                 /**< Packet warning */
            unsigned int sp:1;                 /**< Special handling indicator */
            unsigned int vlan_cnt:2;           /**< VLAN tag count */
            unsigned int it:1;                 /**< TCAM tag indicator */
            unsigned int ts:1;                 /**< Table set indicator */
            unsigned int outer_off:8;          /**< Packet outer offset */
            unsigned int lif:16;               /**< Logical interface id */

            struct pif_mac_prepend mac_meta;
        };
        uint32_t __raw[8];
    };
};

#define PIF_NBI_META_STRUCT pif_nbi_meta

#else

#define PIF_NBI_META_STRUCT nbi_meta_catamaran

#endif /* EN_MAC_PREPEND */

/** pif_pkt_write_header
 *
 * Insert hdrlen bytes by copying data at SOP to (SOP - hdrlen) for copylen
 *
 * @param pktoff         Offset at which to write header (in bytes)
 * @param lmem           Source to write to header (in local mem)
 * @param lmoff          Offset in local mem of header
 * @param hdrlen         Length to insert in bytes
 * @returns              Offset in packet after write (in bytes)
 */
unsigned int
pif_pkt_write_header(unsigned int pktoff,
                     __lmem uint32_t *lmem,
                     unsigned int lmoff,
                     unsigned int hdrlen);

/** pif_pkt_write_vheader
 *
 * Write variable length header to packet buffer.
 *
 * @param pktoff         Offset at which to write header (in bytes)
 * @param lmptr           Source to write to header (in local mem)
 * @param lmoff          Offset in local mem of header
 * @param lmsz         Length to insert in bytes
 * @param hdrlen         Length to insert in bytes
 * @param varptr         Length to insert in bytes
 * @param varoff         Length to insert in bytes
 */
void
pif_pkt_write_vheader(unsigned int pktoff,
                      __lmem uint8_t *lmptr,
                      unsigned int lmoff_b,
                      unsigned int lmsz,
                      unsigned int hdrlen,
                      __cls uint8_t *varptr,
                      unsigned int varoff_b);


/** pif_pkt_free_space
 *
 * Remove hdrlen bytes by moving all packet data from pktoff up/left
 * This is done with bulk engine commands.
 *
 * @param pktoff         Offset at which to remove hdr (in bytes)
 * @param hdrlen         Length to delete in bytes
 *
 */
void
pif_pkt_free_space(unsigned int pktoff,
                   unsigned int hdrlen);


/** pif_pkt_make_space
 *
 * Insert hdrlen bytes (space) by moving all packet from pktoff down/right.
 * This is done with bulk engine commands.
 *
 * @param pktoff         Offset at which to insert space (in bytes)
 * @param hdrlen         Length to insert in bytes
 *
 */
void
pif_pkt_make_space( unsigned int pktoff,
                   unsigned int hdrlen);



/** pif_pkt_insert_header
 *
 * Insert hdrlen bytes by copying data at SOP to (SOP - hdrlen) for copylen
 *
 * @param copylen        Length to copy in bytes (from start of packet)
 * @param hdrlen         Length to insert in bytes
 *
 */
void
pif_pkt_insert_header(unsigned int copylen,
                   unsigned int hdrlen);



/** pif_pkt_remove_header
 *
 * Remove hdrlen bytes by copying data at SOP to (SOP + hdrlen) for copylen
 *
 * @param copylen        Length to copy in bytes (from start of packet)
 * @param hdrlen         Length to remove in bytes
 *
 */
void
pif_pkt_remove_header(unsigned int copylen,
                   unsigned int hdrlen);

/** pif_pkt_dma_packet_down
 *
 * Insert space in a packet by shifting the packet down/right using DMA.
 * Use only CTM buffer if it is enough space, otherwise data in MU buffer
 * is also moved. If new length is more than MU buffer, allocate a larger buffer.
 * Error is returned if larger buffer is not available.
 *
 * @param insertlen         Length to insert in bytes, 64B aligned

 * @return 1 if success otherwise 0
 *
 */
int
pif_pkt_dma_packet_down(unsigned int insertlen);


#endif /* __PIF_PKT_H__ */
