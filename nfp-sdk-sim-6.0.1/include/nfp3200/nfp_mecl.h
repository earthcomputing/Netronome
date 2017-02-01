/*
 * Copyright (C) 2009-2011, Netronome Systems, Inc.  All rights reserved.
 *
 * Disclaimer: this file is provided without any warranty
 * as part of an early-access program, and the content is
 * bound to change before the final release.
 */

#ifndef NFP3200_NFP_MECL_H
#define NFP3200_NFP_MECL_H

/* HGID: nfp3200/lscratch.desc = ade0020e9035 */
#define NFP_MECL_SRAM_BASE             0x0000
#define NFP_MECL_SRAM_LEN              0x10000
/* Register Type: ClsRingBase */
#define NFP_MECL_RING_BASE(_r)         (0x10000 + (0x8 * ((_r) & 0xf)))
#define   NFP_MECL_RING_BASE_FULL                       (0x1 << 31)
#define   NFP_MECL_RING_BASE_NOT_EMPTY                  (0x1 << 30)
#define   NFP_MECL_RING_BASE_REPORT(_x)                 (((_x) & 0xf) << 24)
#define   NFP_MECL_RING_BASE_REPORT_of(_x)              (((_x) >> 24) & 0xf)
#define     NFP_MECL_RING_BASE_REPORT_UNDERFLOW         (1)
#define     NFP_MECL_RING_BASE_REPORT_OVERFLOW          (2)
#define     NFP_MECL_RING_BASE_REPORT_NOT_EMPTY         (4)
#define     NFP_MECL_RING_BASE_REPORT_NOT_FULL          (8)
#define   NFP_MECL_RING_BASE_SIZE(_x)                   (((_x) & 0x7) << 16)
#define   NFP_MECL_RING_BASE_SIZE_of(_x)                (((_x) >> 16) & 0x7)
#define     NFP_MECL_RING_BASE_SIZE_32                  (0)
#define     NFP_MECL_RING_BASE_SIZE_64                  (1)
#define     NFP_MECL_RING_BASE_SIZE_128                 (2)
#define     NFP_MECL_RING_BASE_SIZE_256                 (3)
#define     NFP_MECL_RING_BASE_SIZE_512                 (4)
#define     NFP_MECL_RING_BASE_SIZE_1024                (5)
#define   NFP_MECL_RING_BASE_BASE(_x)                   ((_x) & 0x1ff)
#define   NFP_MECL_RING_BASE_BASE_of(_x)                ((_x) & 0x1ff)
/* Register Type: ClsRingPointers */
#define NFP_MECL_RING_POINTER(_r)      (0x10080 + (0x8 * ((_r) & 0xf)))
#define   NFP_MECL_RING_POINTER_TAIL(_x)                (((_x) & 0x7ff) << 16)
#define   NFP_MECL_RING_POINTER_TAIL_of(_x)             (((_x) >> 16) & 0x7ff)
#define   NFP_MECL_RING_POINTER_HEAD(_x)                ((_x) & 0x7ff)
#define   NFP_MECL_RING_POINTER_HEAD_of(_x)             ((_x) & 0x7ff)
#define NFP_MECL_EM                    0x20000
/* Register Type: ClsAutoFilterStatusMonitor */
#define NFP_MECL_AUTOPUSH_STATUS(_m)   (0x30000 + (0x8 * ((_m) & 0xf)))
#define   NFP_MECL_AUTOPUSH_STATUS_LAST_STATUS          (0x1 << 17)
#define   NFP_MECL_AUTOPUSH_STATUS_EDGE_DETECTED        (0x1 << 16)
#define   NFP_MECL_AUTOPUSH_STATUS_MONITOR(_x)          (((_x) & 0x3) << 8)
#define   NFP_MECL_AUTOPUSH_STATUS_MONITOR_of(_x)       (((_x) >> 8) & 0x3)
#define     NFP_MECL_AUTOPUSH_STATUS_MONITOR_OFF        (0)
#define     NFP_MECL_AUTOPUSH_STATUS_MONITOR_STATUS     (1)
#define     NFP_MECL_AUTOPUSH_STATUS_MONITOR_ONE_SHOT_STATUS (2)
#define     NFP_MECL_AUTOPUSH_STATUS_MONITOR_ONE_SHOT_ACK (3)
#define   NFP_MECL_AUTOPUSH_STATUS_AUTOPUSH(_x)         ((_x) & 0x7)
#define   NFP_MECL_AUTOPUSH_STATUS_AUTOPUSH_of(_x)      ((_x) & 0x7)
/* Register Type: ClsAutoSignal */
#define NFP_MECL_AUTOPUSH_SIGNAL(_s)   (0x30200 + (0x8 * ((_s) & 0x7)))
#define   NFP_MECL_AUTOPUSH_SIGNAL_MASTER(_x)           (((_x) & 0xf) << 24)
#define   NFP_MECL_AUTOPUSH_SIGNAL_MASTER_of(_x)        (((_x) >> 24) & 0xf)
#define   NFP_MECL_AUTOPUSH_SIGNAL_SIGNAL_REF(_x)       (((_x) & 0x7f) << 16)
#define   NFP_MECL_AUTOPUSH_SIGNAL_SIGNAL_REF_of(_x)    (((_x) >> 16) & 0x7f)
#define   NFP_MECL_AUTOPUSH_SIGNAL_DATA_REF(_x)         ((_x) & 0x3fff)
#define   NFP_MECL_AUTOPUSH_SIGNAL_DATA_REF_of(_x)      ((_x) & 0x3fff)
/* Register Type: ClsAutoEventStatus */
#define NFP_MECL_AUTOPUSH_USER_EVENT_STATUS 0x30400
#define   NFP_MECL_AUTOPUSH_USER_EVENT_STATUS_OVERFLOW  (0x1 << 20)
#define   NFP_MECL_AUTOPUSH_USER_EVENT_STATUS_HWM_of(_x) (((_x) >> 16) & 0xf)
#define   NFP_MECL_AUTOPUSH_USER_EVENT_STATUS_READ_of(_x) (((_x) >> 8) & 0xf)
#define   NFP_MECL_AUTOPUSH_USER_EVENT_STATUS_WRITE_of(_x) ((_x) & 0xf)
/* Register Type: ClsAutoEvent */
#define NFP_MECL_AUTOPUSH_USER_EVENT   0x30400
#define   NFP_MECL_AUTOPUSH_USER_EVENT_SOURCE(_x)       (((_x) & 0xfff) << 4)
#define   NFP_MECL_AUTOPUSH_USER_EVENT_TYPE(_x)         ((_x) & 0xf)
/* Register Type: ClsHashMultiply */
#define NFP_MECL_HASH_MULT             0x40000
#define   NFP_MECL_HASH_MULT_MULT_63                    (0x1 << 3)
#define   NFP_MECL_HASH_MULT_MULT_53                    (0x1 << 2)
#define   NFP_MECL_HASH_MULT_MULT_36                    (0x1 << 1)
#define   NFP_MECL_HASH_MULT_MULT_4                     (0x1)
/* Register Type: ClsHashIndexLow */
#define NFP_MECL_HASH_INDEX_LO(_h)     (0x40010 + (0x100 * ((_h) & 0x7)))
#define   NFP_MECL_HASH_INDEX_LO_INDEX_LO_of(_x)        (_x)
/* Register Type: ClsHashIndexHigh */
#define NFP_MECL_HASH_INDEX_HI(_h)     (0x40018 + (0x100 * ((_h) & 0x7)))
#define   NFP_MECL_HASH_INDEX_HI_INDEX_HI_of(_x)        (_x)
#define NFP_MECL_IM                    0x50000
/* Register Type: ClsTrngAccessRequest */
#define NFP_MECL_TRNG_REQUEST          0x60000
#define   NFP_MECL_TRNG_REQUEST_CLEAR_ERR               (0x1 << 31)
#define   NFP_MECL_TRNG_REQUEST_ACCESS(_x)              (((_x) & 0xf) << 8)
#define     NFP_MECL_TRNG_REQUEST_ACCESS_READ           (0)
#define     NFP_MECL_TRNG_REQUEST_ACCESS_WRITE          (1)
#define     NFP_MECL_TRNG_REQUEST_ACCESS_FETCH          (2)
#define     NFP_MECL_TRNG_REQUEST_ACCESS_POLL           (3)
#define   NFP_MECL_TRNG_REQUEST_ADDR(_x)                (((_x) & 0x7) << 4)
/* Register Type: ClsTrngWriteData */
#define NFP_MECL_TRNG_REQUEST_DATA     0x60008
/* Register Type: ClsTrngData */
#define NFP_MECL_TRNG_DATA0            0x60020
#define NFP_MECL_TRNG_DATA1            0x60028
#define NFP_MECL_TRNG_DATA2            0x60030
#define NFP_MECL_TRNG_DATA3            0x60038

/* ME Cluster XPB targets */
#define NFP_XPB_MECL_CLS_CFG(cl)        NFP_XPB_DEST((cl), 1)
#define NFP_XPB_MECL_CLS_IM(cl)	        NFP_XPB_DEST((cl), 2)
#define NFP_XPB_MECL_CLS_ECCMON(cl)     NFP_XPB_DEST((cl), 3)

/* Interrupt Manager pin numbers */
#define NFP_MECL_IM_MEATTN(me)		(16 + (me))
#define NFP_MECL_IM_TRNG		3
#define NFP_MECL_IM_TRNG_HAS_DATA	2
#define NFP_MECL_IM_TRNG_CMD_DONE	1
#define NFP_MECL_IM_TRNG_CMD_ERROR	0

#endif /* NFP3200_NFP_MECL_H */
