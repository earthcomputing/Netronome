/*
 * Copyright (C) 2014-2016,  Netronome Systems, Inc.  All rights reserved.
 *
 * @file        camp_hash_c.h
 * @brief       Prototypes for camp_hash.c
 */

#ifndef _CAMP_HASH_C_H_
#define _CAMP_HASH_C_H_

#include <nfp.h>
#include <stdint.h>
#include <nfp/me.h>


// #include "flow_cache_global_c.h"
/** Hash return value
 */
typedef struct hash_result_t
{
    unsigned int value_0;
    unsigned int value_1;
} hash_result_t;

void camp_hash_init(void);

__gpr hash_result_t camp_hash(__lmem uint32_t *in_key_index, __gpr uint32_t in_key_length);


#endif
