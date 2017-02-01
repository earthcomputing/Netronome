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
 * @file          src/npe_ring.c
 * @brief         Ring operations
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
#include <nfp/mem_ring.h>
#include <nfp/me.h>
#endif

/* npe code */
#include <npe_types.h>
#include <npe_ring.h>


#ifndef __NFP_TOOL_NFCC

#include <stdlib.h>
#include <string.h>
#include <strings.h>

/* ring emulation code for use on GCC */

/* start of linked list of npe_ring structs */
volatile struct npe_ring *npe_ring_first = 0;

/* lock for linked list of npe_ring structs */
pthread_mutex_t npe_ring_mtx = PTHREAD_MUTEX_INITIALIZER;

void npe_ring_init(const char *name, const uint32_t size)
{
    /* allocate, init ring struct */

    struct npe_ring *ring = (struct npe_ring *)malloc(sizeof(struct npe_ring));
    bzero(ring, sizeof(*ring));
    strncpy(ring->name, name, sizeof(ring->name));
    ring->size = size / 4;
    ring->free = size / 4;
    ring->mem = (uint32_t *)malloc(size);
    pthread_mutex_init(&ring->mtx, NULL);


    /* add to linked list of ring structs */
    pthread_mutex_lock(&npe_ring_mtx);

    volatile struct npe_ring *q = npe_ring_first;
    if (q == 0 )
        npe_ring_first = ring;
    else {
        while ( q->next ) {
            q = q->next;
        }
        q->next = ring;
     }

    pthread_mutex_unlock(&npe_ring_mtx);
}


struct npe_ring * npe_get_ring_addr_gcc(const char *name)
{
    pthread_mutex_lock(&npe_ring_mtx);

    volatile struct npe_ring *ring = npe_ring_first;

    while (ring) {
        if ( strncmp((const char *)ring->name, name, sizeof(ring->name)) == 0 )
            break; /* found */
        else
            ring = ring->next;
    }

    pthread_mutex_unlock(&npe_ring_mtx);

    return (struct npe_ring *)ring;
}


int npe_ring_put_gcc(struct npe_ring *ring, void *p_data, size_t size)
{
    int retval;
    int words = size / 4;

    pthread_mutex_lock(&ring->mtx);

    /* if enough room in ring ... */
    if ( ring->free >= words ) {

        /* put data to ring */
        int i;
        uint32_t *q = (uint32_t *)p_data;
        for ( i = 0; i < words; i++ ) {
            ring->mem[ring->tail++] = *q++;
            if ( ring->tail >= ring->size )
                ring->tail = 0;
        }

        retval = (ring->size - ring->free) * 4;

        ring->free -= words;

    } else {

        retval = -1;
    }

    pthread_mutex_unlock(&ring->mtx);

    return retval;
}


void npe_ring_journal_gcc(struct npe_ring *ring, void *p_data, size_t size)
{
    int words = size / 4;

    pthread_mutex_lock(&ring->mtx);

    /* put data to ring, irrespective of free bytes */
    int i;
    uint32_t *q = (uint32_t *)p_data;
    for ( i = 0; i < words; i++ ) {
        ring->mem[ring->tail++] = *q++;
        if ( ring->tail >= ring->size )
            ring->tail = 0;
    }

    if ( ring->free >= words )
        ring->free -= words;
    else
        ring->free = 0;

    pthread_mutex_unlock(&ring->mtx);
}


int npe_ring_get_gcc(struct npe_ring *ring, void *p_data, size_t size)
{
    int retval;
    int words = size / 4;

    pthread_mutex_lock(&ring->mtx);

    /* if enough words in ring ... */
    if ((ring->size - ring->free) >= words ) {

        /* get data from head */
        int i;
        uint32_t *q = (uint32_t *)p_data;
        for ( i = 0; i < words; i++ ) {
            *q++ = ring->mem[ring->head++];
            if ( ring->head >= ring->size )
                ring->head = 0;
        }

        retval = 0;

        ring->free += words;

    } else {

        retval = -1;
    }

    pthread_mutex_unlock(&ring->mtx);

    return retval;
}


int npe_ring_pop_gcc(struct npe_ring *ring, void *p_data, size_t size)
{
    int retval;
    int words = size / 4;

    pthread_mutex_lock(&ring->mtx);

    /* if enough words in ring ... */
    if ((ring->size - ring->free) >= words ) {

        /* get data from tail, in order it was written */
        int i;
        uint32_t *q = (uint32_t *)p_data + (words - 1);
        for ( i = 0; i < words; i++ ) {
            if (ring->tail == 0 )
                ring->tail = ring->size;
            else
                ring->tail--;
            *q-- = ring->mem[ring->tail];
        }

        retval = 0;

        ring->free += words;

    } else {

        retval = -1;
    }

    pthread_mutex_unlock(&ring->mtx);

    return retval;
}

#endif /* GCC */

#define _MULTI_REG_OP( reg_op, s )                                  \
{                                                                   \
    switch ( s ) {                                                  \
                                                                    \
    case 64:                                                        \
        reg_op(0); reg_op(1); reg_op(2); reg_op(3); reg_op(4);      \
        reg_op(5); reg_op(6); reg_op(7); reg_op(8); reg_op(9);      \
        reg_op(10); reg_op(11); reg_op(12); reg_op(13); reg_op(14); \
        reg_op(15);                                                 \
        break;                                                      \
                                                                    \
    case 60:                                                        \
        reg_op(0); reg_op(1); reg_op(2); reg_op(3); reg_op(4);      \
        reg_op(5); reg_op(6); reg_op(7); reg_op(8); reg_op(9);      \
        reg_op(10); reg_op(11); reg_op(12); reg_op(13); reg_op(14); \
        break;                                                      \
                                                                    \
    case 56:                                                        \
        reg_op(0); reg_op(1); reg_op(2); reg_op(3); reg_op(4);      \
        reg_op(5); reg_op(6); reg_op(7); reg_op(8); reg_op(9);      \
        reg_op(10); reg_op(11); reg_op(12); reg_op(13);             \
        break;                                                      \
                                                                    \
    case 52:                                                        \
        reg_op(0); reg_op(1); reg_op(2); reg_op(3); reg_op(4);      \
        reg_op(5); reg_op(6); reg_op(7); reg_op(8); reg_op(9);      \
        reg_op(10); reg_op(11); reg_op(12);                         \
        break;                                                      \
                                                                    \
    case 48:                                                        \
        reg_op(0); reg_op(1); reg_op(2); reg_op(3); reg_op(4);      \
        reg_op(5); reg_op(6); reg_op(7); reg_op(8); reg_op(9);      \
        reg_op(10); reg_op(11);                                     \
        break;                                                      \
                                                                    \
    case 44:                                                        \
        reg_op(0); reg_op(1); reg_op(2); reg_op(3); reg_op(4);      \
        reg_op(5); reg_op(6); reg_op(7); reg_op(8); reg_op(9);      \
        reg_op(10);                                                 \
        break;                                                      \
                                                                    \
    case 40:                                                        \
        reg_op(0); reg_op(1); reg_op(2); reg_op(3); reg_op(4);      \
        reg_op(5); reg_op(6); reg_op(7); reg_op(8); reg_op(9);      \
        break;                                                      \
                                                                    \
    case 36:                                                        \
        reg_op(0); reg_op(1); reg_op(2); reg_op(3); reg_op(4);      \
        reg_op(5); reg_op(6); reg_op(7); reg_op(8);                 \
        break;                                                      \
                                                                    \
    case 32:                                                        \
        reg_op(0); reg_op(1); reg_op(2); reg_op(3); reg_op(4);      \
        reg_op(5); reg_op(6); reg_op(7);                            \
        break;                                                      \
                                                                    \
    case 28:                                                        \
        reg_op(0); reg_op(1); reg_op(2); reg_op(3); reg_op(4);      \
        reg_op(5); reg_op(6);                                       \
        break;                                                      \
                                                                    \
    case 24:                                                        \
       reg_op(0); reg_op(1); reg_op(2); reg_op(3); reg_op(4);       \
       reg_op(5);                                                   \
       break;                                                       \
                                                                    \
    case 20:                                                        \
        reg_op(0); reg_op(1); reg_op(2); reg_op(3); reg_op(4);      \
        break;                                                      \
                                                                    \
    case 16:                                                        \
        reg_op(0);  reg_op(1);  reg_op(2);  reg_op(3);              \
        break;                                                      \
                                                                    \
    case 12:                                                        \
        reg_op(0);  reg_op(1);  reg_op(2);                          \
        break;                                                      \
                                                                    \
    case 8:                                                         \
        reg_op(0);  reg_op(1);                                      \
        break;                                                      \
                                                                    \
    case 4:                                                         \
        reg_op(0);                                                  \
        break;                                                      \
                                                                    \
    default:                                                        \
        break;                                                      \
    }                                                               \
}


__NPE_INTRINSIC int
npe_ring_put(uint32_t ring_num, uint64_t ring_addr, void *p, size_t size)
{
    uint32_t ret_val;

    #ifdef __NFP_TOOL_NFCC
    uint32_t __xrw xfr[16];

    ctassert(size <= 64);
    ctassert(!(size & 3));

    if (__is_in_lmem(p)) {
        #undef _REG_OP
        #define _REG_OP(_x) (xfr[_x] = ((__lmem uint32_t *)p)[_x])
    } else {
        #undef _REG_OP
        #define _REG_OP(_x) (xfr[_x] = ((__gpr uint32_t *)p)[_x])
    }

    _MULTI_REG_OP( _REG_OP, size);

    ret_val = mem_ring_put(ring_num,
                           (mem_ring_addr_t)(ring_addr & 0xffffffff),
                           (__xrw void *)xfr,
                           size);

    #else /* GCC */

    ret_val = npe_ring_put_gcc((struct npe_ring *)ring_addr, (uint32_t *)p, size);

    #endif

    return ret_val;
}


__NPE_INTRINSIC void
npe_ring_journal(uint32_t ring_num, uint64_t ring_addr, void *p, size_t size)
{
    #ifdef __NFP_TOOL_NFCC
    uint32_t __xrw xfr[16];

    ctassert(size <= 64);
    ctassert(!(size & 3));

    /* use fast journal cmd for 1 lword */
    if ( size == 4 ) {
        mem_ring_journal_fast(ring_num,
                              (mem_ring_addr_t)(ring_addr & 0xffffffff),
                              *(uint32_t *)p);
    } else {

        /* otherwise use regular journal cmd */
        if (__is_in_lmem(p)) {
            #undef _REG_OP
            #define _REG_OP(_x) (xfr[_x] = ((__lmem uint32_t *)p)[_x])
        } else {
            #undef _REG_OP
            #define _REG_OP(_x) (xfr[_x] = ((__gpr uint32_t *)p)[_x])
        }

        _MULTI_REG_OP( _REG_OP, size);

        mem_ring_journal(ring_num,
                (mem_ring_addr_t)(ring_addr & 0xffffffff),
                (__xrw void *)xfr,
                size);
    }

    #else /* GCC */

    npe_ring_journal_gcc((struct npe_ring *)ring_addr, (uint32_t *)p, size);

    #endif
}


__NPE_INTRINSIC int
npe_ring_get(uint32_t ring_num, uint64_t ring_addr, void *p, size_t size)
{
    int ret_val;

    #ifdef __NFP_TOOL_NFCC
    uint32_t __xrw xfr[16];

    ctassert(size <= 64);
    ctassert(!(size & 3));

    ret_val = mem_ring_get(ring_num,
                           (mem_ring_addr_t)(ring_addr & 0xffffffff),
                           (__xrw void *)xfr,
                           size);

    if (__is_in_lmem(p)) {
        #undef _REG_OP
        #define _REG_OP(_x) (((__lmem uint32_t *)p)[_x] = xfr[_x])
    } else {
        #undef _REG_OP
        #define _REG_OP(_x) (((__gpr uint32_t *)p)[_x] = xfr[_x])
    }

    _MULTI_REG_OP( _REG_OP, size);

    #else /* GCC */

    ret_val = npe_ring_get_gcc((struct npe_ring *)ring_addr, (uint32_t *)p, size);

    #endif

    return ret_val;
}


__NPE_INTRINSIC int
npe_ring_pop(uint32_t ring_num, uint64_t ring_addr, void *p, size_t size)
{
    int ret_val;

    #ifdef __NFP_TOOL_NFCC
    uint32_t __xrw xfr[16];

    ctassert(size <= 64);
    ctassert(!(size & 3));

    ret_val = mem_ring_pop(ring_num,
                           (mem_ring_addr_t)(ring_addr & 0xffffffff),
                           (__xrw void *)xfr,
                           size);

    if (__is_in_lmem(p)) {
        #undef _REG_OP
        #define _REG_OP(_x) (((__lmem uint32_t *)p)[_x] = xfr[_x])
    } else {
        #undef _REG_OP
        #define _REG_OP(_x) (((__gpr uint32_t *)p)[_x] = xfr[_x])
    }

    _MULTI_REG_OP( _REG_OP, size);

    #else /* GCC */

    ret_val = npe_ring_pop_gcc((struct npe_ring *)ring_addr, (uint32_t *)p, size);

    #endif

    return ret_val;
}
