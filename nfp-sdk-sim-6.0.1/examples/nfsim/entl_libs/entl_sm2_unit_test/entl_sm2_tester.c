/*
 * Copyright (C) 2018,  Earth Computing Inc.  All rights reserved.
 *
 *
 * @file          entl_sm2_tester.c
 * @brief         ENTL State Machine Tester
 *
 * Author:        Atsushi Kasuya
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "cyc_random.h"

#include "entl_sm2_tester.h"

#define ENTL_DEBUG(fmt, args...) if(debug_flag) printf( fmt, ## args )

static entl_state_machine_t esm_copy ;

static debug_flag = 0 ;

static char* state_str( uint32_t state )
{
	switch(state) 
	{
		case ENTL_STATE_IDLE:		return "Idle" ;
		case ENTL_STATE_HELLO:		return "Hello" ;
		case ENTL_STATE_WAIT:		return "Wait" ;
		case ENTL_STATE_SEND:		return "Send" ;
		case ENTL_STATE_RECEIVE:	return "Receive" ;
		case ENTL_STATE_RA:			return "RA" ;
		case ENTL_STATE_SA:			return "SA" ;
		case ENTL_STATE_SB:			return "SB" ;
		case ENTL_STATE_RB:			return "RB" ;
		case ENTL_STATE_RAL:		return "RAL" ;
		case ENTL_STATE_SAL:		return "SAL" ;
		case ENTL_STATE_SBL:		return "SBL" ;
		case ENTL_STATE_RBL:		return "RBL" ;
		case ENTL_STATE_ERROR:		return "Error" ;
	}
	return "Unkown" ;
}

static void print_error( char *name, uint32_t error_flag )
{
	ENTL_DEBUG( "%s ", name ) ;
	if( error_flag == 0 ) ENTL_DEBUG( "No Error " ) ;
	if( error_flag & ENTL_ERROR_FLAG_SEQUENCE ) ENTL_DEBUG( "ERROR_FLAG_SEQUENCE " ) ;
	if( error_flag & ENTL_ERROR_FLAG_LINKDONW ) ENTL_DEBUG( "ERROR_FLAG_LINKDONW " ) ;
	if( error_flag & ENTL_ERROR_FLAG_TIMEOUT ) ENTL_DEBUG( "ERROR_FLAG_TIMEOUT " ) ;
	if( error_flag & ENTL_ERROR_SAME_ADDRESS ) ENTL_DEBUG( "ERROR_SAME_ADDRESS " ) ;
	if( error_flag & ENTL_ERROR_UNKOWN_CMD ) ENTL_DEBUG( "ERROR_UNKOWN_CMD " ) ;
	if( error_flag & ENTL_ERROR_UNKOWN_STATE ) ENTL_DEBUG( "ERROR_UNKOWN_STATE " ) ;
	if( error_flag & ENTL_ERROR_UNEXPECTED_LU ) ENTL_DEBUG( "ERROR_UNEXPECTED_LU " ) ;
	if( error_flag & ENTL_ERROR_FATAL ) ENTL_DEBUG( "ERROR_FATAL " ) ;
	ENTL_DEBUG( "\n" ) ;
}

static char *etype_str( uint64_t daddr ) 
{
	switch( GET_ECLP_ETYPE(daddr) )
	{
		case ENTL_ETYPE_NOP: return "etype: NOP" ;
		case ENTL_ETYPE_HELLO: return "etype: Hello" ;
		case ENTL_ETYPE_ENTL: return "etype: ENTL" ;
		case ENTL_ETYPE_AITS: return "etype: AITS" ;
		case ENTL_ETYPE_AITR: return "etype: AITR" ;
		case ENTL_ETYPE_AITA: return "etype: AITA" ;
	}
	return "etype: Unknow" ;

}

static void print_action( uint32_t action )
{
	if( action = 0 ) ENTL_DEBUG( "NOP " ) ;
	if( action & ENTL_ACTION_SEND ) ENTL_DEBUG( "SEND " ) ;
	if( action & ENTL_ACTION_SEND_AIT ) ENTL_DEBUG( "SEND_AIT " ) ;
	if( action & ENTL_ACTION_PROC_AIT ) ENTL_DEBUG( "PROC_AIT " ) ;
	if( action & ENTL_ACTION_SIG_AIT ) ENTL_DEBUG( "SIG_AIT " ) ;
	if( action & ENTL_ACTION_SEND_DAT ) ENTL_DEBUG( "SEND_DAT " ) ;
	if( action & ENTL_ACTION_SIG_ERR ) ENTL_DEBUG( "SIG_ERR " ) ;
	if( action & ENTL_ACTION_DROP_AIT ) ENTL_DEBUG( "DROP_AIT " ) ;
	if( action & ENTL_ACTION_SEND_ALO ) ENTL_DEBUG( "SEND_ALO " ) ;
	if( action & ENTL_ACTION_SEND_ALO_T ) ENTL_DEBUG( "SEND_ALO_T " ) ;
	if( action & ENTL_ACTION_SEND_ALO_T ) ENTL_DEBUG( "SEND_ALO_F " ) ;
	if( action & ENTL_ACTION_PROPAGATE_AIT ) ENTL_DEBUG( "PROPAGATE_AIT " ) ;
	ENTL_DEBUG( "\n" ) ;
}


static void print_entl_state( char *name, entl_state_t *st )
{
	ENTL_DEBUG(
		"  %s i_know %x i_sent %x send_next %d current state %s \n", 
		name, st->event_i_know, st->event_i_sent, st->event_send_next, state_str(st->current_state) 
	) ;
}

void dump_state( char *str, entl_state_machine_t *stm ) 
{
	ENTL_DEBUG( "%s ENTL %s\n", str, stm->name ) ;
	print_entl_state( "  state", &stm->state ) ;
	print_entl_state( "  error", &stm->error_state ) ;
	print_error( "  error_flag", stm->error_flag ) ;
	print_error( "  p_error_flag", stm->p_error_flag ) ;
	ENTL_DEBUG( "  error_count: %d\n", stm->error_count) ;
	ENTL_DEBUG( "  my_vale: %x\n", stm->my_value) ;
	dump_regs( &stm->ao, 0 ) ;
	ENTL_DEBUG( "  credit %d\n", stm->credit ) ;
}


int expect_state( entl_state_t *st, uint32_t i_know, uint32_t i_sent, uint32_t send_next, uint32_t state ) 
{
	if( st->event_i_know != i_know ) {
		ENTL_DEBUG( "expect_state st->event_i_know %x != i_know %x\n", st->event_i_know, i_know ) ;
		return 0 ;
	}
	if( st->event_i_sent != i_sent ) {
		ENTL_DEBUG( "expect_state st->event_i_sent %x != i_sent %x\n", st->event_i_sent, i_sent ) ;
		return 0 ;
	}
	if( st->event_send_next != send_next ) {
		ENTL_DEBUG( "expect_state st->event_send_next %x != send_next %x\n", st->event_send_next, send_next ) ;
		return 0 ;
	}
	if( st->current_state != state ) {
		ENTL_DEBUG( "expect_state st->current_state %d != state %d\n", st->current_state, state ) ;
		return 0 ;
	}
	ENTL_DEBUG( "expect_state match\n" ) ;
	return 1 ;
}

int expect_state_only( entl_state_t *st, uint32_t state ) 
{
	if( st->current_state != state ) {
		ENTL_DEBUG( "expect_state st->current_state %d != state %d\n", st->current_state, state ) ;
		return 0 ;
	}
	ENTL_DEBUG( "expect_state match\n" ) ;
	return 1 ;
}

int hello_2_wait( entl_state_machine_t *mcn ) 
{
	uint32_t value ;
	uint64_t d_addr ;
	uint64_t d_value ;
	int ret ;
	mcn->state.current_state = ENTL_STATE_HELLO ;
	mcn->state.event_i_know = 0 ;
	mcn->state.event_i_sent = 0 ;
	mcn->state.event_send_next = 0 ;
    mcn->my_value = mt_urandom(0) & 0x7fff ;
    value = mcn->my_value - 1 ;

    d_addr = ENTL_ETYPE_HELLO | value ;

    // int entl_received( __lmem entl_state_machine_t *mcn, uint64_t d_addr, uint64_t s_value, uint64_t *d_value, uint32_t ait_queue, uint32_t ait_command, uint32_t egress_queue ) ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

	dump_state( "hello_2_wait 1", mcn ) ;

	ENTL_DEBUG( "hello_2_wait 1 ret = %d\n", ret ) ;

	if( ret != ENTL_ACTION_SEND ) return 0 ;

	ret = expect_state( &mcn->state, 0, 0, 0, ENTL_STATE_WAIT ) ;
	if( !ret ) return 0 ;

	// int entl_next_send( __lmem entl_state_machine_t *mcn, uint64_t *addr, uint64_t *alo_data, uint32_t alo_command, uint32_t ait_queue) 
	ret = entl_next_send( mcn, &d_addr, &d_value, 0, 0 ) ; 

	dump_state( "hello_2_wait 2", mcn ) ;
	ENTL_DEBUG( "hello_2_wait 2 ret = %d\n", ret ) ;

	if( ret != ENTL_ACTION_SEND ) {
		ENTL_DEBUG( "ret != ENTL_ACTION_SEND\n" ) ;
		return 0 ;
	}

	if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL )  {
		ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL\n" ) ;
		return 0 ;
	}
	if( GET_ECLP_VALUE(d_addr) != 0 )  {
		ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != 0\n" ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, 0, 0, 0, ENTL_STATE_WAIT ) ;

	return ret ;

}

int hello_2_hello( entl_state_machine_t *mcn )
{
	uint32_t value ;
	uint64_t d_addr ;
	uint64_t d_value ;
	int ret ;
	mcn->state.current_state = ENTL_STATE_HELLO ;
	mcn->state.event_i_know = 0 ;
	mcn->state.event_i_sent = 0 ;
	mcn->state.event_send_next = 0 ;
    mcn->my_value = mt_urandom(0) & 0x7fff ;
    value = mcn->my_value + 1 ;

    d_addr = ENTL_ETYPE_HELLO | value ;

    // int entl_received( __lmem entl_state_machine_t *mcn, uint64_t d_addr, uint64_t s_value, uint64_t *d_value, uint32_t ait_queue, uint32_t ait_command, uint32_t egress_queue ) ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

	dump_state( "hello_2_hello 1", mcn ) ;

	ENTL_DEBUG( "hello_2_hello 1 ret = %d\n", ret ) ;

	if( ret != ENTL_ACTION_SEND ) return 0 ;

	ret = expect_state( &mcn->state, 0, 0, 0, ENTL_STATE_HELLO ) ;
	if( !ret ) return 0 ;

	// int entl_next_send( __lmem entl_state_machine_t *mcn, uint64_t *addr, uint64_t *alo_data, uint32_t alo_command, uint32_t ait_queue) 
	ret = entl_next_send( mcn, &d_addr, &d_value, 0, 0 ) ; 

	dump_state( "hello_2_hello 2", mcn ) ;
	ENTL_DEBUG( "hello_2_hello 2 ret = %d d_addr %lx\n", ret, d_addr ) ;

	if( ret != ENTL_ACTION_SEND ) {
		ENTL_DEBUG( "ret != ENTL_ACTION_SEND\n" ) ;
		return 0 ;
	}

	if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_HELLO )  {
		ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_HELLO\n" ) ;
		return 0 ;
	}
	if( GET_ECLP_VALUE(d_addr) != mcn->my_value )  {
		ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->my_value\n" ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, 0, 0, 0, ENTL_STATE_HELLO ) ;

	return ret ;

}

int hello_2_entl( entl_state_machine_t *mcn )
{
	uint64_t d_addr ;
	uint64_t d_value ;
	int ret ;

	mcn->state.current_state = ENTL_STATE_HELLO ;
	mcn->state.event_i_know = 0 ;
	mcn->state.event_i_sent = 0 ;
	mcn->state.event_send_next = 0 ;
    mcn->my_value = mt_urandom(0) & 0x7fff ;

    d_addr = ENTL_ETYPE_ENTL ;

    // int entl_received( __lmem entl_state_machine_t *mcn, uint64_t d_addr, uint64_t s_value, uint64_t *d_value, uint32_t ait_queue, uint32_t ait_command, uint32_t egress_queue ) ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

	dump_state( "hello_2_entl 1", mcn ) ;

	ENTL_DEBUG( "hello_2_entl 1 ret = %d\n", ret ) ;

	if( ret != ENTL_ACTION_SEND ) return 0 ;

	ret = expect_state( &mcn->state, 0, 0, 1, ENTL_STATE_SEND ) ;
	if( !ret ) return 0 ;

	// int entl_next_send( __lmem entl_state_machine_t *mcn, uint64_t *addr, uint64_t *alo_data, uint32_t alo_command, uint32_t ait_queue) 
	ret = entl_next_send( mcn, &d_addr, &d_value, 0, 0 ) ; 

	dump_state( "hello_2_entl 2", mcn ) ;
	ENTL_DEBUG( "hello_2_entl 2 ret = %x d_addr %lx\n", ret, d_addr ) ;

	if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT) ) {
		ENTL_DEBUG( "ret != ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT\n" ) ;
		return 0 ;
	}

	if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL )  {
		ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL\n" ) ;
		return 0 ;
	}
	if( GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent ) {
		ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->my_value\n" ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, 0, 1, 3, ENTL_STATE_RECEIVE ) ;

	return ret ;

}

int hello_2_wait_2_entl( entl_state_machine_t *mcn ) 
{
	uint32_t value ;
	uint64_t d_addr ;
	uint64_t d_value ;
	int ret ;
	mcn->state.current_state = ENTL_STATE_HELLO ;
	mcn->state.event_i_know = 0 ;
	mcn->state.event_i_sent = 0 ;
	mcn->state.event_send_next = 0 ;
    mcn->my_value = mt_urandom(0) & 0x7fff ;
    value = mcn->my_value - 1 ;

    d_addr = ENTL_ETYPE_HELLO | value ;

    // int entl_received( __lmem entl_state_machine_t *mcn, uint64_t d_addr, uint64_t s_value, uint64_t *d_value, uint32_t ait_queue, uint32_t ait_command, uint32_t egress_queue ) ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

	dump_state( "hello_2_wait_2_entl 1", mcn ) ;

	ENTL_DEBUG( "hello_2_wait_2_entl 1 ret = %d\n", ret ) ;

	if( ret != ENTL_ACTION_SEND ) return 0 ;

	ret = expect_state( &mcn->state, 0, 0, 0, ENTL_STATE_WAIT ) ;
	if( !ret ) return 0 ;

	// int entl_next_send( __lmem entl_state_machine_t *mcn, uint64_t *addr, uint64_t *alo_data, uint32_t alo_command, uint32_t ait_queue) 
	ret = entl_next_send( mcn, &d_addr, &d_value, 0, 0 ) ; 

	dump_state( "hello_2_wait_2_entl 2", mcn ) ;
	ENTL_DEBUG( "hello_2_wait_2_entl 2 ret = %d\n", ret ) ;

	if( ret != ENTL_ACTION_SEND ) {
		ENTL_DEBUG( "ret != ENTL_ACTION_SEND\n" ) ;
		return 0 ;
	}

	if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL )  {
		ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL\n" ) ;
		return 0 ;
	}
	if( GET_ECLP_VALUE(d_addr) != 0 )  {
		ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != 0\n" ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, 0, 0, 0, ENTL_STATE_SEND ) ;

    d_addr = ENTL_ETYPE_ENTL | 1 ;

    // int entl_received( __lmem entl_state_machine_t *mcn, uint64_t d_addr, uint64_t s_value, uint64_t *d_value, uint32_t ait_queue, uint32_t ait_command, uint32_t egress_queue ) ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

	dump_state( "hello_2_wait_2_entl 3", mcn ) ;

	ENTL_DEBUG( "hello_2_wait_2_entl 3 ret = %x d_addr %lx \n", ret, d_addr ) ;


	if( ret != ENTL_ACTION_SEND ) {
		ENTL_DEBUG( "ret != ENTL_ACTION_SEND \n" ) ;
		return 0 ;
	}

	ret = entl_next_send( mcn, &d_addr, &d_value, 0, 0 ) ; 

	dump_state( "hello_2_wait_2_entl 4", mcn ) ;
	ENTL_DEBUG( "hello_2_wait_2_entl 4 ret = %x d_addr %lx\n", ret, d_addr ) ;

	if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT) ) {
		ENTL_DEBUG( "ret != ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT\n" ) ;
		return 0 ;
	}

	if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL )  {
		ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL\n" ) ;
		return 0 ;
	}
	if( GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent ) {
		ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->my_value\n" ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, 1, 2, 4, ENTL_STATE_RECEIVE ) ;

	if( !ret ) return 0 ;


	return ret ;

}

int hello_2_entl_loop( entl_state_machine_t *mcn ) 
{
	uint64_t d_addr ;
	uint64_t d_value ;
	uint32_t value, p_sent ;
	int ret ;
	int i ;
	char testName[256] ;

	debug_flag = 0 ;

	mcn->state.current_state = ENTL_STATE_HELLO ;
	mcn->state.event_i_know = 0 ;
	mcn->state.event_i_sent = 0 ;
	mcn->state.event_send_next = 0 ;
    mcn->my_value = mt_urandom(0) & 0x7fff ;

    d_addr = ENTL_ETYPE_ENTL ;

    // int entl_received( __lmem entl_state_machine_t *mcn, uint64_t d_addr, uint64_t s_value, uint64_t *d_value, uint32_t ait_queue, uint32_t ait_command, uint32_t egress_queue ) ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

	dump_state( "hello_2_entl 1", mcn ) ;

	ENTL_DEBUG( "hello_2_entl 1 ret = %d\n", ret ) ;

	if( ret != ENTL_ACTION_SEND ) return 0 ;

	ret = expect_state( &mcn->state, 0, 0, 1, ENTL_STATE_SEND ) ;
	if( !ret ) return 0 ;

	// int entl_next_send( __lmem entl_state_machine_t *mcn, uint64_t *addr, uint64_t *alo_data, uint32_t alo_command, uint32_t ait_queue) 
	ret = entl_next_send( mcn, &d_addr, &d_value, 0, 0 ) ; 

	dump_state( "hello_2_entl 2", mcn ) ;
	ENTL_DEBUG( "hello_2_entl 2 ret = %x d_addr %lx\n", ret, d_addr ) ;

	if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT) ) {
		ENTL_DEBUG( "ret != ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT\n" ) ;
		return 0 ;
	}

	if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL )  {
		ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL\n" ) ;
		return 0 ;
	}
	if( GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent ) {
		ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->my_value\n" ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, 0, 1, 3, ENTL_STATE_RECEIVE ) ;

	// Same value checking 
	value = mcn->state.event_i_know ;
    d_addr = ENTL_ETYPE_ENTL | value ;
    p_sent = mcn->state.event_i_sent ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

	if( ret != ENTL_ACTION_NOP ) {
		ENTL_DEBUG( "ret != ENTL_ACTION_NOP \n" ) ;
		return 0 ;
	}
	ret = expect_state( &mcn->state, value, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_RECEIVE ) ;


	for( i = 0 ; i < 0x1ffff ; i++ ) {
		sprintf( testName, "loop %d", i) ;
		value = ECLP_VALUE_NEXT(mcn->state.event_i_sent) ;
    	d_addr = ENTL_ETYPE_ENTL | value ;
    	p_sent = mcn->state.event_i_sent ;
		ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

		if( ret != ENTL_ACTION_SEND ) {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "ret != ENTL_ACTION_SEND on %s\n", testName ) ;
			return 0 ;
		}

		ret = expect_state( &mcn->state, value, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_SEND ) ;
		if( !ret ) {
			dump_state( testName, mcn ) ;
			return 0 ;
		}
		ret = entl_next_send( mcn, &d_addr, &d_value, 0, 0 ) ; 

		if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT) ) {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "ret != ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT\n" ) ;
			return 0 ;
		}

		if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL )  {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL\n" ) ;
			return 0 ;
		}
		if( GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent ) {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->my_value\n" ) ;
			return 0 ;
		}

		ret = expect_state( &mcn->state, value, ECLP_VALUE_NEXT(value), ECLP_VALUE_PLUS2(ECLP_VALUE_NEXT(value)), ENTL_STATE_RECEIVE ) ;

		if( !ret ) {
			dump_state( testName, mcn ) ;
			return 0 ;
		}
	}

	return ret ;

}

int hello_2_wait_2_entl_loop( entl_state_machine_t *mcn ) 
{
	uint64_t d_addr ;
	uint64_t d_value ;
	uint32_t value, p_sent ;
	int ret ;
	int i ;
	char testName[256] ;

	mcn->state.current_state = ENTL_STATE_HELLO ;
	mcn->state.event_i_know = 0 ;
	mcn->state.event_i_sent = 0 ;
	mcn->state.event_send_next = 0 ;
    mcn->my_value = mt_urandom(0) & 0x7fff ;
    value = mcn->my_value - 1 ;

	debug_flag = 0 ;

    d_addr = ENTL_ETYPE_HELLO | value ;

    // int entl_received( __lmem entl_state_machine_t *mcn, uint64_t d_addr, uint64_t s_value, uint64_t *d_value, uint32_t ait_queue, uint32_t ait_command, uint32_t egress_queue ) ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

	dump_state( "hello_2_wait_2_entl 1", mcn ) ;

	ENTL_DEBUG( "hello_2_wait_2_entl 1 ret = %d\n", ret ) ;

	if( ret != ENTL_ACTION_SEND ) return 0 ;

	ret = expect_state( &mcn->state, 0, 0, 0, ENTL_STATE_WAIT ) ;
	if( !ret ) return 0 ;

	// int entl_next_send( __lmem entl_state_machine_t *mcn, uint64_t *addr, uint64_t *alo_data, uint32_t alo_command, uint32_t ait_queue) 
	ret = entl_next_send( mcn, &d_addr, &d_value, 0, 0 ) ; 

	dump_state( "hello_2_wait_2_entl 2", mcn ) ;
	ENTL_DEBUG( "hello_2_wait_2_entl 2 ret = %d\n", ret ) ;

	if( ret != ENTL_ACTION_SEND ) {
		ENTL_DEBUG( "ret != ENTL_ACTION_SEND\n" ) ;
		return 0 ;
	}

	if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL )  {
		ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL\n" ) ;
		return 0 ;
	}
	if( GET_ECLP_VALUE(d_addr) != 0 )  {
		ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != 0\n" ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, 0, 0, 0, ENTL_STATE_SEND ) ;

    d_addr = ENTL_ETYPE_ENTL | 1 ;

    // int entl_received( __lmem entl_state_machine_t *mcn, uint64_t d_addr, uint64_t s_value, uint64_t *d_value, uint32_t ait_queue, uint32_t ait_command, uint32_t egress_queue ) ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

	dump_state( "hello_2_wait_2_entl 3", mcn ) ;

	ENTL_DEBUG( "hello_2_wait_2_entl 3 ret = %x d_addr %lx \n", ret, d_addr ) ;


	if( ret != ENTL_ACTION_SEND ) {
		ENTL_DEBUG( "ret != ENTL_ACTION_SEND \n" ) ;
		return 0 ;
	}

	ret = entl_next_send( mcn, &d_addr, &d_value, 0, 0 ) ; 

	dump_state( "hello_2_wait_2_entl 4", mcn ) ;
	ENTL_DEBUG( "hello_2_wait_2_entl 4 ret = %x d_addr %lx\n", ret, d_addr ) ;

	if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT) ) {
		ENTL_DEBUG( "ret != ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT\n" ) ;
		return 0 ;
	}

	if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL )  {
		ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL\n" ) ;
		return 0 ;
	}
	if( GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent ) {
		ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->my_value\n" ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, 1, 2, 4, ENTL_STATE_RECEIVE ) ;

	if( !ret ) return 0 ;

	// Same value checking 
	value = mcn->state.event_i_know ;
    d_addr = ENTL_ETYPE_ENTL | value ;
    p_sent = mcn->state.event_i_sent ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

	if( ret != ENTL_ACTION_NOP ) {
		ENTL_DEBUG( "ret != ENTL_ACTION_NOP \n" ) ;
		return 0 ;
	}
	ret = expect_state( &mcn->state, value, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_RECEIVE ) ;


	for( i = 0 ; i < 0x1ffff ; i++ ) {
		sprintf( testName, "loop %d", i) ;
		value = ECLP_VALUE_NEXT(mcn->state.event_i_sent) ;
    	d_addr = ENTL_ETYPE_ENTL | value ;
    	p_sent = mcn->state.event_i_sent ;
		ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

		if( ret != ENTL_ACTION_SEND ) {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "ret != ENTL_ACTION_SEND on %s\n", testName ) ;
			return 0 ;
		}

		ret = expect_state( &mcn->state, value, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_SEND ) ;
		if( !ret ) {
			dump_state( testName, mcn ) ;
			return 0 ;
		}
		ret = entl_next_send( mcn, &d_addr, &d_value, 0, 0 ) ; 

		if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT) ) {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "ret != ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT\n" ) ;
			return 0 ;
		}

		if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL )  {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL\n" ) ;
			return 0 ;
		}
		if( GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent ) {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->my_value\n" ) ;
			return 0 ;
		}

		ret = expect_state( &mcn->state, value, ECLP_VALUE_NEXT(value), ECLP_VALUE_PLUS2(ECLP_VALUE_NEXT(value)), ENTL_STATE_RECEIVE ) ;

		if( !ret ) {
			dump_state( testName, mcn ) ;
			return 0 ;
		}
	}

	return ret ;

}

int entl_ait_send( entl_state_machine_t *mcn ) 
{
	uint64_t d_addr ;
	uint64_t d_value ;
	uint32_t value, p_sent ;
	int ret ;
	int i ;
	char testName[256] ;

	debug_flag = 0 ;
    //entl_set_debug() ;

	mcn->state.current_state = ENTL_STATE_HELLO ;
	mcn->state.event_i_know = 0 ;
	mcn->state.event_i_sent = 0 ;
	mcn->state.event_send_next = 0 ;
    mcn->my_value = mt_urandom(0) & 0x7fff ;

    d_addr = ENTL_ETYPE_ENTL ;

    // int entl_received( __lmem entl_state_machine_t *mcn, uint64_t d_addr, uint64_t s_value, uint64_t *d_value, uint32_t ait_queue, uint32_t ait_command, uint32_t egress_queue ) ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

	dump_state( "hello_2_entl 1", mcn ) ;

	ENTL_DEBUG( "hello_2_entl 1 ret = %d\n", ret ) ;

	if( ret != ENTL_ACTION_SEND ) return 0 ;

	ret = expect_state( &mcn->state, 0, 0, 1, ENTL_STATE_SEND ) ;
	if( !ret ) return 0 ;

	// int entl_next_send( __lmem entl_state_machine_t *mcn, uint64_t *addr, uint64_t *alo_data, uint32_t alo_command, uint32_t ait_queue) 
	ret = entl_next_send( mcn, &d_addr, &d_value, 0, 0 ) ; 

	dump_state( "hello_2_entl 2", mcn ) ;
	ENTL_DEBUG( "hello_2_entl 2 ret = %x d_addr %lx\n", ret, d_addr ) ;

	if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT) ) {
		ENTL_DEBUG( "ret != ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT\n" ) ;
		return 0 ;
	}

	if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL )  {
		ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL\n" ) ;
		return 0 ;
	}
	if( GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent ) {
		ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->my_value\n" ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, 0, 1, 3, ENTL_STATE_RECEIVE ) ;

	// Same value checking 
	value = mcn->state.event_i_know ;
    d_addr = ENTL_ETYPE_ENTL | value ;
    p_sent = mcn->state.event_i_sent ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

	if( ret != ENTL_ACTION_NOP ) {
		ENTL_DEBUG( "ret != ENTL_ACTION_NOP \n" ) ;
		return 0 ;
	}
	ret = expect_state( &mcn->state, value, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_RECEIVE ) ;


	for( i = 0 ; i < 2 ; i++ ) {
		sprintf( testName, "loop %d", i) ;
		value = ECLP_VALUE_NEXT(mcn->state.event_i_sent) ;
    	d_addr = ENTL_ETYPE_ENTL | value ;
    	p_sent = mcn->state.event_i_sent ;
		ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

		if( ret != ENTL_ACTION_SEND ) {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "ret != ENTL_ACTION_SEND on %s\n", testName ) ;
			return 0 ;
		}

		ret = expect_state( &mcn->state, value, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_SEND ) ;
		if( !ret ) {
			dump_state( testName, mcn ) ;
			return 0 ;
		}
		ret = entl_next_send( mcn, &d_addr, &d_value, 0, 0 ) ; 

		if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT) ) {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "ret != ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT\n" ) ;
			return 0 ;
		}

		if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL )  {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL\n" ) ;
			return 0 ;
		}
		if( GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent ) {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent\n" ) ;
			return 0 ;
		}

		ret = expect_state( &mcn->state, value, ECLP_VALUE_NEXT(value), ECLP_VALUE_PLUS2(ECLP_VALUE_NEXT(value)), ENTL_STATE_RECEIVE ) ;

		if( !ret ) {
			dump_state( testName, mcn ) ;
			return 0 ;
		}
	}
	sprintf( testName, "AIT send 1" ) ;

	value = ECLP_VALUE_NEXT(mcn->state.event_i_sent) ;
	d_addr = ENTL_ETYPE_ENTL | value ;
	p_sent = mcn->state.event_i_sent ;
	ret = entl_received( mcn, d_addr, 0, 1, 0, 0 ) ; // AIT queue has one

	if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_AIT) ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_AIT) on %s\n", testName ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, value, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_SEND ) ;
	if( !ret ) {
		dump_state( testName, mcn ) ;
		return 0 ;
	}
	ret = entl_next_send( mcn, &d_addr, &d_value, 0, 1 ) ; // AIT queue has one

	if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_AIT) ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "ret != ENTL_ACTION_SEND | ENTL_ACTION_SEND_AIT\n" ) ;
		return 0 ;
	}

	if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_AITS )  {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_AITS\n" ) ;
		return 0 ;
	}
	if( GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent\n" ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, value, ECLP_VALUE_NEXT(value), ECLP_VALUE_PLUS2(ECLP_VALUE_NEXT(value)), ENTL_STATE_RA ) ;

	if( !ret ) {
		dump_state( testName, mcn ) ;
		return 0 ;
	}

	sprintf( testName, "AIT send 2" ) ;

	value = ECLP_VALUE_NEXT(mcn->state.event_i_sent) ;
	d_addr = ENTL_ETYPE_AITR | value ;
	p_sent = mcn->state.event_i_sent ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ; 

	if( ret != ENTL_ACTION_SEND  ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "ret != ENTL_ACTION_SEND  on %s\n", testName ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, value, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_SA ) ;
	if( !ret ) {
		dump_state( testName, mcn ) ;
		return 0 ;
	}

	sprintf( testName, "AIT send 3" ) ;

	ret = entl_next_send( mcn, &d_addr, &d_value, 1, 0 ) ; // AIT queue has one

	if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SIG_AIT | ENTL_ACTION_DROP_AIT) ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "ret %x != (ENTL_ACTION_SEND | ENTL_ACTION_SIG_AIT | ENTL_ACTION_DROP_AIT) %x on %s\n", ret, (ENTL_ACTION_SEND | ENTL_ACTION_SIG_AIT | ENTL_ACTION_DROP_AIT), testName ) ;
		return 0 ;
	}

	if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_AITA )  {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_AITA\n" ) ;
		return 0 ;
	}
	if( GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent\n" ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, value, ECLP_VALUE_NEXT(value), ECLP_VALUE_PLUS2(ECLP_VALUE_NEXT(value)), ENTL_STATE_RECEIVE ) ;

	if( !ret ) {
		dump_state( testName, mcn ) ;
		return 0 ;
	}

	return ret ;

}

int entl_ait_rejected( entl_state_machine_t *mcn ) 
{
	uint64_t d_addr ;
	uint64_t d_value ;
	uint32_t value, p_sent ;
	int ret ;
	int i ;
	char testName[256] ;

	debug_flag = 0 ;
    //entl_set_debug() ;

	mcn->state.current_state = ENTL_STATE_HELLO ;
	mcn->state.event_i_know = 0 ;
	mcn->state.event_i_sent = 0 ;
	mcn->state.event_send_next = 0 ;
    mcn->my_value = mt_urandom(0) & 0x7fff ;

    d_addr = ENTL_ETYPE_ENTL ;

    // int entl_received( __lmem entl_state_machine_t *mcn, uint64_t d_addr, uint64_t s_value, uint64_t *d_value, uint32_t ait_queue, uint32_t ait_command, uint32_t egress_queue ) ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

	dump_state( "entl_ait_rejected 1", mcn ) ;

	ENTL_DEBUG( "entl_ait_rejected 1 ret = %d\n", ret ) ;

	if( ret != ENTL_ACTION_SEND ) return 0 ;

	ret = expect_state( &mcn->state, 0, 0, 1, ENTL_STATE_SEND ) ;
	if( !ret ) return 0 ;

	// int entl_next_send( __lmem entl_state_machine_t *mcn, uint64_t *addr, uint64_t *alo_data, uint32_t alo_command, uint32_t ait_queue) 
	ret = entl_next_send( mcn, &d_addr, &d_value, 0, 0 ) ; 

	dump_state( "entl_ait_rejected 2", mcn ) ;
	ENTL_DEBUG( "entl_ait_rejected 2 ret = %x d_addr %lx\n", ret, d_addr ) ;

	if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT) ) {
		ENTL_DEBUG( "ret != ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT\n" ) ;
		return 0 ;
	}

	if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL )  {
		ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL\n" ) ;
		return 0 ;
	}
	if( GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent ) {
		ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->my_value\n" ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, 0, 1, 3, ENTL_STATE_RECEIVE ) ;

	// Same value checking 
	value = mcn->state.event_i_know ;
    d_addr = ENTL_ETYPE_ENTL | value ;
    p_sent = mcn->state.event_i_sent ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

	if( ret != ENTL_ACTION_NOP ) {
		ENTL_DEBUG( "ret != ENTL_ACTION_NOP \n" ) ;
		return 0 ;
	}
	ret = expect_state( &mcn->state, value, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_RECEIVE ) ;


	for( i = 0 ; i < 2 ; i++ ) {
		sprintf( testName, "entl_ait_rejected loop %d", i) ;
		value = ECLP_VALUE_NEXT(mcn->state.event_i_sent) ;
    	d_addr = ENTL_ETYPE_ENTL | value ;
    	p_sent = mcn->state.event_i_sent ;
		ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

		if( ret != ENTL_ACTION_SEND ) {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "ret != ENTL_ACTION_SEND on %s\n", testName ) ;
			return 0 ;
		}

		ret = expect_state( &mcn->state, value, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_SEND ) ;
		if( !ret ) {
			dump_state( testName, mcn ) ;
			return 0 ;
		}
		ret = entl_next_send( mcn, &d_addr, &d_value, 0, 0 ) ; 

		if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT) ) {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "ret != ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT\n" ) ;
			return 0 ;
		}

		if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL )  {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL\n" ) ;
			return 0 ;
		}
		if( GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent ) {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent\n" ) ;
			return 0 ;
		}

		ret = expect_state( &mcn->state, value, ECLP_VALUE_NEXT(value), ECLP_VALUE_PLUS2(ECLP_VALUE_NEXT(value)), ENTL_STATE_RECEIVE ) ;

		if( !ret ) {
			dump_state( testName, mcn ) ;
			return 0 ;
		}
	}
	sprintf( testName, "AIT rejected 1" ) ;

	value = ECLP_VALUE_NEXT(mcn->state.event_i_sent) ;
	d_addr = ENTL_ETYPE_ENTL | value ;
	p_sent = mcn->state.event_i_sent ;
	ret = entl_received( mcn, d_addr, 0, 1, 0, 0 ) ; // AIT queue has one

	if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_AIT) ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_AIT) on %s\n", testName ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, value, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_SEND ) ;
	if( !ret ) {
		dump_state( testName, mcn ) ;
		return 0 ;
	}
	ret = entl_next_send( mcn, &d_addr, &d_value, 0, 1 ) ; // AIT queue has one

	if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_AIT) ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "ret != ENTL_ACTION_SEND | ENTL_ACTION_SEND_AIT\n" ) ;
		return 0 ;
	}

	if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_AITS )  {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_AITS\n" ) ;
		return 0 ;
	}
	if( GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent\n" ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, value, ECLP_VALUE_NEXT(value), ECLP_VALUE_PLUS2(ECLP_VALUE_NEXT(value)), ENTL_STATE_RA ) ;

	if( !ret ) {
		dump_state( testName, mcn ) ;
		return 0 ;
	}

	sprintf( testName, "AIT rejected 2" ) ;

	value = ECLP_VALUE_NEXT(mcn->state.event_i_sent) ;
	d_addr = ENTL_ETYPE_ENTL | value ;
	p_sent = mcn->state.event_i_sent ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ; 

	if( ret != ENTL_ACTION_SEND  ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "ret != ENTL_ACTION_SEND  on %s\n", testName ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, value, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_SEND ) ;
	if( !ret ) {
		dump_state( testName, mcn ) ;
		return 0 ;
	}

	return ret ;

}

int entl_ait_receive( entl_state_machine_t *mcn )
{
	uint64_t d_addr ;
	uint64_t d_value ;
	uint32_t value, p_sent ;
	int ret ;
	int i ;
	char testName[256] ;

	//debug_flag = 1 ;
    //entl_set_debug() ;

	mcn->state.current_state = ENTL_STATE_HELLO ;
	mcn->state.event_i_know = 0 ;
	mcn->state.event_i_sent = 0 ;
	mcn->state.event_send_next = 0 ;
    mcn->my_value = mt_urandom(0) & 0x7fff ;

    d_addr = ENTL_ETYPE_ENTL ;

    // int entl_received( __lmem entl_state_machine_t *mcn, uint64_t d_addr, uint64_t s_value, uint64_t *d_value, uint32_t ait_queue, uint32_t ait_command, uint32_t egress_queue ) ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

	dump_state( "entl_ait_receive 1", mcn ) ;

	ENTL_DEBUG( "entl_ait_receive 1 ret = %d\n", ret ) ;

	if( ret != ENTL_ACTION_SEND ) return 0 ;

	ret = expect_state( &mcn->state, 0, 0, 1, ENTL_STATE_SEND ) ;
	if( !ret ) return 0 ;

	// int entl_next_send( __lmem entl_state_machine_t *mcn, uint64_t *addr, uint64_t *alo_data, uint32_t alo_command, uint32_t ait_queue) 
	ret = entl_next_send( mcn, &d_addr, &d_value, 0, 0 ) ; 

	dump_state( "entl_ait_receive 2", mcn ) ;
	ENTL_DEBUG( "entl_ait_receive 2 ret = %x d_addr %lx\n", ret, d_addr ) ;

	if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT) ) {
		ENTL_DEBUG( "ret != ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT\n" ) ;
		return 0 ;
	}

	if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL )  {
		ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL\n" ) ;
		return 0 ;
	}
	if( GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent ) {
		ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->my_value\n" ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, 0, 1, 3, ENTL_STATE_RECEIVE ) ;

	// Same value checking 
	value = mcn->state.event_i_know ;
    d_addr = ENTL_ETYPE_ENTL | value ;
    p_sent = mcn->state.event_i_sent ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

	if( ret != ENTL_ACTION_NOP ) {
		ENTL_DEBUG( "ret != ENTL_ACTION_NOP \n" ) ;
		return 0 ;
	}
	ret = expect_state( &mcn->state, value, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_RECEIVE ) ;


	for( i = 0 ; i < 2 ; i++ ) {
		sprintf( testName, "loop %d", i) ;
		value = ECLP_VALUE_NEXT(mcn->state.event_i_sent) ;
    	d_addr = ENTL_ETYPE_ENTL | value ;
    	p_sent = mcn->state.event_i_sent ;
		ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

		if( ret != ENTL_ACTION_SEND ) {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "ret != ENTL_ACTION_SEND on %s\n", testName ) ;
			return 0 ;
		}

		ret = expect_state( &mcn->state, value, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_SEND ) ;
		if( !ret ) {
			dump_state( testName, mcn ) ;
			return 0 ;
		}
		ret = entl_next_send( mcn, &d_addr, &d_value, 0, 0 ) ; 

		if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT) ) {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "ret != ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT\n" ) ;
			return 0 ;
		}

		if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL )  {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL\n" ) ;
			return 0 ;
		}
		if( GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent ) {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent\n" ) ;
			return 0 ;
		}

		ret = expect_state( &mcn->state, value, ECLP_VALUE_NEXT(value), ECLP_VALUE_PLUS2(ECLP_VALUE_NEXT(value)), ENTL_STATE_RECEIVE ) ;

		if( !ret ) {
			dump_state( testName, mcn ) ;
			return 0 ;
		}
	}
	sprintf( testName, "entl_ait_receive 3" ) ;

	value = ECLP_VALUE_NEXT(mcn->state.event_i_sent) ;
	d_addr = ENTL_ETYPE_AITS | value ;
	p_sent = mcn->state.event_i_sent ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ; // AIT queue has one

	if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_PROC_AIT) ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "ret != (ENTL_ACTION_SEND | ENTL_ACTION_PROC_AIT) on %s\n", testName ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, value, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_SB ) ;
	if( !ret ) {
		dump_state( testName, mcn ) ;
		return 0 ;
	}
	ret = entl_next_send( mcn, &d_addr, &d_value, 0, 0 ) ; // AIT queue has one

	if( ret != (ENTL_ACTION_SEND ) ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "ret != ENTL_ACTION_SEND \n" ) ;
		return 0 ;
	}

	if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_AITR )  {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_AITS\n" ) ;
		return 0 ;
	}
	if( GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent\n" ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, value, ECLP_VALUE_NEXT(value), ECLP_VALUE_PLUS2(ECLP_VALUE_NEXT(value)), ENTL_STATE_RB ) ;

	if( !ret ) {
		dump_state( testName, mcn ) ;
		return 0 ;
	}

	sprintf( testName, "entl_ait_receive 4" ) ;

	value = ECLP_VALUE_NEXT(mcn->state.event_i_sent) ;
	d_addr = ENTL_ETYPE_AITA | value ;
	p_sent = mcn->state.event_i_sent ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ; 

	if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SIG_AIT) ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "ret %x != ENTL_ACTION_SEND | ENTL_ACTION_SIG_AIT %x on %s\n", ret, (ENTL_ACTION_SEND | ENTL_ACTION_SIG_AIT), testName ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, value, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_SEND ) ;
	if( !ret ) {
		dump_state( testName, mcn ) ;
		return 0 ;
	}

	return ret ;

}

int entl_ait_reject( entl_state_machine_t *mcn )
{
	uint64_t d_addr ;
	uint64_t d_value ;
	uint32_t value, p_sent ;
	int ret ;
	int i ;
	char testName[256] ;

	//debug_flag = 1 ;
    //entl_set_debug() ;

	mcn->state.current_state = ENTL_STATE_HELLO ;
	mcn->state.event_i_know = 0 ;
	mcn->state.event_i_sent = 0 ;
	mcn->state.event_send_next = 0 ;
    mcn->my_value = mt_urandom(0) & 0x7fff ;

    d_addr = ENTL_ETYPE_ENTL ;

    // int entl_received( __lmem entl_state_machine_t *mcn, uint64_t d_addr, uint64_t s_value, uint64_t *d_value, uint32_t ait_queue, uint32_t ait_command, uint32_t egress_queue ) ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

	dump_state( "entl_ait_reject 1", mcn ) ;

	ENTL_DEBUG( "entl_ait_reject 1 ret = %d\n", ret ) ;

	if( ret != ENTL_ACTION_SEND ) return 0 ;

	ret = expect_state( &mcn->state, 0, 0, 1, ENTL_STATE_SEND ) ;
	if( !ret ) return 0 ;

	// int entl_next_send( __lmem entl_state_machine_t *mcn, uint64_t *addr, uint64_t *alo_data, uint32_t alo_command, uint32_t ait_queue) 
	ret = entl_next_send( mcn, &d_addr, &d_value, 0, 0 ) ; 

	dump_state( "entl_ait_reject 2", mcn ) ;
	ENTL_DEBUG( "entl_ait_reject 2 ret = %x d_addr %lx\n", ret, d_addr ) ;

	if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT) ) {
		ENTL_DEBUG( "ret != ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT\n" ) ;
		return 0 ;
	}

	if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL )  {
		ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL\n" ) ;
		return 0 ;
	}
	if( GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent ) {
		ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->my_value\n" ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, 0, 1, 3, ENTL_STATE_RECEIVE ) ;

	// Same value checking 
	value = mcn->state.event_i_know ;
    d_addr = ENTL_ETYPE_ENTL | value ;
    p_sent = mcn->state.event_i_sent ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

	if( ret != ENTL_ACTION_NOP ) {
		ENTL_DEBUG( "ret != ENTL_ACTION_NOP \n" ) ;
		return 0 ;
	}
	ret = expect_state( &mcn->state, value, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_RECEIVE ) ;


	for( i = 0 ; i < 2 ; i++ ) {
		sprintf( testName, "loop %d", i) ;
		value = ECLP_VALUE_NEXT(mcn->state.event_i_sent) ;
    	d_addr = ENTL_ETYPE_ENTL | value ;
    	p_sent = mcn->state.event_i_sent ;
		ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

		if( ret != ENTL_ACTION_SEND ) {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "ret != ENTL_ACTION_SEND on %s\n", testName ) ;
			return 0 ;
		}

		ret = expect_state( &mcn->state, value, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_SEND ) ;
		if( !ret ) {
			dump_state( testName, mcn ) ;
			return 0 ;
		}
		ret = entl_next_send( mcn, &d_addr, &d_value, 0, 0 ) ; 

		if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT) ) {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "ret != ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT\n" ) ;
			return 0 ;
		}

		if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL )  {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL\n" ) ;
			return 0 ;
		}
		if( GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent ) {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent\n" ) ;
			return 0 ;
		}

		ret = expect_state( &mcn->state, value, ECLP_VALUE_NEXT(value), ECLP_VALUE_PLUS2(ECLP_VALUE_NEXT(value)), ENTL_STATE_RECEIVE ) ;

		if( !ret ) {
			dump_state( testName, mcn ) ;
			return 0 ;
		}
	}
	sprintf( testName, "entl_ait_reject 3" ) ;

	value = ECLP_VALUE_NEXT(mcn->state.event_i_sent) ;
	d_addr = ENTL_ETYPE_AITS | value ;
	p_sent = mcn->state.event_i_sent ;
	ret = entl_received( mcn, d_addr, 0, MAX_ENTT_QUEUE_SIZE, 0, 0 ) ; // AIT queue has one

	if( ret != (ENTL_ACTION_SEND) ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "ret %x != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT) on %s\n", ret, testName ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, value, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_SEND ) ;
	if( !ret ) {
		dump_state( testName, mcn ) ;
		return 0 ;
	}
	ret = entl_next_send( mcn, &d_addr, &d_value, 0, 0 ) ; // AIT queue has one

	if( ret != ( ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT ) ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "ret %x != ( ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT ) \n", ret ) ;
		return 0 ;
	}

	if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL )  {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL\n" ) ;
		return 0 ;
	}
	if( GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent\n" ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, value, ECLP_VALUE_NEXT(value), ECLP_VALUE_PLUS2(ECLP_VALUE_NEXT(value)), ENTL_STATE_RECEIVE ) ;

	if( !ret ) {
		dump_state( testName, mcn ) ;
		return 0 ;
	}

	return ret ;

}


int entl_alo_send( entl_state_machine_t *mcn ) 
{
	uint64_t d_addr ;
	uint64_t d_value ;
	uint32_t value, p_sent ;
	int ret ;
	int i ;
	char testName[256] ;

	//debug_flag = 1 ;
    //entl_set_debug() ;

	mcn->state.current_state = ENTL_STATE_HELLO ;
	mcn->state.event_i_know = 0 ;
	mcn->state.event_i_sent = 0 ;
	mcn->state.event_send_next = 0 ;
    mcn->my_value = mt_urandom(0) & 0x7fff ;

	alo_regs_init( &mcn->ao ) ;

    d_addr = ENTL_ETYPE_ENTL ;

    // int entl_received( __lmem entl_state_machine_t *mcn, uint64_t d_addr, uint64_t s_value, uint64_t *d_value, uint32_t ait_queue, uint32_t ait_command, uint32_t egress_queue ) ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

	dump_state( "entl_alo_send 1", mcn ) ;

	ENTL_DEBUG( "entl_alo_send 1 ret = %d\n", ret ) ;

	if( ret != ENTL_ACTION_SEND ) return 0 ;

	ret = expect_state( &mcn->state, 0, 0, 1, ENTL_STATE_SEND ) ;
	if( !ret ) return 0 ;

	// int entl_next_send( __lmem entl_state_machine_t *mcn, uint64_t *addr, uint64_t *alo_data, uint32_t alo_command, uint32_t ait_queue) 
	ret = entl_next_send( mcn, &d_addr, &d_value, 0, 0 ) ; 

	dump_state( "entl_alo_send 2", mcn ) ;
	ENTL_DEBUG( "entl_alo_send 2 ret = %x d_addr %lx\n", ret, d_addr ) ;

	if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT) ) {
		ENTL_DEBUG( "ret != ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT\n" ) ;
		return 0 ;
	}

	if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL )  {
		ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL\n" ) ;
		return 0 ;
	}
	if( GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent ) {
		ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->my_value\n" ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, 0, 1, 3, ENTL_STATE_RECEIVE ) ;

	// Same value checking 
	value = mcn->state.event_i_know ;
    d_addr = ENTL_ETYPE_ENTL | value ;
    p_sent = mcn->state.event_i_sent ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

	if( ret != ENTL_ACTION_NOP ) {
		ENTL_DEBUG( "ret != ENTL_ACTION_NOP \n" ) ;
		return 0 ;
	}
	ret = expect_state( &mcn->state, value, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_RECEIVE ) ;


	for( i = 0 ; i < 2 ; i++ ) {
		sprintf( testName, "entl_alo_send loop %d", i) ;
		value = ECLP_VALUE_NEXT(mcn->state.event_i_sent) ;
    	d_addr = ENTL_ETYPE_ENTL | value ;
    	p_sent = mcn->state.event_i_sent ;
		ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

		if( ret != ENTL_ACTION_SEND ) {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "ret != ENTL_ACTION_SEND on %s\n", testName ) ;
			return 0 ;
		}

		ret = expect_state( &mcn->state, value, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_SEND ) ;
		if( !ret ) {
			dump_state( testName, mcn ) ;
			return 0 ;
		}
		ret = entl_next_send( mcn, &d_addr, &d_value, 0, 0 ) ; 

		if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT) ) {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "ret != ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT\n" ) ;
			return 0 ;
		}

		if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL )  {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL\n" ) ;
			return 0 ;
		}
		if( GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent ) {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent\n" ) ;
			return 0 ;
		}

		ret = expect_state( &mcn->state, value, ECLP_VALUE_NEXT(value), ECLP_VALUE_PLUS2(ECLP_VALUE_NEXT(value)), ENTL_STATE_RECEIVE ) ;

		if( !ret ) {
			dump_state( testName, mcn ) ;
			return 0 ;
		}
	}
	sprintf( testName, "entl_alo_send 3" ) ;

	value = ECLP_VALUE_NEXT(mcn->state.event_i_sent) ;
	d_addr = ENTL_ETYPE_ENTL | value ;
	p_sent = mcn->state.event_i_sent ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ; // AIT queue has one

	if( ret != (ENTL_ACTION_SEND ) ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "ret %x != (ENTL_ACTION_SEND ) on %s\n", ret, testName ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, value, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_SEND ) ;
	if( !ret ) {
		dump_state( testName, mcn ) ;
		return 0 ;
	}

	mcn->ao.reg[0] = 100 ;

	ret = entl_next_send( mcn, &d_addr, &d_value, (ALO_RD | ALO_COND_LT), 0 ) ; // AIT queue has one

	sprintf( testName, "entl_alo_send 4" ) ;

	if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_ALO) ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "ret %x != ENTL_ACTION_SEND | ENTL_ACTION_SEND_ALO on %s %x\n", ret, testName, (ALO_RD | ALO_COND_LT) ) ;
		return 0 ;
	}

	if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_AITS )  {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_AITS\n" ) ;
		return 0 ;
	}
	if( GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent\n" ) ;
		return 0 ;
	}

  	if( ALO_OPCODE(d_addr) != (ALO_RD | ALO_COND_LT) ) {
		ENTL_DEBUG( "ALO_OPCODE(d_addr) != (ALO_RD | ALO_COND_LT)\n" ) ;
		return 0 ;
  	}

  	if( d_value != 100 ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "d_value %lx != 100\n", d_value ) ;
		return 0 ;
  	}

	ret = expect_state( &mcn->state, value, ECLP_VALUE_NEXT(value), ECLP_VALUE_PLUS2(ECLP_VALUE_NEXT(value)), ENTL_STATE_RAL ) ;

	if( !ret ) {
		dump_state( testName, mcn ) ;
		return 0 ;
	}

	sprintf( testName, "entl_alo_send 5" ) ;

	value = ECLP_VALUE_NEXT(mcn->state.event_i_sent) ;
	d_addr = ENTL_ETYPE_AITR | value | 1 << 16 ; // state is true 
	p_sent = mcn->state.event_i_sent ;
	ret = entl_received( mcn, d_addr, 500, 0, 0, 0 ) ; 

	if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SIG_AIT ) ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "ret %x != ( ENTL_ACTION_SEND | ENTL_ACTION_SIG_AIT )  on %s\n", ret, testName ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, value, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_SAL ) ;
	if( !ret ) {
		dump_state( testName, mcn ) ;
		return 0 ;
	}

	sprintf( testName, "entl_alo_send 6" ) ;

	ret = entl_next_send( mcn, &d_addr, &d_value, 1, 0 ) ; // AIT queue has one

	if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SIG_AIT | ENTL_ACTION_DROP_AIT) ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "ret %x != (ENTL_ACTION_SEND | ENTL_ACTION_SIG_AIT | ENTL_ACTION_DROP_AIT) %x on %s\n", ret, (ENTL_ACTION_SEND | ENTL_ACTION_SIG_AIT | ENTL_ACTION_DROP_AIT), testName ) ;
		return 0 ;
	}

	if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_AITA )  {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_AITA\n" ) ;
		return 0 ;
	}

	if( GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent\n" ) ;
		return 0 ;
	}

	if( mcn->ao.reg[0] != 500 ) {
		ENTL_DEBUG( "mcn->ao.reg[0] != 500\n" ) ;
		dump_regs( &mcn->ao, testName ) ;
		return 0 ;
	}
	ret = expect_state( &mcn->state, value, ECLP_VALUE_NEXT(value), ECLP_VALUE_PLUS2(ECLP_VALUE_NEXT(value)), ENTL_STATE_RECEIVE ) ;

	if( !ret ) {
		dump_state( testName, mcn ) ;
		return 0 ;
	}





	sprintf( testName, "entl_alo_send 7" ) ;

	value = ECLP_VALUE_NEXT(mcn->state.event_i_sent) ;
	d_addr = ENTL_ETYPE_ENTL | value ;
	p_sent = mcn->state.event_i_sent ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ; // AIT queue has one

	if( ret != (ENTL_ACTION_SEND) ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "ret %x != (ENTL_ACTION_SEND) on %s\n", ret, testName ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, value, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_SEND ) ;
	if( !ret ) {
		dump_state( testName, mcn ) ;
		return 0 ;
	}

	mcn->ao.reg[0] = 100 ;

	sprintf( testName, "entl_alo_send 8" ) ;

	ret = entl_next_send( mcn, &d_addr, &d_value, (ALO_RD | ALO_COND_LT), 0 ) ; // AIT queue has one

	if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_ALO) ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "ret %x != ENTL_ACTION_SEND | ENTL_ACTION_SEND_ALO on %s\n", ret, testName ) ;
		return 0 ;
	}

	if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_AITS )  {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_AITS\n" ) ;
		return 0 ;
	}
	if( GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent\n" ) ;
		return 0 ;
	}

  	if( ALO_OPCODE(d_addr) != (ALO_RD | ALO_COND_LT) ) {
		ENTL_DEBUG( "ALO_OPCODE(d_addr) != (ALO_RD | ALO_COND_LT)\n" ) ;
		return 0 ;
  	}

  	if( d_value != 100 ) {
		ENTL_DEBUG( "d_value != 100\n" ) ;
		return 0 ;
  	}

	ret = expect_state( &mcn->state, value, ECLP_VALUE_NEXT(value), ECLP_VALUE_PLUS2(ECLP_VALUE_NEXT(value)), ENTL_STATE_RAL ) ;

	if( !ret ) {
		dump_state( testName, mcn ) ;
		return 0 ;
	}

	sprintf( testName, "entl_alo_send 9" ) ;

	value = ECLP_VALUE_NEXT(mcn->state.event_i_sent) ;
	d_addr = ENTL_ETYPE_AITR | value  ; // state is false 
	p_sent = mcn->state.event_i_sent ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ; 

	if( ret != ENTL_ACTION_SEND  ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "ret != ENTL_ACTION_SEND  on %s\n", testName ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, value, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_SAL ) ;
	if( !ret ) {
		dump_state( testName, mcn ) ;
		return 0 ;
	}

	sprintf( testName, "entl_alo_send 10" ) ;

	ret = entl_next_send( mcn, &d_addr, &d_value, 0, 0 ) ; // AIT queue has one

	if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SIG_AIT | ENTL_ACTION_DROP_AIT) ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "ret %x != (ENTL_ACTION_SEND | ENTL_ACTION_SIG_AIT | ENTL_ACTION_DROP_AIT) %x on %s\n", ret, (ENTL_ACTION_SEND | ENTL_ACTION_SIG_AIT | ENTL_ACTION_DROP_AIT), testName ) ;
		return 0 ;
	}

	if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_AITA )  {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_AITA\n" ) ;
		return 0 ;
	}

	if( GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent\n" ) ;
		return 0 ;
	}

	if( mcn->ao.reg[0] != 100 ) {
		ENTL_DEBUG( "mcn->ao.reg[0] != 100\n" ) ;
		return 0 ;
	}
	ret = expect_state( &mcn->state, value, ECLP_VALUE_NEXT(value), ECLP_VALUE_PLUS2(ECLP_VALUE_NEXT(value)), ENTL_STATE_RECEIVE ) ;

	if( !ret ) {
		dump_state( testName, mcn ) ;
		return 0 ;
	}

	return ret ;

}

int entl_alo_receive( entl_state_machine_t *mcn ) 
{
	uint64_t d_addr ;
	uint64_t d_value ;
	uint32_t value, p_sent ;
	int ret ;
	int i ;
	char testName[256] ;

	//debug_flag = 1 ;
    //entl_set_debug() ;

	mcn->state.current_state = ENTL_STATE_HELLO ;
	mcn->state.event_i_know = 0 ;
	mcn->state.event_i_sent = 0 ;
	mcn->state.event_send_next = 0 ;
    mcn->my_value = mt_urandom(0) & 0x7fff ;

    d_addr = ENTL_ETYPE_ENTL ;

    // int entl_received( __lmem entl_state_machine_t *mcn, uint64_t d_addr, uint64_t s_value, uint64_t *d_value, uint32_t ait_queue, uint32_t ait_command, uint32_t egress_queue ) ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

	dump_state( "entl_alo_receive 1", mcn ) ;

	ENTL_DEBUG( "entl_alo_receive 1 ret = %d\n", ret ) ;

	if( ret != ENTL_ACTION_SEND ) return 0 ;

	ret = expect_state( &mcn->state, 0, 0, 1, ENTL_STATE_SEND ) ;
	if( !ret ) return 0 ;

	// int entl_next_send( __lmem entl_state_machine_t *mcn, uint64_t *addr, uint64_t *alo_data, uint32_t alo_command, uint32_t ait_queue) 
	ret = entl_next_send( mcn, &d_addr, &d_value, 0, 0 ) ; 

	dump_state( "entl_alo_receive 2", mcn ) ;
	ENTL_DEBUG( "entl_alo_receive 2 ret = %x d_addr %lx\n", ret, d_addr ) ;

	if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT) ) {
		ENTL_DEBUG( "ret != ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT\n" ) ;
		return 0 ;
	}

	if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL )  {
		ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL\n" ) ;
		return 0 ;
	}
	if( GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent ) {
		ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->my_value\n" ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, 0, 1, 3, ENTL_STATE_RECEIVE ) ;

	// Same value checking 
	value = mcn->state.event_i_know ;
    d_addr = ENTL_ETYPE_ENTL | value ;
    p_sent = mcn->state.event_i_sent ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

	if( ret != ENTL_ACTION_NOP ) {
		ENTL_DEBUG( "ret != ENTL_ACTION_NOP \n" ) ;
		return 0 ;
	}
	ret = expect_state( &mcn->state, value, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_RECEIVE ) ;


	for( i = 0 ; i < 2 ; i++ ) {
		sprintf( testName, "loop %d", i) ;
		value = ECLP_VALUE_NEXT(mcn->state.event_i_sent) ;
    	d_addr = ENTL_ETYPE_ENTL | value ;
    	p_sent = mcn->state.event_i_sent ;
		ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

		if( ret != ENTL_ACTION_SEND ) {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "ret != ENTL_ACTION_SEND on %s\n", testName ) ;
			return 0 ;
		}

		ret = expect_state( &mcn->state, value, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_SEND ) ;
		if( !ret ) {
			dump_state( testName, mcn ) ;
			return 0 ;
		}
		ret = entl_next_send( mcn, &d_addr, &d_value, 0, 0 ) ; 

		if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT) ) {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "ret != ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT\n" ) ;
			return 0 ;
		}

		if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL )  {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL\n" ) ;
			return 0 ;
		}
		if( GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent ) {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent\n" ) ;
			return 0 ;
		}

		ret = expect_state( &mcn->state, value, ECLP_VALUE_NEXT(value), ECLP_VALUE_PLUS2(ECLP_VALUE_NEXT(value)), ENTL_STATE_RECEIVE ) ;

		if( !ret ) {
			dump_state( testName, mcn ) ;
			return 0 ;
		}
	}
	sprintf( testName, "entl_alo_receive 3" ) ;

	mcn->ao.reg[0] = 500 ;

	value = ECLP_VALUE_NEXT(mcn->state.event_i_sent) ;
	d_addr = ENTL_ETYPE_AITS | value | ((ALO_WR | ALO_COND_GT) << 16);
	p_sent = mcn->state.event_i_sent ;
	ret = entl_received( mcn, d_addr, 100, 0, 0, 0 ) ; // AIT queue has one

	if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_ALO_T) ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "ret %x != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_ALO_T) on %s\n", ret, testName ) ;
		return 0 ;
	}

	if( mcn->ao.reg[0] != 500 ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "mcn->ao.reg[0] != 100 on %s\n", testName ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, value, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_SBL ) ;
	if( !ret ) {
		dump_state( testName, mcn ) ;
		return 0 ;
	}

	sprintf( testName, "entl_alo_receive 4" ) ;

	ret = entl_next_send( mcn, &d_addr, &d_value, 1, 0 ) ; // command to set true

	if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_ALO) ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "ret %x != ENTL_ACTION_SEND | ENTL_ACTION_SEND_ALO on %s\n", ret, testName ) ;
		return 0 ;
	}

	if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_AITR )  {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_AITR\n" ) ;
		return 0 ;
	}
	if( GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent\n" ) ;
		return 0 ;
	}
	if( ALO_OPCODE(d_addr) != 1 ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "ALO_OPCODE(d_addr) != 1\n" ) ;
		return 0 ;
	}
	ret = expect_state( &mcn->state, value, ECLP_VALUE_NEXT(value), ECLP_VALUE_PLUS2(ECLP_VALUE_NEXT(value)), ENTL_STATE_RBL ) ;

	if( !ret ) {
		dump_state( testName, mcn ) ;
		return 0 ;
	}

	sprintf( testName, "entl_alo_receive 5" ) ;

	value = ECLP_VALUE_NEXT(mcn->state.event_i_sent) ;
	d_addr = ENTL_ETYPE_AITA | value | 0x10000 ; // set true flag
	p_sent = mcn->state.event_i_sent ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ; 

	if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SIG_AIT) ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "ret %x != ENTL_ACTION_SEND | ENTL_ACTION_SIG_AIT %x on %s\n", ret, (ENTL_ACTION_SEND | ENTL_ACTION_SIG_AIT), testName ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, value, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_SEND ) ;
	if( !ret ) {
		dump_state( testName, mcn ) ;
		return 0 ;
	}
	if( mcn->ao.reg[0] != 100 ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "mcn->ao.reg[0] != 100 on %s\n", testName ) ;
		return 0 ;
	}

	sprintf( testName, "entl_alo_receive 6" ) ;

	ret = entl_next_send( mcn, &d_addr, &d_value, 0, 0 ) ; // command to set false

	if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT) ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "ret %x != ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT on %s\n", ret, testName ) ;
		return 0 ;
	}
	ret = expect_state( &mcn->state, value, ECLP_VALUE_NEXT(value), ECLP_VALUE_PLUS2(ECLP_VALUE_NEXT(value)), ENTL_STATE_RECEIVE ) ;

	if( !ret ) {
		dump_state( testName, mcn ) ;
		return 0 ;
	}
	if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL )  {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL\n" ) ;
		return 0 ;
	}

	// reject case
	sprintf( testName, "entl_alo_receive 7" ) ;

	mcn->ao.reg[0] = 100 ;

	value = ECLP_VALUE_NEXT(mcn->state.event_i_sent) ;
	d_addr = ENTL_ETYPE_AITS | value | ((ALO_WR | ALO_COND_GT) << 16);
	p_sent = mcn->state.event_i_sent ;
	ret = entl_received( mcn, d_addr, 100, 0, 0, 0 ) ; // AIT queue has one

	if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_ALO_F) ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "ret %x != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_ALO_F) %lx on %s\n", ret, d_addr, testName ) ;
		return 0 ;
	}

	if( mcn->ao.reg[0] != 100 ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "mcn->ao.reg[0] != 100 on %s\n", testName ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, value, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_SBL ) ;
	if( !ret ) {
		dump_state( testName, mcn ) ;
		return 0 ;
	}

	sprintf( testName, "entl_alo_receive 8" ) ;

	ret = entl_next_send( mcn, &d_addr, &d_value, 0, 0 ) ; // command to set false

	if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_ALO) ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "ret %x != ENTL_ACTION_SEND | ENTL_ACTION_SEND_ALO on %s\n", ret, testName ) ;
		return 0 ;
	}

	if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_AITR )  {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_AITR\n" ) ;
		return 0 ;
	}
	if( GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent\n" ) ;
		return 0 ;
	}
	if( ALO_OPCODE(d_addr) != 0 ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "ALO_OPCODE(d_addr) != 0\n" ) ;
		return 0 ;
	}
	ret = expect_state( &mcn->state, value, ECLP_VALUE_NEXT(value), ECLP_VALUE_PLUS2(ECLP_VALUE_NEXT(value)), ENTL_STATE_RBL ) ;

	if( !ret ) {
		dump_state( testName, mcn ) ;
		return 0 ;
	}

	sprintf( testName, "entl_alo_receive 9" ) ;

	value = ECLP_VALUE_NEXT(mcn->state.event_i_sent) ;
	d_addr = ENTL_ETYPE_AITA | value ; // set true flag
	p_sent = mcn->state.event_i_sent ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ; 

	if( ret != (ENTL_ACTION_SEND) ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "ret %x != ENTL_ACTION_SEND %x on %s\n", ret, (ENTL_ACTION_SEND | ENTL_ACTION_SIG_AIT), testName ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, value, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_SEND ) ;
	if( !ret ) {
		dump_state( testName, mcn ) ;
		return 0 ;
	}
	if( mcn->ao.reg[0] != 100 ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "mcn->ao.reg[0] != 100 on %s\n", testName ) ;
		return 0 ;
	}


	return ret ;

}


int hello_2_hello_same_value( entl_state_machine_t *mcn ) 
{
	uint32_t value ;
	uint64_t d_addr ;
	uint64_t d_value ;
	int ret ;

	//debug_flag = 1 ;
    //entl_set_debug() ;


	mcn->state.current_state = ENTL_STATE_HELLO ;
	mcn->state.event_i_know = 0 ;
	mcn->state.event_i_sent = 0 ;
	mcn->state.event_send_next = 0 ;
    mcn->my_value = mt_urandom(0) & 0x7fff ;
    value = mcn->my_value ;

    d_addr = ENTL_ETYPE_HELLO | value ;

    // int entl_received( __lmem entl_state_machine_t *mcn, uint64_t d_addr, uint64_t s_value, uint64_t *d_value, uint32_t ait_queue, uint32_t ait_command, uint32_t egress_queue ) ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

	dump_state( "hello_2_hello_same_value 1", mcn ) ;

	ENTL_DEBUG( "hello_2_hello_same_value 1 ret = %d\n", ret ) ;

	if( ret != ENTL_ACTION_SEND ) return 0 ;

	if( value == mcn->my_value ) {
	    dump_state( "hello_2_hello_same_value 2", mcn ) ;
		ENTL_DEBUG( " value %x == mcn->my_value\n", value ) ;
	}

	ret = expect_state( &mcn->state, 0, 0, 0, ENTL_STATE_HELLO ) ;
	if( !ret ) return 0 ;

	// int entl_next_send( __lmem entl_state_machine_t *mcn, uint64_t *addr, uint64_t *alo_data, uint32_t alo_command, uint32_t ait_queue) 
	ret = entl_next_send( mcn, &d_addr, &d_value, 0, 0 ) ; 

	dump_state( "hello_2_hello_same_value 3", mcn ) ;
	ENTL_DEBUG( "hello_2_hello_same_value 3 ret = %d\n", ret ) ;

	if( ret != ENTL_ACTION_SEND ) {
		ENTL_DEBUG( "ret != ENTL_ACTION_SEND\n" ) ;
		return 0 ;
	}

	if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_HELLO )  {
		ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL\n" ) ;
		return 0 ;
	}
	if( GET_ECLP_VALUE(d_addr) != mcn->my_value )  {
		ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->my_value\n" ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, 0, 0, 0, ENTL_STATE_HELLO ) ;

	return ret ;

}

int hello_error_hello( entl_state_machine_t *mcn )
{
	uint64_t d_addr ;
	uint64_t d_value ;
	int ret ;

	//debug_flag = 1 ;
    //entl_set_debug() ;

	mcn->state.current_state = ENTL_STATE_HELLO ;
	mcn->state.event_i_know = 0 ;
	mcn->state.event_i_sent = 0 ;
	mcn->state.event_send_next = 0 ;
    mcn->my_value = mt_urandom(0) & 0x7fff ;

    d_addr = ENTL_ETYPE_ENTL | 1 ;  // non zero value

    // int entl_received( __lmem entl_state_machine_t *mcn, uint64_t d_addr, uint64_t s_value, uint64_t *d_value, uint32_t ait_queue, uint32_t ait_command, uint32_t egress_queue ) ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

	dump_state( "hello_error_hello 1", mcn ) ;

	ENTL_DEBUG( "hello_error_hello 1 ret = %d\n", ret ) ;

	if( ret != ENTL_ACTION_SEND ) return 0 ;

	ret = expect_state( &mcn->state, 0, 0, 0, ENTL_STATE_HELLO ) ;
	if( !ret ) return 0 ;

	// int entl_next_send( __lmem entl_state_machine_t *mcn, uint64_t *addr, uint64_t *alo_data, uint32_t alo_command, uint32_t ait_queue) 
	ret = entl_next_send( mcn, &d_addr, &d_value, 0, 0 ) ; 

	dump_state( "hello_error_hello 2", mcn ) ;
	ENTL_DEBUG( "hello_error_hello 2 ret = %x d_addr %lx\n", ret, d_addr ) ;

	if( ret != (ENTL_ACTION_SEND) ) {
		ENTL_DEBUG( "ret != ENTL_ACTION_SEND \n" ) ;
		return 0 ;
	}

	if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_HELLO )  {
		ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL\n" ) ;
		return 0 ;
	}
	if( GET_ECLP_VALUE(d_addr) != mcn->my_value ) {
		ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->my_value\n" ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, 0, 0, 0, ENTL_STATE_HELLO ) ;

	return ret ;

}

int hello_2_entl_error( entl_state_machine_t *mcn )
{
	uint64_t d_addr ;
	uint64_t d_value ;
	int ret ;

	//debug_flag = 1 ;
    //entl_set_debug() ;

	mcn->state.current_state = ENTL_STATE_HELLO ;
	mcn->state.event_i_know = 0 ;
	mcn->state.event_i_sent = 0 ;
	mcn->state.event_send_next = 0 ;
    mcn->my_value = mt_urandom(0) & 0x7fff ;

    d_addr = ENTL_ETYPE_ENTL ;

    // int entl_received( __lmem entl_state_machine_t *mcn, uint64_t d_addr, uint64_t s_value, uint64_t *d_value, uint32_t ait_queue, uint32_t ait_command, uint32_t egress_queue ) ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

	dump_state( "hello_2_entl_error 1", mcn ) ;

	ENTL_DEBUG( "hello_2_entl_error 1 ret = %d\n", ret ) ;

	if( ret != ENTL_ACTION_SEND ) return 0 ;

	ret = expect_state( &mcn->state, 0, 0, 1, ENTL_STATE_SEND ) ;
	if( !ret ) return 0 ;

	// int entl_next_send( __lmem entl_state_machine_t *mcn, uint64_t *addr, uint64_t *alo_data, uint32_t alo_command, uint32_t ait_queue) 
	ret = entl_next_send( mcn, &d_addr, &d_value, 0, 0 ) ; 

	dump_state( "hello_2_entl_error 2", mcn ) ;
	ENTL_DEBUG( "hello_2_entl_error 2 ret = %x d_addr %lx\n", ret, d_addr ) ;

	if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT) ) {
		ENTL_DEBUG( "ret != ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT\n" ) ;
		return 0 ;
	}

	if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL )  {
		ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL\n" ) ;
		return 0 ;
	}
	if( GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent ) {
		ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->my_value\n" ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, 0, 1, 3, ENTL_STATE_RECEIVE ) ;


    d_addr = ENTL_ETYPE_ENTL | 1 ; // sequence Error

    // int entl_received( __lmem entl_state_machine_t *mcn, uint64_t d_addr, uint64_t s_value, uint64_t *d_value, uint32_t ait_queue, uint32_t ait_command, uint32_t egress_queue ) ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

	dump_state( "hello_2_entl_error 3", mcn ) ;

	ENTL_DEBUG( "hello_2_entl_error 4 ret = %d\n", ret ) ;

	if( ret != ENTL_ACTION_ERROR ) return 0 ;

	ret = expect_state( &mcn->state, 0, 0, 0, ENTL_STATE_HELLO ) ;

	ret = expect_state( &mcn->error_state, 0, 1, 3, ENTL_STATE_RECEIVE ) ;

	return ret ;

}

int hello_2_wait_2_entl_error( entl_state_machine_t *mcn ) 
{
	uint32_t value ;
	uint64_t d_addr ;
	uint64_t d_value ;
	int ret ;

	//debug_flag = 1 ;
    //entl_set_debug() ;

	mcn->state.current_state = ENTL_STATE_HELLO ;
	mcn->state.event_i_know = 0 ;
	mcn->state.event_i_sent = 0 ;
	mcn->state.event_send_next = 0 ;
    mcn->my_value = mt_urandom(0) & 0x7fff ;
    value = mcn->my_value - 1 ;

    d_addr = ENTL_ETYPE_HELLO | value ;

    // int entl_received( __lmem entl_state_machine_t *mcn, uint64_t d_addr, uint64_t s_value, uint64_t *d_value, uint32_t ait_queue, uint32_t ait_command, uint32_t egress_queue ) ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

	dump_state( "hello_2_wait_2_entl_error 1", mcn ) ;

	ENTL_DEBUG( "hello_2_wait_2_entl_error 1 ret = %d\n", ret ) ;

	if( ret != ENTL_ACTION_SEND ) return 0 ;

	ret = expect_state( &mcn->state, 0, 0, 0, ENTL_STATE_WAIT ) ;
	if( !ret ) return 0 ;

	// int entl_next_send( __lmem entl_state_machine_t *mcn, uint64_t *addr, uint64_t *alo_data, uint32_t alo_command, uint32_t ait_queue) 
	ret = entl_next_send( mcn, &d_addr, &d_value, 0, 0 ) ; 

	dump_state( "hello_2_wait_2_entl_error 2", mcn ) ;
	ENTL_DEBUG( "hello_2_wait_2_entl_error 2 ret = %d\n", ret ) ;

	if( ret != ENTL_ACTION_SEND ) {
		ENTL_DEBUG( "ret != ENTL_ACTION_SEND\n" ) ;
		return 0 ;
	}

	if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL )  {
		ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL\n" ) ;
		return 0 ;
	}
	if( GET_ECLP_VALUE(d_addr) != 0 )  {
		ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != 0\n" ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, 0, 0, 0, ENTL_STATE_SEND ) ;

    d_addr = ENTL_ETYPE_ENTL | 1 ;

    // int entl_received( __lmem entl_state_machine_t *mcn, uint64_t d_addr, uint64_t s_value, uint64_t *d_value, uint32_t ait_queue, uint32_t ait_command, uint32_t egress_queue ) ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

	dump_state( "hello_2_wait_2_entl_error 3", mcn ) ;

	ENTL_DEBUG( "hello_2_wait_2_entl_error 3 ret = %x d_addr %lx \n", ret, d_addr ) ;


	if( ret != ENTL_ACTION_SEND ) {
		ENTL_DEBUG( "ret != ENTL_ACTION_SEND \n" ) ;
		return 0 ;
	}

	ret = entl_next_send( mcn, &d_addr, &d_value, 0, 0 ) ; 

	dump_state( "hello_2_wait_2_entl_error 4", mcn ) ;
	ENTL_DEBUG( "hello_2_wait_2_entl_error 4 ret = %x d_addr %lx\n", ret, d_addr ) ;

	if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT) ) {
		ENTL_DEBUG( "ret != ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT\n" ) ;
		return 0 ;
	}

	if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL )  {
		ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL\n" ) ;
		return 0 ;
	}
	if( GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent ) {
		ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->my_value\n" ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, 1, 2, 4, ENTL_STATE_RECEIVE ) ;

	if( !ret ) return 0 ;


    d_addr = ENTL_ETYPE_ENTL | 2 ; // sequence Error

    // int entl_received( __lmem entl_state_machine_t *mcn, uint64_t d_addr, uint64_t s_value, uint64_t *d_value, uint32_t ait_queue, uint32_t ait_command, uint32_t egress_queue ) ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

	dump_state( "hello_2_wait_2_entl_error 5", mcn ) ;

	ENTL_DEBUG( "hello_2_wait_2_entl_error 5 ret = %d\n", ret ) ;

	if( ret != ENTL_ACTION_ERROR ) return 0 ;

	ret = expect_state( &mcn->state, 0, 0, 0, ENTL_STATE_HELLO ) ;

	ret = expect_state( &mcn->error_state, 1, 2, 4, ENTL_STATE_RECEIVE ) ;

	return ret ;

}

int entl_ait_send_error( entl_state_machine_t *mcn ) 
{
	uint64_t d_addr ;
	uint64_t d_value ;
	uint32_t value, value1, p_sent ;
	int ret ;
	int i ;
	char testName[256] ;

	//debug_flag = 1 ;
    //entl_set_debug() ;

	mcn->state.current_state = ENTL_STATE_HELLO ;
	mcn->state.event_i_know = 0 ;
	mcn->state.event_i_sent = 0 ;
	mcn->state.event_send_next = 0 ;
    mcn->my_value = mt_urandom(0) & 0x7fff ;

    d_addr = ENTL_ETYPE_ENTL ;

    // int entl_received( __lmem entl_state_machine_t *mcn, uint64_t d_addr, uint64_t s_value, uint64_t *d_value, uint32_t ait_queue, uint32_t ait_command, uint32_t egress_queue ) ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

	dump_state( "entl_ait_send_error 1", mcn ) ;

	ENTL_DEBUG( "entl_ait_send_error 1 ret = %d\n", ret ) ;

	if( ret != ENTL_ACTION_SEND ) return 0 ;

	ret = expect_state( &mcn->state, 0, 0, 1, ENTL_STATE_SEND ) ;
	if( !ret ) return 0 ;

	// int entl_next_send( __lmem entl_state_machine_t *mcn, uint64_t *addr, uint64_t *alo_data, uint32_t alo_command, uint32_t ait_queue) 
	ret = entl_next_send( mcn, &d_addr, &d_value, 0, 0 ) ; 

	dump_state( "entl_ait_send_error 2", mcn ) ;
	ENTL_DEBUG( "entl_ait_send_error 2 ret = %x d_addr %lx\n", ret, d_addr ) ;

	if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT) ) {
		ENTL_DEBUG( "ret != ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT\n" ) ;
		return 0 ;
	}

	if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL )  {
		ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL\n" ) ;
		return 0 ;
	}
	if( GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent ) {
		ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->my_value\n" ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, 0, 1, 3, ENTL_STATE_RECEIVE ) ;

	// Same value checking 
	value = mcn->state.event_i_know ;
    d_addr = ENTL_ETYPE_ENTL | value ;
    p_sent = mcn->state.event_i_sent ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

	if( ret != ENTL_ACTION_NOP ) {
		ENTL_DEBUG( "ret != ENTL_ACTION_NOP \n" ) ;
		return 0 ;
	}
	ret = expect_state( &mcn->state, value, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_RECEIVE ) ;


	for( i = 0 ; i < 2 ; i++ ) {
		sprintf( testName, "loop %d", i) ;
		value = ECLP_VALUE_NEXT(mcn->state.event_i_sent) ;
    	d_addr = ENTL_ETYPE_ENTL | value ;
    	p_sent = mcn->state.event_i_sent ;
		ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

		if( ret != ENTL_ACTION_SEND ) {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "ret != ENTL_ACTION_SEND on %s\n", testName ) ;
			return 0 ;
		}

		ret = expect_state( &mcn->state, value, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_SEND ) ;
		if( !ret ) {
			dump_state( testName, mcn ) ;
			return 0 ;
		}
		ret = entl_next_send( mcn, &d_addr, &d_value, 0, 0 ) ; 

		if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT) ) {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "ret != ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT\n" ) ;
			return 0 ;
		}

		if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL )  {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL\n" ) ;
			return 0 ;
		}
		if( GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent ) {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent\n" ) ;
			return 0 ;
		}

		ret = expect_state( &mcn->state, value, ECLP_VALUE_NEXT(value), ECLP_VALUE_PLUS2(ECLP_VALUE_NEXT(value)), ENTL_STATE_RECEIVE ) ;

		if( !ret ) {
			dump_state( testName, mcn ) ;
			return 0 ;
		}
	}
	sprintf( testName, "entl_ait_send_error 1" ) ;

	value = ECLP_VALUE_NEXT(mcn->state.event_i_sent) ;
	d_addr = ENTL_ETYPE_ENTL | value ;
	p_sent = mcn->state.event_i_sent ;
	ret = entl_received( mcn, d_addr, 0, 1, 0, 0 ) ; // AIT queue has one

	if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_AIT) ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_AIT) on %s\n", testName ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, value, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_SEND ) ;
	if( !ret ) {
		dump_state( testName, mcn ) ;
		return 0 ;
	}
	ret = entl_next_send( mcn, &d_addr, &d_value, 0, 1 ) ; // AIT queue has one

	if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_AIT) ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "ret != ENTL_ACTION_SEND | ENTL_ACTION_SEND_AIT\n" ) ;
		return 0 ;
	}

	if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_AITS )  {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_AITS\n" ) ;
		return 0 ;
	}
	if( GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent\n" ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, value, ECLP_VALUE_NEXT(value), ECLP_VALUE_PLUS2(ECLP_VALUE_NEXT(value)), ENTL_STATE_RA ) ;

	if( !ret ) {
		dump_state( testName, mcn ) ;
		return 0 ;
	}

	sprintf( testName, "entl_ait_send_error 2" ) ;

	value = ECLP_VALUE_PLUS2(mcn->state.event_i_sent) ; // wrong sequense
	value1 = mcn->state.event_i_know ;
	d_addr = ENTL_ETYPE_AITR | value ;
	p_sent = mcn->state.event_i_sent ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ; 

	if( ret != ENTL_ACTION_ERROR  ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "ret != ENTL_ACTION_ERROR  on %s\n", testName ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, 0, 0, 0, ENTL_STATE_HELLO ) ;
	if( !ret ) {
		dump_state( testName, mcn ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->error_state, value1, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_RA ) ;
	if( !ret ) {
		dump_state( testName, mcn ) ;
		return 0 ;
	}

}

int entl_ait_send_comm_error( entl_state_machine_t *mcn ) 
{
	uint64_t d_addr ;
	uint64_t d_value ;
	uint32_t value, value1, p_sent ;
	int ret ;
	int i ;
	char testName[256] ;

	debug_flag = 1 ;
    entl_set_debug() ;

	mcn->state.current_state = ENTL_STATE_HELLO ;
	mcn->state.event_i_know = 0 ;
	mcn->state.event_i_sent = 0 ;
	mcn->state.event_send_next = 0 ;
    mcn->my_value = mt_urandom(0) & 0x7fff ;

    d_addr = ENTL_ETYPE_ENTL ;

    // int entl_received( __lmem entl_state_machine_t *mcn, uint64_t d_addr, uint64_t s_value, uint64_t *d_value, uint32_t ait_queue, uint32_t ait_command, uint32_t egress_queue ) ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

	dump_state( "entl_ait_send_error 1", mcn ) ;

	ENTL_DEBUG( "entl_ait_send_error 1 ret = %d\n", ret ) ;

	if( ret != ENTL_ACTION_SEND ) return 0 ;

	ret = expect_state( &mcn->state, 0, 0, 1, ENTL_STATE_SEND ) ;
	if( !ret ) return 0 ;

	// int entl_next_send( __lmem entl_state_machine_t *mcn, uint64_t *addr, uint64_t *alo_data, uint32_t alo_command, uint32_t ait_queue) 
	ret = entl_next_send( mcn, &d_addr, &d_value, 0, 0 ) ; 

	dump_state( "entl_ait_send_error 2", mcn ) ;
	ENTL_DEBUG( "entl_ait_send_error 2 ret = %x d_addr %lx\n", ret, d_addr ) ;

	if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT) ) {
		ENTL_DEBUG( "ret != ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT\n" ) ;
		return 0 ;
	}

	if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL )  {
		ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL\n" ) ;
		return 0 ;
	}
	if( GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent ) {
		ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->my_value\n" ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, 0, 1, 3, ENTL_STATE_RECEIVE ) ;

	// Same value checking 
	value = mcn->state.event_i_know ;
    d_addr = ENTL_ETYPE_ENTL | value ;
    p_sent = mcn->state.event_i_sent ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

	if( ret != ENTL_ACTION_NOP ) {
		ENTL_DEBUG( "ret != ENTL_ACTION_NOP \n" ) ;
		return 0 ;
	}
	ret = expect_state( &mcn->state, value, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_RECEIVE ) ;


	for( i = 0 ; i < 2 ; i++ ) {
		sprintf( testName, "loop %d", i) ;
		value = ECLP_VALUE_NEXT(mcn->state.event_i_sent) ;
    	d_addr = ENTL_ETYPE_ENTL | value ;
    	p_sent = mcn->state.event_i_sent ;
		ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ;

		if( ret != ENTL_ACTION_SEND ) {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "ret != ENTL_ACTION_SEND on %s\n", testName ) ;
			return 0 ;
		}

		ret = expect_state( &mcn->state, value, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_SEND ) ;
		if( !ret ) {
			dump_state( testName, mcn ) ;
			return 0 ;
		}
		ret = entl_next_send( mcn, &d_addr, &d_value, 0, 0 ) ; 

		if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT) ) {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "ret != ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT\n" ) ;
			return 0 ;
		}

		if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL )  {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_ENTL\n" ) ;
			return 0 ;
		}
		if( GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent ) {
			dump_state( testName, mcn ) ;
			ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent\n" ) ;
			return 0 ;
		}

		ret = expect_state( &mcn->state, value, ECLP_VALUE_NEXT(value), ECLP_VALUE_PLUS2(ECLP_VALUE_NEXT(value)), ENTL_STATE_RECEIVE ) ;

		if( !ret ) {
			dump_state( testName, mcn ) ;
			return 0 ;
		}
	}
	sprintf( testName, "entl_ait_send_error 1" ) ;

	value = ECLP_VALUE_NEXT(mcn->state.event_i_sent) ;
	d_addr = ENTL_ETYPE_ENTL | value ;
	p_sent = mcn->state.event_i_sent ;
	ret = entl_received( mcn, d_addr, 0, 1, 0, 0 ) ; // AIT queue has one

	if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_AIT) ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_AIT) on %s\n", testName ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, value, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_SEND ) ;
	if( !ret ) {
		dump_state( testName, mcn ) ;
		return 0 ;
	}
	ret = entl_next_send( mcn, &d_addr, &d_value, 0, 1 ) ; // AIT queue has one

	if( ret != (ENTL_ACTION_SEND | ENTL_ACTION_SEND_AIT) ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "ret != ENTL_ACTION_SEND | ENTL_ACTION_SEND_AIT\n" ) ;
		return 0 ;
	}

	if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_AITS )  {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_AITS\n" ) ;
		return 0 ;
	}
	if( GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "GET_ECLP_VALUE(d_addr) != mcn->state.event_i_sent\n" ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, value, ECLP_VALUE_NEXT(value), ECLP_VALUE_PLUS2(ECLP_VALUE_NEXT(value)), ENTL_STATE_RA ) ;

	if( !ret ) {
		dump_state( testName, mcn ) ;
		return 0 ;
	}

	sprintf( testName, "entl_ait_send_error 2" ) ;

	value = ECLP_VALUE_NEXT(mcn->state.event_i_sent) ; // good sequense
	value1 = mcn->state.event_i_know ;
	d_addr = ENTL_ETYPE_AITS | value ;  // wrong command
	p_sent = mcn->state.event_i_sent ;
	ret = entl_received( mcn, d_addr, 0, 0, 0, 0 ) ; 

	if( ret != ENTL_ACTION_ERROR  ) {
		dump_state( testName, mcn ) ;
		ENTL_DEBUG( "ret != ENTL_ACTION_ERROR  on %s\n", testName ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->state, 0, 0, 0, ENTL_STATE_HELLO ) ;
	if( !ret ) {
		dump_state( testName, mcn ) ;
		return 0 ;
	}

	ret = expect_state( &mcn->error_state, value1, p_sent, ECLP_VALUE_PLUS2(p_sent), ENTL_STATE_RA ) ;
	if( !ret ) {
		dump_state( testName, mcn ) ;
		return 0 ;
	}

}
