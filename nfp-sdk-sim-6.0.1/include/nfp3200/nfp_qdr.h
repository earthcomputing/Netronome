/*
 * Copyright (C) 2008-2011, Netronome Systems, Inc.  All rights reserved.
 *
 * Disclaimer: this file is provided without any warranty
 * as part of an early-access program, and the content is
 * bound to change before the final release.
 */

#ifndef NFP3200_NFP_QDR_H
#define NFP3200_NFP_QDR_H

/* HGID: nfp3200/qdr.desc = ade0020e9035 */
/* Register Type: QDRSramControl */
#define NFP_QDR_CTL                    0x0000
#define   NFP_QDR_CTL_QDR_SWIZZLE                       (0x1 << 17)
#define     NFP_QDR_CTL_QDR_SWIZZLE_NONE                (0x0)
#define     NFP_QDR_CTL_QDR_SWIZZLE_SWAP                (0x20000)
#define   NFP_QDR_CTL_Q_CTL_MODE                        (0x1 << 16)
#define   NFP_QDR_CTL_QC_IGN_EOP                        (0x1 << 14)
#define     NFP_QDR_CTL_QC_IGN_EOP_OFF                  (0x0)
#define     NFP_QDR_CTL_QC_IGN_EOP_ON                   (0x4000)
#define   NFP_QDR_CTL_QC_IGN_SEG_CNT                    (0x1 << 13)
#define     NFP_QDR_CTL_QC_IGN_SEG_CNT_OFF              (0x0)
#define     NFP_QDR_CTL_QC_IGN_SEG_CNT_ON               (0x2000)
#define   NFP_QDR_CTL_PIPELINE(_x)                      (((_x) & 0x7) << 10)
#define   NFP_QDR_CTL_PIPELINE_of(_x)                   (((_x) >> 10) & 0x7)
#define     NFP_QDR_CTL_PIPELINE_0                      (0)
#define     NFP_QDR_CTL_PIPELINE_1                      (1)
#define     NFP_QDR_CTL_PIPELINE_2                      (2)
#define     NFP_QDR_CTL_PIPELINE_3                      (3)
#define     NFP_QDR_CTL_PIPELINE_4                      (4)
#define   NFP_QDR_CTL_SRAM_SIZE(_x)                     (((_x) & 0x7) << 7)
#define   NFP_QDR_CTL_SRAM_SIZE_of(_x)                  (((_x) >> 7) & 0x7)
#define     NFP_QDR_CTL_SRAM_SIZE_512KX18               (0)
#define     NFP_QDR_CTL_SRAM_SIZE_1MX18                 (1)
#define     NFP_QDR_CTL_SRAM_SIZE_2MX18                 (2)
#define     NFP_QDR_CTL_SRAM_SIZE_4MX18                 (3)
#define     NFP_QDR_CTL_SRAM_SIZE_8MX18                 (4)
#define     NFP_QDR_CTL_SRAM_SIZE_16MX18                (5)
#define     NFP_QDR_CTL_SRAM_SIZE_32MX18                (6)
#define     NFP_QDR_CTL_SRAM_SIZE_EXTERNAL              (7)
#define   NFP_QDR_CTL_UNUSED_SPARE                      (0x1 << 6)
#define   NFP_QDR_CTL_PORT_CTL(_x)                      (((_x) & 0x3) << 4)
#define   NFP_QDR_CTL_PORT_CTL_of(_x)                   (((_x) >> 4) & 0x3)
#define   NFP_QDR_CTL_PAR_EN                            (0x1 << 3)
#define   NFP_QDR_CTL_SRAM_ARB_CONTROL(_x)              ((_x) & 0x7)
#define   NFP_QDR_CTL_SRAM_ARB_CONTROL_of(_x)           ((_x) & 0x7)
/* Register Type: QDRSramParityStatus1 */
#define NFP_QDR_PAR_STATUS_1           0x0004
#define   NFP_QDR_PAR_STATUS_1_WW_PAR(_x)               (((_x) & 0xf) << 28)
#define   NFP_QDR_PAR_STATUS_1_WW_PAR_of(_x)            (((_x) >> 28) & 0xf)
#define     NFP_QDR_PAR_STATUS_1_WW_PAR_OFF             (0)
#define     NFP_QDR_PAR_STATUS_1_WW_PAR_ON              (1)
#define   NFP_QDR_PAR_STATUS_1_ADDRESS_of(_x)           ((_x) & 0xffffff)
/* Register Type: QDRSramParityStatus2 */
#define NFP_QDR_PAR_STATUS_2           0x0008
#define   NFP_QDR_PAR_STATUS_2_MULT_ERR                 (0x1 << 31)
#define   NFP_QDR_PAR_STATUS_2_ME_of(_x)                (((_x) >> 24) & 0x7f)
#define   NFP_QDR_PAR_STATUS_2_THD_of(_x)               (((_x) >> 17) & 0x7)
#define   NFP_QDR_PAR_STATUS_2_SRC                      (0x1 << 16)
#define   NFP_QDR_PAR_STATUS_2_ERROR(_x)                ((_x) & 0xf)
#define   NFP_QDR_PAR_STATUS_2_ERROR_of(_x)             ((_x) & 0xf)
/* Register Type: QDRArm11InterruptSelect */
#define NFP_QDR_INT                    0x0100
#define   NFP_QDR_INT_ACIO_FSM_DONE_INTR_SEL            (0x1)
#define     NFP_QDR_INT_ACIO_FSM_DONE_INTR_SEL_OFF      (0x0)
#define     NFP_QDR_INT_ACIO_FSM_DONE_INTR_SEL_ON       (0x1)
/* Register Type: QDRMiSignature3 */
#define NFP_QDR_MI_SIG3                0x0104
#define   NFP_QDR_MI_SIG3_MISR_RX(_x)                   (_x)
#define   NFP_QDR_MI_SIG3_MISR_RX_of(_x)                (_x)
/* Register Type: QDRInternalPipeline */
#define NFP_QDR_PIPE                   0x0108
#define   NFP_QDR_PIPE_RD_PIPE_LAT(_x)                  ((_x) & 0x3)
#define   NFP_QDR_PIPE_RD_PIPE_LAT_of(_x)               ((_x) & 0x3)
#define     NFP_QDR_PIPE_RD_PIPE_LAT_SUB_ONE            (0)
#define     NFP_QDR_PIPE_RD_PIPE_LAT_ZERO               (1)
#define     NFP_QDR_PIPE_RD_PIPE_LAT_ADD_ONE            (2)
/* Register Type: QDRDftControl */
#define NFP_QDR_DFT                    0x0200
#define   NFP_QDR_DFT_FIXED_DELAY_SEL                   (0x1 << 23)
#define     NFP_QDR_DFT_FIXED_DELAY_SEL_NO_DELAY        (0x0)
#define     NFP_QDR_DFT_FIXED_DELAY_SEL_DELAYED         (0x800000)
#define   NFP_QDR_DFT_SLAVE_DLL_TAP_DLY_SEL             (0x1 << 22)
#define     NFP_QDR_DFT_SLAVE_DLL_TAP_DLY_SEL_320       (0x0)
#define     NFP_QDR_DFT_SLAVE_DLL_TAP_DLY_SEL_1000      (0x400000)
#define   NFP_QDR_DFT_SGXPA_RXCLK_DELAY_ADJUST_SEL_RSH  (0x1 << 21)
#define   NFP_QDR_DFT_MISR_EN                           (0x1 << 20)
#define     NFP_QDR_DFT_MISR_EN_DIS                     (0x0)
#define     NFP_QDR_DFT_MISR_EN_EN                      (0x100000)
#define   NFP_QDR_DFT_DATA_SOURCE_SEL                   (0x1 << 19)
#define     NFP_QDR_DFT_DATA_SOURCE_SEL_CORE            (0x0)
#define     NFP_QDR_DFT_DATA_SOURCE_SEL_PATGEN          (0x80000)
#define   NFP_QDR_DFT_PATGEN_CTL_EN                     (0x1 << 18)
#define     NFP_QDR_DFT_PATGEN_CTL_EN_OFF               (0x0)
#define     NFP_QDR_DFT_PATGEN_CTL_EN_ON                (0x40000)
#define   NFP_QDR_DFT_PATGEN_LFSR_MODE                  (0x1 << 17)
#define     NFP_QDR_DFT_PATGEN_LFSR_MODE_CIRCULAR       (0x0)
#define     NFP_QDR_DFT_PATGEN_LFSR_MODE_LFSR           (0x20000)
#define   NFP_QDR_DFT_PATGEN_EN                         (0x1 << 16)
#define     NFP_QDR_DFT_PATGEN_EN_DIS                   (0x0)
#define     NFP_QDR_DFT_PATGEN_EN_EN                    (0x10000)
#define   NFP_QDR_DFT_IOLB_GROUP_SEL(_x)                (((_x) & 0x3) << 14)
#define   NFP_QDR_DFT_IOLB_GROUP_SEL_of(_x)             (((_x) >> 14) & 0x3)
#define     NFP_QDR_DFT_IOLB_GROUP_SEL_0                (0)
#define     NFP_QDR_DFT_IOLB_GROUP_SEL_1                (1)
#define     NFP_QDR_DFT_IOLB_GROUP_SEL_2                (2)
#define     NFP_QDR_DFT_IOLB_GROUP_SEL_3                (3)
#define   NFP_QDR_DFT_LOW_PWR_MODE                      (0x1 << 13)
#define     NFP_QDR_DFT_LOW_PWR_MODE_NORMAL             (0x0)
#define     NFP_QDR_DFT_LOW_PWR_MODE_LOW                (0x2000)
#define   NFP_QDR_DFT_RX_DLL_OVERRIDE                   (0x1 << 12)
#define     NFP_QDR_DFT_RX_DLL_OVERRIDE_AUTO            (0x0)
#define     NFP_QDR_DFT_RX_DLL_OVERRIDE_RX_DLL          (0x1000)
#define   NFP_QDR_DFT_DLL_BYP                           (0x1 << 9)
#define     NFP_QDR_DFT_DLL_BYP_DLL                     (0x0)
#define     NFP_QDR_DFT_DLL_BYP_DIRECT                  (0x200)
#define   NFP_QDR_DFT_RXCLK_LB_EN                       (0x1 << 8)
#define     NFP_QDR_DFT_RXCLK_LB_EN_DIS                 (0x0)
#define     NFP_QDR_DFT_RXCLK_LB_EN_EN                  (0x100)
#define   NFP_QDR_DFT_OUT_ODT_EN                        (0x1 << 7)
#define     NFP_QDR_DFT_OUT_ODT_EN_DIS                  (0x0)
#define     NFP_QDR_DFT_OUT_ODT_EN_EN                   (0x80)
#define   NFP_QDR_DFT_SDFT_LBK_SDMIFFEN_EN_RSH          (0x1 << 2)
#define     NFP_QDR_DFT_SDFT_LBK_SDMIFFEN_EN_RSH_DIS    (0x0)
#define     NFP_QDR_DFT_SDFT_LBK_SDMIFFEN_EN_RSH_EN     (0x4)
#define   NFP_QDR_DFT_SGX_Q_PWR_SAVE_EN_RSH             (0x1 << 1)
#define     NFP_QDR_DFT_SGX_Q_PWR_SAVE_EN_RSH_DIS       (0x0)
#define     NFP_QDR_DFT_SGX_Q_PWR_SAVE_EN_RSH_EN        (0x2)
#define   NFP_QDR_DFT_IO_LB_EN                          (0x1)
#define     NFP_QDR_DFT_IO_LB_EN_DIS                    (0x0)
#define     NFP_QDR_DFT_IO_LB_EN_EN                     (0x1)
/* Register Type: QDRPatternGeneratorSeed */
#define NFP_QDR_PATGEN_SEED            0x0204
#define   NFP_QDR_PATGEN_SEED_PATGEN_1_SEED(_x)         (((_x) & 0xffff) << 16)
#define   NFP_QDR_PATGEN_SEED_PATGEN_1_SEED_of(_x)      (((_x) >> 16) & 0xffff)
#define   NFP_QDR_PATGEN_SEED_PATGEN_0_SEED(_x)         ((_x) & 0xffff)
#define   NFP_QDR_PATGEN_SEED_PATGEN_0_SEED_of(_x)      ((_x) & 0xffff)
/* Register Type: QDRPatternGeneratorEvenMuxControl */
#define NFP_QDR_PATGEN_MUX_EVEN        0x0208
#define   NFP_QDR_PATGEN_MUX_EVEN_PATGEN_MUX_SEL_EVEN(_x) ((_x) & 0xffffff)
#define   NFP_QDR_PATGEN_MUX_EVEN_PATGEN_MUX_SEL_EVEN_of(_x) ((_x) & 0xffffff)
#define     NFP_QDR_PATGEN_MUX_EVEN_PATGEN_MUX_SEL_EVEN_PATGEN_0 (0)
#define     NFP_QDR_PATGEN_MUX_EVEN_PATGEN_MUX_SEL_EVEN_PATGEN_1 (1)
/* Register Type: QDRPatternGeneratorOddMuxControl */
#define NFP_QDR_PATGEN_MUX_ODD         0x020c
#define   NFP_QDR_PATGEN_MUX_ODD_PATGEN_MUX_SEL_ODD(_x) ((_x) & 0xffffff)
#define   NFP_QDR_PATGEN_MUX_ODD_PATGEN_MUX_SEL_ODD_of(_x) ((_x) & 0xffffff)
#define     NFP_QDR_PATGEN_MUX_ODD_PATGEN_MUX_SEL_ODD_PATGEN_0 (0)
#define     NFP_QDR_PATGEN_MUX_ODD_PATGEN_MUX_SEL_ODD_PATGEN_1 (1)
/* Register Type: QDRPatternGeneratorLoad */
#define NFP_QDR_PATGEN_LOAD            0x0210
#define   NFP_QDR_PATGEN_LOAD_PATGEN_LOAD_NSHIFT        (0x1)
/* Register Type: QDRAcioLoopbackControl0 */
#define NFP_QDR_ACIO_CTL0              0x0214
#define   NFP_QDR_ACIO_CTL0_FSM_START                   (0x1)
/* Register Type: QDRAcioLoopbackControl1 */
#define NFP_QDR_ACIO_CTL1              0x0218
#define   NFP_QDR_ACIO_CTL1_FSM_DONE                    (0x1)
/* Register Type: QDRAcioLoopbackControl2 */
#define NFP_QDR_ACIO_CTL2              0x021c
#define   NFP_QDR_ACIO_CTL2_BURST_COUNT(_x)             (((_x) & 0xff) << 23)
#define   NFP_QDR_ACIO_CTL2_BURST_COUNT_of(_x)          (((_x) >> 23) & 0xff)
#define   NFP_QDR_ACIO_CTL2_LATENCY_COUNT(_x)           (((_x) & 0x1f) << 18)
#define   NFP_QDR_ACIO_CTL2_LATENCY_COUNT_of(_x)        (((_x) >> 18) & 0x1f)
#define   NFP_QDR_ACIO_CTL2_DLL_CTR_SEL                 (0x1 << 17)
#define   NFP_QDR_ACIO_CTL2_DLL_UP_DOWN                 (0x1 << 16)
#define     NFP_QDR_ACIO_CTL2_DLL_UP_DOWN_UP            (0x0)
#define     NFP_QDR_ACIO_CTL2_DLL_UP_DOWN_DOWN          (0x10000)
#define   NFP_QDR_ACIO_CTL2_DLL_STOP_VALUE(_x)          (((_x) & 0xff) << 8)
#define   NFP_QDR_ACIO_CTL2_DLL_STOP_VALUE_of(_x)       (((_x) >> 8) & 0xff)
#define   NFP_QDR_ACIO_CTL2_DLL_START_VALUE(_x)         ((_x) & 0xff)
#define   NFP_QDR_ACIO_CTL2_DLL_START_VALUE_of(_x)      ((_x) & 0xff)
/* Register Type: QDRAcioLoopbackControl3 */
#define NFP_QDR_ACIO_CTL3              0x0220
#define   NFP_QDR_ACIO_CTL3_SWAP                        (0x1 << 15)
#define   NFP_QDR_ACIO_CTL3_COMP_OFFSET_SEL_1(_x)       (((_x) & 0x7) << 12)
#define   NFP_QDR_ACIO_CTL3_COMP_OFFSET_SEL_1_of(_x)    (((_x) >> 12) & 0x7)
#define   NFP_QDR_ACIO_CTL3_ACIO_ERR_INV_SEL            (0x1 << 11)
#define     NFP_QDR_ACIO_CTL3_ACIO_ERR_INV_SEL_NORMAL   (0x0)
#define     NFP_QDR_ACIO_CTL3_ACIO_ERR_INV_SEL_INVERTED (0x800)
#define   NFP_QDR_ACIO_CTL3_FF_NAF_SEL                  (0x1 << 10)
#define     NFP_QDR_ACIO_CTL3_FF_NAF_SEL_ALL_FAIL       (0x0)
#define     NFP_QDR_ACIO_CTL3_FF_NAF_SEL_FIRST_FAIL     (0x400)
#define   NFP_QDR_ACIO_CTL3_ODD_COMPARE_EN              (0x1 << 9)
#define     NFP_QDR_ACIO_CTL3_ODD_COMPARE_EN_MASK_ODD   (0x0)
#define     NFP_QDR_ACIO_CTL3_ODD_COMPARE_EN_ENABLE_ODD (0x200)
#define   NFP_QDR_ACIO_CTL3_EVEN_COMPARE_EN             (0x1 << 8)
#define     NFP_QDR_ACIO_CTL3_EVEN_COMPARE_EN_MASK_EVEN (0x0)
#define     NFP_QDR_ACIO_CTL3_EVEN_COMPARE_EN_ENABLE_EVEN (0x100)
#define   NFP_QDR_ACIO_CTL3_MASK_PINSET_ERR_SET_C       (0x1 << 7)
#define     NFP_QDR_ACIO_CTL3_MASK_PINSET_ERR_SET_C_UNMASKED (0x0)
#define     NFP_QDR_ACIO_CTL3_MASK_PINSET_ERR_SET_C_MASKED (0x80)
#define   NFP_QDR_ACIO_CTL3_MASK_PINSET_ERR_SET_B       (0x1 << 6)
#define     NFP_QDR_ACIO_CTL3_MASK_PINSET_ERR_SET_B_UNMASKED (0x0)
#define     NFP_QDR_ACIO_CTL3_MASK_PINSET_ERR_SET_B_MASKED (0x40)
#define   NFP_QDR_ACIO_CTL3_MASK_PINSET_ERR_SET_A       (0x1 << 5)
#define     NFP_QDR_ACIO_CTL3_MASK_PINSET_ERR_SET_A_UNMASKED (0x0)
#define     NFP_QDR_ACIO_CTL3_MASK_PINSET_ERR_SET_A_MASKED (0x20)
#define   NFP_QDR_ACIO_CTL3_COMP_SKIP_SEL(_x)           (((_x) & 0x3) << 3)
#define   NFP_QDR_ACIO_CTL3_COMP_SKIP_SEL_of(_x)        (((_x) >> 3) & 0x3)
#define     NFP_QDR_ACIO_CTL3_COMP_SKIP_SEL_NONE        (0)
#define     NFP_QDR_ACIO_CTL3_COMP_SKIP_SEL_2           (1)
#define     NFP_QDR_ACIO_CTL3_COMP_SKIP_SEL_4           (2)
#define     NFP_QDR_ACIO_CTL3_COMP_SKIP_SEL_8           (3)
#define   NFP_QDR_ACIO_CTL3_COMP_OFFSET_SEL_0(_x)       ((_x) & 0x7)
#define   NFP_QDR_ACIO_CTL3_COMP_OFFSET_SEL_0_of(_x)    ((_x) & 0x7)
#define     NFP_QDR_ACIO_CTL3_COMP_OFFSET_SEL_0_PATGEN_2 (0)
#define     NFP_QDR_ACIO_CTL3_COMP_OFFSET_SEL_0_PATGEN_3 (1)
#define     NFP_QDR_ACIO_CTL3_COMP_OFFSET_SEL_0_PATGEN_4 (2)
#define     NFP_QDR_ACIO_CTL3_COMP_OFFSET_SEL_0_PATGEN_5 (3)
#define     NFP_QDR_ACIO_CTL3_COMP_OFFSET_SEL_0_PATGEN_6 (4)
#define     NFP_QDR_ACIO_CTL3_COMP_OFFSET_SEL_0_PATGEN_7 (5)
#define     NFP_QDR_ACIO_CTL3_COMP_OFFSET_SEL_0_PATGEN_8 (6)
#define     NFP_QDR_ACIO_CTL3_COMP_OFFSET_SEL_0_PATGEN_9 (7)
/* Register Type: QDRAcioLoopBackFailDLLVal */
#define NFP_QDR_ACIO_CTL4              0x0224
#define   NFP_QDR_ACIO_CTL4_ACIO_FULL_DLL_VALUE(_x)     (_x)
#define   NFP_QDR_ACIO_CTL4_ACIO_FULL_DLL_VALUE_of(_x)  (_x)
/* Register Type: QDRReceiveDllDlySelect */
#define NFP_QDR_RX_DELAY               0x0228
#define   NFP_QDR_RX_DELAY_SLAVE_DLL_DLY_SEL3(_x)       (((_x) & 0xff) << 24)
#define   NFP_QDR_RX_DELAY_SLAVE_DLL_DLY_SEL3_of(_x)    (((_x) >> 24) & 0xff)
#define   NFP_QDR_RX_DELAY_SLAVE_DLL_DLY_SEL2(_x)       (((_x) & 0xff) << 16)
#define   NFP_QDR_RX_DELAY_SLAVE_DLL_DLY_SEL2_of(_x)    (((_x) >> 16) & 0xff)
#define   NFP_QDR_RX_DELAY_SLAVE_DLL_DLY_SEL1(_x)       (((_x) & 0xff) << 8)
#define   NFP_QDR_RX_DELAY_SLAVE_DLL_DLY_SEL1_of(_x)    (((_x) >> 8) & 0xff)
#define   NFP_QDR_RX_DELAY_SLAVE_DLL_DLY_SEL0(_x)       ((_x) & 0xff)
#define   NFP_QDR_RX_DELAY_SLAVE_DLL_DLY_SEL0_of(_x)    ((_x) & 0xff)
/* Register Type: QDRAcioLoopbackError0 */
#define NFP_QDR_ACIO_LOOP_ERR0         0x022c
#define   NFP_QDR_ACIO_LOOP_ERR0_ACIOLB_ERROR_EVEN(_x)  ((_x) & 0xffffff)
#define   NFP_QDR_ACIO_LOOP_ERR0_ACIOLB_ERROR_EVEN_of(_x) ((_x) & 0xffffff)
/* Register Type: QDRAcioLoopbackError1 */
#define NFP_QDR_ACIO_LOOP_ERR1         0x0230
#define   NFP_QDR_ACIO_LOOP_ERR1_ACIOLB_ERROR_ODD(_x)   ((_x) & 0xffffff)
#define   NFP_QDR_ACIO_LOOP_ERR1_ACIOLB_ERROR_ODD_of(_x) ((_x) & 0xffffff)
/* Register Type: QDRMiSignature0 */
#define NFP_QDR_MI_SIG0                0x0234
#define   NFP_QDR_MI_SIG0_MISR_TX_0_W(_x)               (_x)
#define   NFP_QDR_MI_SIG0_MISR_TX_0_W_of(_x)            (_x)
/* Register Type: QDRMiSignature1 */
#define NFP_QDR_MI_SIG1                0x0238
#define   NFP_QDR_MI_SIG1_MISR_TX_1_W(_x)               (_x)
#define   NFP_QDR_MI_SIG1_MISR_TX_1_W_of(_x)            (_x)
/* Register Type: QDRMiSignature2 */
#define NFP_QDR_MI_SIG2                0x023c
#define   NFP_QDR_MI_SIG2_MISR_TX_2_W(_x)               (_x)
#define   NFP_QDR_MI_SIG2_MISR_TX_2_W_of(_x)            (_x)
/* Register Type: QDRReadPointerOffset */
#define NFP_QDR_READ_PTR               0x0240
#define   NFP_QDR_READ_PTR_QDR_TYPE_SEL                 (0x1 << 2)
#define     NFP_QDR_READ_PTR_QDR_TYPE_SEL_QDRII         (0x0)
#define     NFP_QDR_READ_PTR_QDR_TYPE_SEL_QDRI          (0x4)
#define   NFP_QDR_READ_PTR_RD_WR_PTR_OFFSET_SEL(_x)     ((_x) & 0x3)
#define   NFP_QDR_READ_PTR_RD_WR_PTR_OFFSET_SEL_of(_x)  ((_x) & 0x3)
#define     NFP_QDR_READ_PTR_RD_WR_PTR_OFFSET_SEL_6_CYCLE (0)
#define     NFP_QDR_READ_PTR_RD_WR_PTR_OFFSET_SEL_7_CYCLE (1)
#define     NFP_QDR_READ_PTR_RD_WR_PTR_OFFSET_SEL_5_CYCLE_INV (2)
#define     NFP_QDR_READ_PTR_RD_WR_PTR_OFFSET_SEL_6_CYCLE_INV (3)
/* Register Type: QDRReceiveDeskew */
#define NFP_QDR_RX_DESKEW              0x0244
#define   NFP_QDR_RX_DESKEW_DLL_FREQ_SEL(_x)            ((_x) & 0x1ff)
#define   NFP_QDR_RX_DESKEW_DLL_FREQ_SEL_of(_x)         ((_x) & 0x1ff)
/* Register Type: QDRAcioLoopbackExpectData */
#define NFP_QDR_ACIO_LOOP_EXPECTED     0x0248
#define   NFP_QDR_ACIO_LOOP_EXPECTED_SDFT_ACIOLB_EXPECT_DATA_FAIL_RSH_of(_x) ((_x) & 0xffffff)
/* Register Type: QDRAcioLoopbackReadData */
#define NFP_QDR_ACIO_LOOP_READ         0x024c
#define   NFP_QDR_ACIO_LOOP_READ_SDFT_ACIOLB_READ_DATA_FAIL_RSH_of(_x) ((_x) & 0xffffff)

#define NFP_XPB_QDR(x)			NFP_XPB_DEST(22, 1 + (x))
#define NFP_XPB_QDR0			NFP_XPB_QDR(0)
#define NFP_XPB_QDR1			NFP_XPB_QDR(1)
#define NFP_QDR_SIZE			SZ_4K

#define NFP_QDR_CHANNEL(chan)		(0x40000000 * ((chan) & 3))

#endif /* NFP3200_NFP_QDR_H */
