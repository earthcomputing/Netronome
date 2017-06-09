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


 __asm { .alloc_mem fw_table0 imem0 global (FW_TABLE_ENTRY_SIZE*FW_TABLE_SIZE) (FW_TABLE_ENTRY_SIZE) }
 __asm { .alloc_mem fw_table1 imem1 global (FW_TABLE_ENTRY_SIZE*FW_TABLE_SIZE) (FW_TABLE_ENTRY_SIZE) }

__intrinsic __addr40 void *
imem_ptr40(unsigned int off)
{
    __gpr unsigned int lo;
    __gpr unsigned int hi;
    unsigned char isl ;

    lo = off << 6 ;  // 64 byte per entry

    if( lo & 0x400000) {  // iMem has 4M sram (22bit)
      isl = 0x1d ;     // Internal Mem 1
    }
    else {
      isl = 0x1c ;     // Internal Mem 0
    }

    hi = 0x80 | isl;

    return (__addr40 void *)
        (((unsigned long long)hi << 32) | (unsigned long long)lo);
}

struct imem_entry {
    uint32_t __raw[16] ; // 64 byte
} __PACKED;  


__intrinsic void update_entry ( volatile __xwrite struct imem_entry *entry, uint32_t offset ) {
  unsigned int mbox0 ;
  unsigned int mbox1 ;
  unsigned int j ;
  __addr40 void *entry_addr = imem_ptr40(offset) ;
  mem_write32( (void*)&entry, (__addr40 uint8_t *)entry_addr , sizeof(struct imem_entry) );
  mbox0 = (unsigned int)entry_addr ;
  mbox1 = (unsigned int)((unsigned long long)entry_addr >> 32) ;
  local_csr_write(local_csr_mailbox0, mbox0 );
  local_csr_write(local_csr_mailbox1, mbox1 );

}

__intrinsic void read_entry ( volatile __xread struct imem_entry *entry, uint32_t offset ) {
  unsigned int mbox0 ;
  unsigned int mbox1 ;
  __addr40 void *entry_addr = imem_ptr40(offset) ;
  mem_read32( (void*)entry, (__addr40 uint8_t *)entry_addr , sizeof(struct imem_entry) );
  mbox0 = (unsigned int)entry_addr ;
  mbox1 = (unsigned int)((unsigned long long)entry_addr >> 32) ;
  local_csr_write(local_csr_mailbox0, mbox0 );
  local_csr_write(local_csr_mailbox1, mbox1 );
}

int
main(void)
{
    if (__ctx() == 0) {
        unsigned int mbox3, mbox2, mbox1, mbox0 ;
        unsigned int seq_num0 = 1 ;
        unsigned int seq_num = 1 ;
        int i, j ;
        volatile __xwrite struct imem_entry _w_entry ;
        volatile __xread struct imem_entry _r_entry ;
        __addr40 void *entry_addr ;
        for( i = 0 ; i < 4 ; i++ ){
          for( j = 0 ; j < 16 ; j++ ) {
            local_csr_write(local_csr_mailbox0, seq_num0++);    
            _w_entry.__raw[j] = (i << 16) | j ;
          }
          //entry_addr = imem_ptr40(i) ;
          //mem_write32( (void*)&_w_entry, (__addr40 uint8_t *)entry_addr , sizeof(struct imem_entry) );
          update_entry( &_w_entry, i ) ;
          sleep(200) ;                   // let tester see the address on mbox 0, 1
          local_csr_write(local_csr_mailbox3, seq_num++);    // 1, 2
          sleep(100) ;     
        }
        for( i = 0 ; i < 4 ; i++ ){
          read_entry( &_r_entry, i) ;
          sleep(200) ;                   // let tester see the address on mbox 0, 1
          mbox0 = _r_entry.__raw[0] ;
          mbox1 = _r_entry.__raw[1] ;
          mbox2 = _r_entry.__raw[2];
          local_csr_write(local_csr_mailbox0, mbox0 );
          local_csr_write(local_csr_mailbox1, mbox1 );
          local_csr_write(local_csr_mailbox2, mbox2 );
          local_csr_write(local_csr_mailbox3, seq_num++);    // 3, 4
          sleep(100) ;     
        }
        
        sleep(1000) ;     

    }
    else {
      sleep(1000000) ;
    }
    
    return 0;
}	
