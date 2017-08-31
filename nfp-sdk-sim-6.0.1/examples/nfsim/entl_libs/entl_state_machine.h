/**
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

#ifdef __cplusplus 
extern "C" {
#endif

#ifdef NETRONOME_HOST

//typedef unsigned int            uint32_t;
//typedef unsigned long long int  uint64_t;

#define ENTL_DEBUG(fmt, args...) if(debug_flag) printf( fmt, ## args )

#else

#include <nfp.h>

// #define ENTL_DEBUG(fmt, args...) 

#endif  // NETRONOME_HOST
#include <stdint.h>

#include "atomic_link_op.h"

// State definition
#define ENTL_STATE_IDLE     0
#define ENTL_STATE_HELLO    1
#define ENTL_STATE_WAIT     2
#define ENTL_STATE_SEND     3
#define ENTL_STATE_RECEIVE  4
#define ENTL_STATE_RA       5
#define ENTL_STATE_SA       6
#define ENTL_STATE_SB       7
#define ENTL_STATE_RB       8
#define ENTL_STATE_RAL      9
#define ENTL_STATE_SAL      10
#define ENTL_STATE_SBL      11
#define ENTL_STATE_RBL      12
#define ENTL_STATE_ERROR    13

// return value flag for entl_received
#define ENTL_ACTION_NOP             0
#define ENTL_ACTION_SEND            0x0001
#define ENTL_ACTION_SEND_AIT        0x0002
#define ENTL_ACTION_PROC_AIT        0x0004
#define ENTL_ACTION_SIG_AIT         0x0008
#define ENTL_ACTION_SEND_DAT        0x0010
#define ENTL_ACTION_SIG_ERR         0x0020
#define ENTL_ACTION_SET_ADDR        0x0040
#define ENTL_ACTION_DROP_AIT        0x0080
#define ENTL_ACTION_SEND_ALO        0x0100
#define ENTL_ACTION_SEND_ALO_T      0x0200
#define ENTL_ACTION_SEND_ALO_F      0x0400
#define ENTL_ACTION_PROPAGATE_AIT   0x0800
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
// MAC Destination Field Extraction Masks
#define ECLP_FW_MASK        (uint64_t)0x800000000000
#define ECLP_ETYPE_MASK     (uint64_t)0x001f00000000
#define ECLP_OP_ST_MASK     (uint64_t)0x0000ffff0000
#define ECLP_VALUE_MASK     (uint64_t)0x00000000ffff

#define ECLP_VALUE_NEXT(x)  ((x + 1)&0xffff)
#define ECLP_VALUE_PLUS2(x)  ((x + 2)&0xffff)


// MAC Source Field Extraction Masks
#define ECLP_D_MASK         (uint64_t)0x800000000000
#define ECLP_CREDIT_MASK    (uint64_t)0x7fff00000000
#define ECLP_NLKUP_MASK     (uint64_t)0x0000ffffffff

#define GET_ECLP_ETYPE(x)     (x&ECLP_ETYPE_MASK)
#define GET_ECLP_VALUE(x)     (x&ECLP_VALUE_MASK)
#define GET_ECLP_DIRECTION(x) ((x&ECLP_D_MASK)>>47)
#define GET_ECLP_CREDIT(x)    ((x&ECLP_CREDIT_MASK)>>32)
#define GET_ECLP_NLKUP(x)     (x&ECLP_NLKUP_MASK)

// Message sent on the MAC dst addr
#define ENTL_ETYPE_NOP    (uint64_t)0x000000000000
#define ENTL_ETYPE_HELLO  (uint64_t)0x000100000000
#define ENTL_ETYPE_ENTL   (uint64_t)0x000200000000
#define ENTL_ETYPE_AITS   (uint64_t)0x000300000000
#define ENTL_ETYPE_AITR   (uint64_t)0x000400000000
#define ENTL_ETYPE_AITA   (uint64_t)0x000500000000

#define ENTL_DEVICE_NAME_LEN 15

#define MAX_ENTT_QUEUE_SIZE 32

// ALO command decoding
#define ALO_COMMAND_OPCODE(x)   ((x)&0xffff)
#define ALO_COMMAND_DT(x)       ((x >> 16) & 0x1f)
#define ALO_COMMAND_SC(x)       ((x >> 21) & 0x1f)
#define ALO_COMMAND_FW(x)       ( x & 0x80000000 )

#define SET_ECLP_ALO(x)         ( (((uint64_t)ALO_COMMAND_SC(x))<<42) | (((uint64_t)ALO_COMMAND_DT(x))<<37) | (((uint64_t)ALO_COMMAND_OPCODE(x)) << 16) )

#ifndef u32
#define u32 uint32_t
#endif
 
// The data structre represents the internal state of ENTL
typedef struct entl_state {
  uint32_t current_state ;      // 0: idle  1: H 2: W 3:S 4:R
  uint32_t event_i_know ;			// last received event number 
  uint32_t event_i_sent ;			// last event number sent
  uint32_t event_send_next ;			// next event number sent
} entl_state_t ;

typedef volatile struct entl_state_machine {
  entl_state_t state ;
  alo_regs_t ao ;
  entl_state_t error_state ;
  uint32_t error_flag ;				// first error flag 
  uint32_t p_error_flag ;			// when more than 1 error is detected, those error bits or ored to this flag
  uint32_t error_count ;			// Count multiple error, cleared in got_error_state()
  uint32_t state_count ;
  uint32_t my_value ;
  // Atomic Operation Registers
  uint32_t credit ;
#ifdef ENTL_STATE_DEBUG
  uint64_t addr ;
  uint64_t data ;
  uint64_t r_addr ;
  uint64_t r_data ;
#endif
#ifdef NETRONOME_HOST
  char *name ;
#endif
} entl_state_machine_t ;

void entl_state_init( __lmem entl_state_machine_t *mcn ) ;
void entl_set_random( __lmem entl_state_machine_t *mcn ) ;

/**
 * @brief On Received message, this should be called with the massage (MAC source & destination addr)
 * @param mcn : pointer to the state machine structure
 * @param d_addr : MAC destination address field
 * @param s_value : source value for Atomic Link Operation
 * @param d_value : pointer to returning value for Atomic Link Operation
 * @param ait_queue : number of messages in AIT queue
 * @param ait_command : Command field on the top of AIT QUEUE
 * @param egress_queue : number of messages in Egress queue
 *
 * @return ENTL_ACTION defined above
 */
int entl_received( __lmem entl_state_machine_t *mcn, uint64_t d_addr, uint64_t s_value, uint32_t ait_queue, uint32_t ait_command, uint32_t egress_queue ) ;
/**
 * @brief On sending ethernet packet, this function should be called to get the destination MAC address for message
 * @param mcn : pointer to the state machine structure
 * @param addr : pointer to MAC destination adress field, returning the value to be sent
 * @param ait_queue : number of messages in AIT queue
 * @param egress_queue : number of messages in Egress queue
 * @return ENTL_ACTION defined above
 */
int entl_next_send( __lmem entl_state_machine_t *mcn, uint64_t *addr, uint64_t *alo_data, uint32_t alo_command, uint32_t ait_queue) ;

// On receiving error (link down, timeout), this functon should be called to report to the state machine
void entl_state_error( __lmem entl_state_machine_t *mcn, uint32_t error_flag ) ;

void entl_set_debug() ;

#ifdef __cplusplus 
}
#endif


#endif /* _ENTL_STATE_MACHINE_H_ */

/* -*-  Mode:C; c-basic-offset:4; tab-width:4 -*- */
