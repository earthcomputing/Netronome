/*
 * Copyright (C) 2017,  Earth Computing Inc.  All rights reserved.
 *
 *
 * @file          entl_state.h
 * @brief         ENTL state machine 
 *
 * Author:        Atsushi Kasuya
 *
 */

#ifndef _ENTL_STATE_MACHINE_H_
#define _ENTL_STATE_MACHINE_H_

#include <nfp.h>
#include <stdint.h>

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
#define ENTL_MESSAGE_HELLO_U  0x0000
#define ENTL_MESSAGE_HELLO_L  0x0000
#define ENTL_MESSAGE_EVENT_U  0x0001
#define ENTL_MESSAGE_NOP_U    0x0002
#define ENTL_MESSAGE_AIT_U    0x0003
#define ENTL_MESSAGE_ACK_U    0x0004

// When MSB of upper address is set, this is message only, no packet to upper layer
#define ENTL_MESSAGE_ONLY_U	 0x8000
#define ENTL_TEST_MASK        0x7f00
#define ENTL_MESSAGE_MASK     0x00ff

#define ENTL_DEVICE_NAME_LEN 15

#define MAX_ENTT_QUEUE_SIZE 32

#ifndef u32
#define u32 unsigned int
#endif
 
/ The data structre represents the internal state of ENTL
struct entl_state {
  u32 event_i_know ;			// last received event number 
  u32 event_i_sent ;			// last event number sent
  u32 event_send_next ;			// next event number sent
  u32 current_state ;			// 0: idle  1: H 2: W 3:S 4:R
  u32 error_flag ;				// first error flag 
  u32 p_error_flag ;			// when more than 1 error is detected, those error bits or ored to this flag
  u32 error_count ;				// Count multiple error, cleared in got_error_state()
} ;

#endif /* _ENTL_STATE_MACHINE_H_ */

/* -*-  Mode:C; c-basic-offset:4; tab-width:4 -*- */
