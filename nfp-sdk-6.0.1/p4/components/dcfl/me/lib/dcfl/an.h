/*
 * Copyright (C) 2014,  Netronome Systems, Inc.  All rights reserved.
 *
 * @file          lib/dcfl/an.h
 * @brief         Aggregation network 'crossproduct'.
 *
 */
#ifndef _DCFL__AN_H_
#define _DCFL__AN_H_

#if defined(__NFP_LANG_MICROC)
#include <nfp.h>
#include <nfp/me.h>
#include <nfp6000/nfp_me.h>
#include <nfp/mem_bulk.h>
#include <stdint.h>
#include <types.h>
#endif

#define DCFL_CONFIG_AN_LW     8
#if defined(__NFP_LANG_MICROC)
struct dcfl_config_an {
    union {
        struct {
            uint32_t base[DCFL_CONFIG_AN_LW];
        };
        uint32_t values[DCFL_CONFIG_AN_LW];
    };
};
#else
/* placeholder for assembler bitfield definitions */
#endif

/**
 * Aggregation network (number of nodes per block = m):
 * Each offset of each stage must start in a new block.
 *
 *      3                   2                   1
 *    1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 0 | label1                        | metalabel1 (relative pointer) |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * 1 | label2                        | metalabel2 (relative pointer) |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *m-1| pointer to extra entries                                      |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * n | labeln                        | metalabeln (relative pointer) |
 *   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *   etc..
 */

/* Value to test whether priority is uninitialised.
 * No rule should have this priority */
#define DCFL_MAGIC_PRIORITY         ((1<<16) - 1)

#define DCFL_AN_ENTRY_LW            1
#if defined(__NFP_LANG_MICROC)
struct dcfl_an_entry {
    union {
        struct {
            uint32_t label:16;          /* DCFL label  */
            uint32_t mlabel:16;         /* DCFL meta-label */
        };
        struct {
            uint32_t prio_even:16; /* Priority for rules with even AN offset */
            uint32_t prio_odd:16;  /* Priority for rules with odd AN offset */
        };
        uint32_t values[DCFL_AN_ENTRY_LW];
    };
};
#else
#define DCFL_LABEL_IN_AN_bf         0, 31, 16
#define DCFL_METALABEL_IN_AN_bf     0, 15, 0
#define DCFL_PRIORITY_ODD_bf        0, 15, 0
#define DCFL_PRIORITY_EVEN_bf       0, 31, 16
#endif /* defined(__NFP_LANG_MICROC) */

#if defined(__NFP_LANG_MICROC)
/**
 * Perform a DCFL Aggregation Network 'crossproduct
 *
 *
 */
int dcfl_an(__lmem struct dcfl_label_set *labels_in1,
                        uint32_t nlabel1,
                        __lmem struct dcfl_label_set *labels_in2,
                        uint32_t nlabel2,
                        __lmem struct dcfl_label_set *labels_out,
                        uint32_t stage_base, uint32_t stage_base_hi,
                        uint8_t is_not_final_stage
           );


#endif /* defined(__NFP_LANG_MICROC) */

#endif /* !_DCFL__AN_H_ */
