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

#define ENTL_SENDER_ME 32


/* XXX Move to some sort of CT reflect library */
__intrinsic void
entl_reflect_data(unsigned int dst_me, unsigned int dst_xfer,
             unsigned int sig_no, volatile __xwrite void *src_xfer,
             size_t size) ;

#endif