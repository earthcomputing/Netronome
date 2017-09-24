/**
 * Copyright (C) 2017,  Earth Computing Inc.  All rights reserved.
 *
 *
 * @file          egress_queue.c
 * @brief         Egress Queue Access code 
 *
 * Author:        Atsushi Kasuya
 *
 */

#include "nfp_cls.h"

#define PACKET_RING_IMPORT

#include "egress_queue.h"

void egress_queue_init() {
    cls_ring_setup(PACKET_RING_NUM_0,
                   (__cls void *)packet_ring_mem0,
                   (PACKET_RING_SIZE_LW*sizeof(packet_data_t)));
    cls_ring_setup(PACKET_RING_NUM_1,
                   (__cls void *)packet_ring_mem0,
                   (PACKET_RING_SIZE_LW*sizeof(packet_data_t)));
}


/** Add a thread to a Cluster Local Scratch memory ring (queue)
 *
 * @param data          Work received from ring
 * @param ring_number   Ring number to read data from (0 to 15)
 * @param count         Number of 32-bit words to read (1 to 16)
 * @param island_id     Island id indicating CLS locality. See 6xxx databook for Local Scratch
 *                       recommended addressing mode.
 * @param sync          Type of synchronization to use (sig_done or ctx_swap)
 * @param sig_ptr       Signal to raise upon completion
 *
 * Add a thread to the work queue. Adding a thread to a queue that contains
 * work will get the first work and deliver it to the thread. If there are no
 * work on the queue, the thread is added to the ring.
 *
 */

void get_packet_data( __xread packet_data_t *pkt_in, uint32_t ring ) 
{
	SIGNAL sig;

	cls_ring_workq_add_thread_ptr40( pkt_in, ring, 4, __ISLAND, ctx_swap, &sig ) ;

}

void put_packet_data( __xwrite packet_data_t *pkt_out, uint32_t island_ring  ) 
{
    SIGNAL sig;

    cls_ring_workq_add_work_ptr40( pkt_out, island_ring & 0xffff, 4, island_ring >> 16, ctx_swap, &sig ) ;

}



uint32_t get_island_ring( uint32_t port ) {
	switch(port) {
		case 0:		return 32 << 16 | PACKET_RING_NUM_0 ;
		case 1:		return 33 << 16 | PACKET_RING_NUM_0 ;
		case 2:		return 34 << 16 | PACKET_RING_NUM_0 ;
		case 3:		return 35 << 16 | PACKET_RING_NUM_0 ;
		case 4:		return 36 << 16 | PACKET_RING_NUM_0 ;
		case 5:		return 37 << 16 | PACKET_RING_NUM_0 ;
		case 6:		return 38 << 16 | PACKET_RING_NUM_0 ;
		case 7:		return 38 << 16 | PACKET_RING_NUM_1 ;  // Optimized for 8 port 
//		case 8:		return 37 << 16 | PACKET_RING_NUM_1 ;
//		case 9:		return 36 << 16 | PACKET_RING_NUM_1 ;
//		case 10:	return 35 << 16 | PACKET_RING_NUM_1 ;
//		case 11:	return 34 << 16 | PACKET_RING_NUM_1 ;  // netronome can handle up to 12 ports
		default:	return 0xffffffff ;
	}
}

uint32_t get_nbi( uint32_t port ) {
	switch(port) {
		case 0:		return 0 ;
		case 1:		return 1 ;
		case 2:		return 0 ;
		case 3:		return 1 ;
		case 4:		return 0 ;
		case 5:		return 1 ;
		case 6:		return 0 ;
		case 7:		return 1 ;  // reality, up to 8 ports can be supported on current hw
//		case 8:		return 0 ;
//		case 9:		return 1 ;
//		case 10:	return 0 ;
//		case 11:	return 1 ;  // netronome can handle up to 12 ports
		default:	return 0xffffffff ;
	}
}

uint32_t get_sequencer( uint32_t in_port ) 
{
	switch(in_port) {
		case 0:		return 1 ;
		case 1:		return 2 ;
		case 2:		return 3 ;
		case 3:		return 4 ;
		case 4:		return 5 ;
		case 5:		return 6 ;
		case 6:		return 7 ;
		case 7:		return 8 ;  // reality, up to 8 ports can be supported on current hw
//		case 8:		return 0 ;
//		case 9:		return 1 ;
//		case 10:	return 0 ;
//		case 11:	return 1 ;  // netronome can handle up to 12 ports
		default:	return 0xffffffff ;
	}
}

uint32_t get_tm_queue( uint32_t port ) 
{
	switch(port) {
		case 0:		return 0 ;
		case 1:		return 0 ;
		case 2:		return 1 ;
		case 3:		return 1 ;
		case 4:		return 2 ;
		case 5:		return 2 ;
		case 6:		return 3 ;
		case 7:		return 3 ;  // reality, up to 8 ports can be supported on current hw
//		case 8:		return 0 ;
//		case 9:		return 1 ;
//		case 10:	return 0 ;
//		case 11:	return 1 ;  // netronome can handle up to 12 ports
		default:	return 0xffffffff ;
	}	
}

#ifdef PACKET_RING_EXPORT

void init_sequence_num() {
	int i ;
    SIGNAL sig;
	__xwrite uint32_t data_w ;
	data_w = 0 ;
	for( i = 0 ; i < SEQUENCE_NUM_ARRAY_SIZE ; i++ ) {
		cls_write_ptr32(
    		&data_w,
    		&sequence_num_array0[i],
    		sizeof(uint32_t),
    		ctx_swap,
    		&sig 
		);
		cls_write_ptr32(
    		&data_w,
    		&sequence_num_array1[i],
    		sizeof(uint32_t),
    		ctx_swap,
    		&sig 
		);
	}
}

#endif



uint32_t get_sequence_num( uint32_t port ) {
	__xread uint32_t data_r ;
	__xwrite uint32_t data_w ;
	uint32_t seq_num ;
	SIGNAL sig;
	// This function does not use any mutex protection. 
	// This should be called within the mutexed resion provided with reorder mechanism

	uint32_t offset = port >> 1 ;

	if( port & 1 ) {
		cls_read_ptr32(
	    	&data_r,
	    	&sequence_num_array0[offset],
	    	sizeof(uint32_t),
	    	ctx_swap,
	    	&sig 
		);
		seq_num = (uint32_t)*data_r ;
		data_w = seq_num + 1 ;
		cls_write_ptr32(
	    	&data_w,
	    	&sequence_num_array0[offset],
	    	sizeof(uint32_t),
	    	ctx_swap,
	    	&sig 
		);
	}
	else {
		cls_read_ptr32(
	    	&data_r,
	    	&sequence_num_array1[offset],
	    	sizeof(uint32_t),
	    	ctx_swap,
	    	&sig 
		);
		seq_num = (uint32_t)*data_r ;
		data_w = seq_num + 1 ;
		cls_write_ptr32(
	    	&data_w,
	    	&sequence_num_array1[offset],
	    	sizeof(uint32_t),
	    	ctx_swap,
	    	&sig 
		);
	}



	return seq_num ;

}
