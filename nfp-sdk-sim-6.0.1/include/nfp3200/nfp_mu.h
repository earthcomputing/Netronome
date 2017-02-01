/*
 * Copyright (C) 2008-2011, Netronome Systems, Inc.  All rights reserved.
 *
 * Disclaimer: this file is provided without any warranty
 * as part of an early-access program, and the content is
 * bound to change before the final release.
 */

#ifndef NFP3200_NFP_MU_H
#define NFP3200_NFP_MU_H

/* HGID: nfp3200/mu.desc = ade0020e9035 */
/* Register Type: MUConfigCPP */
#define NFP_MU_CFG                     0x0000
#define   NFP_MU_CFG_WAYS(_x)                           (((_x) & 0xff) << 8)
#define   NFP_MU_CFG_WAYS_of(_x)                        (((_x) >> 8) & 0xff)
#define   NFP_MU_CFG_TWO_SIGNALS                        (0x1 << 3)
#define     NFP_MU_CFG_TWO_SIGNALS_ENABLE               (0x8)
#define   NFP_MU_CFG_ADDRESSING(_x)                     (((_x) & 0x3) << 1)
#define   NFP_MU_CFG_ADDRESSING_of(_x)                  (((_x) >> 1) & 0x3)
#define     NFP_MU_CFG_ADDRESSING_40BIT                 (0)
#define     NFP_MU_CFG_ADDRESSING_32BIT                 (1)
#define     NFP_MU_CFG_ADDRESSING_32BITHLR              (2)
#define     NFP_MU_CFG_ADDRESSING_32BITLLR              (3)
#define   NFP_MU_CFG_BULK_ALIGN                         (0x1)
#define     NFP_MU_CFG_BULK_ALIGN_ENFORCE               (0x0)
#define     NFP_MU_CFG_BULK_ALIGN_IGNORE                (0x1)
/* Register Type: MUConfigVQDR */
#define NFP_MU_VQDR_0                  0x0004
#define NFP_MU_VQDR_1                  0x0008
#define NFP_MU_VQDR_2                  0x000c
#define NFP_MU_VQDR_3                  0x0010
#define   NFP_MU_VQDR_QUEUE_TYPE(_x)                    (((_x) & 0x3) << 20)
#define   NFP_MU_VQDR_QUEUE_TYPE_of(_x)                 (((_x) >> 20) & 0x3)
#define     NFP_MU_VQDR_QUEUE_TYPE_SEGMENTS             (0)
#define     NFP_MU_VQDR_QUEUE_TYPE_PACKETS              (1)
#define     NFP_MU_VQDR_QUEUE_TYPE_PACKETS32            (2)
#define     NFP_MU_VQDR_QUEUE_TYPE_BUFFERS              (3)
#define   NFP_MU_VQDR_ENABLE                            (0x1 << 16)
#define     NFP_MU_VQDR_ENABLE_SET                      (0x10000)
#define   NFP_MU_VQDR_BASE(_x)                          ((_x) & 0xffff)
#define   NFP_MU_VQDR_BASE_of(_x)                       ((_x) & 0xffff)
/* Register Type: MUConfigDrams */
#define NFP_MU_DATA_MOVER              0x0014
#define   NFP_MU_DATA_MOVER_CH1_PANIC_FNP               (0x1 << 27)
#define     NFP_MU_DATA_MOVER_CH1_PANIC_FNP_SET         (0x8000000)
#define   NFP_MU_DATA_MOVER_CH1_PANIC_FULL              (0x1 << 26)
#define     NFP_MU_DATA_MOVER_CH1_PANIC_FULL_SET        (0x4000000)
#define   NFP_MU_DATA_MOVER_CH0_PANIC_FNP               (0x1 << 25)
#define     NFP_MU_DATA_MOVER_CH0_PANIC_FNP_SET         (0x2000000)
#define   NFP_MU_DATA_MOVER_CH0_PANIC_FULL              (0x1 << 24)
#define     NFP_MU_DATA_MOVER_CH0_PANIC_FULL_SET        (0x1000000)
#define   NFP_MU_DATA_MOVER_PACE_MODE(_x)               (((_x) & 0x3) << 20)
#define   NFP_MU_DATA_MOVER_PACE_MODE_of(_x)            (((_x) >> 20) & 0x3)
#define     NFP_MU_DATA_MOVER_PACE_MODE_OFF             (0)
#define     NFP_MU_DATA_MOVER_PACE_MODE_4               (1)
#define     NFP_MU_DATA_MOVER_PACE_MODE_5               (2)
#define     NFP_MU_DATA_MOVER_PACE_MODE_6               (3)
#define   NFP_MU_DATA_MOVER_DCACHE_DEPTH(_x)            (((_x) & 0x7) << 16)
#define   NFP_MU_DATA_MOVER_DCACHE_DEPTH_of(_x)         (((_x) >> 16) & 0x7)
#define     NFP_MU_DATA_MOVER_DCACHE_DEPTH_16           (0)
#define     NFP_MU_DATA_MOVER_DCACHE_DEPTH_14           (1)
#define     NFP_MU_DATA_MOVER_DCACHE_DEPTH_12           (2)
#define     NFP_MU_DATA_MOVER_DCACHE_DEPTH_10           (3)
#define     NFP_MU_DATA_MOVER_DCACHE_DEPTH_8            (4)
#define     NFP_MU_DATA_MOVER_DCACHE_DEPTH_6            (5)
#define     NFP_MU_DATA_MOVER_DCACHE_DEPTH_4            (6)
#define     NFP_MU_DATA_MOVER_DCACHE_DEPTH_2            (7)
#define   NFP_MU_DATA_MOVER_DIMMS                       (0x1 << 13)
#define     NFP_MU_DATA_MOVER_DIMMS_1                   (0x0)
#define     NFP_MU_DATA_MOVER_DIMMS_2                   (0x2000)
#define   NFP_MU_DATA_MOVER_RANKS(_x)                   (((_x) & 0x3) << 11)
#define   NFP_MU_DATA_MOVER_RANKS_of(_x)                (((_x) >> 11) & 0x3)
#define     NFP_MU_DATA_MOVER_RANKS_1                   (0)
#define     NFP_MU_DATA_MOVER_RANKS_2                   (1)
#define     NFP_MU_DATA_MOVER_RANKS_4                   (2)
#define   NFP_MU_DATA_MOVER_ROW_BITS(_x)                (((_x) & 0x7) << 8)
#define   NFP_MU_DATA_MOVER_ROW_BITS_of(_x)             (((_x) >> 8) & 0x7)
#define     NFP_MU_DATA_MOVER_ROW_BITS_12               (0)
#define     NFP_MU_DATA_MOVER_ROW_BITS_13               (1)
#define     NFP_MU_DATA_MOVER_ROW_BITS_14               (2)
#define     NFP_MU_DATA_MOVER_ROW_BITS_15               (3)
#define     NFP_MU_DATA_MOVER_ROW_BITS_16               (4)
#define   NFP_MU_DATA_MOVER_TRC_TIMER(_x)               (((_x) & 0x7) << 5)
#define   NFP_MU_DATA_MOVER_TRC_TIMER_of(_x)            (((_x) >> 5) & 0x7)
#define     NFP_MU_DATA_MOVER_TRC_TIMER_40              (0)
#define     NFP_MU_DATA_MOVER_TRC_TIMER_39              (1)
#define     NFP_MU_DATA_MOVER_TRC_TIMER_38              (2)
#define     NFP_MU_DATA_MOVER_TRC_TIMER_37              (3)
#define     NFP_MU_DATA_MOVER_TRC_TIMER_36              (4)
#define     NFP_MU_DATA_MOVER_TRC_TIMER_35              (5)
#define     NFP_MU_DATA_MOVER_TRC_TIMER_34              (6)
#define     NFP_MU_DATA_MOVER_TRC_TIMER_33              (7)
#define   NFP_MU_DATA_MOVER_SNAPSHOT_TIMER(_x)          ((_x) & 0x1f)
#define   NFP_MU_DATA_MOVER_SNAPSHOT_TIMER_of(_x)       ((_x) & 0x1f)
/* Register Type: MUConfigPerformanceBus */
#define NFP_MU_PERF_CFG                0x0018
#define   NFP_MU_PERF_CFG_RFU(_x)                       (((_x) & 0x3) << 30)
#define   NFP_MU_PERF_CFG_RFU_of(_x)                    (((_x) >> 30) & 0x3)
#define   NFP_MU_PERF_CFG_MOD_5_SEL(_x)                 (((_x) & 0xf) << 26)
#define   NFP_MU_PERF_CFG_MOD_5_SEL_of(_x)              (((_x) >> 26) & 0xf)
#define   NFP_MU_PERF_CFG_MOD_4_SEL(_x)                 (((_x) & 0xf) << 22)
#define   NFP_MU_PERF_CFG_MOD_4_SEL_of(_x)              (((_x) >> 22) & 0xf)
#define   NFP_MU_PERF_CFG_MOD_3_SEL(_x)                 (((_x) & 0xf) << 18)
#define   NFP_MU_PERF_CFG_MOD_3_SEL_of(_x)              (((_x) >> 18) & 0xf)
#define   NFP_MU_PERF_CFG_MOD_2_SEL(_x)                 (((_x) & 0xf) << 14)
#define   NFP_MU_PERF_CFG_MOD_2_SEL_of(_x)              (((_x) >> 14) & 0xf)
#define   NFP_MU_PERF_CFG_MOD_1_SEL(_x)                 (((_x) & 0xf) << 10)
#define   NFP_MU_PERF_CFG_MOD_1_SEL_of(_x)              (((_x) >> 10) & 0xf)
#define   NFP_MU_PERF_CFG_MOD_0_SEL(_x)                 (((_x) & 0xf) << 6)
#define   NFP_MU_PERF_CFG_MOD_0_SEL_of(_x)              (((_x) >> 6) & 0xf)
#define   NFP_MU_PERF_CFG_LANE_SEL_HI(_x)               (((_x) & 0x3) << 4)
#define   NFP_MU_PERF_CFG_LANE_SEL_HI_of(_x)            (((_x) >> 4) & 0x3)
#define     NFP_MU_PERF_CFG_LANE_SEL_HI_ZERO            (0)
#define     NFP_MU_PERF_CFG_LANE_SEL_HI_LOW             (1)
#define     NFP_MU_PERF_CFG_LANE_SEL_HI_MID             (2)
#define     NFP_MU_PERF_CFG_LANE_SEL_HI_HIGH            (3)
#define   NFP_MU_PERF_CFG_LANE_SEL_MID(_x)              (((_x) & 0x3) << 2)
#define   NFP_MU_PERF_CFG_LANE_SEL_MID_of(_x)           (((_x) >> 2) & 0x3)
#define     NFP_MU_PERF_CFG_LANE_SEL_MID_ZERO           (0)
#define     NFP_MU_PERF_CFG_LANE_SEL_MID_LOW            (1)
#define     NFP_MU_PERF_CFG_LANE_SEL_MID_MID            (2)
#define     NFP_MU_PERF_CFG_LANE_SEL_MID_HIGH           (3)
#define   NFP_MU_PERF_CFG_LANE_SEL_LO(_x)               ((_x) & 0x3)
#define   NFP_MU_PERF_CFG_LANE_SEL_LO_of(_x)            ((_x) & 0x3)
#define     NFP_MU_PERF_CFG_LANE_SEL_LO_ZERO            (0)
#define     NFP_MU_PERF_CFG_LANE_SEL_LO_LOW             (1)
#define     NFP_MU_PERF_CFG_LANE_SEL_LO_MID             (2)
#define     NFP_MU_PERF_CFG_LANE_SEL_LO_HIGH            (3)
/* Register Type: MUConfigPerformanceTweaks */
#define NFP_MU_TCACHE_CFG              0x001c
#define   NFP_MU_TCACHE_CFG_CPP_LB_INV                  (0x1 << 23)
#define   NFP_MU_TCACHE_CFG_CPP_LB_MASK(_x)             (((_x) & 0x7f) << 16)
#define   NFP_MU_TCACHE_CFG_CPP_LB_MASK_of(_x)          (((_x) >> 16) & 0x7f)
#define   NFP_MU_TCACHE_CFG_LEVEL_SETS                  (0x1 << 2)
#define     NFP_MU_TCACHE_CFG_LEVEL_SETS_SET            (0x4)
#define   NFP_MU_TCACHE_CFG_CHANNEL_LEVEL               (0x1 << 1)
#define     NFP_MU_TCACHE_CFG_CHANNEL_LEVEL_SET         (0x2)
#define   NFP_MU_TCACHE_CFG_BANK_LEVEL                  (0x1)
#define     NFP_MU_TCACHE_CFG_BANK_LEVEL_SET            (0x1)
/* Register Type: MUConfigRealQDR */
#define NFP_MU_REAL_QDR_CFG            0x0020
#define   NFP_MU_REAL_QDR_CFG_ENABLE1                   (0x1 << 15)
#define     NFP_MU_REAL_QDR_CFG_ENABLE1_SET             (0x8000)
#define   NFP_MU_REAL_QDR_CFG_ID1(_x)                   (((_x) & 0x3) << 8)
#define   NFP_MU_REAL_QDR_CFG_ID1_of(_x)                (((_x) >> 8) & 0x3)
#define   NFP_MU_REAL_QDR_CFG_ENABLE0                   (0x1 << 7)
#define     NFP_MU_REAL_QDR_CFG_ENABLE0_SET             (0x80)
#define   NFP_MU_REAL_QDR_CFG_ID0(_x)                   ((_x) & 0x3)
#define   NFP_MU_REAL_QDR_CFG_ID0_of(_x)                ((_x) & 0x3)
/* Register Type: MUDcacheTestPortDisable */
#define NFP_MU_DCACHE_TEST             0x0024
#define   NFP_MU_DCACHE_TEST_CFG_TP_DISABLE             (0x1)
#define     NFP_MU_DCACHE_TEST_CFG_TP_DISABLE_SET       (0x1)
/* Register Type: MUMemoryBISTTrigger */
#define NFP_MU_BIST_TRIGGER            0x0028
#define   NFP_MU_BIST_TRIGGER_CSR_BISTRUN(_x)           ((_x) & 0xff)
#define   NFP_MU_BIST_TRIGGER_CSR_BISTRUN_of(_x)        ((_x) & 0xff)
/* Register Type: MUMemoryBISTStatus */
#define NFP_MU_BIST_STATUS             0x002c
#define   NFP_MU_BIST_STATUS_CSR_BISTFAIL_of(_x)        (((_x) >> 16) & 0xff)
#define   NFP_MU_BIST_STATUS_CSR_BISTDONE_of(_x)        ((_x) & 0xff)
/* HGID: nfp3200/mu_pctl.desc = ade0020e9035 */
/* Register Type: MUPCtlSCfg */
#define NFP_MU_PCTL_SCFG               0x0000
#define   NFP_MU_PCTL_SCFG_RKINF_EN                     (0x1 << 5)
#define   NFP_MU_PCTL_SCFG_DUALPCTLEN                   (0x1 << 4)
#define   NFP_MU_PCTL_SCFG_MODE                         (0x1 << 3)
#define     NFP_MU_PCTL_SCFG_MODE_MASTER                (0x0)
#define     NFP_MU_PCTL_SCFG_MODE_SLAVE                 (0x8)
#define   NFP_MU_PCTL_SCFG_LOOPBACK_EN                  (0x1 << 1)
#define   NFP_MU_PCTL_SCFG_HW_LOW_POWER_EN              (0x1)
/* Register Type: MUPCtlSCtl */
#define NFP_MU_PCTL_SCTL               0x0004
#define   NFP_MU_PCTL_SCTL_STATE(_x)                    ((_x) & 0x7)
#define   NFP_MU_PCTL_SCTL_STATE_of(_x)                 ((_x) & 0x7)
#define     NFP_MU_PCTL_SCTL_STATE_INIT                 (0)
#define     NFP_MU_PCTL_SCTL_STATE_CFG                  (1)
#define     NFP_MU_PCTL_SCTL_STATE_GO                   (2)
#define     NFP_MU_PCTL_SCTL_STATE_SLEEP                (3)
#define     NFP_MU_PCTL_SCTL_STATE_WAKEUP               (4)
/* Register Type: MUPCtlStat */
#define NFP_MU_PCTL_STAT               0x0008
#define   NFP_MU_PCTL_STAT_CTL_of(_x)                   ((_x) & 0x7)
#define     NFP_MU_PCTL_STAT_CTL_INIT_MEM               (0)
#define     NFP_MU_PCTL_STAT_CTL_CONFIG                 (1)
#define     NFP_MU_PCTL_STAT_CTL_CONFIG_REQ             (2)
#define     NFP_MU_PCTL_STAT_CTL_ACCESS                 (3)
#define     NFP_MU_PCTL_STAT_CTL_ACCESS_REQ             (4)
#define     NFP_MU_PCTL_STAT_CTL_LOW_POWER              (5)
#define     NFP_MU_PCTL_STAT_CTL_LOW_POWER_ENTRY_REQ    (6)
#define     NFP_MU_PCTL_STAT_CTL_LOW_POWER_EXIT_REQ     (7)
/* Register Type: MUPCtlMCmd */
#define NFP_MU_PCTL_MCMD               0x0040
#define   NFP_MU_PCTL_MCMD_START                        (0x1 << 31)
#define   NFP_MU_PCTL_MCMD_DELAY(_x)                    (((_x) & 0xf) << 24)
#define   NFP_MU_PCTL_MCMD_DELAY_of(_x)                 (((_x) >> 24) & 0xf)
#define   NFP_MU_PCTL_MCMD_RANKS(_x)                    (((_x) & 0xf) << 20)
#define   NFP_MU_PCTL_MCMD_RANKS_of(_x)                 (((_x) >> 20) & 0xf)
#define     NFP_MU_PCTL_MCMD_RANKS_RANK_0               (1)
#define     NFP_MU_PCTL_MCMD_RANKS_RANK_1               (2)
#define     NFP_MU_PCTL_MCMD_RANKS_RANK_2               (4)
#define     NFP_MU_PCTL_MCMD_RANKS_RANK_3               (8)
#define     NFP_MU_PCTL_MCMD_RANKS_ALL                  (15)
#define   NFP_MU_PCTL_MCMD_DDR3(_x)                     (((_x) & 0x7) << 17)
#define   NFP_MU_PCTL_MCMD_DDR3_of(_x)                  (((_x) >> 17) & 0x7)
#define     NFP_MU_PCTL_MCMD_DDR3_MR0                   (0)
#define     NFP_MU_PCTL_MCMD_DDR3_MR1                   (1)
#define     NFP_MU_PCTL_MCMD_DDR3_MR2                   (2)
#define     NFP_MU_PCTL_MCMD_DDR3_MR3                   (3)
#define   NFP_MU_PCTL_MCMD_ADDR(_x)                     (((_x) & 0x1fff) << 4)
#define   NFP_MU_PCTL_MCMD_ADDR_of(_x)                  (((_x) >> 4) & 0x1fff)
#define   NFP_MU_PCTL_MCMD_CMD(_x)                      ((_x) & 0x7)
#define   NFP_MU_PCTL_MCMD_CMD_of(_x)                   ((_x) & 0x7)
#define     NFP_MU_PCTL_MCMD_CMD_NOP                    (0)
#define     NFP_MU_PCTL_MCMD_CMD_PREA                   (1)
#define     NFP_MU_PCTL_MCMD_CMD_REF                    (2)
#define     NFP_MU_PCTL_MCMD_CMD_MRS                    (3)
#define     NFP_MU_PCTL_MCMD_CMD_ZQCS                   (4)
#define     NFP_MU_PCTL_MCMD_CMD_ZQCL                   (5)
#define     NFP_MU_PCTL_MCMD_CMD_RSTL                   (6)
#define     NFP_MU_PCTL_MCMD_CMD_CKEL                   (7)
/* Register Type: MUPCtlPowCtl */
#define NFP_MU_PCTL_POWCTL             0x0044
#define   NFP_MU_PCTL_POWCTL_START                      (0x1)
/* Register Type: MUPCtlPowStat */
#define NFP_MU_PCTL_POWSTAT            0x0048
#define   NFP_MU_PCTL_POWSTAT_PHY_DONE                  (0x1 << 1)
#define   NFP_MU_PCTL_POWSTAT_POWER_DONE                (0x1)
#define     NFP_MU_PCTL_POWSTAT_POWER_DONE_NO           (0x0)
#define     NFP_MU_PCTL_POWSTAT_POWER_DONE_YES          (0x1)
/* Register Type: MUPCtlMCfg */
#define NFP_MU_PCTL_MCFG               0x0080
#define   NFP_MU_PCTL_MCFG_TFAW(_x)                     (((_x) & 0x3) << 18)
#define   NFP_MU_PCTL_MCFG_TFAW_of(_x)                  (((_x) >> 18) & 0x3)
#define     NFP_MU_PCTL_MCFG_TFAW_4                     (0)
#define     NFP_MU_PCTL_MCFG_TFAW_5                     (1)
#define     NFP_MU_PCTL_MCFG_TFAW_6                     (2)
#define   NFP_MU_PCTL_MCFG_PD_MODE                      (0x1 << 17)
#define     NFP_MU_PCTL_MCFG_PD_MODE_SLOW               (0x0)
#define     NFP_MU_PCTL_MCFG_PD_MODE_FAST               (0x20000)
#define   NFP_MU_PCTL_MCFG_PD_TYPE                      (0x1 << 16)
#define     NFP_MU_PCTL_MCFG_PD_TYPE_PRECHARGE          (0x0)
#define     NFP_MU_PCTL_MCFG_PD_TYPE_ACTIVE             (0x10000)
#define   NFP_MU_PCTL_MCFG_PD_IDLE(_x)                  (((_x) & 0xff) << 8)
#define   NFP_MU_PCTL_MCFG_PD_IDLE_of(_x)               (((_x) >> 8) & 0xff)
#define   NFP_MU_PCTL_MCFG_PAGE_POLICY(_x)              (((_x) & 0x3) << 6)
#define   NFP_MU_PCTL_MCFG_PAGE_POLICY_of(_x)           (((_x) >> 6) & 0x3)
#define     NFP_MU_PCTL_MCFG_PAGE_POLICY_OPEN           (0)
#define     NFP_MU_PCTL_MCFG_PAGE_POLICY_CLOSE          (1)
#define   NFP_MU_PCTL_MCFG_DDR3_EN                      (0x1 << 5)
#define   NFP_MU_PCTL_MCFG_TWO_T_EN                     (0x1 << 3)
#define   NFP_MU_PCTL_MCFG_MEM_BL                       (0x1)
#define     NFP_MU_PCTL_MCFG_MEM_BL_BL4                 (0x0)
#define     NFP_MU_PCTL_MCFG_MEM_BL_BL8                 (0x1)
/* Register Type: MUPCtlPPCfg */
#define NFP_MU_PCTL_PPCFG              0x0084
#define   NFP_MU_PCTL_PPCFG_RPMEM_DIS(_x)               (((_x) & 0xff) << 1)
#define   NFP_MU_PCTL_PPCFG_RPMEM_DIS_of(_x)            (((_x) >> 1) & 0xff)
#define   NFP_MU_PCTL_PPCFG_PPMEM_EN                    (0x1)
/* Register Type: MUPCtlMStat */
#define NFP_MU_PCTL_MSTAT              0x0088
#define   NFP_MU_PCTL_MSTAT_POWER_DOWN                  (0x1)
/* Register Type: MUPCtlODTCfg */
#define NFP_MU_PCTL_ODTCFG             0x008c
#define   NFP_MU_PCTL_ODTCFG_RANK3_ODT_DEFAULT          (0x1 << 28)
#define   NFP_MU_PCTL_ODTCFG_RANK3_ODT_WRITE_SEL        (0x1 << 27)
#define   NFP_MU_PCTL_ODTCFG_RANK3_ODT_WRITE_NSEL       (0x1 << 26)
#define   NFP_MU_PCTL_ODTCFG_RANK3_ODT_READ_SEL         (0x1 << 25)
#define   NFP_MU_PCTL_ODTCFG_RANK3_ODT_READ_NSEL        (0x1 << 24)
#define   NFP_MU_PCTL_ODTCFG_RANK2_ODT_DEFAULT          (0x1 << 20)
#define   NFP_MU_PCTL_ODTCFG_RANK2_ODT_WRITE_SEL        (0x1 << 19)
#define   NFP_MU_PCTL_ODTCFG_RANK2_ODT_WRITE_NSEL       (0x1 << 18)
#define   NFP_MU_PCTL_ODTCFG_RANK2_ODT_READ_SEL         (0x1 << 17)
#define   NFP_MU_PCTL_ODTCFG_RANK2_ODT_READ_NSEL        (0x1 << 16)
#define   NFP_MU_PCTL_ODTCFG_RANK1_ODT_DEFAULT          (0x1 << 12)
#define   NFP_MU_PCTL_ODTCFG_RANK1_ODT_WRITE_SEL        (0x1 << 11)
#define   NFP_MU_PCTL_ODTCFG_RANK1_ODT_WRITE_NSEL       (0x1 << 10)
#define   NFP_MU_PCTL_ODTCFG_RANK1_ODT_READ_SEL         (0x1 << 9)
#define   NFP_MU_PCTL_ODTCFG_RANK1_ODT_READ_NSEL        (0x1 << 8)
#define   NFP_MU_PCTL_ODTCFG_RANK0_ODT_DEFAULT          (0x1 << 4)
#define   NFP_MU_PCTL_ODTCFG_RANK0_ODT_WRITE_SEL        (0x1 << 3)
#define   NFP_MU_PCTL_ODTCFG_RANK0_ODT_WRITE_NSEL       (0x1 << 2)
#define   NFP_MU_PCTL_ODTCFG_RANK0_ODT_READ_SEL         (0x1 << 1)
#define   NFP_MU_PCTL_ODTCFG_RANK0_ODT_READ_NSEL        (0x1)
/* Register Type: MUPCtlDQSECfg */
#define NFP_MU_PCTL_DQSECFG            0x0090
#define   NFP_MU_PCTL_DQSECFG_DV_ALAT(_x)               (((_x) & 0xf) << 20)
#define   NFP_MU_PCTL_DQSECFG_DV_ALAT_of(_x)            (((_x) >> 20) & 0xf)
#define   NFP_MU_PCTL_DQSECFG_DV_ALEN(_x)               (((_x) & 0x3) << 16)
#define   NFP_MU_PCTL_DQSECFG_DV_ALEN_of(_x)            (((_x) >> 16) & 0x3)
#define   NFP_MU_PCTL_DQSECFG_DSE_ALAT(_x)              (((_x) & 0xf) << 12)
#define   NFP_MU_PCTL_DQSECFG_DSE_ALAT_of(_x)           (((_x) >> 12) & 0xf)
#define   NFP_MU_PCTL_DQSECFG_DSE_ALEN(_x)              (((_x) & 0x3) << 8)
#define   NFP_MU_PCTL_DQSECFG_DSE_ALEN_of(_x)           (((_x) >> 8) & 0x3)
#define   NFP_MU_PCTL_DQSECFG_QSE_ALAT(_x)              (((_x) & 0xf) << 4)
#define   NFP_MU_PCTL_DQSECFG_QSE_ALAT_of(_x)           (((_x) >> 4) & 0xf)
#define   NFP_MU_PCTL_DQSECFG_QSE_ALEN(_x)              ((_x) & 0x3)
#define   NFP_MU_PCTL_DQSECFG_QSE_ALEN_of(_x)           ((_x) & 0x3)
/* Register Type: MUPCtlDtuStat */
#define NFP_MU_PCTL_DTUPDES            0x0094
#define   NFP_MU_PCTL_DTUPDES_DTU_RD_MISSING            (0x1 << 13)
#define   NFP_MU_PCTL_DTUPDES_DTU_EAFFL_of(_x)          (((_x) >> 9) & 0xf)
#define   NFP_MU_PCTL_DTUPDES_DTU_RANDOM_ERROR          (0x1 << 8)
#define   NFP_MU_PCTL_DTUPDES_DTU_ERR_B7                (0x1 << 7)
#define   NFP_MU_PCTL_DTUPDES_DTU_ERR_B6                (0x1 << 6)
#define   NFP_MU_PCTL_DTUPDES_DTU_ERR_B5                (0x1 << 5)
#define   NFP_MU_PCTL_DTUPDES_DTU_ERR_B4                (0x1 << 4)
#define   NFP_MU_PCTL_DTUPDES_DTU_ERR_B3                (0x1 << 3)
#define   NFP_MU_PCTL_DTUPDES_DTU_ERR_B2                (0x1 << 2)
#define   NFP_MU_PCTL_DTUPDES_DTU_ERR_B1                (0x1 << 1)
#define   NFP_MU_PCTL_DTUPDES_DTU_ERR_B0                (0x1)
/* Register Type: MUPCtlDtuNA */
#define NFP_MU_PCTL_DTUNA              0x0098
#define   NFP_MU_PCTL_DTUNA_DTU_NUM_ADDRESS_of(_x)      (_x)
/* Register Type: MUPCtlDtuNE */
#define NFP_MU_PCTL_DTUNE              0x009c
#define   NFP_MU_PCTL_DTUNE_DTU_NUM_ERRORS_of(_x)       (_x)
/* Register Type: MUPCtlDtuPRd */
#define NFP_MU_PCTL_DTUPRD0            0x00a0
#define NFP_MU_PCTL_DTUPRD1            0x00a4
#define NFP_MU_PCTL_DTUPRD2            0x00a8
#define NFP_MU_PCTL_DTUPRD3            0x00ac
#define   NFP_MU_PCTL_DTUPRD_DTU_ALLBITS_1_of(_x)       (((_x) >> 16) & 0xffff)
#define   NFP_MU_PCTL_DTUPRD_DTU_ALLBITS_0_of(_x)       ((_x) & 0xffff)
/* Register Type: MUPCtlDtuAWdt */
#define NFP_MU_PCTL_DTUAWDT            0x00b0
#define   NFP_MU_PCTL_DTUAWDT_NUMBER_RANKS(_x)          (((_x) & 0x3) << 9)
#define   NFP_MU_PCTL_DTUAWDT_NUMBER_RANKS_of(_x)       (((_x) >> 9) & 0x3)
#define     NFP_MU_PCTL_DTUAWDT_NUMBER_RANKS_1          (0)
#define     NFP_MU_PCTL_DTUAWDT_NUMBER_RANKS_2          (1)
#define     NFP_MU_PCTL_DTUAWDT_NUMBER_RANKS_3          (2)
#define     NFP_MU_PCTL_DTUAWDT_NUMBER_RANKS_4          (3)
#define   NFP_MU_PCTL_DTUAWDT_ROW_ADDR_WIDTH(_x)        (((_x) & 0x3) << 6)
#define   NFP_MU_PCTL_DTUAWDT_ROW_ADDR_WIDTH_of(_x)     (((_x) >> 6) & 0x3)
#define     NFP_MU_PCTL_DTUAWDT_ROW_ADDR_WIDTH_13_BIT   (0)
#define     NFP_MU_PCTL_DTUAWDT_ROW_ADDR_WIDTH_14_BIT   (1)
#define     NFP_MU_PCTL_DTUAWDT_ROW_ADDR_WIDTH_15_BIT   (2)
#define     NFP_MU_PCTL_DTUAWDT_ROW_ADDR_WIDTH_16_BIT   (3)
#define   NFP_MU_PCTL_DTUAWDT_BANK_ADDR_WIDTH(_x)       (((_x) & 0x3) << 3)
#define   NFP_MU_PCTL_DTUAWDT_BANK_ADDR_WIDTH_of(_x)    (((_x) >> 3) & 0x3)
#define     NFP_MU_PCTL_DTUAWDT_BANK_ADDR_WIDTH_2_BIT   (0)
#define     NFP_MU_PCTL_DTUAWDT_BANK_ADDR_WIDTH_3_BIT   (1)
#define   NFP_MU_PCTL_DTUAWDT_COLUMN_ADDR_WIDTH(_x)     ((_x) & 0x3)
#define   NFP_MU_PCTL_DTUAWDT_COLUMN_ADDR_WIDTH_of(_x)  ((_x) & 0x3)
#define     NFP_MU_PCTL_DTUAWDT_COLUMN_ADDR_WIDTH_9_BIT (0)
#define     NFP_MU_PCTL_DTUAWDT_COLUMN_ADDR_WIDTH_10_BIT (1)
/* Register Type: MUPCtlTogCnt1U */
#define NFP_MU_PCTL_TOGCNT1U           0x00c0
#define   NFP_MU_PCTL_TOGCNT1U_CYCLES(_x)               ((_x) & 0x3ff)
#define   NFP_MU_PCTL_TOGCNT1U_CYCLES_of(_x)            ((_x) & 0x3ff)
/* Register Type: MUPCtlTInit */
#define NFP_MU_PCTL_TINIT              0x00c4
#define   NFP_MU_PCTL_TINIT_TIMER(_x)                   ((_x) & 0x1ff)
#define   NFP_MU_PCTL_TINIT_TIMER_of(_x)                ((_x) & 0x1ff)
/* Register Type: MUPCtlTRstH */
#define NFP_MU_PCTL_TRSTH              0x00c8
#define   NFP_MU_PCTL_TRSTH_TIMER(_x)                   ((_x) & 0x3ff)
#define   NFP_MU_PCTL_TRSTH_TIMER_of(_x)                ((_x) & 0x3ff)
/* Register Type: MUPCtlTogCnt100N */
#define NFP_MU_PCTL_TOGCNT100N         0x00cc
#define   NFP_MU_PCTL_TOGCNT100N_CYCLES(_x)             ((_x) & 0x7f)
#define   NFP_MU_PCTL_TOGCNT100N_CYCLES_of(_x)          ((_x) & 0x7f)
/* Register Type: MUPCtlTRefI */
#define NFP_MU_PCTL_TREFI              0x00d0
#define   NFP_MU_PCTL_TREFI_DATA(_x)                    ((_x) & 0xff)
#define   NFP_MU_PCTL_TREFI_DATA_of(_x)                 ((_x) & 0xff)
/* Register Type: MUPCtlTMRD */
#define NFP_MU_PCTL_TMRD               0x00d4
#define   NFP_MU_PCTL_TMRD_DATA(_x)                     ((_x) & 0x7)
#define   NFP_MU_PCTL_TMRD_DATA_of(_x)                  ((_x) & 0x7)
/* Register Type: MUPCtlTRFC */
#define NFP_MU_PCTL_TRFC               0x00d8
#define   NFP_MU_PCTL_TRFC_DATA(_x)                     ((_x) & 0x1ff)
#define   NFP_MU_PCTL_TRFC_DATA_of(_x)                  ((_x) & 0x1ff)
/* Register Type: MUPCtlTRP */
#define NFP_MU_PCTL_TRP                0x00dc
#define   NFP_MU_PCTL_TRP_DATA(_x)                      ((_x) & 0xf)
#define   NFP_MU_PCTL_TRP_DATA_of(_x)                   ((_x) & 0xf)
/* Register Type: MUPCtlTRTW */
#define NFP_MU_PCTL_TRTW               0x00e0
#define   NFP_MU_PCTL_TRTW_DATA(_x)                     ((_x) & 0x7)
#define   NFP_MU_PCTL_TRTW_DATA_of(_x)                  ((_x) & 0x7)
/* Register Type: MUPCtlTAL */
#define NFP_MU_PCTL_TAL                0x00e4
#define   NFP_MU_PCTL_TAL_DATA(_x)                      ((_x) & 0xf)
#define   NFP_MU_PCTL_TAL_DATA_of(_x)                   ((_x) & 0xf)
/* Register Type: MUPCtlTCL */
#define NFP_MU_PCTL_TCL                0x00e8
#define   NFP_MU_PCTL_TCL_DATA(_x)                      ((_x) & 0xf)
#define   NFP_MU_PCTL_TCL_DATA_of(_x)                   ((_x) & 0xf)
/* Register Type: MUPCtlTCWL */
#define NFP_MU_PCTL_TCWL               0x00ec
#define   NFP_MU_PCTL_TCWL_DATA(_x)                     ((_x) & 0xf)
#define   NFP_MU_PCTL_TCWL_DATA_of(_x)                  ((_x) & 0xf)
/* Register Type: MUPCtlTRAS */
#define NFP_MU_PCTL_TRAS               0x00f0
#define   NFP_MU_PCTL_TRAS_DATA(_x)                     ((_x) & 0x1f)
#define   NFP_MU_PCTL_TRAS_DATA_of(_x)                  ((_x) & 0x1f)
/* Register Type: MUPCtlTRC */
#define NFP_MU_PCTL_TRC                0x00f4
#define   NFP_MU_PCTL_TRC_DATA(_x)                      ((_x) & 0x3f)
#define   NFP_MU_PCTL_TRC_DATA_of(_x)                   ((_x) & 0x3f)
/* Register Type: MUPCtlTRCD */
#define NFP_MU_PCTL_TRCD               0x00f8
#define   NFP_MU_PCTL_TRCD_DATA(_x)                     ((_x) & 0xf)
#define   NFP_MU_PCTL_TRCD_DATA_of(_x)                  ((_x) & 0xf)
/* Register Type: MUPCtlTRRD */
#define NFP_MU_PCTL_TRRD               0x00fc
#define   NFP_MU_PCTL_TRRD_DATA(_x)                     ((_x) & 0x7)
#define   NFP_MU_PCTL_TRRD_DATA_of(_x)                  ((_x) & 0x7)
/* Register Type: MUPCtlTRTP */
#define NFP_MU_PCTL_TRTP               0x0100
#define   NFP_MU_PCTL_TRTP_DATA(_x)                     ((_x) & 0xf)
#define   NFP_MU_PCTL_TRTP_DATA_of(_x)                  ((_x) & 0xf)
/* Register Type: MUPCtlTWR */
#define NFP_MU_PCTL_TWR                0x0104
#define   NFP_MU_PCTL_TWR_DATA(_x)                      ((_x) & 0xf)
#define   NFP_MU_PCTL_TWR_DATA_of(_x)                   ((_x) & 0xf)
/* Register Type: MUPCtlTWTR */
#define NFP_MU_PCTL_TWTR               0x0108
#define   NFP_MU_PCTL_TWTR_DATA(_x)                     ((_x) & 0x7)
#define   NFP_MU_PCTL_TWTR_DATA_of(_x)                  ((_x) & 0x7)
/* Register Type: MUPCtlTEXSR */
#define NFP_MU_PCTL_TEXSR              0x010c
#define   NFP_MU_PCTL_TEXSR_DATA(_x)                    ((_x) & 0x3ff)
#define   NFP_MU_PCTL_TEXSR_DATA_of(_x)                 ((_x) & 0x3ff)
/* Register Type: MUPCtlTXP */
#define NFP_MU_PCTL_TXP                0x0110
#define   NFP_MU_PCTL_TXP_DATA(_x)                      ((_x) & 0x7)
#define   NFP_MU_PCTL_TXP_DATA_of(_x)                   ((_x) & 0x7)
/* Register Type: MUPCtlTXPDLL */
#define NFP_MU_PCTL_TXPDLL             0x0114
#define   NFP_MU_PCTL_TXPDLL_DATA(_x)                   ((_x) & 0x3f)
#define   NFP_MU_PCTL_TXPDLL_DATA_of(_x)                ((_x) & 0x3f)
/* Register Type: MUPCtlTZQCS */
#define NFP_MU_PCTL_TZQCS              0x0118
#define   NFP_MU_PCTL_TZQCS_DATA(_x)                    ((_x) & 0x7f)
#define   NFP_MU_PCTL_TZQCS_DATA_of(_x)                 ((_x) & 0x7f)
/* Register Type: MUPCtlTZQCSI */
#define NFP_MU_PCTL_TZQCSI             0x011c
#define   NFP_MU_PCTL_TZQCSI_DATA(_x)                   (_x)
#define   NFP_MU_PCTL_TZQCSI_DATA_of(_x)                (_x)
/* Register Type: MUPCtlTDQS */
#define NFP_MU_PCTL_TDQS               0x0120
#define   NFP_MU_PCTL_TDQS_DATA(_x)                     ((_x) & 0x7)
#define   NFP_MU_PCTL_TDQS_DATA_of(_x)                  ((_x) & 0x7)
/* Register Type: MUPCtlTCKSRE */
#define NFP_MU_PCTL_TCKSRE             0x0124
#define   NFP_MU_PCTL_TCKSRE_DATA(_x)                   ((_x) & 0xf)
#define   NFP_MU_PCTL_TCKSRE_DATA_of(_x)                ((_x) & 0xf)
/* Register Type: MUPCtlTCKSRX */
#define NFP_MU_PCTL_TCKSRX             0x0128
#define   NFP_MU_PCTL_TCKSRX_DATA(_x)                   ((_x) & 0xf)
#define   NFP_MU_PCTL_TCKSRX_DATA_of(_x)                ((_x) & 0xf)
/* Register Type: MUPCtlTCKE */
#define NFP_MU_PCTL_TCKE               0x012c
#define   NFP_MU_PCTL_TCKE_DATA(_x)                     ((_x) & 0x7)
#define   NFP_MU_PCTL_TCKE_DATA_of(_x)                  ((_x) & 0x7)
/* Register Type: MUPCtlTMOD */
#define NFP_MU_PCTL_TMOD               0x0130
#define   NFP_MU_PCTL_TMOD_DATA(_x)                     ((_x) & 0xf)
#define   NFP_MU_PCTL_TMOD_DATA_of(_x)                  ((_x) & 0xf)
/* Register Type: MUPCtlTRSTL */
#define NFP_MU_PCTL_TRSTL              0x0134
#define   NFP_MU_PCTL_TRSTL_DATA(_x)                    ((_x) & 0x7f)
#define   NFP_MU_PCTL_TRSTL_DATA_of(_x)                 ((_x) & 0x7f)
/* Register Type: MUPCtlTZQCL */
#define NFP_MU_PCTL_TZQCL              0x0138
#define   NFP_MU_PCTL_TZQCL_DATA(_x)                    ((_x) & 0x3ff)
#define   NFP_MU_PCTL_TZQCL_DATA_of(_x)                 ((_x) & 0x3ff)
/* Register Type: MUPCtlDWLCFG0 */
#define NFP_MU_PCTL_DWLCFG0            0x0170
#define   NFP_MU_PCTL_DWLCFG0_T_ADWL_VEC(_x)            ((_x) & 0x1ff)
#define   NFP_MU_PCTL_DWLCFG0_T_ADWL_VEC_of(_x)         ((_x) & 0x1ff)
/* Register Type: MUPCtlDWLCFG1 */
#define NFP_MU_PCTL_DWLCFG1            0x0174
#define   NFP_MU_PCTL_DWLCFG1_T_ADWL_VEC(_x)            ((_x) & 0x1ff)
#define   NFP_MU_PCTL_DWLCFG1_T_ADWL_VEC_of(_x)         ((_x) & 0x1ff)
/* Register Type: MUPCtlDWLCFG2 */
#define NFP_MU_PCTL_DWLCFG2            0x0178
#define   NFP_MU_PCTL_DWLCFG2_T_ADWL_VEC(_x)            ((_x) & 0x1ff)
#define   NFP_MU_PCTL_DWLCFG2_T_ADWL_VEC_of(_x)         ((_x) & 0x1ff)
/* Register Type: MUPCtlDWLCFG3 */
#define NFP_MU_PCTL_DWLCFG3            0x017c
#define   NFP_MU_PCTL_DWLCFG3_T_ADWL_VEC(_x)            ((_x) & 0x1ff)
#define   NFP_MU_PCTL_DWLCFG3_T_ADWL_VEC_of(_x)         ((_x) & 0x1ff)
/* Register Type: MUPCtlDtuWACtl */
#define NFP_MU_PCTL_DTUWACTL           0x0200
#define   NFP_MU_PCTL_DTUWACTL_DTU_WR_RANK(_x)          (((_x) & 0x3) << 30)
#define   NFP_MU_PCTL_DTUWACTL_DTU_WR_RANK_of(_x)       (((_x) >> 30) & 0x3)
#define   NFP_MU_PCTL_DTUWACTL_DTU_WR_ROW(_x)           (((_x) & 0x1ffff) << 13)
#define   NFP_MU_PCTL_DTUWACTL_DTU_WR_ROW_of(_x)        (((_x) >> 13) & 0x1ffff)
#define   NFP_MU_PCTL_DTUWACTL_DTU_WR_BANK(_x)          (((_x) & 0x7) << 10)
#define   NFP_MU_PCTL_DTUWACTL_DTU_WR_BANK_of(_x)       (((_x) >> 10) & 0x7)
#define   NFP_MU_PCTL_DTUWACTL_DTU_WR_COL(_x)           ((_x) & 0x3ff)
#define   NFP_MU_PCTL_DTUWACTL_DTU_WR_COL_of(_x)        ((_x) & 0x3ff)
/* Register Type: MUPCtlDtuRACtl */
#define NFP_MU_PCTL_DTURACTL           0x0204
#define   NFP_MU_PCTL_DTURACTL_DTU_RD_RANK(_x)          (((_x) & 0x3) << 30)
#define   NFP_MU_PCTL_DTURACTL_DTU_RD_RANK_of(_x)       (((_x) >> 30) & 0x3)
#define   NFP_MU_PCTL_DTURACTL_DTU_RD_ROW(_x)           (((_x) & 0x1ffff) << 13)
#define   NFP_MU_PCTL_DTURACTL_DTU_RD_ROW_of(_x)        (((_x) >> 13) & 0x1ffff)
#define   NFP_MU_PCTL_DTURACTL_DTU_RD_BANK(_x)          (((_x) & 0x7) << 10)
#define   NFP_MU_PCTL_DTURACTL_DTU_RD_BANK_of(_x)       (((_x) >> 10) & 0x7)
#define   NFP_MU_PCTL_DTURACTL_DTU_RD_COL(_x)           ((_x) & 0x3ff)
#define   NFP_MU_PCTL_DTURACTL_DTU_RD_COL_of(_x)        ((_x) & 0x3ff)
/* Register Type: MUPCtlDtuCfg */
#define NFP_MU_PCTL_DTUCFG             0x0208
#define   NFP_MU_PCTL_DTUCFG_DTU_ROW_INCREMENTS(_x)     (((_x) & 0x7f) << 16)
#define   NFP_MU_PCTL_DTUCFG_DTU_ROW_INCREMENTS_of(_x)  (((_x) >> 16) & 0x7f)
#define   NFP_MU_PCTL_DTUCFG_DTU_WR_MULTI_RD            (0x1 << 15)
#define   NFP_MU_PCTL_DTUCFG_DTU_DATA_MASK_EN           (0x1 << 14)
#define   NFP_MU_PCTL_DTUCFG_DTU_TARGET_LANE(_x)        (((_x) & 0xf) << 10)
#define   NFP_MU_PCTL_DTUCFG_DTU_TARGET_LANE_of(_x)     (((_x) >> 10) & 0xf)
#define   NFP_MU_PCTL_DTUCFG_DTU_GENERATE_RANDOM        (0x1 << 9)
#define   NFP_MU_PCTL_DTUCFG_DTU_INCR_BANKS             (0x1 << 8)
#define   NFP_MU_PCTL_DTUCFG_DTU_INCR_COLS              (0x1 << 7)
#define   NFP_MU_PCTL_DTUCFG_DTU_NALEN(_x)              (((_x) & 0x3f) << 1)
#define   NFP_MU_PCTL_DTUCFG_DTU_NALEN_of(_x)           (((_x) >> 1) & 0x3f)
#define   NFP_MU_PCTL_DTUCFG_DTU_ENABLE                 (0x1)
#define     NFP_MU_PCTL_DTUCFG_DTU_ENABLE_EN            (0x0)
#define     NFP_MU_PCTL_DTUCFG_DTU_ENABLE_DIS           (0x1)
/* Register Type: MUPCtlDtuECtl */
#define NFP_MU_PCTL_DTUECTL            0x020c
#define   NFP_MU_PCTL_DTUECTL_WR_MULTI_RD_RST           (0x1 << 2)
#define   NFP_MU_PCTL_DTUECTL_RUN_ERROR_REPORTS         (0x1 << 1)
#define   NFP_MU_PCTL_DTUECTL_RUN_DTU                   (0x1)
/* Register Type: MUPCtlDtuWD */
#define NFP_MU_PCTL_DTUWD0             0x0210
#define NFP_MU_PCTL_DTUWD1             0x0214
#define NFP_MU_PCTL_DTUWD2             0x0218
#define NFP_MU_PCTL_DTUWD3             0x021c
#define   NFP_MU_PCTL_DTUWD_DTU_WR_BYTE3(_x)            (((_x) & 0xff) << 24)
#define   NFP_MU_PCTL_DTUWD_DTU_WR_BYTE3_of(_x)         (((_x) >> 24) & 0xff)
#define   NFP_MU_PCTL_DTUWD_DTU_WR_BYTE2(_x)            (((_x) & 0xff) << 16)
#define   NFP_MU_PCTL_DTUWD_DTU_WR_BYTE2_of(_x)         (((_x) >> 16) & 0xff)
#define   NFP_MU_PCTL_DTUWD_DTU_WR_BYTE1(_x)            (((_x) & 0xff) << 8)
#define   NFP_MU_PCTL_DTUWD_DTU_WR_BYTE1_of(_x)         (((_x) >> 8) & 0xff)
#define   NFP_MU_PCTL_DTUWD_DTU_WR_BYTE0(_x)            ((_x) & 0xff)
#define   NFP_MU_PCTL_DTUWD_DTU_WR_BYTE0_of(_x)         ((_x) & 0xff)
/* Register Type: MUPCtlDtuWDM */
#define NFP_MU_PCTL_DTUWDM             0x0220
#define   NFP_MU_PCTL_DTUWDM_DM_WR_BYTE0(_x)            ((_x) & 0xffff)
#define   NFP_MU_PCTL_DTUWDM_DM_WR_BYTE0_of(_x)         ((_x) & 0xffff)
/* Register Type: MUPCtlDtuRD */
#define NFP_MU_PCTL_DTURD0             0x0224
#define NFP_MU_PCTL_DTURD1             0x0228
#define NFP_MU_PCTL_DTURD2             0x022c
#define NFP_MU_PCTL_DTURD3             0x0230
#define   NFP_MU_PCTL_DTURD_DTU_RD_BYTE3_of(_x)         (((_x) >> 24) & 0xff)
#define   NFP_MU_PCTL_DTURD_DTU_RD_BYTE2_of(_x)         (((_x) >> 16) & 0xff)
#define   NFP_MU_PCTL_DTURD_DTU_RD_BYTE1_of(_x)         (((_x) >> 8) & 0xff)
#define   NFP_MU_PCTL_DTURD_DTU_RD_BYTE0_of(_x)         ((_x) & 0xff)
/* Register Type: MUPCtlDtuLfsrWD */
#define NFP_MU_PCTL_DTULFSRWD          0x0234
#define   NFP_MU_PCTL_DTULFSRWD_DTU_LFSR_WSEED(_x)      (_x)
#define   NFP_MU_PCTL_DTULFSRWD_DTU_LFSR_WSEED_of(_x)   (_x)
/* Register Type: MUPCtlDtuLfsrRD */
#define NFP_MU_PCTL_DTULFSRRD          0x0238
#define   NFP_MU_PCTL_DTULFSRRD_DTU_LFSR_RSEED(_x)      (_x)
#define   NFP_MU_PCTL_DTULFSRRD_DTU_LFSR_RSEED_of(_x)   (_x)
/* Register Type: MUPCtlDtuEAF */
#define NFP_MU_PCTL_DTUEAF             0x023c
#define   NFP_MU_PCTL_DTUEAF_EA_RANK_of(_x)             (((_x) >> 30) & 0x3)
#define   NFP_MU_PCTL_DTUEAF_EA_ROW_of(_x)              (((_x) >> 13) & 0x1ffff)
#define   NFP_MU_PCTL_DTUEAF_EA_BANK_of(_x)             (((_x) >> 10) & 0x7)
#define   NFP_MU_PCTL_DTUEAF_EA_COLUMN_of(_x)           ((_x) & 0x3ff)
/* Register Type: MUPCtlPHYPVTCFG */
#define NFP_MU_PCTL_PHYPVTCFG          0x0300
#define   NFP_MU_PCTL_PHYPVTCFG_PV_UPD_REQ_EN           (0x1 << 15)
#define   NFP_MU_PCTL_PHYPVTCFG_PVT_UPD_TRIG_POL        (0x1 << 14)
#define     NFP_MU_PCTL_PHYPVTCFG_PVT_UPD_TRIG_POL_LOW  (0x0)
#define     NFP_MU_PCTL_PHYPVTCFG_PVT_UPD_TRIG_POL_HIGH (0x4000)
#define   NFP_MU_PCTL_PHYPVTCFG_PVT_UPD_TRIG_TYPE       (0x1 << 12)
#define     NFP_MU_PCTL_PHYPVTCFG_PVT_UPD_TRIG_TYPE_ALWAYS (0x0)
#define     NFP_MU_PCTL_PHYPVTCFG_PVT_UPD_TRIG_TYPE_PBT_UPDATE (0x1000)
#define   NFP_MU_PCTL_PHYPVTCFG_PVT_UPD_DONE_POL        (0x1 << 10)
#define     NFP_MU_PCTL_PHYPVTCFG_PVT_UPD_DONE_POL_LOW  (0x0)
#define     NFP_MU_PCTL_PHYPVTCFG_PVT_UPD_DONE_POL_HIGH (0x400)
#define   NFP_MU_PCTL_PHYPVTCFG_PVT_UPD_DONE_TYPE(_x)   (((_x) & 0x3) << 8)
#define   NFP_MU_PCTL_PHYPVTCFG_PVT_UPD_DONE_TYPE_of(_x) (((_x) >> 8) & 0x3)
#define     NFP_MU_PCTL_PHYPVTCFG_PVT_UPD_DONE_TYPE_FIXME1 (0)
#define     NFP_MU_PCTL_PHYPVTCFG_PVT_UPD_DONE_TYPE_FIXME2 (1)
#define     NFP_MU_PCTL_PHYPVTCFG_PVT_UPD_DONE_TYPE_FIXME3 (2)
#define   NFP_MU_PCTL_PHYPVTCFG_PHY_UPD_REQ_EN          (0x1 << 7)
#define     NFP_MU_PCTL_PHYPVTCFG_PHY_UPD_REQ_EN_DIS    (0x0)
#define     NFP_MU_PCTL_PHYPVTCFG_PHY_UPD_REQ_EN_EN     (0x80)
#define   NFP_MU_PCTL_PHYPVTCFG_PHY_UPD_TRIG_POL        (0x1 << 6)
#define     NFP_MU_PCTL_PHYPVTCFG_PHY_UPD_TRIG_POL_LOW  (0x0)
#define     NFP_MU_PCTL_PHYPVTCFG_PHY_UPD_TRIG_POL_HIGH (0x40)
#define   NFP_MU_PCTL_PHYPVTCFG_PHY_UPD_TRIG_TYPE(_x)   (((_x) & 0x3) << 4)
#define   NFP_MU_PCTL_PHYPVTCFG_PHY_UPD_TRIG_TYPE_of(_x) (((_x) >> 4) & 0x3)
#define     NFP_MU_PCTL_PHYPVTCFG_PHY_UPD_TRIG_TYPE_ALWAYS (0)
#define     NFP_MU_PCTL_PHYPVTCFG_PHY_UPD_TRIG_TYPE_PHY_UPDATE (1)
#define     NFP_MU_PCTL_PHYPVTCFG_PHY_UPD_TRIG_TYPE_PHY_UPDATE_IMM (3)
#define   NFP_MU_PCTL_PHYPVTCFG_PHY_UPD_DONE_POL        (0x1 << 2)
#define     NFP_MU_PCTL_PHYPVTCFG_PHY_UPD_DONE_POL_LOW  (0x0)
#define     NFP_MU_PCTL_PHYPVTCFG_PHY_UPD_DONE_POL_HIGH (0x4)
#define   NFP_MU_PCTL_PHYPVTCFG_PHY_UPD_DONE_TYPE(_x)   ((_x) & 0x3)
#define   NFP_MU_PCTL_PHYPVTCFG_PHY_UPD_DONE_TYPE_of(_x) ((_x) & 0x3)
#define     NFP_MU_PCTL_PHYPVTCFG_PHY_UPD_DONE_TYPE_FIXME1 (0)
#define     NFP_MU_PCTL_PHYPVTCFG_PHY_UPD_DONE_TYPE_FIXME2 (1)
#define     NFP_MU_PCTL_PHYPVTCFG_PHY_UPD_DONE_TYPE_FIXME3 (2)
/* Register Type: MUPCtlPHYPVTSTAT */
#define NFP_MU_PCTL_PHYPVTSTAT         0x0304
#define   NFP_MU_PCTL_PHYPVTSTAT_I_PVT_UPD_TRIG         (0x1 << 5)
#define   NFP_MU_PCTL_PHYPVTSTAT_I_PVT_UPD_DONE         (0x1 << 4)
#define   NFP_MU_PCTL_PHYPVTSTAT_I_PHY_UPD_TRIG         (0x1 << 1)
#define   NFP_MU_PCTL_PHYPVTSTAT_I_PHY_UPD_DONE         (0x1)
/* Register Type: MUPCtlPHYTUPDON */
#define NFP_MU_PCTL_PHYTUPDON          0x0308
#define   NFP_MU_PCTL_PHYTUPDON_PHY_T_UPDON(_x)         ((_x) & 0xff)
#define   NFP_MU_PCTL_PHYTUPDON_PHY_T_UPDON_of(_x)      ((_x) & 0xff)
/* Register Type: MUPCtlPHYTUPDDLY */
#define NFP_MU_PCTL_PHYTUPDDLY         0x030c
#define   NFP_MU_PCTL_PHYTUPDDLY_PHY_T_UPDDLY(_x)       ((_x) & 0xf)
#define   NFP_MU_PCTL_PHYTUPDDLY_PHY_T_UPDDLY_of(_x)    ((_x) & 0xf)
/* Register Type: MUPCtlPVTTUPDON */
#define NFP_MU_PCTL_PVTTUPDON          0x0310
#define   NFP_MU_PCTL_PVTTUPDON_PVT_T_UPDON(_x)         ((_x) & 0xff)
#define   NFP_MU_PCTL_PVTTUPDON_PVT_T_UPDON_of(_x)      ((_x) & 0xff)
/* Register Type: MUPCtlPVTTUPDDLY */
#define NFP_MU_PCTL_PVTTUPDDLY         0x0314
#define   NFP_MU_PCTL_PVTTUPDDLY_PVT_T_UPDDLY(_x)       ((_x) & 0xf)
#define   NFP_MU_PCTL_PVTTUPDDLY_PVT_T_UPDDLY_of(_x)    ((_x) & 0xf)
/* Register Type: MUPCtlPHYPVTUPDI */
#define NFP_MU_PCTL_PHYPVTUPDI         0x0318
#define   NFP_MU_PCTL_PHYPVTUPDI_PHYPVT_T_UPDI(_x)      ((_x) & 0xff)
#define   NFP_MU_PCTL_PHYPVTUPDI_PHYPVT_T_UPDI_of(_x)   ((_x) & 0xff)
/* Register Type: MUPCtlPHYIOCRV1 */
#define NFP_MU_PCTL_PHYIOCRV1          0x031c
#define   NFP_MU_PCTL_PHYIOCRV1_AUTO_CMD_PDD(_x)        (((_x) & 0x3) << 28)
#define   NFP_MU_PCTL_PHYIOCRV1_AUTO_CMD_PDD_of(_x)     (((_x) >> 28) & 0x3)
#define     NFP_MU_PCTL_PHYIOCRV1_AUTO_CMD_PDD_FIXME1   (0)
#define     NFP_MU_PCTL_PHYIOCRV1_AUTO_CMD_PDD_FIXME2   (1)
#define     NFP_MU_PCTL_PHYIOCRV1_AUTO_CMD_PDD_FIXME3   (2)
#define     NFP_MU_PCTL_PHYIOCRV1_AUTO_CMD_PDD_FIXME4   (3)
#define   NFP_MU_PCTL_PHYIOCRV1_AUTO_DATA_PDD(_x)       (((_x) & 0x3) << 26)
#define   NFP_MU_PCTL_PHYIOCRV1_AUTO_DATA_PDD_of(_x)    (((_x) >> 26) & 0x3)
#define     NFP_MU_PCTL_PHYIOCRV1_AUTO_DATA_PDD_FIXME1  (0)
#define     NFP_MU_PCTL_PHYIOCRV1_AUTO_DATA_PDD_FIXME2  (1)
#define     NFP_MU_PCTL_PHYIOCRV1_AUTO_DATA_PDD_FIXME3  (2)
#define     NFP_MU_PCTL_PHYIOCRV1_AUTO_DATA_PDD_FIXME4  (3)
#define   NFP_MU_PCTL_PHYIOCRV1_BYTE_OE_CTL(_x)         (((_x) & 0x3) << 16)
#define   NFP_MU_PCTL_PHYIOCRV1_BYTE_OE_CTL_of(_x)      (((_x) >> 16) & 0x3)
#define     NFP_MU_PCTL_PHYIOCRV1_BYTE_OE_CTL_FIXME1    (0)
#define     NFP_MU_PCTL_PHYIOCRV1_BYTE_OE_CTL_FIXME2    (1)
#define     NFP_MU_PCTL_PHYIOCRV1_BYTE_OE_CTL_FIXME3    (2)
#define     NFP_MU_PCTL_PHYIOCRV1_BYTE_OE_CTL_FIXME4    (3)
#define   NFP_MU_PCTL_PHYIOCRV1_DYN_SOC_ODT_ALAT(_x)    (((_x) & 0xf) << 12)
#define   NFP_MU_PCTL_PHYIOCRV1_DYN_SOC_ODT_ALAT_of(_x) (((_x) >> 12) & 0xf)
#define   NFP_MU_PCTL_PHYIOCRV1_DYN_SOC_ODT_ALEN(_x)    (((_x) & 0x3) << 8)
#define   NFP_MU_PCTL_PHYIOCRV1_DYN_SOC_ODT_ALEN_of(_x) (((_x) >> 8) & 0x3)
#define   NFP_MU_PCTL_PHYIOCRV1_DYN_SOC_ODT             (0x1 << 2)
#define   NFP_MU_PCTL_PHYIOCRV1_SOC_ODT_EN              (0x1)
/* Register Type: MUPCtlPHYTUpdWait */
#define NFP_MU_PCTL_PHYTUPDWAIT        0x0320
#define   NFP_MU_PCTL_PHYTUPDWAIT_PHY_T_UPDWAIT(_x)     ((_x) & 0x3f)
#define   NFP_MU_PCTL_PHYTUPDWAIT_PHY_T_UPDWAIT_of(_x)  ((_x) & 0x3f)
/* Register Type: MUPCtlPVTTUpdWait */
#define NFP_MU_PCTL_PVTTUPDWAIT        0x0324
#define   NFP_MU_PCTL_PVTTUPDWAIT_PVT_T_UPDWAIT(_x)     ((_x) & 0x3f)
#define   NFP_MU_PCTL_PVTTUPDWAIT_PVT_T_UPDWAIT_of(_x)  ((_x) & 0x3f)
/* Register Type: MUPCtlPVTUpdI */
#define NFP_MU_PCTL_MUPCTLPVTUPDI      0x0328
#define   NFP_MU_PCTL_MUPCTLPVTUPDI_PVT_T_UPDI(_x)      (_x)
#define   NFP_MU_PCTL_MUPCTLPVTUPDI_PVT_T_UPDI_of(_x)   (_x)
/* Register Type: MUPCtlIPVR */
#define NFP_MU_PCTL_IPVR               0x03f8
#define   NFP_MU_PCTL_IPVR_IP_VERSION_of(_x)            (_x)
/* Register Type: MUPCtlIPTR */
#define NFP_MU_PCTL_IPTR               0x03fc
#define   NFP_MU_PCTL_IPTR_IP_TYPE_of(_x)               (_x)
/* HGID: nfp3200/mu_phy.desc = ade0020e9035 */
/* Register Type: MUPhyRIDR */
#define NFP_MU_PCTL_PHY_RIDR           0x0400
#define   NFP_MU_PCTL_PHY_RIDR_MJREV_of(_x)             (((_x) >> 8) & 0xff)
#define   NFP_MU_PCTL_PHY_RIDR_MNREV_of(_x)             ((_x) & 0xff)
/* Register Type: MUPhyPIR */
#define NFP_MU_PCTL_PHY_PIR            0x0404
#define   NFP_MU_PCTL_PHY_PIR_INHVT                     (0x1 << 28)
#define   NFP_MU_PCTL_PHY_PIR_ZCALUPD                   (0x1 << 27)
#define   NFP_MU_PCTL_PHY_PIR_ZCAL                      (0x1 << 26)
#define   NFP_MU_PCTL_PHY_PIR_ZCALBYP                   (0x1 << 25)
#define   NFP_MU_PCTL_PHY_PIR_ZCKSEL(_x)                (((_x) & 0x3) << 23)
#define   NFP_MU_PCTL_PHY_PIR_ZCKSEL_of(_x)             (((_x) >> 23) & 0x3)
#define     NFP_MU_PCTL_PHY_PIR_ZCKSEL_DIV_1            (0)
#define     NFP_MU_PCTL_PHY_PIR_ZCKSEL_DIV_8            (1)
#define     NFP_MU_PCTL_PHY_PIR_ZCKSEL_DIV_32           (2)
#define     NFP_MU_PCTL_PHY_PIR_ZCKSEL_DIV_64           (3)
#define   NFP_MU_PCTL_PHY_PIR_DLDLMT(_x)                (((_x) & 0xff) << 15)
#define   NFP_MU_PCTL_PHY_PIR_DLDLMT_of(_x)             (((_x) >> 15) & 0xff)
#define   NFP_MU_PCTL_PHY_PIR_FDEPTH(_x)                (((_x) & 0x3) << 13)
#define   NFP_MU_PCTL_PHY_PIR_FDEPTH_of(_x)             (((_x) >> 13) & 0x3)
#define     NFP_MU_PCTL_PHY_PIR_FDEPTH_2                (0)
#define     NFP_MU_PCTL_PHY_PIR_FDEPTH_4                (1)
#define     NFP_MU_PCTL_PHY_PIR_FDEPTH_8                (2)
#define     NFP_MU_PCTL_PHY_PIR_FDEPTH_16               (3)
#define   NFP_MU_PCTL_PHY_PIR_LPFDEPTH(_x)              (((_x) & 0x3) << 11)
#define   NFP_MU_PCTL_PHY_PIR_LPFDEPTH_of(_x)           (((_x) >> 11) & 0x3)
#define     NFP_MU_PCTL_PHY_PIR_LPFDEPTH_2              (0)
#define     NFP_MU_PCTL_PHY_PIR_LPFDEPTH_4              (1)
#define     NFP_MU_PCTL_PHY_PIR_LPFDEPTH_8              (2)
#define     NFP_MU_PCTL_PHY_PIR_LPFDEPTH_16             (3)
#define   NFP_MU_PCTL_PHY_PIR_LPFEN                     (0x1 << 10)
#define   NFP_MU_PCTL_PHY_PIR_MDLEN                     (0x1 << 9)
#define   NFP_MU_PCTL_PHY_PIR_PLLPD                     (0x1 << 8)
#define   NFP_MU_PCTL_PHY_PIR_PLLRST                    (0x1 << 7)
#define   NFP_MU_PCTL_PHY_PIR_PHYHRST                   (0x1 << 6)
#define   NFP_MU_PCTL_PHY_PIR_PHYRST                    (0x1 << 5)
#define   NFP_MU_PCTL_PHY_PIR_CALBYP                    (0x1 << 3)
#define   NFP_MU_PCTL_PHY_PIR_CAL                       (0x1 << 2)
#define   NFP_MU_PCTL_PHY_PIR_INITBYP                   (0x1 << 1)
#define   NFP_MU_PCTL_PHY_PIR_PHYINIT                   (0x1)
/* Register Type: MUPhyPGCR0 */
#define NFP_MU_PCTL_PHY_PGCR0          0x0408
#define   NFP_MU_PCTL_PHY_PGCR0_CKEN(_x)                (((_x) & 0x3f) << 26)
#define   NFP_MU_PCTL_PHY_PGCR0_CKEN_of(_x)             (((_x) >> 26) & 0x3f)
#define   NFP_MU_PCTL_PHY_PGCR0_LBMODE                  (0x1 << 25)
#define   NFP_MU_PCTL_PHY_PGCR0_LBCKSEL(_x)             (((_x) & 0x3) << 23)
#define   NFP_MU_PCTL_PHY_PGCR0_LBCKSEL_of(_x)          (((_x) >> 23) & 0x3)
#define     NFP_MU_PCTL_PHY_PGCR0_LBCKSEL_CK0           (0)
#define     NFP_MU_PCTL_PHY_PGCR0_LBCKSEL_CK1           (1)
#define     NFP_MU_PCTL_PHY_PGCR0_LBCKSEL_CK2           (2)
#define   NFP_MU_PCTL_PHY_PGCR0_LBSEL(_x)               (((_x) & 0x3) << 21)
#define   NFP_MU_PCTL_PHY_PGCR0_LBSEL_of(_x)            (((_x) >> 21) & 0x3)
#define     NFP_MU_PCTL_PHY_PGCR0_LBSEL_FIXME1          (0)
#define     NFP_MU_PCTL_PHY_PGCR0_LBSEL_FIXME2          (1)
#define     NFP_MU_PCTL_PHY_PGCR0_LBSEL_FIXME3          (2)
#define     NFP_MU_PCTL_PHY_PGCR0_LBSEL_FIXME4          (3)
#define   NFP_MU_PCTL_PHY_PGCR0_IOLB                    (0x1 << 20)
#define     NFP_MU_PCTL_PHY_PGCR0_IOLB_IOLB0            (0x0)
#define     NFP_MU_PCTL_PHY_PGCR0_IOLB_IOLB1            (0x100000)
#define   NFP_MU_PCTL_PHY_PGCR0_PLCKSEL                 (0x1 << 19)
#define     NFP_MU_PCTL_PHY_PGCR0_PLCKSEL_PLCKSEL0      (0x0)
#define     NFP_MU_PCTL_PHY_PGCR0_PLCKSEL_PLCKSEL1      (0x80000)
#define   NFP_MU_PCTL_PHY_PGCR0_DTOSEL(_x)              (((_x) & 0x1f) << 14)
#define   NFP_MU_PCTL_PHY_PGCR0_DTOSEL_of(_x)           (((_x) >> 14) & 0x1f)
#define     NFP_MU_PCTL_PHY_PGCR0_DTOSEL_FIXME1         (0)
#define     NFP_MU_PCTL_PHY_PGCR0_DTOSEL_FIXME2         (1)
#define     NFP_MU_PCTL_PHY_PGCR0_DTOSEL_FIXME3         (2)
#define     NFP_MU_PCTL_PHY_PGCR0_DTOSEL_FIXME4         (3)
#define     NFP_MU_PCTL_PHY_PGCR0_DTOSEL_FIXME5         (4)
#define     NFP_MU_PCTL_PHY_PGCR0_DTOSEL_FIXME6         (5)
#define     NFP_MU_PCTL_PHY_PGCR0_DTOSEL_FIXME7         (6)
#define     NFP_MU_PCTL_PHY_PGCR0_DTOSEL_FIXME8         (7)
#define     NFP_MU_PCTL_PHY_PGCR0_DTOSEL_FIXME9         (8)
#define     NFP_MU_PCTL_PHY_PGCR0_DTOSEL_FIXME10        (9)
#define     NFP_MU_PCTL_PHY_PGCR0_DTOSEL_FIXME11        (10)
#define     NFP_MU_PCTL_PHY_PGCR0_DTOSEL_FIXME12        (11)
#define     NFP_MU_PCTL_PHY_PGCR0_DTOSEL_FIXME13        (12)
#define     NFP_MU_PCTL_PHY_PGCR0_DTOSEL_FIXME14        (13)
#define     NFP_MU_PCTL_PHY_PGCR0_DTOSEL_FIXME15        (14)
#define     NFP_MU_PCTL_PHY_PGCR0_DTOSEL_FIXME16        (15)
#define     NFP_MU_PCTL_PHY_PGCR0_DTOSEL_FIXME17        (16)
#define     NFP_MU_PCTL_PHY_PGCR0_DTOSEL_FIXME18        (17)
#define     NFP_MU_PCTL_PHY_PGCR0_DTOSEL_FIXME19        (18)
#define     NFP_MU_PCTL_PHY_PGCR0_DTOSEL_FIXME20        (19)
#define   NFP_MU_PCTL_PHY_PGCR0_OSCWDL(_x)              (((_x) & 0x3) << 12)
#define   NFP_MU_PCTL_PHY_PGCR0_OSCWDL_of(_x)           (((_x) >> 12) & 0x3)
#define     NFP_MU_PCTL_PHY_PGCR0_OSCWDL_NONE           (0)
#define     NFP_MU_PCTL_PHY_PGCR0_OSCWDL_DDR            (1)
#define     NFP_MU_PCTL_PHY_PGCR0_OSCWDL_SDR            (2)
#define     NFP_MU_PCTL_PHY_PGCR0_OSCWDL_BOTH           (3)
#define   NFP_MU_PCTL_PHY_PGCR0_OSCDIV(_x)              (((_x) & 0x7) << 9)
#define   NFP_MU_PCTL_PHY_PGCR0_OSCDIV_of(_x)           (((_x) >> 9) & 0x7)
#define     NFP_MU_PCTL_PHY_PGCR0_OSCDIV_1              (0)
#define     NFP_MU_PCTL_PHY_PGCR0_OSCDIV_256            (1)
#define     NFP_MU_PCTL_PHY_PGCR0_OSCDIV_512            (2)
#define     NFP_MU_PCTL_PHY_PGCR0_OSCDIV_1024           (3)
#define     NFP_MU_PCTL_PHY_PGCR0_OSCDIV_2048           (4)
#define     NFP_MU_PCTL_PHY_PGCR0_OSCDIV_4096           (5)
#define     NFP_MU_PCTL_PHY_PGCR0_OSCDIV_8192           (6)
#define     NFP_MU_PCTL_PHY_PGCR0_OSCDIV_65536          (7)
#define   NFP_MU_PCTL_PHY_PGCR0_OSCEN                   (0x1 << 8)
#define   NFP_MU_PCTL_PHY_PGCR0_DLTST                   (0x1 << 7)
#define   NFP_MU_PCTL_PHY_PGCR0_DLTMODE                 (0x1 << 6)
#define   NFP_MU_PCTL_PHY_PGCR0_RDBVT                   (0x1 << 5)
#define   NFP_MU_PCTL_PHY_PGCR0_WDBVT                   (0x1 << 4)
#define   NFP_MU_PCTL_PHY_PGCR0_RGLVT                   (0x1 << 3)
#define   NFP_MU_PCTL_PHY_PGCR0_RDLVT                   (0x1 << 2)
#define   NFP_MU_PCTL_PHY_PGCR0_WDLVT                   (0x1 << 1)
#define   NFP_MU_PCTL_PHY_PGCR0_WLLVT                   (0x1)
/* Register Type: MUPhyPGCR1 */
#define NFP_MU_PCTL_PHY_PGCR1          0x040c
#define   NFP_MU_PCTL_PHY_PGCR1_WLSELT                  (0x1 << 6)
#define     NFP_MU_PCTL_PHY_PGCR1_WLSELT_1              (0x0)
#define     NFP_MU_PCTL_PHY_PGCR1_WLSELT_2              (0x40)
#define   NFP_MU_PCTL_PHY_PGCR1_WLRANK(_x)              (((_x) & 0x3) << 4)
#define   NFP_MU_PCTL_PHY_PGCR1_WLRANK_of(_x)           (((_x) >> 4) & 0x3)
#define     NFP_MU_PCTL_PHY_PGCR1_WLRANK_1              (0)
#define     NFP_MU_PCTL_PHY_PGCR1_WLRANK_2              (1)
#define     NFP_MU_PCTL_PHY_PGCR1_WLRANK_3              (2)
#define     NFP_MU_PCTL_PHY_PGCR1_WLRANK_4              (3)
#define   NFP_MU_PCTL_PHY_PGCR1_WLUNCRT                 (0x1 << 3)
#define   NFP_MU_PCTL_PHY_PGCR1_WLSTEP                  (0x1 << 2)
#define     NFP_MU_PCTL_PHY_PGCR1_WLSTEP_32             (0x0)
#define     NFP_MU_PCTL_PHY_PGCR1_WLSTEP_1              (0x4)
#define   NFP_MU_PCTL_PHY_PGCR1_WFULL                   (0x1 << 1)
#define   NFP_MU_PCTL_PHY_PGCR1_WLEN                    (0x1)
/* Register Type: MUPhyPGSR0 */
#define NFP_MU_PCTL_PHY_PGSR0          0x0410
#define   NFP_MU_PCTL_PHY_PGSR0_APLOCK                  (0x1 << 5)
#define   NFP_MU_PCTL_PHY_PGSR0_WLERR                   (0x1 << 4)
#define   NFP_MU_PCTL_PHY_PGSR0_WL                      (0x1 << 3)
#define   NFP_MU_PCTL_PHY_PGSR0_CAL                     (0x1 << 2)
#define   NFP_MU_PCTL_PHY_PGSR0_PLLINIT                 (0x1 << 1)
#define   NFP_MU_PCTL_PHY_PGSR0_INIT                    (0x1)
/* Register Type: MUPhyPGSR1 */
#define NFP_MU_PCTL_PHY_PGSR1          0x0414
#define   NFP_MU_PCTL_PHY_PGSR1_DLTCODE_of(_x)          (((_x) >> 1) & 0xffffff)
#define   NFP_MU_PCTL_PHY_PGSR1_DLTDONE                 (0x1)
/* Register Type: MUPhyPLLCR */
#define NFP_MU_PCTL_PHY_PLLCR          0x0418
#define   NFP_MU_PCTL_PHY_PLLCR_BYP                     (0x1 << 31)
#define   NFP_MU_PCTL_PHY_PLLCR_FRQSEL                  (0x1 << 18)
#define     NFP_MU_PCTL_PHY_PLLCR_FRQSEL_FIXME1         (0x0)
#define     NFP_MU_PCTL_PHY_PLLCR_FRQSEL_FIXME2         (0x40000)
#define   NFP_MU_PCTL_PHY_PLLCR_QPMODE                  (0x1 << 17)
#define   NFP_MU_PCTL_PHY_PLLCR_CPPC(_x)                (((_x) & 0xf) << 13)
#define   NFP_MU_PCTL_PHY_PLLCR_CPPC_of(_x)             (((_x) >> 13) & 0xf)
#define   NFP_MU_PCTL_PHY_PLLCR_CPIC(_x)                (((_x) & 0x3) << 11)
#define   NFP_MU_PCTL_PHY_PLLCR_CPIC_of(_x)             (((_x) >> 11) & 0x3)
#define   NFP_MU_PCTL_PHY_PLLCR_GSHIFT                  (0x1 << 10)
#define   NFP_MU_PCTL_PHY_PLLCR_ATOEN(_x)               (((_x) & 0xf) << 6)
#define   NFP_MU_PCTL_PHY_PLLCR_ATOEN_of(_x)            (((_x) >> 6) & 0xf)
#define     NFP_MU_PCTL_PHY_PLLCR_ATOEN_FIXME1          (0)
#define     NFP_MU_PCTL_PHY_PLLCR_ATOEN_FIXME2          (1)
#define     NFP_MU_PCTL_PHY_PLLCR_ATOEN_FIXME3          (2)
#define     NFP_MU_PCTL_PHY_PLLCR_ATOEN_FIXME4          (3)
#define     NFP_MU_PCTL_PHY_PLLCR_ATOEN_FIXME5          (4)
#define     NFP_MU_PCTL_PHY_PLLCR_ATOEN_FIXME6          (5)
#define     NFP_MU_PCTL_PHY_PLLCR_ATOEN_FIXME7          (6)
#define     NFP_MU_PCTL_PHY_PLLCR_ATOEN_FIXME8          (7)
#define     NFP_MU_PCTL_PHY_PLLCR_ATOEN_FIXME9          (8)
#define     NFP_MU_PCTL_PHY_PLLCR_ATOEN_FIXME10         (9)
#define     NFP_MU_PCTL_PHY_PLLCR_ATOEN_FIXME11         (10)
#define   NFP_MU_PCTL_PHY_PLLCR_ATC(_x)                 (((_x) & 0xf) << 2)
#define   NFP_MU_PCTL_PHY_PLLCR_ATC_of(_x)              (((_x) >> 2) & 0xf)
#define   NFP_MU_PCTL_PHY_PLLCR_DTC(_x)                 ((_x) & 0x3)
#define   NFP_MU_PCTL_PHY_PLLCR_DTC_of(_x)              ((_x) & 0x3)
#define     NFP_MU_PCTL_PHY_PLLCR_DTC_DISABLE           (0)
#define     NFP_MU_PCTL_PHY_PLLCR_DTC_X1                (1)
#define     NFP_MU_PCTL_PHY_PLLCR_DTC_FB_X1             (2)
#define     NFP_MU_PCTL_PHY_PLLCR_DTC_REF_CLK           (3)
/* Register Type: MUPhyPTR0 */
#define NFP_MU_PCTL_PHY_PTR0           0x041c
#define   NFP_MU_PCTL_PHY_PTR0_T_PLLPD(_x)              (((_x) & 0x7ff) << 21)
#define   NFP_MU_PCTL_PHY_PTR0_T_PLLPD_of(_x)           (((_x) >> 21) & 0x7ff)
#define   NFP_MU_PCTL_PHY_PTR0_T_PHYRST(_x)             ((_x) & 0x3f)
#define   NFP_MU_PCTL_PHY_PTR0_T_PHYRST_of(_x)          ((_x) & 0x3f)
/* Register Type: MUPhyPTR1 */
#define NFP_MU_PCTL_PHY_PTR1           0x0420
#define   NFP_MU_PCTL_PHY_PTR1_T_PLLLOCK(_x)            (((_x) & 0xffff) << 16)
#define   NFP_MU_PCTL_PHY_PTR1_T_PLLLOCK_of(_x)         (((_x) >> 16) & 0xffff)
#define   NFP_MU_PCTL_PHY_PTR1_T_PLLRST(_x)             ((_x) & 0x1fff)
#define   NFP_MU_PCTL_PHY_PTR1_T_PLLRST_of(_x)          ((_x) & 0x1fff)
/* Register Type: MUPhyPTR2 */
#define NFP_MU_PCTL_PHY_PTR2           0x0424
#define   NFP_MU_PCTL_PHY_PTR2_T_WLDLYS(_x)             (((_x) & 0x1f) << 16)
#define   NFP_MU_PCTL_PHY_PTR2_T_WLDLYS_of(_x)          (((_x) >> 16) & 0x1f)
#define   NFP_MU_PCTL_PHY_PTR2_T_WLO(_x)                (((_x) & 0xf) << 12)
#define   NFP_MU_PCTL_PHY_PTR2_T_WLO_of(_x)             (((_x) >> 12) & 0xf)
#define   NFP_MU_PCTL_PHY_PTR2_T_CALH(_x)               (((_x) & 0xf) << 8)
#define   NFP_MU_PCTL_PHY_PTR2_T_CALH_of(_x)            (((_x) >> 8) & 0xf)
#define   NFP_MU_PCTL_PHY_PTR2_T_CALS(_x)               (((_x) & 0xf) << 4)
#define   NFP_MU_PCTL_PHY_PTR2_T_CALS_of(_x)            (((_x) >> 4) & 0xf)
#define   NFP_MU_PCTL_PHY_PTR2_T_CALON(_x)              ((_x) & 0xf)
#define   NFP_MU_PCTL_PHY_PTR2_T_CALON_of(_x)           ((_x) & 0xf)
/* Register Type: MUPhyACMDLR */
#define NFP_MU_PCTL_PHY_ACMDLR         0x0428
#define   NFP_MU_PCTL_PHY_ACMDLR_MDLD(_x)               (((_x) & 0xff) << 16)
#define   NFP_MU_PCTL_PHY_ACMDLR_MDLD_of(_x)            (((_x) >> 16) & 0xff)
#define   NFP_MU_PCTL_PHY_ACMDLR_TPRD(_x)               (((_x) & 0xff) << 8)
#define   NFP_MU_PCTL_PHY_ACMDLR_TPRD_of(_x)            (((_x) >> 8) & 0xff)
#define   NFP_MU_PCTL_PHY_ACMDLR_IPRD(_x)               ((_x) & 0xff)
#define   NFP_MU_PCTL_PHY_ACMDLR_IPRD_of(_x)            ((_x) & 0xff)
/* Register Type: MUPhyACBDLR */
#define NFP_MU_PCTL_PHY_ACBDLR         0x042c
#define   NFP_MU_PCTL_PHY_ACBDLR_ACBD(_x)               (((_x) & 0x3f) << 18)
#define   NFP_MU_PCTL_PHY_ACBDLR_ACBD_of(_x)            (((_x) >> 18) & 0x3f)
#define   NFP_MU_PCTL_PHY_ACBDLR_CK2BD(_x)              (((_x) & 0x3f) << 12)
#define   NFP_MU_PCTL_PHY_ACBDLR_CK2BD_of(_x)           (((_x) >> 12) & 0x3f)
#define   NFP_MU_PCTL_PHY_ACBDLR_CK1BD(_x)              (((_x) & 0x3f) << 6)
#define   NFP_MU_PCTL_PHY_ACBDLR_CK1BD_of(_x)           (((_x) >> 6) & 0x3f)
#define   NFP_MU_PCTL_PHY_ACBDLR_CK0BD(_x)              ((_x) & 0x3f)
#define   NFP_MU_PCTL_PHY_ACBDLR_CK0BD_of(_x)           ((_x) & 0x3f)
/* Register Type: MUPhyACIOCR */
#define NFP_MU_PCTL_PHY_ACIOCR         0x0430
#define   NFP_MU_PCTL_PHY_ACIOCR_RSTPDR                 (0x1 << 28)
#define   NFP_MU_PCTL_PHY_ACIOCR_RSTPDD                 (0x1 << 27)
#define   NFP_MU_PCTL_PHY_ACIOCR_RSTODT                 (0x1 << 26)
#define   NFP_MU_PCTL_PHY_ACIOCR_RANKPDR(_x)            (((_x) & 0xf) << 22)
#define   NFP_MU_PCTL_PHY_ACIOCR_RANKPDR_of(_x)         (((_x) >> 22) & 0xf)
#define   NFP_MU_PCTL_PHY_ACIOCR_RANKPDD(_x)            (((_x) & 0xf) << 18)
#define   NFP_MU_PCTL_PHY_ACIOCR_RANKPDD_of(_x)         (((_x) >> 18) & 0xf)
#define   NFP_MU_PCTL_PHY_ACIOCR_RANKODT(_x)            (((_x) & 0xf) << 14)
#define   NFP_MU_PCTL_PHY_ACIOCR_RANKODT_of(_x)         (((_x) >> 14) & 0xf)
#define   NFP_MU_PCTL_PHY_ACIOCR_CKPDR(_x)              (((_x) & 0x7) << 11)
#define   NFP_MU_PCTL_PHY_ACIOCR_CKPDR_of(_x)           (((_x) >> 11) & 0x7)
#define   NFP_MU_PCTL_PHY_ACIOCR_CKPDD(_x)              (((_x) & 0x7) << 8)
#define   NFP_MU_PCTL_PHY_ACIOCR_CKPDD_of(_x)           (((_x) >> 8) & 0x7)
#define   NFP_MU_PCTL_PHY_ACIOCR_CKODT(_x)              (((_x) & 0x7) << 5)
#define   NFP_MU_PCTL_PHY_ACIOCR_CKODT_of(_x)           (((_x) >> 5) & 0x7)
#define   NFP_MU_PCTL_PHY_ACIOCR_ACPDR                  (0x1 << 4)
#define   NFP_MU_PCTL_PHY_ACIOCR_ACPDD                  (0x1 << 3)
#define   NFP_MU_PCTL_PHY_ACIOCR_ACODT                  (0x1 << 2)
#define   NFP_MU_PCTL_PHY_ACIOCR_ACOE                   (0x1 << 1)
#define   NFP_MU_PCTL_PHY_ACIOCR_ACIOM                  (0x1)
/* Register Type: MUPhyDXCCR */
#define NFP_MU_PCTL_PHY_DXCCR          0x0434
#define   NFP_MU_PCTL_PHY_DXCCR_DQSNRES(_x)             (((_x) & 0xf) << 9)
#define   NFP_MU_PCTL_PHY_DXCCR_DQSNRES_of(_x)          (((_x) >> 9) & 0xf)
#define   NFP_MU_PCTL_PHY_DXCCR_DQRES(_x)               (((_x) & 0xf) << 5)
#define   NFP_MU_PCTL_PHY_DXCCR_DQRES_of(_x)            (((_x) >> 5) & 0xf)
#define     NFP_MU_PCTL_PHY_DXCCR_DQRES_OPEN            (0)
#define     NFP_MU_PCTL_PHY_DXCCR_DQRES_688             (1)
#define     NFP_MU_PCTL_PHY_DXCCR_DQRES_611             (2)
#define     NFP_MU_PCTL_PHY_DXCCR_DQRES_550             (3)
#define     NFP_MU_PCTL_PHY_DXCCR_DQRES_500             (4)
#define     NFP_MU_PCTL_PHY_DXCCR_DQRES_458             (5)
#define     NFP_MU_PCTL_PHY_DXCCR_DQRES_393             (6)
#define     NFP_MU_PCTL_PHY_DXCCR_DQRES_344             (7)
#define   NFP_MU_PCTL_PHY_DXCCR_DXPDR                   (0x1 << 4)
#define   NFP_MU_PCTL_PHY_DXCCR_DXPDD                   (0x1 << 3)
#define   NFP_MU_PCTL_PHY_DXCCR_MDLEN                   (0x1 << 2)
#define   NFP_MU_PCTL_PHY_DXCCR_DXIOM                   (0x1 << 1)
#define   NFP_MU_PCTL_PHY_DXCCR_DXODT                   (0x1)
/* Register Type: MUPhyZQCR */
#define NFP_MU_PCTL_PHY_ZQCR0          0x05a0
#define NFP_MU_PCTL_PHY_ZQCR1          0x05a8
#define   NFP_MU_PCTL_PHY_ZQCR_IMP_CAL_TRIG             (0x1 << 30)
#define   NFP_MU_PCTL_PHY_ZQCR_IMP_CAL_BYP              (0x1 << 29)
#define   NFP_MU_PCTL_PHY_ZQCR_IMP_OVERRIDE_EN          (0x1 << 28)
#define   NFP_MU_PCTL_PHY_ZQCR_IMP_DIV(_x)              (((_x) & 0xff) << 20)
#define   NFP_MU_PCTL_PHY_ZQCR_IMP_DIV_of(_x)           (((_x) >> 20) & 0xff)
#define   NFP_MU_PCTL_PHY_ZQCR_IMP_OVERRIDE_DATA(_x)    ((_x) & 0xfffff)
#define   NFP_MU_PCTL_PHY_ZQCR_IMP_OVERRIDE_DATA_of(_x) ((_x) & 0xfffff)
/* Register Type: MUPhyZQSR */
#define NFP_MU_PCTL_PHY_ZQSR0          0x05a4
#define NFP_MU_PCTL_PHY_ZQSR1          0x05ac
#define   NFP_MU_PCTL_PHY_ZQSR_IMP_CAL_DONE             (0x1 << 31)
#define   NFP_MU_PCTL_PHY_ZQSR_IMP_CAL_ERROR            (0x1 << 30)
#define   NFP_MU_PCTL_PHY_ZQSR_TERM_PULLUP_CAL_STATUS_of(_x) (((_x) >> 26) & 0x3)
#define   NFP_MU_PCTL_PHY_ZQSR_TERM_PULLDOWN_CAL_STATUS_of(_x) (((_x) >> 24) & 0x3)
#define   NFP_MU_PCTL_PHY_ZQSR_IMP_PULLUP_CAL_STATUS_of(_x) (((_x) >> 22) & 0x3)
#define     NFP_MU_PCTL_PHY_ZQSR_IMP_PULLUP_CAL_STATUS_OK (0)
#define     NFP_MU_PCTL_PHY_ZQSR_IMP_PULLUP_CAL_STATUS_OVERFLOW (1)
#define     NFP_MU_PCTL_PHY_ZQSR_IMP_PULLUP_CAL_STATUS_UNDEFLOW (2)
#define     NFP_MU_PCTL_PHY_ZQSR_IMP_PULLUP_CAL_STATUS_CALIBRATING (3)
#define   NFP_MU_PCTL_PHY_ZQSR_IMP_PULLDOWN_CAL_STATUS_of(_x) (((_x) >> 20) & 0x3)
#define     NFP_MU_PCTL_PHY_ZQSR_IMP_PULLDOWN_CAL_STATUS_OK (0)
#define     NFP_MU_PCTL_PHY_ZQSR_IMP_PULLDOWN_CAL_STATUS_OVERFLOW (1)
#define     NFP_MU_PCTL_PHY_ZQSR_IMP_PULLDOWN_CAL_STATUS_UNDERFLOW (2)
#define     NFP_MU_PCTL_PHY_ZQSR_IMP_PULLDOWN_CAL_STATUS_CALIBRATING (3)
#define   NFP_MU_PCTL_PHY_ZQSR_IMP_CTRL_of(_x)          ((_x) & 0xfffff)
/* Register Type: MUPhyDXGCR */
#define NFP_MU_PCTL_PHY_DXGCR0         0x05c0
#define NFP_MU_PCTL_PHY_DXGCR1         0x0600
#define NFP_MU_PCTL_PHY_DXGCR2         0x0640
#define NFP_MU_PCTL_PHY_DXGCR3         0x0680
#define NFP_MU_PCTL_PHY_DXGCR4         0x06c0
#define NFP_MU_PCTL_PHY_DXGCR5         0x0700
#define NFP_MU_PCTL_PHY_DXGCR6         0x0740
#define NFP_MU_PCTL_PHY_DXGCR7         0x0780
#define NFP_MU_PCTL_PHY_DXGCR8         0x07c0
#define   NFP_MU_PCTL_PHY_DXGCR_POWER_DOWN_RECEIVER     (0x1 << 10)
#define   NFP_MU_PCTL_PHY_DXGCR_POWER_DOWN_DRIVER       (0x1 << 9)
#define   NFP_MU_PCTL_PHY_DXGCR_MASTER_DELAY_LINE_EN    (0x1 << 8)
#define   NFP_MU_PCTL_PHY_DXGCR_WRITE_LEVEL_RANK_EN(_x) (((_x) & 0xf) << 4)
#define   NFP_MU_PCTL_PHY_DXGCR_WRITE_LEVEL_RANK_EN_of(_x) (((_x) >> 4) & 0xf)
#define   NFP_MU_PCTL_PHY_DXGCR_IDDQ_TEST               (0x1 << 3)
#define   NFP_MU_PCTL_PHY_DXGCR_TERM                    (0x1 << 2)
#define   NFP_MU_PCTL_PHY_DXGCR_CAL_BYP                 (0x1 << 1)
#define   NFP_MU_PCTL_PHY_DXGCR_DATA_EN                 (0x1)
/* Register Type: MUPhyDXGSR0 */
#define NFP_MU_PCTL_PHY_DXGSR00        0x05c4
#define NFP_MU_PCTL_PHY_DXGSR01        0x0604
#define NFP_MU_PCTL_PHY_DXGSR02        0x0644
#define NFP_MU_PCTL_PHY_DXGSR03        0x0684
#define NFP_MU_PCTL_PHY_DXGSR04        0x06c4
#define NFP_MU_PCTL_PHY_DXGSR05        0x0704
#define NFP_MU_PCTL_PHY_DXGSR06        0x0744
#define NFP_MU_PCTL_PHY_DXGSR07        0x0784
#define NFP_MU_PCTL_PHY_DXGSR08        0x07c4
#define   NFP_MU_PCTL_PHY_DXGSR0_READ_DQS_PERIOD_of(_x) (((_x) >> 15) & 0xff)
#define   NFP_MU_PCTL_PHY_DXGSR0_DATX8_PLL_LOCK         (0x1 << 14)
#define   NFP_MU_PCTL_PHY_DXGSR0_WRITE_LVL_PERIOD_of(_x) (((_x) >> 6) & 0xff)
#define   NFP_MU_PCTL_PHY_DXGSR0_WRITE_LVL_ERROR        (0x1 << 5)
#define   NFP_MU_PCTL_PHY_DXGSR0_WRITE_LVL_DONE         (0x1 << 4)
#define   NFP_MU_PCTL_PHY_DXGSR0_WRITE_LVL_CAL          (0x1 << 3)
#define   NFP_MU_PCTL_PHY_DXGSR0_READ_DQS_GATING_CAL    (0x1 << 2)
#define   NFP_MU_PCTL_PHY_DXGSR0_READ_DQS_CAL           (0x1 << 1)
#define   NFP_MU_PCTL_PHY_DXGSR0_WRITE_DQ_CAL           (0x1)
/* Register Type: MUPhyDXGSR1 */
#define NFP_MU_PCTL_PHY_DXGSR10        0x05c8
#define NFP_MU_PCTL_PHY_DXGSR11        0x0608
#define NFP_MU_PCTL_PHY_DXGSR12        0x0648
#define NFP_MU_PCTL_PHY_DXGSR13        0x0688
#define NFP_MU_PCTL_PHY_DXGSR14        0x06c8
#define NFP_MU_PCTL_PHY_DXGSR15        0x0708
#define NFP_MU_PCTL_PHY_DXGSR16        0x0748
#define NFP_MU_PCTL_PHY_DXGSR17        0x0788
#define NFP_MU_PCTL_PHY_DXGSR18        0x07c8
#define   NFP_MU_PCTL_PHY_DXGSR1_DELAY_LINE_TEST_CODE_of(_x) (((_x) >> 1) & 0xffffff)
#define   NFP_MU_PCTL_PHY_DXGSR1_DELAY_LINE_TEST_DONE   (0x1)
/* Register Type: MUPhyDXBDLR0 */
#define NFP_MU_PCTL_PHY_DXBDLR00       0x05cc
#define NFP_MU_PCTL_PHY_DXBDLR01       0x060c
#define NFP_MU_PCTL_PHY_DXBDLR02       0x064c
#define NFP_MU_PCTL_PHY_DXBDLR03       0x068c
#define NFP_MU_PCTL_PHY_DXBDLR04       0x06cc
#define NFP_MU_PCTL_PHY_DXBDLR05       0x070c
#define NFP_MU_PCTL_PHY_DXBDLR06       0x074c
#define NFP_MU_PCTL_PHY_DXBDLR07       0x078c
#define NFP_MU_PCTL_PHY_DXBDLR08       0x07cc
#define   NFP_MU_PCTL_PHY_DXBDLR0_DQ4_WRITE_DELAY(_x)   (((_x) & 0x3f) << 24)
#define   NFP_MU_PCTL_PHY_DXBDLR0_DQ4_WRITE_DELAY_of(_x) (((_x) >> 24) & 0x3f)
#define   NFP_MU_PCTL_PHY_DXBDLR0_DQ3_WRITE_DELAY(_x)   (((_x) & 0x3f) << 18)
#define   NFP_MU_PCTL_PHY_DXBDLR0_DQ3_WRITE_DELAY_of(_x) (((_x) >> 18) & 0x3f)
#define   NFP_MU_PCTL_PHY_DXBDLR0_DQ2_WRITE_DELAY(_x)   (((_x) & 0x3f) << 12)
#define   NFP_MU_PCTL_PHY_DXBDLR0_DQ2_WRITE_DELAY_of(_x) (((_x) >> 12) & 0x3f)
#define   NFP_MU_PCTL_PHY_DXBDLR0_DQ1_WRITE_DELAY(_x)   (((_x) & 0x3f) << 6)
#define   NFP_MU_PCTL_PHY_DXBDLR0_DQ1_WRITE_DELAY_of(_x) (((_x) >> 6) & 0x3f)
#define   NFP_MU_PCTL_PHY_DXBDLR0_DQ0_WRITE_DELAY(_x)   ((_x) & 0x3f)
#define   NFP_MU_PCTL_PHY_DXBDLR0_DQ0_WRITE_DELAY_of(_x) ((_x) & 0x3f)
/* Register Type: MUPhyDXBDLR1 */
#define NFP_MU_PCTL_PHY_DXBDLR10       0x05d0
#define NFP_MU_PCTL_PHY_DXBDLR11       0x0610
#define NFP_MU_PCTL_PHY_DXBDLR12       0x0650
#define NFP_MU_PCTL_PHY_DXBDLR13       0x0690
#define NFP_MU_PCTL_PHY_DXBDLR14       0x06d0
#define NFP_MU_PCTL_PHY_DXBDLR15       0x0710
#define NFP_MU_PCTL_PHY_DXBDLR16       0x0750
#define NFP_MU_PCTL_PHY_DXBDLR17       0x0790
#define NFP_MU_PCTL_PHY_DXBDLR18       0x07d0
#define   NFP_MU_PCTL_PHY_DXBDLR1_DQS_WRITE_DELAY(_x)   (((_x) & 0x3f) << 24)
#define   NFP_MU_PCTL_PHY_DXBDLR1_DQS_WRITE_DELAY_of(_x) (((_x) >> 24) & 0x3f)
#define   NFP_MU_PCTL_PHY_DXBDLR1_DQM_WRITE_DELAY(_x)   (((_x) & 0x3f) << 18)
#define   NFP_MU_PCTL_PHY_DXBDLR1_DQM_WRITE_DELAY_of(_x) (((_x) >> 18) & 0x3f)
#define   NFP_MU_PCTL_PHY_DXBDLR1_DQ7_WRITE_DELAY(_x)   (((_x) & 0x3f) << 12)
#define   NFP_MU_PCTL_PHY_DXBDLR1_DQ7_WRITE_DELAY_of(_x) (((_x) >> 12) & 0x3f)
#define   NFP_MU_PCTL_PHY_DXBDLR1_DQ6_WRITE_DELAY(_x)   (((_x) & 0x3f) << 6)
#define   NFP_MU_PCTL_PHY_DXBDLR1_DQ6_WRITE_DELAY_of(_x) (((_x) >> 6) & 0x3f)
#define   NFP_MU_PCTL_PHY_DXBDLR1_DQ5_WRITE_DELAY(_x)   ((_x) & 0x3f)
#define   NFP_MU_PCTL_PHY_DXBDLR1_DQ5_WRITE_DELAY_of(_x) ((_x) & 0x3f)
/* Register Type: MUPhyDXBDLR2 */
#define NFP_MU_PCTL_PHY_DXBDLR20       0x05d4
#define NFP_MU_PCTL_PHY_DXBDLR21       0x0614
#define NFP_MU_PCTL_PHY_DXBDLR22       0x0654
#define NFP_MU_PCTL_PHY_DXBDLR23       0x0694
#define NFP_MU_PCTL_PHY_DXBDLR24       0x06d4
#define NFP_MU_PCTL_PHY_DXBDLR25       0x0714
#define NFP_MU_PCTL_PHY_DXBDLR26       0x0754
#define NFP_MU_PCTL_PHY_DXBDLR27       0x0794
#define NFP_MU_PCTL_PHY_DXBDLR28       0x07d4
#define   NFP_MU_PCTL_PHY_DXBDLR2_DQ_OUT_ENABLE_DELAY(_x) (((_x) & 0x3f) << 6)
#define   NFP_MU_PCTL_PHY_DXBDLR2_DQ_OUT_ENABLE_DELAY_of(_x) (((_x) >> 6) & 0x3f)
#define   NFP_MU_PCTL_PHY_DXBDLR2_DQS_OUT_ENABLE_DELAY(_x) ((_x) & 0x3f)
#define   NFP_MU_PCTL_PHY_DXBDLR2_DQS_OUT_ENABLE_DELAY_of(_x) ((_x) & 0x3f)
/* Register Type: MUPhyDXBDLR3 */
#define NFP_MU_PCTL_PHY_DXBDLR30       0x05d8
#define NFP_MU_PCTL_PHY_DXBDLR31       0x0618
#define NFP_MU_PCTL_PHY_DXBDLR32       0x0658
#define NFP_MU_PCTL_PHY_DXBDLR33       0x0698
#define NFP_MU_PCTL_PHY_DXBDLR34       0x06d8
#define NFP_MU_PCTL_PHY_DXBDLR35       0x0718
#define NFP_MU_PCTL_PHY_DXBDLR36       0x0758
#define NFP_MU_PCTL_PHY_DXBDLR37       0x0798
#define NFP_MU_PCTL_PHY_DXBDLR38       0x07d8
#define   NFP_MU_PCTL_PHY_DXBDLR3_DQ4_READ_DELAY(_x)    (((_x) & 0x3f) << 24)
#define   NFP_MU_PCTL_PHY_DXBDLR3_DQ4_READ_DELAY_of(_x) (((_x) >> 24) & 0x3f)
#define   NFP_MU_PCTL_PHY_DXBDLR3_DQ3_READ_DELAY(_x)    (((_x) & 0x3f) << 18)
#define   NFP_MU_PCTL_PHY_DXBDLR3_DQ3_READ_DELAY_of(_x) (((_x) >> 18) & 0x3f)
#define   NFP_MU_PCTL_PHY_DXBDLR3_DQ2_READ_DELAY(_x)    (((_x) & 0x3f) << 12)
#define   NFP_MU_PCTL_PHY_DXBDLR3_DQ2_READ_DELAY_of(_x) (((_x) >> 12) & 0x3f)
#define   NFP_MU_PCTL_PHY_DXBDLR3_DQ1_READ_DELAY(_x)    (((_x) & 0x3f) << 6)
#define   NFP_MU_PCTL_PHY_DXBDLR3_DQ1_READ_DELAY_of(_x) (((_x) >> 6) & 0x3f)
#define   NFP_MU_PCTL_PHY_DXBDLR3_DQ0_READ_DELAY(_x)    ((_x) & 0x3f)
#define   NFP_MU_PCTL_PHY_DXBDLR3_DQ0_READ_DELAY_of(_x) ((_x) & 0x3f)
/* Register Type: MUPhyDXBDLR4 */
#define NFP_MU_PCTL_PHY_DXBDLR40       0x05dc
#define NFP_MU_PCTL_PHY_DXBDLR41       0x061c
#define NFP_MU_PCTL_PHY_DXBDLR42       0x065c
#define NFP_MU_PCTL_PHY_DXBDLR43       0x069c
#define NFP_MU_PCTL_PHY_DXBDLR44       0x06dc
#define NFP_MU_PCTL_PHY_DXBDLR45       0x071c
#define NFP_MU_PCTL_PHY_DXBDLR46       0x075c
#define NFP_MU_PCTL_PHY_DXBDLR47       0x079c
#define NFP_MU_PCTL_PHY_DXBDLR48       0x07dc
#define   NFP_MU_PCTL_PHY_DXBDLR4_DQS_READ_DELAY(_x)    (((_x) & 0x3f) << 24)
#define   NFP_MU_PCTL_PHY_DXBDLR4_DQS_READ_DELAY_of(_x) (((_x) >> 24) & 0x3f)
#define   NFP_MU_PCTL_PHY_DXBDLR4_DQM_READ_DELAY(_x)    (((_x) & 0x3f) << 18)
#define   NFP_MU_PCTL_PHY_DXBDLR4_DQM_READ_DELAY_of(_x) (((_x) >> 18) & 0x3f)
#define   NFP_MU_PCTL_PHY_DXBDLR4_DQ7_READ_DELAY(_x)    (((_x) & 0x3f) << 12)
#define   NFP_MU_PCTL_PHY_DXBDLR4_DQ7_READ_DELAY_of(_x) (((_x) >> 12) & 0x3f)
#define   NFP_MU_PCTL_PHY_DXBDLR4_DQ6_READ_DELAY(_x)    (((_x) & 0x3f) << 6)
#define   NFP_MU_PCTL_PHY_DXBDLR4_DQ6_READ_DELAY_of(_x) (((_x) >> 6) & 0x3f)
#define   NFP_MU_PCTL_PHY_DXBDLR4_DQ5_READ_DELAY(_x)    ((_x) & 0x3f)
#define   NFP_MU_PCTL_PHY_DXBDLR4_DQ5_READ_DELAY_of(_x) ((_x) & 0x3f)
/* Register Type: MUPhyDXLCDLR0 */
#define NFP_MU_PCTL_PHY_DXLCDLR0_0     0x05e0
#define NFP_MU_PCTL_PHY_DXLCDLR0_1     0x0620
#define NFP_MU_PCTL_PHY_DXLCDLR0_2     0x0660
#define NFP_MU_PCTL_PHY_DXLCDLR0_3     0x06a0
#define NFP_MU_PCTL_PHY_DXLCDLR0_4     0x06e0
#define NFP_MU_PCTL_PHY_DXLCDLR0_5     0x0720
#define NFP_MU_PCTL_PHY_DXLCDLR0_6     0x0760
#define NFP_MU_PCTL_PHY_DXLCDLR0_7     0x07a0
#define NFP_MU_PCTL_PHY_DXLCDLR0_8     0x07e0
#define   NFP_MU_PCTL_PHY_DXLCDLR0__RANK3_WRITE_DELAY(_x) (((_x) & 0xff) << 24)
#define   NFP_MU_PCTL_PHY_DXLCDLR0__RANK3_WRITE_DELAY_of(_x) (((_x) >> 24) & 0xff)
#define   NFP_MU_PCTL_PHY_DXLCDLR0__RANK2_WRITE_DELAY(_x) (((_x) & 0xff) << 16)
#define   NFP_MU_PCTL_PHY_DXLCDLR0__RANK2_WRITE_DELAY_of(_x) (((_x) >> 16) & 0xff)
#define   NFP_MU_PCTL_PHY_DXLCDLR0__RANK1_WRITE_DELAY(_x) (((_x) & 0xff) << 8)
#define   NFP_MU_PCTL_PHY_DXLCDLR0__RANK1_WRITE_DELAY_of(_x) (((_x) >> 8) & 0xff)
#define   NFP_MU_PCTL_PHY_DXLCDLR0__RANK0_WRITE_DELAY(_x) ((_x) & 0xff)
#define   NFP_MU_PCTL_PHY_DXLCDLR0__RANK0_WRITE_DELAY_of(_x) ((_x) & 0xff)
/* Register Type: MUPhyDXLCDLR1 */
#define NFP_MU_PCTL_PHY_DXLCDLR1_0     0x05e4
#define NFP_MU_PCTL_PHY_DXLCDLR1_1     0x0624
#define NFP_MU_PCTL_PHY_DXLCDLR1_2     0x0664
#define NFP_MU_PCTL_PHY_DXLCDLR1_3     0x06a4
#define NFP_MU_PCTL_PHY_DXLCDLR1_4     0x06e4
#define NFP_MU_PCTL_PHY_DXLCDLR1_5     0x0724
#define NFP_MU_PCTL_PHY_DXLCDLR1_6     0x0764
#define NFP_MU_PCTL_PHY_DXLCDLR1_7     0x07a4
#define NFP_MU_PCTL_PHY_DXLCDLR1_8     0x07e4
#define   NFP_MU_PCTL_PHY_DXLCDLR1__READ_DQS_DELAY(_x)  (((_x) & 0xff) << 8)
#define   NFP_MU_PCTL_PHY_DXLCDLR1__READ_DQS_DELAY_of(_x) (((_x) >> 8) & 0xff)
#define   NFP_MU_PCTL_PHY_DXLCDLR1__WRITE_DELAY(_x)     ((_x) & 0xff)
#define   NFP_MU_PCTL_PHY_DXLCDLR1__WRITE_DELAY_of(_x)  ((_x) & 0xff)
/* Register Type: MUPhyDXLCDLR2 */
#define NFP_MU_PCTL_PHY_DXLCDLR2_0     0x05e8
#define NFP_MU_PCTL_PHY_DXLCDLR2_1     0x0628
#define NFP_MU_PCTL_PHY_DXLCDLR2_2     0x0668
#define NFP_MU_PCTL_PHY_DXLCDLR2_3     0x06a8
#define NFP_MU_PCTL_PHY_DXLCDLR2_4     0x06e8
#define NFP_MU_PCTL_PHY_DXLCDLR2_5     0x0728
#define NFP_MU_PCTL_PHY_DXLCDLR2_6     0x0768
#define NFP_MU_PCTL_PHY_DXLCDLR2_7     0x07a8
#define NFP_MU_PCTL_PHY_DXLCDLR2_8     0x07e8
#define   NFP_MU_PCTL_PHY_DXLCDLR2__RNK3_DQS_GATING_DELAY(_x) (((_x) & 0xff) << 24)
#define   NFP_MU_PCTL_PHY_DXLCDLR2__RNK3_DQS_GATING_DELAY_of(_x) (((_x) >> 24) & 0xff)
#define   NFP_MU_PCTL_PHY_DXLCDLR2__RANK2_DQS_GATING_DELAY(_x) (((_x) & 0xff) << 16)
#define   NFP_MU_PCTL_PHY_DXLCDLR2__RANK2_DQS_GATING_DELAY_of(_x) (((_x) >> 16) & 0xff)
#define   NFP_MU_PCTL_PHY_DXLCDLR2__RANK1_DQS_GATING_DELAY(_x) (((_x) & 0xff) << 8)
#define   NFP_MU_PCTL_PHY_DXLCDLR2__RANK1_DQS_GATING_DELAY_of(_x) (((_x) >> 8) & 0xff)
#define   NFP_MU_PCTL_PHY_DXLCDLR2__RANK0_DQS_GATING_DELAY(_x) ((_x) & 0xff)
#define   NFP_MU_PCTL_PHY_DXLCDLR2__RANK0_DQS_GATING_DELAY_of(_x) ((_x) & 0xff)
/* Register Type: MUPhyDXMDLR */
#define NFP_MU_PCTL_PHY_DXMDLR0        0x05ec
#define NFP_MU_PCTL_PHY_DXMDLR1        0x062c
#define NFP_MU_PCTL_PHY_DXMDLR2        0x066c
#define NFP_MU_PCTL_PHY_DXMDLR3        0x06ac
#define NFP_MU_PCTL_PHY_DXMDLR4        0x06ec
#define NFP_MU_PCTL_PHY_DXMDLR5        0x072c
#define NFP_MU_PCTL_PHY_DXMDLR6        0x076c
#define NFP_MU_PCTL_PHY_DXMDLR7        0x07ac
#define NFP_MU_PCTL_PHY_DXMDLR8        0x07ec
#define   NFP_MU_PCTL_PHY_DXMDLR_FIXME1(_x)             (((_x) & 0xff) << 16)
#define   NFP_MU_PCTL_PHY_DXMDLR_FIXME1_of(_x)          (((_x) >> 16) & 0xff)
#define   NFP_MU_PCTL_PHY_DXMDLR_FIXME2(_x)             (((_x) & 0xff) << 8)
#define   NFP_MU_PCTL_PHY_DXMDLR_FIXME2_of(_x)          (((_x) >> 8) & 0xff)
#define   NFP_MU_PCTL_PHY_DXMDLR_FIXME3(_x)             ((_x) & 0xff)
#define   NFP_MU_PCTL_PHY_DXMDLR_FIXME3_of(_x)          ((_x) & 0xff)
/* HGID: nfp3200/mu_pub.desc = ade0020e9035 */
/* Register Type: MUPubMisc */
#define NFP_MU_PCTL_PUB_MISC           0x0480
#define   NFP_MU_PCTL_PUB_MISC_FIXME1                   (0x1 << 2)
#define   NFP_MU_PCTL_PUB_MISC_RESET                    (0x1 << 1)
#define   NFP_MU_PCTL_PUB_MISC_TR_EN                    (0x1)
/* Register Type: MUPubSMCtl */
#define NFP_MU_PCTL_PUB_SMCTL          0x0484
#define   NFP_MU_PCTL_PUB_SMCTL_MSCMP(_x)               (((_x) & 0xffff) << 16)
#define   NFP_MU_PCTL_PUB_SMCTL_MSCMP_of(_x)            (((_x) >> 16) & 0xffff)
#define   NFP_MU_PCTL_PUB_SMCTL_MSWAIT(_x)              ((_x) & 0xffff)
#define   NFP_MU_PCTL_PUB_SMCTL_MSWAIT_of(_x)           ((_x) & 0xffff)
/* Register Type: MUPubSMStat */
#define NFP_MU_PCTL_PUB_SMSTAT         0x0488
#define   NFP_MU_PCTL_PUB_SMSTAT_MSE_of(_x)             (((_x) >> 16) & 0xffff)
#define   NFP_MU_PCTL_PUB_SMSTAT_MSS_of(_x)             ((_x) & 0xffff)
/* Register Type: MUPubMR1 */
#define NFP_MU_PCTL_PUB_MR1            0x048c
#define   NFP_MU_PCTL_PUB_MR1_MR1(_x)                   ((_x) & 0xffff)
#define   NFP_MU_PCTL_PUB_MR1_MR1_of(_x)                ((_x) & 0xffff)
/* Register Type: MUPubTWLMRD */
#define NFP_MU_PCTL_PUB_TWLMRD         0x0490
#define   NFP_MU_PCTL_PUB_TWLMRD_TWLMRD(_x)             ((_x) & 0x3f)
#define   NFP_MU_PCTL_PUB_TWLMRD_TWLMRD_of(_x)          ((_x) & 0x3f)
/* Register Type: MUPubTR_ADDR */
#define NFP_MU_PCTL_PUB_TR_ADDR0       0x0494
#define NFP_MU_PCTL_PUB_TR_ADDR1       0x0498
#define NFP_MU_PCTL_PUB_TR_ADDR2       0x049c
#define NFP_MU_PCTL_PUB_TR_ADDR3       0x04a0
#define   NFP_MU_PCTL_PUB_TR_ADDR_ROW_ADDR(_x)          (((_x) & 0x1ffff) << 13)
#define   NFP_MU_PCTL_PUB_TR_ADDR_ROW_ADDR_of(_x)       (((_x) >> 13) & 0x1ffff)
#define   NFP_MU_PCTL_PUB_TR_ADDR_BANK_ADDR(_x)         (((_x) & 0x7) << 10)
#define   NFP_MU_PCTL_PUB_TR_ADDR_BANK_ADDR_of(_x)      (((_x) >> 10) & 0x7)
#define   NFP_MU_PCTL_PUB_TR_ADDR_COL_ADDR(_x)          ((_x) & 0x3ff)
#define   NFP_MU_PCTL_PUB_TR_ADDR_COL_ADDR_of(_x)       ((_x) & 0x3ff)
/* Register Type: MUPubEYE_TR_LCDL */
#define NFP_MU_PCTL_PUB_EYE_TR_LCDL    0x04a4
#define   NFP_MU_PCTL_PUB_EYE_TR_LCDL_INCR_DLY(_x)      (((_x) & 0xff) << 8)
#define   NFP_MU_PCTL_PUB_EYE_TR_LCDL_INCR_DLY_of(_x)   (((_x) >> 8) & 0xff)
#define   NFP_MU_PCTL_PUB_EYE_TR_LCDL_QRTR_DLY(_x)      ((_x) & 0xff)
#define   NFP_MU_PCTL_PUB_EYE_TR_LCDL_QRTR_DLY_of(_x)   ((_x) & 0xff)
/* Register Type: MUPubPPMCFG */
#define NFP_MU_PCTL_PUB_PPMCFG         0x04a8
#define   NFP_MU_PCTL_PUB_PPMCFG_FIXME2(_x)             (((_x) & 0xf) << 9)
#define   NFP_MU_PCTL_PUB_PPMCFG_FIXME2_of(_x)          (((_x) >> 9) & 0xf)
#define     NFP_MU_PCTL_PUB_PPMCFG_FIXME2_FIXME9        (14)
#define     NFP_MU_PCTL_PUB_PPMCFG_FIXME2_FIXME10       (12)
#define     NFP_MU_PCTL_PUB_PPMCFG_FIXME2_FIXME11       (8)
#define     NFP_MU_PCTL_PUB_PPMCFG_FIXME2_FIXME12       (0)
#define   NFP_MU_PCTL_PUB_PPMCFG_FIXME5(_x)             (((_x) & 0xff) << 1)
#define   NFP_MU_PCTL_PUB_PPMCFG_FIXME5_of(_x)          (((_x) >> 1) & 0xff)
#define   NFP_MU_PCTL_PUB_PPMCFG_FIXME8                 (0x1)
#define     NFP_MU_PCTL_PUB_PPMCFG_FIXME8_FIXME3        (0x0)
#define     NFP_MU_PCTL_PUB_PPMCFG_FIXME8_FIXME4        (0x1)
/* Register Type: MUPubSLCR */
#define NFP_MU_PCTL_PUB_FIXME0         0x04ac
#define   NFP_MU_PCTL_PUB_FIXME0_FIXME13(_x)            (((_x) & 0xf) << 20)
#define   NFP_MU_PCTL_PUB_FIXME0_FIXME13_of(_x)         (((_x) >> 20) & 0xf)
#define   NFP_MU_PCTL_PUB_FIXME0_FIXME14(_x)            (((_x) & 0x3) << 16)
#define   NFP_MU_PCTL_PUB_FIXME0_FIXME14_of(_x)         (((_x) >> 16) & 0x3)
#define   NFP_MU_PCTL_PUB_FIXME0_FIXME15(_x)            (((_x) & 0xf) << 12)
#define   NFP_MU_PCTL_PUB_FIXME0_FIXME15_of(_x)         (((_x) >> 12) & 0xf)
#define   NFP_MU_PCTL_PUB_FIXME0_FIXME16(_x)            (((_x) & 0x3) << 8)
#define   NFP_MU_PCTL_PUB_FIXME0_FIXME16_of(_x)         (((_x) >> 8) & 0x3)
#define   NFP_MU_PCTL_PUB_FIXME0_FIXME17(_x)            (((_x) & 0xf) << 4)
#define   NFP_MU_PCTL_PUB_FIXME0_FIXME17_of(_x)         (((_x) >> 4) & 0xf)
#define   NFP_MU_PCTL_PUB_FIXME0_FIXME18(_x)            ((_x) & 0x3)
#define   NFP_MU_PCTL_PUB_FIXME0_FIXME18_of(_x)         ((_x) & 0x3)
/* Register Type: MUPubBSTCTL */
#define NFP_MU_PCTL_PUB_BSTCTL         0x04c0
#define   NFP_MU_PCTL_PUB_BSTCTL_BIST_DATA_LOOPBACK_SEL(_x) (((_x) & 0xf) << 10)
#define   NFP_MU_PCTL_PUB_BSTCTL_BIST_DATA_LOOPBACK_SEL_of(_x) (((_x) >> 10) & 0xf)
#define   NFP_MU_PCTL_PUB_BSTCTL_BIST_RUN               (0x1 << 9)
#define   NFP_MU_PCTL_PUB_BSTCTL_CLR_ERROR_COUNTERS     (0x1 << 8)
#define   NFP_MU_PCTL_PUB_BSTCTL_WALKING_BIT_SEL        (0x1 << 7)
#define   NFP_MU_PCTL_PUB_BSTCTL_FIXME19                (0x1 << 6)
#define   NFP_MU_PCTL_PUB_BSTCTL_FIXME20                (0x1 << 5)
#define   NFP_MU_PCTL_PUB_BSTCTL_WORD_LINE_A_SEL        (0x1 << 4)
#define   NFP_MU_PCTL_PUB_BSTCTL_WORD_LINE_B_SEL        (0x1 << 3)
#define   NFP_MU_PCTL_PUB_BSTCTL_WORD_LINE_DATA_MASK_SEL (0x1 << 2)
#define   NFP_MU_PCTL_PUB_BSTCTL_BIST_MODE              (0x1 << 1)
#define   NFP_MU_PCTL_PUB_BSTCTL_BIST_EN                (0x1)
/* Register Type: MUPubBSTSTAT */
#define NFP_MU_PCTL_PUB_BSTSTAT        0x04c4
#define   NFP_MU_PCTL_PUB_BSTSTAT_ERROR_DATA_MASK       (0x1 << 5)
#define   NFP_MU_PCTL_PUB_BSTSTAT_ERROR_DATA_CMP        (0x1 << 4)
#define   NFP_MU_PCTL_PUB_BSTSTAT_ERROR_ADDR_CMP        (0x1 << 3)
#define   NFP_MU_PCTL_PUB_BSTSTAT_BIST_RUNNING          (0x1)
/* Register Type: MUPubBSTCNT */
#define NFP_MU_PCTL_PUB_BSTCNT         0x04c8
#define   NFP_MU_PCTL_PUB_BSTCNT_COUNT(_x)              ((_x) & 0xffff)
#define   NFP_MU_PCTL_PUB_BSTCNT_COUNT_of(_x)           ((_x) & 0xffff)
/* Register Type: MUPubBSTWRD */
#define NFP_MU_PCTL_PUB_BSTWRD         0x04cc
#define   NFP_MU_PCTL_PUB_BSTWRD_LINE_B(_x)             (((_x) & 0xffff) << 16)
#define   NFP_MU_PCTL_PUB_BSTWRD_LINE_B_of(_x)          (((_x) >> 16) & 0xffff)
#define   NFP_MU_PCTL_PUB_BSTWRD_LINE_A(_x)             ((_x) & 0xffff)
#define   NFP_MU_PCTL_PUB_BSTWRD_LINE_A_of(_x)          ((_x) & 0xffff)
/* Register Type: MUPubBSTMSK */
#define NFP_MU_PCTL_PUB_BSTMSK         0x04d0
#define   NFP_MU_PCTL_PUB_BSTMSK_MASK(_x)               ((_x) & 0xffff)
#define   NFP_MU_PCTL_PUB_BSTMSK_MASK_of(_x)            ((_x) & 0xffff)
/* Register Type: MUPubBSTSEED */
#define NFP_MU_PCTL_PUB_BSTSEED        0x04d4
#define   NFP_MU_PCTL_PUB_BSTSEED_FIXME21(_x)           (_x)
#define   NFP_MU_PCTL_PUB_BSTSEED_FIXME21_of(_x)        (_x)
/* Register Type: MUPubBSTDWEC */
#define NFP_MU_PCTL_PUB_BSTDWEC        0x04d8
#define   NFP_MU_PCTL_PUB_BSTDWEC_MASK_COUNT_of(_x)     (((_x) >> 16) & 0xffff)
#define   NFP_MU_PCTL_PUB_BSTDWEC_DATA_COUNT_of(_x)     ((_x) & 0xffff)
/* Register Type: MUPubBSTAWEC */
#define NFP_MU_PCTL_PUB_BSTAWEC        0x04dc
#define   NFP_MU_PCTL_PUB_BSTAWEC_ADDR_of(_x)           ((_x) & 0xffff)
/* Register Type: MUPubIPVR */
#define NFP_MU_PCTL_PUB_PUB_IPVR       0x04f8
#define   NFP_MU_PCTL_PUB_PUB_IPVR_PUB_IP_VERSION_of(_x) (_x)
/* Register Type: MUPubIPTR */
#define NFP_MU_PCTL_PUB_PUB_IPTR       0x04fc
#define   NFP_MU_PCTL_PUB_PUB_IPTR_PUB_IP_TYPE_of(_x)   (_x)
/* HGID: nfp3200/mu_tcache_bank.desc = ade0020e9035 */
/* Register Type: MUTCacheConfig */
#define NFP_MU_TCACHE_CONF             0x0000
#define   NFP_MU_TCACHE_CONF_REALLY_DIRTY_WM(_x)        (((_x) & 0xf) << 28)
#define   NFP_MU_TCACHE_CONF_REALLY_DIRTY_WM_of(_x)     (((_x) >> 28) & 0xf)
#define   NFP_MU_TCACHE_CONF_DIRTY_WM(_x)               (((_x) & 0xf) << 24)
#define   NFP_MU_TCACHE_CONF_DIRTY_WM_of(_x)            (((_x) >> 24) & 0xf)
#define   NFP_MU_TCACHE_CONF_HOLDOFF_TIMER(_x)          (((_x) & 0xff) << 16)
#define   NFP_MU_TCACHE_CONF_HOLDOFF_TIMER_of(_x)       (((_x) >> 16) & 0xff)
#define   NFP_MU_TCACHE_CONF_LLR_MASK(_x)               (((_x) & 0xff) << 8)
#define   NFP_MU_TCACHE_CONF_LLR_MASK_of(_x)            (((_x) >> 8) & 0xff)
#define   NFP_MU_TCACHE_CONF_HLR_MASK(_x)               ((_x) & 0xff)
#define   NFP_MU_TCACHE_CONF_HLR_MASK_of(_x)            ((_x) & 0xff)
/* Register Type: MUTCacheConfigClean */
#define NFP_MU_TCACHE_CLEANCONF        0x0004
#define   NFP_MU_TCACHE_CLEANCONF_CLK_DIV(_x)           (((_x) & 0xff) << 8)
#define   NFP_MU_TCACHE_CLEANCONF_CLK_DIV_of(_x)        (((_x) >> 8) & 0xff)
#define   NFP_MU_TCACHE_CLEANCONF_ON_ALLOC              (0x1 << 3)
#define   NFP_MU_TCACHE_CLEANCONF_ON_HLR_RELEASE        (0x1 << 2)
#define   NFP_MU_TCACHE_CLEANCONF_ON_RELEASE            (0x1 << 1)
#define   NFP_MU_TCACHE_CLEANCONF_ENABLE                (0x1)
/* Register Type: MUTCacheCommandStatus */
#define NFP_MU_TCACHE_CMDST            0x0008
#define   NFP_MU_TCACHE_CMDST_ABORTED                   (0x1 << 2)
#define   NFP_MU_TCACHE_CMDST_VALID                     (0x1 << 1)
#define   NFP_MU_TCACHE_CMDST_ERROR                     (0x1)
/* Register Type: MUTCacheCommandControl */
#define NFP_MU_TCACHE_CMDCTL           0x000c
#define   NFP_MU_TCACHE_CMDCTL_SET(_x)                  (((_x) & 0x1ff) << 16)
#define   NFP_MU_TCACHE_CMDCTL_WAYS(_x)                 (((_x) & 0xff) << 8)
#define   NFP_MU_TCACHE_CMDCTL_TYPE(_x)                 ((_x) & 0x7)
#define     NFP_MU_TCACHE_CMDCTL_TYPE_WRWAYS            (4)
#define     NFP_MU_TCACHE_CMDCTL_TYPE_YES               (5)
/* Register Type: MUTCacheWriteData */
#define NFP_MU_TCACHE_WDATA            0x0010
#define   NFP_MU_TCACHE_WDATA_STATE(_x)                 (((_x) & 0x7) << 16)
#define   NFP_MU_TCACHE_WDATA_STATE_of(_x)              (((_x) >> 16) & 0x7)
#define     NFP_MU_TCACHE_WDATA_STATE_LOCKED            (0)
#define     NFP_MU_TCACHE_WDATA_STATE_INUSE             (1)
#define     NFP_MU_TCACHE_WDATA_STATE_DIRTYHLR          (2)
#define     NFP_MU_TCACHE_WDATA_STATE_DIRTYLLR          (3)
#define     NFP_MU_TCACHE_WDATA_STATE_CLEANHLR          (4)
#define     NFP_MU_TCACHE_WDATA_STATE_CLEANLLR          (5)
#define   NFP_MU_TCACHE_WDATA_TAG(_x)                   ((_x) & 0x7fff)
#define   NFP_MU_TCACHE_WDATA_TAG_of(_x)                ((_x) & 0x7fff)
/* Register Type: MUTCacheCommandResponse */
#define NFP_MU_TCACHE_CMDRESP          0x0014
#define   NFP_MU_TCACHE_CMDRESP_VALID                   (0x1 << 31)
#define   NFP_MU_TCACHE_CMDRESP_TYPE_of(_x)             (((_x) >> 28) & 0x7)
#define   NFP_MU_TCACHE_CMDRESP_WAY_of(_x)              (((_x) >> 20) & 0x7)
#define   NFP_MU_TCACHE_CMDRESP_STATE_of(_x)            (((_x) >> 16) & 0x7)
#define   NFP_MU_TCACHE_CMDRESP_TAG_of(_x)              ((_x) & 0x7fff)
/* Register Type: MUTCacheReadData */
#define NFP_MU_TCACHE_RDATA            0x0018
#define   NFP_MU_TCACHE_RDATA_STATE_of(_x)              (((_x) >> 16) & 0x7)
#define   NFP_MU_TCACHE_RDATA_TAG_of(_x)                ((_x) & 0x7fff)
/* Register Type: MUTCacheCleanMarkAll */
#define NFP_MU_TCACHE_CLEANALL         0x0020
#define   NFP_MU_TCACHE_CLEANALL_DATA(_x)               ((_x) & 0x3)
#define     NFP_MU_TCACHE_CLEANALL_DATA_CLEAN           (0)
#define     NFP_MU_TCACHE_CLEANALL_DATA_DIRTY           (1)
#define     NFP_MU_TCACHE_CLEANALL_DATA_URGENT          (2)
/* Register Type: MUTCacheCleanGetDirtiness */
#define NFP_MU_TCACHE_CLEANDIRTY       0x0028
#define   NFP_MU_TCACHE_CLEANDIRTY_TYPE                 (0x1 << 4)
#define     NFP_MU_TCACHE_CLEANDIRTY_TYPE_URGENT        (0x0)
#define     NFP_MU_TCACHE_CLEANDIRTY_TYPE_DIRTY         (0x10)
#define   NFP_MU_TCACHE_CLEANDIRTY_GROUP(_x)            ((_x) & 0xf)
/* Register Type: MUTCacheCleanMarkSet */
#define NFP_MU_TCACHE_CLEANSET         0x002c
#define   NFP_MU_TCACHE_CLEANSET_GROUP(_x)              (((_x) & 0xf) << 9)
#define   NFP_MU_TCACHE_CLEANSET_SET(_x)                (((_x) & 0x1f) << 4)
#define   NFP_MU_TCACHE_CLEANSET_DATA(_x)               ((_x) & 0x3)
#define     NFP_MU_TCACHE_CLEANSET_DATA_CLEAN           (0)
#define     NFP_MU_TCACHE_CLEANSET_DATA_DIRTY           (1)
#define     NFP_MU_TCACHE_CLEANSET_DATA_URGENT          (2)
/* Register Type: MUTCacheDirtiness */
#define NFP_MU_TCACHE_CLEANNESS        0x0030
#define   NFP_MU_TCACHE_CLEANNESS_DATA(_x)              ((_x) & 0xff)
/* Register Type: MUTCachePerformanceCounterMode */
#define NFP_MU_TCACHE_PERFMODE         0x0034
#define   NFP_MU_TCACHE_PERFMODE_DATA(_x)               ((_x) & 0xf)
#define   NFP_MU_TCACHE_PERFMODE_DATA_of(_x)            ((_x) & 0xf)
#define     NFP_MU_TCACHE_PERFMODE_DATA_HOLD            (0)
#define     NFP_MU_TCACHE_PERFMODE_DATA_RESET           (1)
#define     NFP_MU_TCACHE_PERFMODE_DATA_STATS           (2)
#define     NFP_MU_TCACHE_PERFMODE_DATA_REQ0MON         (3)
#define     NFP_MU_TCACHE_PERFMODE_DATA_ALLOCCLEAN      (4)
#define     NFP_MU_TCACHE_PERFMODE_DATA_ALLOCRDY        (5)
#define     NFP_MU_TCACHE_PERFMODE_DATA_ALLOCEVICT      (6)
#define     NFP_MU_TCACHE_PERFMODE_DATA_DIRTY           (7)
#define     NFP_MU_TCACHE_PERFMODE_DATA_SETREQ          (8)
#define     NFP_MU_TCACHE_PERFMODE_DATA_FULLALL         (9)
#define     NFP_MU_TCACHE_PERFMODE_DATA_FULLHIGH        (10)
#define     NFP_MU_TCACHE_PERFMODE_DATA_FULLPAIRS       (11)
/* Register Type: MUTCachePerformanceCounterValue */
#define NFP_MU_TCACHE_PERFCOUNTER0     0x0040
#define NFP_MU_TCACHE_PERFCOUNTER1     0x0044
#define NFP_MU_TCACHE_PERFCOUNTER2     0x0048
#define NFP_MU_TCACHE_PERFCOUNTER3     0x004c
#define NFP_MU_TCACHE_PERFCOUNTER4     0x0050
#define NFP_MU_TCACHE_PERFCOUNTER5     0x0054
#define NFP_MU_TCACHE_PERFCOUNTER6     0x0058
#define NFP_MU_TCACHE_PERFCOUNTER7     0x005c
#define   NFP_MU_TCACHE_PERFCOUNTER_DATA_of(_x)         (_x)

/* Memory Unit */
#define NFP_XPB_MU			NFP_XPB_DEST(21, 1)	/* Memory Unit CSRs */
#define NFP_XPB_MU_PCTL0		NFP_XPB_DEST(21, 2)	/* Memory Unit CSRs */
#define NFP_XPB_MU_PCTL1		NFP_XPB_DEST(21, 3)	/* Memory Unit CSRs */
#define NFP_XPB_MU_DRAM_ECCMON		NFP_XPB_DEST(21, 8)
#define NFP_XPB_MU_DCACHE_ECCMON	NFP_XPB_DEST(21, 10)
#define NFP_XPB_MU_TCACHE(i)		NFP_XPB_DEST(21, 16 + (i*2))	/* MU Tag Cache */
#define NFP_XPB_MU_TCACHE_ECCMON(i)	NFP_XPB_DEST(21, 16 + (i*2) + 1)	/* MU Tag Cache ECC Monitor */

#endif /* NFP3200_NFP_MU_H */
