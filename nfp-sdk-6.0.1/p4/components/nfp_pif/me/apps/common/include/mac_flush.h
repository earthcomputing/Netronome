/* Copyright (C) 2016,  Netronome Systems, Inc.  All rights reserved. */

#ifndef __MAC_FLUSH_H__
#define __MAC_FLUSH_H__

#include <assert.h>
#include <nfp.h>
#include <stdint.h>

#define CDP_PLATFORM    1
#define SF_PLATFORM     2
#define FWA_PLATFORM    3
#define H_PLATFORM      4
#ifndef PLATFORM_PORT_SELECTION
    #warning "Platform was not defined. Defaulting to CDP platform"
    #define PLATFORM_PORT_SELECTION CDP_PLATFORM
#endif

/* A bitmap of ports which are paused. 
 * Bits 0-23 correspond to ports 0-23 on NBI0.
 * Bits 24-47 correspond to ports 0-23 on NBI1. 
 * Store this bitmap in the upper 8 bytes of CLS on every worker island
 * (this file should be included in every worker island via pkt_ops.c).*/

#define MAC_PORT_PAUSED_BITMAP_ADDR 0xfff8
__asm {
    .alloc_mem MAC_PORT_PAUSED_BITMAP cls+MAC_PORT_PAUSED_BITMAP_ADDR island 0x8 addr40
    .init MAC_PORT_PAUSED_BITMAP 0 0
}

#endif /* __MAC_FLUSH_H__ */
