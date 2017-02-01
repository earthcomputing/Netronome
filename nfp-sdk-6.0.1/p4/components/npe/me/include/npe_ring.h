/*
 * Copyright (C) 2012-2016,  Netronome Systems, Inc.  All rights reserved.
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

/* Ring operations */

#ifndef __NPE_RING_H__
#define __NPE_RING_H__

/* Compilation for Netronome NFP */
#if defined(__NFP_TOOL_NFCC)

    #ifndef NFP_LIB_ANY_NFCC_VERSION
        #if (__NFP_TOOL_NFCC < NFP_SW_VERSION(5, 0, 0, 0)) ||   \
            (__NFP_TOOL_NFCC > NFP_SW_VERSION(6, 255, 255, 255))
            #error "This software is not supported for the version of the SDK currently in use."
         #endif
    #endif
#endif


#if defined(__NFP_TOOL_NFCC)
#include <nfp.h>
#include <mem_ring.h>
#endif

#include <stdint.h>
#include <npe_types.h>


/** @file npe_ring.h
 * @addtogroup ring  Ring Operations
 * @{
 */


/** @name Ring related functions
 *  @{
 */


#ifndef __NFP_TOOL_NFCC
#include <pthread.h>

/* struct for emulating nfp mu rings with gcc */

struct npe_ring {

    /* pointer to next npe_ring struct
       in linked list used for maintaining
       multiple npe_rings. */
    struct npe_ring *next;

    /* Text string containing ring 'name'.
       Doing this because NFCC uses a ring 'name' to
       reference the ring, which is used by the linker
       to allocate the ring memory. So for GCC
       compatibility need to use a 'name' to reference
       the ring. */
    char name[32];

    /* lock for this ring */
    pthread_mutex_t mtx;

    /* pointer to ring memory */
    uint32_t *mem;

    /* ring control vars. All reference lwords */
    uint32_t head;
    uint32_t tail;
    uint32_t size;
    uint32_t free;
};

/* allocate memory for a new npe_ring struct,
   initialize it's 'name' and mutex, allocate
   memory for the ring data, and add a pointer to
   npe_ring struct in the npe_ring struct linked
   list */
void npe_ring_init(const char *name, const uint32_t size);


/* note there is no support for 'deallocating' an
   npe_ring, in either NFCC or GCC. in NFCC rings are
   allocated at link time, not at run time, and the
   GCC emulation gives a similar appearance */


/* return address of npe_ring struct with name 'name' */
struct npe_ring * npe_get_ring_addr_gcc(const char *name);

#endif


/** Definitions for Ring size constants (byte count)
    for use with NPE_MEM_RING_INIT()
*/
#define NPE_RING_SIZE_2K   (2048<<0)  /**< 2KB   */
#define NPE_RING_SIZE_4K   (2048<<1)  /**< 4KB   */
#define NPE_RING_SIZE_8K   (2048<<2)  /**< 8KB   */
#define NPE_RING_SIZE_16K  (2048<<3)  /**< 16KB  */
#define NPE_RING_SIZE_32K  (2048<<4)  /**< 32KB  */
#define NPE_RING_SIZE_64K  (2048<<5)  /**< 64KB  */
#define NPE_RING_SIZE_128K (2048<<6)  /**< 128KB */
#define NPE_RING_SIZE_256K (2048<<7)  /**< 256KB */
#define NPE_RING_SIZE_512K (2048<<8)  /**< 512KB */
#define NPE_RING_SIZE_1M   (2048<<9)  /**< 1MB   */
#define NPE_RING_SIZE_2M   (2048<<10) /**< 2MB   */
#define NPE_RING_SIZE_4M   (2048<<11) /**< 4MB   */
#define NPE_RING_SIZE_8M   (2048<<12) /**< 8MB   */
#define NPE_RING_SIZE_16M  (2048<<13) /**< 16MB  */
#define NPE_RING_SIZE_32M  (2048<<14) /**< 32MB  */
#define NPE_RING_SIZE_64M  (2048<<15) /**< 64MB  */

/** Macro to declare and init a memory ring structure
 * @param _name   Name of the ring memory
 * @param _size   Size of the ring (one of NPE_RING_SIZE_xxx)
 * @param _mu     Number of memory unit to put the ring in (0,1, or 2)
 */
#if defined(__NFP_TOOL_NFCC)
#define NPE_MEM_RING_INIT(_name, _size, _mu) \
    MEM_RING_INIT_MU(_name, _size, emem##_mu)
#else /* GCC */
#define NPE_MEM_RING_INIT(_name, _size, _mu) \
    npe_ring_init(#_name, _size);
#endif

/** Macro to get unique ring number of a ring
 * @param _name   Name of the ring memory
 */
#if defined(__NFP_TOOL_NFCC)
#define NPE_MEM_RING_NUM(_name) MEM_RING_GET_NUM(_name)
#else /* GCC */
#define NPE_MEM_RING_NUM(_name) 0
#endif

/** Macro to get memory address of a ring
 *  For NFP this is the upper 8 bits of the 40 bit address of the ring memory
 *  For GCC this is the address of the npe_ring struct
 * @param _name   Name of the ring memory
 */
#if defined(__NFP_TOOL_NFCC)
#define NPE_MEM_RING_ADDR(_name) MEM_RING_GET_MEMADDR(_name)
#else /* GCC */
#define NPE_MEM_RING_ADDR(_name) (uint64_t)npe_get_ring_addr_gcc(#_name)
#endif




/**
 * Add data stored in 'local' memory to the tail of a memory ring and
 * return number of bytes already on ring.  Leave ring as is and return
 * -1 if the ring was too full to complete the request or another error
 * occurred.
 *
 * @b Example:
 * @code
 * uint32_t ring_num;
 * uint64_t ring_addr;
 * NPE_MEM_RING_INIT(ringA, NPE_RING_SIZE_2K, 0)
 * ring_num = NPE_MEM_RING_NUM(ringA);
 * ring_addr = NPE_MEM_RING_ADDR(ringA);
 * __declspec(gp_reg) uint32_t reg[4];
 * uint32_t num_bytes;
 * reg[0] = 0x00;
 * reg[1] = 0x04;
 * reg[2] = 0x08;
 * reg[3] = 0x0C;
 * num_bytes = npe_ring_put(ring_num,
 *                          ring_addr,
 *                          reg,
 *                          sizeof(reg));
 * @endcode
 *
 * @param ring_num  Ring number. For NFCC / NFP this in 0 through 1023
 *                  and is generated using the NPE_MEM_RING_NUM() macro
 * @param ring_addr Ring memory address. For NFCC / NFP this is generated
 *                  using the NPE_MEM_RING_ADDR() macro and contains
 *                  bits 39:32 of the 40 bit memory address on bits 31:24
 *                  in the ring_addr param
 * @param data      Pointer to data to put to ring.
 *                  For NFCC this is NFP lm, gpr, nn_reg, xfr_reg
 * @param size      Size of data to put to ring
 *                  (in bytes, 4 byte multiple, maximum 64)
 */
__NPE_INTRINSIC int npe_ring_put(uint32_t ring_num,
                                 uint64_t ring_addr,
                                 void *data,
                                 size_t size);


/**
 * Add data stored in 'local' memory to the tail of a memory ring. If
 * ring is full or becomes full as a result of the command, old data
 * in the ring is overwritten.
 *
 * @b Example:
 * @code
 * uint32_t ring_num;
 * uint64_t ring_addr;
 * NPE_MEM_RING_INIT(ringA, NPE_RING_SIZE_2K, 0)
 * ring_num = NPE_MEM_RING_NUM(ringA);
 * ring_addr = NPE_MEM_RING_ADDR(ringA);
 * __declspec(gp_reg) uint32_t reg[4];
 * uint32_t num_bytes;
 * reg[0] = 0x00;
 * reg[1] = 0x04;
 * reg[2] = 0x08;
 * reg[3] = 0x0C;
 * npe_ring_journal(ring_num,
 *                  ring_addr,
 *                  reg,
 *                  sizeof(reg));
 * @endcode
 *
 * @param ring_num  Ring number. For NFCC / NFP this is 0 through 1023
 *                  and is generated using the NPE_MEM_RING_NUM() macro
 * @param ring_addr Ring memory address. For NFCC / NFP this is generated
 *                  using the NPE_MEM_RING_ADDR() macro and contains
 *                  bits 39:32 of the 40 bit memory address on bits 31:24
 *                  in the ring_addr param
 * @param data      Pointer to data to add to ring.
 *                  For NFCC this is NFP lm, gpr, nn_reg, xfr_reg
 * @param size      Size of data to put to ring
 *                  (in bytes, 4 byte multiple, maximum 64)
 */
__NPE_INTRINSIC void npe_ring_journal(uint32_t ring_num,
                                      uint64_t ring_addr,
                                      void *data,
                                      size_t size);


/**
 * Remove ring data from head of a memory ring and store it in
 * 'local' memory. Return 0 on success or -1 if there are not
 * enough words in the ring to complete the request.
 *
 * @b Example:
 * @code
 * uint32_t ring_num;
 * uint64_t ring_addr;
 * NPE_MEM_RING_INIT(ringA, NPE_RING_SIZE_2K, 0)
 * ring_num = NPE_MEM_RING_NUM(ringA);
 * ring_addr = NPE_MEM_RING_ADDR(ringA);
 * __declspec(gp_reg) uint32_t regw[4];
 * __declspec(gp_reg) uint32_t regr[4];
 * regw[0] = 0x00;
 * regw[1] = 0x04;
 * regw[2] = 0x08;
 * regw[3] = 0x0C;
 * uint32_t num_bytes, status;
 * num_bytes = npe_ring_put(ring_num,
 *                          ring_addr,
 *                          regw,
 *                          sizeof(regw));
 *
 * status = npe_ring_get(ring_num,
 *                       ring_addr,
 *                       regr,
 *                       sizeof(regr));
 * @endcode
 *
 * @param ring_num  Ring number. For NFCC / NFP this is 0 through 1023
 *                  and is generated using the NPE_MEM_RING_NUM() macro
 * @param ring_addr Ring memory address. For NFCC / NFP this is generated
 *                  using the NPE_MEM_RING_ADDR() macro and contains
 *                  bits 39:32 of the 40 bit memory address on bits 31:24
 *                  in the ring_addr param
 * @param data      Pointer to 'local' memory to load with ring data.
 *                  For NFCC this is NFP lm, gpr, nn_reg, xfr_reg
 * @param size      Size of data to get from ring
 *                  (in bytes, 4 byte multiple, maximum 64)
 */
__NPE_INTRINSIC int npe_ring_get(uint32_t ring_num,
                                 uint64_t ring_addr,
                                 void *data,
                                 size_t size);


/**
 * Remove ring data from tail of a memory ring and store it in
 * 'local' memory. Return 0 on success or -1 if there are not
 * enough words in the ring to complete the request.
 *
 * @b Example:
 * @code
 * uint32_t ring_num;
 * uint64_t ring_addr;
 * NPE_MEM_RING_INIT(ringA, NPE_RING_SIZE_2K, 0)
 * ring_num = NPE_MEM_RING_NUM(ringA);
 * ring_addr = NPE_MEM_RING_ADDR(ringA);
 * __declspec(gp_reg) uint32_t regw[4];
 * __declspec(gp_reg) uint32_t regr[2];
 * regw[0] = 0x00;
 * regw[1] = 0x04;
 * regw[2] = 0x08;
 * regw[3] = 0x0C;
 * uint32_t num_bytes, status;
 * num_bytes = npe_ring_put(ring_num,
 *                          ring_addr,
 *                          regw,
 *                          sizeof(regw));
 *
 * status = npe_ring_pop(ring_num,
 *                       ring_addr,
 *                       regr,
 *                       sizeof(regr));
 * @endcode
 *
 * @param ring_num  Ring number. For NFCC / NFP this is 0 through 1023
 *                  and is generated using the NPE_MEM_RING_NUM() macro
 * @param ring_addr Ring memory address. For NFCC / NFP this is generated
 *                  using the NPE_MEM_RING_ADDR() macro and contains
 *                  bits 39:32 of the 40 bit memory address on bits 31:24
 *                  in the ring_addr param
 * @param data      Pointer to 'local' memory to load with ring data.
 *                  For NFCC this is NFP lm, gpr, nn_reg, xfr_reg
 * @param size      Size of data to pop from ring
 *                  (in bytes, 4 byte multiple, maximum 64)
 */
__NPE_INTRINSIC int npe_ring_pop(uint32_t ring_num,
                                 uint64_t ring_addr,
                                 void *data,
                                 size_t size);

/** @}
 * @}
 */

#endif /* __NPE_RING_H__ */
