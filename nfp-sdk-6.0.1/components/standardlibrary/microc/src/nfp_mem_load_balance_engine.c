/*
 * Copyright (C) 2008-2014 Netronome Systems, Inc.  All rights reserved.
 */

#ifndef __NFP_MEM_LOAD_BALANCE_ENGINE_C__
#define __NFP_MEM_LOAD_BALANCE_ENGINE_C__

#ifndef NFP_LIB_ANY_NFCC_VERSION
    #if (!defined(__NFP_TOOL_NFCC) ||                       \
        (__NFP_TOOL_NFCC < NFP_SW_VERSION(5, 0, 0, 0)) ||   \
        (__NFP_TOOL_NFCC > NFP_SW_VERSION(6, 0, 1, 255)))
        #error "This standard library is not supported for the version of the SDK currently in use."
    #endif
#endif

#include <nfp.h>
#include <nfp_intrinsic.h>
#include <nfp_mem_load_balance_engine.h>

#pragma diag_suppress 279  /* ignore while(0) etc in macros */

/*
* NOTE: When writing inline-asm, it's important not to use reserved words
*       for variables.  Common ones are 'a','b', 'csr', 'state', and
*       'inp_state'.
*/

#define _MEM_LB_SIGNAL_CHECK(sig)                                    \
        if (__is_ct_const(sig))                                      \
        {                                                            \
            CT_ASSERT(sig == sig_done || sig == ctx_swap);           \
        }

#define _MEM_LB_SIGNAL_PAIR_CHECK(sig_pair)                           \
        if (__is_ct_const(sig_pair))                                  \
        {                                                             \
            CT_ASSERT(sig_pair == sig_done);                          \
        }


/*
* Implement a load balancing command.
* Override the length if more than 8.
* sig_done and ctx_swap are supported.
* Set is_read flag if read operation, then read_reg is checked otherwise write_reg.
* Verify the max ref count 1 to .. max_ref_count.
*/

#define _MEM_LB_COMMAND(command, max_ref_count, is_read)                                                                    \
{                                                                                                                           \
    unsigned int hi_addr, low_addr;                                                                                         \
    if (is_read == 1) {                                                                                                     \
        CT_ASSERT(__is_read_reg(xfer));                                                                                     \
    }                                                                                                                       \
    else                                                                                                                    \
    {                                                                                                                       \
        CT_ASSERT(__is_write_reg(xfer));                                                                                    \
    }                                                                                                                       \
                                                                                                                            \
    _MEM_LB_SIGNAL_CHECK(sync);                                                                                             \
    _INTRINSIC_CONVERT_HI_LO_ADDRESS(address);                                                                              \
    if (sync == sig_done)                                                                                                   \
    {                                                                                                                       \
        if (__is_ct_const(count) && count <= 8)                                                                             \
        {                                                                                                                   \
            __asm mem[command, *xfer, hi_addr, << 8, low_addr, __ct_const_val(count)], sig_done[*sig_ptr]                   \
        }                                                                                                                   \
        else                                                                                                                \
        {                                                                                                                   \
            _INTRINSIC_IND_ALU_COUNT_MIN_MAX(count, 1, max_ref_count);                                                      \
            __asm mem[command, *xfer, hi_addr, << 8, low_addr, __ct_const_val(count)], sig_done[*sig_ptr], indirect_ref     \
        }                                                                                                                   \
    }                                                                                                                       \
    else                                                                                                                    \
    {                                                                                                                       \
        if (__is_ct_const(count) && count <= 8)                                                                             \
        {                                                                                                                   \
            __asm mem[command, *xfer, hi_addr, << 8, low_addr, __ct_const_val(count)], ctx_swap[*sig_ptr]                   \
        }                                                                                                                   \
        else                                                                                                                \
        {                                                                                                                   \
            _INTRINSIC_IND_ALU_COUNT_MIN_MAX(count, 1, max_ref_count);                                                      \
            __asm mem[command, *xfer, hi_addr, << 8, low_addr, __ct_const_val(count)], ctx_swap[*sig_ptr], indirect_ref     \
        }                                                                                                                   \
    }                                                                                                                       \
}



__intrinsic
void mem_lb_write_desc(
    mem_lb_desc_in_write_reg_t *xfer,
    volatile void __addr40 __mem *address,
    unsigned int count,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
        _MEM_LB_COMMAND(lb_write_desc, 16, 0);
    }
    _INTRINSIC_END;
}



__intrinsic
void mem_lb_read_desc(
    mem_lb_desc_in_read_reg_t *xfer,
    volatile void __addr40 __mem *address,
    unsigned int count,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
        _MEM_LB_COMMAND(lb_read_desc, 32, 1);
    }
    _INTRINSIC_END;
}

__intrinsic
void mem_lb_write_id_table(
    __xwrite void *xfer,
    volatile void __addr40 __mem *address,
    unsigned int count,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
        _MEM_LB_COMMAND(lb_write_idtable, 16, 0);
    }
    _INTRINSIC_END;
}


__intrinsic
void mem_lb_read_id_table(
    __xread void *xfer,
    volatile void __addr40 __mem *address,
    unsigned int count,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
        _MEM_LB_COMMAND(lb_read_idtable, 16, 1);
    }
    _INTRINSIC_END;
}

__intrinsic
void mem_lb_bucket_write_local(
    __xwrite void *xfer,
    volatile void __addr40 __mem *address,
    unsigned int count,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
        _MEM_LB_COMMAND(lb_bucket_write_local, 8, 0);
    }
    _INTRINSIC_END;
}


__intrinsic
void mem_lb_bucket_read_local(
    __xread void *xfer,
    volatile void __addr40 __mem *address,
    unsigned int count,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
        _MEM_LB_COMMAND(lb_bucket_read_local, 8, 1);
    }
    _INTRINSIC_END;
}


__intrinsic
void mem_lb_bucket_write_dcache(
    __xwrite void *xfer,
    volatile void __addr40 __mem *address,
    unsigned int count,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
        _MEM_LB_COMMAND(lb_bucket_write_dcache, 8, 0);
    }
    _INTRINSIC_END;
}


__intrinsic
void mem_lb_bucket_read_dcache(
    __xread void *xfer,
    volatile void __addr40 __mem *address,
    unsigned int count,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
        _MEM_LB_COMMAND(lb_bucket_read_dcache, 8, 1);
    }
    _INTRINSIC_END;
}

__intrinsic
void mem_lb_stats_read_and_clear_local(
    __xread void *xfer,
    volatile void __addr40 __mem *address,
    unsigned int count,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
        _MEM_LB_COMMAND(lb_push_stats_local_clr, 16, 1);
    }
    _INTRINSIC_END;
}


__intrinsic
void mem_lb_stats_read_local(
    __xread void *xfer,
    volatile void __addr40 __mem *address,
    unsigned int count,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
        _MEM_LB_COMMAND(lb_push_stats_local, 16, 1);
    }
    _INTRINSIC_END;
}


__intrinsic
void mem_lb_stats_read_and_clear_dcache(
    __xread void *xfer,
    volatile void __addr40 __mem *address,
    unsigned int count,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
        _MEM_LB_COMMAND(lb_push_stats_dcache_clr, 16, 1);
    }
    _INTRINSIC_END;
}


__intrinsic
void mem_lb_stats_read_dcache(
    __xread void *xfer,
    volatile void __addr40 __mem *address,
    unsigned int count,
    sync_t sync,
    SIGNAL *sig_ptr
)
{
    _INTRINSIC_BEGIN;
    {
        _MEM_LB_COMMAND(lb_push_stats_dcache, 16, 1);
    }
    _INTRINSIC_END;
}

__intrinsic
mem_lb_lookup_bundle_id_result_in_read_reg_t *mem_lb_lookup_bundle_id(
                mem_lb_lookup_command_format_in_write_reg_t *xfer,
                volatile void __addr40 __mem *address,
                sync_t sync,
                SIGNAL_PAIR *sig_pair_ptr
)
{
    _INTRINSIC_BEGIN;
    {
        unsigned int hi_addr, low_addr;
        CT_ASSERT(__is_write_reg(xfer));
        _MEM_LB_SIGNAL_PAIR_CHECK(sync);

        _INTRINSIC_CONVERT_HI_LO_ADDRESS(address);

        __asm mem[lb_lookup_bundleid, *xfer, hi_addr, <<8, low_addr], sig_done[*sig_pair_ptr]

        return (mem_lb_lookup_bundle_id_result_in_read_reg_t *)(xfer);
    }
    _INTRINSIC_END;
}


__intrinsic
mem_lb_lookup_direct_result_in_read_reg_t *mem_lb_lookup_dcache(
                mem_lb_lookup_command_format_in_write_reg_t *xfer,
                volatile void __addr40 __mem *address,
                sync_t sync,
                SIGNAL_PAIR *sig_pair_ptr
)
{
    _INTRINSIC_BEGIN;
    {
        unsigned int hi_addr, low_addr;
        CT_ASSERT(__is_write_reg(xfer));
        _MEM_LB_SIGNAL_PAIR_CHECK(sync);

        _INTRINSIC_CONVERT_HI_LO_ADDRESS(address);

        __asm mem[lb_lookup_dcache, *xfer, hi_addr, <<8, low_addr], sig_done[*sig_pair_ptr]

        return (mem_lb_lookup_direct_result_in_read_reg_t *)(xfer);
    }
    _INTRINSIC_END;
}


__intrinsic
mem_lb_lookup_direct_result_in_read_reg_t *mem_lb_lookup_id_table(
                mem_lb_lookup_command_format_in_write_reg_t *xfer,
                volatile void __addr40 __mem *address,
                sync_t sync,
                SIGNAL_PAIR *sig_pair_ptr
)
{
    _INTRINSIC_BEGIN;
    {
        unsigned int hi_addr, low_addr;
        CT_ASSERT(__is_write_reg(xfer));
        _MEM_LB_SIGNAL_PAIR_CHECK(sync);

        _INTRINSIC_CONVERT_HI_LO_ADDRESS(address);

        __asm mem[lb_lookup_idtable, *xfer, hi_addr, <<8, low_addr], sig_done[*sig_pair_ptr]

        return (mem_lb_lookup_direct_result_in_read_reg_t *)(xfer);
    }
    _INTRINSIC_END;
}

__intrinsic
void mem_lb_init(
            unsigned int descriptor_array_number,
            unsigned int mu_island,
            unsigned int stat_base_addr,
            enum MEM_LB_LOCATION stat_addr_ref,
            unsigned int hash_base_addr,
            enum MEM_LB_LOCATION hash_addr_ref,
            unsigned int boundaries,
            unsigned int index_bits,
            unsigned int compare_bits,
            unsigned int hash_seed,
            unsigned long long user_data,
            unsigned int count
)
{
    _INTRINSIC_BEGIN;
    {
        SIGNAL                                              sig;
        mem_lb_desc_t                                       lb_write_desc;
        volatile __xwrite unsigned int         lb_write_xfer_desc[6];
        mem_lb_descriptor_address_format_t                  descriptor_address;

        /*
         * descriptor_array_number between 0 and 63
         */
        if (__is_ct_const(descriptor_array_number))
        {
            CT_ASSERT(descriptor_array_number <= 63);
        }
        else
        {
            RT_RANGE_ASSERT(descriptor_array_number <= 63);
        }
        /*
         * mu_island only 8 bits wide
         */
        if (__is_ct_const(mu_island))
        {
            CT_ASSERT((mu_island >> 8) == 0);
        }
        else
        {
            RT_RANGE_ASSERT((mu_island >> 8) == 0);
        }
        /*
         * stat_base_addr only 18 bits wide
         */
        if (__is_ct_const(stat_base_addr))
        {
            CT_ASSERT((stat_base_addr >> 18) == 0);
        }
        else
        {
            RT_RANGE_ASSERT((stat_base_addr >> 18) == 0);
        }
        /*
         * stat_base_addr only 18 bits wide
         */
        if (__is_ct_const(hash_base_addr))
        {
            CT_ASSERT((hash_base_addr >> 18) == 0);
        }
        else
        {
            RT_RANGE_ASSERT((hash_base_addr >> 18) == 0);
        }
        /*
         * hash_seed only 24 bits wide
         */
        if (__is_ct_const(hash_seed))
        {
            CT_ASSERT((hash_seed >> 24) == 0);
        }
        else
        {
            RT_RANGE_ASSERT((hash_seed >> 24) == 0);
        }

        /*
         * Specific configuration for boundaries, index bits and compare bits.
         * Table 9.50, Supported Descriptor Parameters in DB
         */
        if (__is_ct_const(boundaries) && __is_ct_const(index_bits) && __is_ct_const(compare_bits))
        {
            CT_ASSERT(boundaries == 3 || boundaries == 4 || boundaries == 5 || boundaries == 6 || boundaries == 7);
            if (boundaries == 3) CT_ASSERT(index_bits == 0 && compare_bits == 24);
            if (boundaries == 4) CT_ASSERT(index_bits == 2 && compare_bits == 22);
            if (boundaries == 5) CT_ASSERT(index_bits == 8 && compare_bits == 16);
            if (boundaries == 6) CT_ASSERT(index_bits == 12 && compare_bits == 12);
            if (boundaries == 7) CT_ASSERT(index_bits == 16 && compare_bits == 8);
        }
        else
        {
            RT_RANGE_ASSERT(boundaries == 3 || boundaries == 4 || boundaries == 5 || boundaries == 6 || boundaries == 7);
            if (boundaries == 3) RT_RANGE_ASSERT(index_bits == 0 && compare_bits == 24);
            if (boundaries == 4) RT_RANGE_ASSERT(index_bits == 2 && compare_bits == 22);
            if (boundaries == 5) RT_RANGE_ASSERT(index_bits == 8 && compare_bits == 16);
            if (boundaries == 6) RT_RANGE_ASSERT(index_bits == 12 && compare_bits == 12);
            if (boundaries == 7) RT_RANGE_ASSERT(index_bits == 16 && compare_bits == 8);
        }


        descriptor_address.value = 0;
        descriptor_address.array_entry = descriptor_array_number;
        {
            __addr40 __mem void *addr_ptr = (__addr40 __mem void *) ((unsigned long long)mu_island << 32 | descriptor_address.value);
            unsigned int hash_addr;
            unsigned int seed = MEM_LB_HASH_SEED_LOW(hash_seed);

            hash_addr = MEM_LB_HASH_BASE_ADDR_LOW(hash_base_addr);
            lb_write_xfer_desc[0] = (hash_addr << 19) | (stat_addr_ref << 18) | stat_base_addr;

            hash_addr = MEM_LB_HASH_BASE_ADDR_HIGH(hash_base_addr);
            lb_write_xfer_desc[1] = (seed << 19) | (boundaries << 16) | (index_bits << 11) | (compare_bits << 6) | (hash_addr_ref << 5) | hash_addr;

            lb_write_xfer_desc[2] = MEM_LB_HASH_SEED_HIGH(hash_seed);
            lb_write_xfer_desc[3] = 0;
            lb_write_xfer_desc[4] = (unsigned int)(user_data >> 32);
            lb_write_xfer_desc[5] = (unsigned int)(user_data & 0xffffffff);

            mem_lb_write_desc((void *)lb_write_xfer_desc, addr_ptr, count,  ctx_swap, &sig);
        }
    }
    _INTRINSIC_END;
}

#pragma diag_default 279  /* suppressed above */

#endif /* __NFP_MEM_LOAD_BALANCE_ENGINE_C__ */
