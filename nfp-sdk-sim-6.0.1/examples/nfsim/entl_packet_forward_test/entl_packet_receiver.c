/*
 * Copyright (C) 2017,  Earth Computing Inc.  All rights reserved.
 *
 *
 * @file          entl_packet_receiver.c
 * @brief         Packet Receiver Code for Packet Forward Test  
 *
 * Author:        Atsushi Kasuya
 *
 *  One Island is mapped to handle two ports
 *  The receiver code runs on ALL thread on ME_n where n > 1
 *  The thread 0 on ME0/1 is running ENTL send operation
 *  The threads > 0 on ME0/1 are running Packet Forwarding
 *
 *  This mapping may be changed such that any Threads on any ME 
 *
 */
 #include <nfp.h>
#include <me.h>
#include <mutexlv.h>
#include <stdint.h>
#include <pkt/pkt.h>
#include <net/eth.h>
#include <nfp/mem_bulk.h>
#include "cls_reorder.h"
#include "egress_queue.h"
#include "entl_reflect.h"
#include "entl_state_machine.h"
#include "pkt_clone.h"

#define PACKET_RING_IMPORT
#include "egress_queue.h"

#define ETH_P_ECLP  0xEAC0    /* Earth Computing Link Protocol [ NOT AN OFFICIALLY REGISTERED ID ] */

//#define ENTL_STATE_HELLO    1
//#define ENTL_MESSAGE_ONLY     0x800000000000

#define BACKOFF_SLEEP           256

#define PKT_NBI_OFFSET 62

#ifndef PKT_NBI_OFFSET
#define PKT_NBI_OFFSET 64
#endif

#define MAC_PREPEND_BYTES       8

#ifndef NBI
#define NBI 0
#endif

#if (__REVISION_MIN < __REVISION_B0)
#define MS_MAX_OFF  64
#else
#define MS_MAX_OFF  128
#endif

/* convert a 40 bit address to a blm buffer handle */
#define blm_buf_ptr2handle(_bp) (blm_buf_handle_t)\
                                ((unsigned long long)(_bp) >> 11)

struct pkt_hdr {
    struct {
        //uint32_t mac_timestamp;
        //uint32_t mac_prepend;
        uint16_t dummy;
    };
    struct eth_hdr pkt;
    uint64_t data ;
};

struct pkt_rxed {
    struct nbi_meta_catamaran nbi_meta;
    struct pkt_hdr            pkt_hdr;
};


static __xwrite entl_reflect_data_t entl_data_out ;

__declspec(remote) __xread entl_reflect_data_t entl_data_in ;
__declspec(remote) SIGNAL entl_send_sig ;

#define CTM_ALLOC_ERR 0xffffffff
#define MY_VALUE 0xbeef
#define RETRY_CYCLE 1000

//__import __lmem volatile uint64_t  keep_pkt_num ;
//__shared uint32_t alo_command ;
//__shared int  next_flag ;
// MUTEXLV state_lock = 0;
//#define STATE_LOCK_BIT 0

//__import __lmem volatile  entl_send_sig_num;
//#define RECEIVER_ISLAND_NUM     32
//#define RECEIVER_ME_NUM 4
// #define RECEIVER_THREAD_NUM 0



__intrinsic struct pkt_ms_info
my_pkt_msd_write(__addr40 void *pbuf, unsigned char off)
{
    SIGNAL sig;
    __xwrite uint32_t ms[2];

    return __my_pkt_msd_write(pbuf, off, ms, sizeof(ms), ctx_swap, &sig);
}

struct eth_hdr_padded {
    union {
        struct {
          struct eth_hdr hdr ;
          uint8_t padd[2] ;
        }  ;
      };
      uint32_t __raw[8];
} __PACKED;


int receive_packet( struct pkt_rxed *pkt_rxed, size_t size, uint64_t* s_addr )
{
    __xread struct pkt_rxed pkt_rxed_in;
    int island, pnum;
    int pkt_off;
    __mem struct pkt_hdr *pkt_hdr;
    uint8_t *wmem ;
    int64_t d_addr ;
    int64_t data ;
    int64_t d_value ;
    int ret = 0 ;
    unsigned int mbox0, mbox1, mbox2, mbox3 ;

    pkt_nbi_recv(&pkt_rxed_in, sizeof(pkt_rxed->nbi_meta));
    pkt_rxed->nbi_meta = pkt_rxed_in.nbi_meta;

    pkt_off  = PKT_NBI_OFFSET;
    island   = pkt_rxed->nbi_meta.pkt_info.isl;
    pnum     = pkt_rxed->nbi_meta.pkt_info.pnum;
    pkt_hdr  = pkt_ctm_ptr40(island, pnum, pkt_off);

    mem_read32(&(pkt_rxed_in.pkt_hdr), pkt_hdr, sizeof(pkt_rxed_in.pkt_hdr));
    pkt_rxed->pkt_hdr = pkt_rxed_in.pkt_hdr;
    wmem = &pkt_rxed->pkt_hdr.pkt ;
    d_addr = (uint64_t)wmem[0]<<40 | (uint64_t)wmem[1]<<32 | (uint64_t)wmem[2]<<24 | (uint64_t)wmem[3]<<16 | (uint64_t)wmem[4]<<8 | (uint64_t)wmem[5] ; 
    *s_addr = (uint64_t)wmem[6]<<40 | (uint64_t)wmem[7]<<32 | (uint64_t)wmem[8]<<24 | (uint64_t)wmem[9]<<16 | (uint64_t)wmem[10]<<8 | (uint64_t)wmem[11] ; 
    wmem = &pkt_rxed->pkt_hdr.data ;
    data = (uint64_t)wmem[0]<<56 | (uint64_t)wmem[1]<<48 | (uint64_t)wmem[2]<<40 | (uint64_t)wmem[3]<<32 | (uint64_t)wmem[4]<<24 | (uint64_t)wmem[5]<<16 |  (uint64_t)wmem[6]<<8 |(uint64_t)wmem[7] ; 
    if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_NOP ) {
      entl_data_out.d_addr = d_addr ;
      entl_data_out.data = data ;
      entl_data_out.port = pkt_rxed.nbi_meta.port ;
      if( (d_addr & ECLP_FW_MASK) == 0 )
      {
        entl_data_out.island = island ;
        entl_data_out.pnum = pnum ;
        local_csr_write(local_csr_mailbox2, island );
        local_csr_write(local_csr_mailbox3, pnum );      
      }
      else {
        entl_data_out.island = 0 ;
        entl_data_out.pnum = 0 ;        
      }
      ret = ENTL_ACTION_SEND ;
    }
    if( d_addr & ECLP_FW_MASK ) {
      ret |= ENTL_ACTION_FORWARD ;
    }

  mbox2 = d_addr >> 32 ;
  mbox3 = d_addr ;
  local_csr_write(local_csr_mailbox2, mbox2 );
  local_csr_write(local_csr_mailbox3, mbox3 );

#ifdef ENTL_STATE_DEBUG
    state.r_addr = d_addr ;
    state.r_data = data ;
#endif
    //data = (uint64_t)wmem[14]<<56 | (uint64_t)wmem[15]<<48 | (uint64_t)wmem[16]<<40 | (uint64_t)wmem[17]<<32 | (uint64_t)wmem[18]<<24 | (uint64_t)wmem[19]<<16 | (uint64_t)wmem[20]<<8 | (uint64_t)wmem[21] ; 
    //alo_command = ALO_OPCODE(d_addr) ; // keep it for nex_to_send
    //mbox0 = d_addr >> 32 ;
    //mbox1 = d_addr ;
    //mbox2 = data >> 32 ;
    //mbox3 = data ;
    //mbox0 = data >> 32 ;
    //mbox1 = data ;
    //local_csr_write(local_csr_mailbox0, mbox0 );
    //local_csr_write(local_csr_mailbox1, mbox1 );
    //local_csr_write(local_csr_mailbox0, mbox0 );
    //local_csr_write(local_csr_mailbox1, mbox1 );
    //local_csr_write(local_csr_mailbox2, mbox2 );
    //MUTEXLV_lock(state_lock,STATE_LOCK_BIT) ;
// int entl_received( __lmem entl_state_machine_t *mcn, uint64_t d_addr, uint64_t s_value, uint64_t *d_value, uint32_t ait_queue, uint32_t ait_command, uint32_t egress_queue ) ;
    //ret = entl_received( &state, d_addr, data, 0, 0, 0 ) ;
    //MUTEXLV_unlock(state_lock,STATE_LOCK_BIT) ;
    

    return ret;
}




int
main(void)
{

  
  struct pkt_rxed pkt_rxed; /* The packet header received by the thread */
  //__mem struct pkt_hdr *pkt_hdr;    /* The packet in the CTM */
  unsigned int mbox0, mbox1, mbox2 ;
  int ret ;
  int i = 0 ;
  if (__ctx() == 0) {
    reorder_ring_init() ;
  }
  sleep(500) ;
  //while( state.my_value != MY_VALUE ) {
  //sleep(100) ;            
  //}
  //mbox2 = state.my_value ;
  //local_csr_write(local_csr_mailbox2, mbox2 );
  //sleep(100) ;             
  //signal_ctx(0, __signal_number(&entl_send_sig)) ;  // send hello first       
  entl_data_out.d_addr = 0 ;
  entl_data_out.data = 0 ;
  entl_data_out.island = 0 ;
  entl_data_out.pnum = 0 ;
  //if (__ctx() == 0) {
    //local_csr_write(local_csr_mailbox0, 0x10000000 );
    // initial trigger to send hello  
    //entl_reflect( ENTL_SENDER_ME, __xfer_reg_number(&entl_data_in, ENTL_SENDER_ME),
    //  __signal_number(&entl_send_sig, ENTL_SENDER_ME), &entl_data_out,
    //  sizeof(entl_data_out)
    //) ;
    //local_csr_write(local_csr_mailbox0, 0x20000000 );
  //}
  //else {
    sleep(100) ;
  //}    
  for (;;) {
    uint64_t s_addr ;
    int sent = 0 ;
    //mbox0 = local_csr_read(local_csr_mailbox0) ;
    //mbox0 |= (1 << __ctx()) ;
    //local_csr_write(local_csr_mailbox0, mbox0 );
    ret = receive_packet(&pkt_rxed, sizeof(pkt_rxed), &s_addr);
    //local_csr_write(local_csr_mailbox1, ++i | (__ctx() << 16) );
    mbox0 = local_csr_read(local_csr_mailbox0) ;
    mbox0++ ;
    local_csr_write(local_csr_mailbox0, mbox0 );
    //mbox0 = (ret << 16) | 0x8000 | state.state.current_state ;
    //local_csr_write(local_csr_mailbox0, mbox0 );
    if( ret & ENTL_ACTION_SEND ) {
      uint32_t meid = get_entl_sender_meid( pkt_rxed.nbi_meta.port ) ;
      uint32_t me = get_entl_sender_me( pkt_rxed.nbi_meta.port ) ;
      //sleep(100) ;
      // __signal_me( RECEIVER_ISLAND_NUM, RECEIVER_ME_NUM, RECEIVER_THREAD_NUM, entl_send_sig_num ) ;
      //signal_ctx(0, __signal_number(&entl_send_sig)) ;                    
      entl_reflect( meid, __xfer_reg_number(&entl_data_in, me),
        __signal_number(&entl_send_sig, me), &entl_data_out,
        sizeof(entl_data_out)
      ) ;
      mbox1 = local_csr_read(local_csr_mailbox1) ;
      mbox1++ ;
      local_csr_write(local_csr_mailbox1, mbox1 );
      if( !(ret & ENTL_ACTION_FORWARD) ) {
        // participate in ordering 
        int ring = get_ring_num( pkt_rxed.nbi_meta.port) ;
        int sequence = pkt_rxed.nbi_meta.seq_num & REORDER_RING_SEQ_MASK ;
        reorder_lock( ring, sequence) ;
        reorder_unlock( ring, sequence ) ;
      }
    }
    if( ret & ENTL_ACTION_FORWARD) {
      int i ;
      int ring ;
      int sequence ;            
      // lookup forward table 
      __xread fw_table_entry_t entry ;
      __xwrite packet_data_t pkt_out

      read_entry( &entry, s_addr & ECLP_NLKUP_MASK ) ;

      ring = get_ring_num( pkt_rxed.nbi_meta.port ) ;
      sequence = pkt_rxed.nbi_meta.seq_num & REORDER_RING_SEQ_MASK ;
      // do reordering here
      reorder_lock( ring, sequence ) ;

      if( s_addr & ECLP_D_MASK ) {
        // rootward packet
        if( entry.parent == 0 ) {
          // sent to host, TBD
        }
        else {
          uint32_t port = entry.parent-1 ;
          uint32_t e_ring = get_island_ring( port ) ;
          pkt_out.nbi_num = get_nbi( port ) ;
          pkt_out.dir = 1 ; // set direction bit
          pkt_out.sequencer_num = port ; // = port number
          pkt_out.island_num = pkt_rxed.nbi_meta.pkt_info.isl;
          pkt_out.packet_num = pkt_rxed.nbi_meta.pkt_info.pnum ;
          pkt_out.bls = pkt_rxed.nbi_meta.pkt_info.bls ;
          pkt_out.len = pkt_rxed.nbi_meta.pkt_info.len ;
          pkt_out.seq_num = get_sequence_num( port ) ;
          pkt_out.next_lookup_id = entry.next_lookup[port] ;
          put_packet_data( &pkt_out, e_ring ) ;
        }
      }
      else {
        uint16_t fw_vector ;
        fw_vector = entry.fw_vector ;
        if( fw_vector & 1 ){
          // send to host, TBD
          sent = 1 ;
        }
        for( i = 1 ; i < 15 ; i++ ) {
          if( fw_vector & (1 << i) ) {
            uint32_t port = i - 1 ;
            uint32_t e_ring = get_island_ring( port ) ;
            if( port != pkt_rxed.nbi_meta.port ) // do not send to incomming port
            {
              pkt_out.nbi_num = get_nbi( port ) ;
              pkt_out.dir = 0 ; // set direction bit
              pkt_out.sequencer_num = port ; // = port number
              pkt_out.bls = pkt_rxed.nbi_meta.pkt_info.bls ;
              pkt_out.len = pkt_rxed.nbi_meta.pkt_info.len ;
              pkt_out.seq_num = get_sequence_num( port ) ;
              pkt_out.next_lookup_id = entry.next_lookup[port] ;
              pkt_out.island_num = pkt_rxed.nbi_meta.pkt_info.isl;
              if( sent ) {
                struct nbi_meta_pkt_info pkt_info;
                __xwrite uint32_t buf_xw[2];
                __addr40 void *ctm_ptr;
                __gpr struct pkt_clone_destination  destination;
                // need to copy the packet
                destination = pkt_clone(
                  pkt_rxed.nbi_meta.pkt_info.pnum,
                  PKT_CTM_SIZE_256,
                  (__addr40 __mem uint32_t *) pkt_rxed.nbi_meta.muptr,
                  pkt_rxed.nbi_meta.pkt_info.bls, // BLM_BLQ_0 ??
                  PKT_NBI_OFFSET + MAC_PREPEND_BYTES,
                  pkt_rxed.nbi_meta.pkt_info.len,
                  pkt_rxed.nbi_meta.pkt_info.split
                );
                pkt_out.packet_num = destination.ctm_pkt_num ;
                // update packet meta data
                pkt_info.isl = pkt_rxed.nbi_meta.pkt_info.isl ;
                pkt_info.pnum = destination.ctm_pkt_num ;
                pkt_info.split = pkt_rxed.nbi_meta.pkt_info.split ;
                pkt_info.muptr = blm_buf_ptr2handle(destination.mu_ptr);
                ctm_ptr = pkt_ctm_ptr40(__ISLAND, destination.ctm_pkt_num, 0);
                buf_xw[0] = pkt_info.__raw[0];
                buf_xw[1] = pkt_info.__raw[1];
                mem_write32(buf_xw, ctm_ptr, sizeof(buf_xw));
              }
              else {
                pkt_out.packet_num = pkt_rxed.nbi_meta.pkt_info.pnum ;
                sent = 1 ;
              }              
              put_packet_data( &pkt_out, e_ring ) ;
            }
          }
        }

      }

      // release reorder here
      reorder_unlock( ring, sequence ) ;
    }
    // sleep(100) ;

  } // for( ;; )
    
    
  return 0;
} 
