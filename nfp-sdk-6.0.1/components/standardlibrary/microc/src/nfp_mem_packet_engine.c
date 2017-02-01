/*
 * Copyright (C) 2008-2014 Netronome Systems, Inc.  All rights reserved.
 */

#ifndef __NFP_MEM_PACKET_ENGINE_C__
#define __NFP_MEM_PACKET_ENGINE_C__

#ifndef NFP_LIB_ANY_NFCC_VERSION
    #if (!defined(__NFP_TOOL_NFCC) ||                       \
        (__NFP_TOOL_NFCC < NFP_SW_VERSION(5, 0, 0, 0)) ||   \
        (__NFP_TOOL_NFCC > NFP_SW_VERSION(6, 0, 1, 255)))
        #error "This standard library is not supported for the version of the SDK currently in use."
    #endif
#endif

#include <nfp.h>
#include <nfp_intrinsic.h>
#include <nfp_mem_packet_engine.h>

/*
 * NOTE: When writing inline-asm, it's important not to use reserved words
 *       for variables.  Common ones are 'a','b', 'csr', 'state', and
 *       'inp_state'.
 */

 /* constants used internally */
 #define FIXED_REF_COUNT                            1           /* for instructions with fixed ref count of 1 */
 #define MAX_PACKET_NUMBER                          0x1ff       /* max bits for packet number is 9 but in some cases documentation refers to 10 */


 #define _MEM_PACKET_LENGTH_CHECK(length)                           \
        if (__is_ct_const(length))                                  \
        {                                                           \
            CT_ASSERT(length <= 0x3fff);                            \
        }                                                           \
        else                                                        \
        {                                                           \
            RT_RANGE_ASSERT(length <= 0x3fff);                      \
        }

 #define _MEM_CREDIT_BUCKET_CHECK(bucket)                           \
        if (__is_ct_const(bucket))                                  \
        {                                                           \
            CT_ASSERT(bucket <= 2);                                 \
        }                                                           \
        else                                                        \
        {                                                           \
            RT_RANGE_ASSERT(bucket <= 2);                           \
        }

 #define _MEM_REF_COUNT_CHECK(ref, min, max)                        \
        if (__is_ct_const(ref))                                     \
        {                                                           \
            CT_ASSERT(ref <= max && ref >= min);                    \
        }                                                           \
        else                                                        \
        {                                                           \
            RT_RANGE_ASSERT(ref <= max && ref >= min);              \
        }

 #define _MEM_PACKET_NUMBER_CHECK(packet_num)                       \
        if (__is_ct_const(packet_num))                              \
        {                                                           \
            CT_ASSERT(packet_num <= MAX_PACKET_NUMBER);             \
        }                                                           \
        else                                                        \
        {                                                           \
            RT_RANGE_ASSERT(packet_num <= MAX_PACKET_NUMBER);       \
        }

 #define _MEM_PACKET_OFFSET_CHECK(offset, max)                      \
        if (__is_ct_const(offset))                                  \
        {                                                           \
            CT_ASSERT(offset <= max);                               \
        }                                                           \
        else                                                        \
        {                                                           \
            RT_RANGE_ASSERT(offset <= max);                         \
        }

 #define _MEM_PACKET_TRANSFER_TYPE_CHECK(type)                      \
        if (__is_ct_const(type))                                    \
        {                                                           \
            CT_ASSERT(type <= 1);                                   \
        }                                                           \
        else                                                        \
        {                                                           \
            RT_RANGE_ASSERT(type <= 1);                             \
        }

 // Verify 8 bits in length
 #define _MEM_CTM_ISLAND_CHECK(island)                              \
        if (__is_ct_const(island))                                  \
        {                                                           \
            CT_ASSERT(island <= 0xffff);                            \
        }                                                           \
        else                                                        \
        {                                                           \
            RT_RANGE_ASSERT(island <= 0xffff);                      \
        }

// Verify 8 byte aligned
 #define _MEM_8_BYTE_ALIGNMENT_CHECK(value)                         \
        if (__is_ct_const(value))                                   \
        {                                                           \
            CT_ASSERT(value == ((value >> 3) << 3));                \
        }                                                           \
        else                                                        \
        {                                                           \
            RT_RANGE_ASSERT(value == ((value >> 3) << 3));          \
        }


#define _MEM_PACKET_COMPLETE_OVERRIDE(indirect, packet)                                                                                         \
{                                                                                                                                               \
    indirect.alu_ind.value = ovr_signal_and_data_master_bit | ovr_byte_mask_bit | ovr_length_bit | ovr_data_full_ref_bit;                       \
    indirect.alu_ind.value = indirect.alu_ind.value | ((packet.script_offset & 0x1f) << 8) ;                                                    \
    indirect.alu_ind.value = indirect.alu_ind.value |  (((packet.nbi_number << 12) | (packet.retry_bit << 10) | packet.tx_queue_number) << 16); \
    indirect.csr_ind.value = (packet.sequencer_number & 0xff);                                                                                  \
    indirect.csr_ind.value = indirect.csr_ind.value | (packet.sequence_number << 16);                                                           \
}

#define _MEM_DMA_MEMORY_BUFFER_OVERRIDE(indirect, length, emem_hi_addr, ctm_addr)                                                               \
{                                                                                                                                               \
    indirect.alu_ind.value = ovr_length_bit | ovr_data_16bit_imm_bit | ovr_data_master_bit | ovr_byte_mask_bit;                                 \
    indirect.alu_ind.value = indirect.alu_ind.value | ((length - 1) << 8);                                                                      \
    indirect.alu_ind.value = indirect.alu_ind.value | ((((unsigned int)ctm_addr >> 3) & 0xffff) << 16);                                         \
    indirect.csr_ind.value = (emem_hi_addr & 0xff);                                                                                             \
    indirect.csr_ind.value = indirect.csr_ind.value | ((((__island() << 4) | (((unsigned int)ctm_addr >> 17) & 1)) & 0x3ff) << 20);             \
}

#define _MEM_DMA_MEMORY_PACKET_OVERRIDE(indirect, island, offset, pkt_number)                                                                   \
{                                                                                                                                               \
    indirect.alu_ind.value = ovr_length_bit | ovr_data_16bit_imm_bit | ovr_byte_mask_bit;                                                       \
    indirect.alu_ind.value = indirect.alu_ind.value | (((island >> 3) & 0x1f) << 8);                                                            \
    indirect.alu_ind.value = indirect.alu_ind.value | ((pkt_number & 0xffff) << 16);                                                            \
    indirect.csr_ind.value = (offset & 0xff);                                                                                                   \
}

#define _MEM_DMA_MEMORY_PACKET_CONVERT_HI_LO_ADDRESS(address, ctm_island_id)        \
{                                                                                   \
    hi_addr = (unsigned int)((unsigned long long)address>>32);                      \
    mu_island = hi_addr & 0xff;                                                     \
    if (ctm_island_id == 0)                                                         \
    {                                                                               \
        hi_addr = 0;                                                                \
    }                                                                               \
    else                                                                            \
    {                                                                               \
        hi_addr = (ctm_island_id << 24);                                            \
    }                                                                               \
    low_addr = (unsigned int)address;                                               \
    low_addr = ((low_addr >> 3) & 0x1fffffff) | ((mu_island & 0x7) << 29);          \
}

void mem_packet_credit_get(
    __xread void *data,
    enum MEM_PACKET_MASTER_BUCKET credit_bucket,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
        CT_ASSERT(__is_ct_const(sync));
        CT_ASSERT(sync == sig_done || sync == ctx_swap);
        CT_ASSERT(__is_read_reg(data));

        _MEM_CREDIT_BUCKET_CHECK(credit_bucket);

        {
            __gpr unsigned int address;

            address = (credit_bucket & 3);

            if (sync == sig_done)
            {
                __asm
                {
                    mem[packet_credit_get, *data, address, 0, FIXED_REF_COUNT], sig_done[*sig_ptr]
                }
            }
            else
            {
                __asm
                {
                    mem[packet_credit_get, *data, address, 0, FIXED_REF_COUNT], ctx_swap[*sig_ptr]
                }
            }
        }
    }
    _INTRINSIC_END;
}

void mem_packet_alloc(
    __xread mem_packet_alloc_response_t *data,
    enum MEM_PACKET_MASTER_BUCKET credit_bucket,
    enum MEM_PACKET_LENGTH packet_length,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
        CT_ASSERT(__is_ct_const(sync));
        CT_ASSERT(sync == sig_done || sync == ctx_swap);
        CT_ASSERT(__is_read_reg(data));

        _MEM_CREDIT_BUCKET_CHECK(credit_bucket);

        {
            __gpr unsigned int address;
            generic_ind_t ind;

            _INTRINSIC_OVERRIDE_LENGTH(ind, packet_length);

            address = (credit_bucket & 3);

            if (sync == sig_done)
            {
                __asm
                {
                    alu[--, --, B, ALU_INDIRECT_TYPE(ind)]
                    mem[packet_alloc, *data, address, 0, FIXED_REF_COUNT], sig_done[*sig_ptr], indirect_ref
                }
            }
            else
            {
                __asm
                {
                    alu[--, --, B, ALU_INDIRECT_TYPE(ind)]
                    mem[packet_alloc, *data, address, 0, FIXED_REF_COUNT], ctx_swap[*sig_ptr], indirect_ref
                }
            }
        }
    }
    _INTRINSIC_END;
}

void mem_packet_alloc_poll(
    __xread mem_packet_alloc_response_t *data,
    enum MEM_PACKET_MASTER_BUCKET credit_bucket,
    enum MEM_PACKET_LENGTH packet_length,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
        CT_ASSERT(__is_ct_const(sync));
        CT_ASSERT(sync == sig_done || sync == ctx_swap);
        CT_ASSERT(__is_read_reg(data));

        _MEM_CREDIT_BUCKET_CHECK(credit_bucket);

        {
            __gpr unsigned int address;
            generic_ind_t ind;

            _INTRINSIC_OVERRIDE_LENGTH(ind, packet_length);
            address = (credit_bucket & 3);

            if (sync == sig_done)
            {
                __asm
                {
                    alu[--, --, B, ALU_INDIRECT_TYPE(ind)]
                    mem[packet_alloc_poll, *data, address, 0, FIXED_REF_COUNT], sig_done[*sig_ptr],  indirect_ref
                }
            }
            else
            {
                __asm
                {
                    alu[--, --, B, ALU_INDIRECT_TYPE(ind)]
                    mem[packet_alloc_poll, *data, address, 0, FIXED_REF_COUNT], ctx_swap[*sig_ptr],  indirect_ref
                }
            }
        }
    }
    _INTRINSIC_END;
}


void mem_packet_free(
    unsigned int packet_number
)
{
    _INTRINSIC_BEGIN;
    {

        _MEM_PACKET_NUMBER_CHECK(packet_number);

        {
            __gpr unsigned int address;

            address = packet_number & MAX_PACKET_NUMBER;

            __asm
            {
                mem[packet_free, --, address, 0, --]
            }
        }
    }
    _INTRINSIC_END;
}


void mem_packet_free_and_return_pointer(
    __xread void *data,
    unsigned int packet_number,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
        CT_ASSERT(__is_ct_const(sync));
        CT_ASSERT(sync == sig_done || sync == ctx_swap);
        CT_ASSERT(__is_read_reg(data));

        _MEM_PACKET_NUMBER_CHECK(packet_number);

        {
            __gpr unsigned int address;

            address = (packet_number & MAX_PACKET_NUMBER);

            if (sync == sig_done)
            {
                __asm
                {
                    mem[packet_free_and_return_pointer, *data, address, 0, FIXED_REF_COUNT], sig_done[*sig_ptr]
                }
            }
            else
            {
                __asm
                {
                    mem[packet_free_and_return_pointer, *data, address, 0, FIXED_REF_COUNT], ctx_swap[*sig_ptr]
                }
            }
        }
    }
    _INTRINSIC_END;
}


void mem_packet_free_and_signal(
    unsigned int packet_number,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
        CT_ASSERT(__is_ct_const(sync));
        CT_ASSERT(sync == sig_done || sync == ctx_swap);

        _MEM_PACKET_NUMBER_CHECK(packet_number);

        {
            __gpr unsigned int address;

            address = (packet_number & MAX_PACKET_NUMBER);

            if (sync == sig_done)
            {
                __asm
                {
                    mem[packet_free_and_signal, --, address, 0, --], sig_done[*sig_ptr]
                }
            }
            else
            {
                __asm
                {
                    mem[packet_free_and_signal, --, address, 0, --], ctx_swap[*sig_ptr]
                }
            }
        }
    }
    _INTRINSIC_END;
}


void mem_packet_return_pointer(
    __xread void *data,
    unsigned int packet_number,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
        CT_ASSERT(__is_ct_const(sync));
        CT_ASSERT(sync == sig_done || sync == ctx_swap);
        CT_ASSERT(__is_read_reg(data));

        _MEM_PACKET_NUMBER_CHECK(packet_number);

        {
            __gpr unsigned int address;

            address = (packet_number & MAX_PACKET_NUMBER);

            if (sync == sig_done)
            {
                __asm
                {
                    mem[packet_return_pointer, *data, address, 0, FIXED_REF_COUNT], sig_done[*sig_ptr]
                }
            }
            else
            {
                __asm
                {
                    mem[packet_return_pointer, *data, address, 0, FIXED_REF_COUNT], ctx_swap[*sig_ptr]
                }
            }
        }
    }
    _INTRINSIC_END;
}


void mem_packet_read_packet_status(
    __xread mem_packet_read_status_response_t *data,
    unsigned int packet_number,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
        CT_ASSERT(__is_ct_const(sync));
        CT_ASSERT(sync == sig_done || sync == ctx_swap);
        CT_ASSERT(__is_read_reg(data));

        _MEM_PACKET_NUMBER_CHECK(packet_number);

        {
            __gpr unsigned int address;

            address = (packet_number & MAX_PACKET_NUMBER);

            if (sync == sig_done)
            {
                __asm
                {
                    mem[packet_read_packet_status, *data, address, 0, FIXED_REF_COUNT], sig_done[*sig_ptr]
                }
            }
            else
            {
                __asm
                {
                    mem[packet_read_packet_status, *data, address, 0, FIXED_REF_COUNT], ctx_swap[*sig_ptr]
                }
            }
        }
    }
    _INTRINSIC_END;
}



void mem_packet_wait_packet_status(
    __xread mem_packet_read_status_response_t *data,
    unsigned int packet_number,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
        CT_ASSERT(__is_ct_const(sync));
        CT_ASSERT(sync == sig_done || sync == ctx_swap);
        CT_ASSERT(__is_read_reg(data));

        _MEM_PACKET_NUMBER_CHECK(packet_number);

        {
            __gpr unsigned int address;

            address = (packet_number & MAX_PACKET_NUMBER);

            if (sync == sig_done)
            {
                __asm
                {
                    mem[packet_wait_packet_status, *data, address, 0, FIXED_REF_COUNT], sig_done[*sig_ptr]
                }
            }
            else
            {
                __asm
                {
                    mem[packet_wait_packet_status, *data, address, 0, FIXED_REF_COUNT], ctx_swap[*sig_ptr]
                }
            }
        }
    }
    _INTRINSIC_END;
}



void mem_packet_complete_drop(
    mem_packet_complete_request_t packet_complete,
    unsigned int packet_number
)
{
    _INTRINSIC_BEGIN;
    {
#ifdef __NFP_INDIRECT_REF_FORMAT_NFP6000
        generic_ind_t ind;
        __gpr unsigned int address = ((packet_number & MAX_PACKET_NUMBER) << 16) | (packet_complete.total_packet_length & 0x3fff);

        _MEM_PACKET_COMPLETE_OVERRIDE(ind, packet_complete);

        __asm
        {
            local_csr_wr[CMD_INDIRECT_REF_0, CSR_INDIRECT_TYPE(ind)]
            alu[--, --, B, ALU_INDIRECT_TYPE(ind)]
            mem[packet_complete_drop, --, address, 0], indirect_ref

        }
#endif
    }
    _INTRINSIC_END;
}


void mem_packet_complete_unicast(
    mem_packet_complete_request_t packet_complete,
    unsigned int packet_number
)
{
    _INTRINSIC_BEGIN;
    {
#ifdef __NFP_INDIRECT_REF_FORMAT_NFP6000
        generic_ind_t ind;
        __gpr unsigned int address = ((packet_number & MAX_PACKET_NUMBER) << 16) | (packet_complete.total_packet_length & 0x3fff);

        _MEM_PACKET_COMPLETE_OVERRIDE(ind, packet_complete);

        __asm
        {
            local_csr_wr[CMD_INDIRECT_REF_0, CSR_INDIRECT_TYPE(ind)]
            alu[--, --, B, ALU_INDIRECT_TYPE(ind)]
            mem[packet_complete_unicast, --, address, 0], indirect_ref

        }
#endif
    }
    _INTRINSIC_END;
}

void mem_packet_complete_multicast(
    mem_packet_complete_request_t packet_complete,
    unsigned int packet_number
)
{
    _INTRINSIC_BEGIN;
    {
#ifdef __NFP_INDIRECT_REF_FORMAT_NFP6000
        generic_ind_t ind;
        __gpr unsigned int address = ((packet_number & MAX_PACKET_NUMBER) << 16) | (packet_complete.total_packet_length & 0x3fff);;

        _MEM_PACKET_COMPLETE_OVERRIDE(ind, packet_complete);

        __asm
        {
            local_csr_wr[CMD_INDIRECT_REF_0, CSR_INDIRECT_TYPE(ind)]
            alu[--, --, B, ALU_INDIRECT_TYPE(ind)]
            mem[packet_complete_multicast, --, address, 0], indirect_ref

        }
#endif
    }
    _INTRINSIC_END;
}

void mem_packet_complete_multicast_free(
    mem_packet_complete_request_t packet_complete,
    unsigned int packet_number
)
{
    _INTRINSIC_BEGIN;
    {
#ifdef __NFP_INDIRECT_REF_FORMAT_NFP6000
        generic_ind_t ind;
        __gpr unsigned int address = ((packet_number & MAX_PACKET_NUMBER) << 16) | (packet_complete.total_packet_length & 0x3fff);;

        _MEM_PACKET_COMPLETE_OVERRIDE(ind, packet_complete);

        __asm
        {
            local_csr_wr[CMD_INDIRECT_REF_0, CSR_INDIRECT_TYPE(ind)]
            alu[--, --, B, ALU_INDIRECT_TYPE(ind)]
            mem[packet_complete_multicast_free, --, address, 0], indirect_ref

        }
#endif
    }
    _INTRINSIC_END;
}

void mem_packet_add_thread(
    __xread void *data,
    unsigned int packet_offset,
    unsigned int count,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
        CT_ASSERT(__is_ct_const(sync));
        CT_ASSERT(sync == sig_done || sync == ctx_swap);
        CT_ASSERT(__is_read_reg(data));

        _MEM_REF_COUNT_CHECK(count, 6, 32);
        _MEM_PACKET_OFFSET_CHECK(packet_offset, 0x1ff);

        // Verify 4 byte aligned
        if (__is_ct_const(packet_offset))
        {
            CT_ASSERT(packet_offset == ((packet_offset >> 2) << 2) );
        }
        else
        {
            RT_RANGE_ASSERT(packet_offset == ((packet_offset >> 2) << 2));
        }

        {
            __gpr unsigned int address;
            generic_ind_t ind;

            address = (packet_offset & 0x1ff);
            _INTRINSIC_OVERRIDE_LENGTH(ind, count - 1);

            if (sync == sig_done)
            {
                __asm
                {
                    alu[--, --, B, ALU_INDIRECT_TYPE(ind)]
                    mem[packet_add_thread, *data, address, 0, --], sig_done[*sig_ptr], indirect_ref
                }
            }
            else
            {
                __asm
                {
                    alu[--, --, B, ALU_INDIRECT_TYPE(ind)]
                    mem[packet_add_thread, *data, address, 0, --], ctx_swap[*sig_ptr], indirect_ref
                }
            }
        }
    }
    _INTRINSIC_END;
}


void mem_pe_dma_to_memory_packet(
    volatile void __addr40 __mem *address,
    unsigned int packet_offset,
    unsigned int packet_number,
    unsigned int ctm_island,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
#ifdef __NFP_INDIRECT_REF_FORMAT_NFP6000
        CT_ASSERT(__is_ct_const(sync));
        CT_ASSERT(sync == sig_done || sync == ctx_swap);

        _MEM_PACKET_OFFSET_CHECK(packet_offset, 0xff);
        _MEM_8_BYTE_ALIGNMENT_CHECK(packet_offset);                     // packet offset must be 8 byte aligned
        _MEM_PACKET_NUMBER_CHECK(packet_number);
        _MEM_CTM_ISLAND_CHECK(ctm_island);
        _MEM_8_BYTE_ALIGNMENT_CHECK((unsigned int)address);               // emem address must be 8 byte aligned

        {
            generic_ind_t ind;
            unsigned int mu_island, low_addr, hi_addr;

            _MEM_DMA_MEMORY_PACKET_CONVERT_HI_LO_ADDRESS(address, ctm_island);
            _MEM_DMA_MEMORY_PACKET_OVERRIDE(ind, mu_island, packet_offset, packet_number);

            if (sync == sig_done)
            {
                __asm
                {
                    local_csr_wr[CMD_INDIRECT_REF_0, CSR_INDIRECT_TYPE(ind)]
                    alu[--, --, B, ALU_INDIRECT_TYPE(ind)]
                    mem[pe_dma_to_memory_packet, --, hi_addr, << 8, low_addr, --], sig_done[*sig_ptr], indirect_ref
                }
            }
            else
            {
                __asm
                {
                    local_csr_wr[CMD_INDIRECT_REF_0, CSR_INDIRECT_TYPE(ind)]
                    alu[--, --, B, ALU_INDIRECT_TYPE(ind)]
                    mem[pe_dma_to_memory_packet, --, hi_addr, << 8, low_addr, --], ctx_swap[*sig_ptr], indirect_ref
                }
            }
        }
#endif
    }
    _INTRINSIC_END;
}


void mem_pe_dma_to_memory_packet_free(
    volatile void __addr40 __mem *address,
    unsigned int packet_offset,
    unsigned int packet_number,
    unsigned int ctm_island,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
#ifdef __NFP_INDIRECT_REF_FORMAT_NFP6000
        CT_ASSERT(__is_ct_const(sync));
        CT_ASSERT(sync == sig_done || sync == ctx_swap);

        _MEM_PACKET_OFFSET_CHECK(packet_offset, 0xff);
        _MEM_8_BYTE_ALIGNMENT_CHECK(packet_offset);                     // packet offset must be 8 byte aligned
        _MEM_PACKET_NUMBER_CHECK(packet_number);
        _MEM_CTM_ISLAND_CHECK(ctm_island);
        _MEM_8_BYTE_ALIGNMENT_CHECK((unsigned int)address);               // emem address must be 8 byte aligned
        {
            generic_ind_t ind;
            unsigned int mu_island, low_addr, hi_addr;

            _MEM_DMA_MEMORY_PACKET_CONVERT_HI_LO_ADDRESS(address, ctm_island);

            _MEM_DMA_MEMORY_PACKET_OVERRIDE(ind, mu_island, packet_offset, packet_number);

            if (sync == sig_done)
            {
                __asm
                {
                    local_csr_wr[CMD_INDIRECT_REF_0, CSR_INDIRECT_TYPE(ind)]
                    alu[--, --, B, ALU_INDIRECT_TYPE(ind)]
                    mem[pe_dma_to_memory_packet_free, --, hi_addr, << 8, low_addr, --], sig_done[*sig_ptr], indirect_ref
                }
            }
            else
            {
                __asm
                {
                    local_csr_wr[CMD_INDIRECT_REF_0, CSR_INDIRECT_TYPE(ind)]
                    alu[--, --, B, ALU_INDIRECT_TYPE(ind)]
                    mem[pe_dma_to_memory_packet_free, --, hi_addr, << 8, low_addr, --], ctx_swap[*sig_ptr], indirect_ref
                }
            }
        }
#endif
    }
    _INTRINSIC_END;
}


void mem_pe_dma_to_memory_packet_swap(
    volatile void __addr40 __mem *address,
    unsigned int packet_offset,
    unsigned int packet_number,
     unsigned int ctm_island,
   sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
#ifdef __NFP_INDIRECT_REF_FORMAT_NFP6000
        CT_ASSERT(__is_ct_const(sync));
        CT_ASSERT(sync == sig_done || sync == ctx_swap);

        _MEM_PACKET_OFFSET_CHECK(packet_offset, 0xff);
        _MEM_8_BYTE_ALIGNMENT_CHECK(packet_offset);                     // packet offset must be 8 byte aligned
        _MEM_PACKET_NUMBER_CHECK(packet_number);
        _MEM_CTM_ISLAND_CHECK(ctm_island);
        _MEM_8_BYTE_ALIGNMENT_CHECK((unsigned int)address);               // emem address must be 8 byte aligned

        {
            generic_ind_t ind;
            unsigned int mu_island, low_addr, hi_addr;

            _MEM_DMA_MEMORY_PACKET_CONVERT_HI_LO_ADDRESS(address, ctm_island);

            _MEM_DMA_MEMORY_PACKET_OVERRIDE(ind, mu_island, packet_offset, packet_number);

            if (sync == sig_done)
            {
                __asm
                {
                    local_csr_wr[CMD_INDIRECT_REF_0, CSR_INDIRECT_TYPE(ind)]
                    alu[--, --, B, ALU_INDIRECT_TYPE(ind)]
                    mem[pe_dma_to_memory_packet_swap, --, hi_addr, << 8, low_addr, --], sig_done[*sig_ptr], indirect_ref
                }
            }
            else
            {
                __asm
                {
                    local_csr_wr[CMD_INDIRECT_REF_0, CSR_INDIRECT_TYPE(ind)]
                    alu[--, --, B, ALU_INDIRECT_TYPE(ind)]
                    mem[pe_dma_to_memory_packet_swap, --, hi_addr, << 8, low_addr, --], ctx_swap[*sig_ptr], indirect_ref
                }
            }
        }
#endif
    }
    _INTRINSIC_END;
}


void mem_pe_dma_to_memory_packet_free_swap(
    volatile void __addr40 __mem *address,
    unsigned int packet_offset,
    unsigned int packet_number,
    unsigned int ctm_island,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
#ifdef __NFP_INDIRECT_REF_FORMAT_NFP6000
        CT_ASSERT(__is_ct_const(sync));
        CT_ASSERT(sync == sig_done || sync == ctx_swap);

        _MEM_PACKET_OFFSET_CHECK(packet_offset, 0xff);
        _MEM_8_BYTE_ALIGNMENT_CHECK(packet_offset);                     // packet offset must be 8 byte aligned
        _MEM_PACKET_NUMBER_CHECK(packet_number);
        _MEM_CTM_ISLAND_CHECK(ctm_island);
        _MEM_8_BYTE_ALIGNMENT_CHECK((unsigned int)address);               // emem address must be 8 byte aligned

        {
            generic_ind_t ind;
            unsigned int mu_island, low_addr, hi_addr;

            _MEM_DMA_MEMORY_PACKET_CONVERT_HI_LO_ADDRESS(address, ctm_island);

            _MEM_DMA_MEMORY_PACKET_OVERRIDE(ind, mu_island, packet_offset, packet_number);

            if (sync == sig_done)
            {
                __asm
                {
                    local_csr_wr[CMD_INDIRECT_REF_0, CSR_INDIRECT_TYPE(ind)]
                    alu[--, --, B, ALU_INDIRECT_TYPE(ind)]
                    mem[pe_dma_to_memory_packet_free_swap, --, hi_addr, << 8, low_addr, --], sig_done[*sig_ptr], indirect_ref
                }
            }
            else
            {
                __asm
                {
                    local_csr_wr[CMD_INDIRECT_REF_0, CSR_INDIRECT_TYPE(ind)]
                    alu[--, --, B, ALU_INDIRECT_TYPE(ind)]
                    mem[pe_dma_to_memory_packet_free_swap, --, hi_addr, << 8, low_addr, --], ctx_swap[*sig_ptr], indirect_ref
                }
            }
        }
#endif
    }
    _INTRINSIC_END;
}


void mem_pe_dma_to_memory_buffer(
    volatile void __ctm *source_address,
    volatile void __addr40 __mem *destination_address,
    unsigned int ctm_island,
    unsigned int count,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
#ifdef __NFP_INDIRECT_REF_FORMAT_NFP6000
        CT_ASSERT(__is_ct_const(sync));
        CT_ASSERT(sync == sig_done || sync == ctx_swap);

        _MEM_REF_COUNT_CHECK(count, 1, 32)
        _MEM_CTM_ISLAND_CHECK(ctm_island);
        _MEM_8_BYTE_ALIGNMENT_CHECK((unsigned int)source_address);     // CTM address must be 8 byte aligned
        {
            generic_ind_t ind;
            unsigned int hi_addr, low_addr;
            unsigned int count = 1;

            hi_addr = (unsigned int)((unsigned long long)destination_address>>32);
            low_addr = (unsigned int)destination_address;

            _MEM_DMA_MEMORY_BUFFER_OVERRIDE(ind, count, hi_addr, source_address);

            if (ctm_island == 0)
            {
                hi_addr = 0;
            }
            else
            {
                hi_addr = (ctm_island << 24);
            }

            if (sync == sig_done)
            {
                __asm
                {
                    local_csr_wr[CMD_INDIRECT_REF_0, CSR_INDIRECT_TYPE(ind)]
                    alu[--, --, B, ALU_INDIRECT_TYPE(ind)]
                    mem[pe_dma_to_memory_buffer, --, hi_addr, << 8, low_addr, --], sig_done[*sig_ptr], indirect_ref
                }
            }
            else
            {
                __asm
                {
                    local_csr_wr[CMD_INDIRECT_REF_0, CSR_INDIRECT_TYPE(ind)]
                    alu[--, --, B, ALU_INDIRECT_TYPE(ind)]
                    mem[pe_dma_to_memory_buffer, --, hi_addr, <<8, low_addr, --], ctx_swap[*sig_ptr], indirect_ref
                }
            }
        }
#endif
    }
    _INTRINSIC_END;
}


void mem_pe_dma_to_memory_buffer_swap(
    volatile void __ctm *source_address,
    volatile void __addr40 __mem *destination_address,
    unsigned int ctm_island,
    unsigned int count,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
#ifdef __NFP_INDIRECT_REF_FORMAT_NFP6000
        CT_ASSERT(__is_ct_const(sync));
        CT_ASSERT(sync == sig_done || sync == ctx_swap);

        _MEM_REF_COUNT_CHECK(count, 1, 32)
        _MEM_CTM_ISLAND_CHECK(ctm_island);
        {
            generic_ind_t ind;
            unsigned int hi_addr, low_addr;
            unsigned int count = 1;

            hi_addr = (unsigned int)((unsigned long long)destination_address>>32);
            low_addr = (unsigned int)destination_address;

            _MEM_DMA_MEMORY_BUFFER_OVERRIDE(ind, count, hi_addr, source_address);

            if (ctm_island == 0)
            {
                hi_addr = 0;
            }
            else
            {
                hi_addr = (ctm_island << 24);
            }

            if (sync == sig_done)
            {
                __asm
                {
                    local_csr_wr[CMD_INDIRECT_REF_0, CSR_INDIRECT_TYPE(ind)]
                    alu[--, --, B, ALU_INDIRECT_TYPE(ind)]
                    mem[pe_dma_to_memory_buffer_swap, --, hi_addr, << 8, low_addr, --], sig_done[*sig_ptr], indirect_ref
                }
            }
            else
            {
                __asm
                {
                    local_csr_wr[CMD_INDIRECT_REF_0, CSR_INDIRECT_TYPE(ind)]
                    alu[--, --, B, ALU_INDIRECT_TYPE(ind)]
                    mem[pe_dma_to_memory_buffer_swap, --, hi_addr, <<8, low_addr, --], ctx_swap[*sig_ptr], indirect_ref
                }
            }
        }
#endif
    }
    _INTRINSIC_END;
}


void mem_pe_dma_to_memory_buffer_le(
    volatile void __ctm *source_address,
    volatile void __addr40 __mem *destination_address,
    unsigned int ctm_island,
    unsigned int count,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
#ifdef __NFP_INDIRECT_REF_FORMAT_NFP6000
        CT_ASSERT(__is_ct_const(sync));
        CT_ASSERT(sync == sig_done || sync == ctx_swap);

        _MEM_REF_COUNT_CHECK(count, 1, 32)
        _MEM_CTM_ISLAND_CHECK(ctm_island);
        {
            generic_ind_t ind;
            unsigned int hi_addr, low_addr;
            unsigned int count = 1;

            hi_addr = (unsigned int)((unsigned long long)destination_address>>32);
            low_addr = (unsigned int)destination_address;

            _MEM_DMA_MEMORY_BUFFER_OVERRIDE(ind, count, hi_addr, source_address);

            if (ctm_island == 0)
            {
                hi_addr = 0;
            }
            else
            {
                hi_addr = (ctm_island << 24);
            }

            if (sync == sig_done)
            {
                __asm
                {
                    local_csr_wr[CMD_INDIRECT_REF_0, CSR_INDIRECT_TYPE(ind)]
                    alu[--, --, B, ALU_INDIRECT_TYPE(ind)]
                    mem[pe_dma_to_memory_buffer_le, --, hi_addr, << 8, low_addr, --], sig_done[*sig_ptr], indirect_ref
                }
            }
            else
            {
                __asm
                {
                    local_csr_wr[CMD_INDIRECT_REF_0, CSR_INDIRECT_TYPE(ind)]
                    alu[--, --, B, ALU_INDIRECT_TYPE(ind)]
                    mem[pe_dma_to_memory_buffer_le, --, hi_addr, <<8, low_addr, --], ctx_swap[*sig_ptr], indirect_ref
                }
            }
        }
#endif
    }
    _INTRINSIC_END;
}


void mem_pe_dma_to_memory_buffer_le_swap(
    volatile void __ctm *source_address,
    volatile void __addr40 __mem *destination_address,
    unsigned int ctm_island,
    unsigned int count,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
#ifdef __NFP_INDIRECT_REF_FORMAT_NFP6000
        CT_ASSERT(__is_ct_const(sync));
        CT_ASSERT(sync == sig_done || sync == ctx_swap);

        _MEM_REF_COUNT_CHECK(count, 1, 32)
        _MEM_CTM_ISLAND_CHECK(ctm_island);
        {
            generic_ind_t ind;
            unsigned int hi_addr, low_addr;
            unsigned int count = 1;

            hi_addr = (unsigned int)((unsigned long long)destination_address>>32);
            low_addr = (unsigned int)destination_address;

            _MEM_DMA_MEMORY_BUFFER_OVERRIDE(ind, count, hi_addr, source_address);

            if (ctm_island == 0)
            {
                hi_addr = 0;
            }
            else
            {
                hi_addr = (ctm_island << 24);
            }

            if (sync == sig_done)
            {
                __asm
                {
                    local_csr_wr[CMD_INDIRECT_REF_0, CSR_INDIRECT_TYPE(ind)]
                    alu[--, --, B, ALU_INDIRECT_TYPE(ind)]
                    mem[pe_dma_to_memory_buffer_le_swap, --, hi_addr, << 8, low_addr, --], sig_done[*sig_ptr], indirect_ref
                }
            }
            else
            {
                __asm
                {
                    local_csr_wr[CMD_INDIRECT_REF_0, CSR_INDIRECT_TYPE(ind)]
                    alu[--, --, B, ALU_INDIRECT_TYPE(ind)]
                    mem[pe_dma_to_memory_buffer_le_swap, --, hi_addr, <<8, low_addr, --], ctx_swap[*sig_ptr], indirect_ref
                }
            }
        }
#endif
    }
    _INTRINSIC_END;
}

void mem_pe_dma_from_memory_buffer(
    volatile void __addr40 __mem *source_address,
    volatile void __ctm *destination_address,
    unsigned int ctm_island,
    unsigned int count,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
#ifdef __NFP_INDIRECT_REF_FORMAT_NFP6000
        CT_ASSERT(__is_ct_const(sync));
        CT_ASSERT(sync == sig_done || sync == ctx_swap);

        _MEM_REF_COUNT_CHECK(count, 1, 32)
        _MEM_CTM_ISLAND_CHECK(ctm_island);
        {
            generic_ind_t ind;
            unsigned int hi_addr, low_addr;

            hi_addr = (unsigned int)((unsigned long long)source_address>>32);
            low_addr = (unsigned int)source_address;   // 32 lower bits of external mem

            _MEM_DMA_MEMORY_BUFFER_OVERRIDE(ind, count, hi_addr, destination_address);

            if (ctm_island == 0)
            {
                hi_addr = 0;
            }
            else
            {
                hi_addr = (ctm_island << 24);
            }

            if (sync == sig_done)
            {
                __asm
                {
                    local_csr_wr[CMD_INDIRECT_REF_0, CSR_INDIRECT_TYPE(ind)]
                    alu[--, --, B, ALU_INDIRECT_TYPE(ind)]
                    mem[pe_dma_from_memory_buffer, --, hi_addr, << 8, low_addr, --], sig_done[*sig_ptr], indirect_ref
                }
            }
            else
            {
                __asm
                {
                    local_csr_wr[CMD_INDIRECT_REF_0, CSR_INDIRECT_TYPE(ind)]
                    alu[--, --, B, ALU_INDIRECT_TYPE(ind)]
                    mem[pe_dma_from_memory_buffer, --, hi_addr, <<8, low_addr, --], ctx_swap[*sig_ptr], indirect_ref
                }
            }
        }
#endif
    }
    _INTRINSIC_END;
}



void mem_pe_dma_from_memory_buffer_swap(
    volatile void __addr40 __mem *source_address,
    volatile void __ctm *destination_address,
    unsigned int ctm_island,
    unsigned int count,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
#ifdef __NFP_INDIRECT_REF_FORMAT_NFP6000
        CT_ASSERT(__is_ct_const(sync));
        CT_ASSERT(sync == sig_done || sync == ctx_swap);

        _MEM_REF_COUNT_CHECK(count, 1, 32)
        _MEM_CTM_ISLAND_CHECK(ctm_island);
        {
            generic_ind_t ind;
            unsigned int hi_addr, low_addr;

            hi_addr = (unsigned int)((unsigned long long)source_address>>32);
            low_addr = (unsigned int)source_address;   // 32 lower bits of external mem

            _MEM_DMA_MEMORY_BUFFER_OVERRIDE(ind, count, hi_addr, destination_address);

            if (ctm_island == 0)
            {
                hi_addr = 0;
            }
            else
            {
                hi_addr = (ctm_island << 24);
            }

            if (sync == sig_done)
            {
                __asm
                {
                    local_csr_wr[CMD_INDIRECT_REF_0, CSR_INDIRECT_TYPE(ind)]
                    alu[--, --, B, ALU_INDIRECT_TYPE(ind)]
                    mem[pe_dma_from_memory_buffer_swap, --, hi_addr, << 8, low_addr, --], sig_done[*sig_ptr], indirect_ref
                }
            }
            else
            {
                __asm
                {
                    local_csr_wr[CMD_INDIRECT_REF_0, CSR_INDIRECT_TYPE(ind)]
                    alu[--, --, B, ALU_INDIRECT_TYPE(ind)]
                    mem[pe_dma_from_memory_buffer_swap, --, hi_addr, <<8, low_addr, --], ctx_swap[*sig_ptr], indirect_ref
                }
            }
        }
#endif
    }
    _INTRINSIC_END;
}



void mem_pe_dma_from_memory_buffer_le(
    volatile void __addr40 __mem *source_address,
    volatile void __ctm *destination_address,
    unsigned int ctm_island,
    unsigned int count,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
#ifdef __NFP_INDIRECT_REF_FORMAT_NFP6000
        CT_ASSERT(__is_ct_const(sync));
        CT_ASSERT(sync == sig_done || sync == ctx_swap);

        _MEM_REF_COUNT_CHECK(count, 1, 32)
        _MEM_CTM_ISLAND_CHECK(ctm_island);
        {
            generic_ind_t ind;
            unsigned int hi_addr, low_addr;

            hi_addr = (unsigned int)((unsigned long long)source_address>>32);
            low_addr = (unsigned int)source_address;   // 32 lower bits of external mem

            _MEM_DMA_MEMORY_BUFFER_OVERRIDE(ind, count, hi_addr, destination_address);

            if (ctm_island == 0)
            {
                hi_addr = 0;
            }
            else
            {
                hi_addr = (ctm_island << 24);
            }

            if (sync == sig_done)
            {
                __asm
                {
                    local_csr_wr[CMD_INDIRECT_REF_0, CSR_INDIRECT_TYPE(ind)]
                    alu[--, --, B, ALU_INDIRECT_TYPE(ind)]
                    mem[pe_dma_from_memory_buffer_le, --, hi_addr, << 8, low_addr, --], sig_done[*sig_ptr], indirect_ref
                }
            }
            else
            {
                __asm
                {
                    local_csr_wr[CMD_INDIRECT_REF_0, CSR_INDIRECT_TYPE(ind)]
                    alu[--, --, B, ALU_INDIRECT_TYPE(ind)]
                    mem[pe_dma_from_memory_buffer_le, --, hi_addr, <<8, low_addr, --], ctx_swap[*sig_ptr], indirect_ref
                }
            }
        }
#endif
    }
    _INTRINSIC_END;
}


void mem_pe_dma_from_memory_buffer_le_swap(
    volatile void __addr40 __mem *source_address,
    volatile void __ctm *destination_address,
    unsigned int ctm_island,
    unsigned int count,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
#ifdef __NFP_INDIRECT_REF_FORMAT_NFP6000
        CT_ASSERT(__is_ct_const(sync));
        CT_ASSERT(sync == sig_done || sync == ctx_swap);

        _MEM_REF_COUNT_CHECK(count, 1, 32)
        _MEM_CTM_ISLAND_CHECK(ctm_island);
        {
            generic_ind_t ind;
            unsigned int hi_addr, low_addr;

            hi_addr = (unsigned int)((unsigned long long)source_address>>32);
            low_addr = (unsigned int)source_address;   // 32 lower bits of external mem

            _MEM_DMA_MEMORY_BUFFER_OVERRIDE(ind, count, hi_addr, destination_address);


            if (ctm_island == 0)
            {
                hi_addr = 0;
            }
            else
            {
                hi_addr = (ctm_island << 24);
            }

            if (sync == sig_done)
            {
                __asm
                {
                    local_csr_wr[CMD_INDIRECT_REF_0, CSR_INDIRECT_TYPE(ind)]
                    alu[--, --, B, ALU_INDIRECT_TYPE(ind)]
                    mem[pe_dma_from_memory_buffer_le_swap, --, hi_addr, << 8, low_addr, --], sig_done[*sig_ptr], indirect_ref
                }
            }
            else
            {
                __asm
                {
                    local_csr_wr[CMD_INDIRECT_REF_0, CSR_INDIRECT_TYPE(ind)]
                    alu[--, --, B, ALU_INDIRECT_TYPE(ind)]
                    mem[pe_dma_from_memory_buffer_le_swap, --, hi_addr, <<8, low_addr, --], ctx_swap[*sig_ptr], indirect_ref
                }
            }
        }
#endif
    }
    _INTRINSIC_END;
}


void mem_pe_dma_to_memory_indirect(
    __xread unsigned int *data,
    unsigned int packet_number,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
        CT_ASSERT(__is_ct_const(sync));
        CT_ASSERT(sync == sig_done || sync == ctx_swap);

        _MEM_PACKET_NUMBER_CHECK(packet_number);

        {
            unsigned int address = packet_number & MAX_PACKET_NUMBER;

            if (sync == sig_done)
            {
                __asm
                {
                    mem[pe_dma_to_memory_indirect, *data, 0, address, FIXED_REF_COUNT], sig_done[*sig_ptr]
                }
            }
            else
            {
                __asm
                {
                    mem[pe_dma_to_memory_indirect, *data, 0, address, FIXED_REF_COUNT], ctx_swap[*sig_ptr]
                }
            }
        }
    }
    _INTRINSIC_END;
}


void mem_pe_dma_to_memory_indirect_free(
    __xread unsigned int *data,
    unsigned int packet_number,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
        CT_ASSERT(__is_ct_const(sync));
        CT_ASSERT(sync == sig_done || sync == ctx_swap);

        _MEM_PACKET_NUMBER_CHECK(packet_number);

        {
            unsigned int address = packet_number & MAX_PACKET_NUMBER;

            if (sync == sig_done)
            {
                __asm
                {
                    mem[pe_dma_to_memory_indirect_free, *data, 0, address, FIXED_REF_COUNT], sig_done[*sig_ptr]
                }
            }
            else
            {
                __asm
                {
                    mem[pe_dma_to_memory_indirect_free, *data, 0, address, FIXED_REF_COUNT], ctx_swap[*sig_ptr]
                }
            }
        }
    }
    _INTRINSIC_END;
}


void mem_pe_dma_to_memory_indirect_swap(
    __xread unsigned int *data,
    unsigned int packet_number,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
        CT_ASSERT(__is_ct_const(sync));
        CT_ASSERT(sync == sig_done || sync == ctx_swap);

        _MEM_PACKET_NUMBER_CHECK(packet_number);

        {
            unsigned int address = packet_number & MAX_PACKET_NUMBER;

            if (sync == sig_done)
            {
                __asm
                {
                    mem[pe_dma_to_memory_indirect_swap, *data, 0, address, FIXED_REF_COUNT], sig_done[*sig_ptr]
                }
            }
            else
            {
                __asm
                {
                    mem[pe_dma_to_memory_indirect_swap, *data, 0, address, FIXED_REF_COUNT], ctx_swap[*sig_ptr]
                }
            }
        }
    }
    _INTRINSIC_END;
}


void mem_pe_dma_to_memory_indirect_free_swap(
    __xread unsigned int *data,
    unsigned int packet_number,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
        CT_ASSERT(__is_ct_const(sync));
        CT_ASSERT(sync == sig_done || sync == ctx_swap);

        _MEM_PACKET_NUMBER_CHECK(packet_number);

        {
            unsigned int address = packet_number & MAX_PACKET_NUMBER;

            if (sync == sig_done)
            {
                __asm
                {
                    mem[pe_dma_to_memory_indirect_free_swap, *data, 0, address, FIXED_REF_COUNT], sig_done[*sig_ptr]
                }
            }
            else
            {
                __asm
                {
                    mem[pe_dma_to_memory_indirect_free_swap, *data, 0, address, FIXED_REF_COUNT], ctx_swap[*sig_ptr]
                }
            }
        }
    }
    _INTRINSIC_END;
}

#endif /* __NFP_MEM_PACKET_ENGINE_C__ */
