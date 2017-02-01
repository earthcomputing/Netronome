/*
 * Copyright 2016 Netronome, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file          npe_hdr_extract.c
 * @brief         Functions to extract header data from a network packet
 */


#include <npe_arp.h>
#include <npe_eth.h>
#include <npe_gre.h>
#include <npe_ip.h>
#include <npe_tcp.h>
#include <npe_udp.h>

#ifdef __NFP_TOOL_NFCC
#include <std/reg_utils.h>
#define __ASSERT(expr) __ct_assert(expr, #expr)
#else /* GCC */
#include <assert.h>
#include <string.h>
#define __ASSERT assert
#define __gpr
#define __lmem
#define __is_in_lmem(_VAL) 1
#define __is_in_reg_or_lmem(_VAL) 1
#define __is_ct_const(_VAL) 1
#endif

#include <stdint.h>

#include <npe_mem.h>
#include <npe_hdr_extract.h>


/* Internal routines and macros. Mainly ported from flowenv library
   hdr_ext.c in order to correct some errors plus so can be available
   for GCC */

/* Macros to encode and decode the result value */
#define HE_RES(_np, _l)      ((((_np) & 0xffff) << 16) | ((_l) & 0xffff))
#define HE_RES_LEN_of(_x)    (((_x) & 0xffff))
#define HE_RES_PROTO_of(_x)  ((_x) >> 16)


/**
 * Indication of the protocol in the next header
 *
 * The IPv6 extension header are arranged such that one can simply can
 * check with @HE_IP6_EXT (ie bit 8 set) for any of the header, if one
 * wants to skip them.
 */
enum he_proto {
    HE_NONE    = 0,      /**  0: No more headers, just data */
    HE_ETHER,            /**  1: Ethernet header */
    HE_8021Q,            /**  2: 802.1Q (VLAN) header */
    HE_ARP,              /**  3: ARP header */
    HE_IP4,              /**  4: IPv4 header */
    HE_IP6,              /**  5: IPv6 header */
    HE_TCP,              /**  6: TCP header */
    HE_UDP,              /**  7: UDP header */
    HE_GRE,              /**  8: GRE header */
    HE_VXLAN,            /**  9: VXLAN header */
    HE_ESP,              /** 10: ESP header */
    HE_MPLS,             /** 11: MPLS header */

    HE_IP6_EXT =  0x100, /** IPv6 Extension header */
    HE_IP6_HBH =  0x101, /** IPv6 Hop-by-Hop Options header */
    HE_IP6_RT =   0x102, /** IPv6 Routing Extension header */
    HE_IP6_FRAG = 0x103, /** IPv6 Fragmentation header */
    HE_IP6_NONE = 0x104, /** IPv6 No Next Header header */
    HE_IP6_DST =  0x105, /** IPv6 Destination Options header */

    HE_UNKNOWN = 0xffff, /** Unknown/unhandled header */
};

__NPE_INTRINSIC int
he_eth_fit(sz, off)
{
    __ASSERT(sz >= sizeof(struct eth_hdr));
    return (off + sizeof(struct eth_hdr)) <= sz;
}

#ifdef __NFP_TOOL_NFCC
#define HE_ETH_FUNC(dst)                                                \
    *dst =  *(__lmem struct eth_hdr *)(((__lmem char *)src_buf) + off); \
    switch(dst->type) {                                                 \
    case NET_ETH_TYPE_IPV4: next_proto = HE_IP4; break;                 \
    case NET_ETH_TYPE_TPID: next_proto = HE_8021Q; break;               \
    case NET_ETH_TYPE_IPV6: next_proto = HE_IP6; break;                 \
    case NET_ETH_TYPE_ARP: next_proto = HE_ARP; break;                  \
    case NET_ETH_TYPE_MPLS: next_proto = HE_MPLS; break;                \
    default: next_proto = HE_UNKNOWN;                                   \
    }
#else /* GCC */
#define HE_ETH_FUNC(dst)                                                \
    memmove((void *)dst, (void *)src_buf + off, sizeof(struct eth_hdr));\
    switch(dst->type) {                                                 \
    case NET_ETH_TYPE_IPV4: next_proto = HE_IP4; break;                 \
    case NET_ETH_TYPE_TPID: next_proto = HE_8021Q; break;               \
    case NET_ETH_TYPE_IPV6: next_proto = HE_IP6; break;                 \
    case NET_ETH_TYPE_ARP: next_proto = HE_ARP; break;                  \
    case NET_ETH_TYPE_MPLS: next_proto = HE_MPLS; break;                \
    default: next_proto = HE_UNKNOWN;                                   \
    }
#endif

__NPE_INTRINSIC unsigned int
he_eth(void *src_buf, int off, void *dst)
{
    __gpr unsigned int next_proto;

    __ASSERT(__is_in_lmem(src_buf));
    __ASSERT(__is_in_reg_or_lmem(dst));

#ifdef __HE_ETH
#error "Attempting to redefine __HE_ETH"
#endif

    if (__is_in_lmem(dst)) {
#define __HE_ETH ((__lmem struct eth_hdr *)dst)
        HE_ETH_FUNC(__HE_ETH);
#undef __HE_ETH
    } else {
#define __HE_ETH ((__gpr struct eth_hdr *)dst)
        HE_ETH_FUNC(__HE_ETH);
#undef __HE_ETH
    }

    return HE_RES(next_proto, sizeof(struct eth_hdr));
}

__NPE_INTRINSIC int
he_vlan_fit(sz, off)
{
    __ASSERT(__is_ct_const(sz));
    __ASSERT(sz >= sizeof(struct vlan_hdr));
    return (off + sizeof(struct vlan_hdr)) <= sz;
}

#define HE_VLAN_FUNC(dst)                                               \
    *dst = *(__lmem struct vlan_hdr *)(((__lmem char *)src_buf) + off); \
    switch(dst->type) {                                                 \
    case NET_ETH_TYPE_IPV4: next_proto = HE_IP4; break;                 \
    case NET_ETH_TYPE_IPV6: next_proto = HE_IP6; break;                 \
    case NET_ETH_TYPE_MPLS: next_proto = HE_MPLS; break;                \
    default: next_proto = HE_UNKNOWN;                                   \
    }

__NPE_INTRINSIC unsigned int
he_vlan(void *src_buf, int off, void *dst)
{
    __gpr unsigned int next_proto;

#ifdef __HE_VLAN
    #error "Attempting to redefine __HE_VLAN"
#endif

    __ASSERT(__is_in_lmem(src_buf));
    __ASSERT(__is_in_reg_or_lmem(dst));

    if (__is_in_lmem(dst)) {
#define __HE_VLAN ((__lmem struct vlan_hdr *)dst)
        HE_VLAN_FUNC(__HE_VLAN);
#undef __HE_VLAN
    } else {
#define __HE_VLAN ((__gpr struct vlan_hdr *)dst)
        HE_VLAN_FUNC(__HE_VLAN);
#undef __HE_VLAN
    }

    return HE_RES(next_proto, sizeof(struct vlan_hdr));
}

__NPE_INTRINSIC int
he_arp_fit(sz, off)
{
    __ASSERT(__is_ct_const(sz));
    __ASSERT(sz >= sizeof(struct arp_hdr));
    return (off + sizeof(struct arp_hdr)) <= sz;
}

__NPE_INTRINSIC unsigned int
he_arp(void *src_buf, int off, void *dst)
{
    __ASSERT(__is_in_lmem(src_buf));
    __ASSERT(__is_in_reg_or_lmem(dst));

    if (__is_in_lmem(dst)) {
        *((__lmem struct arp_hdr *)dst) =
            *(__lmem struct arp_hdr *)(((__lmem char *)src_buf) + off);
    } else {
        *((__gpr struct arp_hdr *)dst) =
            *(__lmem struct arp_hdr *)(((__lmem char *)src_buf) + off);
    }

    return HE_RES(HE_NONE, sizeof(struct arp_hdr));
}

__NPE_INTRINSIC int
he_ip4_fit(sz, off)
{
    __ASSERT(__is_ct_const(sz));
    __ASSERT(sz >= sizeof(struct ip4_hdr));
    return (off + sizeof(struct ip4_hdr)) <= sz;
}

#define HE_IP4_FUNC(dst)                                                \
    *dst = *(__lmem struct ip4_hdr *)(((__lmem char *)src_buf) + off);  \
                                                                        \
    switch(dst->proto) {                                                \
    case NET_IP_PROTO_TCP: next_proto = HE_TCP; break;                  \
    case NET_IP_PROTO_UDP: next_proto = HE_UDP; break;                  \
    case NET_IP_PROTO_GRE: next_proto = HE_GRE; break;                  \
    case NET_IP_PROTO_ESP: next_proto = HE_ESP; break;                  \
    default: next_proto = HE_UNKNOWN;                                   \
    }                                                                   \
                                                                        \
    if (dst->frag & NET_IP_FLAGS_MF)                                    \
        next_proto = HE_UNKNOWN;                                        \
                                                                        \
    ret = HE_RES(next_proto, 4 * dst->hl);

__NPE_INTRINSIC unsigned int
he_ip4(void *src_buf, int off, void *dst)
{
    __gpr unsigned int next_proto;
    __gpr int ret;

    __ASSERT(__is_in_lmem(src_buf));
    __ASSERT(__is_in_reg_or_lmem(dst));

#ifdef __HE_IP4
    #error "Attempting to redefine __HE_IP4"
#endif

    if (__is_in_lmem(dst)) {
#define __HE_IP4 ((__lmem struct ip4_hdr *)dst)
        HE_IP4_FUNC(__HE_IP4);
#undef __HE_IP4
    } else {
#define __HE_IP4 ((__gpr struct ip4_hdr *)dst)
        HE_IP4_FUNC(__HE_IP4);
#undef __HE_IP4
    }

    return ret;
}

__NPE_INTRINSIC int
he_ip6_fit(sz, off)
{
    __ASSERT(__is_ct_const(sz));
    __ASSERT(sz >= sizeof(struct ip6_hdr));
    return (off + sizeof(struct ip6_hdr)) <= sz;
}

/* We use this portion of the switch statement in several places for
 * parsing IPv6 and extension header. Define it as a macro to avoid
 * code duplication.*/
#define _IP6_PROTO_SWITCH \
    case NET_IP_PROTO_TCP: next_proto = HE_TCP; break;              \
    case NET_IP_PROTO_UDP: next_proto = HE_UDP; break;              \
    case NET_IP_PROTO_GRE: next_proto = HE_GRE; break;              \
    case NET_IP_PROTO_ESP: next_proto = HE_ESP; break;              \
    case NET_IP_PROTO_HOPOPT: next_proto = HE_IP6_HBH; break;       \
    case NET_IP_PROTO_ROUTING: next_proto = HE_IP6_RT; break;       \
    case NET_IP_PROTO_FRAG: next_proto = HE_IP6_FRAG; break;        \
    case NET_IP_PROTO_NONE: next_proto = HE_NONE; break;            \
    case NET_IP_PROTO_DSTOPTS: next_proto = HE_IP6_DST; break;      \
    default: next_proto = HE_UNKNOWN

#define HE_IP6_FUNC(dst)                                                \
    *dst = *(__lmem struct ip6_hdr *)(((__lmem char *)src_buf) + off);  \
    switch(dst->nh) {                                                   \
        _IP6_PROTO_SWITCH;                                              \
    }                                                                   \
    ret = HE_RES(next_proto, sizeof(struct ip6_hdr));

__NPE_INTRINSIC unsigned int
he_ip6(void *src_buf, int off, void *dst)
{
    __gpr unsigned int next_proto;
    __gpr int ret;

    __ASSERT(__is_in_lmem(src_buf));
    __ASSERT(__is_in_reg_or_lmem(dst));

#ifdef __HE_IP6
    #error "Attempting to redefine __HE_IP6"
#endif

    if (__is_in_lmem(dst)) {
#define __HE_IP6 ((__lmem struct ip6_hdr *)dst)
        HE_IP6_FUNC(__HE_IP6);
#undef __HE_IP6
    } else {
#define __HE_IP6 ((__gpr struct ip6_hdr *)dst)
        HE_IP6_FUNC(__HE_IP6);
#undef __HE_IP6
    }

    return ret;
}

/* Minimal IPv6 extension header to find out where the next header
 * starts */
__PACKED struct _ip6_ext {
    uint8_t nh;                     /** Next protocol */
    uint8_t len;                    /** Length of the extension header */
    uint16_t pad0;                  /** Padding */
};

__NPE_INTRINSIC int
he_ip6_ext_skip_fit(sz, off)
{
    __ASSERT(__is_ct_const(sz));
    __ASSERT(sz >= sizeof(struct _ip6_ext));
    return (off + sizeof(struct _ip6_ext)) <= sz;
}

__NPE_INTRINSIC unsigned int
he_ip6_ext_skip(void *src_buf, int off)
{
    __gpr struct _ip6_ext dst;
    __gpr unsigned int next_proto;
    __gpr unsigned int len;

    __ASSERT(__is_in_lmem(src_buf));

    dst = *(__lmem struct _ip6_ext *)(((__lmem char *)src_buf) + off);

    switch(dst.nh) {
        _IP6_PROTO_SWITCH;
    }

    /* IPv6 Frag header is just that, but otherwise the len field is
     * the number of 8 octets + 8 */
    if (next_proto == HE_IP6_FRAG)
        len = sizeof(struct ip6_frag);
    else
        len = 8 + (dst.len * 8);

    return HE_RES(next_proto, len);
}

#undef _IP6_PROTO_SWITCH

__NPE_INTRINSIC int
he_tcp_fit(sz, off)
{
    __ASSERT(__is_ct_const(sz));
    __ASSERT(sz >= sizeof(struct tcp_hdr));
    return (off + sizeof(struct tcp_hdr)) <= sz;
}

#define HE_TCP_FUNC(dst)                                                \
    *dst = *(__lmem struct tcp_hdr *)(((__lmem char *)src_buf) + off);  \
                                                                        \
    next_proto = HE_NONE;                                               \
                                                                        \
    ret = HE_RES(next_proto, 4 * dst->off);

__NPE_INTRINSIC unsigned int
he_tcp(void *src_buf, int off, void *dst)
{
    __gpr unsigned int next_proto;
    __gpr int ret;

    __ASSERT(__is_in_lmem(src_buf));
    __ASSERT(__is_in_reg_or_lmem(dst));

#ifdef __HE_TCP
    #error "Attempting to redefine __HE_TCP"
#endif

    if (__is_in_lmem(dst)) {
#define __HE_TCP ((__lmem struct tcp_hdr *)dst)
        HE_TCP_FUNC(__HE_TCP);
#undef __HE_TCP
    } else {
#define __HE_TCP ((__gpr struct tcp_hdr *)dst)
        HE_TCP_FUNC(__HE_TCP);
#undef __HE_TCP
    }

    return ret;
}

__NPE_INTRINSIC int
he_udp_fit(sz, off)
{
    __ASSERT(__is_ct_const(sz));
    __ASSERT(sz >= sizeof(struct udp_hdr));
    return (off + sizeof(struct udp_hdr)) <= sz;
}

#define HE_UDP_FUNC(dst, prt)                                           \
    *dst = *(__lmem struct udp_hdr *)(((__lmem char *)src_buf) + off);  \
    if (prt && dst->dport == prt)                                       \
        next_proto = HE_VXLAN;                                          \
    else                                                                \
        next_proto = HE_NONE;

__NPE_INTRINSIC unsigned int
he_udp(void *src_buf, int off, void *dst, unsigned int vxln_prt)
{
    __gpr unsigned int next_proto;

    __ASSERT(__is_in_lmem(src_buf));
    __ASSERT(__is_in_reg_or_lmem(dst));

#ifdef __HE_UDP
    #error "Attempting to redefine __HE_UDP"
#endif

    if (__is_in_lmem(dst)) {
#define __HE_UDP ((__lmem struct udp_hdr *)dst)
        HE_UDP_FUNC(__HE_UDP, vxln_prt);
#undef __HE_UDP
    } else {
#define __HE_UDP ((__gpr struct udp_hdr *)dst)
        HE_UDP_FUNC(__HE_UDP, vxln_prt);
#undef __HE_UDP
    }

    return HE_RES(next_proto, sizeof(struct udp_hdr));
}

__NPE_INTRINSIC int
he_gre_fit(sz, off)
{
    /* biggest GRE hdr has the optional checksum , key and sequence
     * number: total of 12B
     */
    __ASSERT(__is_ct_const(sz));
    __ASSERT(sz >= sizeof(struct gre_hdr) + 12);
    return (off + sizeof(struct gre_hdr) + 12) <= sz;
}

#define HE_GRE_FUNC(dst)                                                \
    *dst = *(__lmem struct gre_hdr *)(((__lmem char *)src_buf) + off);  \
                                                                        \
    flags = dst->flags;                                                 \
                                                                        \
    switch(dst->proto) {                                                \
    case NET_ETH_TYPE_TEB: next_proto = HE_ETHER; break;                \
    case NET_ETH_TYPE_IPV4: next_proto = HE_IP4; break ;                \
    case NET_ETH_TYPE_IPV6: next_proto = HE_IP6; break ;                \
    case NET_ETH_TYPE_MPLS: next_proto = HE_MPLS; break;                \
    default: next_proto = HE_UNKNOWN;                                   \
    }

__NPE_INTRINSIC unsigned int
he_gre(void *src_buf, int off, void *dst)
{
    __gpr unsigned int next_proto;
    __gpr uint32_t flags;
    __gpr uint32_t len;

    __ASSERT(__is_in_lmem(src_buf));
    __ASSERT(__is_in_reg_or_lmem(dst));

    if (__is_in_lmem(dst)) {
#define __HE_GRE ((__lmem struct gre_hdr *)dst)
        HE_GRE_FUNC(__HE_GRE);
#undef __HE_GRE
    } else {
#define __HE_GRE ((__gpr struct gre_hdr *)dst)
        HE_GRE_FUNC(__HE_GRE);
#undef __HE_GRE
    }

    /* Work out the length of the whole GRE header */
    len = 4;
    if (flags & NET_GRE_FLAGS_CSUM_PRESENT)
        len += 4;
    if (flags & NET_GRE_FLAGS_KEY_PRESENT)
        len += 4;
    if (flags & NET_GRE_FLAGS_SEQ_PRESENT)
        len += 4;

    return HE_RES(next_proto, len);
}

__NPE_INTRINSIC void
he_gre_nvgre(void *src_buf, int off, void *dst)
{
    __ASSERT(__is_in_lmem(src_buf));
    __ASSERT(__is_in_reg_or_lmem(dst));

    /* move offset to point to the start of the optional fields */
    off += sizeof(struct gre_hdr);

    if (__is_in_lmem(dst)) {
        *((__lmem struct nvgre_ext_hdr *)dst) =
            *(__lmem struct nvgre_ext_hdr *)(((__lmem char *)src_buf) + off);
    } else {
        *((__gpr struct nvgre_ext_hdr *)dst) =
            *(__lmem struct nvgre_ext_hdr *)(((__lmem char *)src_buf) + off);
    }
}

__NPE_INTRINSIC int
he_vxlan_fit(sz, off)
{
    __ASSERT(__is_ct_const(sz));
    __ASSERT(sz >= sizeof(struct vxlan_hdr));
    return (off + sizeof(struct vxlan_hdr)) <= sz;
}

#define HE_VXLAN_FUNC(dst)                                                \
    *dst = *(__lmem struct vxlan_hdr *)(((__lmem char *)src_buf) + off);

__NPE_INTRINSIC unsigned int
he_vxlan(void *src_buf, int off, void *dst)
{
    __gpr unsigned int next_proto;
    __gpr uint32_t len;

    __ASSERT(__is_in_lmem(src_buf));
    __ASSERT(__is_in_reg_or_lmem(dst));

    if (__is_in_lmem(dst)) {
#define __HE_VXLAN ((__lmem struct vxlan_hdr *)dst)
        HE_VXLAN_FUNC(__HE_VXLAN);
#undef __HE_VXLAN
    } else {
#define __HE_VXLAN ((__gpr struct vxlan_hdr *)dst)
        HE_VXLAN_FUNC(__HE_VXLAN);
#undef __HE_VXLAN
    }

    next_proto = HE_ETHER;
    len = sizeof(struct vxlan_hdr);

    return HE_RES(next_proto, len);
}

__NPE_INTRINSIC int
he_mpls_fit(sz, off)
{
    __ASSERT(__is_ct_const(sz));
    __ASSERT(sz >= sizeof(struct mpls_hdr));
    return (off + sizeof(struct mpls_hdr)) <= sz;
}

#define HE_MPLS_FUNC(dst)                                                \
    *dst = *(__lmem struct mpls_hdr *)(((__lmem char *)src_buf) + off);

__NPE_INTRINSIC unsigned int
he_mpls(void *src_buf, int off, void *dst)
{
    __ASSERT(__is_in_lmem(src_buf));
    __ASSERT(__is_in_reg_or_lmem(dst));

    if (__is_in_lmem(dst)) {
#define __HE_MPLS ((__lmem struct mpls_hdr *)dst)
        HE_MPLS_FUNC(__HE_MPLS);
#undef __HE_MPLS
    } else {
#define __HE_MPLS ((__gpr struct mpls_hdr *)dst)
        HE_MPLS_FUNC(__HE_MPLS);
#undef __HE_MPLS
    }

    return HE_RES(HE_NONE, sizeof(struct mpls_hdr));
}

__NPE_INTRINSIC void npe_hdr_extract(npe_mem_ptr_t buf_addr,
                                     uint32_t buf_off,
                                     void *pkt_hdrs,
                                     void *pkt_enc_hdrs)
{
    __lmem struct npe_pkt_hdrs *ph;
    __lmem struct npe_pkt_enc_hdrs *peh;

    __lmem uint32_t src_buf[16];
    __gpr int src_off = buf_off;
    __gpr unsigned int res;
    __gpr unsigned int next_proto;
    __gpr int len;

    __ASSERT(__is_in_lmem(pkt_hdrs));
    __ASSERT(__is_in_lmem(pkt_enc_hdrs));

    ph = pkt_hdrs;
    peh = pkt_enc_hdrs;


    /* Clear the important parts of the local packet header structure */
    ph->present = 0;
    ph->dirty = 0;
    #ifdef __NFP_TOOL_NFCC
    reg_zero(&ph->offsets, sizeof(ph->offsets));
    #else
    memset((void *)&ph->offsets, 0, sizeof(ph->offsets));
    #endif /* __NFP_TOOL_NFCC  */


    /* Read 1st 16 words of packet buffer from memory into local memory */
    npe_mem_read(buf_addr, src_buf, sizeof(src_buf));


    /* Note we don't bother checking if the header fits for the first
     * few headers as they are guaranteed to be in the first 64 -
     * (PKT_START_OFF + NET_CSUM_PREPEND_LEN) bytes. */

    /* Ethernet */
    ph->present |= NPE_HDR_O_ETH;
    ph->offsets[NPE_HDR_OFF_O_ETH] = buf_off;

    res = he_eth(src_buf, src_off, &ph->o_eth);
    len = HE_RES_LEN_of(res);
    next_proto = HE_RES_PROTO_of(res);

    buf_off += len;
    src_off += len;

    if (next_proto == HE_UNKNOWN) {
        goto out_done;
    }

    /* VLAN */
    if (next_proto == HE_8021Q) {

        /* adjust offsets so will read the
           tpid field into ph->ovlan */
        buf_off -= 2;
        src_off -= 2;

        ph->present |= NPE_HDR_O_VLAN;
        ph->offsets[NPE_HDR_OFF_O_VLAN] = buf_off;

        res = he_vlan(src_buf, src_off, &ph->o_vlan);
        len = HE_RES_LEN_of(res);
        next_proto = HE_RES_PROTO_of(res);

        buf_off += len;
        src_off += len;

        if (next_proto == HE_UNKNOWN)
            goto out_done;
    }

    /* IP */
    if (next_proto == HE_IP4) {
        ph->present |= NPE_HDR_O_IP4;
        ph->offsets[NPE_HDR_OFF_O_L3] = buf_off;

        res = he_ip4(src_buf, src_off, &ph->o_ip4);
        len = HE_RES_LEN_of(res);
        next_proto = HE_RES_PROTO_of(res);

    } else if (next_proto == HE_IP6) {
        if (!he_ip6_fit(sizeof(src_buf), src_off)) {
            /* Reload cache. buf_off is starting offset for the next
             * header and this is likely to be a unaligned read. */
            npe_mem_read((buf_addr + buf_off), src_buf, sizeof(src_buf));
            src_off = 0;
        }
        ph->present |= NPE_HDR_O_IP6;
        ph->offsets[NPE_HDR_OFF_O_L3] = buf_off;

        res = he_ip6(src_buf, src_off, &ph->o_ip6);
        len = HE_RES_LEN_of(res);
        next_proto = HE_RES_PROTO_of(res);
        /* TODO: Handle IPv6 extension headers! */
    }

    buf_off += len;
    src_off += len;

    if (next_proto == HE_UNKNOWN)
        goto out_done;

    /* From here on, we need to check if @pkt_buf still has enough
     * bytes in it for the next header. */
    if (((next_proto == HE_TCP) && !he_tcp_fit(sizeof(src_buf), src_off)) ||
        ((next_proto == HE_UDP) && !he_udp_fit(sizeof(src_buf), src_off)) ||
        ((next_proto == HE_GRE) && !he_gre_fit(sizeof(src_buf), src_off))) {
        /* Reload cache. buf_off is starting offset for the next
         * header and this is likely to be a unaligned read. */
        npe_mem_read((buf_addr + buf_off), src_buf, sizeof(src_buf));
        src_off = 0;
    }

    if (next_proto == HE_TCP) {
        ph->present |= NPE_HDR_O_TCP;
        ph->offsets[NPE_HDR_OFF_O_L4] = buf_off;

        res = he_tcp(src_buf, src_off, &ph->o_tcp);
        len = HE_RES_LEN_of(res);
        next_proto = HE_RES_PROTO_of(res);

    } else if (next_proto == HE_UDP) {
        ph->present |= NPE_HDR_O_UDP;
        ph->offsets[NPE_HDR_OFF_O_L4] = buf_off;

        res = he_udp(src_buf, src_off, &ph->o_udp, NET_VXLAN_PORT);
        len = HE_RES_LEN_of(res);

        if (next_proto == HE_VXLAN) {
            buf_off += len;
            src_off += len;

            if (!he_vxlan_fit(sizeof(src_buf), src_off)) {
                /* Reload cache. buf_off is starting offset for the next
                 * header and this is likely to be a unaligned read. */
                npe_mem_read((buf_addr + buf_off), src_buf, sizeof(src_buf));
                src_off = 0;
            }

            ph->present |= NPE_HDR_E_VXLAN;
            ph->offsets[NPE_HDR_OFF_ENCAP] = buf_off;

            res = he_vxlan(src_buf, src_off, &peh->vxlan);
            len = HE_RES_LEN_of(res);
            next_proto = HE_RES_PROTO_of(res);

            goto process_inner;
        } else
            /* if MPLS port number is found process the MPLS header */
            if (ph->o_udp.dport == NET_MPLS_PORT) {
                buf_off += len;
                src_off += len;
                ph->present |= NPE_HDR_MPLS;
                ph->offsets[NPE_HDR_OFF_MPLS] = buf_off;
                res = he_mpls(src_buf, src_off, &ph->mpls);
                len = HE_RES_LEN_of(res);
                next_proto = HE_RES_PROTO_of(res);
                /* JAH JAH May want to do lookup using label to go to correct
                 * next protocol checking logic.
                 */
                goto process_inner;
            }
    } else if (next_proto == HE_GRE) {
        ph->offsets[NPE_HDR_OFF_ENCAP] = buf_off;

        res = he_gre(src_buf, src_off, &peh->gre);
        len = HE_RES_LEN_of(res);
        next_proto = HE_RES_PROTO_of(res);

        /* if GRE indicates that next protocol is MPLS */
        if (next_proto == HE_MPLS) {
            buf_off += len;
            src_off += len;
            ph->present |= NPE_HDR_MPLS;
            ph->offsets[NPE_HDR_OFF_MPLS] = buf_off;
            res = he_mpls(src_buf, src_off, &ph->mpls);
            len = HE_RES_LEN_of(res);
            next_proto = HE_RES_PROTO_of(res);
            goto process_inner;
        }

        /* only support NVGRE */
        if ((next_proto != HE_ETHER) || !(NET_GRE_IS_NVGRE(peh->gre.flags)))
            goto out_done;

        ph->present |= NPE_HDR_E_NVGRE;

        he_gre_nvgre(src_buf, src_off, &peh->nvgre);

        /* note: len was returned by he_gre(), not he_gre_nvgre()*/
    process_inner:
        buf_off += len;
        src_off += len;
        npe_mem_read((buf_addr + buf_off), src_buf, sizeof(src_buf));
        src_off = 0;

        /* if we had MPLS that produced a HE_NONE. Determine next protocol. */
        if (next_proto == HE_NONE) {
            /* if ipv4 */
            if ((src_buf[src_off] & 0xF0000000) == 0x40000000) {
                next_proto = HE_IP4;
                goto process_inner_ip;
            /* else if ipv6 */
            } else if ((src_buf[src_off] & 0xF0000000) == 0x60000000) {
                /* else if ipv6 */
                next_proto = HE_IP6;
                goto process_inner_ip;
            }
            /* else let it fall to ethernet */
        }

        /* Start processing inner */
        if (!he_eth_fit(sizeof(src_buf), src_off)) {
            /* Reload cache. buf_off is starting offset for the next
             * header and this is likely to be a unaligned read. */
            npe_mem_read((buf_addr + buf_off), src_buf, sizeof(src_buf));
            src_off = 0;
        }

        ph->present |= NPE_HDR_I_ETH;
        ph->offsets[NPE_HDR_OFF_I_ETH] = buf_off;

        res = he_eth(src_buf, src_off, &ph->i_eth);
        len = HE_RES_LEN_of(res);
        next_proto = HE_RES_PROTO_of(res);

        buf_off += len;
        src_off += len;

        if (next_proto == HE_UNKNOWN)
            goto out_done;

        /* inner VLAN */
        if (next_proto == HE_8021Q) {
            ph->present |= NPE_HDR_I_VLAN;
            ph->offsets[NPE_HDR_OFF_I_VLAN] = buf_off;

            res = he_vlan(src_buf, src_off, &ph->i_vlan);
            len = HE_RES_LEN_of(res);
            next_proto = HE_RES_PROTO_of(res);

            buf_off += len;
            src_off += len;

            if (next_proto == HE_UNKNOWN)
                goto out_done;
        }

        /* read inner IP so that it is aligned in xfer regs */
        npe_mem_read((buf_addr + buf_off), src_buf, sizeof(src_buf));
        src_off = 0;

    process_inner_ip:
        /* inner IP */
        if (next_proto == HE_IP4) {
            ph->present |= NPE_HDR_I_IP4;
            ph->offsets[NPE_HDR_OFF_I_L3] = buf_off;

            res = he_ip4(src_buf, src_off, &ph->i_ip4);
            len = HE_RES_LEN_of(res);
            next_proto = HE_RES_PROTO_of(res);

        } else if (next_proto == HE_IP6) {
            ph->present |= NPE_HDR_I_IP6;
            ph->offsets[NPE_HDR_OFF_I_L3] = buf_off;

            res = he_ip6(src_buf, src_off, &ph->i_ip6);
            len = HE_RES_LEN_of(res);
            next_proto = HE_RES_PROTO_of(res);
            /* TODO: Handle IPv6 extension headers! */
        }

        buf_off += len;
        src_off += len;

        if (next_proto == HE_UNKNOWN)
            goto out_done;

        if (((next_proto == HE_TCP) && !he_tcp_fit(sizeof(src_buf), src_off)) ||
            ((next_proto == HE_UDP) && !he_udp_fit(sizeof(src_buf), src_off))) {
            /* Reload cache. buf_off is starting offset for the next
             * header and this is likely to be a unaligned read. */
            npe_mem_read((buf_addr + buf_off), src_buf, sizeof(src_buf));
            src_off = 0;
        }

        if (next_proto == HE_TCP) {
            ph->present |= NPE_HDR_I_TCP;
            ph->offsets[NPE_HDR_OFF_I_L4] = buf_off;

            res = he_tcp(src_buf, src_off, &ph->i_tcp);
            len = HE_RES_LEN_of(res);
            next_proto = HE_RES_PROTO_of(res);

        } else if (next_proto == HE_UDP) {
            ph->present |= NPE_HDR_I_UDP;
            ph->offsets[NPE_HDR_OFF_I_L4] = buf_off;

            res = he_udp(src_buf, src_off, &ph->i_udp, 0);
            len = HE_RES_LEN_of(res);
            next_proto = HE_RES_PROTO_of(res);
        }

    }

    buf_off += len;
    src_off += len;


out_done:
    /* Stash away where the payload starts */
    ph->payload_off = buf_off;
}
