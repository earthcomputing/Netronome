/*
 * Copyright (C) 2017,  Earth Computing Inc.  All rights reserved.
 *
 *
 * @file          dispatch_data.h
 * @brief         Dispatch Thread Data for Packet forwarding 
 *
 * Author:        Atsushi Kasuya
 *
 */

#ifndef _DISPATCH_DATA_H_
#define _DISPATCH_DATA_H_


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

#define PACKET_RING_SIZE_LW 256
#define PACKET_RING_NUM 1

 
// The data structre represents the internal state of ENTL
typedef struct thread_data {
    union {
        struct {
          uint32_t island_num ;			// island number 
          uint32_t me_num ;			    // me number
          uint32_t reg_num ;        // transfer reg number
          uint32_t sig_num ;			   // signal number
        };
        unsigned int __raw[4];          /**< Direct access to struct words */
    };
} thread_data_t ;

// Data received on ENTL (non-NOP) that passed to Th0 via master refrect. 
typedef struct entl_info {
  union {
    struct {
      unsigned int credit:15 ;
      unsigned int d:1 ;
      unsigned int value:16 ;
      unsigned int opcode:5 ;
      unsigned int etype:5 ;
      unsigned int dt:5 ;
      unsigned int sc:5 ;
      unsigned int fw:1 ;
    } ;
    unsigned int __raw[4] ;
  } ;
} entl_info_t ;


// Data transfered via transfer register on master refrect.
typedef struct fw_packet_data {
    union {
        struct {
          unsigned int island_num:8 ;     // island number 
          unsigned int nbi_num:8 ;        // NBI number to send the packet to
          unsigned int queue_num:8 ;      // TM queue number
          unsigned int sequencer_num:8 ;  // TM sequencer number  1
          unsigned int packet_num:16 ;     // packet number in CTM
          unsigned int seq_num:16 ;        // sequence number  2
          unsigned int next_lookup_id:32 ;  // 3
          unsigned int credit:15 ;
          unsigned int d:1 ;
          unsigned int padd0:16 ;         // 4
          unsigned int value:16 ;
          unsigned int opcode:16 ;        // 5
          unsigned int etype:5 ;
          unsigned int dt:5 ;
          unsigned int sc:5 ;
          unsigned int fw:1 ;
          unsigned int padd1:16 ;         // 6
        };
        unsigned int __raw[6];          /**< Direct access to struct words */
    };
} fw_packet_data_t ;

// Packet info. queued in the Egress Queue
typedef struct packet_data {
    union {
        struct {
          unsigned int island_num:8 ;     // island number 
          unsigned int nbi_num:8 ;        // NBI number to send the packet to
          unsigned int queue_num:8 ;      // TM queue number
          unsigned int sequencer_num:8 ;  // TM sequencer number  1
          unsigned int packet_num:32 ;     // packet number in CTM
          unsigned int seq_num:32 ;        // sequence number
          unsigned int next_lookup_id:32 ; // next lookup index
        };
        unsigned int __raw[4];          /**< Direct access to struct words */
    };
} packet_data_t ;

#endif /* _DISPATCH_DATA_H_ */

/* -*-  Mode:C; c-basic-offset:4; tab-width:4 -*- */
