/**
 * Copyright (C) 2017,  Earth Computing Inc.  All rights reserved.
 *
 *
 * @file          cls_reorder.h
 * @brief         CLS Reorder code 
 *
 * Author:        Atsushi Kasuya
 *
 */



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

#define _CLS_RING_OFFSET_CHECK(offset)                                  \
        if (__is_ct_const(offset))                                      \
        {                                                               \
            CT_ASSERT(offset <= 0x3fff);                                \
        }                                                               \
        else                                                            \
        {                                                               \
            RT_RANGE_ASSERT(offset <= 0x3fff);                          \
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

#define _CLS_RING_ADDRESS_ORDERED_LOCK(address, ring, offset)                    \
    address = (0xfff << 20) | ((ring & 0x0f) << 16) | ((offset & 0x3fff) << 2) ;


#define _CLS_RING_SET_LOCALITY(locality, island)            \
    locality = (island_id & 0x3f) << 26;


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

//////////////////////////////////////////////////////////////////////
//  nfp_cls_ring code end
//////////////////////////////////////////////////////////////////////


void reorder_ring_init()
{
	cls_ring_setup(REORDER_RING_NUM0, (__cls void *)reorder_ring_mem0, (REORDER_RING_SIZE_LW * 4));
	cls_ring_setup(REORDER_RING_NUM1, (__cls void *)reorder_ring_mem1, (REORDER_RING_SIZE_LW * 4));
}


void reorder_lock( unsigned int ring_num, unsigned int sequence ) {
	SIGNAL sig;
	cls_ring_ordered_lock_ptr32( ring_num, sequence, ctx_swap, &sig ) ;
}

void reorder_unlock( unsigned int ring_num, unsigned int sequence ) {
    cls_ring_ordered_unlock_ptr32(ring_num, sequence ) ;
}


