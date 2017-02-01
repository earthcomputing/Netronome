/*
 * Copyright (C) 2014,  Netronome Systems, Inc.  All rights reserved.
 *
 * @file          lib/dcfl/_c/an.c
 * @brief         DCFL Aggregation network implementation
 */

#include <an.h>

int
dcfl_an(__lmem struct dcfl_label_set *labels_in1, uint32_t nlabel1,
        __lmem struct dcfl_label_set *labels_in2, uint32_t nlabel2,
        __lmem struct dcfl_label_set *labels_out,
        uint32_t stage_base, uint32_t stage_base_hi,
        uint8_t is_not_final_stage
       )
{
    uint32_t labels_in1_id, labels_in2_id, labels_out_cnt = 0;
    uint32_t i, num_an_entries = DCFL_AN_BLOCK_NUM_ENTRIES;
    __declspec(addr40) unsigned long *an_base;
    __declspec(addr40) unsigned long *addr_an_block;
    __xread struct dcfl_an_entry
        an_entry[DCFL_MEM_AN_BLOCK_LW/DCFL_AN_ENTRY_LW];
    __xread struct dcfl_an_entry prio_entry;
    __gpr uint32_t priority_idx, priority;
#ifdef DCFL_SUPPORT_MATCH_FILTERING
    __gpr uint32_t lowest_prio, prev_prio = DCFL_MAGIC_PRIORITY;
    __gpr uint32_t priority_rule_idx, prio_found = 0;
#else
#ifdef DCFL_PRIORITY_SORT
    __gpr uint32_t sorted;
    __gpr uint32_t temp;
#endif
#endif

    if (is_not_final_stage == 0) {
        num_an_entries = DCFL_AN_FIN_BLOCK_NUM_RULES;
    }

    an_base = (__addr40 void *) (((unsigned long long)stage_base_hi << 32) |
               (unsigned long long)stage_base);


    for (labels_in1_id=0; labels_in1_id<nlabel1 ; labels_in1_id++) {
        /* add the offset to read the AN block */
        /* TODO check if this will work for any memory type */
        addr_an_block = (unsigned long *) an_base +
            ((labels_in1->labels[labels_in1_id])
                * DCFL_MEM_AN_BLOCK_LW);

        mem_read64(&an_entry, addr_an_block, sizeof(an_entry));

        start_of_block:

        for (labels_in2_id=0; labels_in2_id<nlabel2 ; labels_in2_id++) {

            /* go through the entries in the AN block */
            for (i = 0; i < num_an_entries; i++) {

                /* if there is a label match add the metalabel to the
                 * labelset and break (same label is only once in the
                 * same block, except for the final stage) */
                if (an_entry[i].label ==
                    labels_in2->labels[labels_in2_id]) {
#ifdef DCFL_SUPPORT_MATCH_FILTERING
                    if (is_not_final_stage == 0) {
                        priority_idx = DCFL_AN_FIN_BLOCK_NUM_RULES + (i/2);
                        mem_read32(&prio_entry, addr_an_block + priority_idx,
                                   sizeof(uint32_t));
                        if (i % 2) {
                            priority = prio_entry.prio_even;
                        } else {
                            priority = prio_entry.prio_odd;
                        }

                        /* Determine the highest priority rule (lowest prio value).
                         * If all priorities are identical, no filtering will occur. */
                        if (prev_prio == DCFL_MAGIC_PRIORITY) {
                            lowest_prio = prev_prio = priority;
                        } else {
                            if (priority != prev_prio) {
                                /* Differing priority in matched result */
                                if (priority < lowest_prio) {
                                    prio_found = 1;
                                    lowest_prio = priority;
                                    priority_rule_idx = labels_out_cnt;
                                }
                            }
                            prev_prio = priority;
                        }
                    }
                    labels_out->labels[labels_out_cnt] =
                        an_entry[i].mlabel;
                    labels_out_cnt++;
#else
#ifdef DCFL_PRIORITY_SORT
                    if (is_not_final_stage == 0) {
                        priority_idx = DCFL_AN_FIN_BLOCK_NUM_RULES + (i/2);
                        mem_read32(&prio_entry, addr_an_block + priority_idx,
                                   sizeof(uint32_t));
                        if (i % 2) {
                            priority = prio_entry.prio_even;
                        } else {
                            priority = prio_entry.prio_odd;
                        }

                        labels_out->results[labels_out_cnt].priority = priority;
                        labels_out->results[labels_out_cnt].rule_id =
                            an_entry[i].mlabel;
                    } else {
                        labels_out->labels[labels_out_cnt] =
                            an_entry[i].mlabel;
                    }
                    labels_out_cnt++;
#else
                    labels_out->labels[labels_out_cnt] =
                        an_entry[i].mlabel;
                    labels_out_cnt++;
#endif
#endif

                    if (is_not_final_stage) {
                        break;
                    }
                }
            } /* for (i=0; */
        } /* for(labels_in2_id=0; */

        /* Extra blocks? */
        if (an_entry[DCFL_AN_BLOCK_NUM_ENTRIES].values[0] > 0) {
            addr_an_block = (__addr40 void *)
                (((unsigned long long)stage_base_hi << 32) |
                (unsigned long long)an_entry[DCFL_AN_BLOCK_NUM_ENTRIES]
                    .values[0]);
            mem_read64(&an_entry, addr_an_block, sizeof(an_entry));
            goto start_of_block;
        }
    } /* for (labels_in1_id=0; */

#ifdef DCFL_SUPPORT_MATCH_FILTERING
    /* Return only the priority rule */
    if (prio_found) {
        labels_out->labels[0] = labels_out->labels[priority_rule_idx];
        return 1;
    }
#endif

    return labels_out_cnt;
}
