/* Copyright (C) 2015,  Netronome Systems, Inc.  All rights reserved.
 *
 * This header contians information for reading and manipulating the managed-C
 * packet metadata.
 *
 */

#ifndef __PKT_META_H__
#define __PKT_META_H__

/* return codes for mc_main() */

#define MC_DROP    -1
#define MC_FORWARD 0

/* register type used for packet metadata */

#ifndef PKT_META_TYPE
#define PKT_META_TYPE __nnr
#endif

/* source/destination of the packet */
/* physical port */
#define PKT_META_SRC_NBI 0
#define PKT_META_DST_NBI PKT_META_SRC_NBI
/* from host/pcie */
#define PKT_META_SRC_PCIE 1
#define PKT_META_DST_PCIE PKT_META_SRC_PCIE

/* packet metadata structure */
struct pkt_meta {
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
            unsigned int resv1:18;

        };
        uint32_t __raw[8];
    };
};

/* extern - packet metadata is declared elsewhere and is global */
extern PKT_META_TYPE struct pkt_meta pif_pkt_info_global;

/* helper macro for declaring pointer to packet metadata */
#define PKT_META_PTR (&pif_pkt_info_global)

/* function for setting how packet are forwarded */
__forceinline void pkt_meta_forward_nbi(PKT_META_TYPE struct pkt_meta *pmeta,
                                        int nbi, int port)
{
    pmeta->eg_port.type = PKT_META_SRC_NBI;
    pmeta->eg_port.nbi = nbi;
    /* there is a 1:4 mapping of port # */
    pmeta->eg_port.port = port << 2;
}

__forceinline void pkt_meta_forward_pcie(PKT_META_TYPE struct pkt_meta *pmeta,
                                        int vf)
{
    pmeta->eg_port.type = PKT_META_SRC_PCIE;
    pmeta->eg_port.nbi = 0;
    pmeta->eg_port.port = vf;
}


#endif /* __PKT_META_H__ */
