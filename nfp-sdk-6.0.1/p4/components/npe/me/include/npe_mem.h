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
 * @file          include/npe_mem.h
 * @brief         Memory operations
 */

#ifndef __NPE_MEM_H_
#define __NPE_MEM_H_

/* Compilation for Netronome NFP */
#if defined(__NFP_TOOL_NFCC)

    #ifndef NFP_LIB_ANY_NFCC_VERSION
        #if (__NFP_TOOL_NFCC < NFP_SW_VERSION(5, 0, 0, 0)) ||   \
            (__NFP_TOOL_NFCC > NFP_SW_VERSION(6, 255, 255, 255))
            #error "This software is not supported for the version of the SDK currently in use."
         #endif
    #endif

#include <nfp.h>
#include <stdint.h>

#else /* defined __NFP_TOOL_NFCC */

#include <stdint.h>

#endif

#include <npe_types.h>

/** @file npe_mem.h
 * @addtogroup mem  Memory operations
 * @{
 */

/** @name Memory related functions
 * @{
 */


/**
 * Write an 8-bit value to memory (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_MEM8(address)
 * unsigned char data = 0x12;
 * npe_mem_write8(NPE_GET_MEM_ADDR_PTR(address), data);
 * @endcode
 *
 * @param addr      Pointer to the memory location to write
 * @param data      8-bit data value to write to memory
 */
__NPE_INTRINSIC void
npe_mem_write8(npe_mem_ptr_t addr, uint8_t data);

/**
 * Write a 32-bit value to memory (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_MEM32(address)
 * unsigned long data = 0x12345678;
 * npe_mem_write32(NPE_GET_MEM_ADDR_PTR(address), data);
 * @endcode
 *
 * @param addr      Pointer to the memory location to write
 * @param data      32-bit data value to write to memory
 */
__NPE_INTRINSIC void
npe_mem_write32(npe_mem_ptr_t addr, uint32_t data);

/**
 * Write a 64-bit value to memory (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_MEM64(address)
 * unsigned long long data = 0x123456789abcdef0ULL;
 * npe_mem_write64(NPE_GET_MEM_ADDR_PTR(address), data);
 * @endcode
 *
 * @param addr      Pointer to the memory location to write
 * @param data      64-bit data value to write to memory
 */
__NPE_INTRINSIC void
npe_mem_write64(npe_mem_ptr_t addr, uint64_t data);

/**
 * Write from 1 to 16 32-bit words to memory (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_MEM32_ARRAY(address,8);
 * uint32_t val[8];
 * val[0] = 0x00;
 * val[1] = 0x04;
 * val[2] = 0x08;
 * val[3] = 0x0C;
 * val[4] = 0x10;
 * val[5] = 0x40;
 * val[6] = 0x80;
 * val[7] = 0xC0;
 * npe_mem_write(NPE_GET_MEM_ADDR_PTR(address), val, sizeof(val));
 * @endcode
 *
 * @param addr      Pointer to the memory location to write
 * @param data      Pointer to data values to write to memory.
 *                  For NFCC this is NFP lm, gpr, nn_reg, xfr_reg
 * @param size      Number of bytes to write ( multiple of 4, max 64)
 */
__NPE_INTRINSIC void
npe_mem_write(npe_mem_ptr_t addr, void *data, size_t size);


/**
 * Read an 8-bit value from memory (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_MEM8(address)
 * unsigned char data;
 * data = npe_mem_read8(NPE_GET_MEM_ADDR_PTR(address));
 * @endcode
 *
 * @param addr      Pointer to the value in memory
 */
__NPE_INTRINSIC uint8_t
npe_mem_read8(npe_mem_ptr_t addr);

/**
 * Read a 32-bit value from memory (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_MEM32(address)
 * unsigned long data;
 * data = npe_mem_read32(NPE_GET_MEM_ADDR_PTR(address));
 * @endcode
 *
 * @param addr      Pointer to the value in memory
 */
__NPE_INTRINSIC uint32_t
npe_mem_read32(npe_mem_ptr_t addr);

/**
 * Read a 64-bit value from memory (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_MEM64(address)
 * unsigned long long data;
 * data = npe_mem_read64(NPE_GET_MEM_ADDR_PTR(address));
 * @endcode
 *
 * @param addr      Pointer to the value in memory
 */
__NPE_INTRINSIC uint64_t
npe_mem_read64(npe_mem_ptr_t addr);

/**
 * Read from 1 to 16 32-bit words from memory (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_MEM32_ARRAY(address,8);
 * uint32_t val[8];
 * val[0] = 0x00;
 * val[1] = 0x04;
 * val[2] = 0x08;
 * val[3] = 0x0C;
 * val[4] = 0x10;
 * val[5] = 0x40;
 * val[6] = 0x80;
 * val[7] = 0xC0;
 * npe_mem_write(NPE_GET_MEM_ADDR_PTR(address), val, sizeof(val));
 * uint32_t valr[8];
 * npe_mem_read(NPE_GET_MEM_ADDR_PTR(address), valr, sizeof(valr));
 * @endcode
 *
 * @param addr      Pointer to the memory location to write
 * @param data      Pointer to location to store values read from memory
 *                  For NFCC this is NFP lm, gpr, nn_reg, xfr_reg
 * @param size      Number of bytes to write ( multiple of 4, max 64)
 */
__NPE_INTRINSIC void
npe_mem_read(npe_mem_ptr_t addr, void *data, size_t size);

/**
 * Copy bytes from one memory location to another
 *
 * @b Example:
 * @code
 * NPE_DECLARE_MEM8_ARRAY(to_address, 8)
 * NPE_DECLARE_MEM8_ARRAY(from_address, 8)
 * size_t size = 3;
 * npe_mem_copy(NPE_GET_MEM_ADDR_PTR(to_address[0]),
 *              NPE_GET_MEM_ADDR_PTR(from_address[0]),
 *              size);
 * @endcode
 *
 * @param to_addr   Pointer to the memory write start address
 * @param from_addr Pointer to the memory read start address
 * @param size      Number of bytes to copy
 */
__NPE_INTRINSIC void
npe_mem_copy(npe_mem_ptr_t to_addr,
             npe_mem_ptr_t from_addr,
             size_t size);

/**
 * Atomically write a 32-bit value to memory (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_ATOMIC_MEM32(address)
 * unsigned long data = 0x12345678;
 * npe_mem_write_atomic32(NPE_GET_ATOMIC_MEM_ADDR_PTR(address), data);
 * @endcode
 *
 * @param addr      Pointer to the value in memory
 * @param data      32-bit data value to write to memory
 */
__NPE_INTRINSIC void
npe_mem_write_atomic32(npe_atomic_mem_ptr_t addr, uint32_t data);

/**
 * Atomically write a 64-bit value to memory (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_ATOMIC_MEM64(address)
 * unsigned long long data = 0x123456789abcdef0ULL;
 * npe_mem_write_atomic64(NPE_GET_ATOMIC_MEM_ADDR_PTR(address), data);
 * @endcode
 *
 * @param addr      Pointer to the value in memory
 * @param data      64-bit data value to write to memory
 */
__NPE_INTRINSIC void
npe_mem_write_atomic64(npe_atomic_mem_ptr_t addr, uint64_t data);

/**
 * Atomically read a 32-bit value from memory (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_ATOMIC_MEM32(address)
 * unsigned long data;
 * data = npe_mem_read_atomic32(NPE_GET_ATOMIC_MEM_ADDR_PTR(address));
 * @endcode
 *
 * @param addr      Pointer to the value in memory
 */
__NPE_INTRINSIC uint32_t
npe_mem_read_atomic32(npe_atomic_mem_ptr_t addr);

/**
 * Atomically read a 64-bit value from memory (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_ATOMIC_MEM64(address)
 * unsigned long long data;
 * data = npe_mem_read_atomic64(NPE_GET_ATOMIC_MEM_ADDR_PTR(address));
 * @endcode
 *
 * @param addr      Pointer to the value in memory
 */
__NPE_INTRINSIC uint64_t
npe_mem_read_atomic64(npe_atomic_mem_ptr_t addr);

/**
 * Atomically add a 32-bit value to memory (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_ATOMIC_MEM32(address)
 * long data = 0x12345678;
 * npe_mem_add32(NPE_GET_ATOMIC_MEM_ADDR_PTR(address), data);
 * @endcode
 *
 * @param addr      Pointer to the value in memory
 * @param data      32-bit data value to add
 */
__NPE_INTRINSIC void
npe_mem_add32(npe_atomic_mem_ptr_t addr, int32_t data);

/**
 * Atomically add with saturation a 32-bit value to memory (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_ATOMIC_MEM32(address)
 * long data = 0x12345678;
 * npe_mem_addsat32(NPE_GET_ATOMIC_MEM_ADDR_PTR(address), data);
 * @endcode
 *
 * @param addr      Pointer to the value in memory
 * @param data      32-bit data value to add
 */
__NPE_INTRINSIC void
npe_mem_addsat32(npe_atomic_mem_ptr_t addr, int32_t data);

/**
 * Atomically add a 64-bit value to memory (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_ATOMIC_MEM64(address)
 * long long data = 0x123456789abcdef0ULL;
 * npe_mem_add64(NPE_GET_ATOMIC_MEM_ADDR_PTR(address), data);
 * @endcode
 *
 * @param addr      Pointer to the value in memory
 * @param data      64-bit data value to add
 */
__NPE_INTRINSIC void
npe_mem_add64(npe_atomic_mem_ptr_t addr, int64_t data);

/**
 * Atomically add with saturation a 64-bit value to memory (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_ATOMIC_MEM64(address)
 * long long data = 0x123456789abcdef0ULL;
 * npe_mem_addsat64(NPE_GET_ATOMIC_MEM_ADDR_PTR(address), data);
 * @endcode
 *
 * @param addr      Pointer to the value in memory
 * @param data      64-bit data value to add
 */
__NPE_INTRINSIC void
npe_mem_addsat64(npe_atomic_mem_ptr_t addr, int64_t data);

/**
 * Atomically subtract from a 32-bit value in memory (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_ATOMIC_MEM32(address)
 * long data = 0x12345678;
 * npe_mem_sub32(NPE_GET_ATOMIC_MEM_ADDR_PTR(address), data);
 * @endcode
 *
 * @param addr      Pointer to the value in memory
 * @param data      32-bit data value to subtract
 */
__NPE_INTRINSIC void
npe_mem_sub32(npe_atomic_mem_ptr_t addr, int32_t data);

/**
 * Atomically subtract with saturation from a 32-bit value in memory
 *  (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_ATOMIC_MEM32(address)
 * unsigned long data = 0x12345678;
 * npe_mem_subsat32(NPE_GET_ATOMIC_MEM_ADDR_PTR(address), data);
 * @endcode
 *
 * @param addr      Pointer to the value in memory
 * @param data      32-bit data value to subtract
 */
__NPE_INTRINSIC void
npe_mem_subsat32(npe_atomic_mem_ptr_t addr, uint32_t data);

/**
 * Atomically subtract from a 64-bit value in memory (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_ATOMIC_MEM64(address)
 * long long data = 0x123456789abcdef0ULL;
 * npe_mem_sub64(NPE_GET_ATOMIC_MEM_ADDR_PTR(address), data);
 * @endcode
 *
 * @param addr      Pointer to the value in memory
 * @param data      64-bit data value to subtract
 */
__NPE_INTRINSIC void
npe_mem_sub64(npe_atomic_mem_ptr_t addr, int64_t data);

/**
 * Atomically subtract with saturation from a 64-bit value in memory
 *  (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_ATOMIC_MEM64(address)
 * unsigned long long data = 0x123456789abcdef0ULL;
 * npe_mem_subsat64(NPE_GET_ATOMIC_MEM_ADDR_PTR(address), data);
 * @endcode
 *
 * @param addr      Pointer to the value in memory
 * @param data      64-bit data value to subtract
 */
__NPE_INTRINSIC void
npe_mem_subsat64(npe_atomic_mem_ptr_t addr, uint64_t data);

/**
 * Atomically xor with a 32-bit value in memory (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_ATOMIC_MEM32(address)
 * unsigned long data = 0x12345678;
 * npe_mem_xor32(NPE_GET_ATOMIC_MEM_ADDR_PTR(address), data);
 * @endcode
 *
 * @param addr      Pointer to the value in memory
 * @param data      32-bit data value to xor
 */
__NPE_INTRINSIC void
npe_mem_xor32(npe_atomic_mem_ptr_t addr, uint32_t data);

/**
 * Atomically clear bits in a 32-bit value in memory (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_ATOMIC_MEM32(address)
 * unsigned long data = 0x12345678;
 * npe_mem_bit_clr32(NPE_GET_ATOMIC_MEM_ADDR_PTR(address), data);
 * @endcode
 *
 * @param addr      Pointer to the value in memory
 * @param data      32-bit value that indicates which bits should be cleared
 */
__NPE_INTRINSIC void
npe_mem_bit_clr32(npe_atomic_mem_ptr_t addr, uint32_t data);

/**
 * Atomically set bits in a 32-bit value in memory (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_ATOMIC_MEM32(address)
 * unsigned long data = 0x12345678;
 * npe_mem_bit_set32(NPE_GET_ATOMIC_MEM_ADDR_PTR(address), data);
 * @endcode
 *
 * @param addr      Pointer to the value in memory
 * @param data      32-bit value that indicates which bits should be set
 */
__NPE_INTRINSIC void
npe_mem_bit_set32(npe_atomic_mem_ptr_t addr, uint32_t data);

/**
 * Atomically decrement a 32-bit value in memory (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_ATOMIC_MEM32(address)
 * npe_mem_decr32(NPE_GET_ATOMIC_MEM_ADDR_PTR(address));
 * @endcode
 *
 * @param addr      Pointer to the value in memory
 */
__NPE_INTRINSIC void
npe_mem_decr32(npe_atomic_mem_ptr_t addr);

/**
 * Atomically decrement a 64-bit value in memory (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_ATOMIC_MEM64(address)
 * npe_mem_decr64(NPE_GET_ATOMIC_MEM_ADDR_PTR(address));
 * @endcode
 *
 * @param addr      Pointer to the value in memory
 */
__NPE_INTRINSIC void
npe_mem_decr64(npe_atomic_mem_ptr_t addr);

/**
 * Atomically increment a 32-bit value in memory (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_ATOMIC_MEM32(address)
 * npe_mem_incr32(NPE_GET_ATOMIC_MEM_ADDR_PTR(address));
 * @endcode
 *
 * @param addr      Pointer to the value in memory
 */
__NPE_INTRINSIC void
npe_mem_incr32(npe_atomic_mem_ptr_t addr);

/**
 * Atomically increment a 64-bit value in memory (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_ATOMIC_MEM64(address)
 * npe_mem_incr64(NPE_GET_ATOMIC_MEM_ADDR_PTR(address));
 * @endcode
 *
 * @param addr      Pointer to the value in memory
 */
__NPE_INTRINSIC void
npe_mem_incr64(npe_atomic_mem_ptr_t addr);

/**
 * Atomically test and add a 32-bit value to memory (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_ATOMIC_MEM32(address)
 * unsigned long mem_data;
 * long data = 0x12345678;
 * mem_data = npe_mem_test_and_add32(NPE_GET_ATOMIC_MEM_ADDR_PTR(address), data);
 * @endcode
 *
 * @param addr      Pointer to the value in memory
 * @param data      32-bit data value to add
 */
__NPE_INTRINSIC uint32_t
npe_mem_test_and_add32(npe_atomic_mem_ptr_t addr, int32_t data);

/**
 * Atomically test and add with saturation a 32-bit value to memory
 *  (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_ATOMIC_MEM32(address)
 * unsigned long mem_data;
 * long data = 0x12345678;
 * mem_data = npe_mem_test_and_addsat32(NPE_GET_ATOMIC_MEM_ADDR_PTR(address), data);
 * @endcode
 *
 * @param addr      Pointer to the value in memory
 * @param data      32-bit data value to add
 */
__NPE_INTRINSIC uint32_t
npe_mem_test_and_addsat32(npe_atomic_mem_ptr_t addr, int32_t data);

/**
 * Atomically test and add a 64-bit value to memory (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_ATOMIC_MEM64(address)
 * unsigned long long mem_data;
 * long long data = 0x123456789abcdef0LL;
 * mem_data = npe_mem_test_and_add64(NPE_GET_ATOMIC_MEM_ADDR_PTR(address), data);
 * @endcode
 *
 * @param addr      Pointer to the value in memory
 * @param data      64-bit data value to add
 */
__NPE_INTRINSIC uint64_t
npe_mem_test_and_add64(npe_atomic_mem_ptr_t addr, int64_t data);

/**
 * Atomically test and add with saturation a 64-bit value to memory
 *  (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_ATOMIC_MEM64(address)
 * unsigned long long mem_data;
 * long long data = 0x123456789abcdef0LL;
 * mem_data = npe_mem_test_and_addsat64(NPE_GET_ATOMIC_MEM_ADDR_PTR(address), data);
 * @endcode
 *
 * @param addr      Pointer to the value in memory
 * @param data      64-bit data value to add
 */
__NPE_INTRINSIC uint64_t
npe_mem_test_and_addsat64(npe_atomic_mem_ptr_t addr, int64_t data);

/**
 * Atomically test and subtract from a 32-bit value in memory
 *  (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_ATOMIC_MEM32(address)
 * unsigned long mem_data;
 * long data = 0x12345678;
 * mem_data = npe_mem_test_and_sub32(NPE_GET_ATOMIC_MEM_ADDR_PTR(address), data);
 * @endcode
 *
 * @param addr      Pointer to the value in memory
 * @param data      32-bit data value to subtract
 */
__NPE_INTRINSIC uint32_t
npe_mem_test_and_sub32(npe_atomic_mem_ptr_t addr, int32_t data);

/**
 * Atomically test and subtract with saturation from a 32-bit value in memory
 *  (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_ATOMIC_MEM32(address)
 * unsigned long mem_data;
 * unsigned long data = 0x12345678;
 * mem_data = npe_mem_test_and_subsat32(NPE_GET_ATOMIC_MEM_ADDR_PTR(address), data);
 * @endcode
 *
 * @param addr      Pointer to the value in memory
 * @param data      32-bit data value to subtract
 */
__NPE_INTRINSIC uint32_t
npe_mem_test_and_subsat32(npe_atomic_mem_ptr_t addr, uint32_t data);

/**
 * Atomically test and subtract from a 64-bit value in memory
 *  (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_ATOMIC_MEM64(address)
 * unsigned long long mem_data;
 * long long data = 0x123456789abcdef0LL;
 * mem_data = npe_mem_test_and_sub64(NPE_GET_ATOMIC_MEM_ADDR_PTR(address), data);
 * @endcode
 *
 * @param addr      Pointer to the value in memory
 * @param data      64-bit data value to subtract
 */
__NPE_INTRINSIC uint64_t
npe_mem_test_and_sub64(npe_atomic_mem_ptr_t addr, int64_t data);

/**
 * Atomically test and subtract with saturation from a 64-bit value in memory
 *  (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_ATOMIC_MEM64(address)
 * unsigned long long mem_data;
 * unsigned long long data = 0x123456789abcdef0ULL;
 * mem_data = npe_mem_test_and_subsat64(NPE_GET_ATOMIC_MEM_ADDR_PTR(address), data);
 * @endcode
 *
 * @param addr      Pointer to the value in memory
 * @param data      64-bit data value to subtract
 */
__NPE_INTRINSIC uint64_t
npe_mem_test_and_subsat64(npe_atomic_mem_ptr_t addr, uint64_t data);

/**
 * Atomically test and xor with a 32-bit value in memory (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_ATOMIC_MEM32(address)
 * unsigned long mem_data;
 * unsigned long data = 0x12345678;
 * mem_data = npe_mem_test_and_xor32(NPE_GET_ATOMIC_MEM_ADDR_PTR(address), data);
 * @endcode
 *
 * @param addr      Pointer to the value in memory
 * @param data      32-bit data value to xor
 */
__NPE_INTRINSIC uint32_t
npe_mem_test_and_xor32(npe_atomic_mem_ptr_t addr, uint32_t data);

/**
 * Atomically test and clear bits in a 32-bit value in memory
 *  (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_ATOMIC_MEM32(address)
 * unsigned long mem_data;
 * unsigned long data = 0x12345678;
 * mem_data = npe_mem_test_and_bit_clr32(NPE_GET_ATOMIC_MEM_ADDR_PTR(address), data);
 * @endcode
 *
 * @param addr      Pointer to the value in memory
 * @param data      32-bit value that indicates which bits should be cleared 
 */
__NPE_INTRINSIC uint32_t
npe_mem_test_and_bit_clr32(npe_atomic_mem_ptr_t addr, uint32_t data);

/**
 * Atomically test and set bits in a 32-bit value in memory
 *  (EMEM, IMEM, or CTM)
 *
 * @b Example:
 * @code
 * NPE_DECLARE_ATOMIC_MEM32(address)
 * unsigned long mem_data;
 * unsigned long data = 0x12345678;
 * mem_data = npe_mem_test_and_bit_set32(NPE_GET_ATOMIC_MEM_ADDR_PTR(address), data);
 * @endcode
 *
 * @param addr      Pointer to the value in memory
 * @param data      32-bit value that indicates which bits should be set
 */
__NPE_INTRINSIC uint32_t
npe_mem_test_and_bit_set32(npe_atomic_mem_ptr_t addr, uint32_t data);

/** @}
 * @}
 */

#endif /* __NPE_MEM_H_ */
