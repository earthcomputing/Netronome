/*
 * Copyright 2015 Netronome, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file          app_master.c
 * @brief         Dummy code for ME serving as the NFD app master
 */

#include <assert.h>
#include <nfp_chipres.h>
#include <nfp.h>

#include <nfp/me.h>
#include <nfp/mem_bulk.h>

#include <nfp6000/nfp_me.h>

#include <hashmap.h>

#ifndef PIF_GLOBAL_FLOWCACHE_DISABLED
#include <flow_cache_c.h>
#endif

#include <pif_common.h>

#include <system_init_c.h>

int
main(void)
{
#ifndef PIF_GLOBAL_FLOWCACHE_DISABLED
    fc_init();
#endif

    if (ctx() == 0) {
        camp_hash_init();
        pif_master_init();
        system_init_done();
    }

    /* wait for ctx 0 to catch up */
    system_reorder_contexts();

    for (;;) {
        if (ctx() == 2) {
            pif_value_set_scan_configs();
            sleep(100);
            ctx_swap();
        } else
            ctx_swap();
    }
}
