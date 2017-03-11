#include <nfp.h>
#include <me.h>
//#include "system_init_c.h"

__declspec(ctm) int old[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
__declspec(ctm) int new[sizeof(old)/sizeof(int)];

SIGNAL entl_send_sig;

int
main(void)
{

    if (__ctx() == 0) {
        int i ;
    	local_csr_write(local_csr_mailbox0, 0);
    	//system_init_wait_done();
        for (i = 0; i < 100 ; i++) {
        	wait_sig_mask(__signals(&entl_send_sig));
    		local_csr_write(local_csr_mailbox0, i+1);
        }
    }
    else if( __ctx() == 1 ) {
        int i ;
    	//system_init_wait_done();
        for (i = 0; i < 100 ; i++) {
        	sleep(100) ;
        	signal_ctx(0, __signal_number(&entl_send_sig)) ;
        }
    }
    return 0;
}	
