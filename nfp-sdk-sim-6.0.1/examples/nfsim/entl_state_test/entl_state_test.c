#include <nfp.h>
#include <me.h>
#include <mutexlv.h>
//#include "system_init_c.h"
#include "entl_state_machine.h"

//__declspec(ctm) int old[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
//__declspec(ctm) int new[sizeof(old)/sizeof(int)];

__shared __lmem entl_state_machine_t state;
MUTEXLV state_lock = 0;
#define STATE_LOCK_BIT 0

SIGNAL entl_send_sig;

__intrinsic int wait_with_timeout( unsigned int cycle ) {
    SIGNAL entl_timeout_sig;
    set_alarm( cycle, &entl_timeout_sig );
    wait_sig_mask(__signals(&entl_send_sig) | __signals(&entl_timeout_sig));
    return signal_test(&entl_timeout_sig);
}

#define RETRY_CYCLE 1000
#define MY_MAC_ADDR 0xbadbeef
int
main(void)
{
 
    if (__ctx() == 0) {
        int ret ;
        int64_t d_addr ;
        unsigned int mbox3, mbox2, mbox1 ;
    	local_csr_write(local_csr_mailbox0, 0);
        MUTEXLV_lock(state_lock,STATE_LOCK_BIT) ;
        entl_state_init( &state ) ;
        //entl_set_random_addr( &state ) ;
        state.my_addr = MY_MAC_ADDR ; //((addr << 32 ) | l_rand()) & 0xffffffffffff ;
        state.state.current_state = ENTL_STATE_HELLO ;
        MUTEXLV_unlock(state_lock,STATE_LOCK_BIT) ;
        for (;;) {
        	ret = wait_with_timeout(RETRY_CYCLE);
            MUTEXLV_lock(state_lock,STATE_LOCK_BIT) ;
            ret = entl_next_send( &state, &d_addr, 0 ) ; 
            MUTEXLV_unlock(state_lock,STATE_LOCK_BIT) ;
            if( ret ) d_addr |= ENTL_MESSAGE_ONLY ;
            mbox1 = d_addr ;
    		local_csr_write(local_csr_mailbox1, mbox1);
            mbox2 = state.my_addr ; 
            local_csr_write(local_csr_mailbox2, mbox2);
            mbox3 = (d_addr >>16 &0xffff0000) | 0x8000 | state.state.current_state ;
            local_csr_write(local_csr_mailbox3, mbox3);
        }
    }
    else if( __ctx() == 1 ) {
        unsigned int mbox0, mbox1, mbox2 ;
        int64_t s_addr ;
        int64_t d_addr ;
        int ret ;
        int i = 0 ;
        //entl_set_random_addr( &state ) ;
        //state.my_addr = 0xbadbeef ; //((addr << 32 ) | l_rand()) & 0xffffffffffff ;
        sleep(100) ;
        //mbox2 = state.my_addr ;
        //local_csr_write(local_csr_mailbox2, mbox2 );
        while( state.my_addr != MY_MAC_ADDR ) {
            sleep(100) ;            
        }
        mbox2 = state.my_addr ;
        local_csr_write(local_csr_mailbox2, mbox2 );
        //MUTEXLV_lock(state_lock,STATE_LOCK_BIT) ;
        //local_csr_write(local_csr_mailbox3, state.state.current_state << 16 | 0x4000);
        // mbox2 = state.my_addr ;
        // local_csr_write(local_csr_mailbox2, mbox2 | 0x80000000 );
        //MUTEXLV_unlock(state_lock,STATE_LOCK_BIT) ;
        //sleep(100) ;
        sleep(100) ;             
        signal_ctx(0, __signal_number(&entl_send_sig)) ;  // send hello first       
        sleep(100) ;             
        for (;;) {
            mbox0 = local_csr_read(local_csr_mailbox0) ;
            mbox1 = local_csr_read(local_csr_mailbox1) ;
            mbox2 = local_csr_read(local_csr_mailbox2) ;
            //local_csr_write(local_csr_mailbox3, i++);
            if( mbox0 != 0 ) {
                local_csr_write(local_csr_mailbox0, 0);  // clear command
                s_addr = (((int64_t)mbox0 & 0xffff) << 32) | mbox1 ;
                //s_addr =  mbox1 ;
                d_addr = (((int64_t)mbox0 & 0xffff0000) << 16) | mbox2 ;
                MUTEXLV_lock(state_lock,STATE_LOCK_BIT) ;
                ret = entl_received( &state, s_addr, d_addr, 0, 0 ) ;
                MUTEXLV_unlock(state_lock,STATE_LOCK_BIT) ;
                //mbox1 = state.my_addr >> 16 ; //< s_addr?0x200:0x010 ;
                //local_csr_write(local_csr_mailbox1, mbox1) ;
                //mbox2 = s_addr >> 16 ; // state.state.current_state | 0x4000 ;
                //local_csr_write(local_csr_mailbox2, mbox2 ) ;
                //local_csr_write(local_csr_mailbox1, state.state.current_state << 16 | ret | 0x8000 );
                //if( ret & ENTL_ACTION_SET_ADDR ) {
                //if( state.my_addr > s_addr ) {
                //    mbox2 = state.my_addr ;
                //    mbox3 = s_addr ;
                //}
                //else {
                //    mbox2 = s_addr ;                    
                //    mbox3 = state.my_addr ;
                //}
                //local_csr_write(local_csr_mailbox2, mbox2 );
                //local_csr_write(local_csr_mailbox3, mbox3 );
                //}
                if( ret & ENTL_ACTION_SEND ) {
                    //sleep(100) ;
                    signal_ctx(0, __signal_number(&entl_send_sig)) ;                    
                }
            }
            //else {
            //    mbox2 = state.my_addr ;
            //    local_csr_write(local_csr_mailbox2, mbox2 | 0x40000000 );                
            //}
            
            sleep(100) ;
            
        }
    }
    return 0;
}	
