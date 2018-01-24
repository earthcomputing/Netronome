/*
 * Copyright (C) 2017,  Earth Computing Inc.  All rights reserved.
 *
 *
 * @file          entl_state_machine.c
 * @brief         ENTL state machine 
 *
 * Author:        Atsushi Kasuya
 *
 */


//#ifndef _PKT_COUNT_C_
//#define _PKT_COUNT_C_

#ifdef NETRONOME_HOST
#include <stdio.h>

static debug_flag = 1 ;

#else

#include <nfp.h>
#include <stdint.h>
#include <stdlib.h>

#include <pkt/pkt.h>

#include <net/csum.h>
#include <net/eth.h>
#include <net/ip.h>
#include <net/tcp.h>
#include <net/udp.h>
#include <net/hdr_ext.h>
#include <nfp/mem_atomic.h>
#include <nfp/mem_bulk.h>
#include <nfp6000/nfp_mac.h>
#include <std/reg_utils.h>

#endif  // NETRONOME_HOST

#include "entl_state_machine_2.h"

__lmem entl_state_machine_t m_mcn[2] ;

__lmem entl_state_machine_t *mcn get_entl_state_machine( int index ) 
{
    return &m_mcn[index] ;
}

void entl_set_debug() 
{
#ifdef NETRONOME_HOST

    debug_flag = 1 ;

#endif

}

void entl_state_init_all() 
{
    int i ;
    for( i = 0 ; i < 2 ; i++) {
        entl_state_init(&m_mcn[i]) ;
        m_mcn[i].smc_id = i ;
        m_mcn[i].monitor_value = 0 ;
        m_mcn[i].monitor_count = 0 ;
    }
}

void entl_state_init( __lmem entl_state_machine_t *mcn ) 
{
    mcn->error_flag = 0 ;
    mcn->p_error_flag = 0 ;
    mcn->error_count = 0 ;
    mcn->state_count = 0 ;
    mcn->state.event_i_know = 0 ;
    mcn->state.event_i_sent = 0 ;
    mcn->state.event_send_next = 0 ;
    mcn->state.current_state = 0 ;
    mcn->state_count = 0 ;
    mcn->error_state.event_i_know = 0 ;
    mcn->error_state.event_i_sent = 0 ;
    mcn->error_state.event_send_next = 0 ;
    mcn->error_state.current_state = 0 ;
    entl_set_random( mcn ) ;
}

#ifndef NETRONOME_HOST
static uint32_t l_rand()
{
    uint32_t ret ;
    ret = rand() ;
    //ret = local_csr_read(local_csr_pseudo_random_number) ;
    return ret ;
}
#endif

void entl_set_random( __lmem entl_state_machine_t *mcn ) 
{
    // uint64_t  addr = l_rand() ;
#ifdef NETRONOME_HOST
    mcn->my_value = mt_urandom(0) & 0xffff ;
#else
    mcn->my_value = 0xbeef ; // l_rand()) & 0xffff ;
#endif
}

#ifdef NETRONOME_HOST
#define ENTL_DEBUG(fmt, args...) if(debug_flag) printf( fmt, ## args )
#endif

void entl_state_hello( __lmem entl_state_machine_t *mcn ) 
{
    //int i ;
    mcn->state_count = 0 ;
    mcn->state.event_i_know = 0 ;
    mcn->state.event_i_sent = 0 ;
    mcn->state.event_send_next = 0 ;
    mcn->state.current_state = ENTL_STATE_HELLO ;
    alo_regs_init( &mcn->ao ) ;
    mcn->credit = 0 ;

    entl_set_random(mcn) ;
}

static void set_error(__lmem entl_state_machine_t *mcn, int flag )
{
    if( mcn->error_count ) {
        mcn->p_error_flag |= flag ;
        mcn->error_count++ ;
    }
    else {
        mcn->error_flag = flag ;
        mcn->error_count++ ;
        mcn->error_state.event_i_know = mcn->state.event_i_know ;
        mcn->error_state.event_i_sent = mcn->state.event_i_sent ;
        mcn->error_state.event_send_next = mcn->state.event_send_next ;
        mcn->error_state.current_state = mcn->state.current_state ;
    }
}

static int is_ENTT_queue_full( int queue_size ) {
    return queue_size >= MAX_ENTT_QUEUE_SIZE ;
}

// check other state machines condition
static int monitor_state( __lmem entl_state_machine_t *mcn )
{
    int id = mcn->smc_id?0:1 ;
    int ret_val = 0 ;
    if( (mcn->state.event_i_know & 0xfffffffe ) == ENTL_MONITOR_START_COUNT ){
        // monitor start point
        __lmem entl_state_machine_t *mon = &m_mcn[id] ;
        mcn->monitor_value = mom->state.event_i_know ;
        mcn->monitor_count = 0 ;
        
    }
    else if( mcn->state.event_i_know & 0xfffffffe ) > ENTL_MONITOR_START_COUNT ) {
        __lmem entl_state_machine_t *mon = &m_mcn[id] ;
            if( mcn->monitor_value && mcn->monitor_value == mom->state.event_i_know ) {
                mcn->monitor_count++ ;
                if( mcn->monitor_count > ENTL_MONITOR_ERROR_COUNT ) {
                    ret_val = 1 ; // tell the state machine is stack
                }
            }
            else {
                mcn->monitor_value = mom->state.event_i_know ;
                mcn->monitor_count = 0 ;
            }
        }
    }
    return ret_val ;
}

// trigger detected from monitoring state machine
int entl_state_resend( __lmem entl_state_machine_t *mcn, uint64_t *addr, uint64_t *alo_data )
{
    int ret = 0 ;
    switch( mcn->state.current_state ) {
        case ENTL_STATE_RECEIVE:
        case ENTL_STATE_RA:     // AIT ENTL_ETYPE_AITS sent, waiting for ENTL_ETYPE_AITR
        case ENTL_STATE_RAL:     // ALO ENTL_ETYPE_AITS sent, waiting for ENTL_ETYPE_AITR
        case ENTL_STATE_RB:  // got AIT, Ack sent, waiting for ack
        case ENTL_STATE_RBL:  // got AITS, Ack sent, waiting for ack
        {
            *addr = mcn->p_addr ;
            *alo_data = mcn->p_data ;
            ret = mcn->p_retval ;
        }
        break ;

        default:
        {

        }
        break ;
    }
    return ret ;
}


// On Received message, this should be called with the massage (MAC source & destination addr)
//   return value : bit 0: send, bit 1: send AIT, bit 2: process AIT
int entl_received( uint64_t d_addr, uint64_t s_value, uint32_t ait_queue, uint32_t ait_command, uint32_t egress_queue ) 
{
    int retval = ENTL_ACTION_NOP ;
    int sm_sel = GET_ECLP_SMSEL(d_addr) ;
    if( sm_sel >= ENTL_NUM_STATE_MACINE ) {
        return retval ; // statemachine index overflow
    }
    __lmem entl_state_machine_t *mcn = &m_mcm[sm_sel];
    if( GET_ECLP_ETYPE(d_addr) == ENTL_ETYPE_NOP) {
        if( egress_queue ) retval = ENTL_ACTION_SEND ;
        return retval ;
    }
    switch( mcn->state.current_state ) {

        case ENTL_STATE_IDLE:
        {
            // say something here as receive something on idle state
        }
        break ;
        case ENTL_STATE_HELLO:
        {
            if( GET_ECLP_ETYPE(d_addr) == ENTL_ETYPE_HELLO )
            {
#ifdef NETRONOME_HOST
                ENTL_DEBUG( "Hello message on my_value %x value %x received on hello state \n", mcn->my_value, GET_ECLP_VALUE(d_addr) ) ;
#endif                
                if( mcn->my_value > GET_ECLP_VALUE(d_addr) ) {
                    mcn->state.event_i_sent = mcn->state.event_i_know = mcn->state.event_send_next = 0 ;
                    mcn->state.current_state = ENTL_STATE_WAIT ;        
                    mcn->state_count = 0 ;
                    retval = ENTL_ACTION_SEND ;
                }
                else if( mcn->my_value ==  GET_ECLP_VALUE(d_addr) ) {
                    entl_set_random( mcn ) ;
                    retval = ENTL_ACTION_SEND ;
                }
                else {
                    retval = ENTL_ACTION_SEND ;
                }
            }
            else if( GET_ECLP_ETYPE(d_addr) == ENTL_ETYPE_ENTL ) {
                // Hello state got event
                if( GET_ECLP_VALUE(d_addr) == 0 ) {
                    mcn->state.event_i_know = 0 ;
                    mcn->state.event_send_next = 1 ;
                    mcn->state.current_state = ENTL_STATE_SEND ;
                    retval = ENTL_ACTION_SEND ;
#ifdef NETRONOME_HOST
                    ENTL_DEBUG( "%s ENTL %d message received on Hello -> Send \n", mcn->name, GET_ECLP_VALUE(d_addr) ) ;           
#endif                
                }
                else {
#ifdef NETRONOME_HOST
                    ENTL_DEBUG( "%s Out of sequence ENTL %d message received on Hello \n", mcn->name, GET_ECLP_VALUE(d_addr) ) ;           
#endif                
                    retval = ENTL_ACTION_SEND ;
                }
            }
            else {
                // Received non hello message on Hello state
#ifdef NETRONOME_HOST
                ENTL_DEBUG( "%s non-hello message %lx received on hello state \n", mcn->name, d_addr ) ;
#endif                
                retval = ENTL_ACTION_SEND ;
            }           
        }
        break ;
        case ENTL_STATE_WAIT:
        {
            if( GET_ECLP_ETYPE(d_addr) == ENTL_ETYPE_HELLO )
            {
                mcn->state_count++ ;
                if( mcn->state_count > ENTL_COUNT_MAX ) {
#ifdef NETRONOME_HOST
                    ENTL_DEBUG( "%s Hello message received overflow %d on Wait state -> Hello state\n", mcn->name, mcn->state_count ) ;
#endif
                    entl_state_hello(mcn) ;       
                    retval = ENTL_ACTION_SEND ;
                }
            }
            else if( GET_ECLP_ETYPE(d_addr) == ENTL_ETYPE_ENTL )
            {
                if( GET_ECLP_VALUE(d_addr) == ECLP_VALUE_NEXT(mcn->state.event_i_sent) ) {
                    mcn->state.event_i_know = GET_ECLP_VALUE(d_addr) ;
                    mcn->state.event_send_next = ECLP_VALUE_NEXT(mcn->state.event_i_know) ;
                    mcn->state.current_state = ENTL_STATE_SEND ;            
                    retval = ENTL_ACTION_SEND ;
#ifdef NETRONOME_HOST
                    ENTL_DEBUG( "%s ENTL message %d received on Wait state -> Send state \n", mcn->name, GET_ECLP_VALUE(d_addr) ) ;
#endif
                }
                else {
                    entl_state_hello(mcn) ;   
#ifdef NETRONOME_HOST
                    ENTL_DEBUG( "%s Wrong ENTL message %d received on Wait state -> Hello state \n", mcn->name, GET_ECLP_VALUE(d_addr) ) ;
#endif
                    retval = ENTL_ACTION_SEND ;
                }
            }
            else {
                // Received non hello message on Wait state
#ifdef NETRONOME_HOST
                ENTL_DEBUG( "%s wrong message %lx received on Wait state -> Hello \n", mcn->name, d_addr ) ;          
#endif
                set_error( mcn, ENTL_ERROR_FLAG_SEQUENCE ) ;
                retval = ENTL_ACTION_ERROR ;
                entl_state_hello(mcn) ;
                retval = ENTL_ACTION_SEND ;
            }       
        }
        break ;
        case ENTL_STATE_SEND:
        {
            if( GET_ECLP_ETYPE(d_addr) == ENTL_ETYPE_ENTL ) {
                if( GET_ECLP_VALUE(d_addr) == mcn->state.event_i_know ) {
#ifdef NETRONOME_HOST
                    ENTL_DEBUG( "%s Same ENTL message %d received on Send state\n", mcn->name, GET_ECLP_VALUE(d_addr) ) ;
#endif
                }
                else {
                    set_error( mcn, ENTL_ERROR_FLAG_SEQUENCE ) ;
                    retval = ENTL_ACTION_ERROR ;
#ifdef NETRONOME_HOST
                    ENTL_DEBUG( "%s Out of Sequence ENTL %d received on Send state -> Hello\n", mcn->name, GET_ECLP_VALUE(d_addr) ) ;
#endif
                }
            }
            else {
                set_error( mcn, ENTL_ERROR_FLAG_SEQUENCE ) ;
                retval = ENTL_ACTION_ERROR ;
#ifdef NETRONOME_HOST
                ENTL_DEBUG( "%s wrong message %lx received on Send state -> Hello\n", mcn->name, d_addr ) ;
#endif
                entl_state_hello(mcn) ;
            }
        }
        break ;
        case ENTL_STATE_RECEIVE:
        {
            if( GET_ECLP_ETYPE(d_addr) == ENTL_ETYPE_ENTL ) {
                if( ECLP_VALUE_PLUS2(mcn->state.event_i_know) == GET_ECLP_VALUE(d_addr) ) {
                    mcn->state.event_i_know = GET_ECLP_VALUE(d_addr) ;
                    mcn->state.event_send_next = ECLP_VALUE_NEXT(mcn->state.event_i_know) ;
                    mcn->state.current_state = ENTL_STATE_SEND ;
                    if( ait_queue ) {  // AIT has priority
                        retval = ENTL_ACTION_SEND | ENTL_ACTION_SEND_AIT ;
                    }
                    else {
                        if( egress_queue ) retval = ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT ;
                        else retval = ENTL_ACTION_SEND ; 
                    }
                    if( monitor_state(mcn) ) {
                        retval |= ENTL_ACTION_TRIGGERD ;
                    }
#ifdef NETRONOME_HOST
                    ENTL_DEBUG( "%s ETL message %d received on Receive -> Send \n", mcn->name, mcn->state.event_i_know ) ;            
#endif
                }
                else if( mcn->state.event_i_know == GET_ECLP_VALUE(d_addr) )
                {
#ifdef NETRONOME_HOST
                    ENTL_DEBUG( "%s same ETL message %d received on Receive\n", mcn->name, mcn->state.event_i_know ) ;         
#endif
                }
                else {
                    set_error( mcn, ENTL_ERROR_FLAG_SEQUENCE ) ;
                    retval = ENTL_ACTION_ERROR ;
#ifdef NETRONOME_HOST
                    ENTL_DEBUG( "%s Out of Sequence ETL message %d received on Receive -> Hello \n", mcn->name, GET_ECLP_VALUE(d_addr) ) ;
#endif
                    entl_state_hello(mcn) ;
                }
            }
            else if( GET_ECLP_ETYPE(d_addr) == ENTL_ETYPE_AITS ) {
                if( ECLP_VALUE_PLUS2(mcn->state.event_i_know) == GET_ECLP_VALUE(d_addr) ) {
                    mcn->state.event_i_know = GET_ECLP_VALUE(d_addr) ;
                    mcn->state.event_send_next = ECLP_VALUE_NEXT(mcn->state.event_i_know) ;
                    if( ALO_OPCODE(d_addr) == ALO_NOP ) {
                        if( is_ENTT_queue_full( ait_queue ) ) {
#ifdef NETRONOME_HOST
                            ENTL_DEBUG( "%s AIT message %d received on Receive with queue full -> Send\n", mcn->name, GET_ECLP_VALUE(d_addr) ) ;            
#endif
                            mcn->state.current_state = ENTL_STATE_SEND ;
                            retval = ENTL_ACTION_SEND  ;
                        }
                        else {
#ifdef NETRONOME_HOST
                            ENTL_DEBUG( "%s AIT message %d received on Receive with queue not full -> SB\n", mcn->name, GET_ECLP_VALUE(d_addr) ) ;            
#endif
                            mcn->state.current_state =  ENTL_STATE_SB;
                            retval = ENTL_ACTION_SEND | ENTL_ACTION_PROC_AIT ;
                            if(d_addr & ECLP_FW_MASK) retval |= ENTL_ACTION_PROPAGATE_AIT ;
                        }
                    }
                    else {
                        // Atomic Link Operation 
                        uint32_t res = alo_exec( &mcn->ao, ALO_OPCODE(d_addr), ALO_DT(d_addr), s_value ) ;
#ifdef NETRONOME_HOST
                        ENTL_DEBUG( "%s ALO opcode %x received with s_value %lx -> res %x return_value %lx SB\n", mcn->name, ALO_OPCODE(d_addr), s_value, res, mcn->ao.return_value ) ;            
#endif
                        mcn->state.current_state =  ENTL_STATE_SBL;
                        if( res ) {
                            retval = ENTL_ACTION_SEND | ENTL_ACTION_SEND_ALO_T ;
                            if(d_addr & ECLP_FW_MASK) retval |= ENTL_ACTION_PROPAGATE_AIT ;
                        }
                        else {
                            retval = ENTL_ACTION_SEND | ENTL_ACTION_SEND_ALO_F ;
                        }
                    }
                }
                else if( mcn->state.event_i_know == GET_ECLP_VALUE(d_addr) )
                {
#ifdef NETRONOME_HOST
                    ENTL_DEBUG( "%s same ETL message %d received on Receive \n", mcn->name, GET_ECLP_VALUE(d_addr) ) ;         
#endif
                }
                else {
                    set_error( mcn, ENTL_ERROR_FLAG_SEQUENCE ) ;
                    retval = ENTL_ACTION_ERROR ;
#ifdef NETRONOME_HOST
                    ENTL_DEBUG( "%s Out of Sequence ETL message %d received on Receive -> Hello \n", mcn->name, GET_ECLP_VALUE(d_addr) ) ;
#endif
                    entl_state_hello(mcn) ;        
                }
            }
            else {
                set_error( mcn, ENTL_ERROR_FLAG_SEQUENCE ) ;
                mcn->state.event_i_know = 0 ;
                mcn->state.event_send_next = 0 ;
                mcn->state.event_i_sent = 0 ;
                mcn->state.current_state = ENTL_STATE_HELLO ;
                retval = ENTL_ACTION_ERROR ;
#ifdef NETRONOME_HOST
                ENTL_DEBUG( "%s Wrong message %04x received on Receive -> Hello \n", mcn->name, GET_ECLP_VALUE(d_addr) ) ;
#endif
                entl_state_hello(mcn) ;           
            }
        }
        break ;
        // AIT 
        case ENTL_STATE_RA:     // AIT ENTL_ETYPE_AITS sent, waiting for ENTL_ETYPE_AITR
        {
            if( GET_ECLP_ETYPE(d_addr) == ENTL_ETYPE_AITR ) {
                if( ECLP_VALUE_PLUS2(mcn->state.event_i_know) == GET_ECLP_VALUE(d_addr) ) {
                    mcn->state.event_i_know = GET_ECLP_VALUE(d_addr) ;
                    mcn->state.event_send_next = ECLP_VALUE_NEXT(mcn->state.event_i_know) ;
                    mcn->state.current_state = ENTL_STATE_SA ;
                    retval = ENTL_ACTION_SEND ;
                    if( monitor_state(mcn) ) {
                        retval |= ENTL_ACTION_TRIGGERD ;
                    }
#ifdef NETRONOME_HOST
                    ENTL_DEBUG( "%s ETL AITR %d received on RA -> SA\n", mcn->name, GET_ECLP_VALUE(d_addr) ) ;         
#endif
                }
                else {
                    set_error( mcn, ENTL_ERROR_FLAG_SEQUENCE ) ;
                    retval = ENTL_ACTION_ERROR ;
#ifdef NETRONOME_HOST
                    ENTL_DEBUG( "%s Out of Sequence AITR message %d received on Am -> Hello \n", mcn->name, GET_ECLP_VALUE(d_addr) ) ;
#endif
                    entl_state_hello(mcn) ;         
                }
            }
            else if( GET_ECLP_ETYPE(d_addr) == ENTL_ETYPE_ENTL ) {
                if( ECLP_VALUE_PLUS2(mcn->state.event_i_know) == GET_ECLP_VALUE(d_addr) ) {
                    // AIT cancelled due to queue full 
                    mcn->state.event_i_know = GET_ECLP_VALUE(d_addr) ;
                    mcn->state.event_send_next = ECLP_VALUE_NEXT(mcn->state.event_i_know) ;
#ifdef NETRONOME_HOST
                    ENTL_DEBUG( "%s AIT cancell message %d received on RA -> Send \n", mcn->name, GET_ECLP_VALUE(d_addr) ) ; 
#endif
                    mcn->state.current_state = ENTL_STATE_SEND ;
                    retval = ENTL_ACTION_SEND ;
                    if( monitor_state(mcn) ) {
                        retval |= ENTL_ACTION_TRIGGERD ;
                    }
                }
                else {
                    set_error( mcn, ENTL_ERROR_FLAG_SEQUENCE ) ;
                    retval = ENTL_ACTION_ERROR ;
#ifdef NETRONOME_HOST
                    ENTL_DEBUG( "%s Wrong message %lx received on Am -> Hello \n", mcn->name, d_addr ) ;
#endif
                    entl_state_hello(mcn) ; 
                }
            }           
            else {
                set_error( mcn, ENTL_ERROR_FLAG_SEQUENCE ) ;
                retval = ENTL_ACTION_ERROR ;
#ifdef NETRONOME_HOST
                ENTL_DEBUG( "%s Wrong message %lx received on Am -> Hello @ %ld sec\n", mcn->name, d_addr ) ;
#endif
                entl_state_hello(mcn) ;            
            }
        }
        break ;
        case ENTL_STATE_RAL:     // ALO ENTL_ETYPE_AITS sent, waiting for ENTL_ETYPE_AITR
        {
            if( GET_ECLP_ETYPE(d_addr) == ENTL_ETYPE_AITR ) {
                if( ECLP_VALUE_PLUS2(mcn->state.event_i_know) == GET_ECLP_VALUE(d_addr) ) {
                    int res ;
                    mcn->state.event_i_know = GET_ECLP_VALUE(d_addr) ;
                    mcn->state.event_send_next = ECLP_VALUE_NEXT(mcn->state.event_i_know) ;
                    res = alo_complete( &mcn->ao, ALO_OPCODE(d_addr), s_value ) ;  // opcode carries the return status
                    mcn->state.current_state = ENTL_STATE_SAL ;
                    retval = ENTL_ACTION_SEND ;
                    if( res ) retval |= ENTL_ACTION_SIG_AIT ;
#ifdef NETRONOME_HOST
                    ENTL_DEBUG( "%s ETL Ack %d received on RAL -> SAL \n", mcn->name, GET_ECLP_VALUE(d_addr) ) ;         
#endif
                }
                else {
                    set_error( mcn, ENTL_ERROR_FLAG_SEQUENCE ) ;
                    retval = ENTL_ACTION_ERROR ;
#ifdef NETRONOME_HOST
                    ENTL_DEBUG( "%s Out of Sequence ETL message %d received on Am -> Hello\n", mcn->name, GET_ECLP_VALUE(d_addr) ) ;
#endif
                    entl_state_hello(mcn) ;          
                }
            }
            else {
                set_error( mcn, ENTL_ERROR_FLAG_SEQUENCE ) ;
                retval = ENTL_ACTION_ERROR ;
#ifdef NETRONOME_HOST
                ENTL_DEBUG( "%s Wrong message %lx received on Am -> Hello \n", mcn->name, GET_ECLP_ETYPE(d_addr) ) ;
#endif
                entl_state_hello(mcn) ;          
            }
        }
        break ;
        case ENTL_STATE_SA:  // AIT sent, Ack received, sending Ack
        case ENTL_STATE_SAL:  // ALO sent, Ack received, sending Ack
        {
            if( GET_ECLP_ETYPE(d_addr) == ENTL_ETYPE_AITR ) {
                if( mcn->state.event_i_know == GET_ECLP_VALUE(d_addr) )
                {
#ifdef NETRONOME_HOST
                    ENTL_DEBUG( "%s same ETL Ack %d received on Bm \n", mcn->name, GET_ECLP_ETYPE(d_addr) ) ;          
#endif
                }
                else {
                    set_error( mcn, ENTL_ERROR_FLAG_SEQUENCE ) ;
                    retval = ENTL_ACTION_ERROR ;
#ifdef NETRONOME_HOST
                    ENTL_DEBUG( "%s Wrong message %lx received on Bm -> Hello \n", mcn->name, d_addr ) ;
#endif
                    entl_state_hello(mcn) ;  
                }
            }
            else {
                set_error( mcn, ENTL_ERROR_FLAG_SEQUENCE ) ;
                retval = ENTL_ACTION_ERROR ;
#ifdef NETRONOME_HOST
                ENTL_DEBUG( "%s Wrong message %lx received on Bm -> Hello \n", mcn->name, d_addr ) ;
#endif
                entl_state_hello(mcn) ;            
            }
        }
        break ;
        case ENTL_STATE_SB:  // AIT message received, sending Ack
        case ENTL_STATE_SBL:  // ALO message received, sending Ack
        {
            if( GET_ECLP_ETYPE(d_addr) == ENTL_ETYPE_AITS ) {
                if( GET_ECLP_VALUE(d_addr) == mcn->state.event_i_know ) {
#ifdef NETRONOME_HOST
                    ENTL_DEBUG( "%s Same ENTL message %d received on Ah state \n", mcn->name, GET_ECLP_VALUE(d_addr) ) ;
#endif
                }
                else {
                    set_error( mcn, ENTL_ERROR_FLAG_SEQUENCE ) ;
                    retval = ENTL_ACTION_ERROR ;
#ifdef NETRONOME_HOST
                    ENTL_DEBUG( "%s Out of Sequence ENTL %d received on Ah state -> Hello \n", mcn->name, GET_ECLP_VALUE(d_addr) ) ;
#endif
                    entl_state_hello(mcn) ;
                }
            }
            else {
                set_error( mcn, ENTL_ERROR_FLAG_SEQUENCE ) ;
                retval = ENTL_ACTION_ERROR ;
#ifdef NETRONOME_HOST
                ENTL_DEBUG( "%s wrong message %lx received on Send state -> Hello \n", mcn->name, d_addr ) ;
#endif
                entl_state_hello(mcn) ;
            }
        }
        break ;
        case ENTL_STATE_RB:  // got AIT, Ack sent, waiting for ack
        {
            if( GET_ECLP_ETYPE(d_addr) == ENTL_ETYPE_AITA ) {
                if( ECLP_VALUE_PLUS2(mcn->state.event_i_know) == GET_ECLP_VALUE(d_addr) ) {
                    mcn->state.event_i_know = GET_ECLP_VALUE(d_addr) ;
                    mcn->state.event_send_next = ECLP_VALUE_NEXT(mcn->state.event_i_know) ;
                    mcn->state.current_state = ENTL_STATE_SEND ;
                    retval = ENTL_ACTION_SEND | ENTL_ACTION_SIG_AIT ;
                    if( monitor_state(mcn) ) {
                        retval |= ENTL_ACTION_TRIGGERD ;
                    }
#ifdef NETRONOME_HOST
                    ENTL_DEBUG( "%s ETL Ack %d received on RB -> Send \n", mcn->name, GET_ECLP_VALUE(d_addr) ) ;           
#endif
                    //push_back_ENTT_queue( &mcn->receive_ATI_queue, mcn->receive_buffer ) ;
                    //mcn->receive_buffer = NULL ;
                }
                else {
                    set_error( mcn, ENTL_ERROR_FLAG_SEQUENCE ) ;
                    retval = ENTL_ACTION_ERROR ;
#ifdef NETRONOME_HOST
                    ENTL_DEBUG( "%s Out of Sequence ETL message %d received on Am -> Hello \n", mcn->name, GET_ECLP_VALUE(d_addr) ) ;
#endif
                    entl_state_hello(mcn) ;
                }
            }           
            else if( GET_ECLP_ETYPE(d_addr) == ENTL_ETYPE_AITS ) {
                if( GET_ECLP_VALUE(d_addr) == mcn->state.event_i_know ) {
#ifdef NETRONOME_HOST
                    ENTL_DEBUG( "%s Same ENTL message %d received on Bh state \n", mcn->name, GET_ECLP_VALUE(d_addr) ) ;
#endif
                }
                else {
                    set_error( mcn, ENTL_ERROR_FLAG_SEQUENCE ) ;
                    retval = ENTL_ACTION_ERROR ;
#ifdef NETRONOME_HOST
                    ENTL_DEBUG( "%s Out of Sequence ENTL %d received on Bh state -> Hello \n", mcn->name, GET_ECLP_VALUE(d_addr) ) ;
#endif
                    entl_state_hello(mcn) ;
                }
            }
            else {
                set_error( mcn, ENTL_ERROR_FLAG_SEQUENCE ) ;
                retval = ENTL_ACTION_ERROR ;
#ifdef NETRONOME_HOST
                ENTL_DEBUG( "%s Wrong message %lx received on Am -> Hello @ %ld sec\n", mcn->name, d_addr ) ;
#endif
                entl_state_hello(mcn) ;          
            }
        }
        break ;
        case ENTL_STATE_RBL:  // got AITS, Ack sent, waiting for ack
        {
            if( GET_ECLP_ETYPE(d_addr) == ENTL_ETYPE_AITA ) {
                if( ECLP_VALUE_PLUS2(mcn->state.event_i_know) == GET_ECLP_VALUE(d_addr) ) {
                    int ret = alo_update( &mcn->ao, ALO_OPCODE(d_addr) ) ;
                    mcn->state.event_i_know = GET_ECLP_VALUE(d_addr) ;
                    mcn->state.event_send_next = ECLP_VALUE_NEXT(mcn->state.event_i_know) ;
                    mcn->state.current_state = ENTL_STATE_SEND ;
                    if( ret ) retval = ENTL_ACTION_SEND | ENTL_ACTION_SIG_AIT ;
                    else retval = ENTL_ACTION_SEND ;
                    if( monitor_state(mcn) ) {
                        retval |= ENTL_ACTION_TRIGGERD ;
                    }
#ifdef NETRONOME_HOST
                    ENTL_DEBUG( "%s ETL Ack %d received ret %d on RBL -> Send \n", mcn->name, GET_ECLP_VALUE(d_addr), ret ) ;        
#endif
                }
                else {
                    set_error( mcn, ENTL_ERROR_FLAG_SEQUENCE ) ;
                    retval = ENTL_ACTION_ERROR ;
#ifdef NETRONOME_HOST
                    ENTL_DEBUG( "%s Out of Sequence ETL message %d received on Am -> Hello \n", mcn->name, GET_ECLP_VALUE(d_addr) ) ;
#endif
                    entl_state_hello(mcn) ;          
                }
            }           
            else if( GET_ECLP_ETYPE(d_addr) == ENTL_ETYPE_AITS ) {
                if( GET_ECLP_VALUE(d_addr) == mcn->state.event_i_know ) {
#ifdef NETRONOME_HOST
                    ENTL_DEBUG( "%s Same ENTL message %d received on Bh state \n", mcn->name, GET_ECLP_VALUE(d_addr) ) ;
#endif
                }
                else {
                    set_error( mcn, ENTL_ERROR_FLAG_SEQUENCE ) ;
                    retval = ENTL_ACTION_ERROR ;
#ifdef NETRONOME_HOST
                    ENTL_DEBUG( "%s Out of Sequence ENTL %d received on Bh state -> Hello \n", mcn->name, GET_ECLP_VALUE(d_addr) ) ;
#endif
                    entl_state_hello(mcn) ;
                }
            }
            else {
                set_error( mcn, ENTL_ERROR_FLAG_SEQUENCE ) ;
                retval = ENTL_ACTION_ERROR ;
#ifdef NETRONOME_HOST
                ENTL_DEBUG( "%s Wrong message %lx received on Am -> Hello @ %ld sec\n", mcn->name, d_addr ) ;
#endif
                entl_state_hello(mcn) ;
            }
        }
        break ;
        default:
        {
#ifdef NETRONOME_HOST
            ENTL_DEBUG( "%s Statemachine on wrong state %d \n", mcn->name, mcn->state.current_state ) ;            
#endif
            set_error( mcn, ENTL_ERROR_UNKOWN_STATE ) ;
            retval = ENTL_ACTION_ERROR ;
            entl_state_hello(mcn) ;
        }
        break ;
    }

    //ENTL_DEBUG( "%s entl_received Statemachine exit on state %d on %ld sec\n", mcn->name, mcn->state.current_state, ts.tv_sec ) ;         


    return retval ;

}

// On sending ethernet packet, this function should be called to get the destination MAC address for message
//   return value : bit 0: send, bit 1: send AIT, bit 2: process AIT
int entl_next_send( __lmem entl_state_machine_t *mcn, uint64_t *addr, uint64_t *alo_data, uint32_t alo_command, uint32_t ait_queue) 
{
    int retval = ENTL_ACTION_NOP ;
    if( mcn->error_count ) {
        *addr = ENTL_ETYPE_NOP ;
#ifdef ENTL_STATE_DEBUG
            mcn->addr = *addr ;
#endif
        //ENTL_DEBUG( "%s entl_next_send called on error count set %d\n", mcn->name, mcn->error_state.error_count ) ;
        return retval ;     
    }
    switch( mcn->state.current_state ) {

        case ENTL_STATE_IDLE:
        {
            // say something here as attempt to send something on idle state
            //ENTL_DEBUG( "%s Message requested on Idle state @ %ld sec\n", mcn->name, ts.tv_sec ) ;          
            *addr = ENTL_ETYPE_NOP ;
#ifdef ENTL_STATE_DEBUG
            mcn->addr = *addr ;
#endif
        }
        break ;
        case ENTL_STATE_HELLO:
        {
#ifdef NETRONOME_HOST
            ENTL_DEBUG( "%s entl_next_send Hello Message requested on Hello state \n", mcn->name ) ;          
#endif
            *addr =  ENTL_ETYPE_HELLO | mcn->my_value ;
#ifdef ENTL_STATE_DEBUG
            mcn->addr = *addr ;
#endif
            retval = ENTL_ACTION_SEND ;
        }
        break ;
        case ENTL_STATE_WAIT:
        {
#ifdef NETRONOME_HOST
            ENTL_DEBUG( "%s entl_next_send ENTL Message requested on Wait state \n", mcn->name ) ;           
#endif
            *addr = ENTL_ETYPE_ENTL ;   // send message with count 0
#ifdef ENTL_STATE_DEBUG
            mcn->addr = *addr ;
#endif
            retval = ENTL_ACTION_SEND ;    // used be done in hello loop in driver, now flag it here
        }
        break ;
        case ENTL_STATE_SEND:
        {
            uint32_t event_i_know = mcn->state.event_i_know ;            // last received event number 
            uint32_t event_i_sent = mcn->state.event_i_sent ;

            mcn->state.event_i_sent = mcn->state.event_send_next ;
            mcn->state.event_send_next = ECLP_VALUE_PLUS2(mcn->state.event_send_next) ;
            *addr = mcn->state.event_i_sent ;
            // Avoiding to send AIT on the very first loop where other side will be in Hello state
#ifdef NETRONOME_HOST
            ENTL_DEBUG( "%s entl_next_send case ENTL_STATE_SEND: i_know %d i_sent %d alo_command %x op %x sc %x \n", mcn->name, event_i_know, event_i_sent, alo_command, ALO_COMMAND_OPCODE(alo_command), ALO_COMMAND_SC(alo_command) ) ;
#endif
            if( (event_i_know || event_i_sent) && (ait_queue || ALO_COMMAND_OPCODE(alo_command) ) ) {
                if( ALO_COMMAND_OPCODE(alo_command) ) {
                    uint32_t ret =  alo_initiate( &mcn->ao, ALO_COMMAND_OPCODE(alo_command), ALO_COMMAND_SC(alo_command), alo_data ) ;
                    if( ret ) {
                        mcn->state.current_state = ENTL_STATE_RAL ;          
                        *addr |= ENTL_ETYPE_AITS | SET_ECLP_ALO(alo_command) ;
#ifdef ENTL_STATE_DEBUG
                        mcn->addr = *addr ;
#endif
                        if( ALO_COMMAND_FW(alo_command) ) *addr |= ECLP_FW_MASK ;
                        retval = ENTL_ACTION_SEND | ENTL_ACTION_SEND_ALO  ;
#ifdef NETRONOME_HOST
                        ENTL_DEBUG( "%s ETL ALO %x requested on Send state -> RAL %lx %x %x\n", mcn->name, alo_command, *alo_data, ALO_COMMAND_OPCODE(alo_command), ALO_COMMAND_SC(alo_command) ) ;
#endif
                    }
                    else if( ait_queue ) {
                         mcn->state.current_state = ENTL_STATE_RA ;          
                        *addr |= ENTL_ETYPE_AITS ;
#ifdef ENTL_STATE_DEBUG
                        mcn->addr = *addr ;
#endif
                        if( ALO_COMMAND_FW(alo_command) ) *addr |= ECLP_FW_MASK ;
                        retval = ENTL_ACTION_SEND | ENTL_ACTION_SEND_AIT  ;
#ifdef NETRONOME_HOST
                        ENTL_DEBUG( "%s ETL AIT Message %d requested on Send state -> RA \n", mcn->name, ait_queue ) ;
#endif
                    }
                    else {
                        *addr = ENTL_ETYPE_NOP ;
                    }
                }
                else {
                    mcn->state.current_state = ENTL_STATE_RA ;          
                    *addr |= ENTL_ETYPE_AITS ;
#ifdef ENTL_STATE_DEBUG
                        mcn->addr = *addr ;
#endif
                    retval = ENTL_ACTION_SEND | ENTL_ACTION_SEND_AIT  ;
#ifdef NETRONOME_HOST
                    ENTL_DEBUG( "%s ETL AIT Message %d requested on Send state -> RA \n", mcn->name, ait_queue ) ;
#endif
                }
            }
            else {
#ifdef NETRONOME_HOST
                ENTL_DEBUG( "%s ETL AIT Message %d %d %d requested on Send state -> Receive %x %x %x\n", mcn->name, ait_queue, event_i_know, event_i_sent, ait_queue, ALO_COMMAND_OPCODE(alo_command), alo_command ) ;
#endif
                mcn->state.current_state = ENTL_STATE_RECEIVE ;         
                *addr |= ENTL_ETYPE_ENTL ;
#ifdef ENTL_STATE_DEBUG
                mcn->addr = *addr ;
#endif
                retval = ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT ;  // data send as optional
            }
            //ENTL_DEBUG( "%s ETL Message %d requested on Send state -> Receive @ %ld sec\n", mcn->name,  ) ;         

        }
        break ;
        case ENTL_STATE_RECEIVE:
        {
            *addr = ENTL_ETYPE_NOP ;
        }
        break ;
        // AIT 
        case ENTL_STATE_RA:
        case ENTL_STATE_RAL:
        {
            *addr = ENTL_ETYPE_NOP ;
        }
        break ;
        case ENTL_STATE_SA:
        {
            mcn->state.event_i_sent = mcn->state.event_send_next ;
            mcn->state.event_send_next = ECLP_VALUE_PLUS2(mcn->state.event_send_next) ;
            *addr = ENTL_ETYPE_AITA | mcn->state.event_i_sent ;
#ifdef ENTL_STATE_DEBUG
            mcn->addr = *addr ;
#endif
            retval = ENTL_ACTION_SEND | ENTL_ACTION_SIG_AIT | ENTL_ACTION_DROP_AIT ;
            mcn->state.current_state = ENTL_STATE_RECEIVE ;
#ifdef NETRONOME_HOST
            ENTL_DEBUG( "%s ETL AIT ACK %d requested on SA state -> Receive \n", mcn->name, mcn->state.event_i_sent ) ;         
#endif
        }
        break ;
        case ENTL_STATE_SAL:
        {
            mcn->state.event_i_sent = mcn->state.event_send_next ;
            mcn->state.event_send_next = ECLP_VALUE_PLUS2(mcn->state.event_send_next) ;
            *addr = ENTL_ETYPE_AITA | mcn->state.event_i_sent | 0x10000 ; // T flag set
#ifdef ENTL_STATE_DEBUG
            mcn->addr = *addr ;
#endif
            retval = ENTL_ACTION_SEND | ENTL_ACTION_SIG_AIT | ENTL_ACTION_DROP_AIT ;
            mcn->state.current_state = ENTL_STATE_RECEIVE ;
#ifdef NETRONOME_HOST
            ENTL_DEBUG( "%s ETL AIT ACK %d requested on SAL state -> Receive \n", mcn->name, mcn->state.event_i_sent ) ;         
#endif
        }
        break ;
        case ENTL_STATE_SB:
        {
            mcn->state.event_i_sent = mcn->state.event_send_next ;
            mcn->state.event_send_next = ECLP_VALUE_PLUS2(mcn->state.event_send_next) ;
            *addr = ENTL_ETYPE_AITR | mcn->state.event_i_sent ;
#ifdef ENTL_STATE_DEBUG
            mcn->addr = *addr ;
#endif
            retval = ENTL_ACTION_SEND ;
            mcn->state.current_state = ENTL_STATE_RB ;          
#ifdef NETRONOME_HOST
            ENTL_DEBUG( "%s ETL AIT ACK %d requested on SB state -> RB\n", mcn->name, mcn->state.event_i_sent ) ;
#endif
        }
        break ;
        case ENTL_STATE_SBL:
        {
            mcn->state.event_i_sent = mcn->state.event_send_next ;
            mcn->state.event_send_next = ECLP_VALUE_PLUS2(mcn->state.event_send_next) ;
            *addr = ENTL_ETYPE_AITR | mcn->state.event_i_sent | ( (uint64_t)mcn->ao.return_flag << 16 ) ; // T flag set
#ifdef ENTL_STATE_DEBUG
            mcn->addr = *addr ;
#endif
            *alo_data = mcn->ao.return_value ;
            retval = ENTL_ACTION_SEND | ENTL_ACTION_SEND_ALO ;
            mcn->state.current_state = ENTL_STATE_RBL ;          
#ifdef NETRONOME_HOST
            ENTL_DEBUG( "%s ETL AIT ACK %d requested on SBL state -> RBL\n", mcn->name, mcn->state.event_i_sent ) ;          
#endif
            
        }
        break ;
        case ENTL_STATE_RB:
        case ENTL_STATE_RBL:
        {
            *addr = ENTL_ETYPE_NOP ;
#ifdef ENTL_STATE_DEBUG
            mcn->addr = *addr ;
#endif
        }
        break ;
        default:
        {
            *addr = ENTL_ETYPE_NOP ;
#ifdef ENTL_STATE_DEBUG
            mcn->addr = *addr ;
#endif
        }
        break ; 
    }
    if(mcn->smc_id) *addr |= ECLP_SMSEL_MASK ; // ENTL State Machine select
    // Keep result for rewinding
    mcn->p_addr = *addr ;
    mcn->p_data = *alo_data ;
    mcn->p_retval = retval ;
    //ENTL_DEBUG( "%s entl_next_send Statemachine exit on state %d on %ld sec\n", mcn->name, mcn->state.current_state, ts.tv_sec ) ;            
    return retval ;
}

// On receiving error (link down, timeout), this functon should be called to report to the state machine
void entl_state_error( __lmem entl_state_machine_t *mcn, uint32_t error_flag ) 
{
    if( mcn->error_flag == 0 ) {
        mcn->error_flag = error_flag ;
        mcn->error_state.event_i_know = mcn->state.event_i_know ;
        mcn->error_state.event_i_sent = mcn->state.event_i_sent ;
        mcn->error_state.event_send_next = mcn->state.event_send_next ;
        mcn->error_state.current_state = mcn->state.current_state ;        
        mcn->error_count ++ ;
    }
    else {
        mcn->p_error_flag |= error_flag ;
        mcn->error_count ++ ;
    }
}

//#endif /* _PKT_COUNT_C_ */

/* -*-  Mode:C; c-basic-offset:4; tab-width:4 -*- */
