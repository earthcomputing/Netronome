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
 * @file          src/npe_cam.c
 * @brief         Memory CAM operations
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

/* flowenv code */
#ifdef __NFP_TOOL_NFCC
#include <assert.h>
#include <nfp_me.h>
#include <nfp/mem_cam.h>
#endif

/* npe code */
#include <npe_types.h>
#include <npe_cam.h>

#ifdef __NFP_TOOL_NFCC

/* Find First Set bit */
unsigned int
_npe_ffs(unsigned int data)
{
    unsigned result;
    __asm ffs[result, data];
    return result;
}

/* Memory CAM Lookup for 128-bit CAM */
int
_npe_mem_cam_lookup_128(npe_mem_ptr_t p_cam,
                       uint32_t *out_result,
                       uint32_t value,
                       uint8_t entry_size)
{
    uint32_t match_index;

    switch(entry_size) {
       case NPE_CAM_ENTRY_8_BITS: {
           __xrw struct mem_cam_8bit data_result;

           data_result.search.value = value & 0xff;

           mem_cam_lookup(&data_result,
                         (__mem __align32 void *)p_cam,
                         128,
                         8);

           if(mem_cam_lookup8_hit(data_result)) {

               if(data_result.result.mask_lo) {

                   match_index = _npe_ffs(data_result.result.mask_lo);

               } else {

                   match_index = 32 + _npe_ffs(data_result.result.mask_hi);
               }
               *out_result = (match_index << 8) | NPE_CAM_MATCH;
           }
           else
               *out_result = NPE_CAM_NO_MATCH;
           break;
       }
       case NPE_CAM_ENTRY_16_BITS: {
           __xrw struct mem_cam_16bit data_result;

           data_result.search.value = value & 0xffff;

           mem_cam_lookup(&data_result,
                         (__mem __align32 void *)p_cam,
                         128,
                         16);

           if(mem_cam_lookup_hit(data_result))
               *out_result = ((data_result.result.match & 0x7f) << 8) |
                              NPE_CAM_MATCH;
           else
               *out_result = NPE_CAM_NO_MATCH;
           break;
       }
       case NPE_CAM_ENTRY_24_BITS: {
           __xrw struct mem_cam_24bit data_result;

           data_result.search.value = value & 0xffffff;

           mem_cam_lookup(&data_result,
                         (__mem __align32 void *)p_cam,
                         128,
                         24);

           if(mem_cam_lookup_hit(data_result))
               *out_result = ((data_result.result.match & 0x7f) << 8) |
                              NPE_CAM_MATCH;
           else
               *out_result = NPE_CAM_NO_MATCH;
           break;
       }
       case NPE_CAM_ENTRY_32_BITS: {
           __xrw struct mem_cam_32bit data_result;

           data_result.search.value = value;

           mem_cam_lookup(&data_result,
                         (__mem __align32 void *)p_cam,
                         128,
                         32);

           if(mem_cam_lookup_hit(data_result))
               *out_result = ((data_result.result.match & 0x7f) << 8) |
                              NPE_CAM_MATCH;
           else
               *out_result = NPE_CAM_NO_MATCH;
           break;
       }
       default:
           return NPE_CAM_FAIL;
           break;
    }

    return NPE_CAM_OK;
}

/* Memory CAM Lookup with Add for 128 Bit CAM */
int
_npe_mem_cam_lookup_add_128(npe_mem_ptr_t p_cam,
                           uint32_t *out_result,
                           uint32_t value,
                           uint8_t entry_size)
{
    switch(entry_size) {
       case NPE_CAM_ENTRY_8_BITS: {
           __xrw struct mem_cam_8bit data_result;

           data_result.search.value = value & 0xff;

           mem_cam_lookup_add(&data_result,
                             (__mem __align32 void *)p_cam,
                             128,
                             8);

           if((data_result.result.mask_lo & 0x7f) == 0x7f)
               *out_result = NPE_CAM_NOT_ADDED_CAM_FULL;

           else if(data_result.result.mask_lo & 0x80)  // Added
               *out_result = ((data_result.result.mask_lo & 0x7f) << 8) |
                              NPE_CAM_ADDED;
           else
               *out_result = ((data_result.result.mask_lo & 0x7f) << 8) |
                              NPE_CAM_MATCH;
           break;
       }
       case NPE_CAM_ENTRY_16_BITS: {
           __xrw struct mem_cam_16bit data_result;

           data_result.search.value = value & 0xffff;

           mem_cam_lookup_add(&data_result,
                             (__mem __align32 void *)p_cam,
                             128,
                             16);

           if(mem_cam_lookup_add_fail(data_result))
               *out_result = NPE_CAM_NOT_ADDED_CAM_FULL;

           else if(mem_cam_lookup_add_added(data_result))
               *out_result = ((data_result.result.match & 0x7f) << 8) |
                              NPE_CAM_ADDED;
           else
               *out_result = ((data_result.result.match & 0x7f) << 8) |
                              NPE_CAM_MATCH;
           break;
       }
       case NPE_CAM_ENTRY_24_BITS: {
           __xrw struct mem_cam_24bit data_result;

           data_result.search.value = value & 0xffffff;

           mem_cam_lookup_add(&data_result,
                             (__mem __align32 void *)p_cam,
                             128,
                             24);

           if(mem_cam_lookup_add_fail(data_result))
               *out_result = NPE_CAM_NOT_ADDED_CAM_FULL;

           else if(mem_cam_lookup_add_added(data_result))
               *out_result = ((data_result.result.match & 0x7f) << 8) |
                              NPE_CAM_ADDED;
           else
               *out_result = ((data_result.result.match & 0x7f) << 8) |
                              NPE_CAM_MATCH;
           break;
       }
       case NPE_CAM_ENTRY_32_BITS: {
           __xrw struct mem_cam_32bit data_result;

           data_result.search.value = value;

           mem_cam_lookup_add(&data_result,
                             (__mem __align32 void *)p_cam,
                             128,
                             32);

           if(mem_cam_lookup_add_fail(data_result))
               *out_result = NPE_CAM_NOT_ADDED_CAM_FULL;

           else if(mem_cam_lookup_add_added(data_result))
               *out_result = ((data_result.result.match & 0x7f) << 8) |
                              NPE_CAM_ADDED;
           else
               *out_result = ((data_result.result.match & 0x7f) << 8) |
                              NPE_CAM_MATCH;
           break;
       }
       default:
           return NPE_CAM_FAIL;
           break;
    }

    return NPE_CAM_OK;
}

/* Memory CAM Lookup for 256-bit CAM */
int
_npe_mem_cam_lookup_256(npe_mem_ptr_t p_cam,
                       uint32_t *out_result,
                       uint32_t value,
                       uint8_t entry_size)
{
    uint32_t match_index;

    switch(entry_size) {
       case NPE_CAM_ENTRY_8_BITS: {
           __xrw struct mem_cam_8bit data_result;

           data_result.search.value = value & 0xff;

           mem_cam_lookup(&data_result,
                         (__mem __align32 void *)p_cam,
                         256,
                         8);

           if(mem_cam_lookup8_hit(data_result)) {

               if(data_result.result.mask_lo) {

                   match_index = _npe_ffs(data_result.result.mask_lo);

               } else {

                   match_index = 32 + _npe_ffs(data_result.result.mask_hi);
               }
               *out_result = (match_index << 8) | NPE_CAM_MATCH;
           }
           else
               *out_result = NPE_CAM_NO_MATCH;
           break;
       }
       case NPE_CAM_ENTRY_16_BITS: {
           __xrw struct mem_cam_16bit data_result;

           data_result.search.value = value & 0xffff;

           mem_cam_lookup(&data_result,
                         (__mem __align32 void *)p_cam,
                         256,
                         16);

           if(mem_cam_lookup_hit(data_result))
               *out_result = ((data_result.result.match & 0x7f) << 8) |
                              NPE_CAM_MATCH;
           else
               *out_result = NPE_CAM_NO_MATCH;
           break;
       }
       case NPE_CAM_ENTRY_24_BITS: {
           __xrw struct mem_cam_24bit data_result;

           data_result.search.value = value & 0xffffff;

           mem_cam_lookup(&data_result,
                         (__mem __align32 void *)p_cam,
                         256,
                         24);

           if(mem_cam_lookup_hit(data_result))
               *out_result = ((data_result.result.match & 0x7f) << 8) |
                              NPE_CAM_MATCH;
           else
               *out_result = NPE_CAM_NO_MATCH;
           break;
       }
       case NPE_CAM_ENTRY_32_BITS: {
           __xrw struct mem_cam_32bit data_result;

           data_result.search.value = value;

           mem_cam_lookup(&data_result,
                         (__mem __align32 void *)p_cam,
                         256,
                         32);

           if(mem_cam_lookup_hit(data_result))
               *out_result = ((data_result.result.match & 0x7f) << 8) |
                              NPE_CAM_MATCH;
           else
               *out_result = NPE_CAM_NO_MATCH;
           break;
       }
       default:
           return NPE_CAM_FAIL;
           break;
    }

    return NPE_CAM_OK;
}

/* Memory CAM Lookup with Add for 256 Bit CAM */
int
_npe_mem_cam_lookup_add_256(npe_mem_ptr_t p_cam,
                           uint32_t *out_result,
                           uint32_t value,
                           uint8_t entry_size)
{
    switch(entry_size) {
       case NPE_CAM_ENTRY_8_BITS: {
           __xrw struct mem_cam_8bit data_result;

           data_result.search.value = value & 0xff;

           mem_cam_lookup_add(&data_result,
                             (__mem __align32 void *)p_cam,
                             256,
                             8);

           if((data_result.result.mask_lo & 0x7f) == 0x7f)
               *out_result = NPE_CAM_NOT_ADDED_CAM_FULL;

           else if(data_result.result.mask_lo & 0x80)  // Added
               *out_result = ((data_result.result.mask_lo & 0x7f) << 8) |
                              NPE_CAM_ADDED;
           else
               *out_result = ((data_result.result.mask_lo & 0x7f) << 8) |
                              NPE_CAM_MATCH;
           break;
       }
       case NPE_CAM_ENTRY_16_BITS: {
           __xrw struct mem_cam_16bit data_result;

           data_result.search.value = value & 0xffff;

           mem_cam_lookup_add(&data_result,
                             (__mem __align32 void *)p_cam,
                             256,
                             16);

           if(mem_cam_lookup_add_fail(data_result))
               *out_result = NPE_CAM_NOT_ADDED_CAM_FULL;

           else if(mem_cam_lookup_add_added(data_result))
               *out_result = ((data_result.result.match & 0x7f) << 8) |
                              NPE_CAM_ADDED;
           else
               *out_result = ((data_result.result.match & 0x7f) << 8) |
                              NPE_CAM_MATCH;
           break;
       }
       case NPE_CAM_ENTRY_24_BITS: {
           __xrw struct mem_cam_24bit data_result;

           data_result.search.value = value & 0xffffff;

           mem_cam_lookup_add(&data_result,
                             (__mem __align32 void *)p_cam,
                             256,
                             24);

           if(mem_cam_lookup_add_fail(data_result))
               *out_result = NPE_CAM_NOT_ADDED_CAM_FULL;

           else if(mem_cam_lookup_add_added(data_result))
               *out_result = ((data_result.result.match & 0x7f) << 8) |
                              NPE_CAM_ADDED;
           else
               *out_result = ((data_result.result.match & 0x7f) << 8) |
                              NPE_CAM_MATCH;
           break;
       }
       case NPE_CAM_ENTRY_32_BITS: {
           __xrw struct mem_cam_32bit data_result;

           data_result.search.value = value;

           mem_cam_lookup_add(&data_result,
                             (__mem __align32 void *)p_cam,
                             256,
                             32);

           if(mem_cam_lookup_add_fail(data_result))
               *out_result = NPE_CAM_NOT_ADDED_CAM_FULL;

           else if(mem_cam_lookup_add_added(data_result))
               *out_result = ((data_result.result.match & 0x7f) << 8) |
                              NPE_CAM_ADDED;
           else
               *out_result = ((data_result.result.match & 0x7f) << 8) |
                              NPE_CAM_MATCH;
           break;
       }
       default:
           return NPE_CAM_FAIL;
           break;
    }

    return NPE_CAM_OK;
}

/* Memory CAM Lookup for 384-bit CAM */
int
_npe_mem_cam_lookup_384(npe_mem_ptr_t p_cam,
                       uint32_t *out_result,
                       uint32_t value,
                       uint8_t entry_size)
{
    uint32_t match_index;

    switch(entry_size) {
       case NPE_CAM_ENTRY_8_BITS: {
           __xrw struct mem_cam_8bit data_result;

           data_result.search.value = value & 0xff;

           mem_cam_lookup(&data_result,
                         (__mem __align32 void *)p_cam,
                         384,
                         8);

           if(data_result.result.mask_lo || data_result.result.mask_hi) {

               if(data_result.result.mask_lo) {

                   match_index = _npe_ffs(data_result.result.mask_lo);

               } else {

                   match_index = 32 + _npe_ffs(data_result.result.mask_hi);
               }
               *out_result = (match_index << 8) | NPE_CAM_MATCH;
           }
           else
               *out_result = NPE_CAM_NO_MATCH;
           break;
       }
       case NPE_CAM_ENTRY_16_BITS: {
           __xrw struct mem_cam_16bit data_result;

           data_result.search.value = value & 0xffff;

           mem_cam_lookup(&data_result,
                         (__mem __align32 void *)p_cam,
                         384,
                         16);

           if(mem_cam_lookup_hit(data_result))
               *out_result = ((data_result.result.match & 0x7f) << 8) |
                              NPE_CAM_MATCH;
           else
               *out_result = NPE_CAM_NO_MATCH;
           break;
       }
       case NPE_CAM_ENTRY_24_BITS: {
           __xrw struct mem_cam_24bit data_result;

           data_result.search.value = value & 0xffffff;

           mem_cam_lookup(&data_result,
                         (__mem __align32 void *)p_cam,
                         384,
                         24);

           if(mem_cam_lookup_hit(data_result))
               *out_result = ((data_result.result.match & 0x7f) << 8) |
                              NPE_CAM_MATCH;
           else
               *out_result = NPE_CAM_NO_MATCH;
           break;
       }
       case NPE_CAM_ENTRY_32_BITS: {
           __xrw struct mem_cam_32bit data_result;

           data_result.search.value = value;

           mem_cam_lookup(&data_result,
                         (__mem __align32 void *)p_cam,
                         384,
                         32);

           if(mem_cam_lookup_hit(data_result))
               *out_result = ((data_result.result.match & 0x7f) << 8) |
                              NPE_CAM_MATCH;
           else
               *out_result = NPE_CAM_NO_MATCH;
           break;
       }
       default:
           return NPE_CAM_FAIL;
           break;
    }

    return NPE_CAM_OK;
}

/* Memory CAM Lookup with Add for 384 Bit CAM */
int
_npe_mem_cam_lookup_add_384(npe_mem_ptr_t p_cam,
                           uint32_t *out_result,
                           uint32_t value,
                           uint8_t entry_size)
{
    uint32_t match_index;

    switch(entry_size) {
       case NPE_CAM_ENTRY_8_BITS: {
           __xrw struct mem_cam_8bit data_result;

           data_result.search.value = value & 0xff;

           mem_cam_lookup_add(&data_result,
                             (__mem __align32 void *)p_cam,
                             384,
                             8);

           if((data_result.result.mask_lo & 0x7f) == 0x7f)
               *out_result = NPE_CAM_NOT_ADDED_CAM_FULL;

           else if(data_result.result.mask_lo & 0x80)  // Added
               *out_result = ((data_result.result.mask_lo & 0x7f) << 8) |
                              NPE_CAM_ADDED;
           else
               *out_result = ((data_result.result.mask_lo & 0x7f) << 8) |
                              NPE_CAM_MATCH;

           break;
       }
       case NPE_CAM_ENTRY_16_BITS: {
           __xrw struct mem_cam_16bit data_result;

           data_result.search.value = value & 0xffff;

           mem_cam_lookup_add(&data_result,
                             (__mem __align32 void *)p_cam,
                             384,
                             16);

           if(mem_cam_lookup_add_fail(data_result))
               *out_result = NPE_CAM_NOT_ADDED_CAM_FULL;

           else if(mem_cam_lookup_add_added(data_result))
               *out_result = ((data_result.result.match & 0x7f) << 8) |
                              NPE_CAM_ADDED;
           else
               *out_result = ((data_result.result.match & 0x7f) << 8) |
                              NPE_CAM_MATCH;
           break;
       }
       case NPE_CAM_ENTRY_24_BITS: {
           __xrw struct mem_cam_24bit data_result;

           data_result.search.value = value & 0xffffff;

           mem_cam_lookup_add(&data_result,
                             (__mem __align32 void *)p_cam,
                             384,
                             24);

           if(mem_cam_lookup_add_fail(data_result))
               *out_result = NPE_CAM_NOT_ADDED_CAM_FULL;

           else if(mem_cam_lookup_add_added(data_result))
               *out_result = ((data_result.result.match & 0x7f) << 8) |
                              NPE_CAM_ADDED;
           else
               *out_result = ((data_result.result.match & 0x7f) << 8) |
                              NPE_CAM_MATCH;
           break;
       }
       case NPE_CAM_ENTRY_32_BITS: {
           __xrw struct mem_cam_32bit data_result;

           data_result.search.value = value;

           mem_cam_lookup_add(&data_result,
                             (__mem __align32 void *)p_cam,
                             384,
                             32);

           if(mem_cam_lookup_add_fail(data_result))
               *out_result = NPE_CAM_NOT_ADDED_CAM_FULL;

           else if(mem_cam_lookup_add_added(data_result))
               *out_result = ((data_result.result.match & 0x7f) << 8) |
                              NPE_CAM_ADDED;
           else
               *out_result = ((data_result.result.match & 0x7f) << 8) |
                              NPE_CAM_MATCH;
           break;
       }
       default:
           return NPE_CAM_FAIL;
           break;
    }

    return NPE_CAM_OK;
}

/* Memory CAM Lookup for 512-bit CAM */
int
_npe_mem_cam_lookup_512(npe_mem_ptr_t p_cam,
                       uint32_t *out_result,
                       uint32_t value,
                       uint8_t entry_size)
{
    uint32_t match_index;

    switch(entry_size) {
       case NPE_CAM_ENTRY_8_BITS: {
           __xrw struct mem_cam_8bit data_result;

           data_result.search.value = value & 0xff;

           mem_cam_lookup(&data_result,
                         (__mem __align32 void *)p_cam,
                         512,
                         8);

           if(data_result.result.mask_lo || data_result.result.mask_hi) {

               if(data_result.result.mask_lo) {

                   match_index = _npe_ffs(data_result.result.mask_lo);

               } else {

                   match_index = 32 + _npe_ffs(data_result.result.mask_hi);
               }
               *out_result = (match_index << 8) | NPE_CAM_MATCH;
           }
           else
               *out_result = NPE_CAM_NO_MATCH;
           break;
       }
       case NPE_CAM_ENTRY_16_BITS: {
           __xrw struct mem_cam_16bit data_result;

           data_result.search.value = value & 0xffff;

           mem_cam_lookup(&data_result,
                         (__mem __align32 void *)p_cam,
                         512,
                         16);

           if(mem_cam_lookup_hit(data_result))
               *out_result = ((data_result.result.match & 0x7f) << 8) |
                              NPE_CAM_MATCH;
           else
               *out_result = NPE_CAM_NO_MATCH;
           break;
       }
       case NPE_CAM_ENTRY_24_BITS: {
           __xrw struct mem_cam_24bit data_result;

           data_result.search.value = value & 0xffffff;

           mem_cam_lookup(&data_result,
                         (__mem __align32 void *)p_cam,
                         512,
                         24);

           if(mem_cam_lookup_hit(data_result))
               *out_result = ((data_result.result.match & 0x7f) << 8) |
                              NPE_CAM_MATCH;
           else
               *out_result = NPE_CAM_NO_MATCH;
           break;
       }
       case NPE_CAM_ENTRY_32_BITS: {
           __xrw struct mem_cam_32bit data_result;

           data_result.search.value = value;

           mem_cam_lookup(&data_result,
                         (__mem __align32 void *)p_cam,
                         512,
                         32);

           if(mem_cam_lookup_hit(data_result))
               *out_result = ((data_result.result.match & 0x7f) << 8) |
                              NPE_CAM_MATCH;
           else
               *out_result = NPE_CAM_NO_MATCH;
           break;
       }
       default:
           return NPE_CAM_FAIL;
           break;
    }

    return NPE_CAM_OK;
}

/* Memory CAM Lookup with Add for 512 Bit CAM */
int
_npe_mem_cam_lookup_add_512(npe_mem_ptr_t p_cam,
                           uint32_t *out_result,
                           uint32_t value,
                           uint8_t entry_size)
{
    uint32_t match_index;

    switch(entry_size) {
       case NPE_CAM_ENTRY_8_BITS: {
           __xrw struct mem_cam_8bit data_result;

           data_result.search.value = value & 0xff;

           mem_cam_lookup_add(&data_result,
                             (__mem __align32 void *)p_cam,
                             512,
                             8);

           if((data_result.result.mask_lo & 0x7f) == 0x7f)
               *out_result = NPE_CAM_NOT_ADDED_CAM_FULL;

           else if(data_result.result.mask_lo & 0x80)  // Added
               *out_result = ((data_result.result.mask_lo & 0x7f) << 8) |
                              NPE_CAM_ADDED;
           else
               *out_result = ((data_result.result.mask_lo & 0x7f) << 8) |
                              NPE_CAM_MATCH;

           break;
       }
       case NPE_CAM_ENTRY_16_BITS: {
           __xrw struct mem_cam_16bit data_result;

           data_result.search.value = value & 0xffff;

           mem_cam_lookup_add(&data_result,
                             (__mem __align32 void *)p_cam,
                             512,
                             16);

           if(mem_cam_lookup_add_fail(data_result))
               *out_result = NPE_CAM_NOT_ADDED_CAM_FULL;

           else if(mem_cam_lookup_add_added(data_result))
               *out_result = ((data_result.result.match & 0x7f) << 8) |
                              NPE_CAM_ADDED;
           else
               *out_result = ((data_result.result.match & 0x7f) << 8) |
                              NPE_CAM_MATCH;
           break;
       }
       case NPE_CAM_ENTRY_24_BITS: {
           __xrw struct mem_cam_24bit data_result;

           data_result.search.value = value & 0xffffff;

           mem_cam_lookup_add(&data_result,
                             (__mem __align32 void *)p_cam,
                             512,
                             24);

           if(mem_cam_lookup_add_fail(data_result))
               *out_result = NPE_CAM_NOT_ADDED_CAM_FULL;

           else if(mem_cam_lookup_add_added(data_result))
               *out_result = ((data_result.result.match & 0x7f) << 8) |
                              NPE_CAM_ADDED;
           else
               *out_result = ((data_result.result.match & 0x7f) << 8) |
                              NPE_CAM_MATCH;
           break;
       }
       case NPE_CAM_ENTRY_32_BITS: {
           __xrw struct mem_cam_32bit data_result;

           data_result.search.value = value;

           mem_cam_lookup_add(&data_result,
                             (__mem __align32 void *)p_cam,
                             512,
                             32);

           if(mem_cam_lookup_add_fail(data_result))
               *out_result = NPE_CAM_NOT_ADDED_CAM_FULL;

           else if(mem_cam_lookup_add_added(data_result))
               *out_result = ((data_result.result.match & 0x7f) << 8) |
                              NPE_CAM_ADDED;
           else
               *out_result = ((data_result.result.match & 0x7f) << 8) |
                              NPE_CAM_MATCH;
           break;
       }
       default:
           return NPE_CAM_FAIL;
           break;
    }

    return NPE_CAM_OK;
}

#else

/* Non NFCC Code */
int
_npe_mem_cam_lookup_128(npe_mem_ptr_t p_cam,
                        uint32_t *out_result,
                        uint32_t value,
                        uint8_t entry_size)
{
    uint32_t i;
    npe_cam_128_t *cam;

    cam = (npe_cam_128_t *)p_cam;
    *out_result = NPE_CAM_NO_MATCH;

    switch(entry_size) {
        case NPE_CAM_ENTRY_8_BITS:

           for(i=0; i<16; i++) {
               if(cam->val8[i] == (value & 0xff)) {
                   *out_result = ((i << 8) | NPE_CAM_MATCH);
                   break;
               }
           }
           break;

        case NPE_CAM_ENTRY_16_BITS:

           for(i=0; i<8; i++) {
               if(cam->val16[i] == (value & 0xffff)) {
                   *out_result = ((i << 8) | NPE_CAM_MATCH);
                   break;
               }
           }
           break;

        case NPE_CAM_ENTRY_24_BITS:

           for(i=0; i<4; i++) {
               if((cam->val24[i] & 0xffffff) == (value & 0xffffff)) {
                   *out_result = ((i << 8) | NPE_CAM_MATCH);
                   break;
               }
           }
           break;

        case NPE_CAM_ENTRY_32_BITS:

           for(i=0; i<4; i++) {
               if(cam->val32[i] == value )  {
                   *out_result = ((i << 8) | NPE_CAM_MATCH);
                   break;
               }
           }
           break;

        default:
           return NPE_CAM_FAIL;
    }

    return NPE_CAM_OK;
}


int
_npe_mem_cam_lookup_256(npe_mem_ptr_t p_cam,
                        uint32_t *out_result,
                        uint32_t value,
                        uint8_t entry_size)
{
    uint32_t i;
    npe_cam_256_t *cam;

    cam = (npe_cam_256_t *)p_cam;
    *out_result = NPE_CAM_NO_MATCH;

    switch(entry_size) {
        case NPE_CAM_ENTRY_8_BITS:

           for(i=0; i<32; i++) {
               if(cam->val8[i] == (value & 0xff)) {
                   *out_result = ((i << 8) | NPE_CAM_MATCH);
                   break;
               }
           }
           break;

        case NPE_CAM_ENTRY_16_BITS:

           for(i=0; i<16; i++) {
               if(cam->val16[i] == (value & 0xffff)) {
                   *out_result = ((i << 8) | NPE_CAM_MATCH);
                   break;
               }
           }
           break;

        case NPE_CAM_ENTRY_24_BITS:

           for(i=0; i<8; i++) {
               if((cam->val24[i] & 0xffffff) == (value & 0xffffff)) {
                   *out_result = ((i << 8) | NPE_CAM_MATCH);
                   break;
               }
           }
           break;

        case NPE_CAM_ENTRY_32_BITS:

           for(i=0; i<8; i++) {
               if(cam->val32[i] == value )  {
                   *out_result = ((i << 8) | NPE_CAM_MATCH);
                   break;
               }
           }
           break;

        default:
           return NPE_CAM_FAIL;
    }

    return NPE_CAM_OK;
}

int
_npe_mem_cam_lookup_384(npe_mem_ptr_t p_cam,
                        uint32_t *out_result,
                        uint32_t value,
                        uint8_t entry_size)
{
    uint32_t i;
    npe_cam_384_t *cam;

    cam = (npe_cam_384_t *)p_cam;
    *out_result = NPE_CAM_NO_MATCH;

    switch(entry_size) {
        case NPE_CAM_ENTRY_8_BITS:

           for(i=0; i<48; i++) {
               if(cam->val8[i] == (value & 0xff)) {
                   *out_result = ((i << 8) | NPE_CAM_MATCH);
                   break;
               }
           }
           break;

        case NPE_CAM_ENTRY_16_BITS:

           for(i=0; i<24; i++) {
               if(cam->val16[i] == (value & 0xffff)) {
                   *out_result = ((i << 8) | NPE_CAM_MATCH);
                   break;
               }
           }
           break;

        case NPE_CAM_ENTRY_24_BITS:

           for(i=0; i<12; i++) {
               if((cam->val24[i] & 0xffffff) == (value & 0xffffff)) {
                   *out_result = ((i << 8) | NPE_CAM_MATCH);
                   break;
               }
           }
           break;

        case NPE_CAM_ENTRY_32_BITS:

           for(i=0; i<12; i++) {
               if(cam->val32[i] == value )  {
                   *out_result = ((i << 8) | NPE_CAM_MATCH);
                   break;
               }
           }
           break;

        default:
           return NPE_CAM_FAIL;
    }

    return NPE_CAM_OK;
}

int
_npe_mem_cam_lookup_512(npe_mem_ptr_t p_cam,
                        uint32_t *out_result,
                        uint32_t value,
                        uint8_t entry_size)
{
    uint32_t i;
    npe_cam_512_t *cam;

    cam = (npe_cam_512_t *)p_cam;
    *out_result = NPE_CAM_NO_MATCH;

    switch(entry_size) {
        case NPE_CAM_ENTRY_8_BITS:

           for(i=0; i<64; i++) {
               if(cam->val8[i] == (value & 0xff)) {
                   *out_result = ((i << 8) | NPE_CAM_MATCH);
                   break;
               }
           }
           break;

        case NPE_CAM_ENTRY_16_BITS:

           for(i=0; i<32; i++) {
               if(cam->val16[i] == (value & 0xffff)) {
                   *out_result = ((i << 8) | NPE_CAM_MATCH);
                   break;
               }
           }
           break;

        case NPE_CAM_ENTRY_24_BITS:

           for(i=0; i<16; i++) {
               if((cam->val24[i] & 0xffffff) == (value & 0xffffff)) {
                   *out_result = ((i << 8) | NPE_CAM_MATCH);
                   break;
               }
           }
           break;

        case NPE_CAM_ENTRY_32_BITS:

           for(i=0; i<16; i++) {
               if(cam->val32[i] == value )  {
                   *out_result = ((i << 8) | NPE_CAM_MATCH);
                   break;
               }
           }
           break;

        default:
           return NPE_CAM_FAIL;
    }

    return NPE_CAM_OK;
}

int
_npe_mem_cam_lookup_add_128(npe_mem_ptr_t p_cam,
                           uint32_t *out_result,
                           uint32_t value,
                           uint8_t entry_size)
{
    uint32_t i;
    npe_cam_128_t *cam;

    cam = (npe_cam_128_t *)p_cam;
    *out_result = NPE_CAM_NO_MATCH;

    switch(entry_size) {
        case NPE_CAM_ENTRY_8_BITS:

           for(i=0; i<16; i++) {
               if(cam->val8[i] == (value & 0xff)) {
                   *out_result = ((i << 8) | NPE_CAM_MATCH);
                   return NPE_CAM_OK;
               }
           }

           for(i=0; i<16; i++) {
               if(cam->val8[i] == 0) {
                   cam->val8[i] = value & 0xff;
                   *out_result = ((i << 8) | NPE_CAM_ADDED);
                   return NPE_CAM_OK;
               }
           }

           if( i == 16)
               *out_result = NPE_CAM_NOT_ADDED_CAM_FULL;

           break;

        case NPE_CAM_ENTRY_16_BITS:

           for(i=0; i<8; i++) {
               if(cam->val16[i] == (value & 0xffff)) {
                   *out_result = ((i << 8) | NPE_CAM_MATCH);
                   return NPE_CAM_OK;
               }
           }

           for(i=0; i<8; i++) {
               if(cam->val16[i] == 0) {
                   cam->val16[i] = value & 0xffff;
                   *out_result = ((i << 8) | NPE_CAM_ADDED);
                   return NPE_CAM_OK;
               }
           }

           if( i == 8)
               *out_result = NPE_CAM_NOT_ADDED_CAM_FULL;

           break;

        case NPE_CAM_ENTRY_24_BITS:

           for(i=0; i<4; i++) {
               if((cam->val24[i] & 0xffffff) == (value & 0xffffff)) {
                   *out_result = ((i << 8) | NPE_CAM_MATCH);
                   return NPE_CAM_OK;
               }
           }

           for(i=0; i<4; i++) {
               if((cam->val24[i] & 0xffffff) == 0) {
                   cam->val24[i] = value & 0xffffff;
                   *out_result = ((i << 8) | NPE_CAM_ADDED);
                   return NPE_CAM_OK;
               }
           }

           if( i == 4)
               *out_result = NPE_CAM_NOT_ADDED_CAM_FULL;

           break;

        case NPE_CAM_ENTRY_32_BITS:

           for(i=0; i<4; i++) {
               if(cam->val32[i] == value )  {
                   *out_result = ((i << 8) | NPE_CAM_MATCH);
                   return NPE_CAM_OK;
               }
           }

           for(i=0; i<4; i++) {
               if(cam->val32[i] == 0) {
                   cam->val32[i] = value;
                   *out_result = ((i << 8) | NPE_CAM_ADDED);
                   return NPE_CAM_OK;
               }
           }

           if( i == 4)
               *out_result = NPE_CAM_NOT_ADDED_CAM_FULL;

           break;

        default:
            return NPE_CAM_FAIL;
    }

    return NPE_CAM_OK;
}


int
_npe_mem_cam_lookup_add_256(npe_mem_ptr_t p_cam,
                           uint32_t *out_result,
                           uint32_t value,
                           uint8_t entry_size)
{
    uint32_t i;
    npe_cam_256_t *cam;

    cam = (npe_cam_256_t *)p_cam;
    *out_result = NPE_CAM_NO_MATCH;

    switch(entry_size) {
        case NPE_CAM_ENTRY_8_BITS:

           for(i=0; i<32; i++) {
               if(cam->val8[i] == (value & 0xff)) {
                   *out_result = ((i << 8) | NPE_CAM_MATCH);
                   return NPE_CAM_OK;
               }
           }

           for(i=0; i<32; i++) {
               if(cam->val8[i] == 0) {
                   cam->val8[i] = value & 0xff;
                   *out_result = ((i << 8) | NPE_CAM_ADDED);
                   return NPE_CAM_OK;
               }
           }

           if( i == 32)
               *out_result = NPE_CAM_NOT_ADDED_CAM_FULL;

           break;

        case NPE_CAM_ENTRY_16_BITS:

           for(i=0; i<16; i++) {
               if(cam->val16[i] == (value & 0xffff)) {
                   *out_result = ((i << 8) | NPE_CAM_MATCH);
                   return NPE_CAM_OK;
               }
           }

           for(i=0; i<16; i++) {
               if(cam->val16[i] == 0) {
                   cam->val16[i] = value & 0xffff;
                   *out_result = ((i << 8) | NPE_CAM_ADDED);
                   return NPE_CAM_OK;
               }
           }

           if( i == 16)
               *out_result = NPE_CAM_NOT_ADDED_CAM_FULL;

           break;

        case NPE_CAM_ENTRY_24_BITS:

           for(i=0; i<8; i++) {
               if((cam->val24[i] & 0xffffff) == (value & 0xffffff)) {
                   *out_result = ((i << 8) | NPE_CAM_MATCH);
                   return NPE_CAM_OK;
               }
           }

           for(i=0; i<8; i++) {
               if((cam->val24[i] & 0xffffff) == 0) {
                   cam->val24[i] = value & 0xffffff;
                   *out_result = ((i << 8) | NPE_CAM_ADDED);
                   return NPE_CAM_OK;
               }
           }

           if( i == 8)
               *out_result = NPE_CAM_NOT_ADDED_CAM_FULL;

           break;

        case NPE_CAM_ENTRY_32_BITS:

           for(i=0; i<8; i++) {
               if(cam->val32[i] == value )  {
                   *out_result = ((i << 8) | NPE_CAM_MATCH);
                   return NPE_CAM_OK;
               }
           }

           for(i=0; i<8; i++) {
               if(cam->val32[i] == 0) {
                   cam->val32[i] = value;
                   *out_result = ((i << 8) | NPE_CAM_ADDED);
                   return NPE_CAM_OK;
               }
           }

           if( i == 8)
               *out_result = NPE_CAM_NOT_ADDED_CAM_FULL;

           break;

        default:
            return NPE_CAM_FAIL;
    }

    return NPE_CAM_OK;
}

int
_npe_mem_cam_lookup_add_384(npe_mem_ptr_t p_cam,
                           uint32_t *out_result,
                           uint32_t value,
                           uint8_t entry_size)
{
    uint32_t i;
    npe_cam_384_t *cam;

    cam = (npe_cam_384_t *)p_cam;
    *out_result = NPE_CAM_NO_MATCH;

    switch(entry_size) {
        case NPE_CAM_ENTRY_8_BITS:

           for(i=0; i<48; i++) {
               if(cam->val8[i] == (value & 0xff)) {
                   *out_result = ((i << 8) | NPE_CAM_MATCH);
                   return NPE_CAM_OK;
               }
           }

           for(i=0; i<48; i++) {
               if(cam->val8[i] == 0) {
                   cam->val8[i] = value & 0xff;
                   *out_result = ((i << 8) | NPE_CAM_ADDED);
                   return NPE_CAM_OK;
               }
           }

           if( i == 48)
               *out_result = NPE_CAM_NOT_ADDED_CAM_FULL;

           break;

        case NPE_CAM_ENTRY_16_BITS:

           for(i=0; i<24; i++) {
               if(cam->val16[i] == (value & 0xffff)) {
                   *out_result = ((i << 8) | NPE_CAM_MATCH);
                   return NPE_CAM_OK;
               }
           }

           for(i=0; i<24; i++) {
               if(cam->val16[i] == 0) {
                   cam->val16[i] = value & 0xffff;
                   *out_result = ((i << 8) | NPE_CAM_ADDED);
                   return NPE_CAM_OK;
               }
           }

           if( i == 24)
               *out_result = NPE_CAM_NOT_ADDED_CAM_FULL;

           break;

        case NPE_CAM_ENTRY_24_BITS:

           for(i=0; i<12; i++) {
               if((cam->val24[i] & 0xffffff) == (value & 0xffffff)) {
                   *out_result = ((i << 8) | NPE_CAM_MATCH);
                   return NPE_CAM_OK;
               }
           }

           for(i=0; i<12; i++) {
               if((cam->val24[i] & 0xffffff) == 0) {
                   cam->val24[i] = value & 0xffffff;
                   *out_result = ((i << 8) | NPE_CAM_ADDED);
                   return NPE_CAM_OK;
               }
           }

           if( i == 12)
               *out_result = NPE_CAM_NOT_ADDED_CAM_FULL;

           break;

        case NPE_CAM_ENTRY_32_BITS:

           for(i=0; i<12; i++) {
               if(cam->val32[i] == value )  {
                   *out_result = ((i << 8) | NPE_CAM_MATCH);
                   return NPE_CAM_OK;
               }
           }

           for(i=0; i<12; i++) {
               if(cam->val32[i] == 0) {
                   cam->val32[i] = value;
                   *out_result = ((i << 8) | NPE_CAM_ADDED);
                   return NPE_CAM_OK;
               }
           }

           if( i == 12)
               *out_result = NPE_CAM_NOT_ADDED_CAM_FULL;

           break;

        default:
            return NPE_CAM_FAIL;
    }

    return NPE_CAM_OK;
}

int
_npe_mem_cam_lookup_add_512(npe_mem_ptr_t p_cam,
                           uint32_t *out_result,
                           uint32_t value,
                           uint8_t entry_size)
{
    uint32_t i;
    npe_cam_512_t *cam;

    cam = (npe_cam_512_t *)p_cam;
    *out_result = NPE_CAM_NO_MATCH;

    switch(entry_size) {
        case NPE_CAM_ENTRY_8_BITS:

           for(i=0; i<64; i++) {
               if(cam->val8[i] == (value & 0xff)) {
                   *out_result = ((i << 8) | NPE_CAM_MATCH);
                   return NPE_CAM_OK;
               }
           }

           for(i=0; i<64; i++) {
               if(cam->val8[i] == 0) {
                   cam->val8[i] = value & 0xff;
                   *out_result = ((i << 8) | NPE_CAM_ADDED);
                   return NPE_CAM_OK;
               }
           }

           if( i == 64)
               *out_result = NPE_CAM_NOT_ADDED_CAM_FULL;

           break;

        case NPE_CAM_ENTRY_16_BITS:

           for(i=0; i<32; i++) {
               if(cam->val16[i] == (value & 0xffff)) {
                   *out_result = ((i << 8) | NPE_CAM_MATCH);
                   return NPE_CAM_OK;
               }
           }

           for(i=0; i<32; i++) {
               if(cam->val16[i] == 0) {
                   cam->val16[i] = value & 0xffff;
                   *out_result = ((i << 8) | NPE_CAM_ADDED);
                   return NPE_CAM_OK;
               }
           }

           if( i == 32)
               *out_result = NPE_CAM_NOT_ADDED_CAM_FULL;

           break;

        case NPE_CAM_ENTRY_24_BITS:

           for(i=0; i<16; i++) {
               if((cam->val24[i] & 0xffffff) == (value & 0xffffff)) {
                   *out_result = ((i << 8) | NPE_CAM_MATCH);
                   return NPE_CAM_OK;
               }
           }

           for(i=0; i<16; i++) {
               if((cam->val24[i] & 0xffffff) == 0) {
                   cam->val24[i] = value & 0xffffff;
                   *out_result = ((i << 8) | NPE_CAM_ADDED);
                   return NPE_CAM_OK;
               }
           }

           if( i == 16)
               *out_result = NPE_CAM_NOT_ADDED_CAM_FULL;

           break;

        case NPE_CAM_ENTRY_32_BITS:

           for(i=0; i<16; i++) {
               if(cam->val32[i] == value )  {
                   *out_result = ((i << 8) | NPE_CAM_MATCH);
                   return NPE_CAM_OK;
               }
           }

           for(i=0; i<16; i++) {
               if(cam->val32[i] == 0) {
                   cam->val32[i] = value;
                   *out_result = ((i << 8) | NPE_CAM_ADDED);
                   return NPE_CAM_OK;
               }
           }

           if( i == 16)
               *out_result = NPE_CAM_NOT_ADDED_CAM_FULL;

           break;

        default:
            return NPE_CAM_FAIL;
    }

    return NPE_CAM_OK;
}

#endif /* __NFP_TOOL_NFCC */

/* Memory CAM Lookup */
int
npe_mem_cam_lookup(npe_mem_ptr_t p_cam,
                   uint32_t *out_result,
                   uint32_t value,
                   uint32_t cam_size,
                   uint8_t entry_size)
{
    int ret;

    switch(cam_size) {
        case NPE_CAM_128_BITS:
            ret = _npe_mem_cam_lookup_128(p_cam, out_result, value, entry_size) ;
            break;
        case NPE_CAM_256_BITS:
            ret = _npe_mem_cam_lookup_256(p_cam, out_result, value, entry_size) ;
            break;
        case NPE_CAM_384_BITS:
            ret = _npe_mem_cam_lookup_384(p_cam, out_result, value, entry_size) ;
            break;
        case NPE_CAM_512_BITS:
            ret = _npe_mem_cam_lookup_512(p_cam, out_result, value, entry_size) ;
            break;
        default:
            ret = NPE_CAM_FAIL;
            break;
    }

    return ret;
}

/* Memory CAM Lookup with add */
int
npe_mem_cam_lookup_add(npe_mem_ptr_t p_cam,
                       uint32_t *out_result,
                       uint32_t value,
                       uint32_t cam_size,
                       uint8_t entry_size)
{
    int ret;

    switch(cam_size) {
        case NPE_CAM_128_BITS:
            ret = _npe_mem_cam_lookup_add_128(p_cam, out_result, value, entry_size) ;
            break;
        case NPE_CAM_256_BITS:
            ret = _npe_mem_cam_lookup_add_256(p_cam, out_result, value, entry_size) ;
            break;
        case NPE_CAM_384_BITS:
            ret = _npe_mem_cam_lookup_add_384(p_cam, out_result, value, entry_size) ;
            break;
        case NPE_CAM_512_BITS:
            ret = _npe_mem_cam_lookup_add_512(p_cam, out_result, value, entry_size) ;
            break;
        default:
            ret = NPE_CAM_FAIL;
            break;
    }

    return ret;
}
