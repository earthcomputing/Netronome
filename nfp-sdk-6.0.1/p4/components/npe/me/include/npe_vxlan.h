/*
 * Copyright (C) 2016,  Netronome Systems, Inc.  All rights reserved.
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
 * @file          npe_vxlan.h
 * @brief         Definitions for VXLAN header parsing
 *
 */

#ifndef _NPE_VXLAN_H_
#define _NPE_VXLAN_H_

/* default VXLAN port in the inner UDP header as per RFC 7348 */
#define NET_VXLAN_PORT             4789

#ifdef __NFP_TOOL_NFCC
#include <nfp.h>
#define __PACKED __packed
#else
#define __PACKED __attribute__((packed))
#endif
#include <stdint.h>

/**
 * VXLAN header structure
 */
__PACKED struct vxlan_hdr {
    unsigned int reserved1:4;
    unsigned int i:1;              /* Indicates valid VNI */
    unsigned int reserved2:3;
    unsigned int reserved3:24;

    unsigned int vni:24;           /* VXLAN Network Identifier */
    unsigned int reserved4:8;
};

#endif /* _NPE_VXLAN_H_ */
