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
    indirect.signal_ctx = 0; //Only ctx0 is waiting for a signal 
    local_csr_write(local_csr_cmd_indirect_ref_0, indirect.__raw);

    /* Currently just support reflect_write_sig_remote */
    __asm {
        alu[--, --, b, 3, <<OV_SIG_NUM]; // Set OV_SIG_NUM and OV_SIG_CTX
        ct[reflect_write_sig_remote, *src_xfer, addr, 0, \
           __ct_const_val(count)], indirect_ref;
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
        case 6:     return __nfp_meid(38,0) ;
        case 7:     return __nfp_meid(38,1) ;  
        //case 8:     return __nfp_meid(36,0) ;
        //case 9:     return __nfp_meid(36,1) ;
        //case 10:    return __nfp_meid(37,0) ;
        //case 11:    return __nfp_meid(37,1) ;  // netronome can handle up to 12 ports
        default:    return 0xffffffff ;
    }
}