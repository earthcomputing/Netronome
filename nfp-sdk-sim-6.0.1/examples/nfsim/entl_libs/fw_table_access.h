/**
 * Copyright (C) 2017,  Earth Computing Inc.  All rights reserved.
 *
 *
 * @file          fw_table_access.h
 * @brief         FW Table Access 
 *
 * Author:        Atsushi Kasuya
 *
 */

#ifndef _FW_TABLE_ACCESS_H_
#define _FW_TABLE_ACCESS_H_

// FW table 65536 64-byte-entry per imem 
#define FW_TABLE_ENTRY_SIZE 64
#define FW_TABLE_SIZE 0x10000


__shared volatile uint64_t  keep_pkt_num ;
//__shared uint32_t alo_command ;
//__shared int  next_flag ;
typedef struct fw_table_entry {
  union {
    struct {
      uint16_t fw_vector ;
      unsigned int parent:4 ;
      unsigned int reserved:4 ;
      uint32_t next_lookup[15] ;
    };
    uint32_t __raw[16] ; // 64 byte
  }
} __PACKED fw_table_entry_t ;  


#ifdef FW_TABLE_ALLOCATE

__export __shared __imem_n(0) fw_table_entry_t fw_table0[FW_TABLE_SIZE] ;
__export __shared __imem_n(1) fw_table_entry_t fw_table1[FW_TABLE_SIZE] ;

#endif

#ifdef FW_TABLE_IMPORT

__import __shared __imem_n(0) fw_table_entry_t fw_table0[FW_TABLE_SIZE] ;
__import __shared __imem_n(1) fw_table_entry_t fw_table1[FW_TABLE_SIZE] ;

#endif


__intrinsic __addr40 void *imem_ptr40(unsigned int off) ;
__intrinsic void update_fw_entry ( volatile __xwrite fw_table_entry_t *entry, uint32_t offset ) ;
__intrinsic void read_fw_entry ( volatile __xread fw_table_entry_t *entry, uint32_t offset ) ;



#endif