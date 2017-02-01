/* Copyright (C) 2015,  Netronome Systems, Inc.  All rights reserved. */

#ifndef __PIF_MEMOPS_H__
#define __PIF_MEMOPS_H__

#include <nfp.h>
#include <stdint.h>
#include <nfp/mem_bulk.h>

/* Copy byte aligned mem data to 4B aligned lm */
__forceinline void
mem2lmem_copy(__mem uint8_t *pktdata,
              __lmem uint32_t *lm_ptr,
              int off_lw,
              int len_lw)
{
    SIGNAL sig;
    __xread uint32_t read_xfer[8];
    unsigned int i, tocopy, chunk, off, copied;

    tocopy = len_lw * 4;
    off = off_lw;
    copied = 0;
    while (tocopy > 0) {
        if (tocopy > 8*4)
            chunk = 8*4;
        else
            chunk = tocopy;
        __mem_read8(read_xfer, pktdata + copied, chunk, 8*4, ctx_swap, &sig);
        for (i = 0; i < chunk/4; i++)
            lm_ptr[i + off] = read_xfer[i];
        tocopy -= chunk;
        copied += chunk;
        off += chunk/4;
    }
}

/* Byte-wise copy from mem to lmem. */
void
mem2lmem_bcopy(__mem uint8_t *pktdata,
               __lmem uint8_t *lm_ptr,
               int lm_off_b,
               int lm_en_b);

/* Byte-wise copy from mem to CLS. */
void
mem2cls_bcopy(__mem uint8_t *pktdata,
              __cls uint8_t *cls_ptr,
              int cls_len_b);


#endif /* __PIF_MEMOPS_H__ */
