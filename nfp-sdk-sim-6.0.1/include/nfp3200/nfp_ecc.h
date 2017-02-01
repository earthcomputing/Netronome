/*
 * Copyright (C) 2009-2011, Netronome Systems, Inc.  All rights reserved.
 *
 * Disclaimer: this file is provided without any warranty
 * as part of an early-access program, and the content is
 * bound to change before the final release.
 */

#ifndef NFP3200_NFP_ECC_H
#define NFP3200_NFP_ECC_H

/* HGID: nfp3200/ecc.desc = ade0020e9035 */
#define NFP_ECC_CTL(_x)                (0x0 + (0x40 * (_x)))
/* Register Type: ECCEnable */
#define NFP_ECC_CTL_ENABLE             0x0000
#define   NFP_ECC_CTL_ENABLE_ALL                        (0x1)
/* Register Type: ECCInjectClear */
#define NFP_ECC_CTL_INJECT_CLEAR       0x0004
/* Register Type: ECCInjectError */
#define NFP_ECC_CTL_INJECT_ECC_PERM    0x0008
#define NFP_ECC_CTL_INJECT_DATA_PERM   0x000c
#define NFP_ECC_CTL_INJECT_ECC_ONCE    0x0010
#define NFP_ECC_CTL_INJECT_DATA_ONCE   0x0014
#define   NFP_ECC_CTL_INJECT_ADDR_MATCH(_x)             (((_x) & 0xff) << 8)
#define   NFP_ECC_CTL_INJECT_DELTA(_x)                  ((_x) & 0xff)
/* Register Type: ECCError */
#define NFP_ECC_CTL_ERR_FIRST          0x0020
#define NFP_ECC_CTL_ERR_LAST           0x0024
#define   NFP_ECC_CTL_ERR_VALID                         (0x1 << 31)
#define   NFP_ECC_CTL_ERR_MULTIBIT                      (0x1 << 25)
#define   NFP_ECC_CTL_ERR_SINGLEBIT                     (0x1 << 24)
#define   NFP_ECC_CTL_ERR_DELTA_of(_x)                  (((_x) >> 16) & 0xff)
#define   NFP_ECC_CTL_ERR_ADDR_of(_x)                   ((_x) & 0xffff)
/* Register Type: ECCClearErrors */
#define NFP_ECC_CTL_CLEAR_ERR          0x0020
/* Register Type: ECCErrorCounts */
#define NFP_ECC_CTL_ERR_COUNT          0x0028
#define NFP_ECC_CTL_ERR_COUNT_RESET    0x002c
#define   NFP_ECC_CTL_ERR_COUNT_MULTIS_of(_x)           (((_x) >> 16) & 0xffff)
#define   NFP_ECC_CTL_ERR_COUNT_SINGLES_of(_x)          ((_x) & 0xffff)
/* Register Type: ECCSRAMTiming */
#define NFP_ECC_CTL_SRAM_TIMING        0x0030
#define   NFP_ECC_CTL_SRAM_TIMING_PARC(_x)              (((_x) & 0x7) << 8)
#define   NFP_ECC_CTL_SRAM_TIMING_PARC_of(_x)           (((_x) >> 8) & 0x7)
#define   NFP_ECC_CTL_SRAM_TIMING_PARB(_x)              (((_x) & 0x7) << 4)
#define   NFP_ECC_CTL_SRAM_TIMING_PARB_of(_x)           (((_x) >> 4) & 0x7)
#define   NFP_ECC_CTL_SRAM_TIMING_PARA(_x)              ((_x) & 0x7)
#define   NFP_ECC_CTL_SRAM_TIMING_PARA_of(_x)           ((_x) & 0x7)
/* Register Type: ECCLongAddress */
#define NFP_ECC_CTL_LONG_ADDR          0x0034

#endif /* NFP3200_NFP_ECC_H */
