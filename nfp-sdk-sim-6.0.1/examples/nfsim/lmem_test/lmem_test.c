#include <nfp.h>
#include <me.h>
#include <mutexlv.h>
#include <stdint.h>
//#include "system_init_c.h"

#define ENTL_STATE_HELLO    1
#define ENTL_MESSAGE_ONLY     0x800000000000
 
// The data structre represents the internal state of ENTL
typedef struct entl_state {
  uint32_t event_i_know ;           // last received event number 
  uint32_t event_i_sent ;           // last event number sent
  uint32_t event_send_next ;            // next event number sent
  uint32_t current_state ;          // 0: idle  1: H 2: W 3:S 4:R
} entl_state_t ;

typedef struct entl_state_machine {
  entl_state_t state ;
  uint32_t error_flag ;             // first error flag 
  uint32_t p_error_flag ;           // when more than 1 error is detected, those error bits or ored to this flag
  uint32_t error_count ;                // Count multiple error, cleared in got_error_state()
  uint32_t state_count ;
  uint64_t my_addr ;
} entl_state_machine_t ;

__shared __lmem entl_state_machine_t state;

SIGNAL entl_send_sig;
 
#define MY_MAC_ADDR 0xbadbeef

int
main(void)
{

    if (__ctx() == 0) {
        int ret ;
        int64_t s_addr ;
        unsigned int mbox3, mbox2, mbox1 ;
    	local_csr_write(local_csr_mailbox0, 0);
        state.my_addr = MY_MAC_ADDR ; 
        mbox3 = state.my_addr ;
        local_csr_write(local_csr_mailbox3, mbox3);
        for (;;) {
        	wait_sig_mask(__signals(&entl_send_sig));
        }
    }
    else if( __ctx() == 1 ) {
        unsigned int mbox0, mbox1, mbox2 ;
        int64_t s_addr, d_addr ;
        int ret ;
        int i = 0 ;
        sleep(100) ;
        mbox2 = state.my_addr ;
        local_csr_write(local_csr_mailbox2, mbox2 );
        local_csr_write(local_csr_mailbox0, i++ );
        while( state.my_addr != MY_MAC_ADDR ) {
            sleep(100) ;            
            mbox2 = state.my_addr ;
            local_csr_write(local_csr_mailbox2, mbox2 );
            local_csr_write(local_csr_mailbox0, i++ );
        }
        mbox2 = state.my_addr ;
        local_csr_write(local_csr_mailbox3, mbox2 );
        sleep(100) ;

        for (;;) {
            mbox0 = local_csr_read(local_csr_mailbox0) ;
            mbox1 = local_csr_read(local_csr_mailbox1) ;
            mbox2 = local_csr_read(local_csr_mailbox2) ;            
            sleep(100) ;
            
        }
    }
    return 0;
}	
