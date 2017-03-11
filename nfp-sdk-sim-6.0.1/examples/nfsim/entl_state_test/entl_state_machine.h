/*
 * Copyright (C) 2017,  Earth Computing Inc.  All rights reserved.
 *
 *
 * @file          entl_state_machine.h
 * @brief         ENTL state machine 
 *
 * Author:        Atsushi Kasuya
 *
 */

#ifndef _ENTL_STATE_MACHINE_H_
#define _ENTL_STATE_MACHINE_H_


#ifdef NETRONOME_HOST

//typedef unsigned int            uint32_t;
//typedef unsigned long long int  uint64_t;

#define __lmem 

#define ENTL_DEBUG(fmt, args...) printf( fmt, ## args )

#else

#include <nfp.h>

//#define ENTL_DEBUG(fmt, args...) 

#endif  // NETRONOME_HOST
#include <stdint.h>

// State definition
#define ENTL_STATE_IDLE     0
#define ENTL_STATE_HELLO    1
#define ENTL_STATE_WAIT     2
#define ENTL_STATE_SEND     3
#define ENTL_STATE_RECEIVE  4
#define ENTL_STATE_AM       5
#define ENTL_STATE_BM       6
#define ENTL_STATE_AH       7
#define ENTL_STATE_BH       8
#define ENTL_STATE_ERROR    9

// return value flag for entl_received
#define ENTL_ACTION_NOP         0
#define ENTL_ACTION_SEND        0x01
#define ENTL_ACTION_SEND_AIT    0x02
#define ENTL_ACTION_PROC_AIT    0x04
#define ENTL_ACTION_SIG_AIT     0x08
#define ENTL_ACTION_SEND_DAT    0x10
#define ENTL_ACTION_SIG_ERR     0x20
#define ENTL_ACTION_SET_ADDR    0x40
#define ENTL_ACTION_DROP_AIT    0x80
#define ENTL_ACTION_ERROR       -1

#define ETH_P_ECLP  0xEAC0    /* Earth Computing Link Protocol [ NOT AN OFFICIALLY REGISTERED ID ] */
#define ETH_P_ECLD  0xEAC1    /* Earth Computing Link Discovery [ NOT AN OFFICIALLY REGISTERED ID ] */
#define ETH_P_ECLL  0xEAC2    /* Earth Computing Link Local Data  [ NOT AN OFFICIALLY REGISTERED ID ] */

// Error type bits
#define ENTL_ERROR_FLAG_SEQUENCE 0x0001
#define ENTL_ERROR_FLAG_LINKDONW 0x0002
#define ENTL_ERROR_FLAG_TIMEOUT  0x0004
#define ENTL_ERROR_SAME_ADDRESS	0x0008
#define ENTL_ERROR_UNKOWN_CMD	  0x0010
#define ENTL_ERROR_UNKOWN_STATE	0x0020
#define ENTL_ERROR_UNEXPECTED_LU 0x0040
#define ENTL_ERROR_FATAL         0x8000

#define ENTL_COUNT_MAX  10
 
// Message sent on the MAC dst addr
#define ENTL_MESSAGE_HELLO  0x000000000000
#define ENTL_MESSAGE_EVENT  0x000100000000
#define ENTL_MESSAGE_NOP    0x000200000000
#define ENTL_MESSAGE_AIT    0x000300000000
#define ENTL_MESSAGE_ACK    0x000400000000

#define ENTL_MESSAGE_ONLY	  0x800000000000
#define ENTL_TEST_MASK        0x7f0000000000
#define ENTL_MESSAGE_MASK     0x00ff00000000
#define ENTL_COUNTER_MASK     0x0000ffffffff

#define ENTL_DEVICE_NAME_LEN 15

#define MAX_ENTT_QUEUE_SIZE 32

#ifndef u32
#define u32 uint32_t
#endif
 
// The data structre represents the internal state of ENTL
typedef struct entl_state {
  uint32_t event_i_know ;			// last received event number 
  uint32_t event_i_sent ;			// last event number sent
  uint32_t event_send_next ;			// next event number sent
  uint32_t current_state ;			// 0: idle  1: H 2: W 3:S 4:R
} entl_state_t ;

typedef volatile struct entl_state_machine {
  entl_state_t state ;
  uint32_t error_flag ;				// first error flag 
  uint32_t p_error_flag ;			// when more than 1 error is detected, those error bits or ored to this flag
  uint32_t error_count ;				// Count multiple error, cleared in got_error_state()
  uint32_t state_count ;
  uint64_t my_addr ;
} entl_state_machine_t ;

void entl_state_init( __lmem entl_state_machine_t *mcn ) ;
void entl_set_random_addr( __lmem entl_state_machine_t *mcn ) ;

// On Received message, this should be called with the massage (MAC source & destination addr)
//   return value : bit 0: send, bit 1: send AIT, bit 2: process AIT
int entl_received( __lmem entl_state_machine_t *mcn, uint64_t s_addr, uint64_t d_addr, uint32_t ait_queue, uint32_t ait_receive ) ;
// On sending ethernet packet, this function should be called to get the destination MAC address for message
//   return value : bit 0: send, bit 1: send AIT, bit 2: process AIT
int entl_next_send( __lmem entl_state_machine_t *mcn, uint64_t *addr, uint32_t ait_queue ) ; 

// On receiving error (link down, timeout), this functon should be called to report to the state machine
void entl_state_error( __lmem entl_state_machine_t *mcn, uint32_t error_flag ) ;




#endif /* _ENTL_STATE_MACHINE_H_ */

/* -*-  Mode:C; c-basic-offset:4; tab-width:4 -*- */
