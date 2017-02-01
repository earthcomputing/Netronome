/*
 * Copyright (C) 2008-2011, Netronome Systems, Inc.  All rights reserved.
 *
 * Disclaimer: this file is provided without any warranty
 * as part of an early-access program, and the content is
 * bound to change before the final release.
 */

#ifndef NFP3200_NFP_XGM_H
#define NFP3200_NFP_XGM_H

/* HGID: nfp3200/xgm.desc = ade0020e9035 */
#define NFP_XGM(_x)                    (0x0 + (0x200 * ((_x) & 0x3)))
/* Register Type: XgmTxControl */
#define NFP_XGM_TX_CTRL                0x0000
#define   NFP_XGM_TX_CTRL_SEND_PAUSE                    (0x1 << 2)
#define   NFP_XGM_TX_CTRL_ZERO_PAUSE                    (0x1 << 1)
#define   NFP_XGM_TX_CTRL_ENABLE                        (0x1)
/* Register Type: XgmTxConfig */
#define NFP_XGM_TX_CFG                 0x0004
#define   NFP_XGM_TX_CFG_PREAMBLE_CRC(_x)               (((_x) & 0x3) << 8)
#define   NFP_XGM_TX_CFG_PREAMBLE_CRC_of(_x)            (((_x) >> 8) & 0x3)
#define     NFP_XGM_TX_CFG_PREAMBLE_CRC_CRC8            (0)
#define     NFP_XGM_TX_CFG_PREAMBLE_CRC_CRC0            (2)
#define     NFP_XGM_TX_CFG_PREAMBLE_CRC_CRC4            (3)
#define   NFP_XGM_TX_CFG_DISABLE_DIC                    (0x1 << 7)
#define   NFP_XGM_TX_CFG_DEC_IPG                        (0x1 << 6)
#define   NFP_XGM_TX_CFG_ENABLE_UNI_DIR                 (0x1 << 5)
#define   NFP_XGM_TX_CFG_MDIO_SPEED(_x)                 (((_x) & 0x7) << 2)
#define   NFP_XGM_TX_CFG_MDIO_SPEED_of(_x)              (((_x) >> 2) & 0x7)
#define     NFP_XGM_TX_CFG_MDIO_SPEED_DIV8              (0)
#define     NFP_XGM_TX_CFG_MDIO_SPEED_DIV16             (1)
#define     NFP_XGM_TX_CFG_MDIO_SPEED_DIV32             (2)
#define     NFP_XGM_TX_CFG_MDIO_SPEED_DIV48             (3)
#define     NFP_XGM_TX_CFG_MDIO_SPEED_DIV64             (4)
#define     NFP_XGM_TX_CFG_MDIO_SPEED_DIV96             (5)
#define     NFP_XGM_TX_CFG_MDIO_SPEED_DIV128            (6)
#define     NFP_XGM_TX_CFG_MDIO_SPEED_DIV224            (7)
#define   NFP_XGM_TX_CFG_STRETCH                        (0x1 << 1)
#define   NFP_XGM_TX_CFG_PAUSE                          (0x1)
/* Register Type: XgmTxPause */
#define NFP_XGM_TX_PAUSE               0x0008
#define   NFP_XGM_TX_PAUSE_T_VAL(_x)                    ((_x) & 0xffff)
#define   NFP_XGM_TX_PAUSE_T_VAL_of(_x)                 ((_x) & 0xffff)
/* Register Type: XgmRxControl */
#define NFP_XGM_RX_CTRL                0x000c
#define   NFP_XGM_RX_CTRL_EN_RX                         (0x1)
/* Register Type: XgmRxConfig */
#define NFP_XGM_RX_CFG                 0x0010
#define   NFP_XGM_RX_CFG_PREAMBLE_STRIPE(_x)            (((_x) & 0x3) << 16)
#define   NFP_XGM_RX_CFG_PREAMBLE_STRIPE_of(_x)         (((_x) >> 16) & 0x3)
#define   NFP_XGM_RX_CFG_LOCAL_FAULT                    (0x1 << 15)
#define   NFP_XGM_RX_CFG_REMOTE_FAULT                   (0x1 << 14)
#define   NFP_XGM_RX_CFG_LEN_ERR_DIS                    (0x1 << 13)
#define   NFP_XGM_RX_CFG_CONV_PREAMBLE                  (0x1 << 12)
#define   NFP_XGM_RX_CFG_ACC_PREAMBLE                   (0x1 << 11)
#define   NFP_XGM_RX_CFG_PASS_PREAMBLE                  (0x1 << 10)
#define   NFP_XGM_RX_CFG_DIS_PAUSE                      (0x1 << 9)
#define   NFP_XGM_RX_CFG_ACC_VLAN                       (0x1 << 8)
#define   NFP_XGM_RX_CFG_ACC_JUMBO                      (0x1 << 7)
#define   NFP_XGM_RX_CFG_FCS_REM                        (0x1 << 6)
#define   NFP_XGM_RX_CFG_DIS_NON_VLAN                   (0x1 << 5)
#define   NFP_XGM_RX_CFG_EN_EXT_MATCH                   (0x1 << 4)
#define   NFP_XGM_RX_CFG_EN_HASH_UNI                    (0x1 << 3)
#define   NFP_XGM_RX_CFG_EN_HASH_MULTI                  (0x1 << 2)
#define   NFP_XGM_RX_CFG_DIS_BROAD                      (0x1 << 1)
#define   NFP_XGM_RX_CFG_COPY_ALL                       (0x1)
/* Register Type: XgmHashLow */
#define NFP_XGM_RX_HASH_LO             0x0014
#define   NFP_XGM_RX_HASH_LO_XXX(_x)                    (_x)
#define   NFP_XGM_RX_HASH_LO_XXX_of(_x)                 (_x)
/* Register Type: XgmHashHigh */
#define NFP_XGM_RX_HASH_HI             0x0018
#define   NFP_XGM_RX_HASH_HI_XXX(_x)                    (_x)
#define   NFP_XGM_RX_HASH_HI_XXX_of(_x)                 (_x)
/* Register Type: XgmExactLow */
#define NFP_XGM_EXACT_MATCH_LO0        0x001c
#define NFP_XGM_EXACT_MATCH_LO1        0x0024
#define NFP_XGM_EXACT_MATCH_LO2        0x002c
#define NFP_XGM_EXACT_MATCH_LO3        0x0034
#define NFP_XGM_EXACT_MATCH_LO4        0x003c
#define NFP_XGM_EXACT_MATCH_LO5        0x0044
#define NFP_XGM_EXACT_MATCH_LO6        0x004c
#define NFP_XGM_EXACT_MATCH_LO7        0x0054
#define   NFP_XGM_EXACT_MATCH_LO_XXX(_x)                (_x)
#define   NFP_XGM_EXACT_MATCH_LO_XXX_of(_x)             (_x)
/* Register Type: XgmExactHigh */
#define NFP_XGM_EXACT_MATCH_HI0        0x0020
#define NFP_XGM_EXACT_MATCH_HI1        0x0028
#define NFP_XGM_EXACT_MATCH_HI2        0x0030
#define NFP_XGM_EXACT_MATCH_HI3        0x0038
#define NFP_XGM_EXACT_MATCH_HI4        0x0040
#define NFP_XGM_EXACT_MATCH_HI5        0x0048
#define NFP_XGM_EXACT_MATCH_HI6        0x0050
#define NFP_XGM_EXACT_MATCH_HI7        0x0058
#define   NFP_XGM_EXACT_MATCH_HI_XXX(_x)                ((_x) & 0xffff)
#define   NFP_XGM_EXACT_MATCH_HI_XXX_of(_x)             ((_x) & 0xffff)
/* Register Type: XgmTypeMatch */
#define NFP_XGM_TYPE_MATCH0            0x005c
#define NFP_XGM_TYPE_MATCH1            0x0060
#define NFP_XGM_TYPE_MATCH2            0x0064
#define NFP_XGM_TYPE_MATCH3            0x0068
#define   NFP_XGM_TYPE_MATCH_EN_CPY                     (0x1 << 31)
#define   NFP_XGM_TYPE_MATCH_TYPE(_x)                   ((_x) & 0xffff)
#define   NFP_XGM_TYPE_MATCH_TYPE_of(_x)                ((_x) & 0xffff)
/* Register Type: XgmIntStatus */
#define NFP_XGM_INT_STS                0x006c
#define   NFP_XGM_INT_STS_EXT_INT                       (0x1 << 10)
#define   NFP_XGM_INT_STS_LNK_FAULT                     (0x1 << 9)
#define   NFP_XGM_INT_STS_PHY_FRM_CMPL                  (0x1 << 8)
#define   NFP_XGM_INT_STS_PAUSE_FRM_TX                  (0x1 << 7)
#define   NFP_XGM_INT_STS_PAUSE_CNTR_TIMEOUT            (0x1 << 6)
#define   NFP_XGM_INT_STS_NON_ZERO_PAUSE                (0x1 << 5)
#define   NFP_XGM_INT_STS_CNTR_OVERFLOW                 (0x1 << 4)
#define   NFP_XGM_INT_STS_TX_ERR_FIFO                   (0x1 << 3)
#define   NFP_XGM_INT_STS_TX_UNDERFLOW                  (0x1 << 2)
#define   NFP_XGM_INT_STS_FRAME_TX                      (0x1 << 1)
#define   NFP_XGM_INT_STS_FRAME_RX                      (0x1)
/* Register Type: XgmIntMask */
#define NFP_XGM_INT_MASK               0x0070
#define   NFP_XGM_INT_MASK_EXT_INT                      (0x1 << 10)
#define   NFP_XGM_INT_MASK_LNK_FAULT                    (0x1 << 9)
#define   NFP_XGM_INT_MASK_PHY_FRM_CMPL                 (0x1 << 8)
#define   NFP_XGM_INT_MASK_PAUSE_FRM_TX                 (0x1 << 7)
#define   NFP_XGM_INT_MASK_PAUSE_CNTR_TIMEOUT           (0x1 << 6)
#define   NFP_XGM_INT_MASK_NON_ZERO_PAUSE               (0x1 << 5)
#define   NFP_XGM_INT_MASK_CNTR_OVERFLOW                (0x1 << 4)
#define   NFP_XGM_INT_MASK_TX_ERR_FIFO                  (0x1 << 3)
#define   NFP_XGM_INT_MASK_TX_UNDERFLOW                 (0x1 << 2)
#define   NFP_XGM_INT_MASK_FRAME_TX                     (0x1 << 1)
#define   NFP_XGM_INT_MASK_FRAME_RX                     (0x1)
/* Register Type: XgmIntEnable */
#define NFP_XGM_INT_ENABLE             0x0074
#define   NFP_XGM_INT_ENABLE_EXT_INT                    (0x1 << 10)
#define   NFP_XGM_INT_ENABLE_LNK_FAULT                  (0x1 << 9)
#define   NFP_XGM_INT_ENABLE_PHY_FRM_CMPL               (0x1 << 8)
#define   NFP_XGM_INT_ENABLE_PAUSE_FRM_TX               (0x1 << 7)
#define   NFP_XGM_INT_ENABLE_PAUSE_CNTR_TIMEOUT         (0x1 << 6)
#define   NFP_XGM_INT_ENABLE_NON_ZERO_PAUSE             (0x1 << 5)
#define   NFP_XGM_INT_ENABLE_CNTR_OVERFLOW              (0x1 << 4)
#define   NFP_XGM_INT_ENABLE_TX_ERR_FIFO                (0x1 << 3)
#define   NFP_XGM_INT_ENABLE_TX_UNDERFLOW               (0x1 << 2)
#define   NFP_XGM_INT_ENABLE_FRAME_TX                   (0x1 << 1)
#define   NFP_XGM_INT_ENABLE_FRAME_RX                   (0x1)
/* Register Type: XgmIntDisable */
#define NFP_XGM_INT_DISABLE            0x0078
#define   NFP_XGM_INT_DISABLE_EXT_INT                   (0x1 << 10)
#define   NFP_XGM_INT_DISABLE_LNK_FAULT                 (0x1 << 9)
#define   NFP_XGM_INT_DISABLE_PHY_FRM_CMPL              (0x1 << 8)
#define   NFP_XGM_INT_DISABLE_PAUSE_FRM_TX              (0x1 << 7)
#define   NFP_XGM_INT_DISABLE_PAUSE_CNTR_TIMEOUT        (0x1 << 6)
#define   NFP_XGM_INT_DISABLE_NON_ZERO_PAUSE            (0x1 << 5)
#define   NFP_XGM_INT_DISABLE_CNTR_OVERFLOW             (0x1 << 4)
#define   NFP_XGM_INT_DISABLE_TX_ERR_FIFO               (0x1 << 3)
#define   NFP_XGM_INT_DISABLE_TX_UNDERFLOW              (0x1 << 2)
#define   NFP_XGM_INT_DISABLE_FRAME_TX                  (0x1 << 1)
#define   NFP_XGM_INT_DISABLE_FRAME_RX                  (0x1)
/* Register Type: XgmPauseCount */
#define NFP_XGM_PAUSE_COUNT            0x007c
#define   NFP_XGM_PAUSE_COUNT_XXX_of(_x)                ((_x) & 0xffff)
/* Register Type: XgmStats */
#define NFP_XGM_STAT_CTRL              0x0080
#define   NFP_XGM_STAT_CTRL_CLEAR                       (0x1 << 2)
#define   NFP_XGM_STAT_CTRL_INC                         (0x1 << 1)
#define   NFP_XGM_STAT_CTRL_TST_MODE_WRITE_EN           (0x1)
/* Register Type: XgmMdioControl */
#define NFP_XGM_MDIO_CTRL              0x0084
#define   NFP_XGM_MDIO_CTRL_FLD30(_x)                   (((_x) & 0x3) << 30)
#define   NFP_XGM_MDIO_CTRL_FLD30_of(_x)                (((_x) >> 30) & 0x3)
#define   NFP_XGM_MDIO_CTRL_OPRTN(_x)                   (((_x) & 0x3) << 28)
#define   NFP_XGM_MDIO_CTRL_OPRTN_of(_x)                (((_x) >> 28) & 0x3)
#define     NFP_XGM_MDIO_CTRL_OPRTN_ADDR_FRAME          (0)
#define     NFP_XGM_MDIO_CTRL_OPRTN_WRITE               (1)
#define     NFP_XGM_MDIO_CTRL_OPRTN_POST_READ_ADDR_INC  (2)
#define     NFP_XGM_MDIO_CTRL_OPRTN_READ                (3)
#define   NFP_XGM_MDIO_CTRL_PORT_ADDR(_x)               (((_x) & 0x1f) << 23)
#define   NFP_XGM_MDIO_CTRL_PORT_ADDR_of(_x)            (((_x) >> 23) & 0x1f)
#define   NFP_XGM_MDIO_CTRL_DEV_ADDR(_x)                (((_x) & 0x1f) << 18)
#define   NFP_XGM_MDIO_CTRL_DEV_ADDR_of(_x)             (((_x) >> 18) & 0x1f)
#define   NFP_XGM_MDIO_CTRL_FLD16(_x)                   (((_x) & 0x3) << 16)
#define   NFP_XGM_MDIO_CTRL_FLD16_of(_x)                (((_x) >> 16) & 0x3)
#define   NFP_XGM_MDIO_CTRL_RAW_DATA(_x)                ((_x) & 0xffff)
#define   NFP_XGM_MDIO_CTRL_RAW_DATA_of(_x)             ((_x) & 0xffff)
/* Register Type: XgmModuleID */
#define NFP_XGM_MOD_ID                 0x00fc
#define   NFP_XGM_MOD_ID_MOD_ID_of(_x)                  (((_x) >> 16) & 0xffff)
#define   NFP_XGM_MOD_ID_MOD_REV_of(_x)                 ((_x) & 0xffff)
/* Register Type: XgmStOctetsLow */
#define NFP_XGM_STAT_TX_OCTETS_LO      0x0100
#define NFP_XGM_STAT_RX_OCTETS_LO      0x013c
#define   NFP_XGM_STAT_OCTETS_LO_of(_x)                 (_x)
/* Register Type: XgmStOctetsHigh */
#define NFP_XGM_STAT_TX_OCTETS_HI      0x0104
#define NFP_XGM_STAT_RX_OCTETS_HI      0x0140
#define   NFP_XGM_STAT_OCTETS_HI_of(_x)                 ((_x) & 0x1fff)
/* Register Type: XgmStFramesLow */
#define NFP_XGM_STAT_TX_FRAMES_LO      0x0108
#define NFP_XGM_STAT_RX_FRAMES_LO      0x0144
#define   NFP_XGM_STAT_FRAMES_LO_of(_x)                 (_x)
/* Register Type: XgmStFramesHigh */
#define NFP_XGM_STAT_TX_FRAMES_HI      0x010c
#define NFP_XGM_STAT_RX_FRAMES_HI      0x0148
#define   NFP_XGM_STAT_FRAMES_HI_of(_x)                 ((_x) & 0xf)
/* Register Type: XgmStFrames */
#define NFP_XGM_STAT_TX_BCAST          0x0110
#define NFP_XGM_STAT_TX_MCAST          0x0114
#define NFP_XGM_STAT_TX_SZ_64          0x011c
#define NFP_XGM_STAT_TX_SZ_127         0x0120
#define NFP_XGM_STAT_TX_SZ_255         0x0124
#define NFP_XGM_STAT_TX_SZ_511         0x0128
#define NFP_XGM_STAT_TX_SZ_1023        0x012c
#define NFP_XGM_STAT_TX_SZ_1518        0x0130
#define NFP_XGM_STAT_TX_SZ_MAX         0x0134
#define NFP_XGM_STAT_RX_BCAST          0x014c
#define NFP_XGM_STAT_RX_MCAST          0x0150
#define NFP_XGM_STAT_RX_SZ_64          0x0158
#define NFP_XGM_STAT_RX_SZ_127         0x015c
#define NFP_XGM_STAT_RX_SZ_255         0x0160
#define NFP_XGM_STAT_RX_SZ_511         0x0164
#define NFP_XGM_STAT_RX_SZ_1023        0x0168
#define NFP_XGM_STAT_RX_SZ_1518        0x016c
#define NFP_XGM_STAT_RX_SZ_MAX         0x0170
#define   NFP_XGM_STAT_FRAMES_of(_x)                    (_x)
/* Register Type: XgmStFrames16 */
#define NFP_XGM_STAT_TX_PAUSE          0x0118
#define NFP_XGM_STAT_TX_ERR            0x0138
#define NFP_XGM_STAT_RX_PAUSE          0x0154
#define NFP_XGM_STAT_RX_SHORT          0x0174
#define NFP_XGM_STAT_RX_LARGE          0x0178
#define NFP_XGM_STAT_RX_JABBER         0x017c
#define NFP_XGM_STAT_RX_ERR            0x0180
#define NFP_XGM_STAT_RX_LEN            0x0184
#define NFP_XGM_STAT_RX_SYM            0x0188
#define   NFP_XGM_STAT_FRAMES16_of(_x)                  ((_x) & 0xffff)

/* XAUI 1 and 2 XGM/SGMII Config base */
#define NFP_XPB_XAUI_CFG0			NFP_XPB_DEST(16, 17)
#define NFP_XPB_XAUI_CFG1			NFP_XPB_DEST(18, 17)

#endif /* NFP3200_NFP_UART_H */
