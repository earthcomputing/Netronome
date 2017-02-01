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
 * @file          include/npe_nfcc_types.h
 * @brief         Standard types for NFCC
 */

/* Type and macro definitions */

#ifndef __NPE_NFCC_TYPES_H__
#define __NPE_NFCC_TYPES_H__

/* Compilation for Netronome NFP */
#ifndef NFP_LIB_ANY_NFCC_VERSION
    #if (__NFP_TOOL_NFCC < NFP_SW_VERSION(5, 0, 0, 0)) ||   \
        (__NFP_TOOL_NFCC > NFP_SW_VERSION(6, 255, 255, 255)))
        #error "This software is not supported for the version of the SDK currently in use."
    #endif
#endif

#include <nfp.h>
#include <stdint.h>


/** Memory address type
 */
typedef __declspec(mem, addr40) npe_mem_addr_t;


/** Memory pointer type
 */
typedef __declspec(mem, addr40) char * npe_mem_ptr_t;


/** Atomic access memory address type
 */
typedef __declspec(mem, addr40) npe_atomic_mem_addr_t;


/** Atomic access memory pointer type
 */
typedef __declspec(mem, addr40) char * npe_atomic_mem_ptr_t;


/** Macros to declare memory sections
 */
#define NPE_DECLARE_MEM8(_name) \
    __declspec(shared, mem, aligned(1)) uint8_t __npe_##_name;

#define NPE_DECLARE_MEM32(_name) \
    __declspec(shared, mem, aligned(4)) uint32_t __npe_##_name;

#define NPE_DECLARE_MEM64(_name) \
    __declspec(shared, mem, aligned(8)) uint64_t __npe_##_name;

#define NPE_DECLARE_MEM8_ARRAY(_name, _num_elements) \
   __declspec(shared, mem, aligned(1)) uint8_t __npe_##_name[(_num_elements)];

#define NPE_DECLARE_MEM32_ARRAY(_name, _num_elements) \
   __declspec(shared, mem, aligned(4)) uint32_t __npe_##_name[(_num_elements)];

#define NPE_DECLARE_MEM64_ARRAY(_name, _num_elements) \
   __declspec(shared, mem, aligned(8)) uint64_t __npe_##_name[(_num_elements)];

/** Macros to declare atomic memory sections
 */
#define NPE_DECLARE_ATOMIC_MEM32(_name) \
     __declspec(shared, mem, aligned(4)) uint32_t __npe_##_name;

#define NPE_DECLARE_ATOMIC_MEM64(_name) \
     __declspec(shared, mem, aligned(8)) uint64_t __npe_##_name;

#define NPE_DECLARE_ATOMIC_MEM32_ARRAY(_name, _num_elements) \
   __declspec(shared, mem, aligned(4)) uint32_t __npe_##_name[(_num_elements)];

#define NPE_DECLARE_ATOMIC_MEM64_ARRAY(_name, _num_elements) \
   __declspec(shared, mem, aligned(8)) uint64_t __npe_##_name[(_num_elements)];


/** Macro to declare an NFCC 'intrinsic' function
 */
#define __NPE_INTRINSIC __intrinsic


#endif /* __NPE_NFCC_TYPES_H__ */
