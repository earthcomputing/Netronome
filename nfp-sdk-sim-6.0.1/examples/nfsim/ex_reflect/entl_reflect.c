/**
 * Copyright (C) 2017,  Earth Computing Inc.  All rights reserved.
 *
 *
 * @file          entl_reflect.c
 * @brief         ENTL refrect code 
 *
 * Author:        Atsushi Kasuya
 *
 */
#include "entl_reflect.h"

__intrinsic void
entl_reflect_data(unsigned int dst_me, unsigned int dst_xfer,
             unsigned int sig_no, volatile __xwrite void *src_xfer, size_t size )
{
    #define OV_SIG_NUM 13

    unsigned int addr;
    unsigned int count = (size >> 2);
    struct nfp_mecsr_cmd_indirect_ref_0 indirect;

    /* ctassert(__is_write_reg(src_xfer)); */ /* TEMP, avoid volatile warnings */
    ctassert(__is_ct_const(size));

    /* Generic address computation.
     * Could be expensive if dst_me, or dst_xfer
     * not compile time constants */
    addr = ((dst_me & 0xFF0)<<20 | ((dst_me & 0xF)<<10 | (dst_xfer & 0x3F)<<2));

    indirect.__raw = 0;
    indirect.signal_num = sig_no;
    local_csr_write(local_csr_cmd_indirect_ref_0, indirect.__raw);

    /* Currently just support reflect_write_sig_remote */
    __asm {
        alu[--, --, b, 1, <<OV_SIG_NUM];
        ct[reflect_write_sig_remote, *src_xfer, addr, 0, \
           __ct_const_val(count)], indirect_ref;
    };
}

#define CT_ASSERT(expr) __ct_assert(expr, #expr)

__intrinsic void entl_reflect_data_ptr40(
    __xwrite void *data,                         /* data to reflect */
    unsigned int remote_island,                  /* remote island number */
    unsigned int remote_ME,                      /* remote ME number */
    unsigned int remote_xfer_reg_number,                           /* remote xfer register number */
    int sig,                                     /* triggered signal number*/
    unsigned int count                           /* number of longwords to reflect */
)
{
        
    /* Address encoding formula for cls reflect
    bits[11:0]  = xfer register address (reg number)
    bits[15:12] = FPC/ME
    bits[23:16] = 8b0
    bits[27:24] = Signal number
    bits[30:28] = ctx
    bit[31]     = Signal reference (If set, signal number comes from bits [27:24]
    */
    unsigned int low_addr = (1 << 31) | (sig << 24) | ((remote_ME & 0xf) << 12) | (remote_xfer_reg_number & 0xfff);
    unsigned int hi_addr = (remote_island & 0x3f) << 26;

    CT_ASSERT(count<=8);
    __asm cls[reflect_write_sig_remote, *data, hi_addr, << 8, low_addr, __ct_const_val(count)]

}
