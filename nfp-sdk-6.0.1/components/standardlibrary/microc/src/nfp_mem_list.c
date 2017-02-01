/*
 * Copyright (C) 2008-2014 Netronome Systems, Inc.  All rights reserved.
 */

#ifndef __NFP_MEM_LIST_C__
#define __NFP_MEM_LIST_C__

#ifndef NFP_LIB_ANY_NFCC_VERSION
    #if (!defined(__NFP_TOOL_NFCC) ||                       \
        (__NFP_TOOL_NFCC < NFP_SW_VERSION(5, 0, 0, 0)) ||   \
        (__NFP_TOOL_NFCC > NFP_SW_VERSION(6, 0, 1, 255)))
        #error "This standard library is not supported for the version of the SDK currently in use."
    #endif
#endif

#include <nfp.h>
#include <nfp_intrinsic.h>
#include <nfp_mem.h>
#include <nfp_mem_list.h>

/*
 * NOTE: When writing inline-asm, it's important not to use reserved words
 *       for variables.  Common ones are 'a','b', 'csr', 'state', and
 *       'inp_state'.
 */


#define _MEM_LIST_Q_ARRAY_NUMBER_CHECK(q_array_number)      \
        if (__is_ct_const(q_array_number))                  \
        {                                                   \
            CT_ASSERT(q_array_number < 3072);               \
        }                                                   \
        else                                                \
        {                                                   \
            RT_RANGE_ASSERT(q_array_number < 3072);         \
        }

#define _MEM_LIST_ALIGN_16_CHECK(addr)                      \
        if (__is_ct_const(addr))                            \
        {                                                   \
            CT_ASSERT((addr & 0xf) == 0);                   \
        }                                                   \
        else                                                \
        {                                                   \
            RT_ALIGN_ASSERT((addr & 0xf) == 0);             \
        }

#define _MEM_LIST_USER_DATA_CHECK(ud)                       \
        if (__is_ct_const(ud))                              \
        {                                                   \
            CT_ASSERT(ud < (1 << 6));                       \
        }                                                   \
        else                                                \
        {                                                   \
            RT_RANGE_ASSERT(ud < (1 << 6));                 \
        }

#define _MEM_LIST_SEG_CNT_CHECK(seg_cnt)                    \
        if (__is_ct_const(seg_cnt))                         \
        {                                                   \
            CT_ASSERT(seg_cnt < (1 << 6));                  \
        }                                                   \
        else                                                \
        {                                                   \
            RT_RANGE_ASSERT(seg_cnt < (1 << 6));            \
        }

#define _MEM_LIST_SIGNAL_CHECK(sig)                         \
        if (__is_ct_const(sig))                             \
        {                                                   \
            CT_ASSERT(sig == sig_done || sig == ctx_swap);  \
        }

#define _MEM_LIST_ISLAND_CHECK(island)                      \
        if (__is_ct_const(island))                          \
        {                                                   \
            CT_ASSERT(island > 0 && island <= 2);           \
        }                                                   \
        else                                                \
        {                                                   \
            RT_RANGE_ASSERT(island > 0 && island <= 2);     \
        }

#ifdef __NFP_INDIRECT_REF_FORMAT_NFP3200
#define _MEM_LIST_QA_OVERRIDE(qa_number)                    \
       __asm { alu[--, --, B, qa_number, <<5] }
#endif

#ifdef __NFP_INDIRECT_REF_FORMAT_NFP6000
#define _MEM_LIST_QA_OVERRIDE(qa_number)                    \
{                                                           \
       generic_ind_t ind;                                   \
       _INTRINSIC_OVERRIDE_FULL_DATA_REF(ind, qa_number);   \
       __asm { alu[--, --, B, ALU_INDIRECT_TYPE(ind)] }     \
}
#endif

#define _MEM_RING_ISLAND_CHECK(island)                                  \
        if (__is_ct_const(island))                                      \
        {                                                               \
            CT_ASSERT(island > 0 && island <= 2);                       \
        }                                                               \
        else                                                            \
        {                                                               \
            RT_RANGE_ASSERT(island > 0 && island <= 2);                 \
        }

#if (_nfp_has_island("i24.mem") && _nfp_has_island("i25.mem") && _nfp_has_island("i26.mem"))
#define _MEM_LIST_CONVERT_QARRAY_TO_HI_ADDRESS(qa_number)                       \
{                                                                               \
    switch (qa_number >> 10)                                                    \
    {                                                                           \
    case 0:                                                                     \
        hi_addr = (unsigned int)((unsigned long long)LoadTimeConstant(i24_emem_base) >> 8);   \
        break;                                                                  \
    case 1:                                                                     \
        hi_addr = (unsigned int)((unsigned long long)LoadTimeConstant(i25_emem_base) >> 8);   \
        break;                                                                  \
    case 2:                                                                     \
        hi_addr = (unsigned int)((unsigned long long)LoadTimeConstant(i26_emem_base)>> 8 );   \
        break;                                                                  \
    default:                                                                    \
        _MEM_RING_ISLAND_CHECK(qa_number >> 10);                                \
    }                                                                           \
    qa_index = qa_number & 0x3ff;                                               \
}
#endif


#if (_nfp_has_island("i24.mem") && _nfp_has_island("i25.mem") && !_nfp_has_island("i26.mem"))
#define _MEM_LIST_CONVERT_QARRAY_TO_HI_ADDRESS(qa_number)                       \
{                                                                               \
    switch (qa_number >> 10)                                                    \
    {                                                                           \
    case 0:                                                                     \
        hi_addr = (unsigned int)((unsigned long long)LoadTimeConstant(i24_emem_base) >> 8);   \
        break;                                                                  \
    case 1:                                                                     \
        hi_addr = (unsigned int)((unsigned long long)LoadTimeConstant(i25_emem_base) >> 8);   \
        break;                                                                  \
    default:                                                                    \
        _MEM_RING_ISLAND_CHECK(qa_number >> 10);                                \
    }                                                                           \
    qa_index = qa_number & 0x3ff;                                               \
}
#endif


#if (_nfp_has_island("i24.mem") && !_nfp_has_island("i25.mem") && !_nfp_has_island("i26.mem"))
#define _MEM_LIST_CONVERT_QARRAY_TO_HI_ADDRESS(qa_number)                       \
{                                                                               \
    switch (qa_number >> 10)                                                    \
    {                                                                           \
    case 0:                                                                     \
        hi_addr = (unsigned int)((unsigned long long)LoadTimeConstant(i24_emem_base) >> 8);   \
        break;                                                                  \
    default:                                                                    \
        _MEM_RING_ISLAND_CHECK(qa_number >> 10);                                \
    }                                                                           \
    qa_index = qa_number & 0x3ff;                                               \
}
#endif





#define _MEM_LIST_READ_DESC(q_number, type, mem_list_desc)              \
{                                                                       \
        unsigned int hi_addr, low_addr;                                 \
        _INTRINSIC_CONVERT_HI_LO_ADDRESS(mem_list_desc);                \
        _MEM_LIST_Q_ARRAY_NUMBER_CHECK(q_number);                       \
        _MEM_LIST_ALIGN_16_CHECK((unsigned int)mem_list_desc);          \
        if (__is_ct_const(mem_list_desc_in_mem->q_type))                \
        {                                                               \
            CT_ASSERT(mem_list_desc->q_type == type);                   \
        }                                                               \
        _MEM_LIST_QA_OVERRIDE(q_number & 0x3ff);                        \
        __asm mem[rd_qdesc, --, hi_addr, <<8, low_addr ], indirect_ref  \
}

/* Extract the 8 top bits from a  40 bit address */
#define _MEM_LIST_EXTRACT_HI_ADDRESS(address)                           \
    hi_addr = (unsigned int)((unsigned long long)address>>32);          \
    hi_addr = (unsigned int)hi_addr<<24;


__intrinsic
void mem_list_read_desc_t0(
    unsigned int q_array_number,
    mem_list_desc_in_mem_t0 *mem_list_desc_in_mem
)
{
    _INTRINSIC_BEGIN;
    {
        _MEM_LIST_READ_DESC(q_array_number, 0, mem_list_desc_in_mem);
    }
    _INTRINSIC_END;
}

_intrinsic
void mem_list_read_desc_t1(
    unsigned int q_array_number,
    mem_list_desc_in_mem_t1 *mem_list_desc_in_mem
)
{
    _INTRINSIC_BEGIN;
    {
        _MEM_LIST_READ_DESC(q_array_number, 1, mem_list_desc_in_mem);
    }
    _INTRINSIC_END;
}

_intrinsic
void mem_list_read_desc_t2(
    unsigned int q_array_number,
    mem_list_desc_in_mem_t2 *mem_list_desc_in_mem
)
{
    _INTRINSIC_BEGIN;
    {
        _MEM_LIST_READ_DESC(q_array_number, 2, mem_list_desc_in_mem);
    }
    _INTRINSIC_END;
}

_intrinsic
void mem_list_read_desc_t3(
    unsigned int q_array_number,
    mem_list_desc_in_mem_t3 *mem_list_desc_in_mem
)
{
    _INTRINSIC_BEGIN;
    {
        _MEM_LIST_READ_DESC(q_array_number, 3, mem_list_desc_in_mem);
    }
    _INTRINSIC_END;
}

#define _MEM_LIST_PUSH_DESC(q_number, xfer)                                                 \
{                                                                                           \
        __gpr unsigned int qa_index;                                           \
        unsigned int hi_addr;                                                               \
        _MEM_LIST_Q_ARRAY_NUMBER_CHECK(q_number);                                           \
        _MEM_LIST_CONVERT_QARRAY_TO_HI_ADDRESS(q_number);                                   \
        CT_ASSERT(__is_read_reg(xfer));                                                     \
        __implicit_write(xfer);                                                             \
        if (sync == sig_done)                                                               \
        {                                                                                   \
            __asm { mem[push_qdesc, *xfer, hi_addr, << 8, qa_index ], sig_done[*sig_ptr] }  \
        }                                                                                   \
        else                                                                                \
        {                                                                                   \
            __asm { mem[push_qdesc, *xfer, hi_addr, << 8, qa_index ], ctx_swap[*sig_ptr]}   \
        }                                                                                   \
}

_intrinsic
void mem_list_push_desc_t0(
    unsigned int q_array_number,
    __xread mem_list_desc_t0 *xfer,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
        _MEM_LIST_PUSH_DESC(q_array_number, xfer)
    }
    _INTRINSIC_END;
}

__intrinsic
void mem_list_push_desc_t1(
    unsigned int q_array_number,
    __xread mem_list_desc_t1 *xfer,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
        _MEM_LIST_PUSH_DESC(q_array_number, xfer);
    }
    _INTRINSIC_END;
}

__intrinsic
void
mem_list_push_desc_t2(
    unsigned int q_array_number,
    __xread mem_list_desc_t2 *xfer,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
        _MEM_LIST_PUSH_DESC(q_array_number, xfer)
    }
    _INTRINSIC_END;
}

__intrinsic
void
mem_list_push_desc_t3(
    unsigned int q_array_number,
    __xread mem_list_desc_t3 *xfer,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
        _MEM_LIST_PUSH_DESC(q_array_number, xfer)
    }
    _INTRINSIC_END;
}

#define _MEM_LIST_WRITE_DESC(q_number, list_desc, type)                         \
{                                                                               \
        unsigned int hi_addr, low_addr;                                         \
        _INTRINSIC_CONVERT_HI_LO_ADDRESS(mem_list_desc_in_mem);                 \
        _MEM_LIST_Q_ARRAY_NUMBER_CHECK(q_number);                               \
        _MEM_LIST_ALIGN_16_CHECK((unsigned int)mem_list_desc_in_mem);           \
        if (__is_ct_const(mem_list_desc_in_mem->q_type))                        \
        {                                                                       \
            CT_ASSERT(mem_list_desc_in_mem->q_type == type);                    \
        }                                                                       \
        {                                                                       \
            _MEM_LIST_QA_OVERRIDE(q_number);                                    \
            __asm mem[wr_qdesc, --, hi_addr, << 8, low_addr ], indirect_ref     \
        }                                                                       \
}

__intrinsic
void
mem_list_write_desc_t0(
    unsigned int q_array_number,
    mem_list_desc_in_mem_t0 *mem_list_desc_in_mem
)
{
    _INTRINSIC_BEGIN;
    {
        _MEM_LIST_WRITE_DESC(q_array_number, mem_list_desc_in_mem, TYPE_0)
    }
    _INTRINSIC_END;
}

__intrinsic
void
mem_list_write_desc_t1(
    unsigned int q_array_number,
    mem_list_desc_in_mem_t1 *mem_list_desc_in_mem
)
{
    _INTRINSIC_BEGIN;
    {
        _MEM_LIST_WRITE_DESC(q_array_number, mem_list_desc_in_mem, TYPE_1)
    }
    _INTRINSIC_END;
}

__intrinsic
void
mem_list_write_desc_t2(
    unsigned int q_array_number,
    mem_list_desc_in_mem_t2 *mem_list_desc_in_mem
)
{
    _INTRINSIC_BEGIN;
    {
        _MEM_LIST_WRITE_DESC(q_array_number, mem_list_desc_in_mem, TYPE_2)
    }
    _INTRINSIC_END;
}

__intrinsic
void
mem_list_write_desc_t3(
    unsigned int q_array_number,
    mem_list_desc_in_mem_t3 *mem_list_desc_in_mem
)
{
    _INTRINSIC_BEGIN;
    {
        _MEM_LIST_WRITE_DESC(q_array_number, mem_list_desc_in_mem, TYPE_3)
    }
    _INTRINSIC_END;
}




__intrinsic
void mem_list_enqueue_t0(unsigned int q_array_number, unsigned int sop, unsigned int eop, unsigned int seg_cnt,
                            mem_list_link_in_mem_t0 *next_ptr
                            )
{
    _INTRINSIC_BEGIN;
    {
        __gpr mem_list_link_t0 mem_list_link;
        unsigned int hi_addr;

        _MEM_LIST_EXTRACT_HI_ADDRESS(next_ptr);
        _MEM_LIST_Q_ARRAY_NUMBER_CHECK(q_array_number);
        _MEM_LIST_SEG_CNT_CHECK(seg_cnt);
        _MEM_LIST_ALIGN_16_CHECK((unsigned int)next_ptr);

        mem_list_link.sop = sop;
        mem_list_link.eop = eop;
        mem_list_link.seg_cnt = seg_cnt;
        mem_list_link.next_ptr = ((unsigned int)next_ptr >> 2);

        _MEM_LIST_QA_OVERRIDE(q_array_number & 0x3ff);
        __asm mem[enqueue, --, hi_addr, << 8, mem_list_link], indirect_ref
    }
    _INTRINSIC_END;
}

__intrinsic
void mem_list_enqueue_t1(unsigned int q_array_number, unsigned int sop, unsigned int eop, unsigned int user_data,
                            mem_list_link_in_mem_t1 *next_ptr
                            )
{
    _INTRINSIC_BEGIN;
    {
        __gpr mem_list_link_t1 mem_list_link;
        unsigned int hi_addr;

        _MEM_LIST_EXTRACT_HI_ADDRESS(next_ptr);
        _MEM_LIST_Q_ARRAY_NUMBER_CHECK(q_array_number);
        _MEM_LIST_ALIGN_16_CHECK((unsigned int)next_ptr);

        mem_list_link.sop = sop;
        mem_list_link.eop = eop;
        mem_list_link.user_data = user_data;
        mem_list_link.next_ptr = ((unsigned int)next_ptr >> 2);

        _MEM_LIST_QA_OVERRIDE(q_array_number & 0x3ff);
        __asm mem[enqueue, --, hi_addr, << 8, mem_list_link], indirect_ref
    }
    _INTRINSIC_END;
}

__intrinsic
void mem_list_enqueue_t2(unsigned int q_array_number, unsigned int sop, unsigned int eop,
                            mem_list_link_in_mem_t2 *next_ptr
                            )
{
    _INTRINSIC_BEGIN;
    {
        __gpr mem_list_link_t2 mem_list_link;
        unsigned int hi_addr;

        _MEM_LIST_EXTRACT_HI_ADDRESS(next_ptr);
        _MEM_LIST_Q_ARRAY_NUMBER_CHECK(q_array_number);
        _MEM_LIST_ALIGN_16_CHECK((unsigned int)next_ptr);

        mem_list_link.sop = sop;
        mem_list_link.eop = eop;
        mem_list_link.next_ptr = ((unsigned int)next_ptr >> 2);

        _MEM_LIST_QA_OVERRIDE(q_array_number & 0x3ff);
        __asm mem[enqueue, --, hi_addr, << 8, mem_list_link], indirect_ref
    }
    _INTRINSIC_END;
}

__intrinsic
void mem_list_enqueue_t3(unsigned int q_array_number, unsigned int sop, unsigned int eop, unsigned int seg_cnt,
                            mem_list_link_in_mem_t3 *next_ptr
                            )
{
    _INTRINSIC_BEGIN;
    {
        __gpr mem_list_link_t3 mem_list_link;
        unsigned int hi_addr;

        _MEM_LIST_EXTRACT_HI_ADDRESS(next_ptr);
        _MEM_LIST_Q_ARRAY_NUMBER_CHECK(q_array_number);
        _MEM_LIST_SEG_CNT_CHECK(seg_cnt);
        _MEM_LIST_ALIGN_16_CHECK((unsigned int)next_ptr);

        /* Zero is not allowed */
        if (__is_ct_const(seg_cnt))
        {
            CT_ASSERT(seg_cnt > 0);
        }
        else
        {
            RT_RANGE_ASSERT(seg_cnt > 0);
        }
        mem_list_link.sop = sop;
        mem_list_link.eop = eop;
        mem_list_link.seg_cnt = seg_cnt;
        mem_list_link.next_ptr = ((unsigned int)next_ptr >> 2);

        _MEM_LIST_QA_OVERRIDE(q_array_number & 0x3ff);
        __asm mem[enqueue, --, hi_addr, << 8, mem_list_link], indirect_ref
    }
    _INTRINSIC_END;
}





#define _MEM_LIST_ENQUEUE_TAIL(q_number, list_tail)                         \
{                                                                           \
        __gpr mem_list_link_t0 mem_list_link;                  \
        unsigned int hi_addr;                                               \
        _MEM_LIST_EXTRACT_HI_ADDRESS(list_tail);                            \
        _MEM_LIST_Q_ARRAY_NUMBER_CHECK(q_array_number);                     \
        _MEM_LIST_ALIGN_16_CHECK((unsigned int)list_tail);                  \
        mem_list_link.value = 0;                                            \
        mem_list_link.next_ptr = ((unsigned int)list_tail >> 2);            \
        {                                                                   \
            _MEM_LIST_QA_OVERRIDE(q_array_number);                          \
            __asm mem[enqueue_tail, --, hi_addr, << 8, mem_list_link], indirect_ref\
        }                                                                   \
}

__intrinsic
void mem_list_enqueue_tail_t0(unsigned int q_array_number, mem_list_link_in_mem_t0 *tail)
{
    _INTRINSIC_BEGIN;
    {
        _MEM_LIST_ENQUEUE_TAIL(q_array_number, tail)
    }
    _INTRINSIC_END;
}

__intrinsic
void mem_list_enqueue_tail_t1(unsigned int q_array_number, mem_list_link_in_mem_t1 *tail)
{
    _INTRINSIC_BEGIN;
    {
        _MEM_LIST_ENQUEUE_TAIL(q_array_number, tail)
    }
    _INTRINSIC_END;
}

__intrinsic
void mem_list_enqueue_tail_t2(unsigned int q_array_number, mem_list_link_in_mem_t2 *tail)
{
    _INTRINSIC_BEGIN;
    {
        _MEM_LIST_ENQUEUE_TAIL(q_array_number, tail)
    }
    _INTRINSIC_END;
}

__intrinsic
void mem_list_enqueue_tail_t3(unsigned int q_array_number, mem_list_link_in_mem_t3 *tail)
{
    _INTRINSIC_BEGIN;
    {
        _MEM_LIST_ENQUEUE_TAIL(q_array_number, tail)
    }
    _INTRINSIC_END;
}



#define _MEM_LIST_DEQUEUE(q_number, xfer)                               \
{                                                                       \
        unsigned int hi_addr, qa_index;                                 \
        _MEM_LIST_Q_ARRAY_NUMBER_CHECK(q_array_number);                 \
        _MEM_LIST_SIGNAL_CHECK(sync);                                   \
        _MEM_LIST_CONVERT_QARRAY_TO_HI_ADDRESS(q_array_number);         \
        if (sync == sig_done)                                           \
        {                                                               \
            __asm mem[dequeue, *mem_list_link, hi_addr, << 8, qa_index], sig_done[*sig_ptr] \
        }                                                               \
        else                                                            \
        {                                                               \
            __asm mem[dequeue, *mem_list_link, hi_addr, << 8, qa_index], ctx_swap[*sig_ptr] \
        }                                                               \
}

__intrinsic
void
mem_list_dequeue_t0(__xread mem_list_link_t0 *mem_list_link,
                        unsigned int q_array_number,
                        sync_t sync,
                        SIGNAL *sig_ptr
                        )
{
    _INTRINSIC_BEGIN;
    {
        _MEM_LIST_DEQUEUE(q_array_number, mem_list_link)
    }
    _INTRINSIC_END;
}

__intrinsic
void
mem_list_dequeue_t1(__xread mem_list_link_t1 *mem_list_link,
                        unsigned int q_array_number,
                        sync_t sync,
                        SIGNAL *sig_ptr
                        )
{
    _INTRINSIC_BEGIN;
    {
        _MEM_LIST_DEQUEUE(q_array_number, mem_list_link)
     }
    _INTRINSIC_END;
}

__intrinsic
void
mem_list_dequeue_t2(__xread mem_list_link_t2 *mem_list_link,
                        unsigned int q_array_number,
                        sync_t sync,
                        SIGNAL *sig_ptr
                    )
{
    _INTRINSIC_BEGIN;
    {
         _MEM_LIST_DEQUEUE(q_array_number, mem_list_link)
     }
    _INTRINSIC_END;
}

__intrinsic
void
mem_list_dequeue_t3(__xread mem_list_link_t3 *mem_list_link,
                        unsigned int q_array_number,
                        sync_t sync,
                        SIGNAL *sig_ptr
                        )
{
    _INTRINSIC_BEGIN;
    {
        _MEM_LIST_DEQUEUE(q_array_number, mem_list_link)
     }
    _INTRINSIC_END;
}



/*
* Setup the descriptor data in emem
* Wait for descriptor to get to emem by reading it back
*
* Setup the Q descriptor for list_num
*/
#define MEM_LIST_DESC_TO_APPLICABLE_ISLAND(base_address)                                                                            \
{                                                                                                                                   \
    {                                                                                                                               \
        __addr40 __mem mem_list_desc_in_mem_t0 *ptr = (__addr40 __mem mem_list_desc_in_mem_t0*)&base_address;                       \
        volatile __xread unsigned int read_back;                                                                                    \
        mem_write_atomic_ptr40((void*)&mem_list_desc, ptr, 4, ctx_swap, &sig);                                                      \
        mem_read_atomic_ptr40((void*)&read_back, ptr, 1, ctx_swap, &sig);                                                           \
    }                                                                                                                               \
    {                                                                                                                               \
        mem_list_desc_in_mem_t0  *mem_list_desc_in_mem = (mem_list_desc_in_mem_t0 *)&base_address;                                  \
        mem_list_read_desc_t0(q_array_number, mem_list_desc_in_mem);                                                                \
    }                                                                                                                               \
}

__intrinsic
void mem_list_init(unsigned int q_array_number, enum MEM_LIST_TYPE type)
{
    unsigned int                                        island_offset = (q_array_number >> 10);
    volatile __xwrite mem_list_desc_t0     mem_list_desc;           /* Ring descriptor */
    SIGNAL                                              sig;

    /*
     * Check parameters
     */
    _MEM_LIST_Q_ARRAY_NUMBER_CHECK(q_array_number);

    if (__is_ct_const(island_offset))
    {
        #if !(_nfp_has_island("i24.mem"))
            if (island_offset == 0)
            {
                CT_ASSERT((island_offset) == 24);       // i24.emem is not supported
            }
        #endif
        #if (!_nfp_has_island("i25.mem"))
            if (island_offset == 1)
            {
                CT_ASSERT((island_offset) == 25);       // i25.emem is not supported
            }
        #endif
        #if !(_nfp_has_island("i26.mem"))
            if (island_offset == 2)
            {
                CT_ASSERT((island_offset) == 26);       // i26.emem is not supported
            }
        #endif
    }
    else
    {
        #if !(_nfp_has_island("i24.mem"))
            if (island_offset == 0)
            {
                RT_ASSERT((island_offset) == 24);
            }
        #endif
        #if (!_nfp_has_island("i25.mem"))
            if (island_offset == 1)
            {
                RT_ASSERT((island_offset) == 25);
            }
        #endif
        #if !(_nfp_has_island("i26.mem"))
            if (island_offset == 2)
            {
                RT_ASSERT((island_offset) == 26);
            }
        #endif
    }
    /*
     * Configure descriptor - Only q_type is set and q_count must be 0
     */
    mem_list_desc.value[0] = 0;
    mem_list_desc.value[1] = type;
    mem_list_desc.value[2] = 0;
    mem_list_desc.value[3] = 0;


    /* q number between 0 and 1023 means first emem island (i24) */
    if (island_offset == 0)
    {
    #if (_nfp_has_island("i24.mem"))
       __emem_n(0) __addr40 __align16 mem_list_desc_t0      emem_addr;      // i24
       MEM_LIST_DESC_TO_APPLICABLE_ISLAND(emem_addr) ;
    #endif
    }
    else if (island_offset == 1)      /* q number between 1023 and 2046 means second emem island (i25) */
    {
    #if (_nfp_has_island("i25.mem"))
       __emem_n(1) __addr40 __align16 mem_list_desc_t0      emem_addr;      // i25
       MEM_LIST_DESC_TO_APPLICABLE_ISLAND(emem_addr) ;
    #endif
    }
    else if (island_offset == 2)     /* q number between 2046 and 3069 means third emem island (i26) */
    {
    #if (_nfp_has_island("i26.mem"))
      __emem_n(2) __addr40 __align16 mem_list_desc_t0      emem_addr;      // i26
        MEM_LIST_DESC_TO_APPLICABLE_ISLAND(emem_addr) ;
    #endif
    }
}



__intrinsic
void mem_list_init_with_loc_and_page(unsigned int q_array_number, enum MEM_LIST_TYPE type, unsigned int loc, unsigned int page)
{
    mem_list_desc_t0                                mem_list_desc;              /* list descriptor */
    unsigned int                                    island_offset = (q_array_number >> 10);
    SIGNAL                                          sig;

    /*
     * Check parameters
     */
    _MEM_LIST_Q_ARRAY_NUMBER_CHECK(q_array_number);
    CT_ASSERT(loc < 4);
    RT_RANGE_ASSERT(loc < 4);

    CT_ASSERT(page < 4);
    RT_RANGE_ASSERT(page < 4);

    if (__is_ct_const(island_offset))
    {
        #if !(_nfp_has_island("i24.mem"))
            if (island_offset == 0)
            {
                CT_ASSERT((island_offset) == 24);       // i24.emem is not supported
            }
        #endif
        #if (!_nfp_has_island("i25.mem"))
            if (island_offset == 1)
            {
                CT_ASSERT((island_offset) == 25);       // i25.emem is not supported
            }
        #endif
        #if !(_nfp_has_island("i26.mem"))
            if (island_offset == 2)
            {
                CT_ASSERT((island_offset) == 26);       // i26.emem is not supported
            }
        #endif
    }
    else
    {
        #if !(_nfp_has_island("i24.mem"))
            if (island_offset == 0)
            {
                RT_ASSERT((island_offset) == 24);
            }
        #endif
        #if (!_nfp_has_island("i25.mem"))
            if (island_offset == 1)
            {
                RT_ASSERT((island_offset) == 25);
            }
        #endif
        #if !(_nfp_has_island("i26.mem"))
            if (island_offset == 2)
            {
                RT_ASSERT((island_offset) == 26);
            }
        #endif
    }
    /*
     * Configure descriptor - Only q_type is set and q_count must be 0
     */
    mem_list_desc.value[0] = 0;
    mem_list_desc.value[1] = type;
    mem_list_desc.value[2] = loc << 30 | page << 24;
    mem_list_desc.value[3] = 0;

    /* q number between 0 and 1023 means first emem island (i24) */
    if (island_offset == 0)
    {
    #if (_nfp_has_island("i24.mem"))
       __emem_n(0) __addr40 __align16 mem_list_desc_t0      emem_addr;      // i24
       MEM_LIST_DESC_TO_APPLICABLE_ISLAND(emem_addr) ;
    #endif
    }
    else if (island_offset == 1)      /* q number between 1023 and 2046 means second emem island (i25) */
    {
    #if (_nfp_has_island("i25.mem"))
       __emem_n(1) __addr40 __align16 mem_list_desc_t0      emem_addr;      // i25
       MEM_LIST_DESC_TO_APPLICABLE_ISLAND(emem_addr) ;
    #endif
    }
    else if (island_offset == 2)     /* q number between 2046 and 3069 means third emem island (i26) */
    {
    #if (_nfp_has_island("i26.mem"))
        __emem_n(2) __addr40 __align16 mem_list_desc_t0      emem_addr;      // i26
        MEM_LIST_DESC_TO_APPLICABLE_ISLAND(emem_addr) ;
    #endif
    }
}

#endif /* __NFP_MEM_LIST_C__ */
