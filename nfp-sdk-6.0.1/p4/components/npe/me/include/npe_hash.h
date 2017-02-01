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
 */

/* Hash operations */

#ifndef __NPE_HASH_H__
#define __NPE_HASH_H__

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

/** @file npe_hash.h
 * @addtogroup hash  Hash Operations
 * @{
 */

/** @name Hash related functions
 *  @{
 */


/**
 * Compute crc 32 over a data region located in 'local' memory.
 *  For NFCC this is NFP lm, gpr, nn_reg, xfr_reg
 *
 * @b Example:
 * @code
 * __declspec(gp_reg) uint32_t reg[4];
 * uint32_t hash_val;
 * reg[0] = 0x00;
 * reg[1] = 0x04;
 * reg[2] = 0x08;
 * reg[3] = 0x0C;
 * hash_val = npe_hash_crc_32(reg,
 *                            sizeof(reg),
 *                            0x12345678,
 *                            NPE_BIG_ENDIAN,
 *                            NPE_BIT_SWAP_DISABLE);
 * @endcode
 *
 * @param p      Pointer to data
 * @param size   Size of region (in bytes, maximum 67), constant
 * @param init   Initial seed value
 * @param endian Big or little endian byte order selection, constant
 * @param swap   Enable/disable bit swap within bytes
 * @return       CRC 32 checksum
 */
__NPE_INTRINSIC uint32_t npe_hash_crc_32(void *p, size_t size,
                                         uint32_t init,
                                         enum npe_endian endian,
                                         enum npe_bit_swap swap);


/**
 * Compute crc 32 over a data region located in 'remote' memory.
 *  For NFCC this is NFP imem, ctm, or emem
 *
 * @b Example:
 * @code
 * __declspec(shared imem export aligned(64)) uint32_t mem[4];
 * uint32_t hash_val;
 * __xwrite uint32_t xfr[4];
 * xfr[0] = 0x00;
 * xfr[1] = 0x04;
 * xfr[2] = 0x08;
 * xfr[3] = 0x0C;
 * mem_write64(&xfr[0], mem, sizeof(mem));
 * hash_val = npe_hash_crc_32_rem(mem,
 *                                sizeof(mem),
 *                                0x12345678,
 *                                NPE_BIG_ENDIAN,
 *                                NPE_BIT_SWAP_DISABLE);
 * @endcode
 *
 * @param p      Pointer to data
 * @param size   Size of region (in bytes)
 * @param init   Initial seed value
 * @param endian Big or little endian byte order selection
 * @param swap   Enable/disable bit swap within bytes
 * @return       CRC 32 checksum
 */
uint32_t npe_hash_crc_32_rem(npe_mem_ptr_t p, size_t size, uint32_t init,
                             enum npe_endian endian, enum npe_bit_swap swap);


/**
 * Compute crc ccitt over a data region located in 'local' memory.
 *  For NFCC this is NFP lm, gpr, nn_reg, xfr_reg
 *
 * @b Example:
 * @code
 * __declspec(gp_reg) uint32_t reg[4];
 * uint32_t hash_val;
 * reg[0] = 0x00;
 * reg[1] = 0x04;
 * reg[2] = 0x08;
 * reg[3] = 0x0C;
 * hash_val = npe_hash_crc_ccitt(reg,
 *                               sizeof(reg),
 *                               0x12345678,
 *                               NPE_BIG_ENDIAN,
 *                               NPE_BIT_SWAP_DISABLE);
 * @endcode
 *
 * @param p      Pointer to data
 * @param size   Size of region (in bytes, maximum 67), constant
 * @param init   Initial seed value
 * @param endian Big or little endian byte order selection, constant
 * @param swap   Enable/disable bit swap within bytes, constant
 * @return       CCITT checksum
 */
__NPE_INTRINSIC uint32_t npe_hash_crc_ccitt(void *p, size_t size,
                                            uint32_t init,
                                            enum npe_endian endian,
                                            enum npe_bit_swap swap);


/**
 * Compute crc ccitt over a data region located in 'remote' memory.
 *  For NFCC this is NFP imem, ctm, or emem
 *
 * @b Example:
 * @code
 * __declspec(shared imem export aligned(64)) uint32_t mem[4];
 * uint32_t hash_val;
 * __xwrite uint32_t xfr[4];
 * xfr[0] = 0x00;
 * xfr[1] = 0x04;
 * xfr[2] = 0x08;
 * xfr[3] = 0x0C;
 * mem_write64(&xfr[0], mem, sizeof(mem));
 * hash_val = npe_hash_crc_ccitt_rem(mem,
 *                                   sizeof(mem),
 *                                   0x12345678,
 *                                   NPE_BIG_ENDIAN,
 *                                   NPE_BIT_SWAP_DISABLE);
 * @endcode
 *
 * @param p      Pointer to data
 * @param size   Size of region (in bytes)
 * @param init   Initial seed value
 * @param endian Big or little endian byte order selection
 * @param swap   Enable/disable bit swap within bytes
 * @return       CCITT checksum
 */
uint32_t npe_hash_crc_ccitt_rem(npe_mem_ptr_t p, size_t size, uint32_t init,
                                enum npe_endian endian, enum npe_bit_swap swap);


/**
 * Compute Fletcher 16 checksum over a data region located in 'local' memory
 *  For NFCC this is NFP lm, gpr, nn_reg, xfr_reg
 *
 * @b Example:
 * @code
 * __declspec(gp_reg) uint32_t reg[4];
 * uint32_t hash_val;
 * reg[0] = 0x00;
 * reg[1] = 0x04;
 * reg[2] = 0x08;
 * reg[3] = 0x0C;
 * hash_val = npe_hash_fletcher_16(reg,
 *                                 sizeof(reg));
 * @endcode
 *
 * @param p     Pointer to data
 * @param size  Size of region (in bytes, maximum 67)
 *
 * @return      16 bit fletcher checksum value
 */
__NPE_INTRINSIC uint16_t npe_hash_fletcher_16(void *p, size_t size);


/**
 * Compute Fletcher 16 checksum over a data region located in 'remote' memory
 *  For NFCC this is NFP imem, ctm, or emem
 *
 * @b Example:
 * @code
 * __declspec(shared imem export aligned(64)) uint32_t mem[4];
 * uint32_t hash_val;
 * __xwrite uint32_t xfr[4];
 * xfr[0] = 0x00;
 * xfr[1] = 0x04;
 * xfr[2] = 0x08;
 * xfr[3] = 0x0C;
 * mem_write64(&xfr[0], mem, sizeof(mem));
 * hash_val = npe_hash_fletcher_16_rem(mem,
 *                                     sizeof(mem));
 * @endcode
 *
 * @param p     Pointer to data
 * @param size  Size of region (in bytes)
 *
 * @return      16 bit fletcher checksum value
 */
uint16_t npe_hash_fletcher_16_rem(npe_mem_ptr_t p, size_t size);


/**
 * Compute Fletcher 32 checksum over a data region located in 'local' memory
 *  For NFCC this is NFP lm, gpr, nn_reg, xfr_reg
 *
 * @b Example:
 * @code
 * __declspec(gp_reg) uint32_t reg[4];
 * uint32_t hash_val;
 * reg[0] = 0x00;
 * reg[1] = 0x04;
 * reg[2] = 0x08;
 * reg[3] = 0x0C;
 * hash_val = npe_hash_fletcher_32(reg,
 *                                 sizeof(reg));
 * @endcode
 *
 * @param p     Pointer to data
 * @param size  Size of region (in bytes, maximum 66, 2 byte multiple)
 *
 * @return      32 bit fletcher checksum value
 */
__NPE_INTRINSIC uint32_t npe_hash_fletcher_32(void *p, size_t size);


/**
 * Compute Fletcher 32 checksum over a data region located in 'remote' memory
 *  For NFCC this is NFP imem, ctm, or emem
 *
 * @b Example:
 * @code
 * __declspec(shared imem export aligned(64)) uint32_t mem[4];
 * uint32_t hash_val;
 * __xwrite uint32_t xfr[4];
 * xfr[0] = 0x00;
 * xfr[1] = 0x04;
 * xfr[2] = 0x08;
 * xfr[3] = 0x0C;
 * mem_write64(&xfr[0], mem, sizeof(mem));
 * hash_val = npe_hash_fletcher_32_rem(mem,
 *                                     sizeof(mem));
 * @endcode
 *
 * @param p     Pointer to data
 * @param size  Size of region (in bytes, 2 byte multiple)
 *
 * @return      32 bit fletcher checksum value
 */
uint32_t npe_hash_fletcher_32_rem(npe_mem_ptr_t p, size_t size);


/**
 * Compute Jenkins Byte hash over a data region located in 'local' memory
 *  For NFCC this is NFP lm, gpr, nn_reg, xfr_reg
 *
 * @b Example:
 * @code
 * __declspec(gp_reg) uint32_t reg[4];
 * uint32_t hash_val;
 * reg[0] = 0x00;
 * reg[1] = 0x04;
 * reg[2] = 0x08;
 * reg[3] = 0x0C;
 * hash_val = npe_hash_jenkins_8(reg,
 *                               sizeof(reg));
 * @endcode
 *
 * @param p     Pointer to data
 * @param size  Size of region (in bytes)
 *
 * @return      32 bit jenkins hash value
 */
__NPE_INTRINSIC uint32_t npe_hash_jenkins_8(void *p, size_t size);


/**
 * Compute Jenkins Byte hash over a data region located in 'remote' memory
 *  For NFCC this is NFP imem, ctm, or emem
 *
 * @b Example:
 * @code
 * __declspec(shared imem export aligned(64)) uint32_t mem[4];
 * uint32_t hash_val;
 * __xwrite uint32_t xfr[4];
 * xfr[0] = 0x00;
 * xfr[1] = 0x04;
 * xfr[2] = 0x08;
 * xfr[3] = 0x0C;
 * mem_write64(&xfr[0], mem, sizeof(mem));
 * hash_val = npe_hash_jenkins_8_rem(mem,
 *                                   sizeof(mem));
 * @endcode
 *
 * @param p     Pointer to data
 * @param size  Size of region (in bytes)
 *
 * @return      32 bit jenkins hash value
 */
uint32_t npe_hash_jenkins_8_rem(npe_mem_ptr_t p, size_t size);


/**
 * Compute Jenkins hash on lwords over a data region located in 'local' memory
 *  For NFCC this is NFP lm, gpr, nn_reg, xfr_reg
 *
 * @b Example:
 * @code
 * __declspec(gp_reg) uint32_t reg[4];
 * uint32_t hash_val;
 * reg[0] = 0x00;
 * reg[1] = 0x04;
 * reg[2] = 0x08;
 * reg[3] = 0x0C;
 * hash_val = npe_hash_jenkins_32(reg,
 *                                sizeof(reg));
 * @endcode
 *
 * @param p     Pointer to data
 * @param size  Size of region (in bytes) Must be
 *              a 4 byte multiple
 *
 * @return      32 bit jenkins hash value
 */
__NPE_INTRINSIC uint32_t npe_hash_jenkins_32(void *p, size_t size);


/**
 * Compute Jenkins hash on lwords over a data region located in 'remote' memory
 *  For NFCC this is NFP imem, ctm, or emem
 *
 * @b Example:
 * @code
 * __declspec(shared imem export aligned(64)) uint32_t mem[4];
 * uint32_t hash_val;
 * __xwrite uint32_t xfr[4];
 * xfr[0] = 0x00;
 * xfr[1] = 0x04;
 * xfr[2] = 0x08;
 * xfr[3] = 0x0C;
 * mem_write64(&xfr[0], mem, sizeof(mem));
 * hash_val = npe_hash_jenkins_32_rem(mem,
 *                                    sizeof(mem));
 * @endcode
 *
 * @param p     Pointer to data
 * @param size  Size of region (in bytes) Must be
 *              a 4 byte multiple
 *
 * @return      32 bit jenkins hash value
 */
uint32_t npe_hash_jenkins_32_rem(npe_mem_ptr_t p, size_t size);

/** @}
 * @}
 */

#endif /* __NPE_HASH_H__ */
