/*
 * Copyright (C) 2014-2016,  Netronome Systems, Inc.  All rights reserved.
 *
 * @file          rotate.c
 * @brief         MEM CAM add increment.
 */

#ifndef _ROTATE_C_
#define _ROTATE_C_

#include <nfp.h>
#include <stdint.h>
#include <assert.h>
#include <rotate_c.h>

__intrinsic
unsigned int rotl(unsigned int value, int shift)
{
    unsigned result;
    if (__is_ct_const(shift))
    {
        ctassert(shift >= 1 && shift <= 31);
        __asm alu_shf[result, --, B, value, <<rot/**/__ct_const_val(shift)]
    }
    else
    {
        result = (((value)<<(shift & 0x1f)) | ((value)>>(32 - (shift & 0x1f))));
    }
    return result;
}


#endif // _ROTATE_C_
