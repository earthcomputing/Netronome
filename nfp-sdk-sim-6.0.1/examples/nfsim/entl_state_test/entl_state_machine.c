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


#ifndef _PKT_COUNT_C_
#define _PKT_COUNT_C_

#ifdef NETRONOME_HOST

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

#include "entl_state_machine.h"

void entl_state_init( __lmem entl_state_machine_t *mcn ) 
{
    int i ;
    mcn->error_flag = 0 ;
    mcn->p_error_flag = 0 ;
    mcn->error_count = 0 ;
    mcn->state_count = 0 ;
    mcn->state.event_i_know = 0 ;
    mcn->state.event_i_sent = 0 ;
    mcn->state.event_send_next = 0 ;
    mcn->state.current_state = 0 ;
    for( i = 0 ; i < 32 ; i++) mcn->ao.reg[i] = 0 ;
    mcn->result_buffer = 0 ;
    mcn->flags = 0 ;

    entl_set_random_addr(mcn) ;
}

#ifndef NETRONOME_HOST
static uint32_t l_rand()
{
    uint32_t ret ;
    ret = rand() ;
    ret = local_csr_read(local_csr_pseudo_random_number) ;
    return ret ;
}
#endif

void entl_set_random_addr( __lmem entl_state_machine_t *mcn ) 
{
    // uint64_t  addr = l_rand() ;
    mcn->my_value = 0xbadbeef ; //((addr << 32 ) | l_rand()) & 0xffffffffffff ;
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
    }
}
static int is_ENTT_queue_full( int queue_size ) {
    return queue_size >= MAX_ENTT_QUEUE_SIZE ;
}

// On Received message, this should be called with the massage (MAC source & destination addr)
//   return value : bit 0: send, bit 1: send AIT, bit 2: process AIT
int entl_received( __lmem entl_state_machine_t *mcn, uint64_t s_addr, uint64_t d_addr, uint32_t ait_queue, uint32_t ait_receive ) 
{
    int retval = ENTL_ACTION_NOP ;
    uint32_t l_daddr = (d_addr & ENTL_COUNTER_MASK) ;
    if( (d_addr & ENTL_MESSAGE_MASK) == ENTL_MESSAGE_NOP) {
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
            if( (d_addr & ENTL_MESSAGE_MASK) == ENTL_MESSAGE_HELLO )
            {
#ifdef NETRONOME_HOST
                ENTL_DEBUG( "Hello message on my_addr %x s_addr %x received on hello state \n", mcn->my_addr, s_addr ) ;
#endif
                if( mcn->my_addr > s_addr ) {
                    mcn->state.event_i_sent = mcn->state.event_i_know = mcn->state.event_send_next = 0 ;
                    mcn->state.current_state = ENTL_STATE_WAIT ;        
                    mcn->state_count = 0 ;
                    retval = ENTL_ACTION_SEND ;
                }
                else if( mcn->my_addr == s_addr ) {
                    // say error as Alan's 1990s problem again
                    retval = ENTL_ACTION_SEND | ENTL_ACTION_SET_ADDR ;
                }
                else {
                    //ENTL_DEBUG( "%s Hello message %d received on wait state but not win @ %ld sec\n", mcn->name, u_saddr, ts.tv_sec ) ;
                    retval = ENTL_ACTION_SEND | ENTL_ACTION_SIG_ERR ;
                }
            }
            else if( (d_addr & ENTL_MESSAGE_MASK) == ENTL_MESSAGE_EVENT ) {
                // Hello state got event
                if( (d_addr & ENTL_COUNTER_MASK) == 0 ) {
                    mcn->state.event_i_know = 0 ;
                    mcn->state.event_send_next = 1 ;
                    mcn->state.current_state = ENTL_STATE_SEND ;
                    retval = ENTL_ACTION_SEND ;
                    //ENTL_DEBUG( "%s ENTL %d message received on Hello -> Send @ %ld sec\n", mcn->name, l_daddr, ts.tv_sec ) ;           
                }
                else {
                    //ENTL_DEBUG( "%s Out of sequence ENTL %d message received on Hello @ %ld sec\n", mcn->name, l_daddr, ts.tv_sec ) ;           
                }
            }
            else {
                // Received non hello message on Hello state
                //ENTL_DEBUG( "%s non-hello message %04x received on hello state @ %ld sec\n", mcn->name, u_daddr, ts.tv_sec ) ;
            }           
        }
        break ;
        case ENTL_STATE_WAIT:
        {
            if( (d_addr & ENTL_MESSAGE_MASK) == ENTL_MESSAGE_HELLO )
            {
                mcn->state_count++ ;
                if( mcn->state_count > ENTL_COUNT_MAX ) {
                    //ENTL_DEBUG( "%s Hello message %d received overflow %d on Wait state -> Hello state @ %ld sec\n", mcn->name, u_saddr, mcn->state_count, ts.tv_sec ) ;
                    mcn->state.event_i_know = 0 ;
                    mcn->state.event_send_next = 0 ;
                    mcn->state.event_i_sent = 0 ;
                    mcn->state.current_state = ENTL_STATE_HELLO ;       
                    retval = ENTL_ACTION_SEND ;
                }
            }
            else if( (d_addr & ENTL_MESSAGE_MASK) == ENTL_MESSAGE_EVENT )
            {
                if( (d_addr & ENTL_COUNTER_MASK) == mcn->state.event_i_sent + 1 ) {
                    mcn->state.event_i_know = (d_addr & ENTL_COUNTER_MASK) ;
                    mcn->state.event_send_next = (d_addr & ENTL_COUNTER_MASK) + 1 ;
                    mcn->state.current_state = ENTL_STATE_SEND ;            
                    retval = ENTL_ACTION_SEND ;
                    //ENTL_DEBUG( "%s ENTL message %d received on Wait state -> Send state @ %ld sec\n", mcn->name, l_daddr, ts.tv_sec ) ;
                }
                else {
                    mcn->state.event_i_know = 0 ;
                    mcn->state.event_send_next = 0 ;
                    mcn->state.event_i_sent = 0 ;
                    mcn->state.current_state = ENTL_STATE_HELLO ;   
                    //ENTL_DEBUG( "%s Wrong ENTL message %d received on Wait state -> Hello state @ %ld sec\n", mcn->name, l_daddr, ts.tv_sec ) ;
                }
            }
            else {
                // Received non hello message on Wait state
                //ENTL_DEBUG( "%s wrong message %04x received on Wait state -> Hello @ %ld sec\n", mcn->name, u_daddr, ts.tv_sec ) ;          
                set_error( mcn, ENTL_ERROR_FLAG_SEQUENCE ) ;
                retval = ENTL_ACTION_ERROR ;        
            }       
        }
        break ;
        case ENTL_STATE_SEND:
        {
            if( (d_addr & ENTL_MESSAGE_MASK) == ENTL_MESSAGE_EVENT || (d_addr & ENTL_MESSAGE_MASK) == ENTL_MESSAGE_ACK ) {
                if( (d_addr & ENTL_COUNTER_MASK) == mcn->state.event_i_know ) {
                    // ENTL_DEBUG( "%s Same ENTL message %d received on Send state @ %ld sec\n", mcn->name, l_daddr, ts.tv_sec ) ;
                }
                else {
                    set_error( mcn, ENTL_ERROR_FLAG_SEQUENCE ) ;
                    retval = ENTL_ACTION_ERROR ;
                    //ENTL_DEBUG( "%s Out of Sequence ENTL %d received on Send state -> Hello @ %ld sec\n", mcn->name, l_daddr, ts.tv_sec ) ;
                }
            }
            else {
                set_error( mcn, ENTL_ERROR_FLAG_SEQUENCE ) ;
                retval = ENTL_ACTION_ERROR ;
                //ENTL_DEBUG( "%s wrong message %04x received on Send state -> Hello @ %ld sec\n", mcn->name, u_daddr, ts.tv_sec ) ;
            }
        }
        break ;
        case ENTL_STATE_RECEIVE:
        {
            if( (d_addr & ENTL_MESSAGE_MASK) == ENTL_MESSAGE_EVENT ) {
                if( mcn->state.event_i_know + 2 == (d_addr & ENTL_COUNTER_MASK) ) {
                    mcn->state.event_i_know = (d_addr & ENTL_COUNTER_MASK) ;
                    mcn->state.event_send_next = (d_addr & ENTL_COUNTER_MASK) + 1 ;
                    mcn->state.current_state = ENTL_STATE_SEND ;
                    if( ait_queue ) {  // AIT has priority
                        retval = ENTL_ACTION_SEND ;
                    }
                    else {
                        retval = ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT ;  // data send as optional
                    }
                    //ENTL_DEBUG( "%s ETL message %d received on Receive -> Send @ %ld sec\n", mcn->name, l_daddr, ts.tv_sec ) ;            
                }
                else if( mcn->state.event_i_know == (d_addr & ENTL_COUNTER_MASK) )
                {
                    //ENTL_DEBUG( "%s same ETL message %d received on Receive @ %ld sec\n", mcn->name, l_daddr, ts.tv_sec ) ;         
                }
                else {
                    set_error( mcn, ENTL_ERROR_FLAG_SEQUENCE ) ;
                    retval = ENTL_ACTION_ERROR ;
                    //ENTL_DEBUG( "%s Out of Sequence ETL message %d received on Receive -> Hello @ %ld sec\n", mcn->name, l_daddr, ts.tv_sec ) ;         
                }
            }
            else if( (d_addr & ENTL_MESSAGE_MASK) == ENTL_MESSAGE_AIT ) {
                if( mcn->state.event_i_know + 2 == (d_addr & ENTL_COUNTER_MASK) ) {
                    mcn->state.event_i_know = (d_addr & ENTL_COUNTER_MASK) ;
                    mcn->state.event_send_next = (d_addr & ENTL_COUNTER_MASK) + 1 ;
                    mcn->state.current_state = ENTL_STATE_SB ;
                    if( is_ENTT_queue_full( ait_receive ) ) {
                        //ENTL_DEBUG( "%s AIT message %d received on Receive with queue full -> Ah @ %ld sec\n", mcn->name, l_daddr, ts.tv_sec ) ;            
                        retval = ENTL_ACTION_PROC_AIT ;
                    }
                    else {
                        retval = ENTL_ACTION_SEND | ENTL_ACTION_PROC_AIT ;
                    }
                    //ENTL_DEBUG( "%s AIT message %d received on Receive -> Ah @ %ld sec\n", mcn->name, l_daddr, ts.tv_sec ) ;            
                }
                else if( mcn->state.event_i_know == (d_addr & ENTL_COUNTER_MASK) )
                {
                    //ENTL_DEBUG( "%s same ETL message %d received on Receive @ %ld sec\n", mcn->name, l_daddr, ts.tv_sec ) ;         
                }
                else {
                    set_error( mcn, ENTL_ERROR_FLAG_SEQUENCE ) ;
                    retval = ENTL_ACTION_ERROR ;
                    //ENTL_DEBUG( "%s Out of Sequence ETL message %d received on Receive -> Hello @ %ld sec\n", mcn->name, l_daddr, ts.tv_sec ) ;         
                }
            }
            else {
                set_error( mcn, ENTL_ERROR_FLAG_SEQUENCE ) ;
                mcn->state.event_i_know = 0 ;
                mcn->state.event_send_next = 0 ;
                mcn->state.event_i_sent = 0 ;
                mcn->state.current_state = ENTL_STATE_HELLO ;
                retval = ENTL_ACTION_ERROR ;
                //ENTL_DEBUG( "%s Wrong message %04x received on Receive -> Hello @ %ld sec\n", mcn->name, u_daddr,   ts.tv_sec ) ;           
            }
        }
        break ;
        // AIT 
        case ENTL_STATE_RA:     // AIT message sent, waiting for ack
        {
            if( (d_addr & ENTL_MESSAGE_MASK) == ENTL_MESSAGE_ACK ) {
                if( mcn->state.event_i_know + 2 == (d_addr & ENTL_COUNTER_MASK) ) {
                    mcn->state.event_i_know = (d_addr & ENTL_COUNTER_MASK) ;
                    mcn->state.event_send_next = (d_addr & ENTL_COUNTER_MASK) + 1 ;
                    mcn->state.current_state = ENTL_STATE_SA ;
                    retval = ENTL_ACTION_SEND ;
                    // ENTL_DEBUG( "%s ETL Ack %d received on Am -> Bm @ %ld sec\n", mcn->name, l_daddr, ts.tv_sec ) ;         
                }
                else {
                    set_error( mcn, ENTL_ERROR_FLAG_SEQUENCE ) ;
                    retval = ENTL_ACTION_ERROR ;
                    // ENTL_DEBUG( "%s Out of Sequence ETL message %d received on Am -> Hello @ %ld sec\n", mcn->name, l_daddr, ts.tv_sec ) ;          
                }
            }
            else if( (d_addr & ENTL_MESSAGE_MASK) == ENTL_MESSAGE_EVENT ) {
                if( mcn->state.event_i_know == (d_addr & ENTL_COUNTER_MASK) )
                {
                    //ENTL_DEBUG( "%s same ETL event %d received on Am @ %ld sec\n", mcn->name, l_daddr, ts.tv_sec ) ;            
                }
                else {
                    set_error( mcn, ENTL_ERROR_FLAG_SEQUENCE ) ;
                    retval = ENTL_ACTION_ERROR ;
                    //ENTL_DEBUG( "%s Wrong message %04x received on Am -> Hello @ %ld sec\n", mcn->name, u_daddr,   ts.tv_sec ) ;            
                }
            }           
            else {
                set_error( mcn, ENTL_ERROR_FLAG_SEQUENCE ) ;
                retval = ENTL_ACTION_ERROR ;
                //ENTL_DEBUG( "%s Wrong message %04x received on Am -> Hello @ %ld sec\n", mcn->name, u_daddr,   ts.tv_sec ) ;            
            }
        }
        break ;
        case ENTL_STATE_SA:  // AIT sent, Ack received, sending Ack
        {
            if( (d_addr & ENTL_MESSAGE_MASK) == ENTL_MESSAGE_ACK ) {
                if( mcn->state.event_i_know == (d_addr & ENTL_COUNTER_MASK) )
                {
                    //ENTL_DEBUG( "%s same ETL Ack %d received on Bm @ %ld sec\n", mcn->name, l_daddr, ts.tv_sec ) ;          
                }
                else {
                    set_error( mcn, ENTL_ERROR_FLAG_SEQUENCE ) ;
                    retval = ENTL_ACTION_ERROR ;
                    //ENTL_DEBUG( "%s Wrong message %04x received on Bm -> Hello @ %ld sec\n", mcn->name, u_daddr,   ts.tv_sec ) ;            
                }
            }
            else {
                set_error( mcn, ENTL_ERROR_FLAG_SEQUENCE ) ;
                retval = ENTL_ACTION_ERROR ;
                //ENTL_DEBUG( "%s Wrong message %04x received on Bm -> Hello @ %ld sec\n", mcn->name, u_daddr,   ts.tv_sec ) ;                            
            }
        }
        break ;
        case ENTL_STATE_SB:  // AIT message received, sending Ack
        {
            if( (d_addr & ENTL_MESSAGE_MASK) == ENTL_MESSAGE_AIT ) {
                if( (d_addr & ENTL_COUNTER_MASK) == mcn->state.event_i_know ) {
                    //ENTL_DEBUG( "%s Same ENTL message %d received on Ah state @ %ld sec\n", mcn->name, l_daddr, ts.tv_sec ) ;
                }
                else {
                    set_error( mcn, ENTL_ERROR_FLAG_SEQUENCE ) ;
                    retval = ENTL_ACTION_ERROR ;
                    // ENTL_DEBUG( "%s Out of Sequence ENTL %d received on Ah state -> Hello @ %ld sec\n", mcn->name, l_daddr, ts.tv_sec ) ;
                }
            }
            else {
                set_error( mcn, ENTL_ERROR_FLAG_SEQUENCE ) ;
                retval = ENTL_ACTION_ERROR ;
                //ENTL_DEBUG( "%s wrong message %04x received on Send state -> Hello @ %ld sec\n", mcn->name, u_daddr, ts.tv_sec ) ;
            }
        }
        break ;
        case ENTL_STATE_RB:  // got AIT, Ack sent, waiting for ack
        {
            if( (d_addr & ENTL_MESSAGE_MASK) == ENTL_MESSAGE_ACK ) {
                if( mcn->state.event_i_know + 2 == (d_addr & ENTL_COUNTER_MASK) ) {
                    mcn->state.event_i_know = (d_addr & ENTL_COUNTER_MASK) ;
                    mcn->state.event_send_next = (d_addr & ENTL_COUNTER_MASK) + 1 ;
                    mcn->state.current_state = ENTL_STATE_SEND ;
                    retval = ENTL_ACTION_SEND | ENTL_ACTION_SIG_AIT ;
                    //ENTL_DEBUG( "%s ETL Ack %d received on Bh -> Send @ %ld sec\n", mcn->name, l_daddr, ts.tv_sec ) ;           
                    //push_back_ENTT_queue( &mcn->receive_ATI_queue, mcn->receive_buffer ) ;
                    //mcn->receive_buffer = NULL ;
                }
                else {
                    set_error( mcn, ENTL_ERROR_FLAG_SEQUENCE ) ;
                    retval = ENTL_ACTION_ERROR ;
                    //ENTL_DEBUG( "%s Out of Sequence ETL message %d received on Am -> Hello @ %ld sec\n", mcn->name, l_daddr, ts.tv_sec ) ;          
                }
            }           
            else if( (d_addr & ENTL_MESSAGE_MASK) == ENTL_MESSAGE_AIT ) {
                if( l_daddr == mcn->state.event_i_know ) {
                    //ENTL_DEBUG( "%s Same ENTL message %d received on Bh state @ %ld sec\n", mcn->name, l_daddr, ts.tv_sec ) ;
                }
                else {
                    set_error( mcn, ENTL_ERROR_FLAG_SEQUENCE ) ;
                    retval = ENTL_ACTION_ERROR ;
                    //ENTL_DEBUG( "%s Out of Sequence ENTL %d received on Bh state -> Hello @ %ld sec\n", mcn->name, l_daddr, ts.tv_sec ) ;
                }
            }
            else {
                set_error( mcn, ENTL_ERROR_FLAG_SEQUENCE ) ;
                retval = ENTL_ACTION_ERROR ;
                //ENTL_DEBUG( "%s Wrong message %04x received on Am -> Hello @ %ld sec\n", mcn->name, u_daddr,   ts.tv_sec ) ;            
            }
        }
        break ;
        default:
        {
            //ENTL_DEBUG( "%s Statemachine on wrong state %d on %ld sec\n", mcn->name, mcn->state.current_state, ts.tv_sec ) ;            
            set_error( mcn, ENTL_ERROR_UNKOWN_STATE ) ;
            retval = ENTL_ACTION_ERROR ;
        }
        break ;
    }

    //ENTL_DEBUG( "%s entl_received Statemachine exit on state %d on %ld sec\n", mcn->name, mcn->state.current_state, ts.tv_sec ) ;         


    return retval ;

}

// On sending ethernet packet, this function should be called to get the destination MAC address for message
//   return value : bit 0: send, bit 1: send AIT, bit 2: process AIT
int entl_next_send( __lmem entl_state_machine_t *mcn, uint64_t *addr, uint32_t ait_queue) 
{
    int retval = ENTL_ACTION_NOP ;
    if( mcn->error_count ) {
        *addr = ENTL_MESSAGE_NOP ;
        //ENTL_DEBUG( "%s entl_next_send called on error count set %d\n", mcn->name, mcn->error_state.error_count ) ;
        return retval ;     
    }
    switch( mcn->state.current_state ) {

        case ENTL_STATE_IDLE:
        {
            // say something here as attempt to send something on idle state
            //ENTL_DEBUG( "%s Message requested on Idle state @ %ld sec\n", mcn->name, ts.tv_sec ) ;          
            *addr = ENTL_MESSAGE_NOP ;
        }
        break ;
        case ENTL_STATE_HELLO:
        {
            //ENTL_DEBUG( "%s repeated Message requested on Hello state @ %ld sec\n", mcn->name, ts.tv_sec ) ;          
            *addr = ENTL_MESSAGE_HELLO ;
            retval = ENTL_ACTION_SEND ;
        }
        break ;
        case ENTL_STATE_WAIT:
        {
            //ENTL_DEBUG( "%s repeated Message requested on Wait state @ %ld sec\n", mcn->name, ts.tv_sec ) ;           
            *addr = ENTL_MESSAGE_EVENT ;   // send message with count 0
            retval = ENTL_ACTION_SEND ;    // used be done in hello loop in driver, now flag it here
        }
        break ;
        case ENTL_STATE_SEND:
        {
            uint32_t event_i_know = mcn->state.event_i_know ;            // last received event number 
            uint32_t event_i_sent = mcn->state.event_i_sent ;

            mcn->state.event_i_sent = mcn->state.event_send_next ;
            mcn->state.event_send_next += 2 ;
            *addr = mcn->state.event_i_sent ;
            // Avoiding to send AIT on the very first loop where other side will be in Hello state
            if( event_i_know && event_i_sent && ait_queue ) {
                mcn->state.current_state = ENTL_STATE_RA ;          
                *addr |= ENTL_MESSAGE_AIT ;
                retval = ENTL_ACTION_SEND | ENTL_ACTION_SEND_AIT  ;
                //ENTL_DEBUG( "%s ETL AIT Message %d requested on Send state -> Am @ %ld sec\n", mcn->name, *l_addr, ts.tv_sec ) ;            
            }
            else {
                mcn->state.current_state = ENTL_STATE_RECEIVE ;         
                *addr |= ENTL_MESSAGE_EVENT ;
                retval = ENTL_ACTION_SEND | ENTL_ACTION_SEND_DAT ;  // data send as optional
            }
            //ENTL_DEBUG( "%s ETL Message %d requested on Send state -> Receive @ %ld sec\n", mcn->name, *l_addr, ts.tv_sec ) ;         

        }
        break ;
        case ENTL_STATE_RECEIVE:
        {
            *addr = ENTL_MESSAGE_NOP ;
        }
        break ;
        // AIT 
        case ENTL_STATE_RA:
        {
            *addr = ENTL_MESSAGE_NOP ;
        }
        break ;
        case ENTL_STATE_SA:
        {
            mcn->state.event_i_sent = mcn->state.event_send_next ;
            mcn->state.event_send_next += 2 ;
            *addr = ENTL_MESSAGE_ACK | mcn->state.event_i_sent ;
            retval = ENTL_ACTION_SEND | ENTL_ACTION_SIG_AIT | ENTL_ACTION_DROP_AIT ;
            mcn->state.current_state = ENTL_STATE_RECEIVE ;
            //ENTL_DEBUG( "%s ETL AIT ACK %d requested on BM state -> Receive @ %ld sec\n", mcn->name, *l_addr, ts.tv_sec ) ;         
        }
        break ;
        case ENTL_STATE_SB:
        {
            if( ait_queue == 0 ) {  // ait receive queue is full
                *addr = ENTL_MESSAGE_NOP ;
            }
            else {
                mcn->state.event_i_sent = mcn->state.event_send_next ;
                mcn->state.event_send_next += 2 ;
                *addr = ENTL_MESSAGE_ACK | mcn->state.event_i_sent ;
                retval = ENTL_ACTION_SEND ;
                mcn->state.current_state = ENTL_STATE_BH ;          
                //ENTL_DEBUG( "%s ETL AIT ACK %d requested on Ah state -> Bh @ %ld sec\n", mcn->name, *l_addr, ts.tv_sec ) ;          
            }
        }
        break ;
        case ENTL_STATE_RB:
        {
            *addr = ENTL_MESSAGE_NOP ;
        }
        break ;
        default:
        {
            *addr = ENTL_MESSAGE_NOP ;
        }
        break ; 
    }

    //ENTL_DEBUG( "%s entl_next_send Statemachine exit on state %d on %ld sec\n", mcn->name, mcn->state.current_state, ts.tv_sec ) ;            
    return retval ;
}

// On receiving error (link down, timeout), this functon should be called to report to the state machine
void entl_state_error( __lmem entl_state_machine_t *mcn, uint32_t error_flag ) 
{
    if( mcn->error_flag == 0 ) {
        mcn->error_flag = error_flag ;
        mcn->error_count ++ ;
    }
    else {
        mcn->p_error_flag |= error_flag ;
        mcn->error_count ++ ;
    }
}

#endif /* _PKT_COUNT_C_ */

/* -*-  Mode:C; c-basic-offset:4; tab-width:4 -*- */
