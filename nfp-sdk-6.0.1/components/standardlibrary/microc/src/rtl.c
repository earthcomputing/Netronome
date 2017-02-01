/*
 * Copyright (C) 2008-2015 Netronome Systems, Inc.  All rights reserved.
 */

#ifndef __RTL_C__
#define __RTL_C__

#ifndef NFP_LIB_ANY_NFCC_VERSION
    #if (!defined(__NFP_TOOL_NFCC) ||                       \
        (__NFP_TOOL_NFCC < NFP_SW_VERSION(5, 0, 0, 0)) ||   \
        (__NFP_TOOL_NFCC > NFP_SW_VERSION(6, 0, 1, 255)))
        #error "This standard library is not supported for the version of the SDK currently in use."
    #endif
#endif

#include <nfp.h>
#include <stdint.h>
#include <mutexlv.h>
#include <nfp_override.h>
#include <nfp_internal.h>
#include <rtl.h>

/*
 * NOTE: When writing inline-asm, it's important not to use reserved words
 * for variables.  Common ones are 'a','b', 'csr', 'state', and
 * 'inp_state'.
 */


// Local function prototypes
__intrinsic void sram_write_ind(__xwrite void *data, volatile void __sram *address, unsigned int max_nn, generic_ind_t ind, sync_t sync, SIGNAL *sig_ptr);


/* 64-bit by 64-bit multiply with 64-bit product */
/* works equally well for signed or unsigned types */

extern long long _mul_64(lohi x, lohi y)
{
    return (long long)x.lo * (long long)y.lo +
            (((long long)x.lo * (long long)y.hi) << 32) +
            (((long long)x.hi * (long long)y.lo) << 32);
}

/*
 * 32-bit unsigned divide  returning either quotient or remainder
 * if mod == 1 then x mod y
 * else x div y
 */

extern unsigned int _div_mod_32(unsigned int x, unsigned int y, unsigned int mod)
{
    int lo = x;
    unsigned int hi = 0;
    unsigned int i;

    if (y == 0)
    {
        //puts("error: div or mod by zero\n");
        //exit(1);
        // we define x % 0 or x / 0 is 0xffffffff
        return 0xffffffff;
    }

    /* hi and lo form a 64-bit dividend.  We shift one bit at a time */
    /* from the lo to the hi word.  Then we check the hi word against */
    /* the divisor.  If it is greater, we subtract the divisor and put */
    /* a 1 in the low bit of lo.  After 32 iterations, lo is the quotient */
    /* and hi is the remainder */

    for (i = 0; i < 32; i++)
    {
        hi <<= 1;
        if (lo < 0)
        {
            hi |= 1;
        }
        lo <<= 1;

        if (hi >= y)
        {
            hi -= y;
            lo |= 1;
        }
    }

    if (mod)
    {
        return hi;
    }
    else
    {
        return lo;
    }
}

/* 32-bit signed remainder */

extern int _mod_s32(int x, int y)
{
    int neg = 0;
    int result;

    if (y == 0)
    {
        // we define x % 0 is 0xffffffff
        return 0xffffffff;
    }

    if (x < 0)
    {
        neg = 1;
        x = -x;
    }
    if (y < 0)
    {
        y = -y;
    }

    result = _div_mod_32(x, y, 1);
    if (neg)
    {
        result = -result;
    }
    return result;
}

/* 32-bit signed quotient */

extern int _div_s32(int x, int y)
{
    int neg;
    int result;

    if (y == 0)
    {
        // we define x / 0 is 0xffffffff
        return 0xffffffff;
    }

    neg = 0;
    if (x < 0)
    {
        neg = 1;
        x = -x;
    }
    if (y < 0)
    {
        neg ^= 1;
        y = -y;
    }

    result = _div_mod_32(x, y, 0);
    if (neg)
    {
        result = -result;
    }
    return result;
}

/* 32-bit unsigned remainder */

extern int _mod_u32(int x, int y)
{
    return _div_mod_32(x, y, 1);
}


/* 32-bit unsigned quotient */

extern int _div_u32(int x, int y)
{
    return _div_mod_32(x, y, 0);
}


/* 64-bit unsigned divide returning either quotient or remainder */

extern unsigned long long _div_mod_64(unsigned long long x, unsigned long long y, unsigned int mod)
{
    long long lo = x;
    unsigned long long hi = 0;
    unsigned int i;

    if (y == 0)
    {
        //puts("error: div or mod by zero\n");
        // we define x % 0 or x / 0 is 0xffffffff
        return 0xffffffffffffffff;
    }

    /* hi and lo form a 128-bit dividend.  We shift one bit at a time */
    /* from the lo to the hi word.  Then we check the hi word against */
    /* the divisor.  If it is greater, we subtract the divisor and put */
    /* a 1 in the low bit of lo.  After 64 iterations, lo is the quotient */
    /* and hi is the remainder */

    for (i = 0; i < 64; i++)
    {
        hi <<= 1;
        if (lo < 0)
        {
            hi |= 1;
        }
        lo <<= 1;

        if (hi >= y)
        {
            hi -= y;
            lo |= 1;
        }
    }

    if (mod)
    {
        return hi;
    }
    else
    {
        return lo;
    }
}

/* 64-bit signed remainder */

extern long long _mod_s64(long long x, long long y)
{
    int neg = 0;
    long long result;

    if (y == 0)
    {
        // we define x % 0 is 0xffffffff
        return 0xffffffffffffffff;
    }

    if (x < 0)
    {
        neg = 1;
        x = -x;
    }
    if (y < 0)
    {
        y = -y;
    }

    result = _div_mod_64(x, y, 1);
    if (neg)
    {
        result = -result;
    }
    return result;
}

/* 64-bit signed quotient */

extern long long _div_s64(long long x, long long y)
{
    int neg;
    long long result;

    if (y == 0)
    {
        // we define x / 0 is 0xffffffff
        return 0xffffffffffffffff;
    }

    neg = 0;
    if (x < 0)
    {
        neg = 1;
        x = -x;
    }
    if (y < 0)
    {
        neg ^= 1;
        y = -y;
    }

    result = _div_mod_64(x, y, 0);
    if (neg)
    {
        result = -result;
    }
    return result;
}

/* 64-bit unsigned remainder */

extern long long _mod_u64(long long x, long long y)
{
    return _div_mod_64(x, y, 1);
}


/* 64-bit unsigned quotient */

extern long long _div_u64(long long x, long long y)
{
    return _div_mod_64(x, y, 0);
}


#if __BIGENDIAN

/* 64-bit shift left */

extern long long _shl_64(long long x, unsigned int y)
{
    long long result;
    int thirtytwo = 32;
    int y1;

    /* truncate shift count to 6 bits */

    y &= 63;

    if (y >= thirtytwo) __asm
    {
//        alu         [y, thirtytwo, -, y]
        alu         [result+4, y, AND, 0]
        alu_shf     [result, --, B, x+4, <<indirect]
    }
    else if (y != 0) __asm
    {
        alu         [y1, thirtytwo, -, y]
        alu         [--, y1, OR, 0]
        dbl_shf     [result, x, x+4, >>indirect]
        alu         [--, y, OR, 0]
        alu_shf     [result+4, --, B, x+4, <<indirect]
    }
    else
    {
        result = x;
    }
    return result;
}

extern long long _shr_64(long long x, unsigned int y)
{
    long long result;

    /* truncate shift count to 6 bits */

    y &= 63;

    if (y > 31) __asm
    {
        alu         [result, y, AND, 0]
        alu_shf     [result+4, --, B, x, >>indirect]
    }
    else __asm
    {
        alu         [--, y, OR, 0]
        dbl_shf     [result+4, x, x+4, >>indirect]
        alu         [--, y, OR, 0]
        alu_shf     [result, --, B, x, >>indirect]
    }
    return result;
}

extern long long _ashr_64(long long x, unsigned int y)
{
    typedef union
    {
        struct
        {
            int hi;
            int lo;
        } i;
        long long ll;
    } two_ints;
    two_ints result;

    /* get sign extend of x */

    int ext = ((two_ints *)(void *)&x)->i.hi >> 31;

    /* truncate shift count to 6 bits */

    y &= 63;

    if (y > 31)
    {
        result.i.hi = ext;
        result.i.lo = dbl_shr(ext, ((two_ints *)(void *)&x)->i.hi, y - 32);
    }
    else
    {
        result.i.hi = dbl_shr(ext, ((two_ints *)(void *)&x)->i.hi, y);
        result.i.lo = dbl_shr(((two_ints *)(void *)&x)->i.hi, ((two_ints *)(void *)&x)->i.lo, y);
    }

    return result.ll;
}

#else

/* 64-bit shift left */

extern long long _shl_64(long long x, unsigned int y)
{
    long long result;
    int thirtytwo = 32;
    int y1;

    /* truncate shift count to 6 bits */

    y &= 63;

    if (y >= 32)
        __asm
    {
//        alu         [y, thirtytwo, -, y]
        alu         [result, y, AND, 0]
        alu_shf     [result+4, --, B, x, <<indirect]
    }
    else if (y != 0) __asm
    {
        alu         [y1, thirtytwo, -, y]
        alu         [--, y1, OR, 0]
        dbl_shf     [result+4, x+4, x, >>indirect]
        alu         [--, y, OR, 0]
        alu_shf     [result, --, B, x, <<indirect]
    }
    else
    {
        result = x;
    }
    return result;
}

extern long long _shr_64(long long x, unsigned int y)
{
    long long result;

    /* truncate shift count to 6 bits */

    y &= 63;

    if (y > 31) __asm
    {
        alu         [result+4, y, AND, 0]
        alu_shf     [result, --, B, x+4, >>indirect]
    }
    else __asm
    {
        alu         [--, y, OR, 0]
        dbl_shf     [result, x+4, x, >>indirect]
        alu         [--, y, OR, 0]
        alu_shf     [result+4, --, B, x+4, >>indirect]
    }
    return result;
}

extern long long _ashr_64(long long x, unsigned int y)
{
    typedef union
    {
        struct
        {
            int lo;
            int hi;
        } i;
        long long ll;
    } two_ints;
    two_ints result;

    /* get sign extend of x */

    int ext = ((two_ints *)(void *)&x)->i.hi >> 31;

    /* truncate shift count to 6 bits */

    y &= 63;

    if (y > 31)
    {
        result.i.hi = ext;
        result.i.lo = dbl_shr(ext, ((two_ints *)(void *)&x)->i.hi, y - 32);
    }
    else
    {
        result.i.hi = dbl_shr(ext, ((two_ints *)(void *)&x)->i.hi, y);
        result.i.lo = dbl_shr(((two_ints *)(void *)&x)->i.hi, ((two_ints *)(void *)&x)->i.lo, y);
    }

    return result.ll;
}

#endif


/*
 * ua_set_
 */


/* unaligned local_mem pointer q:

        *(unsigned char *)((int)q+offset) = (unsigned char)val;
 */

void ua_set_8_lmem(__lmem void *q, unsigned int offset, unsigned int val)
{
    unsigned int v;
    unsigned int shift;

    __lmem uint32_t *p = (__lmem uint32_t *)((int)q + offset);
    v = *p;

#if __BIGENDIAN
    shift = (3 - (int)p) << 3;
#else
    shift = (int)p << 3;
#endif

    v = v & ~(0xff << shift) | ((val & 0xff) << shift);
    *p = v;
}


/* unaligned local_mem pointer q:

        *(unsigned short *)((int)q+offset) = (unsigned short)val;
 */

void ua_set_16_lmem(__lmem void *q, unsigned int offset, unsigned int val)
{
    __lmem unsigned int *ap;
    unsigned int shift;
    unsigned int mask1, mask2, v1, v2;
    unsigned int p = (unsigned int)q + offset;
    unsigned long long l;

    ap = (void *)p;

    shift = (((unsigned int)p << 3) & (3 << 3));

#ifdef __BIGENDIAN
    v2 = val << 16;
    v1 = dbl_shr(0, v2, shift);
    v2 = dbl_shr(v2, 0, shift);
    mask1 = dbl_shr(0, 0xffff0000 , shift);
    mask2 = dbl_shr(0xffff0000, 0, shift);
#else
    v1 = (long long) val << shift;
    v2 = ((long long) val << shift ) >>32;
    l = 0xffffULL << shift;
    mask1 = l;
    mask2 = l >> 32;
#endif


    *ap++ = (*ap & ~ mask1) | (v1 & mask1);
    *ap = (*ap & ~mask2) | (v2 & mask2);
}

/* unaligned local_mem pointer q:

        *(unsigned int *)((int)q+offset) = (unsigned int)val;
 */

void ua_set_32_lmem(__lmem void *q, unsigned int offset, unsigned int val)
{
    __lmem unsigned int *ap;
    unsigned int shift;
    unsigned int mask1, mask2, v1, v2;
    unsigned int p = (unsigned int) q + offset;
    unsigned long long l;

    ap = (void *)p;

    shift = (((unsigned int)p << 3) & (3 << 3));

    v2 = val;

#ifdef __BIGENDIAN
    v1 = dbl_shr(0, v2, shift);
    v2 = dbl_shr(v2, 0, shift);
    mask1 = dbl_shr(0, 0xffffffff , shift);
    mask2 = dbl_shr(0xffffffff, 0, shift);
#else
    v1 = (long long) v2 << shift;
    v2 = ((long long) v2 << shift ) >>32;
    l = 0xffffffffULL << shift;
    mask1 = l;
    mask2 = l >> 32;
#endif

    *ap++ = (*ap & ~ mask1) | (v1 & mask1);
    *ap = (*ap & ~mask2) | (v2 & mask2);
}


/* unaligned cls pointer q:

        *(unsigned char *)((int)q+offset) = (unsigned char)val;
 */
void ua_set_8_cls(__cls void *q, unsigned int offset, unsigned int val)
{
    unsigned int v;
    unsigned int shift;

    __cls uint32_t *p = (__cls uint32_t *)((int)q + offset);
    v = *p;

#if __BIGENDIAN
    shift = (3 - (int)p) << 3;
#else
    shift = (int)p << 3;
#endif

    v = v & ~(0xff << shift) | ((val & 0xff) << shift);
    *p = v;
}

/* unaligned cls pointer q:

        *(unsigned short *)((int)q+offset) = (unsigned short)val;
 */

void ua_set_16_cls(__cls void *q, unsigned int offset, unsigned int val)
{
    unsigned long long v;
    unsigned int shift;

    __cls uint32_t *p = (__cls uint32_t *)((int)q + offset);

    v = *(__cls uint64_t *)p;

#if __BIGENDIAN
    shift = (6 - ((int)p & 3)) << 3;
#else
    shift = ((int)p & 3) << 3;
#endif

    v = v & ~(0xffffULL << shift) | (((unsigned long long)(val & 0xffff)) << shift);

    *(__cls uint64_t *)p = v;
}

/* unaligned cls pointer q:

        *(unsigned int *)((int)q+offset) = (unsigned int)val;
 */

void ua_set_32_cls(__cls void *q, unsigned int offset, unsigned int val)
{
    unsigned long long v;
    unsigned int shift;

    __cls uint32_t *p = (__cls uint32_t *)((int)q + offset);

    v = *(__cls uint64_t *)p;

#if __BIGENDIAN
    shift = (4 - ((int)p & 3)) << 3;
#else
    shift = ((int)p & 3) << 3;
#endif

    v = v & ~(0xffffffffULL << shift) | ((unsigned long long)val << shift);

    *(__cls uint64_t *)p = v;
}


typedef union
{
    struct
    {
        unsigned int a;
        unsigned int b;
        unsigned int c;
    };
    long long ll;
} threelong;


/* unaligned cls pointer q:

        *(unsigned long long *)((int)q+offset) = val;
 */
void ua_set_64_cls(__cls void *q, unsigned int offset, unsigned long long val)
{
    threelong v;
    unsigned int shift, a, b, c;

    __cls uint32_t *p = (__cls uint32_t *)((int)q + offset);

    v = *(__cls threelong *)p;

    shift = ((int)p & 3) << 3;

#if __BIGENDIAN
    if (shift)
    {
        a = (unsigned int)(val >> 32);
        b = (unsigned int)val;

        c = b << (32 - shift);
        b = dbl_shr(a, b, shift);
        a = a >> shift;

        v.c = v.c & (0xffffffff >> shift) | c;
        v.b = b;
        v.a = v.a & (0xffffffff << (32 - shift)) | a;
    }
    else
    {
        v.a = (unsigned int)(val >> 32);
        v.b = (unsigned int)val;
    }

#else

    if (shift)
    {
        a = val;
        b = val >> 32;

        c = b >> (32 - shift);
        b = dbl_shl(b, a, shift);
        a = a << shift;

        v.c = v.c & (0xffffffff << shift) | c;
        v.b = b;
        v.a = v.a & (0xffffffff >> (32 - shift)) | a;
    }
    else
    {
        v.a = val;
        v.b = val >> 32;
    }
#endif

    *(__cls threelong *)p = v;
}

/* unaligned sram pointer q:

        *(unsigned long long *)((int)q+offset) = val;
 */

void ua_set_64_sram(__sram void *q, unsigned int offset, unsigned long long val)
{
    threelong v;
    unsigned int shift, a, b, c;

    __sram uint32_t *p = (__sram uint32_t *)((int)q + offset);

    v = *(__sram threelong *)p;

    shift = ((int)p & 3) << 3;

#if __BIGENDIAN
    if (shift)
    {
        a = (unsigned int)(val >> 32);
        b = (unsigned int)val;

        c = b << (32 - shift);
        b = dbl_shr(a, b, shift);
        a = a >> shift;

        v.c = v.c & (0xffffffff >> shift) | c;
        v.b = b;
        v.a = v.a & (0xffffffff << (32 - shift)) | a;
    }
    else
    {
        v.a = (unsigned int)(val >> 32);
        v.b = (unsigned int)val;
    }

#else

    if (shift)
    {
        a = val;
        b = val >> 32;

        c = b >> (32 - shift);
        b = dbl_shl(b, a, shift);
        a = a << shift;

        v.c = v.c & (0xffffffff << shift) | c;
        v.b = b;
        v.a = v.a & (0xffffffff >> (32 - shift)) | a;
    }
    else
    {
        v.a = val;
        v.b = val >> 32;
    }
#endif

    *(__sram threelong *)p = v;
}

/* unaligned local_mem pointer q:

        *(unsigned long long *)((int)q+offset) = val;
 */

void ua_set_64_lmem(__lmem void *q, unsigned int offset, unsigned long long val)
{
    threelong v;
    unsigned int shift, a, b, c;

    __lmem uint32_t *p = (__lmem uint32_t *)((int)q + offset);

    v = *(__lmem threelong *)p;

    shift = ((int)p & 3) << 3;

#if __BIGENDIAN
    if (shift)
    {
        a = (unsigned int)(val >> 32);
        b = (unsigned int)val;

        c = b << (32 - shift);
        b = dbl_shr(a, b, shift);
        a = a >> shift;

        v.c = v.c & (0xffffffff >> shift) | c;
        v.b = b;
        v.a = v.a & (0xffffffff << (32 - shift)) | a;
    }
    else
    {
        v.a = (unsigned int)(val >> 32);
        v.b = (unsigned int)val;
    }

#else

    if (shift)
    {
        a = val;
        b = val >> 32;

        c = b >> (32 - shift);
        b = dbl_shl(b, a, shift);
        a = a << shift;

        v.c = v.c & (0xffffffff << shift) | c;
        v.b = b;
        v.a = v.a & (0xffffffff >> (32 - shift)) | a;
    }
    else
    {
        v.a = val;
        v.b = val >> 32;
    }
#endif

    *(__lmem threelong *)p = v;
}

/* unaligned local_mem pointer q:

        return *(unsigned char *)((int)q+offset);
 */

unsigned int ua_get_u8_lmem(__lmem void *q, unsigned int offset)
{
    unsigned int v;
    unsigned int shift;

    __lmem uint32_t *p = (__lmem uint32_t *)((int)q + offset);
    v = *p;

#if __BIGENDIAN
    shift = (3 - (int)p) << 3;
#else
    shift = (int)p << 3;
#endif

    return (v >> shift) & 0xff;
}

/* unaligned sram pointer q:

        return *(unsigned char *)((int)q+offset);
 */

unsigned int ua_get_u8_sram(__sram void *q, unsigned int offset)
{
    unsigned int v;
    unsigned int shift;

    __sram uint32_t *p = (__sram uint32_t *)((int)q + offset);
    v = *p;

#if __BIGENDIAN
    shift = (3 - (int)p) << 3;
#else
    shift = (int)p << 3;
#endif

    return (v >> shift) & 0xff;
}


/* unaligned cls pointer q:

        return *(unsigned char *)((int)q+offset);
 */
unsigned int ua_get_u8_cls(__cls void *q, unsigned int offset)
{
    unsigned int v;
    unsigned int shift;

    __cls uint32_t *p = (__cls uint32_t *)((int)q + offset);
    v = *p;

#if __BIGENDIAN
    shift = (3 - (int)p) << 3;
#else
    shift = (int)p << 3;
#endif

    return (v >> shift) & 0xff;
}



/* unaligned local_mem pointer q:

        return *(unsigned short *)((int)q+offset);
 */

unsigned int ua_get_u16_lmem(__lmem void *q, unsigned int offset)
{
    unsigned long long v;
    unsigned int shift;

    __lmem uint64_t *p = (__lmem uint64_t *)((int)q + offset);
    v = *p;

#if __BIGENDIAN
    shift = (6 - ((int)p & 3)) << 3;
#else
    shift = ((int)p & 3) << 3;
#endif

    return (unsigned)(v >> shift) & 0xffff;
}


/* unaligned sram pointer q:

        return *(unsigned short *)((int)q+offset);
 */

unsigned int ua_get_u16_sram(__sram void *q, unsigned int offset)
{
    unsigned long long v;
    unsigned int shift;

    __sram uint64_t *p = (__sram uint64_t *)((int)q + offset);
    v = *p;

#if __BIGENDIAN
    shift = (6 - ((int)p & 3)) << 3;
#else
    shift = ((int)p & 3) << 3;
#endif

    return (unsigned)(v >> shift) & 0xffff;
}


/* unaligned cls pointer q:

        return *(unsigned short *)((int)q+offset);
 */
unsigned int ua_get_u16_cls(__cls void *q, unsigned int offset)
{
    unsigned long long v;
    unsigned int shift;

    __cls uint64_t *p = (__cls uint64_t *)((int)q + offset);
    v = *p;

#if __BIGENDIAN
    shift = (6 - ((int)p & 3)) << 3;
#else
    shift = ((int)p & 3) << 3;
#endif

    return (unsigned)(v >> shift) & 0xffff;
}


/* unaligned local_mem pointer q:

        return *(unsigned int *)((int)q+offset);
 */

unsigned int ua_get_u32_lmem(__lmem void *q, unsigned int offset)
{
    unsigned long long v;
    unsigned int shift;

    __lmem uint64_t *p = (__lmem uint64_t *)((int)q + offset);
    v = *p;

#if __BIGENDIAN
    shift = (4 - ((int)p & 3)) << 3;
#else
    shift = ((int)p & 3) << 3;
#endif

    return (unsigned)(v >> shift);
}


/* unaligned sram pointer q:

        return *(unsigned int *)((int)q+offset);
 */
unsigned int ua_get_u32_sram(__sram void *q, unsigned int offset)
{
    unsigned long long v;
    unsigned int shift;

    __sram uint64_t *p = (__sram uint64_t *)((int)q + offset);
    v = *p;

#if __BIGENDIAN
    shift = (4 - ((int)p & 3)) << 3;
#else
    shift = ((int)p & 3) << 3;
#endif

    return (unsigned)(v >> shift);
}


/* unaligned cls pointer q:

        return *(unsigned int *)((int)q+offset);
 */
unsigned int ua_get_u32_cls(__cls void *q, unsigned int offset)
{
    unsigned long long v;
    unsigned int shift;

    __cls uint64_t *p = (__cls uint64_t *)((int)q + offset);
    v = *p;

#if __BIGENDIAN
    shift = (4 - ((int)p & 3)) << 3;
#else
    shift = ((int)p & 3) << 3;
#endif

    return (unsigned)(v >> shift);
}

/* unaligned local_mem pointer q:

        return *(signed char *)((int)q+offset);
 */
int ua_get_s8_lmem(__lmem void *q, unsigned int offset)
{
    unsigned int v;
    unsigned int shift;

    __lmem uint32_t *p = (__lmem uint32_t *)((int)q + offset);
    v = *p;

#if __BIGENDIAN
    shift = (3 - (int)p) << 3;
#else
    shift = (int)p << 3;
#endif

    return (int)(signed char)((v >> shift) & 0xff);
}


/* unaligned sram pointer q:

        return *(signed char *)((int)q+offset);
 */
int ua_get_s8_sram(__sram void *q, unsigned int offset)
{
    unsigned int v;
    unsigned int shift;

    __sram uint32_t *p = (__sram uint32_t *)((int)q + offset);
    v = *p;

#if __BIGENDIAN
    shift = (3 - (int)p) << 3;
#else
    shift = (int)p << 3;
#endif

    return (int)(signed char)((v >> shift) & 0xff);
}

/* unaligned cls pointer q:

        return *(signed char *)((int)q+offset);
 */
int ua_get_s8_cls(__cls void *q, unsigned int offset)
{
    unsigned int v;
    unsigned int shift;

    __cls uint32_t *p = (__cls uint32_t *)((int)q + offset);
    v = *p;

#if __BIGENDIAN
    shift = (3 - (int)p) << 3;
#else
    shift = (int)p << 3;
#endif

    return (int)(signed char)((v >> shift) & 0xff);
}


/* unaligned local_mem pointer q:

        return *(signed short *)((int)q+offset);
 */
int ua_get_s16_lmem(__lmem void *q, unsigned int offset)
{
    unsigned long long v;
    unsigned int shift;

    __lmem uint64_t *p = (__lmem uint64_t *)((int)q + offset);
    v = *p;

#if __BIGENDIAN
    shift = (6 - ((int)p & 3)) << 3;
#else
    shift = ((int)p & 3) << 3;
#endif

    return (int)(short)((unsigned)(v >> shift) & 0xffff);
}



/* unaligned sram pointer q:

        return *(signed short *)((int)q+offset);
 */
int ua_get_s16_sram(__sram void *q, unsigned int offset)
{
    unsigned long long v;
    unsigned int shift;

    __sram uint64_t *p = (__sram uint64_t *)((int)q + offset);
    v = *p;

#if __BIGENDIAN
    shift = (6 - ((int)p & 3)) << 3;
#else
    shift = ((int)p & 3) << 3;
#endif

    return (int)(short)((unsigned)(v >> shift) & 0xffff);
}

/* unaligned cls pointer q:

        return *(signed short *)((int)q+offset);
 */
int ua_get_s16_cls(__cls void *q, unsigned int offset)
{
    unsigned long long v;
    unsigned int shift;

    __cls uint64_t *p = (__cls uint64_t *)((int)q + offset);
    v = *p;

#if __BIGENDIAN
    shift = (6 - ((int)p & 3)) << 3;
#else
    shift = ((int)p & 3) << 3;
#endif

    return (int)(short)((unsigned)(v >> shift) & 0xffff);
}


/* unaligned local_mem pointer q:

        return *(int *)((int)q+offset);
 */
int ua_get_s32_lmem(__lmem void *q, unsigned int offset)
{
    unsigned long long v;
    unsigned int shift;

    __lmem uint64_t *p = (__lmem uint64_t *)((int)q + offset);
    v = *p;

#if __BIGENDIAN
    shift = (4 - ((int)p & 3)) << 3;
#else
    shift = ((int)p & 3) << 3;
#endif

    return (int)(v >> shift);
}


/* unaligned sram pointer q:

        return *(int *)((int)q+offset);
 */
int ua_get_s32_sram(__sram void *q, unsigned int offset)
{
    unsigned long long v;
    unsigned int shift;

    __sram uint64_t *p = (__sram uint64_t *)((int)q + offset);
    v = *p;

#if __BIGENDIAN
    shift = (4 - ((int)p & 3)) << 3;
#else
    shift = ((int)p & 3) << 3;
#endif

    return (int)(v >> shift);
}

/* unaligned cls pointer q:

        return *(int *)((int)q+offset);
 */
int ua_get_s32_cls(__cls void *q, unsigned int offset)
{
    unsigned long long v;
    unsigned int shift;

    __cls uint64_t *p = (__cls uint64_t *)((int)q + offset);
    v = *p;

#if __BIGENDIAN
    shift = (4 - ((int)p & 3)) << 3;
#else
    shift = ((int)p & 3) << 3;
#endif

    return (int)(v >> shift);
}


/* unaligned sram pointer q:

        return *(unsigned long long *)((int)q+offset);
 */
unsigned long long ua_get_64_sram(__sram void *q, unsigned int offset)
{
    threelong v;
    unsigned int shift, a, b;

    __sram uint32_t *p = (__sram uint32_t *)((int)q + offset);

    v = *(__sram threelong *)p;

    shift = ((int)p & 3) << 3;

#if __BIGENDIAN
    if (shift)
    {
        a = dbl_shl(v.a, v.b, shift);
        b = dbl_shl(v.b, v.c, shift);
        v.a = a;
        v.b = b;
    }
#else
    if (shift)
    {
        a = dbl_shr(v.b, v.a, shift);
        b = dbl_shr(v.c, v.b, shift);
        v.a = a;
        v.b = b;
    }
#endif
    return (unsigned long long)v.ll;
}


/* unaligned cls pointer q:

        return *(unsigned long long *)((int)q+offset);
 */
unsigned long long ua_get_64_cls(__cls void *q, unsigned int offset)
{
    threelong v;
    unsigned int shift, a, b;

    __cls uint32_t *p = (__cls uint32_t *)((int)q + offset);

    v = *(__cls threelong *)p;

    shift = ((int)p & 3) << 3;

#if __BIGENDIAN
    if (shift)
    {
        a = dbl_shl(v.a, v.b, shift);
        b = dbl_shl(v.b, v.c, shift);
        v.a = a;
        v.b = b;
    }
#else
    if (shift)
    {
        a = dbl_shr(v.b, v.a, shift);
        b = dbl_shr(v.c, v.b, shift);
        v.a = a;
        v.b = b;
    }
#endif
    return (unsigned long long)v.ll;
}

/* unaligned local_mem pointer q:

        return *(unsigned long long *)((int)q+offset);
 */
unsigned long long ua_get_64_lmem(__lmem void *q, unsigned int offset)
{
    threelong v;
    unsigned int shift, a, b;

    __lmem uint32_t *p = (__lmem uint32_t *)((int)q + offset);

    v = *(__lmem threelong *)p;

    shift = ((int)p & 3) << 3;

#if __BIGENDIAN
    if (shift)
    {
        a = dbl_shl(v.a, v.b, shift);
        b = dbl_shl(v.b, v.c, shift);
        v.a = a;
        v.b = b;
    }
#else
    if (shift)
    {
        a = dbl_shr(v.b, v.a, shift);
        b = dbl_shr(v.c, v.b, shift);
        v.a = a;
        v.b = b;
    }
#endif
    return (unsigned long long)v.ll;
}



/* unaligned sram pointer q:

        return *(unsigned long long *)((int)q+offset);
 */
unsigned long long ua_get_u64_sram(__sram void *p, unsigned int offset)
{
    return ua_get_64_sram(p, offset);
}

/* unaligned cls pointer q:

        return *(unsigned long long *)((int)q+offset);
 */
unsigned long long ua_get_u64_cls(__cls void *p, unsigned int offset)
{
    return ua_get_64_cls(p, offset);
}

/* unaligned local_mem pointer q:

        return *(unsigned long long *)((int)q+offset);
 */
unsigned long long ua_get_u64_lmem(__lmem void *p, unsigned int offset)
{
    return ua_get_64_lmem(p, offset);
}



/* unaligned sram pointer q:

        return *(long long *)((int)q+offset);
 */
long long ua_get_s64_sram(__sram void *p, unsigned int offset)
{
    return (long long)ua_get_64_sram(p, offset);
}

/* unaligned cls pointer q:

        return *(long long *)((int)q+offset);
 */
long long ua_get_s64_cls(__cls void *p, unsigned int offset)
{
    return (long long)ua_get_64_cls(p, offset);
}

/* unaligned local_mem pointer q:

        return *(long long *)((int)q+offset);
 */
long long ua_get_s64_lmem(__lmem void *p, unsigned int offset)
{
    return (long long)ua_get_64_lmem(p, offset);
}


/*
   unaligned memory copy: sram <- sram

 */
void ua_memcpy_sram_sram(
    __sram void *dst, unsigned int dst_off,
    __sram void *src, unsigned int src_off,
    unsigned int length)
{
    while (length >= 8)
    {
        ua_set_64(dst, dst_off, ua_get_u64(src, src_off));
        length -= 8;
        dst_off += 8;
        src_off += 8;
    }
    if (length >= 4)
    {
        ua_set_32(dst, dst_off, ua_get_u32(src, src_off));
        length -= 4;
        src_off += 4;
        dst_off += 4;
    }
    if (length >= 2)
    {
        ua_set_16(dst, dst_off, ua_get_u16(src, src_off));
        length -= 2;
        src_off += 2;
        dst_off += 2;
    }
    if (length)
    {
        ua_set_8(dst, dst_off, ua_get_u8(src, src_off));
    }
}


/*
   unaligned memory copy: lmem <- lmem

 */
void ua_memcpy_lmem_lmem(
    __lmem void *dst, unsigned int dst_off,
    __lmem void *src, unsigned int src_off,
    unsigned int length)
{
    __lmem int8_t *p;
    __lmem int8_t *q;
    unsigned int i;

    p = ((__lmem int8_t *) dst) + dst_off;
    q = ((__lmem int8_t *) src) + src_off;
    for ( i = 0; i < length; i++ )
    {
        *p++ = *q++;
    }
}

/*
   unaligned memory copy: lmem <- sram

 */
void ua_memcpy_lmem_sram(
    __lmem void *dst, unsigned int dst_off,
    __sram void *src, unsigned int src_off,
    unsigned int length)
{
    __lmem int8_t *p;
    __sram int8_t *q;
    unsigned int i;

    p = ((__lmem int8_t *) dst) + dst_off;
    q = ((__sram int8_t *) src) + src_off;
    for ( i = 0; i < length; i++ )
    {
        *p++ = *q++;
    }
}

/*
   unaligned memory copy: lmem <- cls

 */
void ua_memcpy_lmem_cls(
    __lmem void *dst, unsigned int dst_off,
    __cls void *src, unsigned int src_off,
    unsigned int length)
{
    __lmem int8_t *p;
    __cls int8_t *q;
    unsigned int i;

    p = ((__lmem int8_t *) dst) + dst_off;
    q = ((__cls int8_t *) src) + src_off;
    for ( i = 0; i < length; i++ )
    {
        *p++ = *q++;
    }
}


/*
   unaligned memory copy: sram <- lmem

 */
void ua_memcpy_sram_lmem(
    __sram void *dst, unsigned int dst_off,
    __lmem void *src, unsigned int src_off,
    unsigned int length)
{
    __sram int8_t *p;
    __lmem int8_t *q;
    unsigned int i;

    p = ((__sram int8_t *) dst) + dst_off;
    q = ((__lmem int8_t *) src) + src_off;
    for ( i = 0; i < length; i++ )
    {
        *p++ = *q++;
    }
}


/*
   unaligned memory copy: cls <- lmem

 */
void ua_memcpy_cls_lmem(
    __cls void *dst, unsigned int dst_off,
    __lmem void *src, unsigned int src_off,
    unsigned int length)
{
    __lmem int8_t *q;
    __cls int8_t *p;
    unsigned int i;

    p = ((__cls int8_t *) dst) + dst_off;
    q = ((__lmem int8_t *) src) + src_off;
    for ( i = 0; i < length; i++ )
    {
        *p++ = *q++;
    }
}

/*
   unaligned memory copy: LM + [0..7] <- xfer

   note: This function could be substantially smaller if
         1) byte_align_be[] can take *l$index0++
         2) byte_align_be can be separated
         3) LM offset is ADDed, not ORed
 */

void ua_memcpy_lmem0_7_xfer_w_clr(
    __lmem void *dst,    // aligned (>=4) dst (depends on length)
    unsigned int dst_off,               // 0..7 (better be constant)
    unsigned int ctx,                   // 0..7 (better be constant)
    unsigned int xfer_reg_number,       // __implicit_read(&xfer) should follow this call
    unsigned int src_off,               // any in range of xfer[]
    unsigned int length                 // in byte
)
{
#ifdef __BIGENDIAN
    unsigned int ii, Z, M, lm_off;
    unsigned int d = dst_off & 3;
    unsigned int s = src_off & 3;
    unsigned int tibi = (ctx << 6)                           |
                        (xfer_reg_number + (src_off >> 2) << 2) |
                        ((src_off - dst_off) & 3);
    local_csr_write(local_csr_t_index_byte_index, tibi);
    local_csr_write(local_csr_active_lm_addr_0, (unsigned int)dst);

    // zero out header longwords

    if (length < 4)
    {
        if (length)
        {
            __asm alu[M, --, B, *$index++]
            if (s)
            {
                ii = (4 - s) << 3;
                __asm
                {
                    alu[--, ii, OR, 0]
                    dbl_shf[M, M, *$index, >>indirect]
                }
            }

            switch (length)
            {
                case 0:
                    __asm ld_field_w_clr[Z, 0000b, M, >>0]
                    break;

                case 1:
                    __asm ld_field_w_clr[Z, 1000b, M, >>0]
                    break;

                case 2:
                    __asm ld_field_w_clr[Z, 1100b, M, >>0]
                    break;

                case 3:
                    __asm ld_field_w_clr[Z, 1110b, M, >>0]
                    break;

                default:
                        __impossible_path();
                    __switch_pack(swpack_auto);
            }

            ii = d << 3;
            if (d + length <= 4)
            {
                if (dst_off >> 2)
                {
                    // dst_off = 4..7
                    __asm
                    {
                        alu[*l$index0[0], ii, B, 0]
                        alu_shf[*l$index0[1], --, B, Z, >>indirect]
                    }
                }
                else
                {
                    __asm
                    {
                        alu[--, ii, OR, 0]
                        alu_shf[*l$index0[0], --, B, Z, >>indirect]
                    }
                }
            }
            else
            {
                if (dst_off >> 2)
                {
                    // dst_off = 4..7
                    __asm
                    {
                        alu[*l$index0[0], ii, B, 0]
                        dbl_shf[*l$index0[2], Z, 0, >>indirect]
                        alu[--, ii, OR, 0]
                        alu_shf[*l$index0[1], --, B, Z, >>indirect]
                    }
                }
                else
                {
                    __asm
                    {
                        alu[--, ii, OR, 0]
                        dbl_shf[*l$index0[1], Z, 0, >>indirect]
                        alu[--, ii, OR, 0]
                        alu_shf[*l$index0[0], --, B, Z, >>indirect]

                    }
                }
            }
        } // if (length)
    }
    else
    {
        //length >= 4

#if 0
        //BUG:
        *(unsigned int *)dst = 0;   // not needed when dst_off < 4 but branch is more expensive
        lm_off = 1 + (dst_off >> 2);
#endif

        if (d < s)
        {
            switch (s)
            {
                case 0:
                    __asm ld_field_w_clr[Z, 1111b, *$index++, >>0]
                    break;

                case 1:
                    __asm ld_field_w_clr[Z, 0111b, *$index++, >>0]
                    break;

                case 2:
                    __asm ld_field_w_clr[Z, 0011b, *$index++, >>0]
                    break;

                case 3:
                    __asm ld_field_w_clr[Z, 0001b, *$index++, >>0]
                    break;

                default:
                    __impossible_path();
                    __switch_pack(swpack_auto);
            }

            if (dst_off >> 2)
            {
                // dst_off = 4..7
                __asm alu[*l$index0[0], --, B, 0]
                lm_off = 1;
            }
            else
            {
                lm_off = 0;
            }

            length += d;
        }
        else
        {
            // d >= s
            if (d == s)
            {
                switch (d)
                {
                    case 0:
                        __asm ld_field_w_clr[Z, 1111b, *$index++, >>0]
                        break;

                    case 1:
                        __asm ld_field_w_clr[Z, 0111b, *$index++, >>0]
                        break;

                    case 2:
                        __asm ld_field_w_clr[Z, 0011b, *$index++, >>0]
                        break;

                    case 3:
                        __asm ld_field_w_clr[Z, 0001b, *$index++, >>0]
                        break;

                    default:
                        __impossible_path();
                        __switch_pack(swpack_auto);
                }
            }
            else
            {
                ii = (d - s) << 3;
                switch (d)
                {
                    case 0:
                        __asm
                        {
                            alu[--, ii, OR, 0]
                            ld_field_w_clr[Z, 1111b, *$index, >>indirect]
                        }
                        break;

                    case 1:
                        __asm
                        {
                            alu[--, ii, OR, 0]
                            ld_field_w_clr[Z, 0111b, *$index, >>indirect]
                        }
                        break;

                    case 2:
                        __asm
                        {
                            alu[--, ii, OR, 0]
                            ld_field_w_clr[Z, 0011b, *$index, >>indirect]
                        }
                        break;

                    case 3:
                        __asm
                        {
                            alu[--, ii, OR, 0]
                            ld_field_w_clr[Z, 0001b, *$index, >>indirect]
                        }
                        break;

                    default:
                        __impossible_path();
                        __switch_pack(swpack_auto);
                } // switch(d)
            }

            if (dst_off >> 2)
            {
                // dst_off = 4..7
                __asm alu[*l$index0[0], --, B, 0]
                __asm alu[*l$index0[1], --, B, Z]
                lm_off = 2;
            }
            else
            {
                __asm alu[*l$index0[0], --, B, Z]
                lm_off = 1;
            }
            __asm alu[Z, --, B, *$index++]

            length -= (4 - d);
        }

        switch (length >> 2)
        {
            case 0:
                __asm
                {
                    byte_align_be[         --, Z        ]
                    byte_align_be[          Z, *$index  ]
                }
                break;

            case 1:
                if (lm_off == 0)
                {
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                else if (lm_off == 1)
                {
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                else
                {
                    //ToDo: lm_off must be 2 but not checked
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                break;

            case 2:
                if (lm_off == 0)
                {
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                else if (lm_off == 1)
                {
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                else
                {
                    //ToDo: lm_off must be 2 but not checked
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                break;

            case 3:
                if (lm_off == 0)
                {
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                else if (lm_off == 1)
                {
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                else
                {
                    //ToDo: lm_off must be 2 but not checked
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                break;

            case 4:
                if (lm_off == 0)
                {
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                else if (lm_off == 1)
                {
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                else
                {
                    //ToDo: lm_off must be 2 but not checked
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                break;

            case 5:
                if (lm_off == 0)
                {
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                else if (lm_off == 1)
                {
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                else
                {
                    //ToDo: lm_off must be 2 but not checked
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[*l$index0[6], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                break;

            case 6:
                if (lm_off == 0)
                {
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                else if (lm_off == 1)
                {
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[*l$index0[6], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                else
                {
                    //ToDo: lm_off must be 2 but not checked
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[*l$index0[6], *$index++]
                        byte_align_be[*l$index0[7], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                break;

            case 7:
                if (lm_off == 0)
                {
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[*l$index0[6], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                else if (lm_off == 1)
                {
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[*l$index0[6], *$index++]
                        byte_align_be[*l$index0[7], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                else
                {
                    //ToDo: lm_off must be 2 but not checked
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[*l$index0[6], *$index++]
                        byte_align_be[*l$index0[7], *$index  ]
                    }

                    local_csr_write(local_csr_active_lm_addr_0, (unsigned int)((unsigned int *)dst + 8));

                    __asm
                    {
                        byte_align_be[          --, *$index++]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                break;

            case 8:
                if (lm_off == 0)
                {
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[*l$index0[6], *$index++]
                        byte_align_be[*l$index0[7], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                else if (lm_off == 1)
                {
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[*l$index0[6], *$index++]
                        byte_align_be[*l$index0[7], *$index  ]
                    }

                    local_csr_write(local_csr_active_lm_addr_0, (unsigned int)((unsigned int *)dst + 8));

                    __asm
                    {
                        byte_align_be[          --, *$index++]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                else
                {
                    //ToDo: lm_off must be 2 but not checked
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[*l$index0[6], *$index++]
                        byte_align_be[*l$index0[7], *$index  ]
                    }

                    local_csr_write(local_csr_active_lm_addr_0, (unsigned int)((unsigned int *)dst + 8));

                    __asm
                    {
                        byte_align_be[          --, *$index++]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                break;

            case 9:
                if (lm_off == 0)
                {
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[*l$index0[6], *$index++]
                        byte_align_be[*l$index0[7], *$index  ]
                    }

                    local_csr_write(local_csr_active_lm_addr_0, (unsigned int)((unsigned int *)dst + 8));

                    __asm
                    {
                        byte_align_be[          --, *$index++]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                else if (lm_off == 1)
                {
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[*l$index0[6], *$index++]
                        byte_align_be[*l$index0[7], *$index  ]
                    }

                    local_csr_write(local_csr_active_lm_addr_0, (unsigned int)((unsigned int *)dst + 8));

                    __asm
                    {
                        byte_align_be[          --, *$index++]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                else
                {
                    //ToDo: lm_off must be 2 but not checked
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[*l$index0[6], *$index++]
                        byte_align_be[*l$index0[7], *$index  ]
                    }

                    local_csr_write(local_csr_active_lm_addr_0, (unsigned int)((unsigned int *)dst + 8));

                    __asm
                    {
                        byte_align_be[          --, *$index++]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                break;

            case 10:
                if (lm_off == 0)
                {
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[*l$index0[6], *$index++]
                        byte_align_be[*l$index0[7], *$index  ]
                    }

                    local_csr_write(local_csr_active_lm_addr_0, (unsigned int)((unsigned int *)dst + 8));

                    __asm
                    {
                        byte_align_be[          --, *$index++]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                else if (lm_off == 1)
                {
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[*l$index0[6], *$index++]
                        byte_align_be[*l$index0[7], *$index  ]
                    }

                    local_csr_write(local_csr_active_lm_addr_0, (unsigned int)((unsigned int *)dst + 8));

                    __asm
                    {
                        byte_align_be[          --, *$index++]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                else
                {
                    //ToDo: lm_off must be 2 but not checked
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[*l$index0[6], *$index++]
                        byte_align_be[*l$index0[7], *$index  ]
                    }

                    local_csr_write(local_csr_active_lm_addr_0, (unsigned int)((unsigned int *)dst + 8));

                    __asm
                    {
                        byte_align_be[          --, *$index++]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                break;

            case 11:
                if (lm_off == 0)
                {
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[*l$index0[6], *$index++]
                        byte_align_be[*l$index0[7], *$index  ]
                    }

                    local_csr_write(local_csr_active_lm_addr_0, (unsigned int)((unsigned int *)dst + 8));

                    __asm
                    {
                        byte_align_be[          --, *$index++]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                else if (lm_off == 1)
                {
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[*l$index0[6], *$index++]
                        byte_align_be[*l$index0[7], *$index  ]
                    }

                    local_csr_write(local_csr_active_lm_addr_0, (unsigned int)((unsigned int *)dst + 8));

                    __asm
                    {
                        byte_align_be[          --, *$index++]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                else
                {
                    //ToDo: lm_off must be 2 but not checked
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[*l$index0[6], *$index++]
                        byte_align_be[*l$index0[7], *$index  ]
                    }

                    local_csr_write(local_csr_active_lm_addr_0, (unsigned int)((unsigned int *)dst + 8));

                    __asm
                    {
                        byte_align_be[          --, *$index++]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                break;

            case 12:
                if (lm_off == 0)
                {
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[*l$index0[6], *$index++]
                        byte_align_be[*l$index0[7], *$index  ]
                    }

                    local_csr_write(local_csr_active_lm_addr_0, (unsigned int)((unsigned int *)dst + 8));

                    __asm
                    {
                        byte_align_be[          --, *$index++]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                else if (lm_off == 1)
                {
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[*l$index0[6], *$index++]
                        byte_align_be[*l$index0[7], *$index  ]
                    }

                    local_csr_write(local_csr_active_lm_addr_0, (unsigned int)((unsigned int *)dst + 8));

                    __asm
                    {
                        byte_align_be[          --, *$index++]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                else
                {
                    //ToDo: lm_off must be 2 but not checked
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[*l$index0[6], *$index++]
                        byte_align_be[*l$index0[7], *$index  ]
                    }

                    local_csr_write(local_csr_active_lm_addr_0, (unsigned int)((unsigned int *)dst + 8));

                    __asm
                    {
                        byte_align_be[          --, *$index++]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                break;

            case 13:
                if (lm_off == 0)
                {
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[*l$index0[6], *$index++]
                        byte_align_be[*l$index0[7], *$index  ]
                    }

                    local_csr_write(local_csr_active_lm_addr_0, (unsigned int)((unsigned int *)dst + 8));

                    __asm
                    {
                        byte_align_be[          --, *$index++]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                else if (lm_off == 1)
                {
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[*l$index0[6], *$index++]
                        byte_align_be[*l$index0[7], *$index  ]
                    }

                    local_csr_write(local_csr_active_lm_addr_0, (unsigned int)((unsigned int *)dst + 8));

                    __asm
                    {
                        byte_align_be[          --, *$index++]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                else
                {
                    //ToDo: lm_off must be 2 but not checked
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[*l$index0[6], *$index++]
                        byte_align_be[*l$index0[7], *$index  ]
                    }

                    local_csr_write(local_csr_active_lm_addr_0, (unsigned int)((unsigned int *)dst + 8));

                    __asm
                    {
                        byte_align_be[          --, *$index++]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[*l$index0[6], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                break;

            case 14:
                if (lm_off == 0)
                {
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[*l$index0[6], *$index++]
                        byte_align_be[*l$index0[7], *$index  ]
                    }

                    local_csr_write(local_csr_active_lm_addr_0, (unsigned int)((unsigned int *)dst + 8));

                    __asm
                    {
                        byte_align_be[          --, *$index++]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                else if (lm_off == 1)
                {
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[*l$index0[6], *$index++]
                        byte_align_be[*l$index0[7], *$index  ]
                    }

                    local_csr_write(local_csr_active_lm_addr_0, (unsigned int)((unsigned int *)dst + 8));

                    __asm
                    {
                        byte_align_be[          --, *$index++]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[*l$index0[6], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                else
                {
                    //ToDo: lm_off must be 2 but not checked
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[*l$index0[6], *$index++]
                        byte_align_be[*l$index0[7], *$index  ]
                    }

                    local_csr_write(local_csr_active_lm_addr_0, (unsigned int)((unsigned int *)dst + 8));

                    __asm
                    {
                        byte_align_be[          --, *$index++]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[*l$index0[6], *$index++]
                        byte_align_be[*l$index0[7], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }

                }
                break;

            case 15:
                if (lm_off == 0)
                {
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[*l$index0[6], *$index++]
                        byte_align_be[*l$index0[7], *$index  ]
                    }

                    local_csr_write(local_csr_active_lm_addr_0, (unsigned int)((unsigned int *)dst + 8));

                    __asm
                    {
                        byte_align_be[          --, *$index++]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[*l$index0[6], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                else if (lm_off == 1)
                {
                    __asm
                    {
                        byte_align_be[          --, Z        ]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[*l$index0[6], *$index++]
                        byte_align_be[*l$index0[7], *$index  ]
                    }

                    local_csr_write(local_csr_active_lm_addr_0, (unsigned int)((unsigned int *)dst + 8));

                    __asm
                    {
                        byte_align_be[          --, *$index++]
                        byte_align_be[*l$index0[0], *$index++]
                        byte_align_be[*l$index0[1], *$index++]
                        byte_align_be[*l$index0[2], *$index++]
                        byte_align_be[*l$index0[3], *$index++]
                        byte_align_be[*l$index0[4], *$index++]
                        byte_align_be[*l$index0[5], *$index++]
                        byte_align_be[*l$index0[6], *$index++]
                        byte_align_be[*l$index0[7], *$index++]
                        byte_align_be[           Z, *$index  ]
                    }
                }
                else
                {
                    //ToDo: lm_off must be 2 but not checked
                    //ToDo: dst_off + length too large but not checked
                }
                break;
        } // switch(length >> 2)

        local_csr_write(local_csr_active_lm_addr_0, (unsigned int)((unsigned int *)dst + lm_off + (length >> 2)));

        switch (length & 3)
        {
            case 1:
                __asm ld_field_w_clr[*l$index0, 1000b, Z, >>0]
                break;

            case 2:
                __asm ld_field_w_clr[*l$index0, 1100b, Z, >>0]
                break;

            case 3:
                __asm ld_field_w_clr[*l$index0, 1110b, Z, >>0]
                break;
        }

    } // if (length < 4)
#endif /* __BIGENDIAN */
}

#define _UA_SET_HI_LOW_ADDRESS()                                        \
{                                                                       \
    hi_addr = (unsigned int)hi_q << 24;                                 \
    low_addr = (unsigned int)q;                                         \
}

/* buffer is at address 0 in the DRAM */

volatile __mem __addr40 host_comm_struct *host_comm = (__mem __addr40 void *)(0);

MUTEXLV comm_ready = 0;

/* exit program */

#ifdef __MUTEX_ON_EXIT

void exit(int status)
{
    MUTEXLV_lock(comm_ready, MUTEXLV_host_comm);

    host_comm->data = status;
    host_comm->command = hc_exit;

    while (host_comm->command)
    {
        ctx_swap();
    }

    MUTEXLV_unlock(comm_ready, MUTEXLV_host_comm);
    __asm ctx_arb[kill]
}

#else

#ifdef __NETRONOME_HARDWARE_VALIDATION
#undef put
#undef puts
extern void put(int ch);
extern void puts(__sram char *s);
#endif

/* Do not inline this to enable simulator to detect
 * reaching the exit label. */
void __noinline exit(int status)
{
    // for Netronome Hardware Validation
#ifdef __NETRONOME_HARDWARE_VALIDATION
    put(-1);
#endif

    __asm
    {
        nop
        nop
        nop
        ctx_arb[kill]
    }
}

void exit_thread(int status)
{
    // for Netronome Hardware Validation
#ifdef __NETRONOME_HARDWARE_VALIDATION
    put(-2);
#endif

    __asm
    {
        nop
        nop
        nop
        ctx_arb[kill]
    }
}

#endif



void ua_set_8_sram(__sram void *q, unsigned int offset, unsigned int val)
{
    __sram unsigned int *ap;
    unsigned int shift, byte_shift;
    unsigned int p = (unsigned int) q + offset;
    SIGNAL sig;
    __xwrite int sw;
    generic_ind_t sind;

    byte_shift = (unsigned int)p & 3;
    shift = byte_shift << 3;

//* Override length and byte_mask only */
#if __BIGENDIAN
    sw = val << (24 - shift);
    _INTRINSIC_OVERRIDE_LENGTH_AND_BYTEMASK_IN_ALU(sind, 0, (0x8 >> byte_shift));
#else
    sw = val << shift;
    _INTRINSIC_OVERRIDE_LENGTH_AND_BYTEMASK_IN_ALU(sind, 0, (0x1 << byte_shift));
#endif

    sram_write_ind(&sw, (void *) p, 1, sind, ctx_swap, &sig);
}


/* unaligned sram pointer q:

        *(unsigned short *)((int)q+offset) = (unsigned short)val;
 */
void ua_set_16_sram(__sram void *q, unsigned int offset, unsigned int val)
{
    __sram unsigned int *ap;
    unsigned int shift, byte_shift, v2;
    unsigned int p = (unsigned int) q + offset;
    unsigned int p2 = p + 4;
    SIGNAL sig1, sig2;
    __xwrite int sw1, sw2;
    generic_ind_t sind;
    unsigned long long l;

    byte_shift = (unsigned int)p & 3;
    shift = byte_shift << 3;


#ifdef __NFP_INDIRECT_REF_FORMAT_NFP3200
    sind.value = 0;
    sind.cnt_mask_ind.encoding = 0x3;  /* Override length & byte_mask only */
    #ifdef __BIGENDIAN
        v2 = val << 16;
        sw1 = v2 >> shift;
        sw2 = dbl_shr(v2, 0, shift);
        sind.cnt_mask_ind.byte_mask = 0xcU >> byte_shift;
    #else
        l = (unsigned long long) val << shift;
        sw1 = l;
        sw2 = l >> 32;
        sind.cnt_mask_ind.byte_mask = 0x3U << byte_shift;
    #endif
#endif

#ifdef __NFP_INDIRECT_REF_FORMAT_NFP6000
    #ifdef __BIGENDIAN
        v2 = val << 16;
        sw1 = v2 >> shift;
        sw2 = dbl_shr(v2, 0, shift);

        _INTRINSIC_OVERRIDE_LENGTH_AND_BYTEMASK_IN_ALU(sind, 0, (0xcU >> byte_shift));
    #else
        l = (unsigned long long) val << shift;
        sw1 = l;
        sw2 = l >> 32;

        _INTRINSIC_OVERRIDE_LENGTH_AND_BYTEMASK_IN_ALU(sind, 0, (0x3U <<  byte_shift));
    #endif
#endif

    sram_write_ind(&sw1, (void *) p, 1, sind, sig_done, &sig1);

#ifdef __NFP_INDIRECT_REF_FORMAT_NFP3200
#ifdef __BIGENDIAN
    sind.cnt_mask_ind.byte_mask = 0xcU << (4 - byte_shift);
#else
    sind.cnt_mask_ind.byte_mask = 0x3U << byte_shift >> 4;
#endif
#endif

#ifdef __NFP_INDIRECT_REF_FORMAT_NFP6000
#ifdef __BIGENDIAN
    _INTRINSIC_OVERRIDE_LENGTH_AND_BYTEMASK_IN_ALU(sind, 0, (0xcU << (4 - byte_shift)));
#else
    _INTRINSIC_OVERRIDE_LENGTH_AND_BYTEMASK_IN_ALU(sind, 0, (0x3U << byte_shift >> 4));
#endif
#endif

    sram_write_ind(&sw2, (void *) p2, 1, sind, sig_done, &sig2);
    wait_for_all(&sig1, &sig2);
    __implicit_read(&sw1);
    __implicit_read(&sw2);
}



void ua_set_32_sram(__sram void *q, unsigned int offset, unsigned int val)
{
    __sram unsigned int *ap;
    unsigned int shift, byte_shift, v2;
    unsigned int p = (unsigned int) q + offset;
    unsigned int p2 = (unsigned int) p + 4;
    SIGNAL sig1, sig2;
    __xwrite  int sw1, sw2;
    generic_ind_t sind;
    unsigned long long l;

    byte_shift = (unsigned int)p & 3;

    shift = byte_shift << 3;



#ifdef __NFP_INDIRECT_REF_FORMAT_NFP3200
    sind.value = 0;
    sind.cnt_mask_ind.encoding = 0x3;  /* Override length and byte_mask only */
    #if __BIGENDIAN
        sw1 = dbl_shr(0, val, shift);
        sw2 = dbl_shr(val, 0, shift);
        sind.cnt_mask_ind.byte_mask = 0xfU >> byte_shift;
    #else
        l = (unsigned long long) val << shift;
        sw1 = l;
        sw2 = l >> 32;
        sind.cnt_mask_ind.byte_mask = 0xfU << byte_shift;
    #endif
#endif

#ifdef __NFP_INDIRECT_REF_FORMAT_NFP6000

    #if __BIGENDIAN
        sw1 = dbl_shr(0, val, shift);
        sw2 = dbl_shr(val, 0, shift);

        _INTRINSIC_OVERRIDE_LENGTH_AND_BYTEMASK_IN_ALU(sind, 0, (0xfU >> byte_shift));
    #else
        l = (unsigned long long) val << shift;
        sw1 = l;
        sw2 = l >> 32;

        _INTRINSIC_OVERRIDE_LENGTH_AND_BYTEMASK_IN_ALU(sind, 0, (0xfU << byte_shift));
    #endif
#endif

    sram_write_ind(&sw1, (void *) p, 1, sind, sig_done, &sig1);

#ifdef __NFP_INDIRECT_REF_FORMAT_NFP3200
    #ifdef __BIGENDIAN
        sind.cnt_mask_ind.byte_mask = 0xfU << (4 - byte_shift);
    #else
        sind.cnt_mask_ind.byte_mask = 0xfU << byte_shift >> 4;
    #endif
#endif

#ifdef __NFP_INDIRECT_REF_FORMAT_NFP6000
    #ifdef __BIGENDIAN
        _INTRINSIC_OVERRIDE_LENGTH_AND_BYTEMASK_IN_ALU(sind, 0, (0xfU << (4 - byte_shift)));

    #else
        _INTRINSIC_OVERRIDE_LENGTH_AND_BYTEMASK_IN_ALU(sind, 0, (0xfU << byte_shift >> 4));
    #endif
#endif
    sram_write_ind(&sw2, (void *) p2, 1, sind, sig_done, &sig2);
    wait_for_all(&sig1, &sig2);
    __implicit_read(&sw1);
    __implicit_read(&sw2);
}




/* unaligned mem pointer q:

        *(unsigned char *)((int)q+offset) = (unsigned char)val;
 */

void ua_set_8_mem(__mem void *q, unsigned int offset, unsigned int val)
{
    volatile __mem void *p = (__mem void *)((unsigned int)q + offset);
    __xwrite unsigned int data[2];
    __mem uint8_t *pp = (__mem uint8_t* )((unsigned int)q + offset);
    SIGNAL bulk_sig;
    unsigned int shift, byte_shift;
    generic_ind_t dind;
    byte_shift = (unsigned int)p & 0x7;

#if __BIGENDIAN && __NFP_PERMIT_DRAM_UNALIGNED
    *pp = (uint8_t)val;
#else
#if __BIGENDIAN
    /* Adjust for around the compiler limitations of endian support.
    * Please read 3.1.10.1.2 "DRAM partial writes" in uCSupport.pdf" for detail.
    * */
    byte_shift = (7 - byte_shift + 4) & 0x7;
#endif

    shift = byte_shift << 3;

    data[0] = (val & 0xFF) << shift;
    if (shift < 32)
    {
        data[1] = 0;
    }
    else
    {
        data[1] = val << (shift - 32);
    }

    _INTRINSIC_OVERRIDE_BYTEMASK(dind, (0x1 << byte_shift));

    mem_write64_ind(data, p, 1, dind, sig_done, &bulk_sig);
    __wait_for_all(&bulk_sig);
#endif
}

/* unaligned mem pointer q:

        *(unsigned short *)((int)q+offset) = (unsigned short)val;
 */
void ua_set_16_mem(__mem void *q, unsigned int offset, unsigned int val)
{
    volatile __mem void *p = (__mem void *)((unsigned int)q + offset);
    __xwrite unsigned int data[2];
    SIGNAL bulk_sig;

    unsigned int shift, byte_shift;
#if __BIGENDIAN
#if __NFP_PERMIT_DRAM_UNALIGNED
    __mem uint16_t *pp = (__mem uint16_t* )((unsigned int)q + offset);
#endif

    unsigned int high_byte_shift, low_byte_shift;
    unsigned char low, high;
    unsigned int data0, data1;
#endif

    generic_ind_t dind;

#if __BIGENDIAN && __NFP_PERMIT_DRAM_UNALIGNED
    *pp = (uint16_t)val;
#else
    byte_shift = (unsigned int)p & 0x7;

    if (byte_shift < 7)
    {
        /* Not cross the 64-bit(MEM R/W unit size) */

#if __BIGENDIAN
        /* high byte */
        high = (unsigned char) ((val >> 8) & 0xFF);

        /* Adjust for around the compiler limitations of endian support.
         * Please read 3.1.10.1.2 "DRAM partial writes" in uCSupport.pdf" for detail.
         * */
        high_byte_shift = (7 - byte_shift + 4) & 0x7;

        shift = high_byte_shift << 3;

        data0 = high << shift;
        if (shift < 32)
        {
            data1 = 0;
        }
        else
        {
            data1 = high << (shift - 32);
        }

        /* low byte */
        low = (unsigned char) (val & 0xFF);
        low_byte_shift = (7 - (byte_shift + 1) + 4) & 0x7;
        shift = low_byte_shift << 3;

        data[0] = data0 | (low << shift);
        if (shift >= 32)
        {
            data[1] = data1 | (low << (shift - 32));
        }

#if (defined (__NFP_INDIRECT_REF_FORMAT_NFP3200) || defined(__NFP_INDIRECT_REF_FORMAT_NFP6000))
    _INTRINSIC_OVERRIDE_BYTEMASK(dind, ((0x1 << high_byte_shift) | (0x1 << low_byte_shift)));
#endif
#else /* Big Endian */
        shift = byte_shift << 3;

        data[0] = (val & 0xFFFF) << shift;
        if (shift < 24)
        {
            data[1] = 0;
        }
        else if (shift == 24)
        {
            data[1] = (val >> 8) & 0xFF;
        }
        else   /* shift >= 32 */
        {
            data[1] = val << (shift - 32);
        }


#if (defined (__NFP_INDIRECT_REF_FORMAT_NFP3200) || defined(__NFP_INDIRECT_REF_FORMAT_NFP6000))
    _INTRINSIC_OVERRIDE_BYTEMASK(dind, (0x3 << byte_shift));
#endif
#endif /* Little Endian */

        /* Do writing */
        /* NFP enhanced mode to clear lower 3 bits of address before writing to mem */
        p = (__mem void *)((unsigned int)p & ~7);

        mem_write64_ind(data, p, 1, dind, sig_done, &bulk_sig);
        __wait_for_all(&bulk_sig);
    }
    else
    {
        /* If the short word cross the 64-bit boundary */
#if __BIGENDIAN
        /* high byte */
        ua_set_8_mem(q, offset, (unsigned char)((val >> 8) & 0xFF));
        /* low byte */
        ua_set_8_mem(q, offset + 1, (unsigned char)(val & 0xFF));
#else /* Little endian */
        /* low byte */
        ua_set_8_mem(q, offset, (unsigned char)(val & 0xFF));
        /* high byte */
        ua_set_8_mem(q, offset + 1, (unsigned char)((val >> 8) & 0xFF));
#endif
    }
#endif
}

typedef union
{
    struct
    {
        int a;
        int b;
        int c;
        int d;
    };
    long long ll;
} fourlong;

/* unaligned mem pointer q:

        *(unsigned int *)((int)q+offset) = (unsigned int)val;
 */

void ua_set_32_mem(__mem void *q, unsigned int offset, unsigned int val)
{
    fourlong v;

    union
    {
        struct
        {
            int a;
            int b;
        };
        long long ll;
    } mask, data;
    unsigned int shift;

    __mem uint64_t *p = (__mem uint64_t *)((int)q + offset);
    __mem uint32_t *pp = (__mem uint32_t *)((int)q + offset);

    v = *(__mem fourlong *)p;


#if __BIGENDIAN && __NFP_PERMIT_DRAM_UNALIGNED
    *pp = val;
#else

#if __BIGENDIAN
    shift = (4 - ((int)p & 3)) << 3;
#else
    shift = ((int)p & 3) << 3;
#endif

    mask.ll = ~(0xffffffffULL << shift);
    data.ll = (unsigned long long) val << shift;

    if ((int)p & 4)
    {
        v.b &= mask.a;
        v.c &= mask.b;
        v.b |= data.a;
        v.c |= data.b;
    }
    else
    {
        v.a &= mask.a;
        v.b &= mask.b;
        v.a |= data.a;
        v.b |= data.b;
    }

    *(__mem fourlong *)p = v;
#endif
}

/* unaligned mem pointer q:

        *(unsigned long long *)((int)q+offset) = val;
 */

void ua_set_64_mem(__mem void *q, unsigned int offset, unsigned long long val)
{
    fourlong v;
    unsigned int shift, a, b, c;

    __mem uint64_t *p = (__mem uint64_t *)((int)q + offset);

    v = *(__mem fourlong *)p;

    shift = ((int)p & 7) << 3;

#if __BIGENDIAN && __NFP_PERMIT_DRAM_UNALIGNED
    *p =val;
#else
#if __BIGENDIAN
    if (shift)
    {
        a = val >> 32;
        b = val;

        if (shift == 32)
        {
            v.c = b;
            v.b = a;
        }
        else if (shift < 32)
        {
            c = b << (32 - shift);
            b = dbl_shr(a, b, shift);
            a = a >> shift;

            v.c = v.c & (0xffffffff >> shift) | c;
            v.b = b;
            v.a = v.a & (0xffffffff << (32 - shift)) | a;
        }
        else
        {
            c = b << (64 - shift);
            b = dbl_shr(a, b, shift - 32);
            a = a >> (shift - 32);

            v.d = v.d & (0xffffffff >> (shift - 32)) | c;
            v.c = b;
            v.b = v.b & (0xffffffff << (64 - shift)) | a;
        }
    }
    else
    {
        v.a = val >> 32;
        v.b = val;
    }

#else

    if (shift)
    {
        a = val;
        b = val >> 32;

        if (shift == 32)
        {
            v.c = b;
            v.b = a;
        }
        else if (shift < 32)
        {
            c = b >> (32 - shift);
            b = dbl_shl(b, a, shift);
            a = a << shift;

            v.c = v.c & (0xffffffff << shift) | c;
            v.b = b;
            v.a = v.a & (0xffffffff >> (32 - shift)) | a;
        }
        else
        {
            c = b >> (64 - shift);
            b = dbl_shl(b, a, shift - 32);
            a = a << (shift - 32);

            v.d = v.d & (0xffffffff << (shift - 32)) | c;
            v.c = b;
            v.b = v.b & (0xffffffff >> (64 - shift)) | a;
        }
    }
    else
    {
        v.a = val;
        v.b = val >> 32;
    }
#endif

    *(__mem fourlong *)p = v;
#endif
}





/* unaligned mem pointer q:

        return *(unsigned char *)((int)q+offset);
 */

unsigned int ua_get_u8_mem(__mem void *q, unsigned int offset)
{
    unsigned long long v;
    unsigned int shift;

    __mem uint8_t *pp = (__mem uint8_t *)((int)q + offset);

    __mem uint64_t *p = (__mem uint64_t *)((int)q + offset);

#if __BIGENDIAN && __NFP_PERMIT_DRAM_UNALIGNED
    return *pp;
#else
    v = *p;

#if __BIGENDIAN
    shift = (7 - ((int)p & 7)) << 3;
#else
    shift = ((int)p & 7) << 3;
#endif

    return (unsigned)(v >> shift) & 0xff;
#endif
}



/* unaligned mem pointer q:

        return *(unsigned short *)((int)q+offset);
 */

unsigned int ua_get_u16_mem(__mem void *q, unsigned int offset)
{
    unsigned long long v, v1;
    unsigned int shift, and_result;

#ifdef __NFP_PERMIT_DRAM_UNALIGNED
    __mem uint64_t *p = (__mem uint64_t *)(((int)q + offset) & ~7);
    and_result = ((int)q + offset) & 7;
#else
    __mem uint64_t *p = (__mem uint64_t *)((int)q + offset);
    and_result = (int) p & 7;
#endif

#ifdef __BIGENDIAN
    switch (and_result)
    {
        case 0:
            return (unsigned int)(*p >> 48);

        case 1:
            return (unsigned int)((*p >> 40) & 0xffff);

        case 2:
            return (unsigned int)((*p >> 32) & 0xffff);
        case 3:
            return (unsigned int)((*p >> 24) & 0xffff);

        case 4:
            return (unsigned int)((*p >> 16) & 0xffff);

        case 5:
            return (unsigned int)((*p >> 8) & 0xffff);

        case 6:
            return (unsigned int)((*p) & 0xffff);

        case 7:
            v = *p;
            v1 = *(p + 1);
            return (unsigned int)(((v << 8) & 0xff00) | ((v1 >> 56) & 0xff));

    }
#else
    switch (and_result)
    {
        case 0:
            return (*p) & 0xffff;

        case 1:
            return (*p >> 8) & 0xffff;

        case 2:
            return (*p >> 16) & 0xffff;

        case 3:
            return (*p >> 24) & 0xffff;

        case 4:
            return (*p >> 32) & 0xffff;

        case 5:
            return (*p >> 40) & 0xffff;

        case 6:
            return (*p >> 48);

        case 7:
            v = *p;
            v1 = *(p + 1);
            return ((v >> 56 ) & 0xff) | ((v1 << 8) & 0xff00);
    }
#endif
    return 0;
}


/* unaligned mem pointer q:

        return *(unsigned int *)((int)q+offset);
 */
unsigned int ua_get_u32_mem(__mem void *q, unsigned int offset)
{
    unsigned long long v, v1;
    unsigned int shift;

    __mem uint64_t *p = (__mem uint64_t *)((int)q + offset);
#if __BIGENDIAN && __NFP_PERMIT_DRAM_UNALIGNED
    __mem uint32_t*pp = (__mem uint32_t *)((int)q + offset);
    return *pp;
#else
#if __BIGENDIAN
    switch ((int) p & 7)
    {
        case 0:
            return (*p >> 32);

        case 1:
            return (*p >> 24);

        case 2:
            return (*p >> 16);

        case 3:
            return (*p >> 8);

        case 4:
            return (*p);

        case 5:
            v = *p;
            v1 = *(p + 1);
            return (v << 8) | (v1 >> 56);

        case 6:
            v = *p;
            v1 = *(p + 1);
            return (v << 16) | (v1 >> 48);

        case 7:
            v = *p;
            v1 = *(p + 1);
            return (v << 24) | (v1 >> 40);
    }
#else
    switch ((int) p & 7)
    {
        case 0:
            return (*p);

        case 1:
            return (*p >> 8);

        case 2:
            return (*p >> 16);

        case 3:
            return (*p >> 24);

        case 4:
            return (*p >> 32);

        case 5:
            v = *p;
            v1 = *(p + 1);
            return (v >> 40 ) | (v1 << 24);

        case 6:
            v = *p;
            v1 = *(p + 1);
            return (v >> 48 ) | (v1 << 16);

        case 7:
            v = *p;
            v1 = *(p + 1);
            return (v >> 56 ) | (v1 << 8);
    }
#endif
    return 0;
#endif
}



/* unaligned mem pointer q:

        return *(signed char *)((int)q+offset);
 */
int ua_get_s8_mem(__mem void *q, unsigned int offset)
{
    unsigned long long v;
    unsigned int shift;

    __mem int8_t *pp = (__mem int8_t *)((int)q + offset);

    __mem uint64_t *p = (__mem uint64_t *)((int)q + offset);
    v = *p;

#if __BIGENDIAN && __NFP_PERMIT_DRAM_UNALIGNED
    return *pp;
#else

#if __BIGENDIAN
    shift = (7 - ((int)p & 7)) << 3;
#else
    shift = ((int)p & 7) << 3;
#endif

    return (int)(signed char)((unsigned)(v >> shift) & 0xff);
#endif
}




/* unaligned mem pointer q:

        return *(signed short *)((int)q+offset);
 */
int ua_get_s16_mem(__mem void *q, unsigned int offset)
{
    unsigned long long v, v1;
    unsigned int shift, and_result;

#ifdef __NFP_PERMIT_DRAM_UNALIGNED
    __mem uint64_t *p = (__mem uint64_t *)(((int)q + offset) & ~7);
    and_result = ((int)q + offset) & 7;
#else
    __mem uint64_t *p = (__mem uint64_t *)((int)q + offset);
    and_result = (int) p & 7;
#endif

#ifdef __BIGENDIAN
    switch (and_result)
    {
         case 0:
            return (short) (*p >> 48);

        case 1:
            return (short) ((*p >> 40) & 0xffff);

        case 2:
            return (short) ((*p >> 32) & 0xffff);

        case 3:
            return (short) ((*p >> 24) & 0xffff);

        case 4:
            return (short) ((*p >> 16) & 0xffff);

        case 5:
            return (short) ((*p >> 8) & 0xffff);

        case 6:
            return (short) ((*p) & 0xffff);

       case 7:
            v = *p;
            v1 = *(p + 1);
            return (short) (((v << 8) & 0xff00) | ((v1 >> 56) & 0xff));
    }
#else
    switch (and_result)
    {
        case 0:
            return (short) ((*p) & 0xffff);

        case 1:
            return (short) ((*p >> 8) & 0xffff);

        case 2:
            return (short) ((*p >> 16) & 0xffff);

        case 3:
            return (short) ((*p >> 24) & 0xffff);

        case 4:
            return (short) ((*p >> 32) & 0xffff);

        case 5:
            return (short) ((*p >> 40) & 0xffff);

        case 6:
            return (short) ((*p >> 48));

        case 7:
            v = *p;
            v1 = *(p + 1);
            return (short) ((v >> 56 ) & 0xff) | ((v1 << 8) & 0xff00);
    }
#endif
    return 0;
}





/* unaligned mem pointer q:

        return *(int *)((int)q+offset);
 */

int ua_get_s32_mem(__mem void *q, unsigned int offset)
{
    unsigned long long v, v1;
    unsigned int shift;

    __mem uint64_t *p = (__mem uint64_t *)((int)q + offset);
#if __BIGENDIAN && __NFP_PERMIT_DRAM_UNALIGNED
    __mem int32_t *pp = (__mem int32_t *)((int)q+offset);
    return *pp;
#else
#if __BIGENDIAN
    switch ((int) p & 7)
    {
        case 0:
            return (*p >> 32);

        case 1:
            return (*p >> 24);

        case 2:
            return (*p >> 16);

        case 3:
            return (*p >> 8);

        case 4:
            return (*p);

        case 5:
            v = *p;
            v1 = *(p + 1);
            return (v << 8) | (v1 >> 56);

        case 6:
            v = *p;
            v1 = *(p + 1);
            return (v << 16) | (v1 >> 48);

        case 7:
            v = *p;
            v1 = *(p + 1);
            return (v << 24) | (v1 >> 40);
    }
#else
    switch ((int) p & 7)
    {
        case 0:
            return (*p);

        case 1:
            return (*p >> 8);

        case 2:
            return (*p >> 16);

        case 3:
            return (*p >> 24);

        case 4:
            return (*p >> 32);

        case 5:
            v = *p;
            v1 = *(p + 1);
            return (v >> 40 ) | (v1 << 24);

        case 6:
            v = *p;
            v1 = *(p + 1);
            return (v >> 48 ) | (v1 << 16);

        case 7:
            v = *p;
            v1 = *(p + 1);
            return (v >> 56 ) | (v1 << 8);
    }
#endif
    return 0;
#endif
}



/* unaligned mem pointer q:

        return *(unsigned long long *)((int)q+offset);
 */
unsigned long long ua_get_64_mem(__mem void *q, unsigned int offset)
{
    fourlong v;
    unsigned int shift;

    __mem uint64_t *p = (__mem uint64_t *)((int)q + offset);

    v = *(__mem fourlong *)p;

    shift = ((int)p & 3) << 3;

#if __BIGENDIAN && __NFP_PERMIT_DRAM_UNALIGNED
    return *p;
#else
#if __BIGENDIAN
    if (shift)
    {
        if ((int)p & 4)
        {
            v.a = dbl_shl(v.b, v.c, shift);
            v.b = dbl_shl(v.c, v.d, shift);
        }
        else
        {
            v.a = dbl_shl(v.a, v.b, shift);
            v.b = dbl_shl(v.b, v.c, shift);
        }
    }
    else if ((int)p & 4)
    {
        v.a = v.b;
        v.b = v.c;
    }
#else
    if (shift)
    {
        if ((int)p & 4)
        {
            v.a = dbl_shr(v.c, v.b, shift);
            v.b = dbl_shr(v.d, v.c, shift);
        }
        else
        {
            v.a = dbl_shr(v.b, v.a, shift);
            v.b = dbl_shr(v.c, v.b, shift);
        }
    }
    else if ((int)p & 4)
    {
        v.a = v.b;
        v.b = v.c;
    }
#endif
    return (unsigned long long)v.ll;
#endif
}


/* unaligned mem pointer q:

        return *(unsigned long long *)((int)q+offset);
 */

unsigned long long ua_get_u64_mem(__mem void *p, unsigned int offset)
{
    return ua_get_64_mem(p, offset);
}



/* unaligned mem pointer q:

        return *(long long *)((int)q+offset);
 */
long long ua_get_s64_mem(__mem void *p, unsigned int offset)
{
    return (long long)ua_get_64_mem(p, offset);
}

#ifdef __PTR40
void ua_set_ptr40_8_mem(__mem void *hi_q, __mem void *q, unsigned int offset, unsigned int val)
{
    __mem __addr40 char *pp;
    unsigned int hi_addr;
    unsigned int low_addr;

    _UA_SET_HI_LOW_ADDRESS();
    pp = (__mem __addr40 unsigned char *)(unsigned long long)((((unsigned long long)hi_addr)<<8) + low_addr + offset);

    #if __BIGENDIAN
        *pp = (char)val;
        return;
    #else
        //TODO: Little endian
    #endif
}

void ua_set_ptr40_16_mem(__mem void *hi_q, __mem void *q, unsigned int offset, unsigned int val)
{
    __mem __addr40 unsigned short *pp;
    unsigned int hi_addr;
    unsigned int low_addr;

    _UA_SET_HI_LOW_ADDRESS();
    pp = (__mem __addr40 unsigned short *)(unsigned long long)((((unsigned long long)hi_addr)<<8) + low_addr + offset);

    #if __BIGENDIAN
        *pp = (unsigned short)val;
        return;
    #else
        //TODO: Little endian
    #endif
}

void ua_set_ptr40_32_mem(__mem void *hi_q, __mem void *q, unsigned int offset, unsigned int val)
{
    __mem __addr40 unsigned int *pp;
    unsigned int hi_addr;
    unsigned int low_addr;

   _UA_SET_HI_LOW_ADDRESS();
    pp = (__mem __addr40 unsigned int *)(unsigned long long)((((unsigned long long)hi_addr)<<8) + low_addr + offset);

    #if __BIGENDIAN
        *pp = val;
        return;
    #else
        //TODO: Little endian
    #endif
}

void ua_set_ptr40_64_mem(__mem void *hi_q, __mem void *q, unsigned int offset, unsigned long long val)
{
    __mem __addr40 unsigned long long *pp;
    unsigned int hi_addr;
    unsigned int low_addr;

    _UA_SET_HI_LOW_ADDRESS();
    pp = (__mem __addr40 unsigned long long *)(unsigned long long)((((unsigned long long)hi_addr)<<8) + low_addr + offset);

    #if __BIGENDIAN
        *pp =val;
        return;
    #else
        //TODO: Little endian
    #endif
}

unsigned long long ua_get_ptr40_64_mem(__mem void *hi_q, __mem void *q, unsigned int offset)
{
    __mem __addr40 unsigned long long *p;
    unsigned int hi_addr;
    unsigned int low_addr;

    _UA_SET_HI_LOW_ADDRESS();
    p = (__mem __addr40 unsigned long long *)(unsigned long long)((((unsigned long long)hi_addr)<<8) + low_addr + offset);

    #if __BIGENDIAN
        return *p;
    #else
        //TODO: Little endian
    #endif
}

unsigned int ua_get_ptr40_32_mem(__mem void *hi_q, __mem void *q, unsigned int offset)
{
    __mem __addr40 unsigned int *p;
    unsigned int hi_addr;
    unsigned int low_addr;

    _UA_SET_HI_LOW_ADDRESS();
    p = (__mem __addr40 unsigned int *)(unsigned long long)((((unsigned long long)hi_addr)<<8) + low_addr + offset);

    #if __BIGENDIAN
        return *p;
    #else
        //TODO: Little endian
    #endif
}

unsigned short ua_get_ptr40_16_mem(__mem void *hi_q, __mem void *q, unsigned int offset)
{
    __mem __addr40 unsigned short *p;
    unsigned int hi_addr;
    unsigned int low_addr;

    _UA_SET_HI_LOW_ADDRESS();
    p = (__mem __addr40 unsigned short *)(unsigned long long)((((unsigned long long)hi_addr)<<8) + low_addr + offset);

    #if __BIGENDIAN
        return *p;
    #else
        //TODO: Little endian
    #endif
}

unsigned char ua_get_ptr40_8_mem(__mem void *hi_q, __mem void *q, unsigned int offset)
{
    __mem __addr40 char *p;
    unsigned int hi_addr;
    unsigned int low_addr;

    _UA_SET_HI_LOW_ADDRESS();
    p = (__mem __addr40 unsigned char *)(unsigned long long)((((unsigned long long)hi_addr)<<8) + low_addr + offset);

    #if __BIGENDIAN
        return *p;
    #else
        //TODO: Little endian
    #endif
}

void ua_set_8_mem40(__mem __addr40 void *ptr, unsigned off, unsigned val)
{
    *(__mem __addr40 uint8_t *)((uint64_t)ptr + off) = (uint8_t)val;
}

void ua_set_16_mem40(__mem __addr40 void *ptr, unsigned off, unsigned val)
{
    *(__mem __addr40 uint16_t *)((uint64_t)ptr + off) = (uint16_t)val;
}

void ua_set_32_mem40(__mem __addr40 void *ptr, unsigned off, unsigned val)
{
    *(__mem __addr40 int32_t *)((uint64_t)ptr + off) = val;
}

void ua_set_64_mem40(__mem __addr40 void *ptr, unsigned off, unsigned long long val)
{
    *(__mem __addr40 uint64_t *)((uint64_t)ptr + off) = val;
}

#endif /* __PTR40 */


/*
   unaligned memory copy: mem <- mem

 */
void ua_memcpy_mem_mem(__mem void *dst, unsigned int dst_off, __mem void *src, unsigned int src_off, unsigned int length)
{
    while (length >= 8)
    {
#if __NFP_PERMIT_DRAM_UNALIGNED
        __mem uint64_t *ddst = (__mem uint64_t *)((int)dst + dst_off);
        __mem uint64_t *ssrc = (__mem uint64_t *)((int)src + src_off);
        *ddst = *ssrc;
#else
        ua_set_64(dst, dst_off, ua_get_u64(src, src_off));
#endif
        length -= 8;
        dst_off += 8;
        src_off += 8;
    }
    if (length >= 4)
    {
#if __NFP_PERMIT_DRAM_UNALIGNED
        __mem uint32_t *ddst = (__mem uint32_t *)((int)dst + dst_off);
        __mem uint32_t *ssrc = (__mem uint32_t *)((int)src + src_off);
        *ddst = *ssrc;
#else
        ua_set_32(dst, dst_off, ua_get_u32(src, src_off));
#endif
        length -= 4;
        src_off += 4;
        dst_off += 4;
    }
    if (length >= 2)
    {
#if __NFP_PERMIT_DRAM_UNALIGNED
        __mem uint16_t *ddst = (__mem uint16_t *)((int)dst + dst_off);
        __mem uint16_t *ssrc = (__mem uint16_t *)((int)src + src_off);
        *ddst = *ssrc;
#else
        ua_set_16(dst, dst_off, ua_get_u16(src, src_off));
#endif
        length -= 2;
        src_off += 2;
        dst_off += 2;
    }
    if (length)
    {
#if __NFP_PERMIT_DRAM_UNALIGNED
        __mem uint8_t *ddst = (__mem uint8_t *)((int)dst + dst_off);
        __mem uint8_t *ssrc = (__mem uint8_t *)((int)src + src_off);
        *ddst = *ssrc;
#else
        ua_set_8(dst, dst_off, ua_get_u8(src, src_off));
#endif
    }
}

/*
   unaligned memory copy: mem <- sram

 */
void ua_memcpy_mem_sram(__mem void *dst, unsigned int dst_off, __sram void *src, unsigned int src_off, unsigned int length)
{
    while (length >= 8)
    {
        ua_set_64_mem(dst, dst_off, ua_get_u64_sram(src, src_off));
        length -= 8;
        dst_off += 8;
        src_off += 8;
    }
    if (length >= 4)
    {
        ua_set_32_mem(dst, dst_off, ua_get_u32_sram(src, src_off));
        length -= 4;
        src_off += 4;
        dst_off += 4;
    }
    if (length >= 2)
    {
        ua_set_16_mem(dst, dst_off, ua_get_u16_sram(src, src_off));
        length -= 2;
        src_off += 2;
        dst_off += 2;
    }
    if (length)
    {
        ua_set_8_mem(dst, dst_off, ua_get_u8_sram(src, src_off));
    }
}

/*
   unaligned memory copy: mem <- cls

 */
void ua_memcpy_mem_cls(__mem void *dst, unsigned int dst_off, __cls void *src, unsigned int src_off, unsigned int length)
{
    while (length >= 8)
    {
        ua_set_64_mem(dst, dst_off, ua_get_u64_cls((void *)src, src_off));
        length -= 8;
        dst_off += 8;
        src_off += 8;
    }
    if (length >= 4)
    {
        ua_set_32_mem(dst, dst_off, ua_get_u32_cls((void *)src, src_off));
        length -= 4;
        src_off += 4;
        dst_off += 4;
    }
    if (length >= 2)
    {
        ua_set_16_mem(dst, dst_off, ua_get_u16_cls((void *)src, src_off));
        length -= 2;
        src_off += 2;
        dst_off += 2;
    }
    if (length)
    {
        ua_set_8_mem(dst, dst_off, ua_get_u8_cls((void *)src, src_off));
    }
}


/*
   unaligned memory copy: sram <- mem

 */
void ua_memcpy_sram_mem(__sram void *dst, unsigned int dst_off, __mem void *src, unsigned int src_off, unsigned int length)
{
    while (length >= 8)
    {
#if __NFP_PERMIT_DRAM_UNALIGNED
        __mem uint64_t *ssrc = (__mem uint64_t *)((int)src + src_off);
        ua_set_64(dst, dst_off, *ssrc);
#else
        ua_set_64(dst, dst_off, ua_get_u64(src, src_off));
#endif
        length -= 8;
        dst_off += 8;
        src_off += 8;
    }
    if (length >= 4)
    {
#if __NFP_PERMIT_DRAM_UNALIGNED
        __mem uint32_t *ssrc = (__mem uint32_t *)((int)src + src_off);
        ua_set_32(dst, dst_off, *ssrc);
#else
        ua_set_32(dst, dst_off, ua_get_u32(src, src_off));
#endif
        length -= 4;
        src_off += 4;
        dst_off += 4;
    }
    if (length >= 2)
    {
#if __NFP_PERMIT_DRAM_UNALIGNED
        __mem uint16_t *ssrc = (__mem uint16_t *)((int)src + src_off);
        ua_set_16(dst, dst_off, *ssrc);
#else
        ua_set_16(dst, dst_off, ua_get_u16(src, src_off));
#endif
        length -= 2;
        src_off += 2;
        dst_off += 2;
    }
    if (length)
    {
#if __NFP_PERMIT_DRAM_UNALIGNED
        __mem uint8_t *ssrc = (__mem uint8_t *)((int)src + src_off);
        ua_set_8(dst, dst_off, *ssrc);
#else
        ua_set_8(dst, dst_off, ua_get_u8(src, src_off));
#endif
    }
}


/*
   unaligned memory copy: cls <- mem

 */
void ua_memcpy_cls_mem(__cls void *dst, unsigned int dst_off, __mem void *src, unsigned int src_off, unsigned int length)
{
    while (length >= 8)
    {
#if __NFP_PERMIT_DRAM_UNALIGNED
        __mem uint64_t *ssrc = (__mem uint64_t *)((int)src + src_off);
        ua_set_64_cls(dst, dst_off, *ssrc);
#else
        ua_set_64_cls((void *)dst, dst_off, ua_get_u64(src, src_off));
#endif
        length -= 8;
        dst_off += 8;
        src_off += 8;
    }
    if (length >= 4)
    {
#if __NFP_PERMIT_DRAM_UNALIGNED
        __mem uint32_t *ssrc = (__mem uint32_t *)((int)src + src_off);
        ua_set_32_cls(dst, dst_off, *ssrc);
#else
        ua_set_32_cls((void *)dst, dst_off, ua_get_u32(src, src_off));
#endif
        length -= 4;
        src_off += 4;
        dst_off += 4;
    }
    if (length >= 2)
    {
#if __NFP_PERMIT_DRAM_UNALIGNED
        __mem uint16_t *ssrc = (__mem uint16_t *)((int)src + src_off);
        ua_set_16_cls(dst, dst_off, *ssrc);
#else
        ua_set_16_cls((void *)dst, dst_off, ua_get_u16(src, src_off));
#endif
        length -= 2;
        src_off += 2;
        dst_off += 2;
    }
    if (length)
    {
#if __NFP_PERMIT_DRAM_UNALIGNED
        __mem uint8_t *ssrc = (__mem uint8_t *)((int)src + src_off);
        ua_set_8_cls(dst, dst_off, *ssrc);
#else
        ua_set_8_cls((void *)dst, dst_off, ua_get_u8(src, src_off));
#endif
    }
}


/*
   unaligned memory copy: sram <- cls

 */
void ua_memcpy_sram_cls(__sram void *dst, unsigned int dst_off, __cls void *src, unsigned int src_off, unsigned int length)
{
    while (length >= 8)
    {
        ua_set_64_sram(dst, dst_off, ua_get_u64_cls((__cls void *)src, src_off));
        length -= 8;
        dst_off += 8;
        src_off += 8;
    }
    if (length >= 4)
    {
        ua_set_32_sram(dst, dst_off, ua_get_u32_cls((__cls void *)src, src_off));
        length -= 4;
        src_off += 4;
        dst_off += 4;
    }
    if (length >= 2)
    {
        ua_set_16_sram(dst, dst_off, ua_get_u16_cls((__cls void *)src, src_off));
        length -= 2;
        src_off += 2;
        dst_off += 2;
    }
    if (length)
    {
        ua_set_8_sram(dst, dst_off, ua_get_u8_cls((__cls void *)src, src_off));
    }
}

/*
   unaligned memory copy: cls <- sram

 */
void ua_memcpy_cls_sram(__cls void *dst, unsigned int dst_off, __sram void *src, unsigned int src_off, unsigned int length)
{
    while (length >= 8)
    {
        ua_set_64_cls((__cls void *)dst, dst_off, ua_get_u64_sram(src, src_off));
        length -= 8;
        dst_off += 8;
        src_off += 8;
    }
    if (length >= 4)
    {
        ua_set_32_cls((__cls void *)dst, dst_off, ua_get_u32_sram(src, src_off));
        length -= 4;
        src_off += 4;
        dst_off += 4;
    }
    if (length >= 2)
    {
        ua_set_16_cls((__cls void *)dst, dst_off, ua_get_u16_sram(src, src_off));
        length -= 2;
        src_off += 2;
        dst_off += 2;
    }
    if (length)
    {
        ua_set_8_cls((__cls void *)dst, dst_off, ua_get_u8_sram(src, src_off));
    }
}

/*
   unaligned memory copy: cls <- cls

 */
void ua_memcpy_cls_cls(
    __cls void *dst, unsigned int dst_off,
    __cls void *src, unsigned int src_off,
    unsigned int length)
{
    while (length >= 8)
    {
        ua_set_64_cls((__cls void *)dst, dst_off, ua_get_u64_cls((__cls void *)src, src_off));
        length -= 8;
        dst_off += 8;
        src_off += 8;
    }
    if (length >= 4)
    {
        ua_set_32_cls((__cls void *)dst, dst_off, ua_get_u32_cls((__cls void *)src, src_off));
        length -= 4;
        src_off += 4;
        dst_off += 4;
    }
    if (length >= 2)
    {
        ua_set_16_cls((__cls void *)dst, dst_off, ua_get_u16_cls((__cls void *)src, src_off));
        length -= 2;
        src_off += 2;
        dst_off += 2;
    }
    if (length)
    {
        ua_set_8_cls((__cls void *)dst, dst_off, ua_get_u8_cls((__cls void *)src, src_off));
    }
}

/*
   unaligned memory copy: lmem <- mem

 */
void ua_memcpy_lmem_mem(__lmem void *dst, unsigned int dst_off, __mem void *src, unsigned int src_off, unsigned int length)
{
    __lmem int8_t *p;
    __mem int8_t *q;
    unsigned int i;

    p = ((__lmem int8_t *) dst) + dst_off;
    q = ((__mem int8_t *) src) + src_off;
    for ( i = 0; i < length; i++ )
    {
        *p++ = *q++;
    }
}


/*
   unaligned memory copy: mem <- lmem

 */
void ua_memcpy_mem_lmem(__mem void *dst, unsigned int dst_off, __lmem void *src, unsigned int src_off, unsigned int length)
{
    __mem int8_t *p;
    __lmem int8_t *q;
    unsigned int i;

    p = ((__mem int8_t *) dst) + dst_off;
    q = ((__lmem int8_t *) src) + src_off;
    for ( i = 0; i < length; i++ )
    {
        *p++ = *q++;
    }
}


/*
 * The following were originally part of intrinsic.c.
 */

__intrinsic
void sram_write_ind(__xwrite void *data, volatile void __sram *address, unsigned int max_nn, generic_ind_t ind, sync_t sync, SIGNAL *sig_ptr)
{
    {
        __ct_assert(__is_write_reg(data), "__is_write_reg(data)");
        __ct_assert(__is_ct_const(max_nn), "__is_ct_const(max_nn)");
        __ct_assert(max_nn >= 1 && max_nn <= 16, "max_nn >= 1 && max_nn <= 16");
        __ct_assert(__is_ct_const(sync), "__is_ct_const(sync)");
        __ct_assert(sync == sig_done || sync == ctx_swap, "sync == sig_done || sync == ctx_swap");

#ifdef  __NFP_INDIRECT_REF_FORMAT_NFP6000
        __asm local_csr_wr[CMD_INDIRECT_REF_0, CSR_INDIRECT_TYPE(ind)]
#endif
        __asm alu[--, --, B, ALU_INDIRECT_TYPE(ind)]

        if (sync == sig_done)
        {
            __asm sram[write, *data, address, 0, __ct_const_val(max_nn)], sig_done[*sig_ptr], indirect_ref
        }
        else
        {
            __asm sram[write, *data, address, 0, __ct_const_val(max_nn)], ctx_swap[*sig_ptr], indirect_ref
        }
    }
}


/*
 * The following were originally part of intrinsic.c.
 */

__intrinsic
unsigned int local_csr_read(local_csr_t lcsr)
{
    unsigned int result;
    {
        __ct_assert(__is_ct_const(lcsr), "__is_ct_const(lcsr)");

        __asm
        {
            local_csr_rd[__ct_const_val(lcsr)]
            immed[result, 0]           /* immed immediately follows, don't even use separate __asm block */
        }
    }
    return result;
}

__intrinsic
void local_csr_write(local_csr_t lcsr, unsigned int data)
{
    {
        __ct_assert(__is_ct_const(lcsr), "__is_ct_const(lcsr)");

        __asm local_csr_wr[__ct_const_val(lcsr), data]
    }
}

__intrinsic
unsigned int dbl_shr(
    unsigned int srcA,          /* high 32 bits */
    unsigned int srcB,          /* low 32 bits */
    unsigned int shift_cnt      /* shift count */
)
{
    unsigned result;
    {
        if (__is_ct_const(shift_cnt))
        {
            __ct_assert(shift_cnt >= 1 && shift_cnt <= 31, "shift_cnt >= 1 && shift_cnt <= 31");
            __asm dbl_shf[result, srcA, srcB, >>__ct_const_val(shift_cnt)]
        }
        else
        {
            __asm
            {
                alu[--, shift_cnt, OR, 0]
                dbl_shf[result, srcA, srcB, >>indirect]
            }
        }
    }
    return result;
}

__intrinsic
unsigned int dbl_shl(
    unsigned int srcA,          /* high 32 bits */
    unsigned int srcB,          /* low 32 bits */
    unsigned int shift_cnt      /* shift count */
)
{
    unsigned result;
    {
        result = dbl_shr(srcA, srcB, 32 - shift_cnt);
    }
    return result;
}


#endif /* __RTL_C__ */
