/* Copyright (C) 2015,  Netronome Systems, Inc.  All rights reserved. */

#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

#include <pif_design.h>

#define PIF_NO_NFD

#define PIF_APP_NUM_NBI_CTX     8
#define PIF_APP_NUM_PCIE_CTX    0

/*
 * General
 */

#define PKT_NBI_OFFSET 128
#define EN_MAC_PREPEND 1
#define PKT_MAC_PREPEND_BYTES 8
#define BLM_CUSTOM_CONFIG
#define BLM_BLQ_EMEM_TYPE emem

#define BACKOFF_SLEEP           256

/*
 * DCFL
 */

#define DCFL_SUPPORT_MATCH_FILTERING
#define DCFL_CUSTOM_TABLE_LW 4

/*
 * Flowcache config
 */

#ifndef NUM_IMU
#define FC_LB_IMU 1
#else
#define FC_LB_IMU NUM_IMU
#endif

#ifndef NUM_EMU
#define FC_LB_EMU 2
#else
#define FC_LB_EMU NUM_EMU
#endif

#define FC_ENABLE_OVERFLOW

#ifdef PIF_GLOBAL_GRO_DISABLED
#ifdef PIF_USE_GRO
#undef PIF_USE_GRO
#endif
#endif

/*
 * GRO configuration
 */
#define GRO_NUM_BLOCKS           1
#define GRO_CTX_PER_BLOCK        8
#define GRO_REDUCED              0
#define GRO_ISL                  48
/* Configure which Traffic Manager sequencer GRO uses for NBI egress. */
#define GRO0_NBI_SEQR            1
#define GRO_NBI0                 0
#define GRO_CTX_NFD_BASE         5

/*
 * NFD configuration
 */
#define PCIE_ISL                0
#define PCI                     PCIE_ISL
#define NIC_PCI                 PCIE_ISL

#endif /* __APP_CONFIG_H__ */
