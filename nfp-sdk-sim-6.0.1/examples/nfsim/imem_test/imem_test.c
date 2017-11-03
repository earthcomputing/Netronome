#include <nfp.h>
#include <me.h>
#include <mutexlv.h>
#include <stdint.h>
#include <pkt/pkt.h>
#include <net/eth.h>
#include <nfp/mem_bulk.h>

#ifdef __NFP_TOOL_NFCC
#include <nfp.h>
#define __PACKED __packed
#else
#define __PACKED __attribute__((packed))
#endif

#define BACKOFF_SLEEP           256

#ifndef NBI
#define NBI 0
#endif

#define MY_MAC_ADDR 0xbadbeef
#define RETRY_CYCLE 1000

// FW table 65536 64-byte-entry per imem 
#define FW_TABLE_ENTRY_SIZE 64
#define FW_TABLE_SIZE 0x10000

// Only one ME in the system should define FW_TABLE_ALLOCATE, others should define FW_TABLE_INPORT on compile time
#define FW_TABLE_ALLOCATE
#include "fw_table_access.h"

int
main(void)
{
    if (__ctx() == 0) {
        unsigned int mbox3, mbox2, mbox1, mbox0 ;
        fw_table_entry_t entry, entry1 ;
        //unsigned int seq_num0 = 1 ;
        //unsigned int seq_num = 1 ;
        int i , j;
        // volatile __xwrite struct imem_entry _w_entry ;
        volatile __xread fw_table_entry_t _r_entry ;
        mbox3 = 0x100 ;
        local_csr_write(local_csr_mailbox3, mbox3 );    // 3, 4
        sleep(100) ;     
        mbox3 = 0x200 ;
        local_csr_write(local_csr_mailbox3, mbox3 );    // 3, 4
        for( i = 0 ; i < 100 ; i++ ){
          //mbox3 = 0x300 ;
          //local_csr_write(local_csr_mailbox3, mbox3 );    // 3, 4
          read_entry( &_r_entry, i ) ;
          //mbox3 = 0x400 ;
          //local_csr_write(local_csr_mailbox3, mbox3 );    // 3, 4
          //for( j = 0 ; j < 16 ; j++ ) {
          entry = _r_entry ;
          //  entry.__raw[j] = _r_entry.__raw[j] ;
          //  mbox3 = 0x500 + j ;
          //  local_csr_write(local_csr_mailbox3, mbox3 );    // 3, 4
          //}
          mbox3 = 0x600 ;
          local_csr_write(local_csr_mailbox3, mbox3 );    // 3, 4
          // uint16_t fw_vector ;      // bit 0 to host
          // unsigned int parent:4 ;     // parent port of this tree
          // unsigned int drop_host:1 ;  // do not forward packet from host
          // unsigned int reserved:3 ;   
          // uint32_t next_lookup[15] ;  // next lookup field for the packet

          mbox0 = entry.fw_vector | ((unsigned int)entry.parent << 16) | ((unsigned int)entry.drop_host << 24) ;
          mbox1 = entry.__raw[0] ;
          //mbox1 = entry.next_lookup[0] ;
          //mbox1 = entry.__raw[1] ;
          entry.__raw[0] = 0 ;
          entry1.fw_vector = 1 << (i%16) ;
          entry1.parent = i ;
          entry1.drop_host = (i & 1) ;
          mbox2 = entry1.__raw[0];
          //mbox2 = entry.__raw[2];
          mbox3 = entry1.fw_vector | ((unsigned int)entry1.parent << 16) | ((unsigned int)entry1.drop_host << 24) ;;
          local_csr_write(local_csr_mailbox0, mbox0 );
          local_csr_write(local_csr_mailbox1, mbox1 );
          local_csr_write(local_csr_mailbox2, mbox2 );
          local_csr_write(local_csr_mailbox3, mbox3 );    // 3, 4
          sleep(100) ;               
        }
        
        sleep(1000) ;     

    }
    else {
      sleep(1000000) ;
    }
    
    return 0;
}	
