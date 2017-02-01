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
 *
 * @file          src/npe_list.c
 * @brief         Liked List operations
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
#include <nfp/mem_bulk.h>
#include <nfp/me.h>
#endif

/* npe code */
#include <npe_types.h>
#include <npe_list.h>


#ifndef __NFP_TOOL_NFCC

#include <stdlib.h>
#include <string.h>
#include <strings.h>

/* linked list emulation code for use on GCC */

/* start of linked list of npe_list structs */
volatile struct npe_list *npe_list_first = 0;

/* lock for linked list of npe_list structs */
pthread_mutex_t npe_list_mtx = PTHREAD_MUTEX_INITIALIZER;


void npe_list_allocate(const char *name)
{

    /* allocate, init list struct */

    struct npe_list *p = (struct npe_list *)malloc(sizeof(struct npe_list));
    bzero(p, sizeof(*p));
    strncpy(p->name, name, sizeof(p->name));
    pthread_mutex_init(&p->mtx, NULL);

    /* add to linked list of list structs */
    pthread_mutex_lock(&npe_list_mtx);

    volatile struct npe_list *q = npe_list_first;
    if (q == 0 )
        npe_list_first = p;
    else {
        while ( q->next ) {
            q = q->next;
        }
        q->next = p;
     }

    pthread_mutex_unlock(&npe_list_mtx);
}


struct npe_list * npe_get_list_addr_gcc(const char *name)
{
    pthread_mutex_lock(&npe_list_mtx);

    volatile struct npe_list *q = npe_list_first;

    while (q) {
        if ( strncmp((const char *)q->name, name, sizeof(q->name)) == 0 )
            break; /* found */
        else
            q = q->next;
    }

    pthread_mutex_unlock(&npe_list_mtx);

    return (struct npe_list *)q;
}


void npe_list_enqueue_gcc(struct npe_list *list, uint64_t buffer, uint32_t flags) {

    /* take lock to do enqueue */
    pthread_mutex_lock(&list->mtx);

    /* enqueue data to list */
    uint64_t val = (buffer & ~3) | (flags & 3);
    if (list->length == 0 ) {
        list->head = val;
        list->tail = val;
    } else {
        uint64_t *p = (uint64_t *)(list->tail & ~3);
        *p = val;
        list->tail = val;
    }

    list->length++;

    pthread_mutex_unlock(&list->mtx);
}

void npe_list_dequeue_gcc(struct npe_list *list, uint64_t *buffer, uint32_t *flags) {

    /* take lock */
    pthread_mutex_lock(&list->mtx);

    /* dequeue data from list, if not empty */
    if (list->length) {
        /* return data from list head */
        *flags = list->head & 3;
        *buffer = list->head & 0xfffffffc;
        /* adjust list and head */
        uint64_t *p = (uint64_t *)(list->head & ~3);
        list->head = *p;
        list->length--;
    } else {
        *buffer = 0;
        *flags = 0;
    }

    pthread_mutex_unlock(&list->mtx);
}

#endif /* GCC */


__NPE_INTRINSIC void
npe_list_init(uint32_t list_num, uint64_t list_addr)
{
    #ifdef __NFP_TOOL_NFCC
    SIGNAL s;
    __xwrite uint32_t xfr_wr[4];
    __xread uint32_t xfr_rd[4];
    uint32_t temp;
    uint32_t addr_hi;
    uint32_t addr_lo;

    /* addr bits 39:32 into addr_hi 31:24 */
    addr_hi = (uint32_t)((list_addr >> 8) & 0xff000000);

    /* addr bits 31:0 into addr_lo 31:0 */
    addr_lo = (uint32_t)(list_addr & 0xffffffff);

    /* write descriptor to ddr */
    temp = 0x03fffffc; /* head mask, 30 bits, lword aligned */
    xfr_wr[0] = addr_lo & temp; /* head */

    temp = 0xfffffffc; /* tail mask, 32 bits lword aligned */
    temp = temp & addr_lo; /* address */
    temp = temp | 2; /* type 2 */
    xfr_wr[1] = temp; /* tail */

    xfr_wr[2] = 0; /* locality 0, page 0 */

    xfr_wr[3] = 0; /* reserved */

    mem_write64(&xfr_wr[0],
               (__mem void *)list_addr,
               sizeof(xfr_wr));

    /* read back to make sure it reaches ddr */
    mem_read64(&xfr_rd[0],
               (__mem void *)list_addr,
               sizeof(xfr_rd));

    /* read descriptor into qarray */
    temp = list_num << 16;
    __asm{ alu[--, 8, +, temp]
           mem[rd_qdesc, --, addr_hi, <<8, addr_lo], indirect_ref }

    /* push descriptor back to make sure it reaches qarray */
    __asm { mem[push_qdesc, xfr_rd[0], addr_hi, <<8, list_num], ctx_swap[s] }

    #else

    /* nothing to do for GCC */

    #endif
}


__NPE_INTRINSIC void
npe_list_enqueue(uint32_t list_num, uint64_t list_addr,
                 uint32_t flags, uint64_t buffer)
{
    #ifdef __NFP_TOOL_NFCC

    uint32_t buf_addr = (uint32_t)((buffer & ~3) | (flags & 3));
    uint32_t addr_hi = (uint32_t)((list_addr >> 8) & 0xff000000);
    __asm { alu[--, 0x8, OR, list_num, <<16] }
    __asm { mem[enqueue, --, addr_hi, <<8, buf_addr], indirect_ref }

    #else /* GCC */

    npe_list_enqueue_gcc((struct npe_list *)list_addr, buffer, flags);

    #endif
}


__NPE_INTRINSIC void
npe_list_dequeue(uint32_t list_num, uint64_t list_addr,
                 uint32_t *flags, uint64_t *buffer)
{
    #ifdef __NFP_TOOL_NFCC

    SIGNAL s;
    uint32_t __xread xfr;
    uint32_t addr_hi = (uint32_t)((list_addr >> 8) & 0xff000000);

    __asm { mem[dequeue, xfr, addr_hi, <<8, list_num], ctx_swap[s] }

    *flags = ~xfr & 3; /* Note flags are inverted */
    *buffer = (uint64_t)(xfr & 0xfffffffc);

    #else /* GCC */

    npe_list_dequeue_gcc((struct npe_list *)list_addr, buffer, flags);

    #endif
}
