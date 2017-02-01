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

/* Memory CAM operations */

#ifndef __NPE_CAM_H__
#define __NPE_CAM_H__

/* Compilation for Netronome NFP */
#if defined(__NFP_TOOL_NFCC)

    #ifndef NFP_LIB_ANY_NFCC_VERSION
        #if (__NFP_TOOL_NFCC < NFP_SW_VERSION(5, 0, 0, 0)) ||   \
            (__NFP_TOOL_NFCC > NFP_SW_VERSION(6, 255, 255, 255)))
            #error "This software is not supported for the version of the SDK currently in use."
         #endif
    #endif
#endif

#include <npe_types.h>
#include <stdint.h>

/** @file npe_cam.h
 * @addtogroup Memory CAM operations
 * @{
 */

/** @name Memory CAM related functions
 * @{
 */


/* Return Codes */
#define NPE_CAM_OK     0
#define NPE_CAM_FAIL   1

/* Result Codes */
enum NPE_CAM_RESULT {
    NPE_CAM_NO_MATCH,
    NPE_CAM_MATCH,
    NPE_CAM_ADDED,
    NPE_CAM_NOT_ADDED_CAM_FULL
};

/* Memory CAM Size in Bits */
enum NPE_CAM_SIZE {
    NPE_CAM_128_BITS = 128,
    NPE_CAM_256_BITS = 256,
    NPE_CAM_384_BITS = 384,
    NPE_CAM_512_BITS = 512
};

/* Memory CAM Entry Size in Bits */
enum NPE_CAM_ENTRY_SIZE {
    NPE_CAM_ENTRY_8_BITS = 8,
    NPE_CAM_ENTRY_16_BITS = 16,
    NPE_CAM_ENTRY_24_BITS = 24,
    NPE_CAM_ENTRY_32_BITS =32
};

#ifndef __NFP_TOOL_NFCC

typedef struct {
   union {
      uint8_t  val8[16];
      uint16_t val16[8];
      uint32_t val24[4];
      uint32_t val32[4];
   };
} npe_cam_128_t;

typedef struct {
   union {
      uint8_t  val8[32];
      uint16_t val16[16];
      uint32_t val24[8];
      uint32_t val32[8];
   };
} npe_cam_256_t;

typedef struct {
   union {
      uint8_t  val8[48];
      uint16_t val16[24];
      uint32_t val24[12];
      uint32_t val32[12];
   };
} npe_cam_384_t;

typedef struct {
   union {
      uint8_t  val8[64];
      uint16_t val16[32];
      uint32_t val24[16];
      uint32_t val32[16];
   };
} npe_cam_512_t;

#endif /* __NFP_TOOL_NFCC__ */

/** Memory CAM Lookup
 *
 * @b Example:
 * @code
 * #ifdef __NFP_TOOL_NFCC
 * __declspec(shared export emem0 aligned(64)) unsigned int test_cam128[4];
 * #else
 * unsigned int test_cam128[4];
 * #endif
 * uint32_t result;
 * npe_mem_cam_lookup(test_cam128, &result, 0x11, NPE_CAM_128_BITS, NPE_CAM_ENTRY_8_BITS);
 * @endcode
 *
 * @param p_cam      Pointer to Memory CAM
 * @param out_result Location where lookup result is returned
 *                     index << 8 | one of enum NPE_CAM_RESULT
 * @param value      Value to lookup
 * @param cam_size   CAM Size - one of enum NPE_CAM_SIZE
 * @param entry_size Entry Size - one of enum NPE_CAM_ENTRY_SIZE
 * @return           NPE_CAM_OK:success
 *                   NPE_CAM_FAIL:failure
 */
 int
npe_mem_cam_lookup(npe_mem_ptr_t p_cam,
                   uint32_t *out_result,
                   uint32_t value,
                   uint32_t cam_size,
                   uint8_t entry_size);

/** Memory CAM Lookup With Add
 *
 * @b Example:
 * @code
 * #ifdef __NFP_TOOL_NFCC
 * __declspec(shared export emem0 aligned(64)) unsigned int test_cam128[4];
 * #else
 * unsigned int test_cam128[4];
 * #endif
 * uint32_t result;
 * npe_mem_cam_lookup_add(test_cam128, &result, 0x11, NPE_CAM_128_BITS, NPE_CAM_ENTRY_8_BITS);
 * @endcode
 *
 * @param p_cam      Pointer to Memory CAM
 * @param out_result Location where lookup result is returned
 *                     index << 8 | one of enum NPE_CAM_RESULT
 * @param value      Value to lookup
 * @param cam_size   CAM Size - one of enum NPE_CAM_SIZE
 * @param entry_size Entry Size - one of enum NPE_CAM_ENTRY_SIZE
 * @return           NPE_CAM_OK:success
 *                   NPE_CAM_FAIL:failure
 */
 int
npe_mem_cam_lookup_add(npe_mem_ptr_t p_cam,
                       uint32_t *out_result,
                       uint32_t value,
                       uint32_t cam_size,
                       uint8_t entry_size);

/** @}
 * @}
 */

#endif /* __NPE_CAM_H__ */
