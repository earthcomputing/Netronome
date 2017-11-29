/**
 * Copyright (C) 2017,  Earth Computing Inc.  All rights reserved.
 *
 *
 * @file          pif_common.h
 * @brief         local copy with removed missing files to resolove compile errors somehow 
 *
 * Author:        Atsushi Kasuya
 *
 */
#ifndef __PIF_COMMON_H__
#define __PIF_COMMON_H__

#include <stdint.h>
#include <nfp.h>

#define PIF_PARREP_LEN_LW 10

#include <pif_memops.h>
#include <pif_pkt.h>
#include <pif_headers.h>
#include <pif_parse_errors.h>
#include <pif_parrep.h>
#include <pif_action.h>
#include <pif_lookup.h>
#include <pif_deparse.h>
#include <pif_meter.h>
#include <pif_flcalc_algorithms.h>
#include <pif_debug.h>

#include <pif_pkt_duplication.h>

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
