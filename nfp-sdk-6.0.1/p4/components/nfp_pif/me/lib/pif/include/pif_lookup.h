/* Copyright (C) 2015,  Netronome Systems, Inc.  All rights reserved. */

#ifndef __PIF_LOOKUP_H__
#define __PIF_LOOKUP_H__

#include <nfp.h>
#include <nfp/mem_bulk.h>

#include <dcfl_ctrl.h>

/* Invalid rule identifier to indicate a miss */
#define PIF_LOOKUP_MISS_RULE           (1 << 16)

/* The size of the DCFL table in bytes
 * 20MB is the default; this can be overridden
 * from a build define
 */
#ifndef DCFL_TABLE_SIZE
#define DCFL_TABLE_SIZE (20 * 1024 * 1024)
#endif

#define PIF_ACTIONDATA_FLAG_VALID      (1 << 0)
#define PIF_ACTIONDATA_FLAG_HASDEFAULT (1 << 1)

#ifdef DCFL_SPLIT_ARCH
#define DCFL_KEYQ_NUM 19
#define PIF_LOOKUP_KEY_SIZE_LW      PIF_PARREP_LEN_LW

void pif_lookup_dcfl_init();
#endif

struct pif_actiondata_table_desc {
    __emem uint32_t *table_address;

    uint32_t rule_count;

    uint16_t reserved;
    uint8_t record_shift;
    uint8_t flags;
};

struct pif_lookup_result {
    signed int action_id;
    signed int action_len;
};

struct pif_lookup_result
pif_lookup(unsigned int tableno,
           __lmem uint32_t *parrep,
           __mem uint32_t *action_buffer);

/* Load up the action buffer with a fixed action number
 * no real lookup happens
 */
__forceinline int pif_lookup_static(unsigned int actionid,
                                    __mem uint32_t *action_buffer)
{
#if 0
    /* this code does the same as below with more instructions */
    __gpr union pif_action_opdata opdata_buf;
    __xwrite union pif_action_opdata opdata;

    /* FIXME: the gpr + dual write is not really needed
     *        could be replace with a single ALU + << to the xfer
     */
    opdata_buf.val32 = 0;
    opdata_buf.action_id = actionid;
    opdata = opdata_buf;
#endif
    __xwrite union pif_action_opdata opdata;
    opdata.val32 = actionid << PIF_ACTION_OPDATA_ACTION_ID_off;

    mem_write32(&opdata, action_buffer, PIF_ACTION_OPDATA_LW * 4);

    return PIF_ACTION_OPDATA_LW;
}

/* same as above, put stick in a single lwarg */
__forceinline int pif_lookup_static_lwarg(unsigned int actionid,
                                          __mem uint32_t *action_buffer,
                                          uint32_t lwarg)
{
    __xwrite uint32_t xfer[2];
    __xwrite union pif_action_opdata *opdata = (__xwrite union pif_action_opdata *) &xfer[0];
    opdata->val32 = actionid << PIF_ACTION_OPDATA_ACTION_ID_off;
    xfer[1] = lwarg;

    mem_write32(xfer, action_buffer, PIF_ACTION_OPDATA_LW * 4 + 4);

    return PIF_ACTION_OPDATA_LW + 1;
}

#endif /* __PIF_LOOKUP_H__ */
