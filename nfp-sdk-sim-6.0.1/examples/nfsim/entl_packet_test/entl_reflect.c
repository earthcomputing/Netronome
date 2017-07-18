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
entl_reflect(unsigned int dst_me, unsigned int dst_xfer,
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