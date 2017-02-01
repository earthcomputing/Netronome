/*
 * Copyright (C) 2014 Netronome Systems, Inc.  All rights reserved.
 */

#ifndef __NFP_MATH_C__
#define __NFP_MATH_C__

#ifndef NFP_LIB_ANY_NFCC_VERSION
    #if (!defined(__NFP_TOOL_NFCC) ||                       \
        (__NFP_TOOL_NFCC < NFP_SW_VERSION(5, 0, 0, 0)) ||   \
        (__NFP_TOOL_NFCC > NFP_SW_VERSION(6, 0, 1, 255)))
        #error "This standard library is not supported for the version of the SDK currently in use."
    #endif
#endif

/*
 * NOTE: When writing inline-asm, it's important not to use reserved words
 *       for variables.  Common ones are 'a','b', 'csr', 'state', and
 *       'inp_state'.
 */
#include <nfp.h>
#include <stdint.h>

#define _math_log2_2(n)      ((n&2)?1:0)
#define _math_log2_4(n)      ((n&(0xC))?(2+_math_log2_2(n>>2)):(_math_log2_2(n)))
#define _math_log2_8(n)      ((n&0xF0)?(4+_math_log2_4(n>>4)):(_math_log2_4(n)))
#define _math_log2_16(n)     ((n&0xFF00)?(8+_math_log2_8(n>>8)):(_math_log2_8(n)))
#define _math_log2_32(n)     ((n&0xFFFF0000)?(16+_math_log2_16(n>>16)):(_math_log2_16(n)))
#define _math_log2(n)        ((n)==0?0:_math_log2_32(n))

unsigned int log2(uint32_t value)
{
    _INTRINSIC_BEGIN;
    {
        return _math_log2(value);
    }
    _INTRINSIC_END;
}

#endif /* __NFP_MATH_C__ */
