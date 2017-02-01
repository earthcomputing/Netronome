/*
 * Copyright (C) 2008-2014 Netronome Systems, Inc.  All rights reserved.
 */

#ifndef __NFP_MEM_LOCK_C__
#define __NFP_MEM_LOCK_C__

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
#include <nfp_mem_lock.h>

/*
 * NOTE: When writing inline-asm, it's important not to use reserved words
 *       for variables.  Common ones are 'a','b', 'csr', 'state', and
 *       'inp_state'.
 */

__intrinsic
void mem_lock128_init_ptr32(
    __mem mem_lock128_in_mem_t *mem_lock
)
{
    mem_write_atomic_imm_ptr32(0, (void __addr32 __mem *)mem_lock, sizeof(*mem_lock) / sizeof(unsigned int));
}

#ifdef __PTR40
__intrinsic
void mem_lock128_init_ptr40(
    mem_lock128_ptr40_t mem_lock
)
{
    mem_write_atomic_imm_ptr40(0, mem_lock, sizeof(*mem_lock) / sizeof(unsigned int));
}
#endif

__intrinsic
void mem_lock256_init_ptr32(
    __mem mem_lock256_in_mem_t *mem_lock
)
{
    mem_write_atomic_imm_ptr32(0, (void __addr32 __mem *)mem_lock, sizeof(*mem_lock) / sizeof(unsigned int));
}

#ifdef __PTR40
__intrinsic
void mem_lock256_init_ptr40(
    mem_lock256_ptr40_t mem_lock
)
{
    mem_write_atomic_imm_ptr40(0, mem_lock, sizeof(*mem_lock) / sizeof(unsigned int));
}
#endif

__intrinsic
void mem_lock384_init_ptr32(
    __mem mem_lock384_in_mem_t *mem_lock
)
{
    __addr32 __mem unsigned int *fp = (__addr32 __mem unsigned int *)mem_lock;
    mem_write_atomic_imm_ptr32(0, (void __addr32 __mem *)mem_lock, 8);
    mem_write_atomic_imm_ptr32(0, fp + 8, 4);
}

#ifdef __PTR40
__intrinsic
void mem_lock384_init_ptr40(
    mem_lock384_ptr40_t mem_lock
)
{
    __addr40 __mem unsigned int *fp = (__addr40 __mem unsigned int *)mem_lock;
    mem_write_atomic_imm_ptr40(0, mem_lock, 8);
    mem_write_atomic_imm_ptr40(0, fp + 8, 4);
}
#endif

__intrinsic
void mem_lock512_init_ptr32(
    __mem mem_lock512_in_mem_t *mem_lock
)
{
    __addr32 __mem unsigned int *fp = (__addr32 __mem unsigned int *)mem_lock;
    mem_write_atomic_imm_ptr32(0, (void __addr32 __mem *)mem_lock, sizeof(*mem_lock) / sizeof(unsigned int) / 2);
    mem_write_atomic_imm_ptr32(0, fp + sizeof(*mem_lock) / sizeof(unsigned int) / 2, sizeof(*mem_lock) / sizeof(unsigned int) / 2);
}

#ifdef __PTR40
__intrinsic
void mem_lock512_init_ptr40(
    mem_lock512_ptr40_t mem_lock
)
{
    __addr40 __mem unsigned int *fp = (__addr40 __mem unsigned int *)mem_lock;
    mem_write_atomic_imm_ptr40(0, mem_lock, sizeof(*mem_lock) / sizeof(unsigned int) / 2);
    mem_write_atomic_imm_ptr40(0, fp + sizeof(*mem_lock) / sizeof(unsigned int) / 2, sizeof(*mem_lock) / sizeof(unsigned int) / 2);
}
#endif

enum
{
    MEM_LOCK_REF_COUNT_128 = 0x3,
    MEM_LOCK_REF_COUNT_256 = 0x7,
    MEM_LOCK_REF_COUNT_384 = 0xB,
    MEM_LOCK_REF_COUNT_512 = 0xF
};

#define _MEM_LOCK_PTR32(mmm)                                                                \
__intrinsic                                                                                 \
__xread mem_lock_out_t *mem_lock##mmm##_ptr32(                                 \
    __mem mem_lock##mmm##_in_mem_t *mem_lock,                                 \
    __xwrite mem_lock_in_t *xfer,                                              \
    unsigned int lsbzeroes,                                                                 \
    sync_t sync,                                                                            \
    SIGNAL_PAIR *sig_pair_ptr                                                               \
    )                                                                                       \
{                                                                                           \
    _INTRINSIC_BEGIN;                                                                       \
    {                                                                                       \
        generic_ind_t ind;                                                                  \
        CT_ASSERT(__is_write_reg(xfer));                                                    \
        _INTRINSIC_OVERRIDE_LENGTH_AND_BYTEMASK_IN_ALU(ind, MEM_LOCK_REF_COUNT_##mmm##, lsbzeroes);\
        __asm alu[--, --, B, ALU_INDIRECT_TYPE(ind)]                                        \
        __asm mem[lock##mmm##, *xfer, mem_lock, 0 ], sig_done[*sig_pair_ptr], indirect_ref  \
    }                                                                                       \
    return (__xread mem_lock_out_t *)(xfer);                                   \
    _INTRINSIC_END;                                                                         \
}

#define _MEM_LOCK_PTR40(mmm)                                                                \
__intrinsic                                                                                 \
__xread mem_lock_out_t *mem_lock##mmm##_ptr40(                                 \
    mem_lock##mmm##_ptr40_t mem_lock,                                                       \
    __xwrite mem_lock_in_t *xfer,                                              \
    unsigned int lsbzeroes,                                                                 \
    sync_t sync,                                                                            \
    SIGNAL_PAIR *sig_pair_ptr                                                               \
    )                                                                                       \
{                                                                                           \
    _INTRINSIC_BEGIN;                                                                       \
    {                                                                                       \
        generic_ind_t ind;                                                                  \
        unsigned int hi_addr, low_addr;                                                     \
        _INTRINSIC_CONVERT_HI_LO_ADDRESS(mem_lock);                                         \
        CT_ASSERT(__is_write_reg(xfer));                                                    \
        _INTRINSIC_OVERRIDE_LENGTH_AND_BYTEMASK_IN_ALU(ind, MEM_LOCK_REF_COUNT_##mmm##, lsbzeroes);\
        __asm alu[--, --, B, ALU_INDIRECT_TYPE(ind)]                                        \
        __asm mem[lock##mmm##, *xfer, hi_addr, <<8, low_addr, 0 ], sig_done[*sig_pair_ptr], indirect_ref \
    }                                                                                       \
    return (__xread mem_lock_out_t *)(xfer);                                   \
    _INTRINSIC_END;                                                                         \
}

_MEM_LOCK_PTR32(128)
_MEM_LOCK_PTR32(256)
_MEM_LOCK_PTR32(384)
_MEM_LOCK_PTR32(512)

#ifdef __PTR40
_MEM_LOCK_PTR40(128)
_MEM_LOCK_PTR40(256)
_MEM_LOCK_PTR40(384)
_MEM_LOCK_PTR40(512)
#endif

#define _MEM_UNLOCK_PTR32(mmm)                                                  \
__intrinsic                                                                     \
void mem_unlock##mmm##_ptr32(                                                   \
    __mem mem_lock##mmm##_in_mem_t *mem_lock,                     \
    unsigned int index,                                                         \
    sync_t sync,                                                                \
    SIGNAL *sig_ptr                                                             \
    )                                                                           \
{                                                                               \
    _INTRINSIC_BEGIN;                                                           \
    {                                                                           \
        __xwrite unsigned int xfer = 0;                          \
        mem_write_atomic_ptr32(&xfer,(__addr32 __mem void *)(((__addr32 __mem unsigned int *)mem_lock) + index), 1, sync, sig_ptr); \
    }                                                                           \
    _INTRINSIC_END;                                                             \
}

#define _MEM_UNLOCK_PTR40(mmm)                                                  \
__intrinsic                                                                     \
void mem_unlock##mmm##_ptr40(                                                   \
    mem_lock##mmm##_ptr40_t mem_lock,                                           \
    unsigned int index,                                                         \
    sync_t sync,                                                                \
    SIGNAL *sig_ptr                                                             \
    )                                                                           \
{                                                                               \
    _INTRINSIC_BEGIN;                                                           \
    {                                                                           \
        __xwrite unsigned int xfer = 0;                       \
        mem_write_atomic_ptr40(&xfer,(__addr40 __mem void *)(((__addr40 __mem unsigned int *)mem_lock) + index), 1, sync, sig_ptr); \
    }                                                                           \
    _INTRINSIC_END;                                                             \
}

_MEM_UNLOCK_PTR32(128)
_MEM_UNLOCK_PTR32(256)
_MEM_UNLOCK_PTR32(384)
_MEM_UNLOCK_PTR32(512)

#ifdef __PTR40
_MEM_UNLOCK_PTR40(128)
_MEM_UNLOCK_PTR40(256)
_MEM_UNLOCK_PTR40(384)
_MEM_UNLOCK_PTR40(512)
#endif

#endif /* __NFP_MEM_LOCK_C__ */
