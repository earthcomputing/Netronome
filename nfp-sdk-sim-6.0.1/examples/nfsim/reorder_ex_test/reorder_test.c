#include <nfp.h>
#include <me.h>
#include <mutexlv.h>
#include <stdint.h>
#include <pkt/pkt.h>
#include <net/eth.h>
#include <nfp/mem_bulk.h>
#include <vnic/utils/cls_ring.h>

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
#define REORDER_RING_ADDR 0
#define REORDER_RING_NUM 0
#define REORDER_RING_ISLAND 32
#define REORDER_RING_SIZE_LW 64

// FW table 65536 64-byte-entry per imem 
#define FW_TABLE_ENTRY_SIZE 64
#define FW_TABLE_SIZE 0x10000

#define RETRY_CYCLE 1000

#ifndef _link_sym
#define _link_sym(x) __link_sym(#x)
#endif

 //__asm { .alloc_mem reorder_ring_mem cls+REORDER_RING_ADDR island (REORDER_RING_SIZE_LW*4) (REORDER_RING_SIZE_LW*4) }

__export __shared __cls __align(REORDER_RING_SIZE_LW*4) char reorder_ring_mem[REORDER_RING_SIZE_LW*4] ;
 //__asm { .alloc_mem fw_table0 imem0 global (FW_TABLE_ENTRY_SIZE*FW_TABLE_SIZE) (FW_TABLE_ENTRY_SIZE) }
 //__asm { .alloc_mem fw_table1 imem1 global (FW_TABLE_ENTRY_SIZE*FW_TABLE_SIZE) (FW_TABLE_ENTRY_SIZE) }

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

//////////////////////////////////////////////////////////////////////
//  nfp_cls_ring code copied here (original doesn't compile)
//////////////////////////////////////////////////////////////////////
#define CT_ASSERT(expr) __ct_assert(expr, #expr)
//#define RT_RANGE_ASSERT(exp) RT_ASSERT(exp)
#define RT_ALIGN_ASSERT(exp)
#define RT_RANGE_ASSERT(exp)
#define RT_ASSERT(exp)
#define _INTRINSIC_BEGIN
#define _INTRINSIC_END

#define _CLS_RING_NUMBER_CHECK(ring_number)                             \
        if (__is_ct_const(ring_number))                                 \
        {                                                               \
            CT_ASSERT(ring_number <= 15);                               \
        }                                                               \
        else                                                            \
        {                                                               \
            RT_RANGE_ASSERT(ring_number <= 15);                         \
        }

#define _CLS_RING_OFFSET_CHECK(offset)                                  \
        if (__is_ct_const(offset))                                      \
        {                                                               \
            CT_ASSERT(offset <= 0x3fff);                                \
        }                                                               \
        else                                                            \
        {                                                               \
            RT_RANGE_ASSERT(offset <= 0x3fff);                          \
        }

#define _CLS_RING_ISLAND_CHECK(island_number)                           \
        if (__is_ct_const(island_number))                               \
        {                                                               \
            CT_ASSERT(island_number <= 0x3f);                           \
        }                                                               \
        else                                                            \
        {                                                               \
            RT_ASSERT(island_number <= 0x3f);                           \
        }

#define _CLS_RING_ADDRESS_ORDERED_LOCK(address, ring, offset)                    \
    address = (0xfff << 20) | ((ring & 0x0f) << 16) | ((offset & 0x3fff) << 2) ;


#define _CLS_RING_SET_LOCALITY(locality, island)            \
    locality = (island_id & 0x3f) << 26;

__intrinsic
void cls_ring_ordered_lock_ptr32(
    unsigned int ring_number,
    unsigned int sequence_number,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
        __gpr unsigned int ring_address;

        CT_ASSERT(__is_ct_const(sync));
        CT_ASSERT(sync == sig_done || sync == ctx_swap);
        _CLS_RING_NUMBER_CHECK(ring_number);
        _CLS_RING_OFFSET_CHECK(sequence_number);

        _CLS_RING_ADDRESS_ORDERED_LOCK(ring_address, ring_number, sequence_number);
        if (sync == sig_done)
        {
            __asm
            {
                cls[ring_ordered_lock, --, ring_address, 0], sig_done[*sig_ptr]
            }
        }
        else
        {
            __asm
            {
                cls[ring_ordered_lock, --, ring_address, 0], ctx_swap[*sig_ptr]
            }
        }
    }
_INTRINSIC_END;
}



__intrinsic
void cls_ring_ordered_lock_ptr40(
    unsigned int ring_number,
    unsigned int sequence_number,
    unsigned int island_id,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
        __gpr unsigned int ring_address;
        __gpr unsigned int hi_addr;

        CT_ASSERT(__is_ct_const(sync));
        CT_ASSERT(sync == sig_done || sync == ctx_swap);

        _CLS_RING_NUMBER_CHECK(ring_number);
        _CLS_RING_OFFSET_CHECK(sequence_number);
        _CLS_RING_ISLAND_CHECK(island_id);

        _CLS_RING_SET_LOCALITY(hi_addr, island_id);
        _CLS_RING_ADDRESS_ORDERED_LOCK(ring_address, ring_number, sequence_number);
        if (sync == sig_done)
        {
            __asm
            {
                cls[ring_ordered_lock, --, hi_addr, << 8, ring_address], sig_done[*sig_ptr]
            }
        }
        else
        {
            __asm
            {
                cls[ring_ordered_lock, --, hi_addr, << 8, ring_address], ctx_swap[*sig_ptr]
            }
        }
    }
_INTRINSIC_END;
}


__intrinsic
void cls_ring_ordered_unlock_ptr32(
    unsigned int ring_number,
    unsigned int sequence_number
)
{
    _INTRINSIC_BEGIN;
    {
        __gpr unsigned int ring_address;

        _CLS_RING_NUMBER_CHECK(ring_number);
        _CLS_RING_OFFSET_CHECK(sequence_number);
        _CLS_RING_ADDRESS_ORDERED_LOCK(ring_address, ring_number, sequence_number);
        __asm
        {
            cls[ring_ordered_unlock, --, ring_address, 0]
        }
    }

_INTRINSIC_END;
}

__intrinsic
void cls_ring_ordered_unlock_ptr40(
    unsigned int ring_number,
    unsigned int sequence_number,
    unsigned int island_id
)
{
    _INTRINSIC_BEGIN;
    {
        __gpr unsigned int ring_address;
        __gpr unsigned int hi_addr;

        _CLS_RING_NUMBER_CHECK(ring_number);
        _CLS_RING_OFFSET_CHECK(sequence_number);
        _CLS_RING_ADDRESS_ORDERED_LOCK(ring_address, ring_number, sequence_number);
        _CLS_RING_SET_LOCALITY(hi_addr, island_id);
        __asm
        {
            cls[ring_ordered_unlock, --, hi_addr, << 8, ring_address]
        }
    }

_INTRINSIC_END;
}

//////////////////////////////////////////////////////////////////////
//  nfp_cls_ring code end
//////////////////////////////////////////////////////////////////////

#define INITIAL_DELAY 500
#define RING_OFFSET 1
#define RING_INTERVAL 2

int
main(void)
{
    if (__ctx() == 0) {
        SIGNAL sig;
        //unsigned int mbox3, mbox2, mbox1, mbox0 ;
        unsigned int seq_num = 0x1000 ;
        //int i, j ;
        //volatile __xwrite struct imem_entry _w_entry ;
        //volatile __xread struct imem_entry _r_entry ;
        __addr40 void *entry_addr ;

        local_csr_write(local_csr_mailbox0, seq_num++);    

        cls_ring_setup(REORDER_RING_NUM,
                   (__cls void *)reorder_ring_mem,
                   (REORDER_RING_SIZE_LW * 4));
        local_csr_write(local_csr_mailbox0, seq_num++); 
        /*   
        sleep(INITIAL_DELAY + 1200 ) ;
        local_csr_write(local_csr_mailbox0, seq_num++);    
        cls_ring_ordered_lock_ptr32( REORDER_RING_NUM, 0, ctx_swap, &sig ) ;
        local_csr_write(local_csr_mailbox0, seq_num++);    
        sleep(200) ;
        local_csr_write(local_csr_mailbox0, seq_num++);    
        cls_ring_ordered_unlock_ptr32(REORDER_RING_NUM, 0) ;
        local_csr_write(local_csr_mailbox0, seq_num++);    
        */
        sleep(100000) ;     

    }
    else 
    {
      SIGNAL sig;
      unsigned int ctx = __ctx() ;
      unsigned int dly = INITIAL_DELAY + 1000 - (100 * ctx) ; // reprder the sequence
      unsigned int sequence = RING_OFFSET + ((ctx - 1) * RING_INTERVAL) ;
      sleep(dly) ;
      local_csr_write(local_csr_mailbox1, sequence + 0x3000 );
      cls_ring_ordered_lock_ptr40( REORDER_RING_NUM, sequence, REORDER_RING_ISLAND, ctx_swap, &sig ) ;
      local_csr_write(local_csr_mailbox2, sequence );
      sleep(200) ;
      cls_ring_ordered_unlock_ptr40(REORDER_RING_NUM, sequence, REORDER_RING_ISLAND ) ;
      local_csr_write(local_csr_mailbox3, sequence );
      sleep(29900) ;

    }
    
    return 0;
}	
