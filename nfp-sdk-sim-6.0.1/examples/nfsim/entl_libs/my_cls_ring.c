/*
 * Copyright (C) 2008-2014 Netronome Systems, Inc.  All rights reserved.
 * Copyright (C) 2017,  Earth Computing Inc.  All rights reserved.
 *
 *
 * @file          cls_ring.c
 * @brief         Copy of nfp_cls_ring.c which does not compile 
 *
 * Author:        Atsushi Kasuya
 *
 */
#include <nfp.h>
#include <me.h>
#include <mutexlv.h>
#include <stdint.h>
#include <pkt/pkt.h>
#include <net/eth.h>
#include <nfp/mem_bulk.h>
#include <nfp6000/nfp_cls.h>
#include <assert.h>

#include <nfp_override.h>
#include <nfp_intrinsic.h>

#include <vnic/utils/cls_ring.h>
#include "my_cls_ring.h"

//////////////////////////////////////////////////////////////////////
//  nfp_cls_ring code copied here (original doesn't compile)
//////////////////////////////////////////////////////////////////////
#define CT_ASSERT(expr) __ct_assert(expr, #expr)
//#define RT_RANGE_ASSERT(exp) RT_ASSERT(exp)
#define RT_ALIGN_ASSERT(exp)
#define RT_RANGE_ASSERT(exp)
#define RT_ASSERT(exp)
#define _INTRINSIC_BEGIN
#define _INTRINSIC_END


#define _CLS_RING_NUMBER_CHECK(ring_number)                             \
        if (__is_ct_const(ring_number))                                 \
        {                                                               \
            CT_ASSERT(ring_number <= 15);                               \
        }                                                               \
        else                                                            \
        {                                                               \
            RT_RANGE_ASSERT(ring_number <= 15);                         \
        }

#define _CLS_RING_COUNT_CHECK_16(count)                                 \
        if (__is_ct_const(count))                                       \
        {                                                               \
            CT_ASSERT(count >= 1 && count <= 16);                       \
        }                                                               \
        else                                                            \
        {                                                               \
            RT_RANGE_ASSERT(count >= 1 && count <= 16);                 \
        }

#define _CLS_RING_COUNT_CHECK_32(count)                                 \
        if (__is_ct_const(count))                                       \
        {                                                               \
            CT_ASSERT(count >= 1 && count <= 32);                       \
        }                                                               \
        else                                                            \
        {                                                               \
            RT_RANGE_ASSERT(count >= 1 && count <= 32);                 \
        }

#define _CLS_RING_OFFSET_CHECK(offset)                                  \
        if (__is_ct_const(offset))                                      \
        {                                                               \
            CT_ASSERT(offset <= 0x3fff);                                \
        }                                                               \
        else                                                            \
        {                                                               \
            RT_RANGE_ASSERT(offset <= 0x3fff);                          \
        }

#define _CLS_RING_EVENTS_CHECK(report_events)                           \
        if (__is_ct_const(report_events))                               \
        {                                                               \
            CT_ASSERT(report_events <= 0x0f);                           \
        }                                                               \
        else                                                            \
        {                                                               \
            RT_RANGE_ASSERT(report_events <= 0x0f);                     \
        }

#define _CLS_RING_ISLAND_CHECK(island_number)                           \
        if (__is_ct_const(island_number))                               \
        {                                                               \
            CT_ASSERT(island_number <= 0x3f);                           \
        }                                                               \
        else                                                            \
        {                                                               \
            RT_ASSERT(island_number <= 0x3f);                           \
        }
/*
 * Verify that base address is aligned. Base address is shifted << 7 when written to
 * ring base CSR.
 */
#define _CLS_RING_BASE_ADDRESS_ALIGN_CHECK(addr)            \
        if (__is_ct_const(addr))                            \
        {                                                   \
            CT_ASSERT((addr & 0x7f) == 0);                  \
        }                                                   \
        else                                                \
        {                                                   \
            RT_ALIGN_ASSERT((addr & 0x7f) == 0);            \
        }

/*
 * set the address with ring number and offset (seq number)
 */
#define _CLS_RING_ADDRESS_WITH_OFFSET(address, ring, offset)                    \
    address =  (1 << 20) | ((ring & 0x0f) << 16) | ((offset & 0x3fff) << 2) ;

/*
 * set the address with ring number
 */
#define _CLS_RING_ADDRESS(address, ring)                    \
    address = ((ring & 0x0f) << 2);

/*
 * set the "hi_addr" using island id
 */
#define _CLS_RING_SET_LOCALITY(locality, island)            \
    locality = (island_id & 0x3f) << 26;

/*
 * set the address for ring ordered lock and ordered unlock. The top bits are set.
 */
#define _CLS_RING_ADDRESS_ORDERED_LOCK(address, ring, offset)                    \
    address = (0xfff << 20) | ((ring & 0x0f) << 16) | ((offset & 0x3fff) << 2) ;

/** CLSRingsMap Address map
 */
typedef enum
{
    RING_BASE_0     =   0x10000,

    RING_PTR_0     =   0x10080

} CLS_RING_MAP;


/*
* Implementation for ring commands where the address parameter is already filled in
*/
#define _CLS_RING_IMPLEMENT_ADDRESS_COMMAND(command, is_read, cls_command, max_count)                           \
{                                                                                                               \
    if (is_read)                                                                                                \
    {                                                                                                           \
        CT_ASSERT(__is_read_reg(data));                                                                         \
    }                                                                                                           \
    else                                                                                                        \
    {                                                                                                           \
        CT_ASSERT(__is_write_reg(data));                                                                        \
    }                                                                                                           \
    CT_ASSERT(__is_ct_const(sync));                                                                             \
    CT_ASSERT(sync == sig_done || sync == ctx_swap);                                                            \
    if (__is_ct_const(count) && count <= 8)                                                                     \
    {                                                                                                           \
        CT_ASSERT(count != 0);                                                                                  \
        if (sync == sig_done)                                                                                   \
        {                                                                                                       \
            __asm cls[command, *data, address, 0, __ct_const_val(count)], sig_done[*sig_ptr]                    \
        }                                                                                                       \
        else                                                                                                    \
        {                                                                                                       \
            __asm cls[command, *data, address, 0, __ct_const_val(count)], ctx_swap[*sig_ptr]                    \
        }                                                                                                       \
    }                                                                                                           \
    else                                                                                                        \
    {                                                                                                           \
        if (!__is_ct_const(count))                                                                              \
        {                                                                                                       \
            CT_QPERFINFO_INDIRECT_REF(cls_command);                                                             \
        }                                                                                                       \
        {                                                                                                       \
            _INTRINSIC_IND_ALU_COUNT_MIN_MAX(count, 1, max_count);                                              \
            if (sync == sig_done)                                                                               \
            {                                                                                                   \
                __asm cls[command, *data, address, 0, __ct_const_val(count)], sig_done[*sig_ptr], indirect_ref  \
            }                                                                                                   \
            else                                                                                                \
            {                                                                                                   \
                __asm cls[command, *data, address, 0, __ct_const_val(count)], ctx_swap[*sig_ptr], indirect_ref  \
            }                                                                                                   \
        }                                                                                                       \
    }                                                                                                           \
}



/*
* Implementation for ring commands where the address parameter is already filled in
*/
#define _CLS_RING_IMPLEMENT_ADDRESS_COMMAND_PTR40(command, is_read, cls_command, max_count)                     \
{                                                                                                               \
    if (is_read)                                                                                                \
    {                                                                                                           \
        CT_ASSERT(__is_read_reg(data));                                                                         \
    }                                                                                                           \
    else                                                                                                        \
    {                                                                                                           \
        CT_ASSERT(__is_write_reg(data));                                                                        \
    }                                                                                                           \
    CT_ASSERT(__is_ct_const(sync));                                                                             \
    CT_ASSERT(sync == sig_done || sync == ctx_swap);                                                            \
    if (__is_ct_const(count) && count <= 8)                                                                     \
    {                                                                                                           \
        CT_ASSERT(count != 0);                                                                                  \
        if (sync == sig_done)                                                                                   \
        {                                                                                                       \
            __asm cls[command, *data, hi_addr, <<8, low_addr, __ct_const_val(count)], sig_done[*sig_ptr]        \
        }                                                                                                       \
        else                                                                                                    \
        {                                                                                                       \
            __asm cls[command, *data, hi_addr, <<8, low_addr, __ct_const_val(count)], ctx_swap[*sig_ptr]        \
        }                                                                                                       \
    }                                                                                                           \
    else                                                                                                        \
    {                                                                                                           \
        if (!__is_ct_const(count))                                                                              \
        {                                                                                                       \
            CT_QPERFINFO_INDIRECT_REF(cls_command);                                                             \
        }                                                                                                       \
        {                                                                                                       \
            _INTRINSIC_IND_ALU_COUNT_MIN_MAX(count, 1, max_count);                                              \
            if (sync == sig_done)                                                                               \
            {                                                                                                   \
                __asm cls[command, *data, hi_addr, <<8, low_addr, __ct_const_val(count)], sig_done[*sig_ptr], indirect_ref  \
            }                                                                                                   \
            else                                                                                                \
            {                                                                                                   \
                __asm cls[command, *data, hi_addr, <<8, low_addr, __ct_const_val(count)], ctx_swap[*sig_ptr], indirect_ref  \
            }                                                                                                   \
        }                                                                                                       \
    }                                                                                                           \
}


__intrinsic
void cls_ring_ordered_lock_ptr32(
    unsigned int ring_number,
    unsigned int sequence_number,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
        __gpr unsigned int ring_address;

        CT_ASSERT(__is_ct_const(sync));
        CT_ASSERT(sync == sig_done || sync == ctx_swap);
        _CLS_RING_NUMBER_CHECK(ring_number);
        _CLS_RING_OFFSET_CHECK(sequence_number);

        _CLS_RING_ADDRESS_ORDERED_LOCK(ring_address, ring_number, sequence_number);
        if (sync == sig_done)
        {
            __asm
            {
                cls[ring_ordered_lock, --, ring_address, 0], sig_done[*sig_ptr]
            }
        }
        else
        {
            __asm
            {
                cls[ring_ordered_lock, --, ring_address, 0], ctx_swap[*sig_ptr]
            }
        }
    }
_INTRINSIC_END;
}



__intrinsic
void cls_ring_ordered_lock_ptr40(
    unsigned int ring_number,
    unsigned int sequence_number,
    unsigned int island_id,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
        __gpr unsigned int ring_address;
        __gpr unsigned int hi_addr;

        CT_ASSERT(__is_ct_const(sync));
        CT_ASSERT(sync == sig_done || sync == ctx_swap);

        _CLS_RING_NUMBER_CHECK(ring_number);
        _CLS_RING_OFFSET_CHECK(sequence_number);
        _CLS_RING_ISLAND_CHECK(island_id);

        _CLS_RING_SET_LOCALITY(hi_addr, island_id);
        _CLS_RING_ADDRESS_ORDERED_LOCK(ring_address, ring_number, sequence_number);
        if (sync == sig_done)
        {
            __asm
            {
                cls[ring_ordered_lock, --, hi_addr, << 8, ring_address], sig_done[*sig_ptr]
            }
        }
        else
        {
            __asm
            {
                cls[ring_ordered_lock, --, hi_addr, << 8, ring_address], ctx_swap[*sig_ptr]
            }
        }
    }
_INTRINSIC_END;
}


__intrinsic
void cls_ring_ordered_unlock_ptr32(
    unsigned int ring_number,
    unsigned int sequence_number
)
{
    _INTRINSIC_BEGIN;
    {
        __gpr unsigned int ring_address;

        _CLS_RING_NUMBER_CHECK(ring_number);
        _CLS_RING_OFFSET_CHECK(sequence_number);
        _CLS_RING_ADDRESS_ORDERED_LOCK(ring_address, ring_number, sequence_number);
        __asm
        {
            cls[ring_ordered_unlock, --, ring_address, 0]
        }
    }

_INTRINSIC_END;
}

__intrinsic
void cls_ring_ordered_unlock_ptr40(
    unsigned int ring_number,
    unsigned int sequence_number,
    unsigned int island_id
)
{
    _INTRINSIC_BEGIN;
    {
        __gpr unsigned int ring_address;
        __gpr unsigned int hi_addr;

        _CLS_RING_NUMBER_CHECK(ring_number);
        _CLS_RING_OFFSET_CHECK(sequence_number);
        _CLS_RING_ADDRESS_ORDERED_LOCK(ring_address, ring_number, sequence_number);
        _CLS_RING_SET_LOCALITY(hi_addr, island_id);
        __asm
        {
            cls[ring_ordered_unlock, --, hi_addr, << 8, ring_address]
        }
    }

_INTRINSIC_END;
}

/* Local scratch memory ring workq add thread  */
void cls_ring_workq_add_thread_ptr32(
    __xread void *data,                                 /* work returned                         */
    unsigned int ring_number,                           /* ring number (0 to 15)                 */
    unsigned int count,                                 /* number of longwords to return         */
    sync_t sync,                                        /* type of synchronization to use         */
    SIGNAL *sig_ptr                                     /* signal to raise upon completion        */
)
{
    _INTRINSIC_BEGIN;
    {
        __gpr unsigned int address;
        _CLS_RING_ADDRESS(address, ring_number);
        _CLS_RING_NUMBER_CHECK(ring_number);
        _CLS_RING_COUNT_CHECK_16(count);

        //_CLS_RING_IMPLEMENT_ADDRESS_COMMAND(ring_workq_add_thread, 1, cls_ring_workq_add_thread, 16);
                __asm cls[ring_workq_add_thread, *data, address, 0, __ct_const_val(count)], ctx_swap[*sig_ptr], indirect_ref  
    }


_INTRINSIC_END;
}

/* Local scratch memory ring workq add thread  */
void cls_ring_workq_add_thread_ptr40(
    __xread void *data,                                 /* work returned                         */
    unsigned int ring_number,                           /* ring number (0 to 15)                 */
    unsigned int count,                                 /* number of longwords to return         */
    unsigned int island_id,
    sync_t sync,                                        /* type of synchronization to use         */
    SIGNAL *sig_ptr                                     /* signal to raise upon completion        */
)
{
    _INTRINSIC_BEGIN;
    {
        __gpr unsigned int low_addr;
        __gpr unsigned int hi_addr;

        _CLS_RING_ADDRESS(low_addr, ring_number);
        _CLS_RING_NUMBER_CHECK(ring_number);
        _CLS_RING_COUNT_CHECK_16(count);
        _CLS_RING_SET_LOCALITY(hi_addr, island_id);
        //_CLS_RING_IMPLEMENT_ADDRESS_COMMAND_PTR40(ring_workq_add_thread, 1, cls_ring_workq_add_thread, 16);
            __asm cls[ring_workq_add_thread, *data, hi_addr, <<8, low_addr, __ct_const_val(count)], ctx_swap[*sig_ptr]        
    }


_INTRINSIC_END;
}


/* Local scratch memory ring workq add work  */
void cls_ring_workq_add_work_ptr32(
    __xwrite void *data,                                /* work to add to ring                   */
    unsigned int ring_number,                           /* ring number (0 to 15)                 */
    unsigned int count,                                 /* number of longwords to return         */
    sync_t sync,                                        /* type of synchronization to use        */
    SIGNAL *sig_ptr                                     /* signal to raise upon completion       */
)
{
    _INTRINSIC_BEGIN;
    {
        __gpr unsigned int address;
        _CLS_RING_ADDRESS(address, ring_number);
        _CLS_RING_NUMBER_CHECK(ring_number);
        _CLS_RING_COUNT_CHECK_16(count);

        //_CLS_RING_IMPLEMENT_ADDRESS_COMMAND(ring_workq_add_work, 0, cls_ring_workq_add_work, 16);
                __asm cls[ring_workq_add_work, *data, address, 0, __ct_const_val(count)], ctx_swap[*sig_ptr], indirect_ref  
    }

_INTRINSIC_END;
}


/* Local scratch memory ring workq add work  */
void cls_ring_workq_add_work_ptr40(
    __xwrite void *data,                                /* work to add to ring                   */
    unsigned int ring_number,                           /* ring number (0 to 15)                 */
    unsigned int count,                                 /* number of longwords to return         */
    unsigned int island_id,
    sync_t sync,                                        /* type of synchronization to use        */
    SIGNAL *sig_ptr                                     /* signal to raise upon completion       */
)
{
    _INTRINSIC_BEGIN;
    {
        __gpr unsigned int low_addr;
        __gpr unsigned int hi_addr;

        _CLS_RING_ADDRESS(low_addr, ring_number);
        _CLS_RING_NUMBER_CHECK(ring_number);
        _CLS_RING_COUNT_CHECK_16(count);
        _CLS_RING_SET_LOCALITY(hi_addr, island_id);
        //_CLS_RING_IMPLEMENT_ADDRESS_COMMAND_PTR40(ring_workq_add_work, 0, cls_ring_workq_add_work, 16);
            __asm cls[ring_workq_add_work, *data, hi_addr, <<8, low_addr, __ct_const_val(count)], ctx_swap[*sig_ptr]        
    }

_INTRINSIC_END;
}

__intrinsic
void cls_read_ring_ptr( __xread void *data, unsigned int ring_number )
{
    __cls char *ring_addr = (__cls char *)NFP_CLS_RINGS_RING_BASE(ring_number);

    cls_read( data, ring_addr + 0x80, sizeof(unsigned int)) ;

}


__intrinsic void
cls_ring_setup(unsigned int rnum, __cls void *base, size_t size)
{
    unsigned int entries = size / 4;
    __xwrite unsigned int cls_ring_base;
    __xwrite unsigned int cls_ring_ptrs = 0;
    __cls char *ring_addr = (__cls char *)NFP_CLS_RINGS_RING_BASE(rnum);
    SIGNAL s1, s2;

    try_ctassert(rnum < 16);

    /* Note, size is in bytes and an entry is 32bits */
    ctassert(__is_ct_const(size));
    ctassert(__is_log2(entries));
    ctassert((entries >= 32) && (entries <= SZ_1K));

    /* NB: if base is linker assigned, this can't be evaluated
     * at compile time */
    cls_ring_base = (NFP_CLS_RINGS_RING_BASE_SIZE(__log2(entries) - 5) |
                     NFP_CLS_RINGS_RING_BASE_BASE(((unsigned int)base) >> 7));

    __cls_write(&cls_ring_base, ring_addr, sizeof(cls_ring_base),
                sizeof(cls_ring_base), sig_done, &s1);
    __cls_write(&cls_ring_ptrs, ring_addr + 0x80, sizeof(cls_ring_ptrs),
                sizeof(cls_ring_ptrs), sig_done, &s2);
    wait_for_all(&s1, &s2);
}


/* Local scratch memory read              */

__intrinsic
void cls_read_be_ptr32(
    __xread void *data,                                 /* data to read                           */
    volatile void __cls *address,                       /* address to read from                   */
    unsigned int count,                                 /* number of longwords to read            */
    sync_t sync,                                        /* type of synchronization to use         */
    SIGNAL *sig_ptr                                     /* signal to raise upon completion        */
)
{
    _INTRINSIC_BEGIN;
    {
        CT_ASSERT(__is_read_reg(data));
        CT_ASSERT(__is_ct_const(sync));
        CT_ASSERT(sync == sig_done || sync == ctx_swap);

        if (__is_ct_const(count) && count <= 8)
        {
            CT_ASSERT(count != 0);
            if (sync == sig_done)
            {
                __asm cls[read_be, *data, address, 0, __ct_const_val(count)], sig_done[*sig_ptr]
            }
            else
            {
                __asm cls[read_be, *data, address, 0, __ct_const_val(count)], ctx_swap[*sig_ptr]
            }
        }
        else
        {
            if (!__is_ct_const(count))
            {
                CT_QPERFINFO_INDIRECT_REF(cls_read_be);
            }
            {
                _INTRINSIC_IND_ALU_COUNT_MIN_MAX(count, 1, INTRINSIC_IND_ALU_COUNT_MAX);
                if (sync == sig_done)
                {
                    __asm cls[read_be, *data, address, 0, __ct_const_val(count)], sig_done[*sig_ptr], indirect_ref
                }
                else
                {
                    __asm cls[read_be, *data, address, 0, __ct_const_val(count)], ctx_swap[*sig_ptr], indirect_ref
                }
            }
        }
    }
    _INTRINSIC_END;
}


/* Local scratch memory read little endian    */

__intrinsic void cls_read_le_ptr32(
    __xread void *data,                                 /* data to read                           */
    volatile void __cls *address,                       /* address to read from                   */
    unsigned int count,                                 /* number of longwords to read            */
    sync_t sync,                                        /* type of synchronization to use         */
    SIGNAL *sig_ptr                                     /* signal to raise upon completion        */
)
{
    _INTRINSIC_BEGIN;
    {
        CT_ASSERT(__is_read_reg(data));
        CT_ASSERT(__is_ct_const(sync));
        CT_ASSERT(sync == sig_done || sync == ctx_swap);

        if (__is_ct_const(count) && count <= 8)
        {
            CT_ASSERT(count != 0);
            if (sync == sig_done)
            {
                __asm cls[read_le, *data, address, 0, __ct_const_val(count)], sig_done[*sig_ptr]
            }
            else
            {
                __asm cls[read_le, *data, address, 0, __ct_const_val(count)], ctx_swap[*sig_ptr]
            }
        }
        else
        {
            if (!__is_ct_const(count))
            {
                CT_QPERFINFO_INDIRECT_REF(cls_read_le);
            }
            {
                _INTRINSIC_IND_ALU_COUNT_MIN_MAX(count, 1, INTRINSIC_IND_ALU_COUNT_MAX);
                if (sync == sig_done)
                {
                    __asm cls[read_le, *data, address, 0, __ct_const_val(count)], sig_done[*sig_ptr], indirect_ref
                }
                else
                {
                    __asm cls[read_le, *data, address, 0, __ct_const_val(count)], ctx_swap[*sig_ptr], indirect_ref
                }
            }
        }
    }
    _INTRINSIC_END;
}




/* Read longwords from Cluster Local Scratch.    */

__intrinsic void cls_read_ptr32(
    __xread void *data,                                 /* data to read                           */
    volatile void __cls *address,                       /* address to read from                   */
    unsigned int count,                                 /* number of longwords to read            */
    sync_t sync,                                        /* type of synchronization to use         */
    SIGNAL *sig_ptr                                     /* signal to raise upon completion        */
    )
{
#ifdef __BIGENDIAN
    cls_read_be_ptr32(data, address, count, sync, sig_ptr);
#else
    cls_read_le_ptr32(data, address, count, sync, sig_ptr);
#endif
}


/* Local scratch memory write             */

__intrinsic void cls_write_be_ptr32(
    __xwrite void *data,                                /* data to write                          */
    volatile void __addr32 __cls *address,              /* address to write                       */
    unsigned int count,                                 /* number of longwords to write           */
    sync_t sync,                                        /* type of synchronization to use         */
    SIGNAL *sig_ptr                                     /* signal to raise upon completion        */
)
{
    _INTRINSIC_BEGIN;
    {
        CT_ASSERT(__is_write_reg(data));
        CT_ASSERT(__is_ct_const(sync));
        CT_ASSERT(sync == sig_done || sync == ctx_swap);

        if (__is_ct_const(count) && count <= 8)
        {
            CT_ASSERT(count != 0);
            if (sync == sig_done)
            {
                __asm cls[write_be, *data, address, 0, __ct_const_val(count)], sig_done[*sig_ptr]
            }
            else
            {
                __asm cls[write_be, *data, address, 0, __ct_const_val(count)], ctx_swap[*sig_ptr]
            }
        }
        else
        {
            if (!__is_ct_const(count))
            {
                CT_QPERFINFO_INDIRECT_REF(cls_write_be);
            }
            {
                _INTRINSIC_IND_ALU_COUNT_MIN_MAX(count, 1, INTRINSIC_IND_ALU_COUNT_MAX);
                if (sync == sig_done)
                {
                    __asm cls[write_be, *data, address, 0, __ct_const_val(count)], sig_done[*sig_ptr], indirect_ref
                }
                else
                {
                    __asm cls[write_be, *data, address, 0, __ct_const_val(count)], ctx_swap[*sig_ptr], indirect_ref
                }
            }
        }
    }
    _INTRINSIC_END;
}

/* Write longwords to Cluster Local Scratch in Little Endian mode.    */

__intrinsic void cls_write_le_ptr32(
    __xwrite void *data,                                /* data to write                          */
    volatile void __cls *address,                       /* address to write                       */
    unsigned int count,                                 /* number of longwords to write           */
    sync_t sync,                                        /* type of synchronization to use         */
    SIGNAL *sig_ptr                                     /* signal to raise upon completion        */
)
{
    _INTRINSIC_BEGIN;
    {
        CT_ASSERT(__is_write_reg(data));
        CT_ASSERT(__is_ct_const(sync));
        CT_ASSERT(sync == sig_done || sync == ctx_swap);

        if (__is_ct_const(count) && count <= 8)
        {
            CT_ASSERT(count != 0);
            if (sync == sig_done)
            {
                __asm cls[write_le, *data, address, 0, __ct_const_val(count)], sig_done[*sig_ptr]
            }
            else
            {
                __asm cls[write_le, *data, address, 0, __ct_const_val(count)], ctx_swap[*sig_ptr]
            }
        }
        else
        {
            if (!__is_ct_const(count))
            {
                CT_QPERFINFO_INDIRECT_REF(cls_write_le);
            }
            {
                _INTRINSIC_IND_ALU_COUNT_MIN_MAX(count, 1, INTRINSIC_IND_ALU_COUNT_MAX);
                if (sync == sig_done)
                {
                    __asm cls[write_le, *data, address, 0, __ct_const_val(count)], sig_done[*sig_ptr], indirect_ref
                }
                else
                {
                    __asm cls[write_le, *data, address, 0, __ct_const_val(count)], ctx_swap[*sig_ptr], indirect_ref
                }
            }
        }
    }
    _INTRINSIC_END;
}



/* Write longwords to Cluster Local Scratch.      */

__intrinsic void cls_write_ptr32(
    __xwrite void *data,                                /* data to write                          */
    volatile void __cls *address,                       /* address to write                       */
    unsigned int count,                                 /* number of longwords to write           */
    sync_t sync,                                        /* type of synchronization to use         */
    SIGNAL *sig_ptr                                     /* signal to raise upon completion        */
)
{
#ifdef __BIGENDIAN
    cls_write_be_ptr32(data, address, count, sync, sig_ptr);
#else
    cls_write_le_ptr32(data, address, count, sync, sig_ptr);
#endif
}


#define _MEM_ATOMIC_CMD_NO_DATA(cmdname, addr)                                 \
do {                                                                           \
    unsigned int addr_hi, addr_lo;                                             \
                                                                               \
    addr_hi = ((unsigned long long int)addr >> 8) & 0xff000000;                \
    addr_lo = (unsigned long long int)addr & 0xffffffff;                       \
                                                                               \
    __asm { mem[cmdname, --, addr_hi, <<8, addr_lo] }                          \
} while(0)

__intrinsic void
mem_incr64(__mem void *addr)
{
    _MEM_ATOMIC_CMD_NO_DATA(incr64, addr);
}
