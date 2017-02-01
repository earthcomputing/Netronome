/*
 * Copyright (C) 2008-2014 Netronome Systems, Inc.  All rights reserved.
 */
#ifndef __NFP_OVERRIDE_C__
#define __NFP_OVERRIDE_C__

#ifndef NFP_LIB_ANY_NFCC_VERSION
    #if (!defined(__NFP_TOOL_NFCC) ||                       \
        (__NFP_TOOL_NFCC < NFP_SW_VERSION(5, 0, 0, 0)) ||   \
        (__NFP_TOOL_NFCC > NFP_SW_VERSION(6, 0, 1, 255)))
        #error "This standard library is not supported for the version of the SDK currently in use."
    #endif
#endif

#include <nfp.h>
#include <nfp_intrinsic.h>

/*
 * NOTE: When writing inline-asm, it's important not to use reserved words
 *       for variables.  Common ones are 'a','b', 'csr', 'state', and
 *       'inp_state'.
 */

#pragma diag_suppress 279  /* ignore while(0) etc in macros */

#define _OVR_LIMIT_CHECK(_value, _min, _max)                            \
do                                                                      \
{                                                                       \
    if (__is_ct_const(_value))                                          \
    {                                                                   \
        CT_ASSERT(((_value) >= (_min)) && ((_value) <= (_max)));        \
    }                                                                   \
    else                                                                \
    {                                                                   \
        RT_RANGE_ASSERT(((_value) >= (_min)) && ((_value) <= (_max)));  \
    }                                                                   \
} while(0)


#define _OVR_ERROR(f, s)                                                \
do                                                                      \
{                                                                       \
    if (__is_ct_const(f))                                               \
    {                                                                   \
        __ct_assert(0, s);                                              \
    }                                                                   \
    else                                                                \
    {                                                                   \
       RT_ASSERT(0);                                                    \
    }                                                                   \
} while(0)

#define _OVR_BYTE_MASK_CHECK(value)     _OVR_LIMIT_CHECK(value, 0, 0xff)
#define _OVR_LENGTH_CHECK(value)        _OVR_LIMIT_CHECK(value, 0, 0x1f)
#define _OVR_SIGNAL_NUMBER_CHECK(value) _OVR_LIMIT_CHECK(value, 0, 0xf)
#define _OVR_CTX_CHECK(value)           _OVR_LIMIT_CHECK(value, 0, 0x7)

#ifdef __NFP_INDIRECT_REF_FORMAT_NFP3200
    #define _OVR_XFER_CHECK(value)          _OVR_LIMIT_CHECK(value, 0, 0x3fff)
    #define _OVR_ME_CHECK(value)            _OVR_LIMIT_CHECK(value, 0, 0xff)
    #define _OVR_MASTER_CHECK(value)        _OVR_LIMIT_CHECK(value, 0, 0xff)
#endif

#ifdef __NFP_INDIRECT_REF_FORMAT_NFP6000
    #define _OVR_ISLAND_MASTER_CHECK(value)         _OVR_LIMIT_CHECK(value, 0, 0x3f)
    #define _OVR_SIGNAL_DATA_MASTER_CHECK(value)    _OVR_LIMIT_CHECK(value, 0, 0x3fff)
    #define _OVR_ISLAND_DATA_MASTER_CHECK(value)          _OVR_LIMIT_CHECK(value, 0, 0x3ff)

    #define _OVR_DATA_FULL_REF_CHECK(value)         _OVR_LIMIT_CHECK(value, 0, 0x3fff)
    #define _OVR_DATA_16BIT_IMM_CHECK(value)        _OVR_LIMIT_CHECK(value, 0, 0xffff)
    #define _OVR_DATA_CTX(value)                    _OVR_LIMIT_CHECK(value, 0, 0x7)
    #define _OVR_DATA_SIGNAL_CTX(value)             _OVR_LIMIT_CHECK(value, 0, 0x7)
    #define _OVR_DATA_OFFSET_CTX(value)             _OVR_LIMIT_CHECK(value, 0, 0x7f)

    #define _OVR_SIGNAL_MASTER_CHECK(value)         _OVR_LIMIT_CHECK(value, 0, 0xf)
#endif



/* Initialize an override word */
__intrinsic
void ovr_init(generic_ind_t *ind, unsigned int fields)
{

#ifdef __NFP_INDIRECT_REF_FORMAT_NFP3200
    /* ALL */
    if ((fields == (ovr_byte_mask | ovr_signal_ctx | ovr_xfer | ovr_length | ovr_signal_and_data_master)) ||
        (fields == (ovr_byte_mask | ovr_signal_ctx | ovr_xfer | ovr_length | ovr_me)))
    {
        *(unsigned int*)ind = 0xc0000000;
    }
    else
    /* 0111 - SIGNAL AND DATA_MASTER, SIGNAL CTX, XFER & LENGTH */
    if ((fields == (ovr_ctx | ovr_xfer | ovr_length | ovr_signal_and_data_master)) ||
        (fields == (ovr_ctx | ovr_xfer | ovr_length | ovr_me)))
    {
        *(unsigned int*)ind = 0x70000000;
    }
    else
    /* 0110 - SIGNAL MASTER, CTX & SIGNAL NUMBER */
    if (fields == (ovr_signal_master | ovr_ctx | ovr_signal_number))
    {
        *(unsigned int*)ind = 0x60000000;
    }
    else
    /* 1010 - DATA_MASTER, XFER & CTX */
    if (fields == (ovr_data_master | ovr_xfer | ovr_ctx))
    {
        *(unsigned int*)ind = 0xa0000000;
    }
    else
    /* 1011 - SIGNAL AND DATA_MASTER, XFER & CTX */
    if ((fields == (ovr_xfer | ovr_ctx | ovr_signal_and_data_master)) ||
        (fields == (ovr_xfer | ovr_ctx | ovr_me)))
    {
        *(unsigned int*)ind = 0xb0000000;
    }
    else
    /* 0101 - SIGNAL MASTER & CTX */
    if (fields == (ovr_signal_master | ovr_ctx))
    {
        *(unsigned int*)ind = 0x50000000;
    }
    else
    /* 1000 - DATA_MASTER & XFER */
    if (fields == (ovr_data_master | ovr_xfer))
    {
        *(unsigned int*)ind = 0x80000000;
    }
    else
    /* 1001 - DATA_MASTER & XFER */
    if ((fields == (ovr_xfer | ovr_signal_and_data_master)) ||
        (fields == (ovr_xfer | ovr_me)))
    {
        *(unsigned int*)ind = 0x90000000;
    }
    else
    /* 0001 - XFER & LENGTH */
    if (fields == (ovr_xfer | ovr_length))
    {
        *(unsigned int*)ind = 0x10000000;
    }
    else
    /* 0011 - LENGTH & BYTE MASK */
    if (fields == (ovr_byte_mask | ovr_length))
    {
        *(unsigned int*)ind = 0x30000000;
    }
    else
    /* 0000 - XFER */
    if (fields == ovr_xfer)
    {
        *(unsigned int*)ind = 0x00000000;
    }
    else
    /* 0010 - LENGTH */
    if (fields == ovr_length)
    {
        *(unsigned int*)ind = 0x20000000;
    }
    else
    /* 0100 - BYTEMASK */
    if (fields == ovr_byte_mask)
    {
        *(unsigned int*)ind = 0x40000000;
    }
    else
    {
        _OVR_ERROR(fields,"Unsupported override");
    }
#endif


#ifdef __NFP_INDIRECT_REF_FORMAT_NFP6000
    unsigned int alu = fields;

    (*ind).alu_ind.value = 0;
    (*ind).csr_ind.value = 0;


    /* Check that only one override uses the data field in the ALU result */
    if ((fields & ovr_data_byte_mask) &&
        (fields & (ovr_data_full_ref | ovr_data_16bit_imm | ovr_data_ctx
            | ovr_data_and_signal_ctx | ovr_data_ctx_offset))) {
        _OVR_ERROR(alu, "Unsupported override");
    } else if ((fields & ovr_data_full_ref) &&
        (fields & (ovr_data_byte_mask | ovr_data_16bit_imm | ovr_data_ctx
                | ovr_data_and_signal_ctx | ovr_data_ctx_offset))) {
        _OVR_ERROR(alu, "Unsupported override");
    } else if ((fields & ovr_data_16bit_imm) &&
        (fields & (ovr_data_byte_mask | ovr_data_full_ref | ovr_data_ctx
                | ovr_data_and_signal_ctx | ovr_data_ctx_offset))) {
        _OVR_ERROR(alu, "Unsupported override");
    } else if ((fields & ovr_data_ctx) &&
       (fields & (ovr_data_byte_mask | ovr_data_16bit_imm | ovr_data_full_ref
                | ovr_data_and_signal_ctx | ovr_data_ctx_offset))) {
        _OVR_ERROR(alu, "Unsupported override");
    } else if ((fields & ovr_data_and_signal_ctx) &&
       (fields & (ovr_data_byte_mask | ovr_data_16bit_imm | ovr_data_ctx | ovr_data_full_ref
                | ovr_data_ctx_offset))) {
        _OVR_ERROR(alu, "Unsupported override");
    } else if ((fields & ovr_data_ctx_offset) &&
            (fields & (ovr_data_byte_mask | ovr_data_16bit_imm | ovr_data_ctx | ovr_data_full_ref
                | ovr_data_and_signal_ctx ))) {
        _OVR_ERROR(alu, "Unsupported override");
    }

    /* Check that only one override uses the island master in the CSR IND result */
    if ((fields & ovr_island_and_data_master) &&
        (fields & (ovr_signal_island_and_data_master | ovr_island_master))) {
        _OVR_ERROR(alu, "Unsupported override");
    } else if ((fields & ovr_signal_island_and_data_master) &&
        (fields & (ovr_island_and_data_master | ovr_island_master))) {
        _OVR_ERROR(alu, "Unsupported override");
    } else if ((fields & ovr_island_master) &&
        (fields & (ovr_signal_island_and_data_master | ovr_island_and_data_master))) {
        _OVR_ERROR(alu, "Unsupported override");
    }

    /* Check that only one override uses the signal master in the CSR IND result */
    if ((fields & ovr_signal_master) &&
        (fields & ovr_signal_island_and_data_master)) {
        _OVR_ERROR(alu, "Unsupported override");
    }

            /* Bytemask  */
        if (fields & ovr_byte_mask)
        {
            (*ind).alu_ind.value |= ovr_byte_mask_bit;
        }
        if (fields & ovr_data_byte_mask)
        {
            (*ind).alu_ind.value |= ovr_data_byte_mask_bit;
        }
        if (fields & ovr_length)
        {
            (*ind).alu_ind.value |= ovr_length_bit;
        }

        /* Data fields */
        if (fields & ovr_data_full_ref)
        {
            (*ind).alu_ind.value |= ovr_data_full_ref_bit;
        }
        if (fields & ovr_data_16bit_imm)
        {
            (*ind).alu_ind.value |= ovr_data_16bit_imm_bit;
        }
        if (fields & ovr_data_ctx)
        {
            (*ind).alu_ind.value |= ovr_data_ctx_bit;
        }
        if (fields & ovr_data_and_signal_ctx)
        {
            (*ind).alu_ind.value |= ovr_data_and_signal_ctx_bit;
        }
        if (fields & ovr_data_ctx_offset)
        {
            (*ind).alu_ind.value |= ovr_data_ctx_offset_bit;
        }

        if (fields & ovr_signal_ctx)
        {
            (*ind).alu_ind.value |= ovr_signal_ctx_bit;
        }
        if (fields & ovr_signal_master)
        {
            (*ind).alu_ind.value |= ovr_signal_master_bit;
        }
        if (fields & ovr_signal_number)
        {
            (*ind).alu_ind.value |= ovr_signal_number_bit;
        }

        /* Master fields */
        if (fields & ovr_island_and_data_master)
        {
            (*ind).alu_ind.value |= ovr_data_master_bit;
        }

        if (fields & ovr_signal_island_and_data_master)
        {
            (*ind).alu_ind.value |= ovr_signal_and_data_master_bit;
        }

        if (fields & ovr_island_master)
        {
            (*ind).alu_ind.value |= ovr_island_master_bit;
        }

#endif
}


/* Set the value of an override field */
__intrinsic
void ovr_set(generic_ind_t *ind, ovr_field_t field, int value)
{
#ifdef __NFP_INDIRECT_REF_FORMAT_NFP6000
    {
    unsigned int alu = (*ind).alu_ind.value;

    CT_ASSERT(__is_ct_const(field));

        /* Bytemask in CSR */
        if (field & ovr_byte_mask)
        {
            unsigned int result = alu & ovr_byte_mask_bit;
            _OVR_BYTE_MASK_CHECK(value);

            if (result)
            {
                (*ind).csr_ind.byte_mask = value;
            } else {
                _OVR_ERROR(result, "Cannot set ovr_byte_mask");
            }
        }
        /* Bytemask in data (ALU) */
        if (field & ovr_data_byte_mask)
        {
            unsigned int result = alu & ovr_data_byte_mask_bit;
            _OVR_BYTE_MASK_CHECK(value);

            if (result)
            {
                (*ind).alu_ind.data16 = value;
            } else {
                _OVR_ERROR(result, "Cannot set ovr_data_byte_mask");
            }
        }
        if (field & ovr_length)
        {
            unsigned int result = alu & ovr_length_bit;
            _OVR_LENGTH_CHECK(value);

            if (result)
            {
                (*ind).alu_ind.ref_count = value;
            } else {
                _OVR_ERROR(result, "Cannot set ovr_length");
            }
        }

        /* Data fields */
        if (field & ovr_data_full_ref)
        {
            unsigned int result = alu & ovr_data_full_ref_bit;
            _OVR_DATA_FULL_REF_CHECK(value);

            if (result)
            {
                (*ind).alu_ind.data16 = value;
            } else {
                _OVR_ERROR(result, "Cannot set ovr_data_full_ref");
            }
        }
        if (field & ovr_data_16bit_imm)
        {
            unsigned int result = alu & ovr_data_16bit_imm_bit;
           _OVR_DATA_16BIT_IMM_CHECK(value);

            if (result)
            {
                (*ind).alu_ind.data16 = value;
            } else {
                _OVR_ERROR(result, "Cannot set ovr_data_16bit_imm");
            }
        }
        if (field & ovr_data_ctx)
        {
            unsigned int result = alu & ovr_data_ctx_bit;
            _OVR_DATA_CTX(value);

            if (result)
            {
                (*ind).alu_ind.data16 = value;
            } else {
                _OVR_ERROR(result, "Cannot set ovr_data_ctx");
            }
        }
        if (field & ovr_data_and_signal_ctx)
        {
            unsigned int result = alu & ovr_data_and_signal_ctx_bit;
            _OVR_DATA_SIGNAL_CTX(value);

            if (result)
            {
                (*ind).alu_ind.data16 = value;
            } else {
                _OVR_ERROR(result, "Cannot set ovr_data_and_signal_ctx");
            }
         }
        if (field & ovr_data_ctx_offset)
        {
            unsigned int result = alu & ovr_data_ctx_offset_bit;
           _OVR_DATA_OFFSET_CTX(value);

            if (result)
            {
                (*ind).alu_ind.data16 = value << 7;
            } else {
                _OVR_ERROR(result, "Cannot set ovr_data_ctx_offset");
            }
        }

        if (field & ovr_signal_ctx)
        {
            unsigned int result = alu & ovr_signal_ctx_bit;
            _OVR_CTX_CHECK(value);

            if (result)
            {
                (*ind).csr_ind.signal_ctx = value;
            } else {
                _OVR_ERROR(result, "Cannot set ovr_signal_ctx");
            }
        }
        if (field & ovr_signal_master)
        {
            unsigned int result = alu & ovr_signal_master_bit;
            _OVR_SIGNAL_MASTER_CHECK(value);

            if (result)
            {
                (*ind).csr_ind.signal_master = value;
            } else {
                _OVR_ERROR(result, "Cannot set ovr_signal_master");
            }
        }
        if (field & ovr_signal_number)
        {
            unsigned int result = alu & ovr_signal_number_bit;
            _OVR_SIGNAL_NUMBER_CHECK(value);

            if (result)
            {
                (*ind).csr_ind.signal_num = value;
            } else {
                _OVR_ERROR(result, "Cannot set ovr_signal_number");
            }
        }

        /* Master fields */
        if (field & ovr_island_and_data_master)
        {
            unsigned int result = alu & ovr_data_master_bit;
            _OVR_ISLAND_DATA_MASTER_CHECK(value);

            if (result)
            {
                (*ind).csr_ind.value |= ((value & 0x3ff) << 20);      // 10 bits - island+master
            } else {
                _OVR_ERROR(result, "Cannot set ovr_island_and_data_master");
            }
        }

        if (field & ovr_signal_island_and_data_master)
        {
            unsigned int result = alu & ovr_signal_and_data_master_bit;
            _OVR_SIGNAL_DATA_MASTER_CHECK(value);

            if (result)
            {
                (*ind).csr_ind.value |=  ((value & 0x3fff) << 16);     // 14 bits - island+master+signal_master
            } else {
                _OVR_ERROR(result, "Cannot set ovr_signal_and_data_master");
            }
        }

        if (field & ovr_island_master)
        {
            unsigned int result = alu & ovr_island_master_bit;
            _OVR_ISLAND_MASTER_CHECK(value);

            if (result)
            {
                (*ind).csr_ind.island = value;      // 6 bits - island
            } else {
                _OVR_ERROR(result, "Cannot set ovr_island_master");
            }
        }
    }
#endif  /* #ifdef __NFP_INDIRECT_REF_FORMAT_NFP6000 */

#ifdef __NFP_INDIRECT_REF_FORMAT_NFP3200
{
    unsigned int u = (unsigned int)(*ind).value>>28;
    CT_ASSERT(__is_ct_const(field));
     /* Override all */
    if ((u & 0xc) == 0xc)
    {
        if (field & ovr_byte_mask)
        {
            _OVR_BYTE_MASK_CHECK(value);
            (*ind).all_ind.byte_mask = value;
        }
        if (field & ovr_ctx)
        {
            _OVR_ERROR(u, "Cannot set ovr_ctx");
        }
        if (field & ovr_length)
        {
            _OVR_LENGTH_CHECK(value);
            (*ind).all_ind.ref_count = value;
        }
        if (field & ovr_me)
        {
            _OVR_LIMIT_CHECK(value, 0, 0x1f);       /* Only 5 bits possible with this override */
            (*ind).all_ind.master = value | 0x08;   /* Set bit 3 to ensure the PPID is interpreted as ME number */
        }
        if (field & ovr_xfer)
        {
            _OVR_LIMIT_CHECK(value, 0, 0xff);       /* Only 8 bits possible with this override */
            (*ind).all_ind.xadd_imm = value;
        }
        if (field & ovr_signal_ctx)
        {
            _OVR_CTX_CHECK(value);
            (*ind).all_ind.sig_ctx = value;
        }
        if (field & ovr_signal_master)
        {
            _OVR_ERROR(u, "Cannot set ovr_signal_master");
        }
        if (field & ovr_signal_number)
        {
            _OVR_ERROR(u, "Cannot set ovr_signal_number");
        }
        if (field & ovr_data_master)
        {
            _OVR_ERROR(u, "Cannot set ovr_data_master");
        }
        if (field & ovr_signal_and_data_master)
        {
            _OVR_MASTER_CHECK(value);
            (*ind).all_ind.master = value;
        }
    }
    else
    {
        switch(u & 0xf)
        {
            /* XFER */
            case 0x0:
                if (field & ovr_byte_mask)
                {
                    _OVR_ERROR(u, "Cannot set ovr_byte_mask");
                }
                if (field & ovr_ctx)
                {
                    _OVR_ERROR(u, "Cannot set ovr_ctx");
                }
                if (field & ovr_length)
                {
                    _OVR_LENGTH_CHECK(value);
                    (*ind).xfer_cnt_ind.ref_count = value;
                }
                if (field & ovr_me)
                {
                    _OVR_ERROR(u, "Cannot set ovr_me");
                }
                if (field & ovr_xfer)
                {
                    _OVR_XFER_CHECK(value);
                    (*ind).xfer_cnt_ind.xadd_imm = value;
                }
                if (field & ovr_signal_ctx)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_ctx");
                }
                if (field & ovr_signal_master)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_master");
                }
                if (field & ovr_signal_number)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_number");
                }
                if (field & ovr_data_master)
                {
                    _OVR_ERROR(u, "Cannot set ovr_data_master");
                }
                if (field & ovr_signal_and_data_master)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_and_data_master");
                }
                break;

            /* XFER & LENGTH */
            case 0x1:
                if (field & ovr_byte_mask)
                {
                    _OVR_ERROR(u, "Cannot set ovr_byte_mask");
                }
                if (field & ovr_ctx)
                {
                    _OVR_ERROR(u, "Cannot set ovr_ctx");
                }
                if (field & ovr_length)
                {
                    _OVR_LENGTH_CHECK(value);
                    (*ind).xfer_cnt_ind.ref_count = value;
                }
                if (field & ovr_me)
                {
                    _OVR_ERROR(u, "Cannot set ovr_me");
                }
                if (field & ovr_xfer)
                {
                    _OVR_XFER_CHECK(value);
                    (*ind).xfer_cnt_ind.xadd_imm = value;
                }
                if (field & ovr_signal_ctx)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_ctx");
                }
                if (field & ovr_signal_master)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_master");
                }
                if (field & ovr_signal_number)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_number");
                }
                if (field & ovr_data_master)
                {
                    _OVR_ERROR(u, "Cannot set ovr_data_master");
                }
                if (field & ovr_signal_and_data_master)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_and_data_master");
                }
                break;

            /* LENGTH */
            case 0x2:
                if (field & ovr_byte_mask)
                {
                    _OVR_ERROR(u, "ovr_byte_mask");
                }
                if (field & ovr_ctx)
                {
                    _OVR_ERROR(u, "Cannot set ovr_ctx");
                }
                if (field & ovr_length)
                {
                    _OVR_LENGTH_CHECK(value);
                    (*ind).xfer_cnt_ind.ref_count = value;
                }
                if (field & ovr_me)
                {
                    _OVR_ERROR(u, "Cannot set ovr_me");
                }
                if (field & ovr_xfer)
                {
                    _OVR_ERROR(u, "Cannot set ovr_xfer");
                }
                if (field & ovr_signal_ctx)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_ctx");
                }
                if (field & ovr_signal_master)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_master");
                }
                if (field & ovr_signal_number)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_number");
                }
                if (field & ovr_data_master)
                {
                    _OVR_ERROR(u, "Cannot set ovr_data_master");
                }
                if (field & ovr_signal_and_data_master)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_and_data_master");
                }
                break;

            /* LENGTH & BYTE_MASK */
            case 0x3:
                if (field & ovr_byte_mask)
                {
                    _OVR_BYTE_MASK_CHECK(value);
                     (*ind).cnt_mask_ind.byte_mask = value;
                }
                if (field & ovr_ctx)
                {
                    _OVR_ERROR(u, "Cannot set ovr_ctx");
                }
                if (field & ovr_length)
                {
                    _OVR_LENGTH_CHECK(value);
                    (*ind).cnt_mask_ind.ref_count = value;
                }
                if (field & ovr_me)
                {
                    _OVR_ERROR(u, "Cannot set ovr_me");
                }
                if (field & ovr_xfer)
                {
                    _OVR_ERROR(u, "Cannot set ovr_xfer");
                }
                if (field & ovr_signal_ctx)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_ctx");
                }
                if (field & ovr_signal_master)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_master");
                }
                if (field & ovr_signal_number)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_number");
                }
                if (field & ovr_data_master)
                {
                    _OVR_ERROR(u, "Cannot set ovr_data_master");
                }
                if (field & ovr_signal_and_data_master)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_and_data_master");
                }
                break;

            /*  BYTE_MASK */
            case 0x4:
                if (field & ovr_byte_mask)
                {
                    _OVR_BYTE_MASK_CHECK(value);
                    (*ind).mask_ind.byte_mask = value;
                }
                if (field & ovr_ctx)
                {
                    _OVR_ERROR(u, "Cannot set ovr_ctx");
                }
                if (field & ovr_length)
                {
                    _OVR_ERROR(u,"Cannot set ovr_length 2");
                }
                if (field & ovr_me)
                {
                    _OVR_ERROR(u, "Cannot set ovr_me");
                }
                if (field & ovr_xfer)
                {
                    _OVR_ERROR(u, "Cannot set ovr_xfer");
                }
                if (field & ovr_signal_ctx)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_ctx");
                }
                if (field & ovr_signal_master)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_master");
                }
                if (field & ovr_signal_number)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_number");
                }
                if (field & ovr_data_master)
                {
                    _OVR_ERROR(u, "Cannot set ovr_data_master");
                }
                if (field & ovr_signal_and_data_master)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_and_data_master");
                }
                break;

            /*  SIGNAL AND DATA MASTER & CONTEXT */
            case 0x5:
                if (field & ovr_byte_mask)
                {
                    _OVR_ERROR(u, "Cannot set ovr_byte_mask");
                }
                if (field & ovr_ctx)
                {
                    _OVR_CTX_CHECK(value);
                    (*ind).me_ctx_ind.ctx = value;
                }
                if (field & ovr_length)
                {
                    _OVR_ERROR(u, "Cannot set ovr_length 3");
                }
                if (field & ovr_me)
                {
                    _OVR_ME_CHECK(value);
                    (*ind).me_ctx_ind.master = value | 0x08;   /* Set bit 3 to ensure the PPID is interpreted as ME number */
                }
                if (field & ovr_xfer)
                {
                    _OVR_ERROR(u, "Cannot set ovr_xfer");
                }
                if (field & ovr_signal_ctx)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_ctx");
                }
                if (field & ovr_signal_master)
                {
                    _OVR_MASTER_CHECK(value);
                    (*ind).me_ctx_ind.master = value;
                }
                if (field & ovr_signal_number)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_number");
                }
                if (field & ovr_data_master)
                {
                    _OVR_ERROR(u, "Cannot set ovr_data_master");
                }
                if (field & ovr_signal_and_data_master)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_and_data_master");
                }
                break;

            /*  SIGNAL MASTER, CTX & SIGNAL NUMBER */
            case 0x6:
                if (field & ovr_byte_mask)
                {
                    _OVR_ERROR(u, "Cannot set ovr_byte_mask");
                }
                if (field & ovr_ctx)
                {
                    _OVR_CTX_CHECK(value);
                    (*ind).me_ctx_sig_ind.ctx = value;
                }
                if (field & ovr_length)
                {
                    _OVR_ERROR(u, "Cannot set ovr_length 4");
                }
                if (field & ovr_me)
                {
                    _OVR_ERROR(u, "Cannot set ovr_me");
                }
                if (field & ovr_xfer)
                {
                    _OVR_ERROR(u, "Cannot set ovr_xfer");
                }
                if (field & ovr_signal_ctx)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_ctx");
                }
                if (field & ovr_signal_master)
                {
                    _OVR_MASTER_CHECK(value);
                    (*ind).me_ctx_sig_ind.master = value;
                }
                if (field & ovr_signal_number)
                {
                    _OVR_SIGNAL_NUMBER_CHECK(value);
                    (*ind).me_ctx_sig_ind.sig = value;
                }
                if (field & ovr_data_master)
                {
                    _OVR_ERROR(u, "Cannot set ovr_data_master");
                }
                if (field & ovr_signal_and_data_master)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_and_data_master");
                }
                break;

            /*  SIGNAL AND DATA MASTER, SIGNAL CTX, XFER & LENGTH */
            case 0x7:
                if (field & ovr_byte_mask)
                {
                    _OVR_ERROR(u, "Cannot set ovr_byte_mask");
                }
                if (field & ovr_ctx)
                {
                    _OVR_CTX_CHECK(value);
                    (*ind).me_xfer_ctx_cnt_ind.ctx = value;
                }
                if (field & ovr_length)
                {
                    _OVR_LENGTH_CHECK(value);
                    (*ind).me_xfer_ctx_cnt_ind.ref_count = value;
                }
                if (field & ovr_me)
                {
                    _OVR_ME_CHECK(value);
                    (*ind).me_xfer_ctx_cnt_ind.master = value | 0x8;
                }
                if (field & ovr_xfer)
                {
                    _OVR_LIMIT_CHECK(value, 0, 0xff);  /* Only 8 bits */
                    (*ind).me_xfer_ctx_cnt_ind.xadd_imm = value;
                }
                if (field & ovr_signal_ctx)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_ctx");
                }
                if (field & ovr_signal_master)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_master");
                }
                if (field & ovr_signal_number)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_number");
                }
                if (field & ovr_data_master)
                {
                    _OVR_ERROR(u, "Cannot set ovr_data_master");
                }
                if (field & ovr_signal_and_data_master)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_and_data_master");
                }
                break;

            /*  DATA MASTER, XFER */
            case 0x8:
                if (field & ovr_byte_mask)
                {
                    _OVR_ERROR(u, "Cannot set ovr_byte_mask");
                }
                if (field & ovr_ctx)
                {
                    _OVR_ERROR(u, "Cannot set ovr_ctx");
                }
                if (field & ovr_length)
                {
                    _OVR_ERROR(u, "Cannot set ovr_length 5");
                }
                if (field & ovr_me)
                {
                    _OVR_ERROR(u, "Cannot set ovr_me");
                }
                if (field & ovr_xfer)
                {
                    _OVR_XFER_CHECK(value);
                    (*ind).me_xfer_ind.xadd_imm = value;
                }
                if (field & ovr_signal_ctx)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_ctx");
                }
                if (field & ovr_signal_master)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_master");
                }
                if (field & ovr_signal_number)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_number");
                }
                if (field & ovr_data_master)
                {
                    _OVR_MASTER_CHECK(value);
                    (*ind).me_xfer_ind.master = value;
                }
                if (field & ovr_signal_and_data_master)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_and_data_master");
                }
                break;

            /*  ME, XFER */
            case 0x9:
                if (field & ovr_byte_mask)
                {
                    _OVR_ERROR(u, "Cannot set ovr_byte_mask");
                }
                if (field & ovr_ctx)
                {
                    _OVR_ERROR(u, "Cannot set ovr_ctx");
                }
                if (field & ovr_length)
                {
                    _OVR_ERROR(u, "Cannot set ovr_length 6");
                }
                if (field & ovr_me)
                {
                    (*ind).me_xfer_ind.master = value | 0x08;   /* Set bit 3 to ensure the PPID is interpreted as ME number */;
                }
                if (field & ovr_xfer)
                {
                    _OVR_XFER_CHECK(value);
                    (*ind).me_xfer_ind.xadd_imm = value;
                }
                if (field & ovr_signal_ctx)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_ctx");
                }
                if (field & ovr_signal_master)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_master");
                }
                if (field & ovr_signal_number)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_number");
                }
                if (field & ovr_data_master)
                {
                    _OVR_ERROR(u, "Cannot set ovr_data_master");
                }
                if (field & ovr_signal_and_data_master)
                {
                    _OVR_MASTER_CHECK(value);
                    (*ind).me_xfer_ind.master = value;
                }
                break;

            /*  DATA MASTER, XFER & CTX */
            case 0xa:
                if (field & ovr_byte_mask)
                {
                    _OVR_ERROR(u, "Cannot set ovr_byte_mask");
                }
                if (field & ovr_ctx)
                {
                    _OVR_CTX_CHECK(value);
                    (*ind).me_xfer_ctx_ind.ctx = value;
                }
                if (field & ovr_length)
                {
                    _OVR_ERROR(u, "Cannot set ovr_length 7");
                }
                if (field & ovr_me)
                {
                    _OVR_ERROR(u, "Cannot set ovr_me");
                }
                if (field & ovr_xfer)
                {
                    _OVR_XFER_CHECK(value);
                    (*ind).me_xfer_ctx_ind.xadd_imm = value;
                }
                if (field & ovr_signal_ctx)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_ctx");
                }
                if (field & ovr_signal_master)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_master");
                }
                if (field & ovr_signal_number)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_number");
                }
                if (field & ovr_data_master)
                {
                    _OVR_MASTER_CHECK(value);
                    (*ind).me_xfer_ctx_ind.master = value;
                }
                if (field & ovr_signal_and_data_master)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_and_data_master");
                }
                break;


            /*  SIGNAL AND DATA MASTER, XFER & CTX */
            case 0xb:
                if (field & ovr_byte_mask)
                {
                    _OVR_ERROR(u, "Cannot set ovr_byte_mask");
                }
                if (field & ovr_ctx)
                {
                    _OVR_CTX_CHECK(value);
                    (*ind).me_xfer_ctx_ind.ctx = value;
                }
                if (field & ovr_length)
                {
                    _OVR_ERROR(u, "Cannot set ovr_length 8");
                }
                if (field & ovr_me)
                {
                    _OVR_ME_CHECK(value);
                    (*ind).me_xfer_ctx_ind.master = value | 0x08;   /* Set bit 3 to ensure the PPID is interpreted as ME number */
                }
                if (field & ovr_xfer)
                {
                    _OVR_XFER_CHECK(value);
                    (*ind).me_xfer_ctx_ind.xadd_imm = value;
                }
                if (field & ovr_signal_ctx)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_ctx");
                }
                if (field & ovr_signal_master)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_master");
                }
                if (field & ovr_signal_number)
                {
                    _OVR_ERROR(u, "Cannot set ovr_signal_number");
                }
                if (field & ovr_data_master)
                {
                    _OVR_ERROR(u, "Cannot set ovr_data_master");
                }
                if (field & ovr_signal_and_data_master)
                {
                    _OVR_MASTER_CHECK(value);
                    (*ind).me_xfer_ctx_ind.master = value;
                }
                break;

            default:
                _OVR_ERROR(u, "Unknown override");
                break;
        } //switch
    }
    }
#endif  /* #ifdef __NFP_INDIRECT_REF_FORMAT_NFP3200 */
}

#pragma diag_default 279  /* suppressed above */

#endif /* __NFP_OVERRIDE_C__ */
