/*
 * Copyright (C) 2008-2014 Netronome Systems, Inc.  All rights reserved.
 */

#ifndef __NFP_MEM_CAM_C__
#define __NFP_MEM_CAM_C__

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
#include <nfp_mem_cam.h>

/*
 * NOTE: When writing inline-asm, it's important not to use reserved words
 * for variables.  Common ones are 'a','b', 'csr', 'state', and
 * 'inp_state'.
 */


#define _MEM_CAM_PARAM_ALIGN_16_CHECK(addr)                 \
        if (__is_ct_const(addr))                            \
        {                                                   \
            CT_ASSERT((addr & 0xf) == 0);                   \
        }                                                   \
        else                                                \
        {                                                   \
            RT_ALIGN_ASSERT((addr & 0xf) == 0);             \
        }

#define _MEM_CAM_PARAM_SIGNAL_CHECK(sig)                    \
        if (__is_ct_const(sig))                             \
        {                                                   \
            CT_ASSERT(sig == sig_done || sig == ctx_swap);  \
        }

#define _MEM_CAM_PARAM_SIGNAL_PAIR_CHECK(sync)              \
        if (__is_ct_const(sync))                            \
        {                                                   \
            CT_ASSERT(sync == sig_done);                    \
        }

#define _MEM_CAM_INDEX_CHECK(index, max_value)              \
        if (__is_ct_const(index))                           \
        {                                                   \
            CT_ASSERT((index) < (max_value));               \
        }                                                   \
        else                                                \
        {                                                   \
            RT_RANGE_ASSERT((index) < (max_value));         \
        }

#define _MEM_CAM_ENTRY_RANGE_CHECK(value, max_value)        \
        if (__is_ct_const(value))                           \
        {                                                   \
            CT_ASSERT((value) <= (max_value));              \
        }                                                   \
        else                                                \
        {                                                   \
            RT_RANGE_ASSERT((value) <= (max_value));        \
        }

__intrinsic
void mem_cam128_init_ptr32(
    __mem mem_cam128_in_mem_t *cam
)
{
    mem_write_atomic_imm_ptr32(0, (volatile __addr32 __mem void *)cam, sizeof(*cam) / sizeof(unsigned int));
}

#ifdef __PTR40
__intrinsic
void mem_cam128_init_ptr40(
    mem_cam128_ptr40_t cam
)
{
    mem_write_atomic_imm_ptr40(0, cam, sizeof(*cam) / sizeof(unsigned int));
}
#endif

__intrinsic
void mem_cam256_init_ptr32(
    __mem mem_cam256_in_mem_t *cam
)
{
    mem_write_atomic_imm_ptr32(0, (void __addr32 __mem *)cam, sizeof(*cam) / sizeof(unsigned int));
}

#ifdef __PTR40
__intrinsic
void mem_cam256_init_ptr40(
    mem_cam256_ptr40_t cam
)
{
    mem_write_atomic_imm_ptr40(0, cam, sizeof(*cam) / sizeof(unsigned int));
}
#endif

__intrinsic
void mem_cam384_init_ptr32(
    __mem mem_cam384_in_mem_t *cam
)
{
    __addr32 __mem unsigned int *fp = (__addr32 __mem unsigned int *)cam;
    mem_write_atomic_imm_ptr32(0, (void __addr32 __mem *)cam, 8);
    mem_write_atomic_imm_ptr32(0, fp + 8, 4);
}

#ifdef __PTR40
__intrinsic
void mem_cam384_init_ptr40(
    mem_cam384_ptr40_t cam
)
{
    __addr40 __mem unsigned int *fp = (__addr40 __mem unsigned int *)cam;
    mem_write_atomic_imm_ptr40(0, cam, 8);
    mem_write_atomic_imm_ptr40(0, fp + 8, 4);
}
#endif

__intrinsic
void mem_cam512_init_ptr32(
    __mem mem_cam512_in_mem_t *cam
)
{
    __addr32 __mem unsigned int *fp = (__addr32 __mem unsigned int *)cam;
    mem_write_atomic_imm_ptr32(0, (void __addr32 __mem *)cam, sizeof(*cam) / sizeof(unsigned int) / 2);
    mem_write_atomic_imm_ptr32(0, fp + sizeof(*cam) / sizeof(unsigned int) / 2, sizeof(*cam) / sizeof(unsigned int) / 2);
}

#ifdef __PTR40
__intrinsic
void mem_cam512_init_ptr40(
    mem_cam512_ptr40_t cam
)
{
    __addr40 __mem unsigned int *fp = (__addr40 __mem unsigned int *)cam;
    mem_write_atomic_imm_ptr40(0, cam, sizeof(*cam) / sizeof(unsigned int) / 2);
    mem_write_atomic_imm_ptr40(0, fp + sizeof(*cam) / sizeof(unsigned int) / 2, sizeof(*cam) / sizeof(unsigned int) / 2);
}
#endif

#define _MEM_CAMNNN_SET8_PTR32(nnn)                                 \
__intrinsic                                                         \
void mem_cam##nnn##_set8_ptr32(                                     \
    __mem mem_cam##nnn##_in_mem_t *cam,                             \
    unsigned int index,                                             \
    unsigned int value,                                             \
    sync_t sync,                                                    \
    SIGNAL *sig_ptr                                                 \
    )                                                               \
{                                                                   \
    _MEM_CAM_INDEX_CHECK(index,sizeof(*cam));                       \
    _MEM_CAM_ENTRY_RANGE_CHECK(value,0xff);                         \
    CT_ASSERT(__is_ct_const(sync));                                 \
    {                                                               \
        __xwrite unsigned int v;                                    \
        generic_ind_t ind;                                          \
        __addr32 __mem unsigned char *ptr = (__addr32 __mem  unsigned char *)cam + index;  \
        v = (value << 24) >> ((index & 3) << 3);                    \
        _INTRINSIC_OVERRIDE_BYTEMASK(ind, 8 >> (index & 3));        \
        mem_write_atomic_ind_ptr32(&v,(__addr32 __mem void *)ptr,1,ind,sync,sig_ptr);    \
    }                                                               \
}

#define _MEM_CAMNNN_SET8_PTR40(nnn)                                 \
__intrinsic                                                         \
void mem_cam##nnn##_set8_ptr40(                                     \
    mem_cam##nnn##_ptr40_t cam,                                     \
    unsigned int index,                                             \
    unsigned int value,                                             \
    sync_t sync,                                                    \
    SIGNAL *sig_ptr                                                 \
    )                                                               \
{                                                                   \
    _MEM_CAM_INDEX_CHECK(index,sizeof(*cam));                       \
    _MEM_CAM_ENTRY_RANGE_CHECK(value,0xff);                         \
    CT_ASSERT(__is_ct_const(sync));                                 \
    {                                                               \
        __xwrite unsigned int v;                                    \
        generic_ind_t ind;                                          \
        __addr40 __mem unsigned char *ptr = (__addr40 __mem unsigned char *)cam + index; \
        v = (value << 24) >> ((index & 3) << 3);                    \
        _INTRINSIC_OVERRIDE_BYTEMASK(ind, 8 >> (index & 3));        \
        mem_write_atomic_ind_ptr40(&v,(__addr40 __mem void *)ptr,1,ind,sync,sig_ptr); \
    }                                                               \
}

_MEM_CAMNNN_SET8_PTR32(128)
_MEM_CAMNNN_SET8_PTR32(256)
_MEM_CAMNNN_SET8_PTR32(384)
_MEM_CAMNNN_SET8_PTR32(512)

#ifdef __PTR40
_MEM_CAMNNN_SET8_PTR40(128)
_MEM_CAMNNN_SET8_PTR40(256)
_MEM_CAMNNN_SET8_PTR40(384)
_MEM_CAMNNN_SET8_PTR40(512)
#endif

#define _MEM_CAMNNN_SET16_PTR32(nnn)                                \
__intrinsic                                                         \
void mem_cam##nnn##_set16_ptr32(                                    \
    __mem mem_cam##nnn##_in_mem_t *cam,                             \
    unsigned int index,                                             \
    unsigned int value,                                             \
    sync_t sync,                                                    \
    SIGNAL *sig_ptr                                                 \
    )                                                               \
{                                                                   \
    _MEM_CAM_INDEX_CHECK(index,sizeof(*cam)/2);                     \
    _MEM_CAM_ENTRY_RANGE_CHECK(value,0xffff);                       \
    CT_ASSERT(__is_ct_const(sync));                                 \
    {                                                               \
        __xwrite unsigned int v;                                    \
        generic_ind_t ind;                                          \
        __addr32 __mem unsigned short *ptr = (__addr32 __mem unsigned short *)cam + index;                \
        v = (value << 16) >> ((index & 1) << 4);                    \
        _INTRINSIC_OVERRIDE_BYTEMASK(ind, 0x0c >> ((index & 1) << 1));     \
        mem_write_atomic_ind_ptr32(&v,(__addr32 __mem void*)ptr,1,ind,sync,sig_ptr);     \
    }                                                               \
}

#define _MEM_CAMNNN_SET16_PTR40(nnn)                                \
__intrinsic                                                         \
void mem_cam##nnn##_set16_ptr40(                                    \
    mem_cam##nnn##_ptr40_t cam,                                     \
    unsigned int index,                                             \
    unsigned int value,                                             \
    sync_t sync,                                                    \
    SIGNAL *sig_ptr                                                 \
    )                                                               \
{                                                                   \
    _MEM_CAM_INDEX_CHECK(index,sizeof(*cam)/2);                     \
    _MEM_CAM_ENTRY_RANGE_CHECK(value,0xffff);                       \
    CT_ASSERT(__is_ct_const(sync));                                 \
    {                                                               \
        __xwrite unsigned int v;                                    \
        generic_ind_t ind;                                          \
        __addr40 __mem unsigned short *ptr = (__addr40 __mem unsigned short *)cam + index; \
        v = (value << 16) >> ((index & 1) << 4);                    \
        _INTRINSIC_OVERRIDE_BYTEMASK(ind, 0x0c >> ((index & 1) << 1));     \
        mem_write_atomic_ind_ptr40(&v,(__addr40 __mem void*)ptr,1,ind,sync,sig_ptr); \
    }                                                               \
}

_MEM_CAMNNN_SET16_PTR32(128)
_MEM_CAMNNN_SET16_PTR32(256)
_MEM_CAMNNN_SET16_PTR32(384)
_MEM_CAMNNN_SET16_PTR32(512)

#ifdef __PTR40
_MEM_CAMNNN_SET16_PTR40(128)
_MEM_CAMNNN_SET16_PTR40(256)
_MEM_CAMNNN_SET16_PTR40(384)
_MEM_CAMNNN_SET16_PTR40(512)
#endif

#define _MEM_CAMNNN_SET24_PTR32(nnn)                                \
__intrinsic                                                         \
void mem_cam##nnn##_set24_ptr32(                                    \
    __mem mem_cam##nnn##_in_mem_t *cam,                             \
    unsigned int index,                                             \
    unsigned int value,                                             \
    sync_t sync,                                                    \
    SIGNAL *sig_ptr                                                 \
    )                                                               \
{                                                                   \
    _MEM_CAM_INDEX_CHECK(index,(sizeof(*cam)/sizeof(unsigned int)));\
    CT_ASSERT(__is_ct_const(sync));                                 \
    {                                                               \
        __xwrite unsigned int v = value;                            \
        __addr32 __mem unsigned int *ptr = (__addr32 __mem unsigned int *)cam + index;             \
        mem_write_atomic_ptr32(&v,(__addr32 __mem void*)ptr,1,sync,sig_ptr);             \
    }                                                               \
}

#define _MEM_CAMNNN_SET24_PTR40(nnn)                                \
__intrinsic                                                         \
void mem_cam##nnn##_set24_ptr40(                                    \
    mem_cam##nnn##_ptr40_t cam,                                     \
    unsigned int index,                                             \
    unsigned int value,                                             \
    sync_t sync,                                                    \
    SIGNAL *sig_ptr                                                 \
    )                                                               \
{                                                                   \
    _MEM_CAM_INDEX_CHECK(index,(sizeof(*cam)/sizeof(unsigned int)));\
    CT_ASSERT(__is_ct_const(sync));                                 \
    {                                                               \
        __xwrite unsigned int v = value;                            \
        __addr40 __mem unsigned int *ptr = (__addr40 __mem unsigned int *)cam + index; \
        mem_write_atomic_ptr40(&v,(__addr40 __mem void*)ptr,1,sync,sig_ptr); \
    }                                                               \
}

_MEM_CAMNNN_SET24_PTR32(128)
_MEM_CAMNNN_SET24_PTR32(256)
_MEM_CAMNNN_SET24_PTR32(384)
_MEM_CAMNNN_SET24_PTR32(512)

#ifdef __PTR40
_MEM_CAMNNN_SET24_PTR40(128)
_MEM_CAMNNN_SET24_PTR40(256)
_MEM_CAMNNN_SET24_PTR40(384)
_MEM_CAMNNN_SET24_PTR40(512)
#endif

#define _MEM_CAMNNN_SET32_PTR32(nnn)                                \
__intrinsic                                                         \
void mem_cam##nnn##_set32_ptr32(                                    \
    __mem mem_cam##nnn##_in_mem_t *cam,                             \
    unsigned int index,                                             \
    unsigned int value,                                             \
    sync_t sync,                                                    \
    SIGNAL *sig_ptr                                                 \
    )                                                               \
{                                                                   \
    _MEM_CAM_INDEX_CHECK(index,(sizeof(*cam)/sizeof(unsigned int)));\
    CT_ASSERT(__is_ct_const(sync));                                 \
    {                                                               \
        __xwrite unsigned int v = value;                            \
        __addr32 __mem unsigned int *ptr = (__addr32 __mem unsigned int*)cam + index;             \
        mem_write_atomic_ptr32(&v,(__addr32 __mem void*)ptr,1,sync,sig_ptr);             \
    }                                                               \
}

#define _MEM_CAMNNN_SET32_PTR40(nnn)                                \
__intrinsic                                                         \
void mem_cam##nnn##_set32_ptr40(                                    \
    mem_cam##nnn##_ptr40_t cam,                                     \
    unsigned int index,                                             \
    unsigned int value,                                             \
    sync_t sync,                                                    \
    SIGNAL *sig_ptr                                                 \
    )                                                               \
{                                                                   \
    _MEM_CAM_INDEX_CHECK(index,(sizeof(*cam)/sizeof(unsigned int)));\
    CT_ASSERT(__is_ct_const(sync));                                 \
    {                                                               \
        __xwrite unsigned int v = value;                            \
        __addr40 __mem unsigned int *ptr = (__addr40 __mem unsigned int*)cam + index; \
        mem_write_atomic_ptr40(&v,(__addr40 __mem void*)ptr,1,sync,sig_ptr); \
    }                                                               \
}

_MEM_CAMNNN_SET32_PTR32(128)
_MEM_CAMNNN_SET32_PTR32(256)
_MEM_CAMNNN_SET32_PTR32(384)
_MEM_CAMNNN_SET32_PTR32(512)

#ifdef __PTR40
_MEM_CAMNNN_SET32_PTR40(128)
_MEM_CAMNNN_SET32_PTR40(256)
_MEM_CAMNNN_SET32_PTR40(384)
_MEM_CAMNNN_SET32_PTR40(512)
#endif

#define _MEM_CAM128_256_LOOKUP_PTR32(mmm,nn)                                        \
__intrinsic                                                                         \
__xread mem_cam_lookup##nn##_out_t *mem_cam##mmm##_lookup##nn##_ptr32(              \
    __mem mem_cam##mmm##_in_mem_t *cam,                                             \
    __xwrite mem_cam_lookup##nn##_in_t *xfer,                                       \
    sync_t sync,                                                                    \
    SIGNAL_PAIR *sig_pair_ptr                                                       \
    )                                                                               \
{                                                                                   \
    _INTRINSIC_BEGIN;                                                               \
    {                                                                               \
        CT_ASSERT(__is_write_reg(xfer));                                            \
        CT_ASSERT(__is_ct_const(sync));                                             \
        CT_ASSERT(sync == sig_done);                                                \
        __asm                                                                       \
        {                                                                           \
            mem[cam##mmm##_lookup##nn##, *xfer, cam, 0 ], sig_done[*sig_pair_ptr]   \
        }                                                                           \
    }                                                                               \
    return (__xread mem_cam_lookup##nn##_out_t *)(xfer);                            \
    _INTRINSIC_END;                                                                 \
}

#define _MEM_CAM128_256_LOOKUP_PTR40(mmm,nn)                                        \
__intrinsic                                                                         \
__xread mem_cam_lookup##nn##_out_t *mem_cam##mmm##_lookup##nn##_ptr40(              \
    mem_cam##mmm##_ptr40_t cam,                                                     \
    __xwrite mem_cam_lookup##nn##_in_t *xfer,                                       \
    sync_t sync,                                                                    \
    SIGNAL_PAIR *sig_pair_ptr                                                       \
    )                                                                               \
{                                                                                   \
    _INTRINSIC_BEGIN;                                                               \
    {                                                                               \
        __gpr unsigned int hi_addr, low_addr;                                       \
        CT_ASSERT(__is_write_reg(xfer));                                            \
        CT_ASSERT(__is_ct_const(sync));                                             \
        CT_ASSERT(sync == sig_done);                                                \
        _INTRINSIC_CONVERT_HI_LO_ADDRESS(cam);                                      \
        __asm                                                                       \
        {                                                                           \
            mem[cam##mmm##_lookup##nn##, *xfer, hi_addr, <<8, low_addr], sig_done[*sig_pair_ptr] \
        }                                                                           \
    }                                                                               \
    return (__xread mem_cam_lookup##nn##_out_t *)(xfer);                            \
    _INTRINSIC_END;                                                                 \
}


enum
{
    CAM_REF_COUNT_128_8 = 0,
    CAM_REF_COUNT_128_16,
    CAM_REF_COUNT_128_24,
    CAM_REF_COUNT_128_32,

    CAM_REF_COUNT_256_8 = 4,
    CAM_REF_COUNT_256_16,
    CAM_REF_COUNT_256_24,
    CAM_REF_COUNT_256_32,

    CAM_REF_COUNT_384_8 = 8,
    CAM_REF_COUNT_384_16,
    CAM_REF_COUNT_384_24,
    CAM_REF_COUNT_384_32,

    CAM_REF_COUNT_512_8 = 12,
    CAM_REF_COUNT_512_16,
    CAM_REF_COUNT_512_24,
    CAM_REF_COUNT_512_32
};

#define _MEM_CAM384_512_LOOKUP_PTR32(mmm,nn)                                            \
__intrinsic                                                                             \
__xread mem_cam_lookup##nn##_out_t *mem_cam##mmm##_lookup##nn##_ptr32(                  \
    __mem mem_cam##mmm##_in_mem_t *cam,                                                 \
    __xwrite mem_cam_lookup##nn##_in_t *xfer,                                           \
    sync_t sync,                                                                        \
    SIGNAL_PAIR *sig_pair_ptr                                                           \
    )                                                                                   \
{                                                                                       \
    _INTRINSIC_BEGIN;                                                                   \
    {                                                                                   \
        generic_ind_t ind;                                                              \
        CT_ASSERT(__is_write_reg(xfer));                                                \
        CT_ASSERT(__is_ct_const(sync));                                                 \
        CT_ASSERT(sync == sig_done);                                                    \
        _INTRINSIC_OVERRIDE_LENGTH(ind, CAM_REF_COUNT_##mmm##_##nn);                    \
        __asm alu[--, --, B, ALU_INDIRECT_TYPE(ind)]                                    \
        __asm mem[cam##mmm##_lookup##nn##, *xfer, cam, 0 ], sig_done[*sig_pair_ptr], indirect_ref    \
    }                                                                                   \
    return (__xread mem_cam_lookup##nn##_out_t *)(xfer);                                \
    _INTRINSIC_END;                                                                     \
}


#define _MEM_CAM384_512_LOOKUP_PTR40(mmm,nn)                                            \
__intrinsic                                                                             \
__xread mem_cam_lookup##nn##_out_t *mem_cam##mmm##_lookup##nn##_ptr40(                  \
    mem_cam##mmm##_ptr40_t cam,                                                         \
    __xwrite mem_cam_lookup##nn##_in_t *xfer,                                           \
    sync_t sync,                                                                        \
    SIGNAL_PAIR *sig_pair_ptr                                                           \
    )                                                                                   \
{                                                                                       \
    _INTRINSIC_BEGIN;                                                                   \
    {                                                                                   \
        generic_ind_t ind;                                                              \
        unsigned int hi_addr, low_addr;                                                 \
        CT_ASSERT(__is_write_reg(xfer));                                                \
        CT_ASSERT(__is_ct_const(sync));                                                 \
        CT_ASSERT(sync == sig_done);                                                    \
        _INTRINSIC_CONVERT_HI_LO_ADDRESS(cam);                                          \
        _INTRINSIC_OVERRIDE_LENGTH(ind, CAM_REF_COUNT_##mmm##_##nn);                    \
        __asm alu[--, --, B, ALU_INDIRECT_TYPE(ind)]                                    \
        __asm mem[cam##mmm##_lookup##nn##, *xfer, hi_addr, <<8, low_addr], sig_done[*sig_pair_ptr], indirect_ref \
    }                                                                                   \
    return (__xread mem_cam_lookup##nn##_out_t *)(xfer);                                \
    _INTRINSIC_END;                                                                     \
}

_MEM_CAM128_256_LOOKUP_PTR32(128, 8)
_MEM_CAM128_256_LOOKUP_PTR32(256, 8)
_MEM_CAM384_512_LOOKUP_PTR32(384, 8)
_MEM_CAM384_512_LOOKUP_PTR32(512, 8)

_MEM_CAM128_256_LOOKUP_PTR32(128, 16)
_MEM_CAM128_256_LOOKUP_PTR32(256, 16)
_MEM_CAM384_512_LOOKUP_PTR32(384, 16)
_MEM_CAM384_512_LOOKUP_PTR32(512, 16)

_MEM_CAM128_256_LOOKUP_PTR32(128, 24)
_MEM_CAM128_256_LOOKUP_PTR32(256, 24)
_MEM_CAM384_512_LOOKUP_PTR32(384, 24)
_MEM_CAM384_512_LOOKUP_PTR32(512, 24)

_MEM_CAM128_256_LOOKUP_PTR32(128, 32)
_MEM_CAM128_256_LOOKUP_PTR32(256, 32)
_MEM_CAM384_512_LOOKUP_PTR32(384, 32)
_MEM_CAM384_512_LOOKUP_PTR32(512, 32)

#ifdef __PTR40
_MEM_CAM128_256_LOOKUP_PTR40(128, 8)
_MEM_CAM128_256_LOOKUP_PTR40(256, 8)
_MEM_CAM384_512_LOOKUP_PTR40(384, 8)
_MEM_CAM384_512_LOOKUP_PTR40(512, 8)

_MEM_CAM128_256_LOOKUP_PTR40(128, 16)
_MEM_CAM128_256_LOOKUP_PTR40(256, 16)
_MEM_CAM384_512_LOOKUP_PTR40(384, 16)
_MEM_CAM384_512_LOOKUP_PTR40(512, 16)

_MEM_CAM128_256_LOOKUP_PTR40(128, 24)
_MEM_CAM128_256_LOOKUP_PTR40(256, 24)
_MEM_CAM384_512_LOOKUP_PTR40(384, 24)
_MEM_CAM384_512_LOOKUP_PTR40(512, 24)

_MEM_CAM128_256_LOOKUP_PTR40(128, 32)
_MEM_CAM128_256_LOOKUP_PTR40(256, 32)
_MEM_CAM384_512_LOOKUP_PTR40(384, 32)
_MEM_CAM384_512_LOOKUP_PTR40(512, 32)
#endif

#define _MEM_CAM128_256_LOOKUP_ADD_PTR32(mmm,nn)                                \
__intrinsic                                                                     \
__xread mem_cam_lookup##nn##_add_out_t *mem_cam##mmm##_lookup##nn##_add_ptr32(  \
    __mem mem_cam##mmm##_in_mem_t *cam,                                         \
    __xwrite mem_cam_lookup##nn##_in_t *xfer,                                   \
    sync_t sync,                                                                \
    SIGNAL_PAIR *sig_pair_ptr                                                   \
    )                                                                           \
{                                                                               \
    _INTRINSIC_BEGIN;                                                           \
    {                                                                           \
        CT_ASSERT(__is_write_reg(xfer));                                        \
        CT_ASSERT(__is_ct_const(sync));                                         \
        CT_ASSERT(sync == sig_done);                                            \
        __asm                                                                   \
        {                                                                       \
            mem[cam##mmm##_lookup##nn##_add, *xfer, cam, 0 ], sig_done[*sig_pair_ptr]  \
        }                                                                       \
    }                                                                           \
    return (__xread mem_cam_lookup##nn##_add_out_t *)(xfer);                    \
    _INTRINSIC_END;                                                             \
}

#define _MEM_CAM128_256_LOOKUP_ADD_PTR40(mmm,nn)                                \
__intrinsic                                                                     \
__xread mem_cam_lookup##nn##_add_out_t *mem_cam##mmm##_lookup##nn##_add_ptr40(  \
    mem_cam##mmm##_ptr40_t cam,                                                 \
    __xwrite mem_cam_lookup##nn##_in_t *xfer,                                   \
    sync_t sync,                                                                \
    SIGNAL_PAIR *sig_pair_ptr                                                   \
    )                                                                           \
{                                                                               \
    _INTRINSIC_BEGIN;                                                           \
    {                                                                           \
        unsigned int hi_addr, low_addr;                                         \
        CT_ASSERT(__is_write_reg(xfer));                                        \
        CT_ASSERT(__is_ct_const(sync));                                         \
        CT_ASSERT(sync == sig_done);                                            \
        _INTRINSIC_CONVERT_HI_LO_ADDRESS(cam);                                  \
        __asm                                                                   \
        {                                                                       \
            mem[cam##mmm##_lookup##nn##_add, *xfer, hi_addr, <<8, low_addr], sig_done[*sig_pair_ptr]  \
        }                                                                       \
    }                                                                           \
    return (__xread mem_cam_lookup##nn##_add_out_t *)(xfer);                    \
    _INTRINSIC_END;                                                             \
}

#define _MEM_CAM384_512_LOOKUP_ADD_PTR32(mmm,nn)                                \
__intrinsic                                                                     \
__xread mem_cam_lookup##nn##_add_out_t *mem_cam##mmm##_lookup##nn##_add_ptr32(  \
    __mem mem_cam##mmm##_in_mem_t *cam,                                         \
    __xwrite mem_cam_lookup##nn##_in_t *xfer,                                   \
    sync_t sync,                                                                \
    SIGNAL_PAIR *sig_pair_ptr                                                   \
    )                                                                           \
{                                                                               \
    _INTRINSIC_BEGIN;                                                           \
    {                                                                           \
        generic_ind_t ind;                                                      \
        CT_ASSERT(__is_write_reg(xfer));                                        \
        CT_ASSERT(__is_ct_const(sync));                                         \
        CT_ASSERT(sync == sig_done);                                            \
        _INTRINSIC_OVERRIDE_LENGTH(ind, CAM_REF_COUNT_##mmm##_##nn);            \
        __asm alu[--, --, B, ALU_INDIRECT_TYPE(ind)]                            \
        __asm mem[cam##mmm##_lookup##nn##_add, *xfer, cam, 0 ], sig_done[*sig_pair_ptr], indirect_ref \
    }                                                                           \
    return (__xread mem_cam_lookup##nn##_add_out_t *)(xfer);                    \
    _INTRINSIC_END;                                                             \
}

#define _MEM_CAM384_512_LOOKUP_ADD_PTR40(mmm,nn)                                \
__intrinsic                                                                     \
__xread mem_cam_lookup##nn##_add_out_t *mem_cam##mmm##_lookup##nn##_add_ptr40(  \
    mem_cam##mmm##_ptr40_t cam,                                                 \
    __xwrite mem_cam_lookup##nn##_in_t *xfer,                                   \
    sync_t sync,                                                                \
    SIGNAL_PAIR *sig_pair_ptr                                                   \
    )                                                                           \
{                                                                               \
    _INTRINSIC_BEGIN;                                                           \
    {                                                                           \
        generic_ind_t ind;                                                      \
        unsigned int hi_addr, low_addr;                                         \
        CT_ASSERT(__is_write_reg(xfer));                                        \
        CT_ASSERT(__is_ct_const(sync));                                         \
        CT_ASSERT(sync == sig_done);                                            \
        _INTRINSIC_CONVERT_HI_LO_ADDRESS(cam);                                  \
        _INTRINSIC_OVERRIDE_LENGTH(ind, CAM_REF_COUNT_##mmm##_##nn);            \
        __asm alu[--, --, B, ALU_INDIRECT_TYPE(ind)]                            \
        __asm mem[cam##mmm##_lookup##nn##_add, *xfer, hi_addr, <<8, low_addr], sig_done[*sig_pair_ptr], indirect_ref \
    }                                                                           \
    return (__xread mem_cam_lookup##nn##_add_out_t *)(xfer);                    \
    _INTRINSIC_END;                                                             \
}

_MEM_CAM128_256_LOOKUP_ADD_PTR32(128, 8)
_MEM_CAM128_256_LOOKUP_ADD_PTR32(256, 8)
_MEM_CAM384_512_LOOKUP_ADD_PTR32(384, 8)
_MEM_CAM384_512_LOOKUP_ADD_PTR32(512, 8)

_MEM_CAM128_256_LOOKUP_ADD_PTR32(128, 16)
_MEM_CAM128_256_LOOKUP_ADD_PTR32(256, 16)
_MEM_CAM384_512_LOOKUP_ADD_PTR32(384, 16)
_MEM_CAM384_512_LOOKUP_ADD_PTR32(512, 16)

_MEM_CAM128_256_LOOKUP_ADD_PTR32(128, 32)
_MEM_CAM128_256_LOOKUP_ADD_PTR32(256, 32)
_MEM_CAM384_512_LOOKUP_ADD_PTR32(384, 32)
_MEM_CAM384_512_LOOKUP_ADD_PTR32(512, 32)

#ifdef __PTR40
_MEM_CAM128_256_LOOKUP_ADD_PTR40(128, 8)
_MEM_CAM128_256_LOOKUP_ADD_PTR40(256, 8)
_MEM_CAM384_512_LOOKUP_ADD_PTR40(384, 8)
_MEM_CAM384_512_LOOKUP_ADD_PTR40(512, 8)

_MEM_CAM128_256_LOOKUP_ADD_PTR40(128, 16)
_MEM_CAM128_256_LOOKUP_ADD_PTR40(256, 16)
_MEM_CAM384_512_LOOKUP_ADD_PTR40(384, 16)
_MEM_CAM384_512_LOOKUP_ADD_PTR40(512, 16)

_MEM_CAM128_256_LOOKUP_ADD_PTR40(128, 32)
_MEM_CAM128_256_LOOKUP_ADD_PTR40(256, 32)
_MEM_CAM384_512_LOOKUP_ADD_PTR40(384, 32)
_MEM_CAM384_512_LOOKUP_ADD_PTR40(512, 32)
#endif



/*
 * Lookup24 Add takes different parameters due to high byte usage
 */
#define _MEM_CAM128_256_LOOKUP24_ADD_PTR32(mmm)                                 \
__intrinsic                                                                     \
__xread mem_cam_lookup24_add_out_t *mem_cam##mmm##_lookup24_add_ptr32(          \
    __mem mem_cam##mmm##_in_mem_t *cam,                                         \
    __xwrite mem_cam_lookup24_add_in_t *xfer,                                   \
    sync_t sync,                                                                \
    SIGNAL_PAIR *sig_pair_ptr                                                   \
    )                                                                           \
{                                                                               \
    _INTRINSIC_BEGIN;                                                           \
    {                                                                           \
        CT_ASSERT(__is_write_reg(xfer));                                        \
        CT_ASSERT(__is_ct_const(sync));                                         \
        CT_ASSERT(sync == sig_done);                                            \
        __asm                                                                   \
        {                                                                       \
            mem[cam##mmm##_lookup24_add, *xfer, cam, 0 ], sig_done[*sig_pair_ptr]  \
        }                                                                       \
    }                                                                           \
    return (__xread mem_cam_lookup24_add_out_t *)(xfer);                        \
    _INTRINSIC_END;                                                             \
}

#define _MEM_CAM128_256_LOOKUP24_ADD_PTR40(mmm)                                 \
__intrinsic                                                                     \
__xread mem_cam_lookup24_add_out_t *mem_cam##mmm##_lookup24_add_ptr40(          \
    mem_cam##mmm##_ptr40_t cam,                                                 \
    __xwrite mem_cam_lookup24_add_in_t *xfer,                                   \
    sync_t sync,                                                                \
    SIGNAL_PAIR *sig_pair_ptr                                                   \
    )                                                                           \
{                                                                               \
    _INTRINSIC_BEGIN;                                                           \
    {                                                                           \
        unsigned int hi_addr, low_addr;                                         \
        CT_ASSERT(__is_write_reg(xfer));                                        \
        CT_ASSERT(__is_ct_const(sync));                                         \
        CT_ASSERT(sync == sig_done);                                            \
        _INTRINSIC_CONVERT_HI_LO_ADDRESS(cam);                                  \
        __asm                                                                   \
        {                                                                       \
            mem[cam##mmm##_lookup24_add, *xfer, hi_addr, <<8, low_addr], sig_done[*sig_pair_ptr]  \
        }                                                                       \
    }                                                                           \
    return (__xread mem_cam_lookup24_add_out_t *)(xfer);                        \
    _INTRINSIC_END;                                                             \
}

#define _MEM_CAM384_512_LOOKUP24_ADD_PTR32(mmm)                                 \
__intrinsic                                                                     \
__xread mem_cam_lookup24_add_out_t *mem_cam##mmm##_lookup24_add_ptr32(          \
    __mem mem_cam##mmm##_in_mem_t *cam,                                         \
    __xwrite mem_cam_lookup24_add_in_t *xfer,                                   \
    sync_t sync,                                                                \
    SIGNAL_PAIR *sig_pair_ptr                                                   \
    )                                                                           \
{                                                                               \
    _INTRINSIC_BEGIN;                                                           \
    {                                                                           \
        generic_ind_t ind;                                                      \
        CT_ASSERT(__is_write_reg(xfer));                                        \
        CT_ASSERT(__is_ct_const(sync));                                         \
        CT_ASSERT(sync == sig_done);                                            \
        _INTRINSIC_OVERRIDE_LENGTH(ind, CAM_REF_COUNT_##mmm##_24);              \
        __asm alu[--, --, B, ALU_INDIRECT_TYPE(ind)]                            \
        __asm mem[cam##mmm##_lookup24_add, *xfer, cam, 0 ], sig_done[*sig_pair_ptr], indirect_ref \
    }                                                                           \
    return (__xread mem_cam_lookup24_add_out_t *)(xfer);                        \
    _INTRINSIC_END;                                                             \
}

#define _MEM_CAM384_512_LOOKUP24_ADD_PTR40(mmm)                                 \
__intrinsic                                                                     \
__xread mem_cam_lookup24_add_out_t *mem_cam##mmm##_lookup24_add_ptr40(          \
    mem_cam##mmm##_ptr40_t cam,                                                 \
    __xwrite mem_cam_lookup24_add_in_t *xfer,                                   \
    sync_t sync,                                                                \
    SIGNAL_PAIR *sig_pair_ptr                                                   \
    )                                                                           \
{                                                                               \
    _INTRINSIC_BEGIN;                                                           \
    {                                                                           \
        generic_ind_t ind;                                                      \
        unsigned int hi_addr, low_addr;                                         \
        CT_ASSERT(__is_write_reg(xfer));                                        \
        CT_ASSERT(__is_ct_const(sync));                                         \
        CT_ASSERT(sync == sig_done);                                            \
        _INTRINSIC_CONVERT_HI_LO_ADDRESS(cam);                                  \
        _INTRINSIC_OVERRIDE_LENGTH(ind, CAM_REF_COUNT_##mmm##_24);              \
        __asm alu[--, --, B, ALU_INDIRECT_TYPE(ind)]                            \
        __asm mem[cam##mmm##_lookup24_add, *xfer, hi_addr, <<8, low_addr], sig_done[*sig_pair_ptr], indirect_ref \
    }                                                                           \
    return (__xread mem_cam_lookup24_add_out_t *)(xfer);                        \
    _INTRINSIC_END;                                                             \
}

_MEM_CAM128_256_LOOKUP24_ADD_PTR32(128)
_MEM_CAM128_256_LOOKUP24_ADD_PTR32(256)
_MEM_CAM384_512_LOOKUP24_ADD_PTR32(384)
_MEM_CAM384_512_LOOKUP24_ADD_PTR32(512)

#ifdef __PTR40
_MEM_CAM128_256_LOOKUP24_ADD_PTR40(128)
_MEM_CAM128_256_LOOKUP24_ADD_PTR40(256)
_MEM_CAM384_512_LOOKUP24_ADD_PTR40(384)
_MEM_CAM384_512_LOOKUP24_ADD_PTR40(512)
#endif

/*
 * Add or Inc
 */
#define _MEM_CAM128_256_LOOKUP24_ADD_INC_PTR32(mmm)                             \
__intrinsic                                                                     \
__xread mem_cam_lookup24_add_inc_out_t *mem_cam##mmm##_lookup24_add_inc_ptr32(  \
    __mem mem_cam##mmm##_in_mem_t *cam,                                         \
    __xwrite mem_cam_lookup24_add_inc_in_t *xfer,                               \
    sync_t sync,                                                                \
    SIGNAL_PAIR *sig_pair_ptr                                                   \
    )                                                                           \
{                                                                               \
    _INTRINSIC_BEGIN;                                                           \
    {                                                                           \
        generic_ind_t ind;                                                      \
        CT_ASSERT(__is_write_reg(xfer));                                        \
        CT_ASSERT(__is_ct_const(sync));                                         \
        CT_ASSERT(sync == sig_done);                                            \
        _INTRINSIC_OVERRIDE_LENGTH(ind, CAM_REF_COUNT_##mmm##_24 | 2);          \
        __asm alu[--, --, B, ALU_INDIRECT_TYPE(ind)]                            \
        __asm mem[cam_lookup24_add_inc, *xfer, cam, 0 ], sig_done[*sig_pair_ptr], indirect_ref \
    }                                                                           \
    return (__xread mem_cam_lookup24_add_inc_out_t *)(xfer);                    \
    _INTRINSIC_END;                                                             \
}

#define _MEM_CAM128_256_LOOKUP24_ADD_INC_PTR40(mmm)                             \
__intrinsic                                                                     \
__xread mem_cam_lookup24_add_inc_out_t *mem_cam##mmm##_lookup24_add_inc_ptr40(  \
    mem_cam##mmm##_ptr40_t cam,                                                 \
    __xwrite mem_cam_lookup24_add_inc_in_t *xfer,                               \
    sync_t sync,                                                                \
    SIGNAL_PAIR *sig_pair_ptr                                                   \
    )                                                                           \
{                                                                               \
    _INTRINSIC_BEGIN;                                                           \
    {                                                                           \
        generic_ind_t ind;                                                      \
        unsigned int hi_addr, low_addr;                                         \
        CT_ASSERT(__is_write_reg(xfer));                                        \
        CT_ASSERT(__is_ct_const(sync));                                         \
        CT_ASSERT(sync == sig_done);                                            \
        _INTRINSIC_CONVERT_HI_LO_ADDRESS(cam);                                  \
        _INTRINSIC_OVERRIDE_LENGTH(ind, CAM_REF_COUNT_##mmm##_24 | 2);          \
        __asm alu[--, --, B, ALU_INDIRECT_TYPE(ind)]                            \
        __asm mem[cam_lookup24_add_inc, *xfer, hi_addr, <<8, low_addr], sig_done[*sig_pair_ptr], indirect_ref \
    }                                                                           \
    return (__xread mem_cam_lookup24_add_inc_out_t *)(xfer);                    \
    _INTRINSIC_END;                                                             \
}

#define _MEM_CAM384_512_LOOKUP24_ADD_INC_PTR32(mmm)                             \
__intrinsic                                                                     \
__xread mem_cam_lookup24_add_inc_out_t *mem_cam##mmm##_lookup24_add_inc_ptr32(  \
    __mem mem_cam##mmm##_in_mem_t *cam,                                         \
    __xwrite mem_cam_lookup24_add_inc_in_t *xfer,                               \
    sync_t sync,                                                                \
    SIGNAL_PAIR *sig_pair_ptr                                                   \
    )                                                                           \
{                                                                               \
    _INTRINSIC_BEGIN;                                                           \
    {                                                                           \
        generic_ind_t ind;                                                      \
        CT_ASSERT(__is_write_reg(xfer));                                        \
        CT_ASSERT(__is_ct_const(sync));                                         \
        CT_ASSERT(sync == sig_done);                                            \
        _INTRINSIC_OVERRIDE_LENGTH(ind, CAM_REF_COUNT_##mmm##_8 | 2);           \
        __asm alu[--, --, B, ALU_INDIRECT_TYPE(ind)]                            \
        __asm mem[cam##mmm##_lookup24_add_inc, *xfer, cam, 0 ], sig_done[*sig_pair_ptr], indirect_ref \
    }                                                                           \
    return (__xread mem_cam_lookup24_add_inc_out_t *)(xfer);                    \
    _INTRINSIC_END;                                                             \
}

#define _MEM_CAM384_512_LOOKUP24_ADD_INC_PTR40(mmm)                             \
__intrinsic                                                                     \
__xread mem_cam_lookup24_add_inc_out_t *mem_cam##mmm##_lookup24_add_inc_ptr40(  \
    mem_cam##mmm##_ptr40_t cam,                                                 \
    __xwrite mem_cam_lookup24_add_inc_in_t *xfer,                               \
    sync_t sync,                                                                \
    SIGNAL_PAIR *sig_pair_ptr                                                   \
    )                                                                           \
{                                                                               \
    _INTRINSIC_BEGIN;                                                           \
    {                                                                           \
        generic_ind_t ind;                                                      \
        unsigned int hi_addr, low_addr;                                         \
        CT_ASSERT(__is_write_reg(xfer));                                        \
        CT_ASSERT(__is_ct_const(sync));                                         \
        CT_ASSERT(sync == sig_done);                                            \
        _INTRINSIC_CONVERT_HI_LO_ADDRESS(cam);                                  \
        _INTRINSIC_OVERRIDE_LENGTH(ind, CAM_REF_COUNT_##mmm##_8 | 2);           \
        __asm alu[--, --, B, ALU_INDIRECT_TYPE(ind)]                            \
        __asm mem[cam##mmm##_lookup24_add_inc, *xfer, hi_addr, <<8, low_addr], sig_done[*sig_pair_ptr], indirect_ref \
    }                                                                           \
    return (__xread mem_cam_lookup24_add_inc_out_t *)(xfer);                    \
    _INTRINSIC_END;                                                             \
}

_MEM_CAM128_256_LOOKUP24_ADD_INC_PTR32(128)
_MEM_CAM128_256_LOOKUP24_ADD_INC_PTR32(256)
_MEM_CAM384_512_LOOKUP24_ADD_INC_PTR32(384)
_MEM_CAM384_512_LOOKUP24_ADD_INC_PTR32(512)

#ifdef __PTR40
_MEM_CAM128_256_LOOKUP24_ADD_INC_PTR40(128)
_MEM_CAM128_256_LOOKUP24_ADD_INC_PTR40(256)
_MEM_CAM384_512_LOOKUP24_ADD_INC_PTR40(384)
_MEM_CAM384_512_LOOKUP24_ADD_INC_PTR40(512)
#endif


#endif  /* __NFP_MEM_CAM_C__ */
