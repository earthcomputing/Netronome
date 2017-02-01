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
 * @file          include/npe_common_types.h
 * @brief         Standard types
 */

/* Type and macro definitions */

#ifndef __NPE_COMMON_TYPES_H__
#define __NPE_COMMON_TYPES_H__


/** Endian Selection
 */
enum npe_endian {
    NPE_BIG_ENDIAN    = 0x00,  /**< Big endian byte ordering */
    NPE_LITTLE_ENDIAN = 0x01,  /**< Little endian byte ordering */
};


/** Bit Swap Enable/Disable
 */
enum npe_bit_swap {
    NPE_BIT_SWAP_DISABLE = 0x00, /**< Disable bit swapping */
    NPE_BIT_SWAP_ENABLE  = 0x01, /**< Enable bit swapping */
};


/** Macro returns memory address from name
 */
#define NPE_GET_MEM_ADDR(_name) \
    ((npe_mem_addr_t)(&__npe_##_name[0]))

/** Macro returns memory address pointer from name
 */
#define NPE_GET_MEM_ADDR_PTR(_name) \
    ((npe_mem_ptr_t)(&__npe_##_name))

#define NPE_GET_ATOMIC_MEM_ADDR_PTR(_name) \
    ((npe_atomic_mem_ptr_t)(&__npe_##_name))


#endif /* __NPE_COMMON_TYPES_H__ */
