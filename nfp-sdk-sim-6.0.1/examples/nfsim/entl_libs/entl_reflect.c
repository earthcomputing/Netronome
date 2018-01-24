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
entl_reflect(unsigned int dst_isl, unsigned int dst_me, unsigned int dst_xfer,
             unsigned int sig_no, volatile __xwrite void *src_xfer, size_t size )
{
    #define OV_SIG_NUM 13

    unsigned int low_addr ;
    unsigned int hi_addr ;
    unsigned int count = (size >> 2);
    //struct nfp_mecsr_cmd_indirect_ref_0 indirect;

    /* ctassert(__is_write_reg(src_xfer)); */ /* TEMP, avoid volatile warnings */
    //ctassert(__is_ct_const(size));

    /* Address encoding formula for cls reflect
            bits[11:0]  = xfer register address (reg number)
            bits[15:12] = FPC/ME
            bits[23:16] = 8b0
            bits[27:24] = Signal number
            bits[30:28] = ctx
            bit[31]     = Signal reference (If set, signal number comes from bits [27:24]
    */
    low_addr = (1 << 31) | (sig_no << 24) | (((dst_me+4) & 0xf) << 12) | (dst_xfer & 0xfff);
    hi_addr = (dst_isl & 0x3f) << 26;

    /* Currently just support reflect_write_sig_remote */
    __asm {
        cls[reflect_write_sig_remote, *src_xfer, hi_addr, << 8, low_addr, __ct_const_val(count)]
    };
}

uint32_t get_entl_sender_me( uint32_t port ) {
    switch(port) {
        case 0:     return 0 ;
        case 1:     return 0 ;
        case 2:     return 0 ;
        case 3:     return 0 ;
        case 4:     return 0 ;
        case 5:     return 0 ;
        case 6:     return 0 ;
        case 7:     return 1 ;  
        //case 8:     return 0 ;
        //case 9:     return 1 ;
        //case 10:    return 0 ;
        //case 11:    return 1 ;  // netronome can handle up to 12 ports
        default:    return 0xffffffff ;
    }
}
uint32_t get_entl_sender_meid( uint32_t port ) {
    switch(port) {
        case 0:     return __nfp_meid(32,0) ;
        case 1:     return __nfp_meid(33,0) ;
        case 2:     return __nfp_meid(34,0) ;
        case 3:     return __nfp_meid(35,0) ;
        case 4:     return __nfp_meid(36,0) ;
        case 5:     return __nfp_meid(37,0) ;
        //case 6:     return __nfp_meid(38,0) ;
        //case 7:     return __nfp_meid(38,1) ;  
        //case 8:     return __nfp_meid(36,0) ;
        //case 9:     return __nfp_meid(36,1) ;
        //case 10:    return __nfp_meid(37,0) ;
        //case 11:    return __nfp_meid(37,1) ;  // netronome can handle up to 12 ports
        default:    return 0xffffffff ;
    }
}