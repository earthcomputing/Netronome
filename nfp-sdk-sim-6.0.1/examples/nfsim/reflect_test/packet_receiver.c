#include <nfp.h>
#include <me.h>
#include <mutexlv.h>
#include <stdint.h>
#include <pkt/pkt.h>
#include <net/eth.h>
#include <nfp/mem_bulk.h>
//#include "system_init_c.h"
#include "entl_reflect.h"
#include "entl_state_machine.h"

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

__shared __lmem volatile entl_state_machine_t state;

struct eth_hdr_padded {
    union {
        struct {
          struct eth_hdr hdr ;
          uint8_t padd[2] ;
        }  ;
      };
      uint32_t __raw[8];
} __PACKED;


int receive_packet( struct pkt_rxed *pkt_rxed, size_t size )
{
    __xread struct pkt_rxed pkt_rxed_in;
    int island, pnum;
    int pkt_off;
    __mem struct pkt_hdr *pkt_hdr;
    uint8_t *wmem ;
    int64_t s_addr ;
    int64_t d_addr ;
    int64_t data ;
    int64_t d_value ;
    int ret ;
    unsigned int mbox2, mbox3 ;

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
    s_addr = (uint64_t)wmem[6]<<40 | (uint64_t)wmem[7]<<32 | (uint64_t)wmem[8]<<24 | (uint64_t)wmem[9]<<16 | (uint64_t)wmem[10]<<8 | (uint64_t)wmem[11] ; 
    wmem = &pkt_rxed->pkt_hdr.data ;
    data = (uint64_t)wmem[0]<<56 | (uint64_t)wmem[1]<<48 | (uint64_t)wmem[2]<<40 | (uint64_t)wmem[3]<<32 | (uint64_t)wmem[4]<<24 | (uint64_t)wmem[5]<<16 |  (uint64_t)wmem[6]<<8 |(uint64_t)wmem[7] ; 
    //if( GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_NOP ) {
      entl_data_out.d_addr = d_addr ;
      entl_data_out.data = data ;
      if( (d_addr & ECLP_FW_MASK) == 0 )
      {
        entl_data_out.island = island ;
        entl_data_out.pnum = pnum ;
        //local_csr_write(local_csr_mailbox2, island );
        //local_csr_write(local_csr_mailbox3, pnum );      
      }
      else {
        entl_data_out.island = 0 ;
        entl_data_out.pnum = 0 ;        
      }
      ret = ENTL_ACTION_SEND ;
    //}

  mbox2 = d_addr  ;
  mbox3 = __ctx() ;
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

    {
      struct pkt_rxed pkt_rxed; /* The packet header received by the thread */
      //__mem struct pkt_hdr *pkt_hdr;    /* The packet in the CTM */
        unsigned int mbox0, mbox1, mbox2 ;
        int ret ;
        int i = 0 ;
        if( __ctx() == 0 ) {
          mbox0 = 1 ;
          local_csr_write(local_csr_mailbox0, mbox0 );

          sleep(500) ;
          entl_data_out.d_addr = 0 ;
          entl_data_out.data = 0 ;
          entl_data_out.island = 0 ;
          entl_data_out.pnum = 0 ;
          entl_reflect( ENTL_SENDER_ME, __xfer_reg_number(&entl_data_in, ENTL_SENDER_ME),
            __signal_number(&entl_send_sig, ENTL_SENDER_ME), &entl_data_out,
            sizeof(entl_data_out)
          ) ;
          mbox1 = 1 ;
          local_csr_write(local_csr_mailbox1, mbox1 );
        }
        else {
                  sleep(500) ;
        }
        //while( state.my_value != MY_VALUE ) {
        //sleep(100) ;            
        //}
        //mbox2 = state.my_value ;
        //local_csr_write(local_csr_mailbox2, mbox2 );
        //sleep(100) ;             
        //signal_ctx(0, __signal_number(&entl_send_sig)) ;  // send hello first       

    //if (__ctx() == 0) {
        //local_csr_write(local_csr_mailbox0, 0x10000000 );
        // initial trigger to send hello  

        //local_csr_write(local_csr_mailbox0, 0x20000000 );
      //}
      //else {
        sleep(100) ;       
        for (;;) {
          //mbox0 = local_csr_read(local_csr_mailbox0) ;
          //mbox0 |= (1 << __ctx()) ;
          //local_csr_write(local_csr_mailbox0, mbox0 );
          ret = receive_packet(&pkt_rxed, sizeof(pkt_rxed));
          //local_csr_write(local_csr_mailbox1, ++i | (__ctx() << 16) );
          mbox0 = local_csr_read(local_csr_mailbox0) ;
          mbox0++ ;
          local_csr_write(local_csr_mailbox0, mbox0 );
          //mbox0 = (ret << 16) | 0x8000 | state.state.current_state ;
          //local_csr_write(local_csr_mailbox0, mbox0 );
          //if( ret & ENTL_ACTION_SEND ) {
            //sleep(100) ;
            // __signal_me( RECEIVER_ISLAND_NUM, RECEIVER_ME_NUM, RECEIVER_THREAD_NUM, entl_send_sig_num ) ;
            //signal_ctx(0, __signal_number(&entl_send_sig)) ;                    
            entl_reflect( ENTL_SENDER_ME, __xfer_reg_number(&entl_data_in, ENTL_SENDER_ME),
              __signal_number(&entl_send_sig, ENTL_SENDER_ME), &entl_data_out,
              sizeof(entl_data_out)
            ) ;
            mbox1 = local_csr_read(local_csr_mailbox1) ;
            mbox1++ ;
            local_csr_write(local_csr_mailbox1, mbox1 );

          //}            
          // sleep(100) ;
    
        }
      }
    //}
    return 0;
}	
