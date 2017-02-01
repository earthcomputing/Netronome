/* Copyright (C) 2016,  Netronome Systems, Inc.  All rights reserved. */

#include "pif_memops.h"
#include <nfp/cls.h>

#define _PIF_MEMOPS_XFR_CNT  8

/* Byte-wise copy from mem to lmem. */
void
mem2lmem_bcopy(__mem uint8_t *pktdata,
               __lmem uint8_t *lm_ptr,
               int lm_off_b,
               int lm_len_b)
{
    SIGNAL sig;
    __xread uint8_t xfr_data[_PIF_MEMOPS_XFR_CNT * 4];
    unsigned int i, xfr_b, pkt_off;
    int rem_b = lm_len_b;

    pkt_off = 0;
    while (rem_b > 0) {
        if (rem_b > _PIF_MEMOPS_XFR_CNT*4)
            xfr_b = _PIF_MEMOPS_XFR_CNT*4;
        else
            xfr_b = rem_b;
        __mem_read8(xfr_data, pktdata + pkt_off, xfr_b, _PIF_MEMOPS_XFR_CNT*4, ctx_swap, &sig);
        for (i = 0; i < xfr_b; i++)
            lm_ptr[lm_off_b + i] = xfr_data[i];
        rem_b -= xfr_b;
        pkt_off += xfr_b;
        lm_off_b += xfr_b;
    }
}

/* Byte-wise copy from mem to CLS. */
void
mem2cls_bcopy(__mem uint8_t *pktdata,
              __cls uint8_t *cls_ptr,
              int cls_len_b)
{
    SIGNAL sig;
    __xread uint32_t xfr_rd_data[_PIF_MEMOPS_XFR_CNT];
    __xwrite uint32_t xfr_wr_data[_PIF_MEMOPS_XFR_CNT];
    unsigned int i, xfr_b, off_b, pkt_off;
    int rem_b = cls_len_b;

    off_b = 0;
    pkt_off = 0;
    while (rem_b > 0) {
        if (rem_b > _PIF_MEMOPS_XFR_CNT*4)
            xfr_b = _PIF_MEMOPS_XFR_CNT*4;
        else
            xfr_b = rem_b;
        __mem_read8(xfr_rd_data, pktdata + pkt_off, xfr_b, _PIF_MEMOPS_XFR_CNT*4, ctx_swap, &sig);

        for (i = 0; i < xfr_b; i+=4)
            xfr_wr_data[i/4] = xfr_rd_data[i/4];

        /* cls_write expects bytes written to be a multiple of 4. */
        {
            __gpr uint32_t xfr_4b = xfr_b + (xfr_b % 4);
            __cls_write(xfr_wr_data, cls_ptr+off_b, xfr_4b, _PIF_MEMOPS_XFR_CNT*4,
                ctx_swap, &sig);
        }

        rem_b -= xfr_b;
        pkt_off += xfr_b;
        off_b += xfr_b;
    }
}

