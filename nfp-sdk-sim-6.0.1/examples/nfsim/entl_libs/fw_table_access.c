/**
 * Copyright (C) 2017,  Earth Computing Inc.  All rights reserved.
 *
 *
 * @file          fw_table_access.c
 * @brief         FW Table Access functions
 *
 * Author:        Atsushi Kasuya
 *
 */
#include <nfp.h>
#include <me.h>
#include <mutexlv.h>
#include <stdint.h>
#include <pkt/pkt.h>
#include <net/eth.h>
#include <nfp/mem_bulk.h>

#define FW_TABLE_IMPORT
#include "fw_table_access.h"


__intrinsic __addr40 void *imem_ptr40(unsigned int off)
{
    __gpr unsigned int lo;
    __gpr unsigned int hi;
    unsigned char isl ;

    lo = off << 6 ;  // 64 byte per entry

    if( off < FW_TABLE_SIZE_0 ) {  // iMem has 4M sram (22bit)
      isl = 0x1d ;     // Internal Mem 1
    }
    else {
      isl = 0x1c ;     // Internal Mem 0

      
    }

    hi = 0x80 | isl;

    return (__addr40 void *)
        (((unsigned long long)hi << 32) | (unsigned long long)lo);
}

__intrinsic void update_entry ( volatile __xwrite fw_table_entry_t *entry, uint32_t offset ) 
{
  __addr40 void *entry_addr = imem_ptr40(offset) ;
  mem_write32( (void*)&entry, (__addr40 uint8_t *)entry_addr , sizeof(struct imem_entry) );
  //mbox0 = (unsigned int)entry_addr ;
  //mbox1 = (unsigned int)((unsigned long long)entry_addr >> 32) ;
}

__intrinsic void read_entry ( volatile __xread fw_table_entry_t *entry, uint32_t offset ) 
{
  __addr40 void *entry_addr = imem_ptr40(offset) ;
  mem_read32( (void*)entry, (__addr40 uint8_t *)entry_addr , sizeof(struct imem_entry) );
}


