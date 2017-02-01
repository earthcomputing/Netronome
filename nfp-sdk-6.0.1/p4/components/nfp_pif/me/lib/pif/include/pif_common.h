/* Copyright (C) 2015,  Netronome Systems, Inc.  All rights reserved. */

#ifndef __PIF_COMMON_H__
#define __PIF_COMMON_H__

#include <stdint.h>
#include <nfp.h>

#include <pif_design.h>

#include <pif_memops.h>
#include <pif_pkt.h>
#include <pif_headers.h>
#include <pif_parse_errors.h>
#include <pif_parrep.h>
#include <pif_action.h>
#include <pif_actions.h>
#include <pif_ctlflow.h>
#include <pif_tables.h>
#include <pif_lookup.h>
#include <pif_deparse.h>
#include <pif_registers.h>
#include <pif_digests.h>
#include <pif_field_lists.h>
#include <pif_meter.h>
#include <pif_meters.h>
#include <pif_global.h>
#include <pif_pkt_duplication.h>
#include <pif_pkt_recurse.h>
#include <pif_pkt_clone.h>
#include <pif_flcalc_algorithms.h>
#include <pif_flcalc.h>
#include <pif_debug.h>

/* export the nfcc built-in */
void __debug_label(const char *);

/* FIXME: move somewhere sensible */
int pif_parrep_extract(__lmem uint32_t *lm_ptr,
                       __mem uint8_t *pktdata,
                       unsigned int pktlen,
                       __lmem uint32_t *fk_ptr,
                       __gpr uint32_t *fk_len);

int pif_parrep_exceptions(__lmem uint32_t *parrep,
                          int result);

/* FIXME: move somewhere sensible */
void pif_master_init(void);

/* extern for flowcache invalidation/bypassing */
extern __nnr uint32_t pif_flowcache_bypass;


#endif /* __PIF_COMMON_H__ */
