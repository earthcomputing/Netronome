/**
 * Copyright (C) 2017,  Earth Computing Inc.  All rights reserved.
 *
 *
 * @file          entl_reflect.h
 * @brief         ENTL refrect  
 *
 * Author:        Atsushi Kasuya
 *
 */
#include <assert.h>
#include <nfp.h>
#include <me.h>
#include <mutexlv.h>
#include <stdint.h>
#include <nfp6000/nfp_me.h>


#ifndef _ENTL_REFLECT_H_
#define _ENTL_REFLECT_H_


typedef struct entl_reflect_data {
	uint64_t d_addr ;
	uint64_t data ;
	int island ;
	int pnum ;
} entl_reflect_data_t ;

#ifdef REFRECT_REG_ALLOCATE

#if (REFRECT_PORT == 0)

__visible __xread entl_reflect_data_t entl_data_in_p0 ;
__visible SIGNAL entl_send_sig_p0 ;

#define ENTL_DATA_IN entl_data_in_p0
#define ENTL_SEND_SIG entl_send_sig_p0

#endif

#if (REFRECT_PORT == 1)

__visible __xread entl_reflect_data_t entl_data_in_p1 ;
__visible SIGNAL entl_send_sig_p1 ;

#define ENTL_DATA_IN entl_data_in_p1
#define ENTL_SEND_SIG entl_send_sig_p1

#endif

#if (REFRECT_PORT == 2)

__visible __xread entl_reflect_data_t entl_data_in_p2 ;
__visible SIGNAL entl_send_sig_p2 ;

#define ENTL_DATA_IN entl_data_in_p2
#define ENTL_SEND_SIG entl_send_sig_p2

#endif

#if (REFRECT_PORT == 2)

__visible __xread entl_reflect_data_t entl_data_in_p2 ;
__visible SIGNAL entl_send_sig_p2 ;

#define ENTL_DATA_IN entl_data_in_p2
#define ENTL_SEND_SIG entl_send_sig_p2

#endif

#if (REFRECT_PORT == 2)

__visible __xread entl_reflect_data_t entl_data_in_p2 ;
__visible SIGNAL entl_send_sig_p2 ;

#define ENTL_DATA_IN entl_data_in_p2
#define ENTL_SEND_SIG entl_send_sig_p2

#endif

#if (REFRECT_PORT == 2)

__visible __xread entl_reflect_data_t entl_data_in_p2 ;
__visible SIGNAL entl_send_sig_p2 ;

#define ENTL_DATA_IN entl_data_in_p2
#define ENTL_SEND_SIG entl_send_sig_p2

#endif
#if (REFRECT_PORT == 2)

__visible __xread entl_reflect_data_t entl_data_in_p2 ;
__visible SIGNAL entl_send_sig_p2 ;

#define ENTL_DATA_IN entl_data_in_p2
#define ENTL_SEND_SIG entl_send_sig_p2

#endif

#endif

#ifdef FW_TABLE_IMPORT

__import __shared __imem_n(0) fw_table_entry_t fw_table0[FW_TABLE_SIZE] ;
__import __shared __imem_n(1) fw_table_entry_t fw_table1[FW_TABLE_SIZE] ;

#endif

int entl_remote_meid( int port ) ;
int entl_remote_signum( int port ) ;
int entl_xfer_reg_number( int port ) ;

/* XXX Move to some sort of CT reflect library */
__intrinsic void
entl_reflect(unsigned int dst_me, unsigned int dst_xfer,
             unsigned int sig_no, volatile __xwrite void *src_xfer,
             size_t size) ;

#endif