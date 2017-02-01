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

/* Linked List operations */

#ifndef __NPE_LIST_H__
#define __NPE_LIST_H__

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


/** @file npe_list.h
 * @addtogroup list  Linked List Operations
 * @{
 */


/** @name Linked List related functions
 *  @{
 */


#ifndef __NFP_TOOL_NFCC
#include <pthread.h>

/* struct for emulating nfp mu linked lists with gcc */

struct npe_list {

    /* pointer to next npe_list struct
       in linked list used for maintaining
       multiple npe_lists. */
    struct npe_list *next;

    /* Text string containing list 'name'.
       Doing this because NFCC uses a list 'name' to
       reference the list, which is used by the linker
       to allocate the list memory. So for GCC
       compatibility need to use a 'name' to reference
       the list. */
    char name[32];

    /* lock for this list */
    pthread_mutex_t mtx;

    /* list control vars */
    uint64_t head;
    uint64_t tail;
    uint32_t length;
};


/* allocate memory for a new npe_list struct,
   initialize it's 'name' and mutex,  and add a pointer to
   it in the npe_list struct linked list */
void npe_list_allocate(const char *name);


/* note there is no support for 'deallocating' an
   npe_list, in either NFCC or GCC. in NFCC lists are
   allocated at link time, not at run time, and the
   GCC emulation gives a similar appearance */


/* return address of npe_list struct with name 'name' */
struct npe_list * npe_get_list_addr_gcc(const char *name);

#endif



/** Macro to allocate a linked list. Allocates a unique nfp6000
 *  'queue array' number, and memory for the queue descriptor
 *  initialization.
 *  @param _name   Name of the list memory
 *  @param _mu     Number of memory unit to put the linked list in (0,1, or 2)
 */
#if defined(__NFP_TOOL_NFCC)
#define NPE_MEM_LIST_ALLOC(_name, _mu) \
    _NFP_CHIPRES_ASM(.alloc_resource _name##_rnum emem##_mu##_queues global 1) \
    _NFP_CHIPRES_ASM(.alloc_mem _name emem##_mu global 16 16 )
#else /* GCC */
#define NPE_MEM_LIST_ALLOC(_name, _mu) \
    npe_list_allocate(#_name);
#endif

/** Macro to get unique list number of a linked list
 *  @param _name   Name of the list memory
 */
#if defined(__NFP_TOOL_NFCC)
#define NPE_MEM_LIST_NUM(_name) __link_sym(#_name "_rnum")
#else /* GCC */
#define NPE_MEM_LIST_NUM(_name) 0
#endif

/** Macro to get 40 bit memory address of a linked list.
 *  For NFP this is the address of the descriptor init memory.
 *  For GCC this is the address of the npe_list struct
 *  @param _name   Name of the list
 */
#if defined(__NFP_TOOL_NFCC)
#define NPE_MEM_LIST_ADDR(_name) (uint64_t)__link_sym(#_name)
#else /* GCC */
#define NPE_MEM_LIST_ADDR(_name) (uint64_t)npe_get_list_addr_gcc(#_name)
#endif


/**
 * Initialize a linked list structure previously allocated using
 * NPE_MEM_LIST_ALLOC
 *
 * @param list_num  List number. For NFCC / NFP this is 0 through 1023
 *                  and is generated using the NPE_MEM_LIST_NUM() macro
 * @param list_addr List descriptor memory address. For NFCC / NFP this
 *                  is the 40 bit address of the list memory and can be
 *                  generated using NPE_MEM_LIST_ADDR()
 */
__NPE_INTRINSIC void npe_list_init(uint32_t list_num, uint64_t list_addr);


/**
 * Add a word to the tail of a linked list stored in memory.
 *
 * Typically, the value being added is a pointer to a packet data buffer.
 * Bit 0 is used to flag SOP ( Start of Packet ) and bit 1 is used to flag
 * EOP ( End of Packet ) to support storing a packet across multiple buffers.
 *
 * If the linked list is empty, the new entry is stored as the head and tail
 * pointers. If the linked list is not empty, the new entry is stored in the 1st7
 * lword of the buffer pointed to by the current tail pointer.
 *
 * For NFP6000 40 bit addressing, the packet buffer must reside in the same
 * memory unit that contains the link list queue engine hardware. The upper 8
 * bits are not stored in the linked list, only the lower 32 bits are stored.
 *
 * @b Example:
 * @code
 * .alloc_mem pktBuffer emem0 global 2048 2048 )
 * uint32_t list_num;
 * uint64_t list_addr;
 * NPE_MEM_LIST_ALLOC(listA, 0)
 * list_num = NPE_MEM_LIST_NUM(listA);
 * list_addr = NPE_MEM_LIST_ADDR(listA);
 * npe_list_init(list_num, list_addr);
 * npe_list_enqueue(list_num,
 *                  list_addr,
 *                  3,
 *                  &pktBuffer)
 * @endcode
 *
 * @param list_num  List number. For NFCC / NFP this is 0 through 1023
 *                  and is generated using the NPE_MEM_LIST_NUM() macro
 * @param list_addr List memory address. For NFCC / NFP this is a 40 bit
 *                  address generated using the NPE_MEM_LIST_ADDR() macro
 * @param flags     bit 0: SOP (1 if 1st buffer in a packet)
 *                  bit 1: EOP (1 if last buffer in a packet)
 * @param buffer    Address of pkt buffer to enqueue. For NFCC only bits
 *                  31:0 are used. Bits 1:0 should be 0.
 */
__NPE_INTRINSIC void npe_list_enqueue(uint32_t list_num,
                                      uint64_t list_addr,
                                      uint32_t flags,
                                      uint64_t buffer);


/**
 * Remove word from the head of a linked list stored in memory. The
 * removed word should have been added using npe_list_enqueue.
 *
 * Typically, the value being removed is a pointer to a packet data buffer.
 * Bit 0 is used to flag SOP ( Start of Packet ) and bit 1 is used to flag
 * EOP ( End of Packet ) to support storing a packet across multiple buffers.
 *
 * Refer to npe_list_enqueue for additional information.
 *
 * @b Example:
 * @code
 * uint32_t list_num;
 * uint32_t list_addr;
 * list_num = NPE_MEM_LIST_NUM(listA);
 * list_addr = NPE_MEM_LIST_ADDR(listA);
 * uint32_t flags;
 * uint64_t pktBufferAddr;
 * npe_list_dequeue(list_num,
 *                  list_addr_hi,
 *                  &flags,
 *                  &pktBufferAddr);
 *
 * @endcode
 *
 * @param list_num  List number. For NFCC / NFP this is 0 through 1023
 *                  and is generated using the NPE_MEM_LIST_NUM() macro
 * @param list_addr List memory address. For NFCC / NFP this is a 40 bit
 *                  address generated using the NPE_MEM_LIST_ADDR() macro
 * @param flags     bit 0: SOP (1 if 1st buffer in a packet)
 *                  bit 1: EOP (1 if last buffer in a packet)
 * @param buffer    Address of location to store dequeued pkt buffer addr value.
 *                  For NFCC returns bits 31:0 of address, bits 1:0 should be 0
 */
__NPE_INTRINSIC void npe_list_dequeue(uint32_t list_num,
                                      uint64_t list_addr,
                                      uint32_t *flags,
                                      uint64_t *buffer);

/** @}
 * @}
 */

#endif /* __NPE_LIST_H__ */
