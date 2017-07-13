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

__import __shared __cls __align(PACKET_RING_SIZE_LW*sizeof(packet_data_t)) char packet_ring_mem[PACKET_RING_SIZE_LW*sizeof(packet_data_t)] ;

#define RING_OFFSET 0x90000

//  #define INITIAL_DELAY 900

#define INITIAL_DELAY 500

int
main(void)
{
    {
        SIGNAL sig;
        unsigned int ctx = __ctx() ;
        unsigned int seq_num = 0x1000 * (ctx + 1) ;
        __xwrite packet_data_t pkt_out;
        __gpr packet_data_t pkt_out_reg ;
        unsigned int dly = INITIAL_DELAY + (100 * ctx) ; // reprder the sequence
        unsigned int sequence = RING_OFFSET + seq_num  ;
        pkt_out_reg.__raw[0] = sequence ;
        pkt_out_reg.__raw[1] = sequence + 1 ;
        pkt_out_reg.__raw[2] = sequence + 2 ;
        pkt_out_reg.__raw[3] = sequence + 3 ;
        pkt_out = pkt_out_reg ;
        sleep(dly) ;
        local_csr_write(local_csr_mailbox0, seq_num++); 
        cls_ring_workq_add_work_ptr40(&pkt_out, PACKET_RING_NUM, 4, 32, ctx_swap, &sig ) ;
        local_csr_write(local_csr_mailbox0, seq_num++);   
        pkt_out = pkt_out_reg ;
        //pkt_in_reg.__raw[0] = pkt_in.__raw[0] ;
        local_csr_write(local_csr_mailbox1, pkt_out_reg.__raw[0]);    
        local_csr_write(local_csr_mailbox2, pkt_out_reg.__raw[1]);    
        local_csr_write(local_csr_mailbox3, pkt_out_reg.__raw[2]);    
        for(;;) {
            sleep(800) ;
            // second try
            local_csr_write(local_csr_mailbox0, seq_num++); 
            cls_ring_workq_add_work_ptr40(&pkt_out, PACKET_RING_NUM, 4, 32, ctx_swap, &sig ) ;
            local_csr_write(local_csr_mailbox0, seq_num++);   
            pkt_out = pkt_out_reg ;
            //pkt_in_reg.__raw[0] = pkt_in.__raw[0] ;
            local_csr_write(local_csr_mailbox1, pkt_out_reg.__raw[0]);    
            local_csr_write(local_csr_mailbox2, pkt_out_reg.__raw[1]);    
            local_csr_write(local_csr_mailbox3, pkt_out_reg.__raw[2]);                        
        }
 
        sleep(10000) ;
    }
    
    return 0;
}	
