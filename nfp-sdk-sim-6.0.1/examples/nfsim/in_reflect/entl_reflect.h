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
	int res0 ;
	int res1 ;
} entl_reflect_data_t ;

__intrinsic void
entl_reflect_data(unsigned int dst_me, unsigned int dst_xfer,
             unsigned int sig_no, volatile __xwrite void *src_xfer,
             size_t size) ;

/** Copy local transfer registers to remote microengine.
 * Signal remote ME (destination) after transfer registers are written.
 *
 * @param data          Data to reflect
 * @param remote_island Remote island number (0 for local island)
 * @param remote_ME     Remote FPC or ME number
 * @param remote_xfer   Remote xfer register reg number
 * @param sig           Triggered signal number
 * @param count         Number of 32-bit words to reflect
 *
 * @note __xfer_reg_number() can be used to get remote_xfer register number.
 *
 */
__intrinsic
void entl_reflect_data_ptr40(
    __xwrite void *data,
    unsigned int remote_island,
    unsigned int remote_ME,
    unsigned int remote_xfer,
    int sig,
    unsigned int count
);

#endif