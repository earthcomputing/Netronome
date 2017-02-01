/* Copyright (C) 2015,  Netronome Systems, Inc.  All rights reserved. */

#ifndef __APP_COUNTERS_H__
#define __APP_COUNTERS_H__

#include <nfp/mem_atomic.h>

/*
 * counters defines
 */

#define PIF_COUNTER(_suffix) \
    extern __export __emem uint64_t pif_counter_##_suffix;

#define PIF_COUNTER_ARRAY(_suffix, _sz) \
    extern __export __emem uint64_t pif_counter_##_suffix[_sz];

#include "app_counter.inc"

#undef PIF_COUNTER_ARRAY
#undef PIF_COUNTER

/*
 * Macros for doing counts
 */

#define PIF_COUNT(_cnt_suffix) \
    mem_incr64((__mem void *)(&pif_counter_##_cnt_suffix));

/* use this for ones you want only for debug */
#define PIF_COUNT_DEBUG(_cnt_suffix) \
    mem_incr64((__mem void *)(&pif_counter_##_cnt_suffix));

#define PIF_COUNT_RANGE(_cnt_suffix, _offset) \
    mem_incr64((__mem void *)(pif_counter_##_cnt_suffix + _offset));

#endif /* __APP_COUNTERS_H__ */
