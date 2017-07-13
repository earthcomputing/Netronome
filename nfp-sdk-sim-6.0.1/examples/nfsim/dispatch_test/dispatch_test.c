#include <nfp.h>
#include <me.h>
#include <mutexlv.h>
#include <stdint.h>
#include <pkt/pkt.h>
#include <net/eth.h>
#include <nfp/mem_bulk.h>
#include <vnic/utils/cls_ring.h>
#include "my_cls_ring.h"
#include "dispatch_data.h"

#ifdef __NFP_TOOL_NFCC
#include <nfp.h>
#define __PACKED __packed
#else
#define __PACKED __attribute__((packed))
#endif


#ifndef _link_sym
#define _link_sym(x) __link_sym(#x)
#endif

__export __shared __cls __align(PACKET_RING_SIZE_LW*sizeof(packet_data_t)) char packet_ring_mem[PACKET_RING_SIZE_LW*sizeof(packet_data_t)] ;

//#define INITIAL_DELAY 1000
//#define MIDDLE_DELAY 0
#define INITIAL_DELAY 0
#define MIDDLE_DELAY 1000


int
main(void)
{
    if (__ctx() == 0) {
        SIGNAL sig;
        //unsigned int mbox3, mbox2, mbox1, mbox0 ;
        unsigned int seq_num = 0x1000 ;
        __xread packet_data_t pkt_in;
        __gpr packet_data_t pkt_in_reg ;
        __xread unsigned int ring_ptr ;
        unsigned int dly = INITIAL_DELAY ;
    
        local_csr_write(local_csr_mailbox0, seq_num++);    

        cls_ring_setup(PACKET_RING_NUM,
                   (__cls void *)packet_ring_mem,
                   (PACKET_RING_SIZE_LW*sizeof(packet_data_t)));

        while( dly ) {
            local_csr_write(local_csr_mailbox0, seq_num++); 
            cls_read_ring_ptr( &ring_ptr, PACKET_RING_NUM ) ;
            local_csr_write(local_csr_mailbox3, ring_ptr);      
            sleep(50) ;
            dly -= 50 ;
            //local_csr_write(local_csr_mailbox0, seq_num++); 
        }
        local_csr_write(local_csr_mailbox0, seq_num++); 
        //cls_ring_workq_add_thread_ptr32(&pkt_in, PACKET_RING_NUM, 4, ctx_swap, &sig ) ;
        cls_ring_workq_add_thread_ptr40(&pkt_in, PACKET_RING_NUM, 4, 32, ctx_swap, &sig ) ;
        local_csr_write(local_csr_mailbox0, seq_num++);   
        pkt_in_reg = pkt_in ;
        //pkt_in_reg.__raw[0] = pkt_in.__raw[0] ;

        local_csr_write(local_csr_mailbox1, pkt_in_reg.__raw[2]);    
        local_csr_write(local_csr_mailbox2, pkt_in_reg.__raw[3]); 

        for(;;) {
            sleep(800) ;
            local_csr_write(local_csr_mailbox0, seq_num++); 
            cls_read_ring_ptr( &ring_ptr, PACKET_RING_NUM ) ;
            local_csr_write(local_csr_mailbox3, ring_ptr);      
            cls_ring_workq_add_thread_ptr40(&pkt_in, PACKET_RING_NUM, 4, 32, ctx_swap, &sig ) ;
            //cls_ring_workq_add_thread_ptr32(&pkt_in, PACKET_RING_NUM, 4, ctx_swap, &sig ) ;
            local_csr_write(local_csr_mailbox0, seq_num++);   
            pkt_in_reg = pkt_in ;
            //pkt_in_reg.__raw[0] = pkt_in.__raw[0] ;
            local_csr_write(local_csr_mailbox1, pkt_in_reg.__raw[2]);    
            local_csr_write(local_csr_mailbox2, pkt_in_reg.__raw[3]);            
        }


    }
    else 
    {
        SIGNAL sig;
        unsigned int ctx = __ctx() ;
        unsigned int dly = INITIAL_DELAY + (100 * ctx) + ((ctx>2)?MIDDLE_DELAY:0); // Half will get the work immediately
        unsigned int seq_num = 0x1000 * ctx ;
        __xread packet_data_t pkt_in;
        __gpr packet_data_t pkt_in_reg ;
        __xread unsigned int ring_ptr ;

        sleep(dly) ;
        local_csr_write(local_csr_mailbox0, seq_num++); 
        cls_read_ring_ptr( &ring_ptr, PACKET_RING_NUM ) ;
        local_csr_write(local_csr_mailbox3, ring_ptr);      
        cls_ring_workq_add_thread_ptr40(&pkt_in, PACKET_RING_NUM, 4, 32, ctx_swap, &sig ) ;
        //cls_ring_workq_add_thread_ptr32(&pkt_in, PACKET_RING_NUM, 4, ctx_swap, &sig ) ;
        local_csr_write(local_csr_mailbox0, seq_num++);   
        pkt_in_reg = pkt_in ;
        //pkt_in_reg.__raw[0] = pkt_in.__raw[0] ;
        local_csr_write(local_csr_mailbox1, pkt_in_reg.__raw[2]);    
        local_csr_write(local_csr_mailbox2, pkt_in_reg.__raw[3]);    
        //local_csr_write(local_csr_mailbox3, pkt_in_reg.__raw[2]);    
        for(;;) {
            sleep(800) ;
            local_csr_write(local_csr_mailbox0, seq_num++); 
            cls_read_ring_ptr( &ring_ptr, PACKET_RING_NUM ) ;
            local_csr_write(local_csr_mailbox3, ring_ptr);      
            cls_ring_workq_add_thread_ptr40(&pkt_in, PACKET_RING_NUM, 4, 32, ctx_swap, &sig ) ;
            //cls_ring_workq_add_thread_ptr32(&pkt_in, PACKET_RING_NUM, 4, ctx_swap, &sig ) ;
            local_csr_write(local_csr_mailbox0, seq_num++);   
            pkt_in_reg = pkt_in ;
            //pkt_in_reg.__raw[0] = pkt_in.__raw[0] ;
            local_csr_write(local_csr_mailbox1, pkt_in_reg.__raw[2]);    
            local_csr_write(local_csr_mailbox2, pkt_in_reg.__raw[3]);            
        }
 
        sleep(29900) ;

    }
    
    return 0;
}	
