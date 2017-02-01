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
 * @file          include/npe_gcc_types.h
 * @brief         Standard types for GCC
 */

/* Type and macro definitions */

#ifndef __NPE_GCC_TYPES_H__
#define __NPE_GCC_TYPES_H__

#include <stdint.h>
#include <stdio.h>


/** Memory address type
 */
typedef unsigned long long npe_mem_addr_t;


/** Memory pointer type
 */
typedef char * npe_mem_ptr_t;


/** Atomic access memory address type
 */
#ifndef __STDC_NO_ATOMICS__
typedef _Atomic unsigned long long npe_atomic_mem_addr_t;
#else
typedef unsigned long long npe_atomic_mem_addr_t;
#endif


/** Atomic access memory pointer type
 */
#ifndef __STDC_NO_ATOMICS__
typedef _Atomic char * npe_atomic_mem_ptr_t;
#else
typedef char * npe_atomic_mem_ptr_t;
#endif


/** Macros to declare memory sections
 */
#define NPE_DECLARE_MEM8(_name) uint8_t __npe_##_name;

#define NPE_DECLARE_MEM32(_name) uint32_t __npe_##_name;

#define NPE_DECLARE_MEM64(_name) uint64_t __npe_##_name;

#define NPE_DECLARE_MEM8_ARRAY(_name, _num_elements) \
    uint8_t __npe_##_name[(_num_elements)];

#define NPE_DECLARE_MEM32_ARRAY(_name, _num_elements) \
    uint32_t __npe_##_name[(_num_elements)];

#define NPE_DECLARE_MEM64_ARRAY(_name, _num_elements) \
    uint64_t __npe_##_name[(_num_elements)];

/** Macros to declare atomic memory sections
 */
#ifndef __STDC_NO_ATOMICS__

#define NPE_DECLARE_ATOMIC_MEM32(_name) _Atomic uint32_t __npe_##_name;

#define NPE_DECLARE_ATOMIC_MEM64(_name) _Atomic uint64_t __npe_##_name;

#define NPE_DECLARE_ATOMIC_MEM32_ARRAY(_name, _num_elements) \
    _Atomic uint32_t __npe_##_name[(_num_elements)];

#define NPE_DECLARE_ATOMIC_MEM64_ARRAY(_name, _num_elements) \
    _Atomic uint64_t __npe_##_name[(_num_elements)];

#else

#define NPE_DECLARE_ATOMIC_MEM32(_name) uint32_t __npe_##_name;

#define NPE_DECLARE_ATOMIC_MEM64(_name) uint64_t __npe_##_name;

#define NPE_DECLARE_ATOMIC_MEM32_ARRAY(_name, _num_elements) \
    uint32_t __npe_##_name[(_num_elements)];

#define NPE_DECLARE_ATOMIC_MEM64_ARRAY(_name, _num_elements) \
    uint64_t __npe_##_name[(_num_elements)];

#endif


/** Macro to declare an NFCC 'intrinsic' function
 */
#define __NPE_INTRINSIC


#endif /* __NPE_GCC_TYPES_H__ */
