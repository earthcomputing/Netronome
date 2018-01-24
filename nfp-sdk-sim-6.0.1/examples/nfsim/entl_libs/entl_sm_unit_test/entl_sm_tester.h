/*
 * Copyright (C) 2017,  Earth Computing Inc.  All rights reserved.
 *
 *
 * @file          entl_sm_tester.h
 * @brief         ENTL State Machine Tester
 *
 * Author:        Atsushi Kasuya
 *
 */

#ifndef _ENTL_SM_TESTER_H_
#define _ENTL_SM_TESTER_H_

#include "entl_state_machine.h"

#ifdef __cplusplus 
extern "C" {
#endif

void dump_state( char *str, entl_state_machine_t *stm ) ;

int expect_state( entl_state_t *st, uint32_t i_know, uint32_t i_sent, uint32_t send_next, uint32_t state ) ;
int expect_state_only( entl_state_t *st, uint32_t state ) ;

int hello_2_wait( entl_state_machine_t *mcn ) ;

int hello_2_hello( entl_state_machine_t *mcn ) ;

int hello_2_entl( entl_state_machine_t *mcn ) ;

int hello_2_wait_2_entl( entl_state_machine_t *mcn ) ;

int hello_2_entl_loop( entl_state_machine_t *mcn ) ;

int hello_2_wait_2_entl_loop( entl_state_machine_t *mcn ) ;

int entl_ait_send( entl_state_machine_t *mcn ) ;
int entl_ait_rejected( entl_state_machine_t *mcn ) ;
int entl_ait_receive( entl_state_machine_t *mcn ) ;
int entl_ait_reject( entl_state_machine_t *mcn ) ;

int entl_alo_send( entl_state_machine_t *mcn ) ;

int entl_alo_receive( entl_state_machine_t *mcn ) ;

int hello_2_hello_same_value( entl_state_machine_t *mcn ) ;

int hello_error_hello( entl_state_machine_t *mcn ) ;

int hello_2_entl_error( entl_state_machine_t *mcn ) ;

int hello_2_wait_2_entl_error( entl_state_machine_t *mcn ) ;

int entl_ait_send_error( entl_state_machine_t *mcn ) ;

int entl_ait_send_comm_error( entl_state_machine_t *mcn ) ;

#ifdef __cplusplus 
}
#endif

#endif /* _ENTL_SM_TESTER_H_ */
