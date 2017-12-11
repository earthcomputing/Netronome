#include <nfp.h>
#include <me.h>
#include <mutexlv.h>
#include <stdint.h>
#include <pkt/pkt.h>
#include <net/eth.h>
#include <nfp/mem_bulk.h>
//#include "system_init_c.h"
#include "entl_reflect.h"

//#define ENTL_SENDER_ME 32
#define ENTL_SENDER_ME 0
#define ENTL_SENDER_ISL 32
#define ENTL_SENDER_ISLME __nfp_meid(ENTL_SENDER_ISL,ENTL_SENDER_ME)

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

__remote __xread entl_reflect_data_t entl_data_in ;

__declspec(remote) SIGNAL entl_send_sig ;

#define CTM_ALLOC_ERR 0xffffffff
#define MY_VALUE 0xbeef
#define RETRY_CYCLE 1000

//__import __lmem volatile uint64_t  keep_pkt_num ;
//__shared uint32_t alo_command ;
//__shared int  next_flag ;

//__lmem volatile entl_state_machine_t state;
// MUTEXLV state_lock = 0;
//#define STATE_LOCK_BIT 0

//__import __lmem volatile  entl_send_sig_num;
//#define RECEIVER_ISLAND_NUM     32
//#define RECEIVER_ME_NUM 4
// #define RECEIVER_THREAD_NUM 0


struct eth_hdr_padded {
    union {
        struct {
          struct eth_hdr hdr ;
          uint8_t padd[2] ;
        }  ;
      };
      uint32_t __raw[8];
} __PACKED;


int
main(void)
{

    {
      struct pkt_rxed pkt_rxed; /* The packet header received by the thread */
      //__mem struct pkt_hdr *pkt_hdr;    /* The packet in the CTM */
        unsigned int mbox0, mbox1, mbox2 ;
        int ret ;
        int i = 0 ;
        //while( state.my_value != MY_VALUE ) {
        //sleep(100) ;            
        //}
        //mbox2 = state.my_value ;
        //local_csr_write(local_csr_mailbox2, mbox2 );
        //sleep(100) ;             
        //signal_ctx(0, __signal_number(&entl_send_sig)) ;  // send hello first       
        if( __ctx() == 0 ) {
          local_csr_write(local_csr_mailbox3, 55 );
          sleep(500) ;
          for( i = 0 ; i < 1000 ; i++ ) {
            entl_data_out.d_addr = i + 500 ;
            entl_data_out.data = i + 1000 ;
            entl_data_out.island = 0 ;
            entl_data_out.pnum = 0 ;
            // initial trigger to send hello  
            mbox2 = i + 200 ;
            local_csr_write(local_csr_mailbox2, mbox2 );
            /*
            entl_reflect_data( ENTL_SENDER_ISLME, __xfer_reg_number(&entl_data_in, ENTL_SENDER_ISLME),
              __signal_number(&entl_send_sig, ENTL_SENDER_ISLME), &entl_data_out,
              sizeof(entl_data_out)
            ) ;
            */
            /* */
            entl_reflect_data_ptr40(
              &entl_data_out, ENTL_SENDER_ISL, ENTL_SENDER_ME, __xfer_reg_number(&entl_data_in, ENTL_SENDER_ISLME),
              __signal_number(&entl_send_sig, ENTL_SENDER_ISLME), 8
            ) ;
            /* */
            mbox1 = i + 200 ;
            local_csr_write(local_csr_mailbox3, mbox1 );
          }
        }
        else sleep(10000) ;       
        
    }
    return 0;
}	
