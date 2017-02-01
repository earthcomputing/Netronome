/*
 * Copyright (C) 2016 Netronome, Inc. All rights reserved.
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
 * @file          include/npe_hdr_extract.h
 * @brief         Packet Header extract operations
 */

#ifndef _NPE_HDR_EXTRACT_H_
#define _NPE_HDR_EXTRACT_H_


/* Compilation for Netronome NFP */
#if defined(__NFP_TOOL_NFCC)
    #ifndef NFP_LIB_ANY_NFCC_VERSION
        #if (__NFP_TOOL_NFCC < NFP_SW_VERSION(5, 0, 0, 0)) ||   \
            (__NFP_TOOL_NFCC > NFP_SW_VERSION(6, 255, 255, 255))
            #error "This software is not supported for the version of the SDK currently in use."
         #endif
    #endif

#include <nfp.h>
#include <nfp_chipres.h>
#endif

#include <stdint.h>

#include <npe_eth.h>
#include <npe_gre.h>
#include <npe_ip.h>
#include <npe_mpls.h>
#include <npe_tcp.h>
#include <npe_udp.h>
#include <npe_vxlan.h>

#include <npe_types.h>


/** @file npe_hdr_extract.h
 * @addtogroup hdrext Packet Header Extract Operations
 * @{
 */

/** @name Packet Header Extract related functions
 *  @{
 */



/*
 * Header extract definitions
 */

/* Enum to indicate which fields are present in
   returned packet header data struct */
enum npe_pkt_hdr_present {
    NPE_HDR_O_ETH      = (1 << 0),
    NPE_HDR_O_VLAN     = (1 << 1),
    NPE_HDR_O_IP4      = (1 << 2),
    NPE_HDR_O_IP6      = (1 << 3),
    NPE_HDR_O_IP6_EXT  = (1 << 4),
    NPE_HDR_O_TCP      = (1 << 5),
    NPE_HDR_O_UDP      = (1 << 6),
    NPE_HDR_E_NVGRE    = (1 << 7),
    NPE_HDR_E_VXLAN    = (1 << 8),
    NPE_HDR_I_ETH      = (1 << 9),
    NPE_HDR_I_VLAN     = (1 << 10),
    NPE_HDR_I_IP4      = (1 << 11),
    NPE_HDR_I_IP6      = (1 << 12),
    NPE_HDR_I_IP6_EXT  = (1 << 13),
    NPE_HDR_I_TCP      = (1 << 14),
    NPE_HDR_I_UDP      = (1 << 15),
    NPE_HDR_MPLS       = (1 << 16),
    NPE_HDR_CMSG       = (1 << 17),
};

/* Enum to indicate which offset in the offsets array
   to use for which header */
enum npe_pkt_hdr_offs_idx {
    NPE_HDR_OFF_O_ETH = 0,
    NPE_HDR_OFF_O_VLAN,
    NPE_HDR_OFF_O_L3,
    NPE_HDR_OFF_O_L4,
    NPE_HDR_OFF_ENCAP,
    NPE_HDR_OFF_I_ETH,
    NPE_HDR_OFF_I_VLAN,
    NPE_HDR_OFF_I_L3,
    NPE_HDR_OFF_I_L4,
    NPE_HDR_OFF_MPLS,
};


/*
 * Structure to contain packet headers.  Provides copy of standard
 * headers in a packet as well as the offsets where they are located
 * in the packet buffer in memory. A bitmask is maintained to
 * indicate which headers are valid. A second bitmask is used to
 * track which packet headers have been modified and must be written
 * back to memory.
 */
struct npe_pkt_hdrs {
    uint32_t present;           /* Bitmap of present headers */
    uint32_t dirty;             /* Bitmap of dirty headers */

    uint32_t offsets[10];       /* Offset in packet buffer for headers */

    struct eth_hdr o_eth;       /* (Outer) Ethernet header */

    struct mpls_hdr mpls;       /* Holds the last popped MPLS header */

    uint16_t payload_off;       /* Offset into packet buffer (in memory)
                                 * where the payload of the packet starts */

    struct vlan_hdr o_vlan;     /* (Outer) VLAN header */

    union {
        struct ip4_hdr o_ip4;   /* (Outer) IPv4 header, or */
        struct ip6_hdr o_ip6;   /* (Outer) IPv6 header or */
    };

    union {
        struct tcp_hdr o_tcp;   /* (Outer) TCP header, or */
        struct udp_hdr o_udp;   /* (Outer) UDP header */
    };

    struct eth_hdr i_eth;       /* (Inner) Ethernet header */

    struct vlan_hdr i_vlan;     /* (Inner) VLAN header */

    union {
        struct ip4_hdr i_ip4;   /* (Inner) IPv4 header, or */
        struct ip6_hdr i_ip6;   /* (Inner) IPv6 header */
    };

    union {
        struct tcp_hdr i_tcp;   /* (Inner) TCP header, or */
        struct udp_hdr i_udp;   /* (Inner) UDP header */
    };
};


/*
 * Structure to return packet encapsulation headers. Provides copy of
 * standard encapsulation headers, when they are present
 */
struct npe_pkt_enc_hdrs {
    union {
        struct {
            struct gre_hdr gre;
            struct nvgre_ext_hdr nvgre;
        };
        struct vxlan_hdr vxlan;
    };
};


/**
 * Extract headers from a network packet located in packet buffer memory.
 * Store headers and related control info in a local packet header
 * structure. For nfp, the local packet header structures are stored in
 * the microengine local memory.
 *
 * @param buf_addr    Address of packet data in packet buffer memory
 * @param buf_off     Offset from buf_addr to start of packet data
 * @param pkt_hdr     Struct in which to return packet headers result
 * @param pkt_enc_hdr Struct in which to return packet encapsulation header
 */
__NPE_INTRINSIC void npe_hdr_extract(npe_mem_ptr_t buf_addr,
                                     uint32_t buf_off,
                                     void *pkt_hdrs,
                                     void *pkt_enc_hdrs);

/** @}
 * @}
 */

#endif /* _NPE_PKT_HDR_EXTRACT_H_ */
