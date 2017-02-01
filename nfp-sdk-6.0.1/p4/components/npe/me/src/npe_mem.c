/*
 * Copyright (C) 2012-2015,  Netronome Systems, Inc.  All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file          src/npe_mem.c
 * @brief         Memory operations
 */

/* Compilation for Netronome NFP */
#if defined(__NFP_TOOL_NFCC)
    #ifndef NFP_LIB_ANY_NFCC_VERSION
        #if (__NFP_TOOL_NFCC < NFP_SW_VERSION(5, 0, 0, 0)) ||   \
            (__NFP_TOOL_NFCC > NFP_SW_VERSION(6, 255, 255, 255))
            #error "This software is not supported for the version of the SDK currently in use."
         #endif
    #endif
#endif

/* npe code */
#include <npe_types.h>
#include <npe_mem.h>

#ifndef __NFP_TOOL_NFCC
#include <string.h>
#ifndef __STDC_NO_ATOMICS__
#include <stdatomic.h>
#endif
#endif


/**
 * Write a value to memory (EMEM, IMEM, or CTM)
 * @param addr      Pointer to the value in memory
 * @param data      8-bit, 32-bit or 64-bit data value to write to memory
 */
__NPE_INTRINSIC void
npe_mem_write8(npe_mem_ptr_t addr, uint8_t data)
{
#ifdef __NFP_TOOL_NFCC
    uint32_t addr_hi, addr_lo;
    __declspec(write_reg) uint32_t xfr_reg;
    SIGNAL_PAIR sig;

    addr_hi = ((uint64_t)addr >> 8) & 0xff000000;
    addr_lo =  (uint64_t)addr & 0xffffffff;
    xfr_reg = data << 24;

    __asm { mem[write8, xfr_reg, addr_hi, <<8, addr_lo, 1], ctx_swap[sig] }
#else
    *addr = data;
#endif
}

__NPE_INTRINSIC void
npe_mem_write32(npe_mem_ptr_t addr, uint32_t data)
{
#ifdef __NFP_TOOL_NFCC
    uint32_t addr_hi, addr_lo;
    __declspec(write_reg) uint32_t xfr_reg;
    SIGNAL_PAIR sig;

    addr_hi = ((uint64_t)addr >> 8) & 0xff000000;
    addr_lo =  (uint64_t)addr & 0xffffffff;
    xfr_reg = data;

    __asm { mem[write32, xfr_reg, addr_hi, <<8, addr_lo, 1], ctx_swap[sig] }
#else
    *((uint32_t *)addr) = data;
#endif
}

__NPE_INTRINSIC void
npe_mem_write64(npe_mem_ptr_t addr, uint64_t data)
{
#ifdef __NFP_TOOL_NFCC
    uint32_t addr_hi, addr_lo;
    __declspec(write_reg) uint32_t xfr_reg[2];
    SIGNAL_PAIR sig;

    addr_hi = ((uint64_t)addr >> 8) & 0xff000000;
    addr_lo =  (uint64_t)addr & 0xffffffff;
    xfr_reg[0] = (uint32_t) data;
    xfr_reg[1] = (uint32_t) (data >> 32);

    __asm { mem[write, xfr_reg, addr_hi, <<8, addr_lo, 1], ctx_swap[sig] }
#else
    *((uint64_t *)addr) = data;
#endif
}

#define _MULTI_REG_OP( reg_op, s )                                  \
{                                                                   \
    switch ( s ) {                                                  \
                                                                    \
    case 64:                                                        \
        reg_op(0); reg_op(1); reg_op(2); reg_op(3); reg_op(4);      \
        reg_op(5); reg_op(6); reg_op(7); reg_op(8); reg_op(9);      \
        reg_op(10); reg_op(11); reg_op(12); reg_op(13); reg_op(14); \
        reg_op(15);                                                 \
        break;                                                      \
                                                                    \
    case 60:                                                        \
        reg_op(0); reg_op(1); reg_op(2); reg_op(3); reg_op(4);      \
        reg_op(5); reg_op(6); reg_op(7); reg_op(8); reg_op(9);      \
        reg_op(10); reg_op(11); reg_op(12); reg_op(13); reg_op(14); \
        break;                                                      \
                                                                    \
    case 56:                                                        \
        reg_op(0); reg_op(1); reg_op(2); reg_op(3); reg_op(4);      \
        reg_op(5); reg_op(6); reg_op(7); reg_op(8); reg_op(9);      \
        reg_op(10); reg_op(11); reg_op(12); reg_op(13);             \
        break;                                                      \
                                                                    \
    case 52:                                                        \
        reg_op(0); reg_op(1); reg_op(2); reg_op(3); reg_op(4);      \
        reg_op(5); reg_op(6); reg_op(7); reg_op(8); reg_op(9);      \
        reg_op(10); reg_op(11); reg_op(12);                         \
        break;                                                      \
                                                                    \
    case 48:                                                        \
        reg_op(0); reg_op(1); reg_op(2); reg_op(3); reg_op(4);      \
        reg_op(5); reg_op(6); reg_op(7); reg_op(8); reg_op(9);      \
        reg_op(10); reg_op(11);                                     \
        break;                                                      \
                                                                    \
    case 44:                                                        \
        reg_op(0); reg_op(1); reg_op(2); reg_op(3); reg_op(4);      \
        reg_op(5); reg_op(6); reg_op(7); reg_op(8); reg_op(9);      \
        reg_op(10);                                                 \
        break;                                                      \
                                                                    \
    case 40:                                                        \
        reg_op(0); reg_op(1); reg_op(2); reg_op(3); reg_op(4);      \
        reg_op(5); reg_op(6); reg_op(7); reg_op(8); reg_op(9);      \
        break;                                                      \
                                                                    \
    case 36:                                                        \
        reg_op(0); reg_op(1); reg_op(2); reg_op(3); reg_op(4);      \
        reg_op(5); reg_op(6); reg_op(7); reg_op(8);                 \
        break;                                                      \
                                                                    \
    case 32:                                                        \
        reg_op(0); reg_op(1); reg_op(2); reg_op(3); reg_op(4);      \
        reg_op(5); reg_op(6); reg_op(7);                            \
        break;                                                      \
                                                                    \
    case 28:                                                        \
        reg_op(0); reg_op(1); reg_op(2); reg_op(3); reg_op(4);      \
        reg_op(5); reg_op(6);                                       \
        break;                                                      \
                                                                    \
    case 24:                                                        \
       reg_op(0); reg_op(1); reg_op(2); reg_op(3); reg_op(4);       \
       reg_op(5);                                                   \
       break;                                                       \
                                                                    \
    case 20:                                                        \
        reg_op(0); reg_op(1); reg_op(2); reg_op(3); reg_op(4);      \
        break;                                                      \
                                                                    \
    case 16:                                                        \
        reg_op(0);  reg_op(1);  reg_op(2);  reg_op(3);              \
        break;                                                      \
                                                                    \
    case 12:                                                        \
        reg_op(0);  reg_op(1);  reg_op(2);                          \
        break;                                                      \
                                                                    \
    case 8:                                                         \
        reg_op(0);  reg_op(1);                                      \
        break;                                                      \
                                                                    \
    case 4:                                                         \
        reg_op(0);                                                  \
        break;                                                      \
                                                                    \
    default:                                                        \
        break;                                                      \
    }                                                               \
}

#define _MEM_CMD_32(cmdname, data, addr, size)                          \
do {                                                                    \
    uint32_t ind;                                                       \
    unsigned int count = size > 64 ? 64 >> 2 : (size >> 2);             \
    unsigned int addr_hi, addr_lo;                                      \
    SIGNAL_PAIR sig;                                                    \
    addr_hi = ((unsigned long long int)addr >> 8) & 0xff000000;         \
    addr_lo = (unsigned long long int)addr & 0xffffffff;                \
                                                                        \
    if (__is_ct_const(size)) {                                          \
        if (count <= 8) {                                               \
            __asm { mem[cmdname, *data, addr_hi, <<8, addr_lo,          \
                        count], ctx_swap[sig] }                         \
        } else {                                                        \
            ind = (count - 1) << 8 | 1 << 7;                            \
            __asm { alu[--, --, B, ind] }                               \
            __asm { mem[cmdname, *data, addr_hi, <<8, addr_lo,          \
                        count], ctx_swap[sig], indirect_ref }           \
        }                                                               \
    } else {                                                            \
        ind = (count - 1) << 8 | 1 << 7;                                \
        __asm { alu[--, --, B, ind] }                                   \
        __asm { mem[cmdname, *data, addr_hi, <<8, addr_lo,              \
                    count], ctx_swap[sig], indirect_ref }               \
    }                                                                   \
} while (0)

/**
 * Write from 1 to 16 32-bit words to memory (EMEM, IMEM, or CTM)
 *
 * @param addr      Pointer to the memory location to write
 * @param data      Pointer to data values to write to memory.
 *                  For NFCC this is NFP lm, gpr, nn_reg, xfr_reg
 * @param size      Number of bytes to write ( multiple of 4, max 64)
 */
__NPE_INTRINSIC void
npe_mem_write(npe_mem_ptr_t addr, void *data, size_t size)
{
#ifdef __NFP_TOOL_NFCC
    __declspec(write_reg) uint32_t xfr_reg[16];

    if (__is_in_lmem(data)) {
        #undef _REG_OP
        #define _REG_OP(_x) (xfr_reg[_x] = ((__lmem uint32_t *)data)[_x])
    } else {
        #undef _REG_OP
        #define _REG_OP(_x) (xfr_reg[_x] = ((__gpr uint32_t *)data)[_x])
    }

    _MULTI_REG_OP( _REG_OP, size);

    _MEM_CMD_32(write32, xfr_reg, addr, size);

#else

    int i;
    uint32_t *pa = (uint32_t *)addr;
    uint32_t *pd = (uint32_t *)data;
    for ( i = 0; i < size; i+=4 )
        *pa++ = *pd++;

#endif
}


/**
 * Read a value from memory (EMEM, IMEM, or CTM)
 * @param addr      Pointer to the value in memory
 */
__NPE_INTRINSIC uint8_t
npe_mem_read8(npe_mem_ptr_t addr)
{
#ifdef __NFP_TOOL_NFCC
    uint32_t addr_hi, addr_lo;
    __declspec(read_reg) uint32_t xfr_reg;
    SIGNAL_PAIR sig;

    addr_hi = ((uint64_t)addr >> 8) & 0xff000000;
    addr_lo =  (uint64_t)addr & 0xffffffff;

    __asm { mem[read8, xfr_reg, addr_hi, <<8, addr_lo, 1], ctx_swap[sig] }

    return ((uint8_t)(xfr_reg >> 24));
#else
    return *addr;
#endif
}

__NPE_INTRINSIC uint32_t
npe_mem_read32(npe_mem_ptr_t addr)
{
#ifdef __NFP_TOOL_NFCC
    uint32_t addr_hi, addr_lo;
    __declspec(read_reg) uint32_t xfr_reg;
    SIGNAL_PAIR sig;

    addr_hi = ((uint64_t)addr >> 8) & 0xff000000;
    addr_lo =  (uint64_t)addr & 0xffffffff;

    __asm { mem[read32, xfr_reg, addr_hi, <<8, addr_lo, 1], ctx_swap[sig] }

    return xfr_reg;
#else
    return *((uint32_t *)addr);
#endif
}

__NPE_INTRINSIC uint64_t
npe_mem_read64(npe_mem_ptr_t addr)
{
#ifdef __NFP_TOOL_NFCC
    uint32_t addr_hi, addr_lo;
    __declspec(read_reg) uint32_t xfr_reg[2];
    SIGNAL_PAIR sig;

    addr_hi = ((uint64_t)addr >> 8) & 0xff000000;
    addr_lo =  (uint64_t)addr & 0xffffffff;

    __asm { mem[read, xfr_reg, addr_hi, <<8, addr_lo, 1], ctx_swap[sig] }

    return ((((uint64_t)xfr_reg[1]) << 32) | xfr_reg[0]);
#else
    return *((uint64_t *)addr);
#endif
}

/**
 * Read from 1 to 16 32-bit words from memory (EMEM, IMEM, or CTM)
 *
 * @param addr      Pointer to the memory location to read
 * @param data      Pointer to location to store read data values.
 *                  For NFCC this is NFP lm, gpr, nn_reg, xfr_reg
 * @param size      Number of bytes to read ( multiple of 4, max 64)
 */
__NPE_INTRINSIC void
npe_mem_read(npe_mem_ptr_t addr, void *data, size_t size)
{
#ifdef __NFP_TOOL_NFCC
    __declspec(read_reg) uint32_t xfr_reg[16];

    if (__is_in_lmem(data)) {
        #undef _REG_OP
        #define _REG_OP(_x) (((__lmem uint32_t *)data)[_x] = xfr_reg[_x])
    } else {
        #undef _REG_OP
        #define _REG_OP(_x) (((__gpr uint32_t *)data)[_x] = xfr_reg[_x])
    }

    _MEM_CMD_32(read32, xfr_reg, addr, size);

    _MULTI_REG_OP( _REG_OP, size);

#else

    int i;
    uint32_t *pa = (uint32_t *)addr;
    uint32_t *pd = (uint32_t *)data;
    for ( i = 0; i < size; i+=4 )
        *pd++ = *pa++;

#endif
}

/**
 * Copy bytes from one memory location to another
 * @param to_addr   Pointer to the memory write start address
 * @param from_addr Pointer to the memory read start address
 * @param size      Number of bytes to copy
 */
__NPE_INTRINSIC void
npe_mem_copy(npe_mem_ptr_t to_addr,
             npe_mem_ptr_t from_addr,
             size_t size)
{
#ifdef __NFP_TOOL_NFCC
    uint32_t i;
    uint32_t addr_hi_from, addr_lo_from;
    uint32_t addr_hi_to, addr_lo_to;
    __declspec(read_reg)  uint64_t xfr_reg_rd64[8];
    __declspec(write_reg) uint64_t xfr_reg_wr64[8];
    __declspec(read_reg)  uint32_t xfr_reg_rd32;
    __declspec(write_reg) uint32_t xfr_reg_wr32;
    SIGNAL_PAIR sig;

    /* Do as many 64-bit xfers as we can */

    i = size >> 3; /* # of 64-bit xfers we can do */

    while (i > 0) {

        /* Try to do 8 64-bit xfers */

        if (i >= 8) {
            addr_hi_from = ((uint64_t)from_addr >> 8) & 0xff000000;
            addr_lo_from =  (uint64_t)from_addr;

            __asm { mem[read, xfr_reg_rd64, addr_hi_from, <<8, addr_lo_from, 8], \
                    ctx_swap[sig] }

            xfr_reg_wr64[0] = xfr_reg_rd64[0];
            xfr_reg_wr64[1] = xfr_reg_rd64[1];
            xfr_reg_wr64[2] = xfr_reg_rd64[2];
            xfr_reg_wr64[3] = xfr_reg_rd64[3];
            xfr_reg_wr64[4] = xfr_reg_rd64[4];
            xfr_reg_wr64[5] = xfr_reg_rd64[5];
            xfr_reg_wr64[6] = xfr_reg_rd64[6];
            xfr_reg_wr64[7] = xfr_reg_rd64[7];

            addr_hi_to = ((uint64_t)to_addr >> 8) & 0xff000000;
            addr_lo_to =  (uint64_t)to_addr;

            __asm { mem[write, xfr_reg_wr64, addr_hi_to, <<8, addr_lo_to, 8], \
                    ctx_swap[sig] }

            from_addr += 64;
            to_addr   += 64;

            i -= 8;
        }
        else if (i >= 4) {
            addr_hi_from = ((uint64_t)from_addr >> 8) & 0xff000000;
            addr_lo_from =  (uint64_t)from_addr;

            __asm { mem[read, xfr_reg_rd64, addr_hi_from, <<8, addr_lo_from, 4], \
                    ctx_swap[sig] }

            xfr_reg_wr64[0] = xfr_reg_rd64[0];
            xfr_reg_wr64[1] = xfr_reg_rd64[1];
            xfr_reg_wr64[2] = xfr_reg_rd64[2];
            xfr_reg_wr64[3] = xfr_reg_rd64[3];

            addr_hi_to = ((uint64_t)to_addr >> 8) & 0xff000000;
            addr_lo_to =  (uint64_t)to_addr;

            __asm { mem[write, xfr_reg_wr64, addr_hi_to, <<8, addr_lo_to, 4], \
                    ctx_swap[sig] }

            from_addr += 32;
            to_addr   += 32;

            i -= 4;
        }
        else if (i >= 2) {
            addr_hi_from = ((uint64_t)from_addr >> 8) & 0xff000000;
            addr_lo_from =  (uint64_t)from_addr;

            __asm { mem[read, xfr_reg_rd64, addr_hi_from, <<8, addr_lo_from, 2], \
                    ctx_swap[sig] }

            xfr_reg_wr64[0] = xfr_reg_rd64[0];
            xfr_reg_wr64[1] = xfr_reg_rd64[1];

            addr_hi_to = ((uint64_t)to_addr >> 8) & 0xff000000;
            addr_lo_to =  (uint64_t)to_addr;

            __asm { mem[write, xfr_reg_wr64, addr_hi_to, <<8, addr_lo_to, 2], \
                    ctx_swap[sig] }

            from_addr += 16;
            to_addr   += 16;

            i -= 2;
        }
        else if (i == 1) {
            addr_hi_from = ((uint64_t)from_addr >> 8) & 0xff000000;
            addr_lo_from =  (uint64_t)from_addr;

            __asm { mem[read, xfr_reg_rd64, addr_hi_from, <<8, addr_lo_from, 1], \
                    ctx_swap[sig] }

            xfr_reg_wr64[0] = xfr_reg_rd64[0];

            addr_hi_to = ((uint64_t)to_addr >> 8) & 0xff000000;
            addr_lo_to =  (uint64_t)to_addr;

            __asm { mem[write, xfr_reg_wr64, addr_hi_to, <<8, addr_lo_to, 1], \
                    ctx_swap[sig] }

            from_addr += 8;
            to_addr   += 8;

            i -= 1;
        }
    }

    /* Max bytes left = 7 */

    i = size & 0x7;

    if (i >= 4) {

        /* Can do 1 32-bit xfer */

        addr_hi_from = ((uint64_t)from_addr >> 8) & 0xff000000;
        addr_lo_from =  (uint64_t)from_addr;

        __asm { mem[read32, xfr_reg_rd32, addr_hi_from, <<8, addr_lo_from, 1], \
                ctx_swap[sig] }

        xfr_reg_wr32 = xfr_reg_rd32;

        addr_hi_to = ((uint64_t)to_addr >> 8) & 0xff000000;
        addr_lo_to =  (uint64_t)to_addr;

        __asm { mem[write32, xfr_reg_wr32, addr_hi_to, <<8, addr_lo_to, 1], \
                ctx_swap[sig] }

        from_addr += 4;
        to_addr   += 4;

        i -= 4;
    }

    /* Max bytes left = 3 */

    while (i > 0) {

        addr_hi_from = ((uint64_t)from_addr >> 8) & 0xff000000;
        addr_lo_from =  (uint64_t)from_addr;

        __asm { mem[read8, xfr_reg_rd32, addr_hi_from, <<8, addr_lo_from, 1], \
                ctx_swap[sig] }

        xfr_reg_wr32 = xfr_reg_rd32;

        addr_hi_to = ((uint64_t)to_addr >> 8) & 0xff000000;
        addr_lo_to =  (uint64_t)to_addr;

        __asm { mem[write8, xfr_reg_wr32, addr_hi_to, <<8, addr_lo_to, 1], \
                ctx_swap[sig] }

        from_addr += 1;
        to_addr   += 1;

        i -= 1;
    }
#else
    memmove((void *)to_addr, (const void *)from_addr, size);
#endif
}


#ifndef __STDC_NO_ATOMICS__


/**
 * Atomic read/write a value from/to memory (EMEM, IMEM, or CTM)
 * @param addr      Pointer to the value in memory
 * @param data      32-bit or 64-bit data value to write
 */
__NPE_INTRINSIC void
npe_mem_write_atomic32(npe_atomic_mem_ptr_t addr, uint32_t data)
{
#ifdef __NFP_TOOL_NFCC
    uint32_t addr_hi, addr_lo;
    __declspec(write_reg) uint32_t xfr_reg;
    SIGNAL_PAIR sig;

    addr_hi = ((uint64_t)addr >> 8) & 0xff000000;
    addr_lo =  (uint64_t)addr & 0xffffffff;
    xfr_reg = data;

    __asm { mem[atomic_write, xfr_reg, addr_hi, <<8, addr_lo, 1], ctx_swap[sig] }
#else
    __atomic_store_n((_Atomic uint32_t *)addr, data, __ATOMIC_SEQ_CST);
#endif
}

__NPE_INTRINSIC void
npe_mem_write_atomic64(npe_atomic_mem_ptr_t addr, uint64_t data)
{
#ifdef __NFP_TOOL_NFCC
    uint32_t addr_hi, addr_lo;
    __declspec(write_reg) uint32_t xfr_reg[2];
    SIGNAL_PAIR sig;

    addr_hi = ((uint64_t)addr >> 8) & 0xff000000;
    addr_lo =  (uint64_t)addr & 0xffffffff;
    xfr_reg[0] = (uint32_t) data;
    xfr_reg[1] = (uint32_t) (data >> 32);

    __asm { mem[atomic_write, xfr_reg, addr_hi, <<8, addr_lo, 2], ctx_swap[sig] }
#else
    __atomic_store_n((_Atomic uint64_t *)addr, data, __ATOMIC_SEQ_CST);
#endif
}

__NPE_INTRINSIC uint32_t
npe_mem_read_atomic32(npe_atomic_mem_ptr_t addr)
{
#ifdef __NFP_TOOL_NFCC
    uint32_t addr_hi, addr_lo;
    __declspec(read_reg) uint32_t xfr_reg;
    SIGNAL_PAIR sig;

    addr_hi = ((uint64_t)addr >> 8) & 0xff000000;
    addr_lo =  (uint64_t)addr & 0xffffffff;

    __asm { mem[atomic_read, xfr_reg, addr_hi, <<8, addr_lo, 1], ctx_swap[sig] }

    return xfr_reg;
#else
    return __atomic_load_n((_Atomic uint32_t *)addr, __ATOMIC_SEQ_CST);
#endif
}

__NPE_INTRINSIC uint64_t
npe_mem_read_atomic64(npe_atomic_mem_ptr_t addr)
{
#ifdef __NFP_TOOL_NFCC
    uint32_t addr_hi, addr_lo;
    __declspec(read_reg) uint32_t xfr_reg[2];
    SIGNAL_PAIR sig;

    addr_hi = ((uint64_t)addr >> 8) & 0xff000000;
    addr_lo =  (uint64_t)addr & 0xffffffff;

    __asm { mem[atomic_read, xfr_reg, addr_hi, <<8, addr_lo, 2], ctx_swap[sig] }

    return ((((uint64_t)xfr_reg[1]) << 32) | xfr_reg[0]);
#else
    return __atomic_load_n((_Atomic uint64_t *)addr, __ATOMIC_SEQ_CST);
#endif
}

/**
 * Atomic add/subtract/xor to/from/with a value in memory (EMEM, IMEM, or CTM)
 * @param addr      Pointer to the value in memory
 * @param data      8-bit, 32-bit or 64-bit data value to add/subtract
 *
 * These functions add to, subtract from or xor with a
 * single 32 bit or 64 bit word in NFP memory.
 */
__NPE_INTRINSIC void
npe_mem_add32(npe_atomic_mem_ptr_t addr, int32_t data)
{
#ifdef __NFP_TOOL_NFCC
    uint32_t addr_hi, addr_lo;
    __declspec(write_reg) uint32_t xfr_reg;
    SIGNAL_PAIR sig;

    addr_hi = ((uint64_t)addr >> 8) & 0xff000000;
    addr_lo =  (uint64_t)addr & 0xffffffff;
    xfr_reg = data;

    __asm { mem[add, xfr_reg, addr_hi, <<8, addr_lo, 1], ctx_swap[sig] }
#else
    __atomic_add_fetch((_Atomic uint32_t *)addr, data, __ATOMIC_SEQ_CST);
#endif
}

__NPE_INTRINSIC void
npe_mem_addsat32(npe_atomic_mem_ptr_t addr, int32_t data)
{
#ifdef __NFP_TOOL_NFCC
    uint32_t addr_hi, addr_lo;
    __declspec(write_reg) uint32_t xfr_reg;
    SIGNAL_PAIR sig;

    addr_hi = ((uint64_t)addr >> 8) & 0xff000000;
    addr_lo =  (uint64_t)addr & 0xffffffff;
    xfr_reg = data;

    __asm { mem[addsat, xfr_reg, addr_hi, <<8, addr_lo, 1], ctx_swap[sig] }
#else
    /* Memory contents are always treated as unsigned,
     * but for saturating adds, write_data is signed
     * memory = ((write_data < 0) && ((read_data + write_data) < 0)) ?
     *                0 : ((read_data + write_data) > 32hffffffff) ?
     *                   32hffffffff : (read_data + write_data)
     */

    __transaction_atomic {
        uint32_t temp;
        int64_t temp64;

        temp = *((uint32_t *)addr);

        temp64 = (uint64_t)temp + (int64_t)data;

        temp = ((data < 0) && (temp64 < 0)) ?
                    0 : (temp64 > 0xffffffffULL) ?
                       0xffffffff : (temp + data);

        *((uint32_t *)addr) = temp;
    }
#endif
}

__NPE_INTRINSIC void
npe_mem_add64(npe_atomic_mem_ptr_t addr, int64_t data)
{
#ifdef __NFP_TOOL_NFCC
    uint32_t addr_hi, addr_lo;
    __declspec(write_reg) uint32_t xfr_reg[2];
    SIGNAL_PAIR sig;

    addr_hi = ((uint64_t)addr >> 8) & 0xff000000;
    addr_lo =  (uint64_t)addr & 0xffffffff;
    xfr_reg[0] = (uint32_t) data;
    xfr_reg[1] = (uint32_t) (data >> 32);

    __asm { mem[add64, xfr_reg, addr_hi, <<8, addr_lo, 1], ctx_swap[sig] }
#else
    __atomic_add_fetch((_Atomic uint64_t *)addr, data, __ATOMIC_SEQ_CST);
#endif
}

__NPE_INTRINSIC void
npe_mem_addsat64(npe_atomic_mem_ptr_t addr, int64_t data)
{
#ifdef __NFP_TOOL_NFCC
    uint32_t addr_hi, addr_lo;
    __declspec(write_reg) uint32_t xfr_reg[2];
    SIGNAL_PAIR sig;

    addr_hi = ((uint64_t)addr >> 8) & 0xff000000;
    addr_lo =  (uint64_t)addr & 0xffffffff;
    xfr_reg[0] = (uint32_t) data;
    xfr_reg[1] = (uint32_t) (data >> 32);

    __asm { mem[addsat64, xfr_reg, addr_hi, <<8, addr_lo, 1], ctx_swap[sig] }
#else
    /* Memory contents are always treated as unsigned,
     * but for saturating adds, write_data is signed
     * memory = ((write_data < 0) && ((read_data + write_data) < 0)) ?
     *                0 : ((read_data + write_data) > 64hffffffff) ?
     *                   64hffffffff : (read_data + write_data)
     */

    __transaction_atomic {
        uint64_t temp, temp64_lo;
        int64_t  temp64_hi;

        temp = *((uint64_t *)addr);

        temp64_lo = temp + (int64_t)data;
        temp64_hi = (int64_t)data;

        if (temp64_hi & 0x8000000000000000ULL)
            temp64_hi = 0xffffffffffffffffULL;
        else
            temp64_hi = 0;

        if (temp64_lo < temp)
            ++temp64_hi;

        temp = ((data < 0) && (temp64_hi < 0)) ? 0 : (temp64_hi) ?
                       0xffffffffffffffffULL : (temp + data);

        *((uint64_t *)addr) = temp;
    }
#endif
}

__NPE_INTRINSIC void
npe_mem_sub32(npe_atomic_mem_ptr_t addr, int32_t data)
{
#ifdef __NFP_TOOL_NFCC
    uint32_t addr_hi, addr_lo;
    __declspec(write_reg) uint32_t xfr_reg;
    SIGNAL_PAIR sig;

    addr_hi = ((uint64_t)addr >> 8) & 0xff000000;
    addr_lo =  (uint64_t)addr & 0xffffffff;
    xfr_reg = data;

    __asm { mem[sub, xfr_reg, addr_hi, <<8, addr_lo, 1], ctx_swap[sig] }
#else
    __atomic_sub_fetch((_Atomic uint32_t *)addr, data, __ATOMIC_SEQ_CST);
#endif
}

__NPE_INTRINSIC void
npe_mem_subsat32(npe_atomic_mem_ptr_t addr, uint32_t data)
{
#ifdef __NFP_TOOL_NFCC
    uint32_t addr_hi, addr_lo;
    __declspec(write_reg) uint32_t xfr_reg;
    SIGNAL_PAIR sig;

    addr_hi = ((uint64_t)addr >> 8) & 0xff000000;
    addr_lo =  (uint64_t)addr & 0xffffffff;
    xfr_reg = data;

    __asm { mem[subsat, xfr_reg, addr_hi, <<8, addr_lo, 1], ctx_swap[sig] }
#else
    /* Memory contents are always treated as unsigned,
     * and for saturating subtract the write_data is also unsigned
     * memory = (read_data < write_data) ? 0 : read_data - write_data
     */

    __transaction_atomic {
        uint32_t temp;

        temp = *((uint32_t *)addr);
        temp = (temp < data) ? 0 : temp - data;
        *((uint32_t *)addr) = temp;
    }
#endif
}

__NPE_INTRINSIC void
npe_mem_sub64(npe_atomic_mem_ptr_t addr, int64_t data)
{
#ifdef __NFP_TOOL_NFCC
    uint32_t addr_hi, addr_lo;
    __declspec(write_reg) uint32_t xfr_reg[2];
    SIGNAL_PAIR sig;

    addr_hi = ((uint64_t)addr >> 8) & 0xff000000;
    addr_lo =  (uint64_t)addr & 0xffffffff;
    xfr_reg[0] = (uint32_t) data;
    xfr_reg[1] = (uint32_t) (data >> 32);

    __asm { mem[sub64, xfr_reg, addr_hi, <<8, addr_lo, 1], ctx_swap[sig] }
#else
    __atomic_sub_fetch((_Atomic uint64_t *)addr, data, __ATOMIC_SEQ_CST);
#endif
}

__NPE_INTRINSIC void
npe_mem_subsat64(npe_atomic_mem_ptr_t addr, uint64_t data)
{
#ifdef __NFP_TOOL_NFCC
    uint32_t addr_hi, addr_lo;
    __declspec(write_reg) uint32_t xfr_reg[2];
    SIGNAL_PAIR sig;

    addr_hi = ((uint64_t)addr >> 8) & 0xff000000;
    addr_lo =  (uint64_t)addr & 0xffffffff;
    xfr_reg[0] = (uint32_t) data;
    xfr_reg[1] = (uint32_t) (data >> 32);

    __asm { mem[subsat64, xfr_reg, addr_hi, <<8, addr_lo, 1], ctx_swap[sig] }
#else
    /* Memory contents are always treated as unsigned,
     * and for saturating subtract the write_data is also unsigned
     * memory = (read_data < write_data) ? 0 : read_data - write_data
     */

    __transaction_atomic {
        uint64_t temp;

        temp = *((uint64_t *)addr);
        temp = (temp < data) ? 0 : temp - data;
        *((uint64_t *)addr) = temp;
    }
#endif
}

__NPE_INTRINSIC void
npe_mem_xor32(npe_atomic_mem_ptr_t addr, uint32_t data)
{
#ifdef __NFP_TOOL_NFCC
    uint32_t addr_hi, addr_lo;
    __declspec(write_reg) uint32_t xfr_reg;
    SIGNAL_PAIR sig;

    addr_hi = ((uint64_t)addr >> 8) & 0xff000000;
    addr_lo =  (uint64_t)addr & 0xffffffff;
    xfr_reg = data;

    __asm { mem[xor, xfr_reg, addr_hi, <<8, addr_lo, 1], ctx_swap[sig] }
#else
    __atomic_xor_fetch((_Atomic uint32_t *)addr, data, __ATOMIC_SEQ_CST);
#endif
}

/**
 * Atomic set/clear bits in a value in memory (EMEM, IMEM, or CTM)
 * @param addr      Pointer to the value in memory
 * @param data      32-bit value that indicates which bits should be modified
 *
 * When the data bit is a 1 the bit in the memory word will be modified.
 * When the data bit is a 0 the bit in the memory word will not be modified.
 */
__NPE_INTRINSIC void
npe_mem_bit_clr32(npe_atomic_mem_ptr_t addr, uint32_t data)
{
#ifdef __NFP_TOOL_NFCC
    uint32_t addr_hi, addr_lo;
    __declspec(write_reg) uint32_t xfr_reg;
    SIGNAL_PAIR sig;

    addr_hi = ((uint64_t)addr >> 8) & 0xff000000;
    addr_lo =  (uint64_t)addr & 0xffffffff;
    xfr_reg = data;

    __asm { mem[clr, xfr_reg, addr_hi, <<8, addr_lo, 1], ctx_swap[sig] }
#else
    __atomic_and_fetch((_Atomic uint32_t *)addr, ~data, __ATOMIC_SEQ_CST);
#endif
}

__NPE_INTRINSIC void
npe_mem_bit_set32(npe_atomic_mem_ptr_t addr, uint32_t data)
{
#ifdef __NFP_TOOL_NFCC
    uint32_t addr_hi, addr_lo;
    __declspec(write_reg) uint32_t xfr_reg;
    SIGNAL_PAIR sig;

    addr_hi = ((uint64_t)addr >> 8) & 0xff000000;
    addr_lo =  (uint64_t)addr & 0xffffffff;
    xfr_reg = data;

    __asm { mem[set, xfr_reg, addr_hi, <<8, addr_lo, 1], ctx_swap[sig] }
#else
    __atomic_or_fetch((_Atomic uint32_t *)addr, data, __ATOMIC_SEQ_CST);
#endif
}

/**
 * Atomic increment/decrement of a value in memory (EMEM, IMEM, or CTM)
 * @param addr      Pointer to the value in memory
 *
 * These functions increment or decrement a single 32 bit or 64 bit word in NFP
 * memory.
 */
__NPE_INTRINSIC void
npe_mem_decr32(npe_atomic_mem_ptr_t addr)
{
#ifdef __NFP_TOOL_NFCC
    uint32_t addr_hi, addr_lo;

    addr_hi = ((uint64_t)addr >> 8) & 0xff000000;
    addr_lo =  (uint64_t)addr & 0xffffffff;

    __asm { mem[decr, --, addr_hi, <<8, addr_lo] }
#else
    __atomic_sub_fetch((_Atomic uint32_t *)addr, 1UL, __ATOMIC_SEQ_CST);
#endif
}

__NPE_INTRINSIC void
npe_mem_decr64(npe_atomic_mem_ptr_t addr)
{
#ifdef __NFP_TOOL_NFCC
    uint32_t addr_hi, addr_lo;

    addr_hi = ((uint64_t)addr >> 8) & 0xff000000;
    addr_lo =  (uint64_t)addr & 0xffffffff;

    __asm { mem[decr64, --, addr_hi, <<8, addr_lo] }
#else
    __atomic_sub_fetch((_Atomic uint64_t *)addr, 1ULL, __ATOMIC_SEQ_CST);
#endif
}

__NPE_INTRINSIC void
npe_mem_incr32(npe_atomic_mem_ptr_t addr)
{
#ifdef __NFP_TOOL_NFCC
    uint32_t addr_hi, addr_lo;

    addr_hi = ((uint64_t)addr >> 8) & 0xff000000;
    addr_lo =  (uint64_t)addr & 0xffffffff;

    __asm { mem[incr, --, addr_hi, <<8, addr_lo] }
#else
    __atomic_add_fetch((_Atomic uint32_t *)addr, 1UL, __ATOMIC_SEQ_CST);
#endif
}

__NPE_INTRINSIC void
npe_mem_incr64(npe_atomic_mem_ptr_t addr)
{
#ifdef __NFP_TOOL_NFCC
    uint32_t addr_hi, addr_lo;

    addr_hi = ((uint64_t)addr >> 8) & 0xff000000;
    addr_lo =  (uint64_t)addr & 0xffffffff;

    __asm { mem[incr64, --, addr_hi, <<8, addr_lo] }
#else
    __atomic_add_fetch((_Atomic uint64_t *)addr, 1UL, __ATOMIC_SEQ_CST);
#endif
}

/**
 * Atomic test and add/subtract/xor to/from/with a value in memory
 * (EMEM, IMEM, or CTM)
 * @param addr      Pointer to the value in memory
 * @param data      8-bit, 32-bit or 64-bit data value to add/subtract/xor
 * @return          32-bit or 64-bit unmodified value read from memory
 *
 * These functions add to, subtract from, or xor with,
 * a single 32 bit or 64 bit word in NFP memory.
 */
__NPE_INTRINSIC uint32_t
npe_mem_test_and_add32(npe_atomic_mem_ptr_t addr, int32_t data)
{
#ifdef __NFP_TOOL_NFCC
    uint32_t addr_hi, addr_lo;
    __declspec(read_write_reg) uint32_t xfr_reg;
    SIGNAL_PAIR sig;

    addr_hi = ((uint64_t)addr >> 8) & 0xff000000;
    addr_lo =  (uint64_t)addr & 0xffffffff;
    xfr_reg = data;

    __asm { mem[test_add, xfr_reg, addr_hi, <<8, addr_lo, 1], sig_done[sig] }
    __asm { ctx_arb[sig] }

    return xfr_reg;
#else
    return __atomic_fetch_add((_Atomic uint32_t *)addr, data, __ATOMIC_SEQ_CST);
#endif
}

__NPE_INTRINSIC uint32_t
npe_mem_test_and_addsat32(npe_atomic_mem_ptr_t addr, int32_t data)
{
#ifdef __NFP_TOOL_NFCC
    uint32_t addr_hi, addr_lo;
    __declspec(read_write_reg) uint32_t xfr_reg;
    SIGNAL_PAIR sig;

    addr_hi = ((uint64_t)addr >> 8) & 0xff000000;
    addr_lo =  (uint64_t)addr & 0xffffffff;
    xfr_reg = data;

    __asm { mem[test_addsat, xfr_reg, addr_hi, <<8, addr_lo, 1], sig_done[sig] }
    __asm { ctx_arb[sig] }

    return xfr_reg;
#else
    /* Memory contents are always treated as unsigned,
     * but for saturating adds, write_data is signed
     * memory = ((write_data < 0) && ((read_data + write_data) < 0)) ?
     *                0 : ((read_data + write_data) > 32hffffffff) ?
     *                   32hffffffff : (read_data + write_data)
     */

    uint32_t temp;

    __transaction_atomic {
        uint32_t temp1;
        int64_t temp64;

        temp = *((uint32_t *)addr);

        temp64 = (uint64_t)temp + (int64_t)data;

        temp1 = ((data < 0) && (temp64 < 0)) ?
                    0 : (temp64 > 0xffffffffULL) ?
                       0xffffffff : (temp + data);

        *((uint32_t *)addr) = temp1;
    }

    return temp;
#endif
}

__NPE_INTRINSIC uint64_t
npe_mem_test_and_add64(npe_atomic_mem_ptr_t addr, int64_t data)
{
#ifdef __NFP_TOOL_NFCC
    uint32_t addr_hi, addr_lo;
    __declspec(read_write_reg) uint32_t xfr_reg[2];
    SIGNAL_PAIR sig;

    addr_hi = ((uint64_t)addr >> 8) & 0xff000000;
    addr_lo =  (uint64_t)addr & 0xffffffff;
    xfr_reg[0] = (uint32_t) data;
    xfr_reg[1] = (uint32_t) (data >> 32);

    __asm { mem[test_add64, xfr_reg, addr_hi, <<8, addr_lo, 1], sig_done[sig] }
    __asm { ctx_arb[sig] }

    return ((((uint64_t)xfr_reg[1]) << 32) | xfr_reg[0]);
#else
    return __atomic_fetch_add((_Atomic uint64_t *)addr, data, __ATOMIC_SEQ_CST);
#endif
}

__NPE_INTRINSIC uint64_t
npe_mem_test_and_addsat64(npe_atomic_mem_ptr_t addr, int64_t data)
{
#ifdef __NFP_TOOL_NFCC
    uint32_t addr_hi, addr_lo;
    __declspec(read_write_reg) uint32_t xfr_reg[2];
    SIGNAL_PAIR sig;

    addr_hi = ((uint64_t)addr >> 8) & 0xff000000;
    addr_lo =  (uint64_t)addr & 0xffffffff;
    xfr_reg[0] = (uint32_t) data;
    xfr_reg[1] = (uint32_t) (data >> 32);

    __asm { mem[test_addsat64, xfr_reg, addr_hi, <<8, addr_lo, 1], sig_done[sig] }
    __asm { ctx_arb[sig] }

    return ((((uint64_t)xfr_reg[1]) << 32) | xfr_reg[0]);
#else
    /* Memory contents are always treated as unsigned,
     * but for saturating adds, write_data is signed
     * memory = ((write_data < 0) && ((read_data + write_data) < 0)) ?
     *                0 : ((read_data + write_data) > 64hffffffff) ?
     *                   64hffffffff : (read_data + write_data)
     */

    uint64_t temp;

    __transaction_atomic {
        uint64_t temp1, temp64_lo;
        int64_t  temp64_hi;

        temp = *((uint64_t *)addr);

        temp64_lo = temp + (int64_t)data;
        temp64_hi = (int64_t)data;

        if (temp64_hi & 0x8000000000000000ULL)
            temp64_hi = 0xffffffffffffffffULL;
        else
            temp64_hi = 0;

        if (temp64_lo < temp)
            ++temp64_hi;

        temp1 = ((data < 0) && (temp64_hi < 0)) ? 0 : (temp64_hi) ?
                       0xffffffffffffffffULL : (temp + data);

        *((uint64_t *)addr) = temp1;
    }

    return temp;
#endif
}

__NPE_INTRINSIC uint32_t
npe_mem_test_and_sub32(npe_atomic_mem_ptr_t addr, int32_t data)
{
#ifdef __NFP_TOOL_NFCC
    uint32_t addr_hi, addr_lo;
    __declspec(read_write_reg) uint32_t xfr_reg;
    SIGNAL_PAIR sig;

    addr_hi = ((uint64_t)addr >> 8) & 0xff000000;
    addr_lo =  (uint64_t)addr & 0xffffffff;
    xfr_reg = data;

    __asm { mem[test_sub, xfr_reg, addr_hi, <<8, addr_lo, 1], sig_done[sig] }
    __asm { ctx_arb[sig] }

    return xfr_reg;
#else
    return __atomic_fetch_sub((_Atomic uint32_t *)addr, data, __ATOMIC_SEQ_CST);
#endif
}

__NPE_INTRINSIC uint32_t
npe_mem_test_and_subsat32(npe_atomic_mem_ptr_t addr, uint32_t data)
{
#ifdef __NFP_TOOL_NFCC
    uint32_t addr_hi, addr_lo;
    __declspec(read_write_reg) uint32_t xfr_reg;
    SIGNAL_PAIR sig;

    addr_hi = ((uint64_t)addr >> 8) & 0xff000000;
    addr_lo =  (uint64_t)addr & 0xffffffff;
    xfr_reg = data;

    __asm { mem[test_subsat, xfr_reg, addr_hi, <<8, addr_lo, 1], sig_done[sig] }
    __asm { ctx_arb[sig] }

    return xfr_reg;
#else
    /* Memory contents are always treated as unsigned,
     * and for saturating subtract the write_data is also unsigned
     * memory = (read_data < write_data) ? 0 : read_data - write_data
     */

    uint32_t temp;

    __transaction_atomic {
        uint32_t temp1;

        temp = *((uint32_t *)addr);
        temp1 = (temp < data) ? 0 : temp - data;
        *((uint32_t *)addr) = temp1;
    }

    return temp;
#endif
}

__NPE_INTRINSIC uint64_t
npe_mem_test_and_sub64(npe_atomic_mem_ptr_t addr, int64_t data)
{
#ifdef __NFP_TOOL_NFCC
    uint32_t addr_hi, addr_lo;
    __declspec(read_write_reg) uint32_t xfr_reg[2];
    SIGNAL_PAIR sig;

    addr_hi = ((uint64_t)addr >> 8) & 0xff000000;
    addr_lo =  (uint64_t)addr & 0xffffffff;
    xfr_reg[0] = (uint32_t) data;
    xfr_reg[1] = (uint32_t) (data >> 32);

    __asm { mem[test_sub64, xfr_reg, addr_hi, <<8, addr_lo, 1], sig_done[sig] }
    __asm { ctx_arb[sig] }

    return ((((uint64_t)xfr_reg[1]) << 32) | xfr_reg[0]);
#else
    return __atomic_fetch_sub((_Atomic uint64_t *)addr, data, __ATOMIC_SEQ_CST);
#endif
}

__NPE_INTRINSIC uint64_t
npe_mem_test_and_subsat64(npe_atomic_mem_ptr_t addr, uint64_t data)
{
#ifdef __NFP_TOOL_NFCC
    uint32_t addr_hi, addr_lo;
    __declspec(read_write_reg) uint32_t xfr_reg[2];
    SIGNAL_PAIR sig;

    addr_hi = ((uint64_t)addr >> 8) & 0xff000000;
    addr_lo =  (uint64_t)addr & 0xffffffff;
    xfr_reg[0] = (uint32_t) data;
    xfr_reg[1] = (uint32_t) (data >> 32);

    __asm { mem[test_subsat64, xfr_reg, addr_hi, <<8, addr_lo, 1], sig_done[sig] }
    __asm { ctx_arb[sig] }

    return ((((uint64_t)xfr_reg[1]) << 32) | xfr_reg[0]);
#else
    /* Memory contents are always treated as unsigned,
     * and for saturating subtract the write_data is also unsigned
     * memory = (read_data < write_data) ? 0 : read_data - write_data
     */

    uint64_t temp;

    __transaction_atomic {
        uint64_t temp1;

        temp = *((uint64_t *)addr);
        temp1 = (temp < data) ? 0 : temp - data;
        *((uint64_t *)addr) = temp1;
    }

    return temp;
#endif
}

__NPE_INTRINSIC uint32_t
npe_mem_test_and_xor32(npe_atomic_mem_ptr_t addr, uint32_t data)
{
#ifdef __NFP_TOOL_NFCC
    uint32_t addr_hi, addr_lo;
    __declspec(read_write_reg) uint32_t xfr_reg;
    SIGNAL_PAIR sig;

    addr_hi = ((uint64_t)addr >> 8) & 0xff000000;
    addr_lo =  (uint64_t)addr & 0xffffffff;
    xfr_reg = data;

    __asm { mem[test_xor, xfr_reg, addr_hi, <<8, addr_lo, 1], sig_done[sig] }
    __asm { ctx_arb[sig] }

    return xfr_reg;
#else
    return __atomic_fetch_xor((_Atomic uint32_t *)addr, data, __ATOMIC_SEQ_CST);
#endif
}

/**
 * Atomic test and set/clear bits in a value in memory (EMEM, IMEM, or CTM)
 */
__NPE_INTRINSIC uint32_t
npe_mem_test_and_bit_clr32(npe_atomic_mem_ptr_t addr, uint32_t data)
{
#ifdef __NFP_TOOL_NFCC
    uint32_t addr_hi, addr_lo;
    __declspec(read_write_reg) uint32_t xfr_reg;
    SIGNAL_PAIR sig;

    addr_hi = ((uint64_t)addr >> 8) & 0xff000000;
    addr_lo =  (uint64_t)addr & 0xffffffff;
    xfr_reg = data;

    __asm { mem[test_clr, xfr_reg, addr_hi, <<8, addr_lo, 1], sig_done[sig] }
    __asm { ctx_arb[sig] }

    return xfr_reg;
#else
    return __atomic_fetch_and((_Atomic uint32_t *)addr, ~data, __ATOMIC_SEQ_CST);
#endif
}

__NPE_INTRINSIC uint32_t
npe_mem_test_and_bit_set32(npe_atomic_mem_ptr_t addr, uint32_t data)
{
#ifdef __NFP_TOOL_NFCC
    uint32_t addr_hi, addr_lo;
    __declspec(read_write_reg) uint32_t xfr_reg;
    SIGNAL_PAIR sig;

    addr_hi = ((uint64_t)addr >> 8) & 0xff000000;
    addr_lo =  (uint64_t)addr & 0xffffffff;
    xfr_reg = data;

    __asm { mem[test_set, xfr_reg, addr_hi, <<8, addr_lo, 1], sig_done[sig] }
    __asm { ctx_arb[sig] }

    return xfr_reg;
#else
    return __atomic_fetch_or((_Atomic uint32_t *)addr, data, __ATOMIC_SEQ_CST);
#endif
}


#endif
