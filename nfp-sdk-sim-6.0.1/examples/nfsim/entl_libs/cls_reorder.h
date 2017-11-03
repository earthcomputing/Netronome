/**
 * Copyright (C) 2017,  Earth Computing Inc.  All rights reserved.
 *
 *
 * @file          cls_reorder.h
 * @brief         CLS Reorder code 
 *
 * Author:        Atsushi Kasuya
 *
 */


#ifndef _CLS_REORDER_H_
#define _CLS_REORDER_H_

#define REORDER_RING_ADDR 0
#define REORDER_RING_NUM0 0
#define REORDER_RING_NUM1 1
#define REORDER_RING_SIZE_LW 64
#define REORDER_RING_SEQ_MASK 0x3f 

// Reorder ring is selected by incomming port number & 1 
// as two ports are assigned to an island.
//  Need to redesign this scheme when we use all island to receive packets from any ports. 
//   At this point, just use fixed island per port for a simple implementation. 
//
#ifdef REORDER_RING_EXPORT

__export __shared __cls __align(REORDER_RING_SIZE_LW*4) char reorder_ring_mem0[REORDER_RING_SIZE_LW*4] ;
__export __shared __cls __align(REORDER_RING_SIZE_LW*4) char reorder_ring_mem1[REORDER_RING_SIZE_LW*4] ;

#endif

#ifdef REORDER_RING_IMPORT

 __import __shared __cls __align(REORDER_RING_SIZE_LW*4) char reorder_ring_mem0[REORDER_RING_SIZE_LW*4] ;
 __import __shared __cls __align(REORDER_RING_SIZE_LW*4) char reorder_ring_mem1[REORDER_RING_SIZE_LW*4] ;

#endif

//  nfp_cls_ring code copied here (original doesn't compile)

__intrinsic
void cls_ring_ordered_lock_ptr32(
    unsigned int ring_number,
    unsigned int sequence_number,
    sync_t sync,
    SIGNAL *sig_ptr
) ;

 __intrinsic
void cls_ring_ordered_lock_ptr40(
    unsigned int ring_number,
    unsigned int sequence_number,
    unsigned int island_id,
    sync_t sync,
    SIGNAL *sig_ptr
) ;

__intrinsic
void cls_ring_ordered_unlock_ptr32(
    unsigned int ring_number,
    unsigned int sequence_number
) ;

__intrinsic
void cls_ring_ordered_unlock_ptr40(
    unsigned int ring_number,
    unsigned int sequence_number,
    unsigned int island_id
) ;

void reorder_ring_init() ;

void reorder_lock( unsigned int ring_num, unsigned int sequence ) ;

void reorder_unlock( unsigned int ring_num, unsigned int sequence ) ;

uint32_t get_ring_num( unsigned int port ) ;

#endif
