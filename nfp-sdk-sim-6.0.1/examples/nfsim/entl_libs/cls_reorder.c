/**
 * Copyright (C) 2017,  Earth Computing Inc.  All rights reserved.
 *
 *
 * @file          cls_reorder.c
 * @brief         CLS Reorder code 
 *
 * Author:        Atsushi Kasuya
 *
 */

#include <nfp.h>
#include <me.h>
#include <mutexlv.h>
#include <stdint.h>
#include <pkt/pkt.h>
#include <net/eth.h>
#include <nfp/mem_bulk.h>
#include <vnic/utils/cls_ring.h>

#include "my_cls_ring.h"

#define REORDER_RING_IMPORT
#include "cls_reorder.h"


void reorder_ring_init()
{
	cls_ring_setup(REORDER_RING_NUM0, (__cls void *)reorder_ring_mem0, (REORDER_RING_SIZE_LW * 4));
	cls_ring_setup(REORDER_RING_NUM1, (__cls void *)reorder_ring_mem1, (REORDER_RING_SIZE_LW * 4));
}


void reorder_lock( unsigned int ring_num, unsigned int sequence ) {
	SIGNAL sig;
	cls_ring_ordered_lock_ptr32( ring_num, sequence, ctx_swap, &sig ) ;
}

void reorder_unlock( unsigned int ring_num, unsigned int sequence ) {
    cls_ring_ordered_unlock_ptr32(ring_num, sequence ) ;
}

// Optimized for 8 port, last Island supports two ports
uint32_t get_ring_num( unsigned int port ) {
    if( port == 7 ) return REORDER_RING_NUM1 ;
    else return REORDER_RING_NUM0 ;
}
