/**
 * Copyright (C) 2012 Netronome Systems, Inc.  All rights reserved.
 *
 * NFP6XXX ilkn BFM bus header:
 * Defines structure for ilkn nfp_sal_ilkn_* API
 *
 */

#ifndef __ILKN_BUS_H__
#define __ILKN_BUS_H__

#ifdef _WIN32
    #define __PACKED__
    #pragma pack(push, 1)
#else
    #define __PACKED__ __attribute__ ((__packed__))
#endif

#include <stdint.h>

#define ILKN_WORD_SZ 32 //interlaken BFM data width is 32B (256b)

struct ilkn_buffer {
  uint64_t      data_3;
  uint64_t      data_2;
  uint64_t      data_1;
  uint64_t      data_0;
  uint8_t       valid;
  uint16_t      app0;
  uint8_t       chan;
  uint8_t       chanext;
  uint8_t       multiple_use;
  uint8_t       sz;
  uint8_t       sob;
  uint8_t       eob;
  uint8_t       sop;
  uint8_t       eop;
  uint8_t       eop_err;
  uint8_t       app1;
} __PACKED__;


union ilkn_buffer_u {
  struct ilkn_buffer pstruct;
  uint8_t             parray[ILKN_WORD_SZ];
} __PACKED__;


struct ilkn_ctrl {
    int sop;
    int eop;
    int sob;
    int eob;
    int sz;
};


#ifdef _WIN32
    #pragma pack(pop)
#endif


#endif

