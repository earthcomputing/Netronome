/*
 * Copyright (C) 2008-2011, Netronome Systems, Inc.  All rights reserved.
 *
 * Disclaimer: this file is provided without any warranty
 * as part of an early-access program, and the content is
 * bound to change before the final release.
 */

#ifndef NFP3200_NFP_MSF_H
#define NFP3200_NFP_MSF_H

/* HGID: nfp3200/msf.desc = ade0020e9035 */
/* Register Type: MsfRxCtrl */
#define NFP_MSF_RX_CONTROL             0x0000
#define   NFP_MSF_RX_CTL_EN_S                           (0x1 << 30)
#define   NFP_MSF_RX_CTL_SPI4_CSUM_MODE                 (0x1 << 27)
#define     NFP_MSF_RX_CTL_SPI4_CSUM_MODE_1COMPL        (0x0)
#define     NFP_MSF_RX_CTL_SPI4_CSUM_MODE_CSUM          (0x8000000)
#define   NFP_MSF_RX_CTL_DATA_DIP4_DIS                  (0x1 << 26)
#define     NFP_MSF_RX_CTL_DATA_DIP4_DIS_CHECK          (0x0)
#define     NFP_MSF_RX_CTL_DATA_DIP4_DIS_INGORE         (0x4000000)
#define   NFP_MSF_RX_CTL_FLWCTL_VPAR_TYPE               (0x1 << 25)
#define     NFP_MSF_RX_CTL_FLWCTL_VPAR_TYPE_VERT        (0x0)
#define     NFP_MSF_RX_CTL_FLWCTL_VPAR_TYPE_DIP16       (0x2000000)
#define   NFP_MSF_RX_CTL_FLWCTL_VPAR_DIS                (0x1 << 23)
#define     NFP_MSF_RX_CTL_FLWCTL_VPAR_DIS_CALC         (0x0)
#define     NFP_MSF_RX_CTL_FLWCTL_VPAR_DIS_IGNORE       (0x800000)
#define   NFP_MSF_RX_CTL_FLWCTL_HPAR_DIS                (0x1 << 21)
#define     NFP_MSF_RX_CTL_FLWCTL_HPAR_DIS_CALC         (0x0)
#define     NFP_MSF_RX_CTL_FLWCTL_HPAR_DIS_IGNORE       (0x200000)
#define   NFP_MSF_RX_CTL_RSTAT_OV_VALUE(_x)             (((_x) & 0x3) << 18)
#define   NFP_MSF_RX_CTL_RSTAT_OV_VALUE_of(_x)          (((_x) >> 18) & 0x3)
#define   NFP_MSF_RX_CTL_RSTAT_OVERRIDE                 (0x1 << 17)
#define   NFP_MSF_RX_CTL_DUPLEX_MODE                    (0x1 << 16)
#define     NFP_MSF_RX_CTL_DUPLEX_MODE_SIMPLEX          (0x0)
#define     NFP_MSF_RX_CTL_DUPLEX_MODE_DUPLEX           (0x10000)
#define   NFP_MSF_RX_CTL_RSTAT_CLOCK                    (0x1 << 13)
#define     NFP_MSF_RX_CTL_RSTAT_CLOCK_FALLING          (0x0)
#define     NFP_MSF_RX_CTL_RSTAT_CLOCK_RISING           (0x2000)
#define   NFP_MSF_RX_CTL_RSTAT_SELECT                   (0x1 << 12)
#define     NFP_MSF_RX_CTL_RSTAT_SELECT_LVTTL           (0x0)
#define     NFP_MSF_RX_CTL_RSTAT_SELECT_LVDS            (0x1000)
#define   NFP_MSF_RX_CTL_RX_CAL_MODE                    (0x1 << 11)
#define     NFP_MSF_RX_CTL_RX_CAL_MODE_CON              (0x0)
#define     NFP_MSF_RX_CTL_RX_CAL_MODE_OVRD             (0x800)
#define   NFP_MSF_RX_CTL_RBUF_ELEM_SIZE_0(_x)           (((_x) & 0x3) << 2)
#define   NFP_MSF_RX_CTL_RBUF_ELEM_SIZE_0_of(_x)        (((_x) >> 2) & 0x3)
#define     NFP_MSF_RX_CTL_RBUF_ELEM_SIZE_0_64          (0)
#define     NFP_MSF_RX_CTL_RBUF_ELEM_SIZE_0_128         (1)
#define     NFP_MSF_RX_CTL_RBUF_ELEM_SIZE_0_256         (2)
#define   NFP_MSF_RX_CTL_RBUF_PART(_x)                  ((_x) & 0x3)
#define   NFP_MSF_RX_CTL_RBUF_PART_of(_x)               ((_x) & 0x3)
#define     NFP_MSF_RX_CTL_RBUF_PART_1_WAY              (0)
/* Register Type: MsfTxCtrl */
#define NFP_MSF_TX_CONTROL             0x0004
#define   NFP_MSF_TX_CTL_EN_S                           (0x1 << 29)
#define   NFP_MSF_TX_CTL_FLUSH_PAR0                     (0x1 << 24)
#define   NFP_MSF_TX_CTL_STS_MODE_UPDATE                (0x1 << 22)
#define     NFP_MSF_TX_CTL_STS_MODE_UPDATE_ANY          (0x0)
#define     NFP_MSF_TX_CTL_STS_MODE_UPDATE_TSTAT        (0x400000)
#define   NFP_MSF_TX_CTL_STS_READ_MODE                  (0x1 << 21)
#define     NFP_MSF_TX_CTL_STS_READ_MODE_ILLEGAL        (0x0)
#define     NFP_MSF_TX_CTL_STS_READ_MODE_SATISFIED      (0x200000)
#define   NFP_MSF_TX_CTL_CCLK_SRC(_x)                   (((_x) & 0x3) << 19)
#define   NFP_MSF_TX_CTL_CCLK_SRC_of(_x)                (((_x) >> 19) & 0x3)
#define     NFP_MSF_TX_CTL_CCLK_SRC_RCLK                (0)
#define     NFP_MSF_TX_CTL_CCLK_SRC_TCLK                (1)
#define     NFP_MSF_TX_CTL_CCLK_SRC_DIV                 (2)
#define   NFP_MSF_TX_CTL_FLWCTL_VPAR_TYPE               (0x1 << 18)
#define     NFP_MSF_TX_CTL_FLWCTL_VPAR_TYPE_VERT        (0x0)
#define     NFP_MSF_TX_CTL_FLWCTL_VPAR_TYPE_DIP16       (0x40000)
#define   NFP_MSF_TX_CTL_DUPLEX_MODE                    (0x1 << 16)
#define     NFP_MSF_TX_CTL_DUPLEX_MODE_SIMPLEX          (0x0)
#define     NFP_MSF_TX_CTL_DUPLEX_MODE_DUPLEX           (0x10000)
#define   NFP_MSF_TX_CTL_TSTAT_CLOCK                    (0x1 << 13)
#define     NFP_MSF_TX_CTL_TSTAT_CLOCK_FALLING          (0x0)
#define     NFP_MSF_TX_CTL_TSTAT_CLOCK_RISING           (0x2000)
#define   NFP_MSF_TX_CTL_TSTAT_SELECT                   (0x1 << 12)
#define     NFP_MSF_TX_CTL_TSTAT_SELECT_LVTTL           (0x0)
#define     NFP_MSF_TX_CTL_TSTAT_SELECT_LVDS            (0x1000)
#define   NFP_MSF_TX_CTL_CLK_SOURCE                     (0x1 << 11)
#define     NFP_MSF_TX_CTL_CLK_SOURCE_TCLK              (0x0)
#define     NFP_MSF_TX_CTL_CLK_SOURCE_DIV               (0x800)
#define   NFP_MSF_TX_CTL_ENABLE                         (0x1 << 10)
#define     NFP_MSF_TX_CTL_ENABLE_LOW                   (0x0)
#define     NFP_MSF_TX_CTL_ENABLE_NORMAL                (0x400)
#define   NFP_MSF_TX_CTL_IDLE                           (0x1 << 9)
#define     NFP_MSF_TX_CTL_IDLE_SPI4                    (0x0)
#define     NFP_MSF_TX_CTL_IDLE_CSIX                    (0x200)
#define   NFP_MSF_TX_CTL_TSTAT_EN                       (0x1 << 8)
#define     NFP_MSF_TX_CTL_TSTAT_EN_IGNORE              (0x0)
#define     NFP_MSF_TX_CTL_TSTAT_EN_FIFO                (0x100)
#define   NFP_MSF_TX_CTL_TBUF_ELEM_SIZE_0(_x)           (((_x) & 0x3) << 2)
#define   NFP_MSF_TX_CTL_TBUF_ELEM_SIZE_0_of(_x)        (((_x) >> 2) & 0x3)
#define     NFP_MSF_TX_CTL_TBUF_ELEM_SIZE_0_64          (0)
#define     NFP_MSF_TX_CTL_TBUF_ELEM_SIZE_0_128         (1)
#define     NFP_MSF_TX_CTL_TBUF_ELEM_SIZE_0_256         (2)
#define   NFP_MSF_TX_CTL_TBUF_PART(_x)                  ((_x) & 0x3)
#define   NFP_MSF_TX_CTL_TBUF_PART_of(_x)               ((_x) & 0x3)
#define     NFP_MSF_TX_CTL_TBUF_PART_1_WAY              (0)
/* Register Type: MsfIntrptStatus */
#define NFP_MSF_INTR_STS               0x0008
#define   NFP_MSF_INTR_STS_FCEFIFO_OVFLW_CNT(_x)        (((_x) & 0xff) << 24)
#define   NFP_MSF_INTR_STS_FCEFIFO_OVFLW_CNT_of(_x)     (((_x) >> 24) & 0xff)
#define   NFP_MSF_INTR_STS_RBUF_OVFLW_CNT(_x)           (((_x) & 0xff) << 16)
#define   NFP_MSF_INTR_STS_RBUF_OVFLW_CNT_of(_x)        (((_x) >> 16) & 0xff)
#define   NFP_MSF_INTR_STS_FCIFIFO_PARITY_ERR           (0x1 << 15)
#define   NFP_MSF_INTR_STS_FLWCTL_TRAIN_STOPPED         (0x1 << 14)
#define   NFP_MSF_INTR_STS_CAL_TRAIN_STOPPED            (0x1 << 13)
#define   NFP_MSF_INTR_STS_DATA_TRAIN_STOPPED           (0x1 << 12)
#define   NFP_MSF_INTR_STS_DET_CSIX_FC_IDLE             (0x1 << 11)
#define   NFP_MSF_INTR_STS_REC_FLW_CTL_TRAIN            (0x1 << 9)
#define   NFP_MSF_INTR_STS_REC_CAL_TRAIN                (0x1 << 8)
#define   NFP_MSF_INTR_STS_REC_DATA_TRAIN               (0x1 << 7)
#define   NFP_MSF_INTR_STS_FCIFIFO_ERR                  (0x1 << 6)
#define   NFP_MSF_INTR_STS_DETECT_NO_CAL                (0x1 << 5)
#define     NFP_MSF_INTR_STS_DETECT_NO_CAL_ACTIVE       (0x0)
#define     NFP_MSF_INTR_STS_DETECT_NO_CAL_INACTIVE     (0x20)
#define   NFP_MSF_INTR_STS_TBUF_ERROR                   (0x1 << 4)
#define   NFP_MSF_INTR_STS_TSTAT_PAR_ERR                (0x1 << 3)
#define   NFP_MSF_INTR_STS_DIP4_ERR                     (0x1 << 2)
/* Register Type: MsfIntrptEn */
#define NFP_MSF_INTR_ENABLE            0x000c
#define   NFP_MSF_INTR_ENABLE_FCEFIFO_OVFLW_CNT         (0x1 << 24)
#define   NFP_MSF_INTR_ENABLE_RBUF_OVFLW_CNT            (0x1 << 16)
#define   NFP_MSF_INTR_ENABLE_FCIFIFO_PARITY_ERR        (0x1 << 15)
#define   NFP_MSF_INTR_ENABLE_FLWCTL_TRAIN_STOPPED      (0x1 << 14)
#define   NFP_MSF_INTR_ENABLE_CAL_TRAIN_STOPPED         (0x1 << 13)
#define   NFP_MSF_INTR_ENABLE_DATA_TRAIN_STOPPED        (0x1 << 12)
#define   NFP_MSF_INTR_ENABLE_DET_CSIX_FC_IDLE          (0x1 << 11)
#define   NFP_MSF_INTR_ENABLE_REC_FLW_CTL_TRAIN         (0x1 << 9)
#define   NFP_MSF_INTR_ENABLE_REC_CAL_TRAIN             (0x1 << 8)
#define   NFP_MSF_INTR_ENABLE_REC_TRAIN                 (0x1 << 7)
#define   NFP_MSF_INTR_ENABLE_FCIFIFO_ERR               (0x1 << 6)
#define   NFP_MSF_INTR_ENABLE_DETECT_NO_CAL             (0x1 << 5)
#define   NFP_MSF_INTR_ENABLE_TBUF_ERROR                (0x1 << 4)
#define   NFP_MSF_INTR_ENABLE_TSTAT_PAR_ERR             (0x1 << 3)
#define   NFP_MSF_INTR_ENABLE_DIP4_ERR                  (0x1 << 2)
/* Register Type: MsfFCEgressStatus */
#define NFP_MSF_FC_EGRESS_STS          0x0014
#define   NFP_MSF_FC_EGRESS_STS_FCEFIFO_FULL            (0x1 << 4)
#define   NFP_MSF_FC_EGRESS_STS_TM_DREADY               (0x1 << 3)
#define   NFP_MSF_FC_EGRESS_STS_TM_CREADY               (0x1 << 2)
#define   NFP_MSF_FC_EGRESS_STS_SF_DREADY               (0x1 << 1)
#define   NFP_MSF_FC_EGRESS_STS_SF_CREADY               (0x1)
/* Register Type: MsfFCIngressStatus */
#define NFP_MSF_FC_INGRESS_STS         0x0018
#define   NFP_MSF_FC_INGRESS_STS_TM_DREADY              (0x1 << 3)
#define   NFP_MSF_FC_INGRESS_STS_TM_CREADY              (0x1 << 2)
#define   NFP_MSF_FC_INGRESS_STS_SF_DREADY              (0x1 << 1)
#define   NFP_MSF_FC_INGRESS_STS_SF_CREADY              (0x1)
/* Register Type: MsfHWMCtrl */
#define NFP_MSF_HWM_CTRL               0x0024
#define   NFP_MSF_HWM_CTRL_FCIFIFO_EXT_HWM(_x)          (((_x) & 0x3) << 10)
#define   NFP_MSF_HWM_CTRL_FCIFIFO_EXT_HWM_of(_x)       (((_x) >> 10) & 0x3)
#define     NFP_MSF_HWM_CTRL_FCIFIFO_EXT_HWM_32         (0)
#define     NFP_MSF_HWM_CTRL_FCIFIFO_EXT_HWM_64         (1)
#define     NFP_MSF_HWM_CTRL_FCIFIFO_EXT_HWM_128        (2)
#define     NFP_MSF_HWM_CTRL_FCIFIFO_EXT_HWM_192        (3)
#define   NFP_MSF_HWM_CTRL_FCIFIFO_INT_HWM(_x)          (((_x) & 0x3) << 8)
#define   NFP_MSF_HWM_CTRL_FCIFIFO_INT_HWM_of(_x)       (((_x) >> 8) & 0x3)
#define     NFP_MSF_HWM_CTRL_FCIFIFO_INT_HWM_16         (0)
#define     NFP_MSF_HWM_CTRL_FCIFIFO_INT_HWM_32         (1)
#define     NFP_MSF_HWM_CTRL_FCIFIFO_INT_HWM_64         (2)
#define     NFP_MSF_HWM_CTRL_FCIFIFO_INT_HWM_128        (3)
#define   NFP_MSF_HWM_CTRL_FCEFIFO_HWM(_x)              (((_x) & 0x3) << 6)
#define   NFP_MSF_HWM_CTRL_FCEFIFO_HWM_of(_x)           (((_x) >> 6) & 0x3)
#define     NFP_MSF_HWM_CTRL_FCEFIFO_HWM_16             (0)
#define     NFP_MSF_HWM_CTRL_FCEFIFO_HWM_32             (1)
#define     NFP_MSF_HWM_CTRL_FCEFIFO_HWM_64             (2)
#define     NFP_MSF_HWM_CTRL_FCEFIFO_HWM_128            (3)
#define   NFP_MSF_HWM_CTRL_RBUF_S_HWM(_x)               (((_x) & 0x3) << 4)
#define   NFP_MSF_HWM_CTRL_RBUF_S_HWM_of(_x)            (((_x) >> 4) & 0x3)
/* Register Type: MsfFCStatusOvrd */
#define NFP_MSF_FC_STS_OVRD            0x0028
#define   NFP_MSF_FC_STS_OVRD_EGRESS_FORCE_EN           (0x1 << 9)
#define     NFP_MSF_FC_STS_OVRD_EGRESS_FORCE_EN_FC_EGRESS_STS (0x0)
#define     NFP_MSF_FC_STS_OVRD_EGRESS_FORCE_EN_EGRESS_FORCE (0x200)
#define   NFP_MSF_FC_STS_OVRD_INGRESS_FORCE_EN          (0x1 << 8)
#define     NFP_MSF_FC_STS_OVRD_INGRESS_FORCE_EN_RXCSRB (0x0)
#define     NFP_MSF_FC_STS_OVRD_INGRESS_FORCE_EN_INGRESS_FORCE (0x100)
#define   NFP_MSF_FC_STS_OVRD_EGRESS_FORCE_SF_CREADY    (0x1 << 7)
#define   NFP_MSF_FC_STS_OVRD_EGRESS_FORCE_SF_DREADY    (0x1 << 6)
#define   NFP_MSF_FC_STS_OVRD_EGRESS_FORCE_TM_CREADY    (0x1 << 5)
#define   NFP_MSF_FC_STS_OVRD_EGRESS_FORCE_TM_DREADY    (0x1 << 4)
#define   NFP_MSF_FC_STS_OVRD_INGRESS_FORCE_SF_CREADY   (0x1 << 3)
#define   NFP_MSF_FC_STS_OVRD_INGRESS_FORCE_SF_DREADY   (0x1 << 2)
#define   NFP_MSF_FC_STS_OVRD_INGRESS_FORCE_TM_CREADY   (0x1 << 1)
#define   NFP_MSF_FC_STS_OVRD_INGRESS_FORCE_TM_DREADY   (0x1)
/* Register Type: MsfClkCtrl */
#define NFP_MSF_CLK_CTRL               0x002c
#define   NFP_MSF_CLK_CTRL_RCLK_REF_LOW_POWER           (0x1 << 15)
#define     NFP_MSF_CLK_CTRL_RCLK_REF_LOW_POWER_ENABLE  (0x0)
#define     NFP_MSF_CLK_CTRL_RCLK_REF_LOW_POWER_DISABLE (0x8000)
#define   NFP_MSF_CLK_CTRL_TX_FC_LOW_POWER              (0x1 << 14)
#define     NFP_MSF_CLK_CTRL_TX_FC_LOW_POWER_ENABLE     (0x0)
#define     NFP_MSF_CLK_CTRL_TX_FC_LOW_POWER_DISABLE    (0x4000)
#define   NFP_MSF_CLK_CTRL_RXCCLK_DLL_RESET             (0x1 << 13)
#define     NFP_MSF_CLK_CTRL_RXCCLK_DLL_RESET_HOLD      (0x0)
#define     NFP_MSF_CLK_CTRL_RXCCLK_DLL_RESET_REMOVE    (0x2000)
#define   NFP_MSF_CLK_CTRL_RCLK_DLL_RESET               (0x1 << 12)
#define     NFP_MSF_CLK_CTRL_RCLK_DLL_RESET_HOLD        (0x0)
#define     NFP_MSF_CLK_CTRL_RCLK_DLL_RESET_REMOVE      (0x1000)
#define   NFP_MSF_CLK_CTRL_RXCCLK_DLL_EN                (0x1 << 11)
#define     NFP_MSF_CLK_CTRL_RXCCLK_DLL_EN_INT          (0x0)
#define     NFP_MSF_CLK_CTRL_RXCCLK_DLL_EN_DLL          (0x800)
#define   NFP_MSF_CLK_CTRL_RCLK_DLL_EN                  (0x1 << 10)
#define     NFP_MSF_CLK_CTRL_RCLK_DLL_EN_INT            (0x0)
#define     NFP_MSF_CLK_CTRL_RCLK_DLL_EN_DLL            (0x400)
#define   NFP_MSF_CLK_CTRL_RSX_SECTION_EN               (0x1 << 9)
#define     NFP_MSF_CLK_CTRL_RSX_SECTION_EN_IN_RESET    (0x0)
#define     NFP_MSF_CLK_CTRL_RSX_SECTION_EN_RUNNING     (0x200)
#define   NFP_MSF_CLK_CTRL_TSX_SECTION_EN               (0x1 << 8)
#define     NFP_MSF_CLK_CTRL_TSX_SECTION_EN_IN_RESET    (0x0)
#define     NFP_MSF_CLK_CTRL_TSX_SECTION_EN_RUNNING     (0x100)
#define   NFP_MSF_CLK_CTRL_RCLK_REF_EN                  (0x1 << 7)
#define     NFP_MSF_CLK_CTRL_RCLK_REF_EN_NULL           (0x0)
#define     NFP_MSF_CLK_CTRL_RCLK_REF_EN_RCLK           (0x80)
#define   NFP_MSF_CLK_CTRL_TXCCLK_EN                    (0x1 << 6)
#define     NFP_MSF_CLK_CTRL_TXCCLK_EN_NULL             (0x0)
#define     NFP_MSF_CLK_CTRL_TXCCLK_EN_TXCCLK_SOURCE    (0x40)
#define   NFP_MSF_CLK_CTRL_RSCLK_EN                     (0x1 << 5)
#define     NFP_MSF_CLK_CTRL_RSCLK_EN_NULL              (0x0)
#define     NFP_MSF_CLK_CTRL_RSCLK_EN_QUART_RCLK        (0x20)
#define   NFP_MSF_CLK_CTRL_TCLK_EN                      (0x1 << 4)
#define     NFP_MSF_CLK_CTRL_TCLK_EN_NULL               (0x0)
#define     NFP_MSF_CLK_CTRL_TCLK_EN_TCLK_SOURCE        (0x10)
#define   NFP_MSF_CLK_CTRL_RX_FC_SECTION_EN             (0x1 << 3)
#define     NFP_MSF_CLK_CTRL_RX_FC_SECTION_EN_IN_RESET  (0x0)
#define     NFP_MSF_CLK_CTRL_RX_FC_SECTION_EN_RUNNING   (0x8)
#define   NFP_MSF_CLK_CTRL_TX_FC_SECTION_EN             (0x1 << 2)
#define     NFP_MSF_CLK_CTRL_TX_FC_SECTION_EN_IN_RESET  (0x0)
#define     NFP_MSF_CLK_CTRL_TX_FC_SECTION_EN_RUNNING   (0x4)
#define   NFP_MSF_CLK_CTRL_RX_SECTION_EN                (0x1 << 1)
#define     NFP_MSF_CLK_CTRL_RX_SECTION_EN_IN_RESET     (0x0)
#define     NFP_MSF_CLK_CTRL_RX_SECTION_EN_RUNNING      (0x2)
#define   NFP_MSF_CLK_CTRL_TX_SECTION_EN                (0x1)
#define     NFP_MSF_CLK_CTRL_TX_SECTION_EN_IN_RESET     (0x0)
#define     NFP_MSF_CLK_CTRL_TX_SECTION_EN_RUNNING      (0x1)
/* Register Type: MsfRxThdFreelist */
#define NFP_MSF_RX_THD_FREELIST_0      0x0030
#define NFP_MSF_RX_THD_FREELIST_1      0x0034
#define   NFP_MSF_RX_THD_FREELIST_XFER_REG_4            (0x1 << 19)
#define   NFP_MSF_RX_THD_FREELIST_ME_CLUS_3_1(_x)       (((_x) & 0x7) << 16)
#define   NFP_MSF_RX_THD_FREELIST_SIG_NO(_x)            (((_x) & 0xf) << 12)
#define   NFP_MSF_RX_THD_FREELIST_ME_CLUS_0             (0x1 << 11)
#define   NFP_MSF_RX_THD_FREELIST_M_TYPE                (0x1 << 10)
#define   NFP_MSF_RX_THD_FREELIST_ME_NO(_x)             (((_x) & 0x7) << 7)
#define   NFP_MSF_RX_THD_FREELIST_THD(_x)               (((_x) & 0x7) << 4)
#define   NFP_MSF_RX_THD_FREELIST_XFER_REG_3_0(_x)      ((_x) & 0xf)
/* Register Type: MsfRxPortMap */
#define NFP_MSF_RX_PORT_MAP            0x0040
#define   NFP_MSF_RX_PORT_MAP_PORT_NO(_x)               ((_x) & 0xff)
#define   NFP_MSF_RX_PORT_MAP_PORT_NO_of(_x)            ((_x) & 0xff)
/* Register Type: MsfRBufElemDone */
#define NFP_MSF_RBUF_ELEM_DONE         0x0044
#define   NFP_MSF_RBUF_ELEM_DONE_ELE_TO_FREE(_x)        ((_x) & 0x7f)
/* Register Type: MsfRxCalendarLen */
#define NFP_MSF_RX_CAL_LEN             0x0048
#define   NFP_MSF_RX_CAL_LEN_LENGTH(_x)                 ((_x) & 0xff)
#define   NFP_MSF_RX_CAL_LEN_LENGTH_of(_x)              ((_x) & 0xff)
/* Register Type: MsfFCEFifoValidate */
#define NFP_MSF_FCEFIFO_VALIDATE       0x004c
#define   NFP_MSF_FCEFIFO_VALIDATE_ANY_DATA(_x)         (_x)
/* Register Type: MsfRxThdFreelistTimeout */
#define NFP_MSF_RX_THD_FL_TIMEOUT0     0x0050
#define NFP_MSF_RX_THD_FL_TIMEOUT1     0x0054
#define   NFP_MSF_RX_THD_FL_TIMEOUT_INTERVAL(_x)        ((_x) & 0x1fff)
#define   NFP_MSF_RX_THD_FL_TIMEOUT_INTERVAL_of(_x)     ((_x) & 0x1fff)
/* Register Type: MsfTxSeq */
#define NFP_MSF_TX_SEQ0                0x0060
#define   NFP_MSF_TX_SEQ_EMPTY                          (0x1 << 31)
#define     NFP_MSF_TX_SEQ_EMPTY_VALID                  (0x0)
#define     NFP_MSF_TX_SEQ_EMPTY_NO_VALID               (0x80000000)
#define   NFP_MSF_TX_SEQ_COUNT_of(_x)                   ((_x) & 0xff)
/* Register Type: MsfTxCalendarLen */
#define NFP_MSF_TX_CAL_LEN             0x0070
#define   NFP_MSF_TX_CAL_LEN_LENGTH(_x)                 ((_x) & 0xff)
#define   NFP_MSF_TX_CAL_LEN_LENGTH_of(_x)              ((_x) & 0xff)
/* Register Type: MsfTrainData */
#define NFP_MSF_TRAIN_DAT              0x00a0
#define   NFP_MSF_TRAIN_DAT_ALPHA(_x)                   (((_x) & 0x3f) << 10)
#define   NFP_MSF_TRAIN_DAT_ALPHA_of(_x)                (((_x) >> 10) & 0x3f)
#define   NFP_MSF_TRAIN_DAT_SING_TRAIN                  (0x1 << 9)
#define     NFP_MSF_TRAIN_DAT_SING_TRAIN_NONE           (0x0)
#define     NFP_MSF_TRAIN_DAT_SING_TRAIN_TRANSMIT       (0x200)
#define   NFP_MSF_TRAIN_DAT_CONT_TRAIN                  (0x1 << 8)
#define     NFP_MSF_TRAIN_DAT_CONT_TRAIN_NONE           (0x0)
#define     NFP_MSF_TRAIN_DAT_CONT_TRAIN_TRANSMIT       (0x100)
#define   NFP_MSF_TRAIN_DAT_DETECT_NO_CAL               (0x1 << 3)
#define     NFP_MSF_TRAIN_DAT_DETECT_NO_CAL_FIXME34     (0x0)
#define     NFP_MSF_TRAIN_DAT_DETECT_NO_CAL_FIXME35     (0x8)
#define   NFP_MSF_TRAIN_DAT_TRAIN_EN_TSTAT              (0x1 << 2)
#define     NFP_MSF_TRAIN_DAT_TRAIN_EN_TSTAT_FIXME0     (0x0)
#define     NFP_MSF_TRAIN_DAT_TRAIN_EN_TSTAT_FIXME1     (0x4)
#define   NFP_MSF_TRAIN_DAT_RSTAT_EN                    (0x1 << 1)
#define     NFP_MSF_TRAIN_DAT_RSTAT_EN_FIXME2           (0x0)
#define     NFP_MSF_TRAIN_DAT_RSTAT_EN_FIXME3           (0x2)
#define   NFP_MSF_TRAIN_DAT_IGN_TRAIN                   (0x1)
#define     NFP_MSF_TRAIN_DAT_IGN_TRAIN_FIXME4          (0x0)
#define     NFP_MSF_TRAIN_DAT_IGN_TRAIN_FIXME5          (0x1)
/* Register Type: MsfTrainCalendar */
#define NFP_MSF_TRAIN_CAL              0x00a4
#define   NFP_MSF_TRAIN_CAL_ALPHA(_x)                   (((_x) & 0x3f) << 10)
#define   NFP_MSF_TRAIN_CAL_SING_TRAIN                  (0x1 << 9)
#define     NFP_MSF_TRAIN_CAL_SING_TRAIN_FIXME6         (0x0)
#define     NFP_MSF_TRAIN_CAL_SING_TRAIN_FIXME7         (0x200)
#define   NFP_MSF_TRAIN_CAL_CONT_TRAIN                  (0x1 << 8)
#define     NFP_MSF_TRAIN_CAL_CONT_TRAIN_FIXME8         (0x0)
#define     NFP_MSF_TRAIN_CAL_CONT_TRAIN_FIXME9         (0x100)
#define   NFP_MSF_TRAIN_CAL_IGN_TRAIN                   (0x1)
#define     NFP_MSF_TRAIN_CAL_IGN_TRAIN_FIXME10         (0x0)
#define     NFP_MSF_TRAIN_CAL_IGN_TRAIN_FIXME11         (0x1)
/* Register Type: MsfTrainFlowCtrl */
#define NFP_MSF_TRAIN_FLOW_CTRL        0x00a8
#define   NFP_MSF_TRAIN_FLOW_CTRL_FORCE_FCDEAD          (0x1 << 17)
#define     NFP_MSF_TRAIN_FLOW_CTRL_FORCE_FCDEAD_FIXME12 (0x0)
#define     NFP_MSF_TRAIN_FLOW_CTRL_FORCE_FCDEAD_FIXME13 (0x20000)
#define   NFP_MSF_TRAIN_FLOW_CTRL_FORCE_FCIDLE          (0x1 << 16)
#define     NFP_MSF_TRAIN_FLOW_CTRL_FORCE_FCIDLE_FIXME14 (0x0)
#define     NFP_MSF_TRAIN_FLOW_CTRL_FORCE_FCIDLE_FIXME15 (0x10000)
#define   NFP_MSF_TRAIN_FLOW_CTRL_ALPHA(_x)             (((_x) & 0x3f) << 10)
#define   NFP_MSF_TRAIN_FLOW_CTRL_ALPHA_of(_x)          (((_x) >> 10) & 0x3f)
#define   NFP_MSF_TRAIN_FLOW_CTRL_SING_TRAIN            (0x1 << 9)
#define     NFP_MSF_TRAIN_FLOW_CTRL_SING_TRAIN_FIXME16  (0x0)
#define     NFP_MSF_TRAIN_FLOW_CTRL_SING_TRAIN_FIXME17  (0x200)
#define   NFP_MSF_TRAIN_FLOW_CTRL_CONT_TRAIN            (0x1 << 8)
#define     NFP_MSF_TRAIN_FLOW_CTRL_CONT_TRAIN_FIXME18  (0x0)
#define     NFP_MSF_TRAIN_FLOW_CTRL_CONT_TRAIN_FIXME19  (0x100)
#define   NFP_MSF_TRAIN_FLOW_CTRL_TD_EN_CDEAD           (0x1 << 6)
#define     NFP_MSF_TRAIN_FLOW_CTRL_TD_EN_CDEAD_FIXME20 (0x0)
#define     NFP_MSF_TRAIN_FLOW_CTRL_TD_EN_CDEAD_FIXME21 (0x40)
#define   NFP_MSF_TRAIN_FLOW_CTRL_DET_FCDEAD            (0x1 << 5)
#define     NFP_MSF_TRAIN_FLOW_CTRL_DET_FCDEAD_FIXME22  (0x0)
#define     NFP_MSF_TRAIN_FLOW_CTRL_DET_FCDEAD_FIXME23  (0x20)
#define   NFP_MSF_TRAIN_FLOW_CTRL_DET_FCIDLE            (0x1 << 4)
#define     NFP_MSF_TRAIN_FLOW_CTRL_DET_FCIDLE_FIXME24  (0x0)
#define     NFP_MSF_TRAIN_FLOW_CTRL_DET_FCIDLE_FIXME25  (0x10)
#define   NFP_MSF_TRAIN_FLOW_CTRL_DET_TXCFC_SUS         (0x1 << 3)
#define     NFP_MSF_TRAIN_FLOW_CTRL_DET_TXCFC_SUS_FIXME26 (0x0)
#define     NFP_MSF_TRAIN_FLOW_CTRL_DET_TXCFC_SUS_FIXME27 (0x8)
#define   NFP_MSF_TRAIN_FLOW_CTRL_TRAIN_EN_CFC          (0x1 << 2)
#define     NFP_MSF_TRAIN_FLOW_CTRL_TRAIN_EN_CFC_FIXME28 (0x0)
#define     NFP_MSF_TRAIN_FLOW_CTRL_TRAIN_EN_CFC_FIXME29 (0x4)
#define   NFP_MSF_TRAIN_FLOW_CTRL_RXCFC_EN              (0x1 << 1)
#define     NFP_MSF_TRAIN_FLOW_CTRL_RXCFC_EN_FIXME30    (0x0)
#define     NFP_MSF_TRAIN_FLOW_CTRL_RXCFC_EN_FIXME31    (0x2)
#define   NFP_MSF_TRAIN_FLOW_CTRL_IGN_TRAIN             (0x1)
#define     NFP_MSF_TRAIN_FLOW_CTRL_IGN_TRAIN_FIXME32   (0x0)
#define     NFP_MSF_TRAIN_FLOW_CTRL_IGN_TRAIN_FIXME33   (0x1)
/* Register Type: MsfFCIFifo */
#define NFP_MSF_FCIFIFO(_fifo)         (0x100 + (0x4 * ((_fifo) & 0xf)))
#define   NFP_MSF_FCIFIFO_CWRD_of(_x)                   (_x)
/* Register Type: MsfFCEFifo */
#define NFP_MSF_FCEFIFO(_fifo)         (0x140 + (0x4 * ((_fifo) & 0xf)))
#define   NFP_MSF_FCEFIFO_CWRD_of(_x)                   (_x)
/* Register Type: MsfRxDeskew */
#define NFP_MSF_RX_DESKEW_RDAT(_pin)   (0x300 + (0x4 * ((_pin) & 0xf)))
#define NFP_MSF_RX_DESKEW_RCTL         0x0340
#define NFP_MSF_RX_DESKEW_RPAR         0x0344
#define NFP_MSF_RX_DESKEW_RPROT        0x0348
#define NFP_MSF_RX_DESKEW_RXCSOF       0x0354
#define NFP_MSF_RX_DESKEW_RXCDAT0      0x0358
#define NFP_MSF_RX_DESKEW_RXCDAT1      0x035c
#define NFP_MSF_RX_DESKEW_RXCDAT2      0x0360
#define NFP_MSF_RX_DESKEW_RXCDAT3      0x0364
#define NFP_MSF_RX_DESKEW_RXPAR        0x0368
#define NFP_MSF_RX_DESKEW_RXCSRB       0x036c
#define   NFP_MSF_RX_DESKEW_VALUE(_x)                   ((_x) & 0x1f)
#define   NFP_MSF_RX_DESKEW_VALUE_of(_x)                ((_x) & 0x1f)
#define     NFP_MSF_RX_DESKEW_VALUE_90                  (1)
#define     NFP_MSF_RX_DESKEW_VALUE_112_5               (1)
#define     NFP_MSF_RX_DESKEW_VALUE_135                 (2)
#define     NFP_MSF_RX_DESKEW_VALUE_157_7               (3)
#define     NFP_MSF_RX_DESKEW_VALUE_180                 (4)
#define     NFP_MSF_RX_DESKEW_VALUE_202_5               (5)
#define     NFP_MSF_RX_DESKEW_VALUE_225                 (6)
#define     NFP_MSF_RX_DESKEW_VALUE_247_5               (7)
#define     NFP_MSF_RX_DESKEW_VALUE_270                 (8)
#define     NFP_MSF_RX_DESKEW_VALUE_292_5               (9)
#define     NFP_MSF_RX_DESKEW_VALUE_315                 (10)
#define     NFP_MSF_RX_DESKEW_VALUE_337_5               (11)
#define     NFP_MSF_RX_DESKEW_VALUE_0                   (12)
#define     NFP_MSF_RX_DESKEW_VALUE_22_5                (13)
#define     NFP_MSF_RX_DESKEW_VALUE_45                  (14)
#define     NFP_MSF_RX_DESKEW_VALUE_67_5                (15)
/* Register Type: MsfSpi4DynFiltThresh */
#define NFP_MSF_SPI4_DYNFILT_THRESH    0x034c
#define   NFP_MSF_SPI4_DYNFILT_THRESH_NUM_SAMPL(_x)     ((_x) & 0x7)
#define   NFP_MSF_SPI4_DYNFILT_THRESH_NUM_SAMPL_of(_x)  ((_x) & 0x7)
/* Register Type: MsfDllDataDelayCtl */
#define NFP_MSF_DLL_DAT_DELAY_CTRL     0x0350
#define   NFP_MSF_DLL_DAT_DELAY_CTRL_SELECT(_x)         ((_x) & 0x3)
#define     NFP_MSF_DLL_DAT_DELAY_CTRL_SELECT_NOMINAL   (0)
#define     NFP_MSF_DLL_DAT_DELAY_CTRL_SELECT_PLUS_50P  (1)
#define     NFP_MSF_DLL_DAT_DELAY_CTRL_SELECT_MINUS_50P (2)
#define     NFP_MSF_DLL_DAT_DELAY_CTRL_SELECT_MINUS_100P (3)
/* Register Type: MsfFCDynFiltThresh */
#define NFP_MSF_FC_DYNFILT_THRESH      0x0370
#define   NFP_MSF_FC_DYNFILT_THRESH_NUM_SAMPL(_x)       ((_x) & 0x7)
#define   NFP_MSF_FC_DYNFILT_THRESH_NUM_SAMPL_of(_x)    ((_x) & 0x7)
/* Register Type: MsfFCDllDataDelayCtl */
#define NFP_MSF_FC_DLL_DAT_DELAY_CTRL  0x0374
#define   NFP_MSF_FC_DLL_DAT_DELAY_CTRL_SELECT(_x)      ((_x) & 0x3)
#define     NFP_MSF_FC_DLL_DAT_DELAY_CTRL_SELECT_NOMINAL (0)
#define     NFP_MSF_FC_DLL_DAT_DELAY_CTRL_SELECT_PLUS_50P (1)
#define     NFP_MSF_FC_DLL_DAT_DELAY_CTRL_SELECT_MINUS_50P (2)
#define     NFP_MSF_FC_DLL_DAT_DELAY_CTRL_SELECT_MINUS_100P (3)
/* Register Type: MsfTxMultiplePortStatus */
#define NFP_MSF_TX_MPORT_STS(_x)       (0x380 + (0x4 * ((_x) & 0xf)))
#define   NFP_MSF_TX_MPORT_STS_PORT15_of(_x)            (((_x) >> 30) & 0x3)
#define   NFP_MSF_TX_MPORT_STS_PORT14_of(_x)            (((_x) >> 28) & 0x3)
#define   NFP_MSF_TX_MPORT_STS_PORT13_of(_x)            (((_x) >> 26) & 0x3)
#define   NFP_MSF_TX_MPORT_STS_PORT12_of(_x)            (((_x) >> 24) & 0x3)
#define   NFP_MSF_TX_MPORT_STS_PORT11_of(_x)            (((_x) >> 22) & 0x3)
#define   NFP_MSF_TX_MPORT_STS_PORT10_of(_x)            (((_x) >> 20) & 0x3)
#define   NFP_MSF_TX_MPORT_STS_PORT9_of(_x)             (((_x) >> 18) & 0x3)
#define   NFP_MSF_TX_MPORT_STS_PORT8_of(_x)             (((_x) >> 16) & 0x3)
#define   NFP_MSF_TX_MPORT_STS_PORT7_of(_x)             (((_x) >> 14) & 0x3)
#define   NFP_MSF_TX_MPORT_STS_PORT6_of(_x)             (((_x) >> 12) & 0x3)
#define   NFP_MSF_TX_MPORT_STS_PORT5_of(_x)             (((_x) >> 10) & 0x3)
#define   NFP_MSF_TX_MPORT_STS_PORT4_of(_x)             (((_x) >> 8) & 0x3)
#define   NFP_MSF_TX_MPORT_STS_PORT3_of(_x)             (((_x) >> 6) & 0x3)
#define   NFP_MSF_TX_MPORT_STS_PORT2_of(_x)             (((_x) >> 4) & 0x3)
#define   NFP_MSF_TX_MPORT_STS_PORT1_of(_x)             (((_x) >> 2) & 0x3)
#define   NFP_MSF_TX_MPORT_STS_PORT0_of(_x)             ((_x) & 0x3)
#define     NFP_MSF_TX_MPORT_STS_STARVING               (0)
#define     NFP_MSF_TX_MPORT_STS_HUNGRY                 (1)
#define     NFP_MSF_TX_MPORT_STS_SATISFIED              (2)
#define     NFP_MSF_TX_MPORT_STS_UNDEFINED              (3)
/* Register Type: MsfRxPhasemon */
#define NFP_MSF_RX_PHASEMON_RDAT(_pin) (0x400 + (0x4 * ((_pin) & 0xf)))
#define NFP_MSF_RX_PHASEMON_RCTRL      0x0440
#define NFP_MSF_RX_PHASEMON_RPAR       0x0444
#define NFP_MSF_RX_PHASEMON_RPROT      0x0448
#define NFP_MSF_RX_PHASEMON_RXCSOF     0x0454
#define NFP_MSF_RX_PHASEMON_RXCDAT0    0x0458
#define NFP_MSF_RX_PHASEMON_RXCDAT1    0x045c
#define NFP_MSF_RX_PHASEMON_RXCDAT2    0x0460
#define NFP_MSF_RX_PHASEMON_RXCDAT3    0x0464
#define NFP_MSF_RX_PHASEMON_RXCPAR     0x0468
#define NFP_MSF_RX_PHASEMON_RXSRB      0x046c
#define   NFP_MSF_RX_PHASEMON_TR_STATUS                 (0x1 << 4)
#define     NFP_MSF_RX_PHASEMON_TR_STATUS_SUCCESS       (0x0)
#define     NFP_MSF_RX_PHASEMON_TR_STATUS_FAIL          (0x10)
#define   NFP_MSF_RX_PHASEMON_DESKEW_VALUE_of(_x)       ((_x) & 0xf)
#define     NFP_MSF_RX_PHASEMON_DESKEW_VALUE_90         (0)
#define     NFP_MSF_RX_PHASEMON_DESKEW_VALUE_112_5      (1)
#define     NFP_MSF_RX_PHASEMON_DESKEW_VALUE_135        (2)
#define     NFP_MSF_RX_PHASEMON_DESKEW_VALUE_157_5      (3)
#define     NFP_MSF_RX_PHASEMON_DESKEW_VALUE_180        (4)
#define     NFP_MSF_RX_PHASEMON_DESKEW_VALUE_202_5      (5)
#define     NFP_MSF_RX_PHASEMON_DESKEW_VALUE_225        (6)
#define     NFP_MSF_RX_PHASEMON_DESKEW_VALUE_247_5      (7)
#define     NFP_MSF_RX_PHASEMON_DESKEW_VALUE_270        (8)
#define     NFP_MSF_RX_PHASEMON_DESKEW_VALUE_292_5      (9)
#define     NFP_MSF_RX_PHASEMON_DESKEW_VALUE_315        (10)
#define     NFP_MSF_RX_PHASEMON_DESKEW_VALUE_337_5      (11)
#define     NFP_MSF_RX_PHASEMON_DESKEW_VALUE_0          (12)
#define     NFP_MSF_RX_PHASEMON_DESKEW_VALUE_22_5       (13)
#define     NFP_MSF_RX_PHASEMON_DESKEW_VALUE_45         (14)
#define     NFP_MSF_RX_PHASEMON_DESKEW_VALUE_67_5       (15)
/* Register Type: MsfRxPortCalStat */
#define NFP_MSF_RX_PORT_CAL_STS(_port) (0x500 + (0x4 * ((_port) & 0xff)))
#define   NFP_MSF_RX_PORT_CAL_STS_PORT_CAL_STS(_x)      ((_x) & 0x3)
#define   NFP_MSF_RX_PORT_CAL_STS_PORT_CAL_STS_of(_x)   ((_x) & 0x3)
/* Register Type: MsfTxCalendar */
#define NFP_MSF_TX_CAL(_slot)          (0x1000 + (0x4 * ((_slot) & 0xff)))
#define   NFP_MSF_TX_CAL_PORT_NUM(_x)                   ((_x) & 0xff)
#define   NFP_MSF_TX_CAL_PORT_NUM_of(_x)                ((_x) & 0xff)
/* Register Type: MsfTxPortStat */
#define NFP_MSF_TX_PORT_STS(_port)     (0x1400 + (0x4 * ((_port) & 0xff)))
#define   NFP_MSF_TX_PORT_STS_STATUS_of(_x)             ((_x) & 0x3)
#define     NFP_MSF_TX_PORT_STS_STATUS_STARVING         (0)
#define     NFP_MSF_TX_PORT_STS_STATUS_HUNGRY           (1)
#define     NFP_MSF_TX_PORT_STS_STATUS_SATISFIED        (2)
#define     NFP_MSF_TX_PORT_STS_STATUS_UNDEFINED        (3)
/* Register Type: MsfTBufElemCtrlA */
#define NFP_MSF_TBUF_ELEM_CTRL_A(_elem) (0x1800 + (0x8 * ((_elem) & 0x7f)))
#define   NFP_MSF_TBUF_ELEM_CTRL_A_PAYLOAD_LEN(_x)      (((_x) & 0xff) << 24)
#define   NFP_MSF_TBUF_ELEM_CTRL_A_PREPEND_OFF(_x)      (((_x) & 0x7) << 21)
#define   NFP_MSF_TBUF_ELEM_CTRL_A_PREPEND_LEN(_x)      (((_x) & 0x1f) << 16)
#define   NFP_MSF_TBUF_ELEM_CTRL_A_PAYLOAD_OFF(_x)      (((_x) & 0x7) << 13)
#define   NFP_MSF_TBUF_ELEM_CTRL_A_SKIP                 (0x1 << 11)
#define   NFP_MSF_TBUF_ELEM_CTRL_A_ABORT                (0x1 << 10)
#define   NFP_MSF_TBUF_ELEM_CTRL_A_SOP                  (0x1 << 9)
#define   NFP_MSF_TBUF_ELEM_CTRL_A_EOP                  (0x1 << 8)
#define   NFP_MSF_TBUF_ELEM_CTRL_A_ADR(_x)              ((_x) & 0xff)
/* Register Type: MsfTBufElemCtrlB */
#define NFP_MSF_TBUF_ELEM_CTRL_B(_elem) (0x1804 + (0x8 * ((_elem) & 0x7f)))
#define NFP_MSF_RBUF_BASE              0x2000
#define NFP_MSF_RBUF_LEN               0x2000
#define NFP_MSF_TBUF_BASE              0x2000
#define NFP_MSF_TBUF_LEN               0x2000

/* HGID: nfp3200/msf_cl.desc = ade0020e9035 */
/* Register Type: MsclBlkRst */
#define NFP_MSFCL_MSCL_BLK_RST         0x0000
#define   NFP_MSFCL_MSCL_BLK_RST_SPARE2_CLKGATE_EN      (0x1 << 15)
#define   NFP_MSFCL_MSCL_BLK_RST_SPARE1_CLKGATE_EN      (0x1 << 14)
#define   NFP_MSFCL_MSCL_BLK_RST_SPARE0_CLKGATE_EN      (0x1 << 13)
#define   NFP_MSFCL_MSCL_BLK_RST_SERDESMUX_CLKGATE_EN   (0x1 << 12)
#define   NFP_MSFCL_MSCL_BLK_RST_SPIMUX_CLKGATE_EN      (0x1 << 11)
#define   NFP_MSFCL_MSCL_BLK_RST_LK_CLKGATE_EN          (0x1 << 10)
#define   NFP_MSFCL_MSCL_BLK_RST_XU_CLKGATE_EN          (0x1 << 9)
#define   NFP_MSFCL_MSCL_BLK_RST_MSF_CLKGATE_EN         (0x1 << 8)
#define   NFP_MSFCL_MSCL_BLK_RST_SPARE2_RST             (0x1 << 7)
#define   NFP_MSFCL_MSCL_BLK_RST_SPARE1_RST             (0x1 << 6)
#define   NFP_MSFCL_MSCL_BLK_RST_SPARE0_RST             (0x1 << 5)
#define   NFP_MSFCL_MSCL_BLK_RST_SERDESMUX_BLK_RST      (0x1 << 4)
#define   NFP_MSFCL_MSCL_BLK_RST_SPIMUX_BLK_RST         (0x1 << 3)
#define   NFP_MSFCL_MSCL_BLK_RST_LK_BLK_RST             (0x1 << 2)
#define   NFP_MSFCL_MSCL_BLK_RST_XU_BLK_RST             (0x1 << 1)
#define   NFP_MSFCL_MSCL_BLK_RST_MSF_BLK_RST            (0x1)
/* Register Type: MsclSubblkRst */
#define NFP_MSFCL_MSCL_SUBBLK_RST      0x0004
#define   NFP_MSFCL_MSCL_SUBBLK_RST_XU_CFG_1_3_RST      (0x1 << 19)
#define   NFP_MSFCL_MSCL_SUBBLK_RST_XU_CFG_0_RST        (0x1 << 18)
#define   NFP_MSFCL_MSCL_SUBBLK_RST_XU_REF_RST          (0x1 << 17)
#define   NFP_MSFCL_MSCL_SUBBLK_RST_XU_MAC_RST          (0x1 << 16)
#define   NFP_MSFCL_MSCL_SUBBLK_RST_XU_PCS_RST          (0x1 << 15)
#define   NFP_MSFCL_MSCL_SUBBLK_RST_XU_XG2G_RST         (0x1 << 14)
#define   NFP_MSFCL_MSCL_SUBBLK_RST_XU_XCV_RST(_x)      (((_x) & 0xf) << 10)
#define   NFP_MSFCL_MSCL_SUBBLK_RST_XU_XCV_RST_of(_x)   (((_x) >> 10) & 0xf)
#define   NFP_MSFCL_MSCL_SUBBLK_RST_PHY_C_RDY_RST_EN    (0x1 << 9)
#define   NFP_MSFCL_MSCL_SUBBLK_RST_LK_OOB_RX_RST       (0x1 << 8)
#define   NFP_MSFCL_MSCL_SUBBLK_RST_LK_OOB_TX_RST       (0x1 << 7)
#define   NFP_MSFCL_MSCL_SUBBLK_RST_LK_SERDES_RX_RST(_x) (((_x) & 0xf) << 3)
#define   NFP_MSFCL_MSCL_SUBBLK_RST_LK_SERDES_RX_RST_of(_x) (((_x) >> 3) & 0xf)
#define   NFP_MSFCL_MSCL_SUBBLK_RST_LK_SERDES_TX_RST    (0x1 << 2)
#define   NFP_MSFCL_MSCL_SUBBLK_RST_LK_IP_RX_RST        (0x1 << 1)
#define   NFP_MSFCL_MSCL_SUBBLK_RST_LK_IP_TX_RST        (0x1)
/* Register Type: MsclMuxSel */
#define NFP_MSFCL_MSCL_MUX_SEL         0x0008
#define   NFP_MSFCL_MSCL_MUX_SEL_LK_SEL_OOB_LVCMOS_SEL  (0x1 << 13)
#define     NFP_MSFCL_MSCL_MUX_SEL_LK_SEL_OOB_LVCMOS_SEL_LVCS (0x0)
#define     NFP_MSFCL_MSCL_MUX_SEL_LK_SEL_OOB_LVCMOS_SEL_LVCMOS (0x2000)
#define   NFP_MSFCL_MSCL_MUX_SEL_MSCL_C2C_FC_SEL        (0x1 << 12)
#define   NFP_MSFCL_MSCL_MUX_SEL_MSCL_XULK_MCLK_SEL     (0x1 << 11)
#define   NFP_MSFCL_MSCL_MUX_SEL_LK_SEL_OOB_TSTAT       (0x1 << 10)
#define     NFP_MSFCL_MSCL_MUX_SEL_LK_SEL_OOB_TSTAT_IB  (0x0)
#define     NFP_MSFCL_MSCL_MUX_SEL_LK_SEL_OOB_TSTAT_OOB (0x400)
#define   NFP_MSFCL_MSCL_MUX_SEL_MSCL_SDMUX_TX_SWZL(_x) (((_x) & 0xf) << 6)
#define   NFP_MSFCL_MSCL_MUX_SEL_MSCL_SDMUX_TX_SWZL_of(_x) (((_x) >> 6) & 0xf)
#define     NFP_MSFCL_MSCL_MUX_SEL_MSCL_SDMUX_TX_SWZL_XAUI_MODE (0)
#define     NFP_MSFCL_MSCL_MUX_SEL_MSCL_SDMUX_TX_SWZL_LAKEN_MODE (15)
#define   NFP_MSFCL_MSCL_MUX_SEL_MSCL_SDMUX_RX_SWZL(_x) (((_x) & 0xf) << 2)
#define   NFP_MSFCL_MSCL_MUX_SEL_MSCL_SDMUX_RX_SWZL_of(_x) (((_x) >> 2) & 0xf)
#define     NFP_MSFCL_MSCL_MUX_SEL_MSCL_SDMUX_RX_SWZL_XAUI_MODE (0)
#define     NFP_MSFCL_MSCL_MUX_SEL_MSCL_SDMUX_RX_SWZL_LAKEN_MODE (15)
#define   NFP_MSFCL_MSCL_MUX_SEL_MSCL_MUX_SEL(_x)       ((_x) & 0x3)
#define   NFP_MSFCL_MSCL_MUX_SEL_MSCL_MUX_SEL_of(_x)    ((_x) & 0x3)
#define     NFP_MSFCL_MSCL_MUX_SEL_MSCL_MUX_SEL_XAUI_MODE (0)
#define     NFP_MSFCL_MSCL_MUX_SEL_MSCL_MUX_SEL_LAKEN_MODE (1)
#define     NFP_MSFCL_MSCL_MUX_SEL_MSCL_MUX_SEL_SPI_MODE (2)
/* Register Type: MsclPhy6gClkCtrl */
#define NFP_MSFCL_PHY_CLK_CTRL         0x000c
#define   NFP_MSFCL_PHY_CLK_CTRL_C_SCR_MODE             (0x1 << 13)
#define   NFP_MSFCL_PHY_CLK_CTRL_C_VCO_CAL              (0x1 << 12)
#define   NFP_MSFCL_PHY_CLK_CTRL_C_RES_CAL              (0x1 << 11)
#define   NFP_MSFCL_PHY_CLK_CTRL_C_RST                  (0x1 << 10)
#define   NFP_MSFCL_PHY_CLK_CTRL_C_SYSCLK_EN            (0x1 << 9)
#define   NFP_MSFCL_PHY_CLK_CTRL_C_SYSCLK_TERM_SEL      (0x1 << 8)
#define   NFP_MSFCL_PHY_CLK_CTRL_C_SYSCLK_OUT_CML_EN    (0x1 << 7)
#define   NFP_MSFCL_PHY_CLK_CTRL_C_SYSCLK_SEL           (0x1 << 6)
#define   NFP_MSFCL_PHY_CLK_CTRL_C_SYSCLK_DIV2_SEL      (0x1 << 5)
#define   NFP_MSFCL_PHY_CLK_CTRL_C_SYSCLK_RATE(_x)      (((_x) & 0x7) << 2)
#define   NFP_MSFCL_PHY_CLK_CTRL_C_SYSCLK_RATE_of(_x)   (((_x) >> 2) & 0x7)
#define   NFP_MSFCL_PHY_CLK_CTRL_C_CLKBUF_R_EN          (0x1 << 1)
#define   NFP_MSFCL_PHY_CLK_CTRL_C_CLKBUF_L_EN          (0x1)
/* Register Type: MsclPhy6gClkStat */
#define NFP_MSFCL_PHY_CLK_STATUS       0x0010
#define   NFP_MSFCL_PHY_CLK_STATUS_C_VCO_CAL_DONE       (0x1 << 3)
#define   NFP_MSFCL_PHY_CLK_STATUS_C_RES_CAL_DONE       (0x1 << 2)
#define   NFP_MSFCL_PHY_CLK_STATUS_C_CMULOCK            (0x1 << 1)
#define   NFP_MSFCL_PHY_CLK_STATUS_C_RDY                (0x1)
/* Register Type: MsclPhy6gTxSync */
#define NFP_MSFCL_PHY_TX_SYNC          0x0014
#define   NFP_MSFCL_PHY_TX_SYNC_C_TSYNC_CLK_MODE        (0x1 << 1)
#define   NFP_MSFCL_PHY_TX_SYNC_C_RST_TSYNC             (0x1)
/* Register Type: MsclPhy6gGlblEn */
#define NFP_MSFCL_PHY_GLOBAL_ENS       0x0018
#define   NFP_MSFCL_PHY_GLOBAL_ENS_C_BIAS_EN            (0x1 << 2)
#define   NFP_MSFCL_PHY_GLOBAL_ENS_C_CMU_EN             (0x1 << 1)
#define   NFP_MSFCL_PHY_GLOBAL_ENS_C_IDDQ               (0x1)
/* Register Type: MsclPhy6gClkbufEnParRate */
#define NFP_MSFCL_PHY_CLKBUF_EN_PAR_RATE 0x001c
#define   NFP_MSFCL_PHY_CLKBUF_EN_PAR_RATE_L3_PHY_CLKBUF_EN (0x1 << 11)
#define   NFP_MSFCL_PHY_CLKBUF_EN_PAR_RATE_L2_PHY_CLKBUF_EN (0x1 << 10)
#define   NFP_MSFCL_PHY_CLKBUF_EN_PAR_RATE_L1_PHY_CLKBUF_EN (0x1 << 9)
#define   NFP_MSFCL_PHY_CLKBUF_EN_PAR_RATE_L0_PHY_CLKBUF_EN (0x1 << 8)
#define   NFP_MSFCL_PHY_CLKBUF_EN_PAR_RATE_L3_PHY_PAR_RATE (0x1 << 3)
#define     NFP_MSFCL_PHY_CLKBUF_EN_PAR_RATE_L3_PHY_PAR_RATE_8B_10B (0x0)
#define     NFP_MSFCL_PHY_CLKBUF_EN_PAR_RATE_L3_PHY_PAR_RATE_16B_20B (0x8)
#define   NFP_MSFCL_PHY_CLKBUF_EN_PAR_RATE_L2_PHY_PAR_RATE (0x1 << 2)
#define     NFP_MSFCL_PHY_CLKBUF_EN_PAR_RATE_L2_PHY_PAR_RATE_8B_10B (0x0)
#define     NFP_MSFCL_PHY_CLKBUF_EN_PAR_RATE_L2_PHY_PAR_RATE_16B_20B (0x4)
#define   NFP_MSFCL_PHY_CLKBUF_EN_PAR_RATE_L1_PHY_PAR_RATE (0x1 << 1)
#define     NFP_MSFCL_PHY_CLKBUF_EN_PAR_RATE_L1_PHY_PAR_RATE_8B_10B (0x0)
#define     NFP_MSFCL_PHY_CLKBUF_EN_PAR_RATE_L1_PHY_PAR_RATE_16B_20B (0x2)
#define   NFP_MSFCL_PHY_CLKBUF_EN_PAR_RATE_L0_PHY_PAR_RATE (0x1)
#define     NFP_MSFCL_PHY_CLKBUF_EN_PAR_RATE_L0_PHY_PAR_RATE_8B_10B (0x0)
#define     NFP_MSFCL_PHY_CLKBUF_EN_PAR_RATE_L0_PHY_PAR_RATE_16B_20B (0x1)
/* Register Type: MsclPhy6gTxBnd */
#define NFP_MSFCL_PHY_TX_BAND          0x0020
#define   NFP_MSFCL_PHY_TX_BAND_L3_PHY_TX_BAND(_x)      (((_x) & 0x3) << 6)
#define   NFP_MSFCL_PHY_TX_BAND_L3_PHY_TX_BAND_of(_x)   (((_x) >> 6) & 0x3)
#define     NFP_MSFCL_PHY_TX_BAND_L3_PHY_TX_BAND_FULL   (0)
#define     NFP_MSFCL_PHY_TX_BAND_L3_PHY_TX_BAND_HALF   (1)
#define     NFP_MSFCL_PHY_TX_BAND_L3_PHY_TX_BAND_QUARTER (3)
#define   NFP_MSFCL_PHY_TX_BAND_L2_PHY_TX_BAND(_x)      (((_x) & 0x3) << 4)
#define   NFP_MSFCL_PHY_TX_BAND_L2_PHY_TX_BAND_of(_x)   (((_x) >> 4) & 0x3)
#define     NFP_MSFCL_PHY_TX_BAND_L2_PHY_TX_BAND_FULL   (0)
#define     NFP_MSFCL_PHY_TX_BAND_L2_PHY_TX_BAND_HALF   (1)
#define     NFP_MSFCL_PHY_TX_BAND_L2_PHY_TX_BAND_QUARTER (3)
#define   NFP_MSFCL_PHY_TX_BAND_L1_PHY_TX_BAND(_x)      (((_x) & 0x3) << 2)
#define   NFP_MSFCL_PHY_TX_BAND_L1_PHY_TX_BAND_of(_x)   (((_x) >> 2) & 0x3)
#define     NFP_MSFCL_PHY_TX_BAND_L1_PHY_TX_BAND_FULL   (0)
#define     NFP_MSFCL_PHY_TX_BAND_L1_PHY_TX_BAND_HALF   (1)
#define     NFP_MSFCL_PHY_TX_BAND_L1_PHY_TX_BAND_QUARTER (3)
#define   NFP_MSFCL_PHY_TX_BAND_L0_PHY_TX_BAND(_x)      ((_x) & 0x3)
#define   NFP_MSFCL_PHY_TX_BAND_L0_PHY_TX_BAND_of(_x)   ((_x) & 0x3)
#define     NFP_MSFCL_PHY_TX_BAND_L0_PHY_TX_BAND_FULL   (0)
#define     NFP_MSFCL_PHY_TX_BAND_L0_PHY_TX_BAND_HALF   (1)
#define     NFP_MSFCL_PHY_TX_BAND_L0_PHY_TX_BAND_QUARTER (3)
/* Register Type: MsclPhy6gTxEnTclkInEn */
#define NFP_MSFCL_PHY_TX_EN_TCLK_IN_EN 0x0024
#define   NFP_MSFCL_PHY_TX_EN_TCLK_IN_EN_L3_PHY_TX_EN   (0x1 << 11)
#define   NFP_MSFCL_PHY_TX_EN_TCLK_IN_EN_L2_PHY_TX_EN   (0x1 << 10)
#define   NFP_MSFCL_PHY_TX_EN_TCLK_IN_EN_L1_PHY_TX_EN   (0x1 << 9)
#define   NFP_MSFCL_PHY_TX_EN_TCLK_IN_EN_L0_PHY_TX_EN   (0x1 << 8)
#define   NFP_MSFCL_PHY_TX_EN_TCLK_IN_EN_L3_PHY_TCLK_IN_EN (0x1 << 3)
#define   NFP_MSFCL_PHY_TX_EN_TCLK_IN_EN_L2_PHY_TCLK_IN_EN (0x1 << 2)
#define   NFP_MSFCL_PHY_TX_EN_TCLK_IN_EN_L1_PHY_TCLK_IN_EN (0x1 << 1)
#define   NFP_MSFCL_PHY_TX_EN_TCLK_IN_EN_L0_PHY_TCLK_IN_EN (0x1)
/* Register Type: MsclPhy6gResetTsyncTclkSync */
#define NFP_MSFCL_PHY_RST_TSYNC_TCLK_SYNC 0x0028
#define   NFP_MSFCL_PHY_RST_TSYNC_TCLK_SYNC_L3_PHY_RST_TSYNC_EN (0x1 << 11)
#define   NFP_MSFCL_PHY_RST_TSYNC_TCLK_SYNC_L2_PHY_RST_TSYNC_EN (0x1 << 10)
#define   NFP_MSFCL_PHY_RST_TSYNC_TCLK_SYNC_L1_PHY_RST_TSYNC_EN (0x1 << 9)
#define   NFP_MSFCL_PHY_RST_TSYNC_TCLK_SYNC_L0_PHY_RST_TSYNC_EN (0x1 << 8)
#define   NFP_MSFCL_PHY_RST_TSYNC_TCLK_SYNC_L3_PHY_TCLK_SYNC (0x1 << 3)
#define   NFP_MSFCL_PHY_RST_TSYNC_TCLK_SYNC_L2_PHY_TCLK_SYNC (0x1 << 2)
#define   NFP_MSFCL_PHY_RST_TSYNC_TCLK_SYNC_L1_PHY_TCLK_SYNC (0x1 << 1)
#define   NFP_MSFCL_PHY_RST_TSYNC_TCLK_SYNC_L0_PHY_TCLK_SYNC (0x1)
/* Register Type: MsclPhy6g10CursorEmp */
#define NFP_MSFCL_PHY_10_CURSOR_EMP    0x002c
#define   NFP_MSFCL_PHY_10_CURSOR_EMP_L1_PHY_EMP_PRE(_x) (((_x) & 0x7) << 13)
#define   NFP_MSFCL_PHY_10_CURSOR_EMP_L1_PHY_EMP_PRE_of(_x) (((_x) >> 13) & 0x7)
#define   NFP_MSFCL_PHY_10_CURSOR_EMP_L1_PHY_EMP_POST(_x) (((_x) & 0x1f) << 8)
#define   NFP_MSFCL_PHY_10_CURSOR_EMP_L1_PHY_EMP_POST_of(_x) (((_x) >> 8) & 0x1f)
#define   NFP_MSFCL_PHY_10_CURSOR_EMP_L0_PHY_EMP_PRE(_x) (((_x) & 0x7) << 5)
#define   NFP_MSFCL_PHY_10_CURSOR_EMP_L0_PHY_EMP_PRE_of(_x) (((_x) >> 5) & 0x7)
#define   NFP_MSFCL_PHY_10_CURSOR_EMP_L0_PHY_EMP_POST(_x) ((_x) & 0x1f)
#define   NFP_MSFCL_PHY_10_CURSOR_EMP_L0_PHY_EMP_POST_of(_x) ((_x) & 0x1f)
/* Register Type: MsclPhy6g32CursorEmp */
#define NFP_MSFCL_PHY_32_CURSOR_EMP    0x0030
#define   NFP_MSFCL_PHY_32_CURSOR_EMP_L3_PHY_EMP_PRE(_x) (((_x) & 0x7) << 13)
#define   NFP_MSFCL_PHY_32_CURSOR_EMP_L3_PHY_EMP_PRE_of(_x) (((_x) >> 13) & 0x7)
#define   NFP_MSFCL_PHY_32_CURSOR_EMP_L3_PHY_EMP_POST(_x) (((_x) & 0x1f) << 8)
#define   NFP_MSFCL_PHY_32_CURSOR_EMP_L3_PHY_EMP_POST_of(_x) (((_x) >> 8) & 0x1f)
#define   NFP_MSFCL_PHY_32_CURSOR_EMP_L2_PHY_EMP_PRE(_x) (((_x) & 0x7) << 5)
#define   NFP_MSFCL_PHY_32_CURSOR_EMP_L2_PHY_EMP_PRE_of(_x) (((_x) >> 5) & 0x7)
#define   NFP_MSFCL_PHY_32_CURSOR_EMP_L2_PHY_EMP_POST(_x) ((_x) & 0x1f)
#define   NFP_MSFCL_PHY_32_CURSOR_EMP_L2_PHY_EMP_POST_of(_x) ((_x) & 0x1f)
/* Register Type: MsclPhy6g10DrvLvl */
#define NFP_MSFCL_PHY_10_DRV_LVL       0x0034
#define   NFP_MSFCL_PHY_10_DRV_LVL_L1_PHY_DRV_LVL(_x)   (((_x) & 0x1f) << 8)
#define   NFP_MSFCL_PHY_10_DRV_LVL_L1_PHY_DRV_LVL_of(_x) (((_x) >> 8) & 0x1f)
#define   NFP_MSFCL_PHY_10_DRV_LVL_L0_PHY_DRV_LVL(_x)   ((_x) & 0x1f)
#define   NFP_MSFCL_PHY_10_DRV_LVL_L0_PHY_DRV_LVL_of(_x) ((_x) & 0x1f)
/* Register Type: MsclPhy6g32DrvLvl */
#define NFP_MSFCL_PHY_32_DRV_LVL       0x0038
#define   NFP_MSFCL_PHY_32_DRV_LVL_L3_PHY_DRV_LVL(_x)   (((_x) & 0x1f) << 8)
#define   NFP_MSFCL_PHY_32_DRV_LVL_L3_PHY_DRV_LVL_of(_x) (((_x) >> 8) & 0x1f)
#define   NFP_MSFCL_PHY_32_DRV_LVL_L2_PHY_DRV_LVL(_x)   ((_x) & 0x1f)
#define   NFP_MSFCL_PHY_32_DRV_LVL_L2_PHY_DRV_LVL_of(_x) ((_x) & 0x1f)
/* Register Type: MsclPhy6g10LnMod */
#define NFP_MSFCL_PHY_10_LANE_MODE     0x003c
#define   NFP_MSFCL_PHY_10_LANE_MODE_L1_PHY_LANE_MODE(_x) (((_x) & 0xff) << 8)
#define   NFP_MSFCL_PHY_10_LANE_MODE_L1_PHY_LANE_MODE_of(_x) (((_x) >> 8) & 0xff)
#define   NFP_MSFCL_PHY_10_LANE_MODE_L0_PHY_LANE_MODE(_x) ((_x) & 0xff)
#define   NFP_MSFCL_PHY_10_LANE_MODE_L0_PHY_LANE_MODE_of(_x) ((_x) & 0xff)
/* Register Type: MsclPhy6g32LnMod */
#define NFP_MSFCL_PHY_32_LANE_MODE     0x0040
#define   NFP_MSFCL_PHY_32_LANE_MODE_L3_PHY_LANE_MODE(_x) (((_x) & 0xff) << 8)
#define   NFP_MSFCL_PHY_32_LANE_MODE_L3_PHY_LANE_MODE_of(_x) (((_x) >> 8) & 0xff)
#define   NFP_MSFCL_PHY_32_LANE_MODE_L2_PHY_LANE_MODE(_x) ((_x) & 0xff)
#define   NFP_MSFCL_PHY_32_LANE_MODE_L2_PHY_LANE_MODE_of(_x) ((_x) & 0xff)
/* Register Type: MsclPhy6gDrvEnTxHiZ */
#define NFP_MSFCL_PHY_DRV_EN_TX_HIGHZ  0x0044
#define   NFP_MSFCL_PHY_DRV_EN_TX_HIGHZ_L3_PHY_DRV_EN   (0x1 << 11)
#define   NFP_MSFCL_PHY_DRV_EN_TX_HIGHZ_L2_PHY_DRV_EN   (0x1 << 10)
#define   NFP_MSFCL_PHY_DRV_EN_TX_HIGHZ_L1_PHY_DRV_EN   (0x1 << 9)
#define   NFP_MSFCL_PHY_DRV_EN_TX_HIGHZ_L0_PHY_DRV_EN   (0x1 << 8)
#define   NFP_MSFCL_PHY_DRV_EN_TX_HIGHZ_L3_PHY_TX_HIGHZ (0x1 << 3)
#define   NFP_MSFCL_PHY_DRV_EN_TX_HIGHZ_L2_PHY_TX_HIGHZ (0x1 << 2)
#define   NFP_MSFCL_PHY_DRV_EN_TX_HIGHZ_L1_PHY_TX_HIGHZ (0x1 << 1)
#define   NFP_MSFCL_PHY_DRV_EN_TX_HIGHZ_L0_PHY_TX_HIGHZ (0x1)
/* Register Type: MsclPhy6gRxBnd */
#define NFP_MSFCL_PHY_RX_BAND          0x0048
#define   NFP_MSFCL_PHY_RX_BAND_L3_PHY_RX_BAND(_x)      (((_x) & 0x3) << 6)
#define   NFP_MSFCL_PHY_RX_BAND_L3_PHY_RX_BAND_of(_x)   (((_x) >> 6) & 0x3)
#define     NFP_MSFCL_PHY_RX_BAND_L3_PHY_RX_BAND_FULL   (0)
#define     NFP_MSFCL_PHY_RX_BAND_L3_PHY_RX_BAND_HALF   (1)
#define     NFP_MSFCL_PHY_RX_BAND_L3_PHY_RX_BAND_QUARTER (3)
#define   NFP_MSFCL_PHY_RX_BAND_L2_PHY_RX_BAND(_x)      (((_x) & 0x3) << 4)
#define   NFP_MSFCL_PHY_RX_BAND_L2_PHY_RX_BAND_of(_x)   (((_x) >> 4) & 0x3)
#define     NFP_MSFCL_PHY_RX_BAND_L2_PHY_RX_BAND_FULL   (0)
#define     NFP_MSFCL_PHY_RX_BAND_L2_PHY_RX_BAND_HALF   (1)
#define     NFP_MSFCL_PHY_RX_BAND_L2_PHY_RX_BAND_QUARTER (3)
#define   NFP_MSFCL_PHY_RX_BAND_L1_PHY_RX_BAND(_x)      (((_x) & 0x3) << 2)
#define   NFP_MSFCL_PHY_RX_BAND_L1_PHY_RX_BAND_of(_x)   (((_x) >> 2) & 0x3)
#define     NFP_MSFCL_PHY_RX_BAND_L1_PHY_RX_BAND_FULL   (0)
#define     NFP_MSFCL_PHY_RX_BAND_L1_PHY_RX_BAND_HALF   (1)
#define     NFP_MSFCL_PHY_RX_BAND_L1_PHY_RX_BAND_QUARTER (3)
#define   NFP_MSFCL_PHY_RX_BAND_L0_PHY_RX_BAND(_x)      ((_x) & 0x3)
#define   NFP_MSFCL_PHY_RX_BAND_L0_PHY_RX_BAND_of(_x)   ((_x) & 0x3)
#define     NFP_MSFCL_PHY_RX_BAND_L0_PHY_RX_BAND_FULL   (0)
#define     NFP_MSFCL_PHY_RX_BAND_L0_PHY_RX_BAND_HALF   (1)
#define     NFP_MSFCL_PHY_RX_BAND_L0_PHY_RX_BAND_QUARTER (3)
/* Register Type: MsclPhy6gRxEn */
#define NFP_MSFCL_PHY_RX_EN            0x004c
#define   NFP_MSFCL_PHY_RX_EN_L3_PHY_RX_EN              (0x1 << 3)
#define   NFP_MSFCL_PHY_RX_EN_L2_PHY_RX_EN              (0x1 << 2)
#define   NFP_MSFCL_PHY_RX_EN_L1_PHY_RX_EN              (0x1 << 1)
#define   NFP_MSFCL_PHY_RX_EN_L0_PHY_RX_EN              (0x1)
/* Register Type: MsclPhy6gCdrRstRstRsync */
#define NFP_MSFCL_PHY_CDR_RST_RST_RSYNC 0x0050
#define   NFP_MSFCL_PHY_CDR_RST_RST_RSYNC_L3_PHY_CDR_RST (0x1 << 11)
#define   NFP_MSFCL_PHY_CDR_RST_RST_RSYNC_L2_PHY_CDR_RST (0x1 << 10)
#define   NFP_MSFCL_PHY_CDR_RST_RST_RSYNC_L1_PHY_CDR_RST (0x1 << 9)
#define   NFP_MSFCL_PHY_CDR_RST_RST_RSYNC_L0_PHY_CDR_RST (0x1 << 8)
#define   NFP_MSFCL_PHY_CDR_RST_RST_RSYNC_L3_PHY_RST_RSYNC (0x1 << 3)
#define   NFP_MSFCL_PHY_CDR_RST_RST_RSYNC_L2_PHY_RST_RSYNC (0x1 << 2)
#define   NFP_MSFCL_PHY_CDR_RST_RST_RSYNC_L1_PHY_RST_RSYNC (0x1 << 1)
#define   NFP_MSFCL_PHY_CDR_RST_RST_RSYNC_L0_PHY_RST_RSYNC (0x1)
/* Register Type: MsclPhy6gCdrFreqloopEn */
#define NFP_MSFCL_PHY_CDR_FREQLOOP_EN  0x0054
#define   NFP_MSFCL_PHY_CDR_FREQLOOP_EN_L3_PHY_CDR_FREQLOOP_EN (0x1 << 3)
#define   NFP_MSFCL_PHY_CDR_FREQLOOP_EN_L2_PHY_CDR_FREQLOOP_EN (0x1 << 2)
#define   NFP_MSFCL_PHY_CDR_FREQLOOP_EN_L1_PHY_CDR_FREQLOOP_EN (0x1 << 1)
#define   NFP_MSFCL_PHY_CDR_FREQLOOP_EN_L0_PHY_CDR_FREQLOOP_EN (0x1)
/* Register Type: MsclPhy6gCdrFreqloopGain */
#define NFP_MSFCL_PHY_CDR_FREQLOOP_GAIN 0x0058
#define   NFP_MSFCL_PHY_CDR_FREQLOOP_GAIN_L3_PHY_CDR_FREQLOOP_GAIN(_x) (((_x) & 0x3) << 6)
#define   NFP_MSFCL_PHY_CDR_FREQLOOP_GAIN_L3_PHY_CDR_FREQLOOP_GAIN_of(_x) (((_x) >> 6) & 0x3)
#define   NFP_MSFCL_PHY_CDR_FREQLOOP_GAIN_L2_PHY_CDR_FREQLOOP_GAIN(_x) (((_x) & 0x3) << 4)
#define   NFP_MSFCL_PHY_CDR_FREQLOOP_GAIN_L2_PHY_CDR_FREQLOOP_GAIN_of(_x) (((_x) >> 4) & 0x3)
#define   NFP_MSFCL_PHY_CDR_FREQLOOP_GAIN_L1_PHY_CDR_FREQLOOP_GAIN(_x) (((_x) & 0x3) << 2)
#define   NFP_MSFCL_PHY_CDR_FREQLOOP_GAIN_L1_PHY_CDR_FREQLOOP_GAIN_of(_x) (((_x) >> 2) & 0x3)
#define   NFP_MSFCL_PHY_CDR_FREQLOOP_GAIN_L0_PHY_CDR_FREQLOOP_GAIN(_x) ((_x) & 0x3)
#define   NFP_MSFCL_PHY_CDR_FREQLOOP_GAIN_L0_PHY_CDR_FREQLOOP_GAIN_of(_x) ((_x) & 0x3)
/* Register Type: MsclPhy6gCdrThresh */
#define NFP_MSFCL_PHY_CDR_THRESHOLD    0x005c
#define   NFP_MSFCL_PHY_CDR_THRESHOLD_L3_PHY_CDR_THRESHOLD(_x) (((_x) & 0x3) << 6)
#define   NFP_MSFCL_PHY_CDR_THRESHOLD_L3_PHY_CDR_THRESHOLD_of(_x) (((_x) >> 6) & 0x3)
#define   NFP_MSFCL_PHY_CDR_THRESHOLD_L2_PHY_CDR_THRESHOLD(_x) (((_x) & 0x3) << 4)
#define   NFP_MSFCL_PHY_CDR_THRESHOLD_L2_PHY_CDR_THRESHOLD_of(_x) (((_x) >> 4) & 0x3)
#define   NFP_MSFCL_PHY_CDR_THRESHOLD_L1_PHY_CDR_THRESHOLD(_x) (((_x) & 0x3) << 2)
#define   NFP_MSFCL_PHY_CDR_THRESHOLD_L1_PHY_CDR_THRESHOLD_of(_x) (((_x) >> 2) & 0x3)
#define   NFP_MSFCL_PHY_CDR_THRESHOLD_L0_PHY_CDR_THRESHOLD(_x) ((_x) & 0x3)
#define   NFP_MSFCL_PHY_CDR_THRESHOLD_L0_PHY_CDR_THRESHOLD_of(_x) ((_x) & 0x3)
/* Register Type: MsclPhy6gCdrFreeze */
#define NFP_MSFCL_PHY_CDR_FREEZE       0x0060
#define   NFP_MSFCL_PHY_CDR_FREEZE_L3_PHY_CDR_FREEZE    (0x1 << 3)
#define   NFP_MSFCL_PHY_CDR_FREEZE_L2_PHY_CDR_FREEZE    (0x1 << 2)
#define   NFP_MSFCL_PHY_CDR_FREEZE_L1_PHY_CDR_FREEZE    (0x1 << 1)
#define   NFP_MSFCL_PHY_CDR_FREEZE_L0_PHY_CDR_FREEZE    (0x1)
/* Register Type: MsclPhy6gCdrDnUp */
#define NFP_MSFCL_PHY_CDR_DN_UP        0x0064
#define   NFP_MSFCL_PHY_CDR_DN_UP_L3_PHY_CDR_DN         (0x1 << 11)
#define   NFP_MSFCL_PHY_CDR_DN_UP_L2_PHY_CDR_DN         (0x1 << 10)
#define   NFP_MSFCL_PHY_CDR_DN_UP_L1_PHY_CDR_DN         (0x1 << 9)
#define   NFP_MSFCL_PHY_CDR_DN_UP_L0_PHY_CDR_DN         (0x1 << 8)
#define   NFP_MSFCL_PHY_CDR_DN_UP_L3_PHY_CDR_UP         (0x1 << 3)
#define   NFP_MSFCL_PHY_CDR_DN_UP_L2_PHY_CDR_UP         (0x1 << 2)
#define   NFP_MSFCL_PHY_CDR_DN_UP_L1_PHY_CDR_UP         (0x1 << 1)
#define   NFP_MSFCL_PHY_CDR_DN_UP_L0_PHY_CDR_UP         (0x1)
/* Register Type: MsclPhy6gRxSigdetEn */
#define NFP_MSFCL_PHY_RX_SIGDET_EN     0x0068
#define   NFP_MSFCL_PHY_RX_SIGDET_EN_L3_PHY_RX_SIGDET_EN (0x1 << 3)
#define   NFP_MSFCL_PHY_RX_SIGDET_EN_L2_PHY_RX_SIGDET_EN (0x1 << 2)
#define   NFP_MSFCL_PHY_RX_SIGDET_EN_L1_PHY_RX_SIGDET_EN (0x1 << 1)
#define   NFP_MSFCL_PHY_RX_SIGDET_EN_L0_PHY_RX_SIGDET_EN (0x1)
/* Register Type: MsclPhy6gRxSigdetLvl */
#define NFP_MSFCL_PHY_RX_SIGDET_LVL    0x006c
#define   NFP_MSFCL_PHY_RX_SIGDET_LVL_L3_PHY_RX_SIGDET_LVL(_x) (((_x) & 0x3) << 6)
#define   NFP_MSFCL_PHY_RX_SIGDET_LVL_L3_PHY_RX_SIGDET_LVL_of(_x) (((_x) >> 6) & 0x3)
#define   NFP_MSFCL_PHY_RX_SIGDET_LVL_L2_PHY_RX_SIGDET_LVL(_x) (((_x) & 0x3) << 4)
#define   NFP_MSFCL_PHY_RX_SIGDET_LVL_L2_PHY_RX_SIGDET_LVL_of(_x) (((_x) >> 4) & 0x3)
#define   NFP_MSFCL_PHY_RX_SIGDET_LVL_L1_PHY_RX_SIGDET_LVL(_x) (((_x) & 0x3) << 2)
#define   NFP_MSFCL_PHY_RX_SIGDET_LVL_L1_PHY_RX_SIGDET_LVL_of(_x) (((_x) >> 2) & 0x3)
#define   NFP_MSFCL_PHY_RX_SIGDET_LVL_L0_PHY_RX_SIGDET_LVL(_x) ((_x) & 0x3)
#define   NFP_MSFCL_PHY_RX_SIGDET_LVL_L0_PHY_RX_SIGDET_LVL_of(_x) ((_x) & 0x3)
/* Register Type: MsclPhy6gRxSigdet */
#define NFP_MSFCL_PHY_RX_SIGDET        0x0070
#define   NFP_MSFCL_PHY_RX_SIGDET_L3_PHY_RX_SIGDET      (0x1 << 3)
#define   NFP_MSFCL_PHY_RX_SIGDET_L2_PHY_RX_SIGDET      (0x1 << 2)
#define   NFP_MSFCL_PHY_RX_SIGDET_L1_PHY_RX_SIGDET      (0x1 << 1)
#define   NFP_MSFCL_PHY_RX_SIGDET_L0_PHY_RX_SIGDET      (0x1)
/* Register Type: MsclPhy6gRxEq */
#define NFP_MSFCL_PHY_RX_EQ            0x0074
#define   NFP_MSFCL_PHY_RX_EQ_L3_PHY_RX_EQ(_x)          (((_x) & 0xf) << 12)
#define   NFP_MSFCL_PHY_RX_EQ_L3_PHY_RX_EQ_of(_x)       (((_x) >> 12) & 0xf)
#define   NFP_MSFCL_PHY_RX_EQ_L2_PHY_RX_EQ(_x)          (((_x) & 0xf) << 8)
#define   NFP_MSFCL_PHY_RX_EQ_L2_PHY_RX_EQ_of(_x)       (((_x) >> 8) & 0xf)
#define   NFP_MSFCL_PHY_RX_EQ_L1_PHY_RX_EQ(_x)          (((_x) & 0xf) << 4)
#define   NFP_MSFCL_PHY_RX_EQ_L1_PHY_RX_EQ_of(_x)       (((_x) >> 4) & 0xf)
#define   NFP_MSFCL_PHY_RX_EQ_L0_PHY_RX_EQ(_x)          ((_x) & 0xf)
#define   NFP_MSFCL_PHY_RX_EQ_L0_PHY_RX_EQ_of(_x)       ((_x) & 0xf)
/* Register Type: MsclPhy6gRxCmRxAcCouple */
#define NFP_MSFCL_PHY_RX_CM_RX_AC_COUPLE 0x0078
#define   NFP_MSFCL_PHY_RX_CM_RX_AC_COUPLE_L3_PHY_RX_CM (0x1 << 11)
#define   NFP_MSFCL_PHY_RX_CM_RX_AC_COUPLE_L2_PHY_RX_CM (0x1 << 10)
#define   NFP_MSFCL_PHY_RX_CM_RX_AC_COUPLE_L1_PHY_RX_CM (0x1 << 9)
#define   NFP_MSFCL_PHY_RX_CM_RX_AC_COUPLE_L0_PHY_RX_CM (0x1 << 8)
#define   NFP_MSFCL_PHY_RX_CM_RX_AC_COUPLE_L3_PHY_RX_AC_COUPLE (0x1 << 3)
#define   NFP_MSFCL_PHY_RX_CM_RX_AC_COUPLE_L2_PHY_RX_AC_COUPLE (0x1 << 2)
#define   NFP_MSFCL_PHY_RX_CM_RX_AC_COUPLE_L1_PHY_RX_AC_COUPLE (0x1 << 1)
#define   NFP_MSFCL_PHY_RX_CM_RX_AC_COUPLE_L0_PHY_RX_AC_COUPLE (0x1)
/* Register Type: MsclPhy6gRxHiZRxTerm */
#define NFP_MSFCL_PHY_RX_HIGHZ_RX_TERM 0x007c
#define   NFP_MSFCL_PHY_RX_HIGHZ_RX_TERM_L3_PHY_RX_HIGHZ (0x1 << 11)
#define   NFP_MSFCL_PHY_RX_HIGHZ_RX_TERM_L2_PHY_RX_HIGHZ (0x1 << 10)
#define   NFP_MSFCL_PHY_RX_HIGHZ_RX_TERM_L1_PHY_RX_HIGHZ (0x1 << 9)
#define   NFP_MSFCL_PHY_RX_HIGHZ_RX_TERM_L0_PHY_RX_HIGHZ (0x1 << 8)
#define   NFP_MSFCL_PHY_RX_HIGHZ_RX_TERM_L3_PHY_RX_TERM (0x1 << 3)
#define   NFP_MSFCL_PHY_RX_HIGHZ_RX_TERM_L2_PHY_RX_TERM (0x1 << 2)
#define   NFP_MSFCL_PHY_RX_HIGHZ_RX_TERM_L1_PHY_RX_TERM (0x1 << 1)
#define   NFP_MSFCL_PHY_RX_HIGHZ_RX_TERM_L0_PHY_RX_TERM (0x1)
/* Register Type: MsclPhy6gBistRxRstTxRst */
#define NFP_MSFCL_PHY_BIST_RX_RST_TX_RST 0x0080
#define   NFP_MSFCL_PHY_BIST_RX_RST_TX_RST_L3_PHY_BIST_RX_RST (0x1 << 11)
#define   NFP_MSFCL_PHY_BIST_RX_RST_TX_RST_L2_PHY_BIST_RX_RST (0x1 << 10)
#define   NFP_MSFCL_PHY_BIST_RX_RST_TX_RST_L1_PHY_BIST_RX_RST (0x1 << 9)
#define   NFP_MSFCL_PHY_BIST_RX_RST_TX_RST_L0_PHY_BIST_RX_RST (0x1 << 8)
#define   NFP_MSFCL_PHY_BIST_RX_RST_TX_RST_L3_PHY_BIST_TX_RST (0x1 << 3)
#define   NFP_MSFCL_PHY_BIST_RX_RST_TX_RST_L2_PHY_BIST_TX_RST (0x1 << 2)
#define   NFP_MSFCL_PHY_BIST_RX_RST_TX_RST_L1_PHY_BIST_TX_RST (0x1 << 1)
#define   NFP_MSFCL_PHY_BIST_RX_RST_TX_RST_L0_PHY_BIST_TX_RST (0x1)
/* Register Type: MsclPhy6gBistForceErrBerClear */
#define NFP_MSFCL_PHY_BIST_FORCE_ERR_BER_CLR 0x0084
#define   NFP_MSFCL_PHY_BIST_FORCE_ERR_BER_CLR_L3_PHY_BIST_FORCE_ERR (0x1 << 11)
#define   NFP_MSFCL_PHY_BIST_FORCE_ERR_BER_CLR_L2_PHY_BIST_FORCE_ERR (0x1 << 10)
#define   NFP_MSFCL_PHY_BIST_FORCE_ERR_BER_CLR_L1_PHY_BIST_FORCE_ERR (0x1 << 9)
#define   NFP_MSFCL_PHY_BIST_FORCE_ERR_BER_CLR_L0_PHY_BIST_FORCE_ERR (0x1 << 8)
#define   NFP_MSFCL_PHY_BIST_FORCE_ERR_BER_CLR_L3_PHY_BIST_BER_CLR (0x1 << 3)
#define   NFP_MSFCL_PHY_BIST_FORCE_ERR_BER_CLR_L2_PHY_BIST_BER_CLR (0x1 << 2)
#define   NFP_MSFCL_PHY_BIST_FORCE_ERR_BER_CLR_L1_PHY_BIST_BER_CLR (0x1 << 1)
#define   NFP_MSFCL_PHY_BIST_FORCE_ERR_BER_CLR_L0_PHY_BIST_BER_CLR (0x1)
/* Register Type: MsclPhy6gBistMode */
#define NFP_MSFCL_PHY_BIST_MODE        0x0088
#define   NFP_MSFCL_PHY_BIST_MODE_L3_PHY_BIST_MODE(_x)  (((_x) & 0xf) << 12)
#define   NFP_MSFCL_PHY_BIST_MODE_L3_PHY_BIST_MODE_of(_x) (((_x) >> 12) & 0xf)
#define   NFP_MSFCL_PHY_BIST_MODE_L2_PHY_BIST_MODE(_x)  (((_x) & 0xf) << 8)
#define   NFP_MSFCL_PHY_BIST_MODE_L2_PHY_BIST_MODE_of(_x) (((_x) >> 8) & 0xf)
#define   NFP_MSFCL_PHY_BIST_MODE_L1_PHY_BIST_MODE(_x)  (((_x) & 0xf) << 4)
#define   NFP_MSFCL_PHY_BIST_MODE_L1_PHY_BIST_MODE_of(_x) (((_x) >> 4) & 0xf)
#define   NFP_MSFCL_PHY_BIST_MODE_L0_PHY_BIST_MODE(_x)  ((_x) & 0xf)
#define   NFP_MSFCL_PHY_BIST_MODE_L0_PHY_BIST_MODE_of(_x) ((_x) & 0xf)
/* Register Type: MsclPhy6gBistTogBistErr */
#define NFP_MSFCL_PHY_BIST_TOG_BIST_ERR 0x008c
#define   NFP_MSFCL_PHY_BIST_TOG_BIST_ERR_L3_PHY_BIST_TOG (0x1 << 11)
#define   NFP_MSFCL_PHY_BIST_TOG_BIST_ERR_L2_PHY_BIST_TOG (0x1 << 10)
#define   NFP_MSFCL_PHY_BIST_TOG_BIST_ERR_L1_PHY_BIST_TOG (0x1 << 9)
#define   NFP_MSFCL_PHY_BIST_TOG_BIST_ERR_L0_PHY_BIST_TOG (0x1 << 8)
#define   NFP_MSFCL_PHY_BIST_TOG_BIST_ERR_L3_PHY_BIST_ERR (0x1 << 3)
#define   NFP_MSFCL_PHY_BIST_TOG_BIST_ERR_L2_PHY_BIST_ERR (0x1 << 2)
#define   NFP_MSFCL_PHY_BIST_TOG_BIST_ERR_L1_PHY_BIST_ERR (0x1 << 1)
#define   NFP_MSFCL_PHY_BIST_TOG_BIST_ERR_L0_PHY_BIST_ERR (0x1)
/* Register Type: MsclPhy6gBistErrCnt */
#define NFP_MSFCL_PHY_BIST_ERR_CNT     0x0090
#define   NFP_MSFCL_PHY_BIST_ERR_CNT_L3_PHY_BIST_ERR_CNT(_x) (((_x) & 0xf) << 12)
#define   NFP_MSFCL_PHY_BIST_ERR_CNT_L3_PHY_BIST_ERR_CNT_of(_x) (((_x) >> 12) & 0xf)
#define   NFP_MSFCL_PHY_BIST_ERR_CNT_L2_PHY_BIST_ERR_CNT(_x) (((_x) & 0xf) << 8)
#define   NFP_MSFCL_PHY_BIST_ERR_CNT_L2_PHY_BIST_ERR_CNT_of(_x) (((_x) >> 8) & 0xf)
#define   NFP_MSFCL_PHY_BIST_ERR_CNT_L1_PHY_BIST_ERR_CNT(_x) (((_x) & 0xf) << 4)
#define   NFP_MSFCL_PHY_BIST_ERR_CNT_L1_PHY_BIST_ERR_CNT_of(_x) (((_x) >> 4) & 0xf)
#define   NFP_MSFCL_PHY_BIST_ERR_CNT_L0_PHY_BIST_ERR_CNT(_x) ((_x) & 0xf)
#define   NFP_MSFCL_PHY_BIST_ERR_CNT_L0_PHY_BIST_ERR_CNT_of(_x) ((_x) & 0xf)
/* Register Type: MsclPhy6gBistSyncPatSync */
#define NFP_MSFCL_PHY_BIST_SYNC_PAT_SYNC 0x0094
#define   NFP_MSFCL_PHY_BIST_SYNC_PAT_SYNC_L3_PHY_BIST_SYNC (0x1 << 11)
#define   NFP_MSFCL_PHY_BIST_SYNC_PAT_SYNC_L2_PHY_BIST_SYNC (0x1 << 10)
#define   NFP_MSFCL_PHY_BIST_SYNC_PAT_SYNC_L1_PHY_BIST_SYNC (0x1 << 9)
#define   NFP_MSFCL_PHY_BIST_SYNC_PAT_SYNC_L0_PHY_BIST_SYNC (0x1 << 8)
#define   NFP_MSFCL_PHY_BIST_SYNC_PAT_SYNC_L3_PHY_BIST_PAT_SYNC (0x1 << 3)
#define   NFP_MSFCL_PHY_BIST_SYNC_PAT_SYNC_L2_PHY_BIST_PAT_SYNC (0x1 << 2)
#define   NFP_MSFCL_PHY_BIST_SYNC_PAT_SYNC_L1_PHY_BIST_PAT_SYNC (0x1 << 1)
#define   NFP_MSFCL_PHY_BIST_SYNC_PAT_SYNC_L0_PHY_BIST_PAT_SYNC (0x1)
/* Register Type: MsclPhy6gLineLpbSerLpbEn */
#define NFP_MSFCL_PHY_LINE_LPB_SER_LPB_EN 0x0098
#define   NFP_MSFCL_PHY_LINE_LPB_SER_LPB_EN_L3_PHY_LINE_LPB_EN (0x1 << 11)
#define   NFP_MSFCL_PHY_LINE_LPB_SER_LPB_EN_L2_PHY_LINE_LPB_EN (0x1 << 10)
#define   NFP_MSFCL_PHY_LINE_LPB_SER_LPB_EN_L1_PHY_LINE_LPB_EN (0x1 << 9)
#define   NFP_MSFCL_PHY_LINE_LPB_SER_LPB_EN_L0_PHY_LINE_LPB_EN (0x1 << 8)
#define   NFP_MSFCL_PHY_LINE_LPB_SER_LPB_EN_L3_PHY_SER_LPB_EN (0x1 << 3)
#define   NFP_MSFCL_PHY_LINE_LPB_SER_LPB_EN_L2_PHY_SER_LPB_EN (0x1 << 2)
#define   NFP_MSFCL_PHY_LINE_LPB_SER_LPB_EN_L1_PHY_SER_LPB_EN (0x1 << 1)
#define   NFP_MSFCL_PHY_LINE_LPB_SER_LPB_EN_L0_PHY_SER_LPB_EN (0x1)
/* Register Type: MsclPhy6gRclkLpbParLpbEn */
#define NFP_MSFCL_PHY_RCLK_LPB_PAR_LPB_EN 0x009c
#define   NFP_MSFCL_PHY_RCLK_LPB_PAR_LPB_EN_L3_PHY_RCLK_LPB_EN (0x1 << 11)
#define   NFP_MSFCL_PHY_RCLK_LPB_PAR_LPB_EN_L2_PHY_RCLK_LPB_EN (0x1 << 10)
#define   NFP_MSFCL_PHY_RCLK_LPB_PAR_LPB_EN_L1_PHY_RCLK_LPB_EN (0x1 << 9)
#define   NFP_MSFCL_PHY_RCLK_LPB_PAR_LPB_EN_L0_PHY_RCLK_LPB_EN (0x1 << 8)
#define   NFP_MSFCL_PHY_RCLK_LPB_PAR_LPB_EN_L3_PHY_PAR_LPB_EN (0x1 << 3)
#define   NFP_MSFCL_PHY_RCLK_LPB_PAR_LPB_EN_L2_PHY_PAR_LPB_EN (0x1 << 2)
#define   NFP_MSFCL_PHY_RCLK_LPB_PAR_LPB_EN_L1_PHY_PAR_LPB_EN (0x1 << 1)
#define   NFP_MSFCL_PHY_RCLK_LPB_PAR_LPB_EN_L0_PHY_PAR_LPB_EN (0x1)
/* Register Type: MsclPhy6gTxRclkEn */
#define NFP_MSFCL_PHY_TX_RCLK_EN       0x00a0
#define   NFP_MSFCL_PHY_TX_RCLK_EN_L3_PHY_TX_RCLK_EN    (0x1 << 3)
#define   NFP_MSFCL_PHY_TX_RCLK_EN_L2_PHY_TX_RCLK_EN    (0x1 << 2)
#define   NFP_MSFCL_PHY_TX_RCLK_EN_L1_PHY_TX_RCLK_EN    (0x1 << 1)
#define   NFP_MSFCL_PHY_TX_RCLK_EN_L0_PHY_TX_RCLK_EN    (0x1)
/* Register Type: MsclPhy6gSjMag */
#define NFP_MSFCL_PHY_SJ_MAG           0x00a4
#define   NFP_MSFCL_PHY_SJ_MAG_L3_PHY_SJ_MAG(_x)        (((_x) & 0x7) << 12)
#define   NFP_MSFCL_PHY_SJ_MAG_L3_PHY_SJ_MAG_of(_x)     (((_x) >> 12) & 0x7)
#define   NFP_MSFCL_PHY_SJ_MAG_L2_PHY_SJ_MAG(_x)        (((_x) & 0x7) << 8)
#define   NFP_MSFCL_PHY_SJ_MAG_L2_PHY_SJ_MAG_of(_x)     (((_x) >> 8) & 0x7)
#define   NFP_MSFCL_PHY_SJ_MAG_L1_PHY_SJ_MAG(_x)        (((_x) & 0x7) << 4)
#define   NFP_MSFCL_PHY_SJ_MAG_L1_PHY_SJ_MAG_of(_x)     (((_x) >> 4) & 0x7)
#define   NFP_MSFCL_PHY_SJ_MAG_L0_PHY_SJ_MAG(_x)        ((_x) & 0x7)
#define   NFP_MSFCL_PHY_SJ_MAG_L0_PHY_SJ_MAG_of(_x)     ((_x) & 0x7)
/* Register Type: MsclPhy6gSjFreq */
#define NFP_MSFCL_PHY_SJ_FREQ          0x00a8
#define   NFP_MSFCL_PHY_SJ_FREQ_L3_PHY_SJ_FREQ(_x)      (((_x) & 0x7) << 12)
#define   NFP_MSFCL_PHY_SJ_FREQ_L3_PHY_SJ_FREQ_of(_x)   (((_x) >> 12) & 0x7)
#define   NFP_MSFCL_PHY_SJ_FREQ_L2_PHY_SJ_FREQ(_x)      (((_x) & 0x7) << 8)
#define   NFP_MSFCL_PHY_SJ_FREQ_L2_PHY_SJ_FREQ_of(_x)   (((_x) >> 8) & 0x7)
#define   NFP_MSFCL_PHY_SJ_FREQ_L1_PHY_SJ_FREQ(_x)      (((_x) & 0x7) << 4)
#define   NFP_MSFCL_PHY_SJ_FREQ_L1_PHY_SJ_FREQ_of(_x)   (((_x) >> 4) & 0x7)
#define   NFP_MSFCL_PHY_SJ_FREQ_L0_PHY_SJ_FREQ(_x)      ((_x) & 0x7)
#define   NFP_MSFCL_PHY_SJ_FREQ_L0_PHY_SJ_FREQ_of(_x)   ((_x) & 0x7)
/* Register Type: MsclPhy6gPpm */
#define NFP_MSFCL_PHY_PPM              0x00ac
#define   NFP_MSFCL_PHY_PPM_L3_PHY_PPM(_x)              (((_x) & 0xf) << 12)
#define   NFP_MSFCL_PHY_PPM_L3_PHY_PPM_of(_x)           (((_x) >> 12) & 0xf)
#define   NFP_MSFCL_PHY_PPM_L2_PHY_PPM(_x)              (((_x) & 0xf) << 8)
#define   NFP_MSFCL_PHY_PPM_L2_PHY_PPM_of(_x)           (((_x) >> 8) & 0xf)
#define   NFP_MSFCL_PHY_PPM_L1_PHY_PPM(_x)              (((_x) & 0xf) << 4)
#define   NFP_MSFCL_PHY_PPM_L1_PHY_PPM_of(_x)           (((_x) >> 4) & 0xf)
#define   NFP_MSFCL_PHY_PPM_L0_PHY_PPM(_x)              ((_x) & 0xf)
#define   NFP_MSFCL_PHY_PPM_L0_PHY_PPM_of(_x)           ((_x) & 0xf)
/* Register Type: MsclIoPwrdn */
#define NFP_MSFCL_CLKREF_PAD           0x00b0
#define   NFP_MSFCL_CLKREF_PAD_FCIO_PWRDN               (0x1 << 1)
#define   NFP_MSFCL_CLKREF_PAD_SPI4_PWRDN               (0x1)
/* Register Type: MsclIlkPad */
#define NFP_MSFCL_ILK_PAD              0x00b4
#define   NFP_MSFCL_ILK_PAD_LVCMOS_DRIVE(_x)            (((_x) & 0x3) << 2)
#define   NFP_MSFCL_ILK_PAD_LVCMOS_DRIVE_of(_x)         (((_x) >> 2) & 0x3)
#define     NFP_MSFCL_ILK_PAD_LVCMOS_DRIVE_DRIVE_10MA   (0)
#define     NFP_MSFCL_ILK_PAD_LVCMOS_DRIVE_DRIVE_8MA    (1)
#define     NFP_MSFCL_ILK_PAD_LVCMOS_DRIVE_DRIVE_4MA    (2)
#define     NFP_MSFCL_ILK_PAD_LVCMOS_DRIVE_DRIVE_2MA    (3)
#define   NFP_MSFCL_ILK_PAD_OE_TFC_LVCMOS               (0x1 << 1)
#define   NFP_MSFCL_ILK_PAD_OE_TFC_LVDS                 (0x1)
/* Register Type: MsclCtlMemRwm */
#define NFP_MSFCL_MSCL_CTL_MEM_RWM     0x00b8
#define   NFP_MSFCL_MSCL_CTL_MEM_RWM_DEV9(_x)           (((_x) & 0x7) << 27)
#define   NFP_MSFCL_MSCL_CTL_MEM_RWM_DEV9_of(_x)        (((_x) >> 27) & 0x7)
#define   NFP_MSFCL_MSCL_CTL_MEM_RWM_DEV8(_x)           (((_x) & 0x7) << 24)
#define   NFP_MSFCL_MSCL_CTL_MEM_RWM_DEV8_of(_x)        (((_x) >> 24) & 0x7)
#define   NFP_MSFCL_MSCL_CTL_MEM_RWM_DEV7(_x)           (((_x) & 0x7) << 21)
#define   NFP_MSFCL_MSCL_CTL_MEM_RWM_DEV7_of(_x)        (((_x) >> 21) & 0x7)
#define   NFP_MSFCL_MSCL_CTL_MEM_RWM_DEV6(_x)           (((_x) & 0x7) << 18)
#define   NFP_MSFCL_MSCL_CTL_MEM_RWM_DEV6_of(_x)        (((_x) >> 18) & 0x7)
#define   NFP_MSFCL_MSCL_CTL_MEM_RWM_DEV5(_x)           (((_x) & 0x7) << 15)
#define   NFP_MSFCL_MSCL_CTL_MEM_RWM_DEV5_of(_x)        (((_x) >> 15) & 0x7)
#define   NFP_MSFCL_MSCL_CTL_MEM_RWM_DEV4(_x)           (((_x) & 0x7) << 12)
#define   NFP_MSFCL_MSCL_CTL_MEM_RWM_DEV4_of(_x)        (((_x) >> 12) & 0x7)
#define   NFP_MSFCL_MSCL_CTL_MEM_RWM_DEV3(_x)           (((_x) & 0x7) << 9)
#define   NFP_MSFCL_MSCL_CTL_MEM_RWM_DEV3_of(_x)        (((_x) >> 9) & 0x7)
#define   NFP_MSFCL_MSCL_CTL_MEM_RWM_DEV2(_x)           (((_x) & 0x7) << 6)
#define   NFP_MSFCL_MSCL_CTL_MEM_RWM_DEV2_of(_x)        (((_x) >> 6) & 0x7)
#define   NFP_MSFCL_MSCL_CTL_MEM_RWM_DEV1(_x)           (((_x) & 0x7) << 3)
#define   NFP_MSFCL_MSCL_CTL_MEM_RWM_DEV1_of(_x)        (((_x) >> 3) & 0x7)
#define   NFP_MSFCL_MSCL_CTL_MEM_RWM_DEV0(_x)           ((_x) & 0x7)
#define   NFP_MSFCL_MSCL_CTL_MEM_RWM_DEV0_of(_x)        ((_x) & 0x7)
/* Register Type: MsclCtlCsrBistrun */
#define NFP_MSFCL_MSCL_CTL_CSR_BISTRUN 0x00bc
#define   NFP_MSFCL_MSCL_CTL_CSR_BISTRUN_BISTRUN(_x)    ((_x) & 0x7)
#define   NFP_MSFCL_MSCL_CTL_CSR_BISTRUN_BISTRUN_of(_x) ((_x) & 0x7)
/* Register Type: MsclCtlCsrBiststat */
#define NFP_MSFCL_MSCL_CTL_CSR_BISTSTATUS 0x00c0
#define   NFP_MSFCL_MSCL_CTL_CSR_BISTSTATUS_BISTFAIL_of(_x) (((_x) >> 6) & 0x3f)
#define   NFP_MSFCL_MSCL_CTL_CSR_BISTSTATUS_BISTDONE_of(_x) ((_x) & 0x3f)
/* Register Type: MsclCtlCsrDllTstclk */
#define NFP_MSFCL_MSCL_CTL_CSR_DLL_TSTCLK 0x00c4
#define   NFP_MSFCL_MSCL_CTL_CSR_DLL_TSTCLK_LVDS_TSTCLK_EN (0x1 << 10)
#define   NFP_MSFCL_MSCL_CTL_CSR_DLL_TSTCLK_SPI_DLL_TSTCLK_SEL(_x) (((_x) & 0x3) << 8)
#define   NFP_MSFCL_MSCL_CTL_CSR_DLL_TSTCLK_SPI_DLL_TSTCLK_SEL_of(_x) (((_x) >> 8) & 0x3)
#define   NFP_MSFCL_MSCL_CTL_CSR_DLL_TSTCLK_FC_DLL_TSTC(_x) (((_x) & 0xf) << 4)
#define   NFP_MSFCL_MSCL_CTL_CSR_DLL_TSTCLK_FC_DLL_TSTC_of(_x) (((_x) >> 4) & 0xf)
#define   NFP_MSFCL_MSCL_CTL_CSR_DLL_TSTCLK_SPI_DLL_TSTC(_x) ((_x) & 0xf)
#define   NFP_MSFCL_MSCL_CTL_CSR_DLL_TSTCLK_SPI_DLL_TSTC_of(_x) ((_x) & 0xf)
/* Register Type: MsclLkCtlTx */
#define NFP_MSFCL_LK_CTL_TX            0x0100
#define   NFP_MSFCL_LK_CTL_TX_RX_FC_STAT_SEL(_x)        (((_x) & 0x7) << 22)
#define   NFP_MSFCL_LK_CTL_TX_RX_FC_STAT_SEL_of(_x)     (((_x) >> 22) & 0x7)
#define   NFP_MSFCL_LK_CTL_TX_RX_PACKET_MODE            (0x1 << 21)
#define   NFP_MSFCL_LK_CTL_TX_TX_DISABLE_SKIPWORD       (0x1 << 20)
#define   NFP_MSFCL_LK_CTL_TX_TX_RDYOUT_THRESH(_x)      (((_x) & 0xf) << 16)
#define   NFP_MSFCL_LK_CTL_TX_TX_RDYOUT_THRESH_of(_x)   (((_x) >> 16) & 0xf)
#define   NFP_MSFCL_LK_CTL_TX_TXOOB_CALLEN(_x)          (((_x) & 0xff) << 8)
#define   NFP_MSFCL_LK_CTL_TX_TXOOB_CALLEN_of(_x)       (((_x) >> 8) & 0xff)
#define   NFP_MSFCL_LK_CTL_TX_TX_FC_CALLEN(_x)          (((_x) & 0xf) << 4)
#define   NFP_MSFCL_LK_CTL_TX_TX_FC_CALLEN_of(_x)       (((_x) >> 4) & 0xf)
#define     NFP_MSFCL_LK_CTL_TX_TX_FC_CALLEN_CAL_16     (0)
#define     NFP_MSFCL_LK_CTL_TX_TX_FC_CALLEN_CAL_32     (1)
#define     NFP_MSFCL_LK_CTL_TX_TX_FC_CALLEN_CAL_64     (3)
#define     NFP_MSFCL_LK_CTL_TX_TX_FC_CALLEN_CAL_128    (7)
#define     NFP_MSFCL_LK_CTL_TX_TX_FC_CALLEN_CAL_256    (15)
#define   NFP_MSFCL_LK_CTL_TX_TX_RLIM_EN                (0x1 << 1)
#define   NFP_MSFCL_LK_CTL_TX_TX_EN                     (0x1)
/* Register Type: MsclLkCtlTxProtocol */
#define NFP_MSFCL_LK_CTL_TX_PROTOCOL   0x0104
#define   NFP_MSFCL_LK_CTL_TX_PROTOCOL_TX_BURSTSHORT(_x) (((_x) & 0x7) << 18)
#define   NFP_MSFCL_LK_CTL_TX_PROTOCOL_TX_BURSTSHORT_of(_x) (((_x) >> 18) & 0x7)
#define     NFP_MSFCL_LK_CTL_TX_PROTOCOL_TX_BURSTSHORT_BURSTSHORT_32 (0)
#define     NFP_MSFCL_LK_CTL_TX_PROTOCOL_TX_BURSTSHORT_BURSTSHORT_64 (1)
#define     NFP_MSFCL_LK_CTL_TX_PROTOCOL_TX_BURSTSHORT_BURSTSHORT_96 (2)
#define     NFP_MSFCL_LK_CTL_TX_PROTOCOL_TX_BURSTSHORT_BURSTSHORT_128 (3)
#define     NFP_MSFCL_LK_CTL_TX_PROTOCOL_TX_BURSTSHORT_BURSTSHORT_160 (4)
#define     NFP_MSFCL_LK_CTL_TX_PROTOCOL_TX_BURSTSHORT_BURSTSHORT_192 (5)
#define     NFP_MSFCL_LK_CTL_TX_PROTOCOL_TX_BURSTSHORT_BURSTSHORT_224 (6)
#define     NFP_MSFCL_LK_CTL_TX_PROTOCOL_TX_BURSTSHORT_BURSTSHORT_256 (7)
#define   NFP_MSFCL_LK_CTL_TX_PROTOCOL_TX_BURSTMAX(_x)  (((_x) & 0x3) << 16)
#define   NFP_MSFCL_LK_CTL_TX_PROTOCOL_TX_BURSTMAX_of(_x) (((_x) >> 16) & 0x3)
#define     NFP_MSFCL_LK_CTL_TX_PROTOCOL_TX_BURSTMAX_BURSTMAX_64 (0)
#define     NFP_MSFCL_LK_CTL_TX_PROTOCOL_TX_BURSTMAX_BURSTMAX_128 (1)
#define     NFP_MSFCL_LK_CTL_TX_PROTOCOL_TX_BURSTMAX_BURSTMAX_192 (2)
#define     NFP_MSFCL_LK_CTL_TX_PROTOCOL_TX_BURSTMAX_BURSTMAX_256 (3)
#define   NFP_MSFCL_LK_CTL_TX_PROTOCOL_TX_MFLEN_MINUS1(_x) ((_x) & 0xffff)
#define   NFP_MSFCL_LK_CTL_TX_PROTOCOL_TX_MFLEN_MINUS1_of(_x) ((_x) & 0xffff)
/* Register Type: MsclLkCtlTxRlim */
#define NFP_MSFCL_LK_CTL_TX_RLIM       0x0108
#define   NFP_MSFCL_LK_CTL_TX_RLIM_TX_RLIM_INTV(_x)     (((_x) & 0xff) << 24)
#define   NFP_MSFCL_LK_CTL_TX_RLIM_TX_RLIM_INTV_of(_x)  (((_x) >> 24) & 0xff)
#define   NFP_MSFCL_LK_CTL_TX_RLIM_TX_RLIM_DELTA(_x)    (((_x) & 0xfff) << 12)
#define   NFP_MSFCL_LK_CTL_TX_RLIM_TX_RLIM_DELTA_of(_x) (((_x) >> 12) & 0xfff)
#define   NFP_MSFCL_LK_CTL_TX_RLIM_TX_RLIM_MAX(_x)      ((_x) & 0xfff)
#define   NFP_MSFCL_LK_CTL_TX_RLIM_TX_RLIM_MAX_of(_x)   ((_x) & 0xfff)
/* Register Type: MsclLkCtlTxMubits */
#define NFP_MSFCL_LK_CTL_TX_MUBITS     0x010c
#define   NFP_MSFCL_LK_CTL_TX_MUBITS_TX_MUBITS(_x)      ((_x) & 0xff)
#define   NFP_MSFCL_LK_CTL_TX_MUBITS_TX_MUBITS_of(_x)   ((_x) & 0xff)
/* Register Type: MsclLkCtlTxLaneCtl */
#define NFP_MSFCL_LK_CTL_TX_LANE_CTL   0x0110
#define   NFP_MSFCL_LK_CTL_TX_LANE_CTL_TX_LAST_LANE(_x) (((_x) & 0x3) << 3)
#define   NFP_MSFCL_LK_CTL_TX_LANE_CTL_TX_LAST_LANE_of(_x) (((_x) >> 3) & 0x3)
#define   NFP_MSFCL_LK_CTL_TX_LANE_CTL_TX_HAS_BAD_LANE  (0x1 << 2)
#define   NFP_MSFCL_LK_CTL_TX_LANE_CTL_TX_BAD_LANE(_x)  ((_x) & 0x3)
#define   NFP_MSFCL_LK_CTL_TX_LANE_CTL_TX_BAD_LANE_of(_x) ((_x) & 0x3)
/* Register Type: MsclLkCtlRxForceResync */
#define NFP_MSFCL_LK_CTL_RX_FORCE_RESYNC 0x0114
#define   NFP_MSFCL_LK_CTL_RX_FORCE_RESYNC_RX_FORCE_RESYNC (0x1)
/* Register Type: MsclLkCtlRxLaneCtl */
#define NFP_MSFCL_LK_CTL_RX_LANE_CTL   0x0118
#define   NFP_MSFCL_LK_CTL_RX_LANE_CTL_RX_LAST_LANE(_x) (((_x) & 0x3) << 3)
#define   NFP_MSFCL_LK_CTL_RX_LANE_CTL_RX_LAST_LANE_of(_x) (((_x) >> 3) & 0x3)
#define   NFP_MSFCL_LK_CTL_RX_LANE_CTL_RX_HAS_BAD_LANE  (0x1 << 2)
#define   NFP_MSFCL_LK_CTL_RX_LANE_CTL_RX_BAD_LANE(_x)  ((_x) & 0x3)
#define   NFP_MSFCL_LK_CTL_RX_LANE_CTL_RX_BAD_LANE_of(_x) ((_x) & 0x3)
/* Register Type: MsclLkCtlRxProtocol */
#define NFP_MSFCL_LK_CTL_RX_PROTOCOL   0x011c
#define   NFP_MSFCL_LK_CTL_RX_PROTOCOL_RX_BURSTMAX(_x)  (((_x) & 0x3) << 16)
#define   NFP_MSFCL_LK_CTL_RX_PROTOCOL_RX_BURSTMAX_of(_x) (((_x) >> 16) & 0x3)
#define     NFP_MSFCL_LK_CTL_RX_PROTOCOL_RX_BURSTMAX_BURSTMAX_64 (0)
#define     NFP_MSFCL_LK_CTL_RX_PROTOCOL_RX_BURSTMAX_BURSTMAX_128 (1)
#define     NFP_MSFCL_LK_CTL_RX_PROTOCOL_RX_BURSTMAX_BURSTMAX_192 (2)
#define     NFP_MSFCL_LK_CTL_RX_PROTOCOL_RX_BURSTMAX_BURSTMAX_256 (3)
#define   NFP_MSFCL_LK_CTL_RX_PROTOCOL_RX_MFLEN_MINUS1(_x) ((_x) & 0xffff)
#define   NFP_MSFCL_LK_CTL_RX_PROTOCOL_RX_MFLEN_MINUS1_of(_x) ((_x) & 0xffff)
/* Register Type: MsclLkCtlTstatCal */
#define NFP_MSFCL_LK_CTL_TSTAT_CAL     0x0120
#define   NFP_MSFCL_LK_CTL_TSTAT_CAL_TSTAT_CAL_EN       (0x1 << 16)
#define   NFP_MSFCL_LK_CTL_TSTAT_CAL_TSTAT_CAL_M(_x)    (((_x) & 0xff) << 8)
#define   NFP_MSFCL_LK_CTL_TSTAT_CAL_TSTAT_CAL_M_of(_x) (((_x) >> 8) & 0xff)
#define   NFP_MSFCL_LK_CTL_TSTAT_CAL_TSTAT_CAL_LEN(_x)  ((_x) & 0xff)
#define   NFP_MSFCL_LK_CTL_TSTAT_CAL_TSTAT_CAL_LEN_of(_x) ((_x) & 0xff)
/* Register Type: MsclLkCtlRstatCal */
#define NFP_MSFCL_LK_CTL_RSTAT_CAL     0x0124
#define   NFP_MSFCL_LK_CTL_RSTAT_CAL_RSTAT_CAL_EN       (0x1 << 16)
#define   NFP_MSFCL_LK_CTL_RSTAT_CAL_RSTAT_CAL_M(_x)    (((_x) & 0xff) << 8)
#define   NFP_MSFCL_LK_CTL_RSTAT_CAL_RSTAT_CAL_M_of(_x) (((_x) >> 8) & 0xff)
#define   NFP_MSFCL_LK_CTL_RSTAT_CAL_RSTAT_CAL_LEN(_x)  ((_x) & 0xff)
#define   NFP_MSFCL_LK_CTL_RSTAT_CAL_RSTAT_CAL_LEN_of(_x) ((_x) & 0xff)
/* Register Type: MsclLkCtlRxGasket */
#define NFP_MSFCL_LK_CTL_RX_GASKET     0x0128
#define   NFP_MSFCL_LK_CTL_RX_GASKET_RX_LINK_FC_EN      (0x1 << 12)
#define   NFP_MSFCL_LK_CTL_RX_GASKET_RBUFSIZE_X32(_x)   (((_x) & 0xf) << 8)
#define   NFP_MSFCL_LK_CTL_RX_GASKET_RBUFSIZE_X32_of(_x) (((_x) >> 8) & 0xf)
#define     NFP_MSFCL_LK_CTL_RX_GASKET_RBUFSIZE_X32_RBUF_SIZE_64 (2)
#define     NFP_MSFCL_LK_CTL_RX_GASKET_RBUFSIZE_X32_RBUF_SIZE_128 (4)
#define     NFP_MSFCL_LK_CTL_RX_GASKET_RBUFSIZE_X32_RBUF_SIZE_256 (8)
#define   NFP_MSFCL_LK_CTL_RX_GASKET_RXFIFO_THRESHOLD(_x) ((_x) & 0xff)
#define   NFP_MSFCL_LK_CTL_RX_GASKET_RXFIFO_THRESHOLD_of(_x) ((_x) & 0xff)
/* Register Type: MsclLkCtlTxGasket */
#define NFP_MSFCL_LK_CTL_TX_GASKET     0x012c
#define   NFP_MSFCL_LK_CTL_TX_GASKET_TX_LINK_FC_CHAN_EN (0x1 << 20)
#define   NFP_MSFCL_LK_CTL_TX_GASKET_BURSTSIZE_X32(_x)  (((_x) & 0xf) << 16)
#define   NFP_MSFCL_LK_CTL_TX_GASKET_BURSTSIZE_X32_of(_x) (((_x) >> 16) & 0xf)
#define   NFP_MSFCL_LK_CTL_TX_GASKET_TXFIFO_LINK_THRESHOLD(_x) (((_x) & 0xff) << 8)
#define   NFP_MSFCL_LK_CTL_TX_GASKET_TXFIFO_LINK_THRESHOLD_of(_x) (((_x) >> 8) & 0xff)
#define   NFP_MSFCL_LK_CTL_TX_GASKET_TXFIFO_STAT_THRESHOLD(_x) ((_x) & 0xff)
#define   NFP_MSFCL_LK_CTL_TX_GASKET_TXFIFO_STAT_THRESHOLD_of(_x) ((_x) & 0xff)
/* Register Type: MsclLkCtlLinkFcChanEn031000 */
#define NFP_MSFCL_LK_CTL_LINK_FC_CHAN_EN_031000 0x0130
#define   NFP_MSFCL_LK_CTL_LINK_FC_CHAN_EN_031000_LINK_FC_CHAN_EN_031000(_x) (_x)
#define   NFP_MSFCL_LK_CTL_LINK_FC_CHAN_EN_031000_LINK_FC_CHAN_EN_031000_of(_x) (_x)
/* Register Type: MsclLkCtlLinkFcChanEn063032 */
#define NFP_MSFCL_LK_CTL_LINK_FC_CHAN_EN_063032 0x0134
#define   NFP_MSFCL_LK_CTL_LINK_FC_CHAN_EN_063032_LINK_FC_CHAN_EN_063032(_x) (_x)
#define   NFP_MSFCL_LK_CTL_LINK_FC_CHAN_EN_063032_LINK_FC_CHAN_EN_063032_of(_x) (_x)
/* Register Type: MsclLkCtlLinkFcChanEn095064 */
#define NFP_MSFCL_LK_CTL_LINK_FC_CHAN_EN_095064 0x0138
#define   NFP_MSFCL_LK_CTL_LINK_FC_CHAN_EN_095064_LINK_FC_CHAN_EN_095064(_x) (_x)
#define   NFP_MSFCL_LK_CTL_LINK_FC_CHAN_EN_095064_LINK_FC_CHAN_EN_095064_of(_x) (_x)
/* Register Type: MsclLkCtlLinkFcChanEn127096 */
#define NFP_MSFCL_LK_CTL_LINK_FC_CHAN_EN_127096 0x013c
#define   NFP_MSFCL_LK_CTL_LINK_FC_CHAN_EN_127096_LINK_FC_CHAN_EN_127096(_x) (_x)
#define   NFP_MSFCL_LK_CTL_LINK_FC_CHAN_EN_127096_LINK_FC_CHAN_EN_127096_of(_x) (_x)
/* Register Type: MsclLkCtlLinkFcChanEn159128 */
#define NFP_MSFCL_LK_CTL_LINK_FC_CHAN_EN_159128 0x0140
#define   NFP_MSFCL_LK_CTL_LINK_FC_CHAN_EN_159128_LINK_FC_CHAN_EN_159128(_x) (_x)
#define   NFP_MSFCL_LK_CTL_LINK_FC_CHAN_EN_159128_LINK_FC_CHAN_EN_159128_of(_x) (_x)
/* Register Type: MsclLkCtlLinkFcChanEn191160 */
#define NFP_MSFCL_LK_CTL_LINK_FC_CHAN_EN_191160 0x0144
#define   NFP_MSFCL_LK_CTL_LINK_FC_CHAN_EN_191160_LINK_FC_CHAN_EN_191160(_x) (_x)
#define   NFP_MSFCL_LK_CTL_LINK_FC_CHAN_EN_191160_LINK_FC_CHAN_EN_191160_of(_x) (_x)
/* Register Type: MsclLkCtlLinkFcChanEn223192 */
#define NFP_MSFCL_LK_CTL_LINK_FC_CHAN_EN_223192 0x0148
#define   NFP_MSFCL_LK_CTL_LINK_FC_CHAN_EN_223192_LINK_FC_CHAN_EN_223192(_x) (_x)
#define   NFP_MSFCL_LK_CTL_LINK_FC_CHAN_EN_223192_LINK_FC_CHAN_EN_223192_of(_x) (_x)
/* Register Type: MsclLkCtlLinkFcChanEn255224 */
#define NFP_MSFCL_LK_CTL_LINK_FC_CHAN_EN_255224 0x014c
#define   NFP_MSFCL_LK_CTL_LINK_FC_CHAN_EN_255224_LINK_FC_CHAN_EN_255224(_x) (_x)
#define   NFP_MSFCL_LK_CTL_LINK_FC_CHAN_EN_255224_LINK_FC_CHAN_EN_255224_of(_x) (_x)
/* Register Type: MsclLkStatRxErr0 */
#define NFP_MSFCL_LK_STAT_RX_ERR0      0x0150
#define   NFP_MSFCL_LK_STAT_RX_ERR0_RX_BAD_TYPE_ERR_of(_x) (((_x) >> 28) & 0xf)
#define   NFP_MSFCL_LK_STAT_RX_ERR0_RX_FRAMING_ERR_of(_x) (((_x) >> 24) & 0xf)
#define   NFP_MSFCL_LK_STAT_RX_ERR0_RX_MF_ERR_of(_x)    (((_x) >> 20) & 0xf)
#define   NFP_MSFCL_LK_STAT_RX_ERR0_RX_DESCRAM_ERR_of(_x) (((_x) >> 16) & 0xf)
#define   NFP_MSFCL_LK_STAT_RX_ERR0_RX_MF_REPEAT_ERR_of(_x) (((_x) >> 12) & 0xf)
#define   NFP_MSFCL_LK_STAT_RX_ERR0_RX_MF_LEN_ERR_of(_x) (((_x) >> 8) & 0xf)
#define   NFP_MSFCL_LK_STAT_RX_ERR0_RX_CRC32_ERR_of(_x) (((_x) >> 4) & 0xf)
#define   NFP_MSFCL_LK_STAT_RX_ERR0_RX_SYNCED_ERR_of(_x) ((_x) & 0xf)
/* Register Type: MsclLkStatRxErr1 */
#define NFP_MSFCL_LK_STAT_RX_ERR1      0x0154
#define   NFP_MSFCL_LK_STAT_RX_ERR1_OB_INTFSTAT_CHANGE  (0x1 << 20)
#define   NFP_MSFCL_LK_STAT_RX_ERR1_OB_LANESTAT_CHANGE  (0x1 << 19)
#define   NFP_MSFCL_LK_STAT_RX_ERR1_IB_INTFSTAT_CHANGE  (0x1 << 18)
#define   NFP_MSFCL_LK_STAT_RX_ERR1_IB_LANESTAT_CHANGE  (0x1 << 17)
#define   NFP_MSFCL_LK_STAT_RX_ERR1_RSTAT_OVF_ERR       (0x1 << 16)
#define   NFP_MSFCL_LK_STAT_RX_ERR1_TX_BURST_ERR        (0x1 << 15)
#define   NFP_MSFCL_LK_STAT_RX_ERR1_RX_AFF_OVF_ERR      (0x1 << 14)
#define   NFP_MSFCL_LK_STAT_RX_ERR1_TX_AFF_OVF_ERR      (0x1 << 13)
#define   NFP_MSFCL_LK_STAT_RX_ERR1_RX_OOB_OVF_ERR      (0x1 << 12)
#define   NFP_MSFCL_LK_STAT_RX_ERR1_RX_OOB_CRC_ERR      (0x1 << 11)
#define   NFP_MSFCL_LK_STAT_RX_ERR1_TX_OVFOUT_ERR       (0x1 << 10)
#define   NFP_MSFCL_LK_STAT_RX_ERR1_TX_UNDERFLOW_ERR    (0x1 << 9)
#define   NFP_MSFCL_LK_STAT_RX_ERR1_TX_OVF_ERR          (0x1 << 8)
#define   NFP_MSFCL_LK_STAT_RX_ERR1_RX_OVF_ERR          (0x1 << 7)
#define   NFP_MSFCL_LK_STAT_RX_ERR1_RX_MEOP_ERR         (0x1 << 6)
#define   NFP_MSFCL_LK_STAT_RX_ERR1_RX_MSOP_ERR         (0x1 << 5)
#define   NFP_MSFCL_LK_STAT_RX_ERR1_RX_CRC24_ERR        (0x1 << 4)
#define   NFP_MSFCL_LK_STAT_RX_ERR1_RX_MISALIGNED_ERR   (0x1 << 3)
#define   NFP_MSFCL_LK_STAT_RX_ERR1_RX_ALIGNED_ERR      (0x1 << 2)
#define   NFP_MSFCL_LK_STAT_RX_ERR1_RX_BURST_ERR        (0x1 << 1)
#define   NFP_MSFCL_LK_STAT_RX_ERR1_RX_BURSTMAX_ERR     (0x1)
/* Register Type: MsclLkStatRxLive */
#define NFP_MSFCL_LK_STAT_RX_LIVE      0x0158
#define   NFP_MSFCL_LK_STAT_RX_LIVE_INT_OUT             (0x1 << 23)
#define   NFP_MSFCL_LK_STAT_RX_LIVE_RSTAT_FRAME_OK      (0x1 << 22)
#define   NFP_MSFCL_LK_STAT_RX_LIVE_RX_OOB_INTFSTAT     (0x1 << 21)
#define   NFP_MSFCL_LK_STAT_RX_LIVE_RX_OOB_LANESTAT_of(_x) (((_x) >> 17) & 0xf)
#define   NFP_MSFCL_LK_STAT_RX_LIVE_RX_DIAGWORD_INTFSTAT_of(_x) (((_x) >> 13) & 0xf)
#define   NFP_MSFCL_LK_STAT_RX_LIVE_RX_DIAGWORD_LANESTAT_of(_x) (((_x) >> 9) & 0xf)
#define   NFP_MSFCL_LK_STAT_RX_LIVE_RX_CRC32_VLD_of(_x) (((_x) >> 5) & 0xf)
#define   NFP_MSFCL_LK_STAT_RX_LIVE_RX_ALIGNED          (0x1 << 4)
#define   NFP_MSFCL_LK_STAT_RX_LIVE_RX_SYNCED_of(_x)    ((_x) & 0xf)
/* Register Type: MsclLkStatRxMubits */
#define NFP_MSFCL_LK_STAT_RX_MUBITS    0x015c
#define   NFP_MSFCL_LK_STAT_RX_MUBITS_RX_MUBITS_of(_x)  ((_x) & 0xff)
/* Register Type: MsclLkStatRxFcStat */
#define NFP_MSFCL_LK_STAT_RX_FC_STAT   0x0160
#define   NFP_MSFCL_LK_STAT_RX_FC_STAT_RX_FC_STAT_of(_x) (_x)
/* Register Type: MsclLkStatTxPktCnt */
#define NFP_MSFCL_LK_STAT_TX_PKT_CNT   0x0164
#define   NFP_MSFCL_LK_STAT_TX_PKT_CNT_TX_PKT_CNT_of(_x) (_x)
/* Register Type: MsclLkStatRxPktCnt */
#define NFP_MSFCL_LK_STAT_RX_PKT_CNT   0x0168
#define   NFP_MSFCL_LK_STAT_RX_PKT_CNT_RX_PKT_CNT_of(_x) (_x)
/* Register Type: MsclLkStatTxErrCnt */
#define NFP_MSFCL_LK_STAT_TX_ERR_CNT   0x016c
#define   NFP_MSFCL_LK_STAT_TX_ERR_CNT_TX_ERR_CNT_of(_x) (_x)
/* Register Type: MsclLkStatRxErrCnt */
#define NFP_MSFCL_LK_STAT_RX_ERR_CNT   0x0170
#define   NFP_MSFCL_LK_STAT_RX_ERR_CNT_RX_ERR_CNT_of(_x) (_x)
/* Register Type: MsclLkStatTxByteCnt */
#define NFP_MSFCL_LK_STAT_TX_BYTE_CNT  0x0174
#define   NFP_MSFCL_LK_STAT_TX_BYTE_CNT_TX_BYTE_CNT_of(_x) (_x)
/* Register Type: MsclLkStatRxByteCnt */
#define NFP_MSFCL_LK_STAT_RX_BYTE_CNT  0x0178
#define   NFP_MSFCL_LK_STAT_RX_BYTE_CNT_RX_BYTE_CNT_of(_x) (_x)
/* Register Type: MsclLkStatErr0IntEn */
#define NFP_MSFCL_LK_STAT_ERR0_INT_EN  0x017c
#define   NFP_MSFCL_LK_STAT_ERR0_INT_EN_ERR0_INT_EN_of(_x) (_x)
/* Register Type: MsclLkStatErr1IntEn */
#define NFP_MSFCL_LK_STAT_ERR1_INT_EN  0x0180
#define   NFP_MSFCL_LK_STAT_ERR1_INT_EN_ERR1_INT_EN_of(_x) ((_x) & 0x3fffff)
/* Register Type: MsclXuCtlRamHWM */
#define NFP_MSFCL_XU_CTL_RAM_HWM       0x0200
#define   NFP_MSFCL_XU_CTL_RAM_HWM_EG_RAM_HWM_3(_x)     (((_x) & 0x3) << 14)
#define   NFP_MSFCL_XU_CTL_RAM_HWM_EG_RAM_HWM_3_of(_x)  (((_x) >> 14) & 0x3)
#define     NFP_MSFCL_XU_CTL_RAM_HWM_EG_RAM_HWM_3_HWM_3_QTR (3)
#define     NFP_MSFCL_XU_CTL_RAM_HWM_EG_RAM_HWM_3_HWM_1_HALF (2)
#define     NFP_MSFCL_XU_CTL_RAM_HWM_EG_RAM_HWM_3_HWM_1_QTR (1)
#define   NFP_MSFCL_XU_CTL_RAM_HWM_EG_RAM_HWM_2(_x)     (((_x) & 0x3) << 12)
#define   NFP_MSFCL_XU_CTL_RAM_HWM_EG_RAM_HWM_2_of(_x)  (((_x) >> 12) & 0x3)
#define     NFP_MSFCL_XU_CTL_RAM_HWM_EG_RAM_HWM_2_HWM_3_QTR (3)
#define     NFP_MSFCL_XU_CTL_RAM_HWM_EG_RAM_HWM_2_HWM_1_HALF (2)
#define     NFP_MSFCL_XU_CTL_RAM_HWM_EG_RAM_HWM_2_HWM_1_QTR (1)
#define   NFP_MSFCL_XU_CTL_RAM_HWM_EG_RAM_HWM_1(_x)     (((_x) & 0x3) << 10)
#define   NFP_MSFCL_XU_CTL_RAM_HWM_EG_RAM_HWM_1_of(_x)  (((_x) >> 10) & 0x3)
#define     NFP_MSFCL_XU_CTL_RAM_HWM_EG_RAM_HWM_1_HWM_3_QTR (3)
#define     NFP_MSFCL_XU_CTL_RAM_HWM_EG_RAM_HWM_1_HWM_1_HALF (2)
#define     NFP_MSFCL_XU_CTL_RAM_HWM_EG_RAM_HWM_1_HWM_1_QTR (1)
#define   NFP_MSFCL_XU_CTL_RAM_HWM_EG_RAM_HWM_0(_x)     (((_x) & 0x3) << 8)
#define   NFP_MSFCL_XU_CTL_RAM_HWM_EG_RAM_HWM_0_of(_x)  (((_x) >> 8) & 0x3)
#define     NFP_MSFCL_XU_CTL_RAM_HWM_EG_RAM_HWM_0_HWM_3_QTR (3)
#define     NFP_MSFCL_XU_CTL_RAM_HWM_EG_RAM_HWM_0_HWM_1_HALF (2)
#define     NFP_MSFCL_XU_CTL_RAM_HWM_EG_RAM_HWM_0_HWM_1_QTR (1)
#define   NFP_MSFCL_XU_CTL_RAM_HWM_IG_RAM_HWM_3(_x)     (((_x) & 0x3) << 6)
#define   NFP_MSFCL_XU_CTL_RAM_HWM_IG_RAM_HWM_3_of(_x)  (((_x) >> 6) & 0x3)
#define     NFP_MSFCL_XU_CTL_RAM_HWM_IG_RAM_HWM_3_HWM_3_QTR (3)
#define     NFP_MSFCL_XU_CTL_RAM_HWM_IG_RAM_HWM_3_HWM_1_HALF (2)
#define     NFP_MSFCL_XU_CTL_RAM_HWM_IG_RAM_HWM_3_HWM_1_QTR (1)
#define   NFP_MSFCL_XU_CTL_RAM_HWM_IG_RAM_HWM_2(_x)     (((_x) & 0x3) << 4)
#define   NFP_MSFCL_XU_CTL_RAM_HWM_IG_RAM_HWM_2_of(_x)  (((_x) >> 4) & 0x3)
#define     NFP_MSFCL_XU_CTL_RAM_HWM_IG_RAM_HWM_2_HWM_3_QTR (3)
#define     NFP_MSFCL_XU_CTL_RAM_HWM_IG_RAM_HWM_2_HWM_1_HALF (2)
#define     NFP_MSFCL_XU_CTL_RAM_HWM_IG_RAM_HWM_2_HWM_1_QTR (1)
#define   NFP_MSFCL_XU_CTL_RAM_HWM_IG_RAM_HWM_1(_x)     (((_x) & 0x3) << 2)
#define   NFP_MSFCL_XU_CTL_RAM_HWM_IG_RAM_HWM_1_of(_x)  (((_x) >> 2) & 0x3)
#define     NFP_MSFCL_XU_CTL_RAM_HWM_IG_RAM_HWM_1_HWM_3_QTR (3)
#define     NFP_MSFCL_XU_CTL_RAM_HWM_IG_RAM_HWM_1_HWM_1_HALF (2)
#define     NFP_MSFCL_XU_CTL_RAM_HWM_IG_RAM_HWM_1_HWM_1_QTR (1)
#define   NFP_MSFCL_XU_CTL_RAM_HWM_IG_RAM_HWM_0(_x)     ((_x) & 0x3)
#define   NFP_MSFCL_XU_CTL_RAM_HWM_IG_RAM_HWM_0_of(_x)  ((_x) & 0x3)
#define     NFP_MSFCL_XU_CTL_RAM_HWM_IG_RAM_HWM_0_HWM_3_QTR (3)
#define     NFP_MSFCL_XU_CTL_RAM_HWM_IG_RAM_HWM_0_HWM_1_HALF (2)
#define     NFP_MSFCL_XU_CTL_RAM_HWM_IG_RAM_HWM_0_HWM_1_QTR (1)
/* Register Type: MsclXuCtlEnetMacSpd */
#define NFP_MSFCL_XU_CTL_ENETMACSPEED  0x0204
#define   NFP_MSFCL_XU_CTL_ENETMACSPEED_ENETMACSPEED    (0x1)
#define     NFP_MSFCL_XU_CTL_ENETMACSPEED_ENETMACSPEED_SPEED_10G (0x0)
#define     NFP_MSFCL_XU_CTL_ENETMACSPEED_ENETMACSPEED_SPEED_1G (0x1)
/* Register Type: MsclXuCtlXgmiiSel */
#define NFP_MSFCL_XU_CTL_XGMII_SEL     0x0208
#define   NFP_MSFCL_XU_CTL_XGMII_SEL_XGMII_SEL          (0x1)
#define     NFP_MSFCL_XU_CTL_XGMII_SEL_XGMII_SEL_GMII   (0x0)
#define     NFP_MSFCL_XU_CTL_XGMII_SEL_XGMII_SEL_XGMII  (0x1)
/* Register Type: MsclXuCtlChecksumPrepend */
#define NFP_MSFCL_XU_CTL_CKSUMPREPEND  0x020c
#define   NFP_MSFCL_XU_CTL_CKSUMPREPEND_IG_TIMEPREPEND(_x) (((_x) & 0xf) << 12)
#define   NFP_MSFCL_XU_CTL_CKSUMPREPEND_IG_TIMEPREPEND_of(_x) (((_x) >> 12) & 0xf)
#define   NFP_MSFCL_XU_CTL_CKSUMPREPEND_IG_CHECKPREPEND(_x) (((_x) & 0xf) << 8)
#define   NFP_MSFCL_XU_CTL_CKSUMPREPEND_IG_CHECKPREPEND_of(_x) (((_x) >> 8) & 0xf)
#define   NFP_MSFCL_XU_CTL_CKSUMPREPEND_EG_TIMEPREPEND(_x) (((_x) & 0xf) << 4)
#define   NFP_MSFCL_XU_CTL_CKSUMPREPEND_EG_TIMEPREPEND_of(_x) (((_x) >> 4) & 0xf)
#define   NFP_MSFCL_XU_CTL_CKSUMPREPEND_EG_CHECKPREPEND(_x) ((_x) & 0xf)
#define   NFP_MSFCL_XU_CTL_CKSUMPREPEND_EG_CHECKPREPEND_of(_x) ((_x) & 0xf)
/* Register Type: MsclXuCtlUnused */
#define NFP_MSFCL_XU_CTL_UNUSED        0x0210
#define   NFP_MSFCL_XU_CTL_UNUSED_UNUSED(_x)            ((_x) & 0xf)
#define   NFP_MSFCL_XU_CTL_UNUSED_UNUSED_of(_x)         ((_x) & 0xf)
/* Register Type: MsclXuCtlSpareReg */
#define NFP_MSFCL_XU_CTL_SPARE_REG     0x0214
#define   NFP_MSFCL_XU_CTL_SPARE_REG_SPARE_REG_UNUSED(_x) ((_x) & 0xffff)
#define   NFP_MSFCL_XU_CTL_SPARE_REG_SPARE_REG_UNUSED_of(_x) ((_x) & 0xffff)
/* Register Type: MsclXuCtlIngresRamclr */
#define NFP_MSFCL_XU_CTL_IG_RAMCLR     0x0218
#define   NFP_MSFCL_XU_CTL_IG_RAMCLR_IG_RAMCLR(_x)      ((_x) & 0xf)
#define   NFP_MSFCL_XU_CTL_IG_RAMCLR_IG_RAMCLR_of(_x)   ((_x) & 0xf)
/* Register Type: MsclXuCtlPolInvTx */
#define NFP_MSFCL_XU_CTL_POLARITY_INV_TX 0x021c
#define   NFP_MSFCL_XU_CTL_POLARITY_INV_TX_POLARITY_INV_TX(_x) ((_x) & 0xf)
#define   NFP_MSFCL_XU_CTL_POLARITY_INV_TX_POLARITY_INV_TX_of(_x) ((_x) & 0xf)
/* Register Type: MsclXuCtlPolInv */
#define NFP_MSFCL_XU_CTL_POLARITY_INV  0x0220
#define   NFP_MSFCL_XU_CTL_POLARITY_INV_POLARITY_INV(_x) ((_x) & 0xf)
#define   NFP_MSFCL_XU_CTL_POLARITY_INV_POLARITY_INV_of(_x) ((_x) & 0xf)
/* Register Type: MsclXuCtlMaxburst */
#define NFP_MSFCL_XU_CTL_MAXBURST      0x0224
#define   NFP_MSFCL_XU_CTL_MAXBURST_MAXBURST_UPDATE_FIELD(_x) (((_x) & 0x3f) << 2)
#define   NFP_MSFCL_XU_CTL_MAXBURST_MAXBURST_UPDATE_FIELD_of(_x) (((_x) >> 2) & 0x3f)
#define   NFP_MSFCL_XU_CTL_MAXBURST_MAXBURST_SIZE_FIELD(_x) ((_x) & 0x3)
#define   NFP_MSFCL_XU_CTL_MAXBURST_MAXBURST_SIZE_FIELD_of(_x) ((_x) & 0x3)
#define     NFP_MSFCL_XU_CTL_MAXBURST_MAXBURST_SIZE_FIELD_CREDIT_64 (0)
#define     NFP_MSFCL_XU_CTL_MAXBURST_MAXBURST_SIZE_FIELD_CREDIT_128 (1)
#define     NFP_MSFCL_XU_CTL_MAXBURST_MAXBURST_SIZE_FIELD_CREDIT_192 (2)
#define     NFP_MSFCL_XU_CTL_MAXBURST_MAXBURST_SIZE_FIELD_CREDIT_256 (3)
/* Register Type: MsclXuStatTimstmp */
#define NFP_MSFCL_XU_STAT_TSTAMP       0x0228
#define   NFP_MSFCL_XU_STAT_TSTAMP_TSTAMP_of(_x)        (_x)
/* Register Type: MsclXuCtlTxSpiLinkfcEn */
#define NFP_MSFCL_XU_CTL_TX_SPI_LINKFC_EN 0x022c
#define   NFP_MSFCL_XU_CTL_TX_SPI_LINKFC_EN_RX_SPI_LINKFC_EN (0x1 << 1)
#define   NFP_MSFCL_XU_CTL_TX_SPI_LINKFC_EN_TX_SPI_LINKFC_EN (0x1)
/* Register Type: MsclXuStatLiveStat */
#define NFP_MSFCL_XU_STAT_LIVE_STAT    0x0230
#define   NFP_MSFCL_XU_STAT_LIVE_STAT_MAC_INT_of(_x)    (((_x) >> 28) & 0xf)
#define   NFP_MSFCL_XU_STAT_LIVE_STAT_EG_FIFO_HAVE_CH3  (0x1 << 27)
#define   NFP_MSFCL_XU_STAT_LIVE_STAT_EG_FIFO_LEVEL_CH3_of(_x) (((_x) >> 25) & 0x3)
#define     NFP_MSFCL_XU_STAT_LIVE_STAT_EG_FIFO_LEVEL_CH3_LEVEL_0 (0)
#define     NFP_MSFCL_XU_STAT_LIVE_STAT_EG_FIFO_LEVEL_CH3_LEVEL_1 (1)
#define     NFP_MSFCL_XU_STAT_LIVE_STAT_EG_FIFO_LEVEL_CH3_LEVEL_2 (2)
#define     NFP_MSFCL_XU_STAT_LIVE_STAT_EG_FIFO_LEVEL_CH3_LEVEL_3 (3)
#define   NFP_MSFCL_XU_STAT_LIVE_STAT_EG_FIFO_HAVE_CH2  (0x1 << 24)
#define   NFP_MSFCL_XU_STAT_LIVE_STAT_EG_FIFO_LEVEL_CH2_of(_x) (((_x) >> 22) & 0x3)
#define     NFP_MSFCL_XU_STAT_LIVE_STAT_EG_FIFO_LEVEL_CH2_LEVEL_0 (0)
#define     NFP_MSFCL_XU_STAT_LIVE_STAT_EG_FIFO_LEVEL_CH2_LEVEL_1 (1)
#define     NFP_MSFCL_XU_STAT_LIVE_STAT_EG_FIFO_LEVEL_CH2_LEVEL_2 (2)
#define     NFP_MSFCL_XU_STAT_LIVE_STAT_EG_FIFO_LEVEL_CH2_LEVEL_3 (3)
#define   NFP_MSFCL_XU_STAT_LIVE_STAT_EG_FIFO_HAVE_CH1  (0x1 << 21)
#define   NFP_MSFCL_XU_STAT_LIVE_STAT_EG_FIFO_LEVEL_CH1_of(_x) (((_x) >> 19) & 0x3)
#define     NFP_MSFCL_XU_STAT_LIVE_STAT_EG_FIFO_LEVEL_CH1_LEVEL_0 (0)
#define     NFP_MSFCL_XU_STAT_LIVE_STAT_EG_FIFO_LEVEL_CH1_LEVEL_1 (1)
#define     NFP_MSFCL_XU_STAT_LIVE_STAT_EG_FIFO_LEVEL_CH1_LEVEL_2 (2)
#define     NFP_MSFCL_XU_STAT_LIVE_STAT_EG_FIFO_LEVEL_CH1_LEVEL_3 (3)
#define   NFP_MSFCL_XU_STAT_LIVE_STAT_EG_FIFO_HAVE_CH0  (0x1 << 18)
#define   NFP_MSFCL_XU_STAT_LIVE_STAT_EG_FIFO_LEVEL_CH0_of(_x) (((_x) >> 16) & 0x3)
#define     NFP_MSFCL_XU_STAT_LIVE_STAT_EG_FIFO_LEVEL_CH0_LEVEL_0 (0)
#define     NFP_MSFCL_XU_STAT_LIVE_STAT_EG_FIFO_LEVEL_CH0_LEVEL_1 (1)
#define     NFP_MSFCL_XU_STAT_LIVE_STAT_EG_FIFO_LEVEL_CH0_LEVEL_2 (2)
#define     NFP_MSFCL_XU_STAT_LIVE_STAT_EG_FIFO_LEVEL_CH0_LEVEL_3 (3)
#define   NFP_MSFCL_XU_STAT_LIVE_STAT_IG_CREDIT_AVAIL_CH3 (0x1 << 15)
#define   NFP_MSFCL_XU_STAT_LIVE_STAT_IG_CREDIT_AVAIL_CH2 (0x1 << 14)
#define   NFP_MSFCL_XU_STAT_LIVE_STAT_IG_CREDIT_AVAIL_CH1 (0x1 << 13)
#define   NFP_MSFCL_XU_STAT_LIVE_STAT_IG_CREDIT_AVAIL_CH0 (0x1 << 12)
#define   NFP_MSFCL_XU_STAT_LIVE_STAT_IG_FIFO_HAVE_CH3  (0x1 << 11)
#define   NFP_MSFCL_XU_STAT_LIVE_STAT_IG_FIFO_LEVEL_CH3_of(_x) (((_x) >> 9) & 0x3)
#define     NFP_MSFCL_XU_STAT_LIVE_STAT_IG_FIFO_LEVEL_CH3_LEVEL_0 (0)
#define     NFP_MSFCL_XU_STAT_LIVE_STAT_IG_FIFO_LEVEL_CH3_LEVEL_1 (1)
#define     NFP_MSFCL_XU_STAT_LIVE_STAT_IG_FIFO_LEVEL_CH3_LEVEL_2 (2)
#define     NFP_MSFCL_XU_STAT_LIVE_STAT_IG_FIFO_LEVEL_CH3_LEVEL_3 (3)
#define   NFP_MSFCL_XU_STAT_LIVE_STAT_IG_FIFO_HAVE_CH2  (0x1 << 8)
#define   NFP_MSFCL_XU_STAT_LIVE_STAT_IG_FIFO_LEVEL_CH2_of(_x) (((_x) >> 6) & 0x3)
#define     NFP_MSFCL_XU_STAT_LIVE_STAT_IG_FIFO_LEVEL_CH2_LEVEL_0 (0)
#define     NFP_MSFCL_XU_STAT_LIVE_STAT_IG_FIFO_LEVEL_CH2_LEVEL_1 (1)
#define     NFP_MSFCL_XU_STAT_LIVE_STAT_IG_FIFO_LEVEL_CH2_LEVEL_2 (2)
#define     NFP_MSFCL_XU_STAT_LIVE_STAT_IG_FIFO_LEVEL_CH2_LEVEL_3 (3)
#define   NFP_MSFCL_XU_STAT_LIVE_STAT_IG_FIFO_HAVE_CH1  (0x1 << 5)
#define   NFP_MSFCL_XU_STAT_LIVE_STAT_IG_FIFO_LEVEL_CH1_of(_x) (((_x) >> 3) & 0x3)
#define     NFP_MSFCL_XU_STAT_LIVE_STAT_IG_FIFO_LEVEL_CH1_LEVEL_0 (0)
#define     NFP_MSFCL_XU_STAT_LIVE_STAT_IG_FIFO_LEVEL_CH1_LEVEL_1 (1)
#define     NFP_MSFCL_XU_STAT_LIVE_STAT_IG_FIFO_LEVEL_CH1_LEVEL_2 (2)
#define     NFP_MSFCL_XU_STAT_LIVE_STAT_IG_FIFO_LEVEL_CH1_LEVEL_3 (3)
#define   NFP_MSFCL_XU_STAT_LIVE_STAT_IG_FIFO_HAVE_CH0  (0x1 << 2)
#define   NFP_MSFCL_XU_STAT_LIVE_STAT_IG_FIFO_LEVEL_CH0_of(_x) ((_x) & 0x3)
#define     NFP_MSFCL_XU_STAT_LIVE_STAT_IG_FIFO_LEVEL_CH0_LEVEL_0 (0)
#define     NFP_MSFCL_XU_STAT_LIVE_STAT_IG_FIFO_LEVEL_CH0_LEVEL_1 (1)
#define     NFP_MSFCL_XU_STAT_LIVE_STAT_IG_FIFO_LEVEL_CH0_LEVEL_2 (2)
#define     NFP_MSFCL_XU_STAT_LIVE_STAT_IG_FIFO_LEVEL_CH0_LEVEL_3 (3)
/* Register Type: MsclXuStatIgErr */
#define NFP_MSFCL_XU_STAT_IG_ERR       0x0234
#define   NFP_MSFCL_XU_STAT_IG_ERR_IG_FRAME_ERR_CH3     (0x1 << 15)
#define   NFP_MSFCL_XU_STAT_IG_ERR_IG_FRAME_ERR_CH2     (0x1 << 14)
#define   NFP_MSFCL_XU_STAT_IG_ERR_IG_FRAME_ERR_CH1     (0x1 << 13)
#define   NFP_MSFCL_XU_STAT_IG_ERR_IG_FRAME_ERR_CH0     (0x1 << 12)
#define   NFP_MSFCL_XU_STAT_IG_ERR_IG_FILTERED_CH3      (0x1 << 11)
#define   NFP_MSFCL_XU_STAT_IG_ERR_IG_FILTERED_CH2      (0x1 << 10)
#define   NFP_MSFCL_XU_STAT_IG_ERR_IG_FILTERED_CH1      (0x1 << 9)
#define   NFP_MSFCL_XU_STAT_IG_ERR_IG_FILTERED_CH0      (0x1 << 8)
#define   NFP_MSFCL_XU_STAT_IG_ERR_IG_CK_FIFO_OVR_ERR_CH3 (0x1 << 7)
#define   NFP_MSFCL_XU_STAT_IG_ERR_IG_CK_FIFO_OVR_ERR_CH2 (0x1 << 6)
#define   NFP_MSFCL_XU_STAT_IG_ERR_IG_CK_FIFO_OVR_ERR_CH1 (0x1 << 5)
#define   NFP_MSFCL_XU_STAT_IG_ERR_IG_CK_FIFO_OVR_ERR_CH0 (0x1 << 4)
#define   NFP_MSFCL_XU_STAT_IG_ERR_IG_RAMFIFO_OVR_ERR_CH3 (0x1 << 3)
#define   NFP_MSFCL_XU_STAT_IG_ERR_IG_RAMFIFO_OVR_ERR_CH2 (0x1 << 2)
#define   NFP_MSFCL_XU_STAT_IG_ERR_IG_RAMFIFO_OVR_ERR_CH1 (0x1 << 1)
#define   NFP_MSFCL_XU_STAT_IG_ERR_IG_RAMFIFO_OVR_ERR_CH0 (0x1)
/* Register Type: MsclXuStatEgErr */
#define NFP_MSFCL_XU_STAT_EG_ERR       0x0238
#define   NFP_MSFCL_XU_STAT_EG_ERR_EG_PARSE_ERR_CH3     (0x1 << 23)
#define   NFP_MSFCL_XU_STAT_EG_ERR_EG_PARSE_ERR_CH2     (0x1 << 22)
#define   NFP_MSFCL_XU_STAT_EG_ERR_EG_PARSE_ERR_CH1     (0x1 << 21)
#define   NFP_MSFCL_XU_STAT_EG_ERR_EG_PARSE_ERR_CH0     (0x1 << 20)
#define   NFP_MSFCL_XU_STAT_EG_ERR_EG_NO_SOP_ERR_CH3    (0x1 << 19)
#define   NFP_MSFCL_XU_STAT_EG_ERR_EG_NO_SOP_ERR_CH2    (0x1 << 18)
#define   NFP_MSFCL_XU_STAT_EG_ERR_EG_NO_SOP_ERR_CH1    (0x1 << 17)
#define   NFP_MSFCL_XU_STAT_EG_ERR_EG_NO_SOP_ERR_CH0    (0x1 << 16)
#define   NFP_MSFCL_XU_STAT_EG_ERR_EG_FRAME_ERR_CH3     (0x1 << 15)
#define   NFP_MSFCL_XU_STAT_EG_ERR_EG_FRAME_ERR_CH2     (0x1 << 14)
#define   NFP_MSFCL_XU_STAT_EG_ERR_EG_FRAME_ERR_CH1     (0x1 << 13)
#define   NFP_MSFCL_XU_STAT_EG_ERR_EG_FRAME_ERR_CH0     (0x1 << 12)
#define   NFP_MSFCL_XU_STAT_EG_ERR_EG_FILTERED_CH3      (0x1 << 11)
#define   NFP_MSFCL_XU_STAT_EG_ERR_EG_FILTERED_CH2      (0x1 << 10)
#define   NFP_MSFCL_XU_STAT_EG_ERR_EG_FILTERED_CH1      (0x1 << 9)
#define   NFP_MSFCL_XU_STAT_EG_ERR_EG_FILTERED_CH0      (0x1 << 8)
#define   NFP_MSFCL_XU_STAT_EG_ERR_EG_CK_FIFO_OVR_ERR_CH3 (0x1 << 7)
#define   NFP_MSFCL_XU_STAT_EG_ERR_EG_CK_FIFO_OVR_ERR_CH2 (0x1 << 6)
#define   NFP_MSFCL_XU_STAT_EG_ERR_EG_CK_FIFO_OVR_ERR_CH1 (0x1 << 5)
#define   NFP_MSFCL_XU_STAT_EG_ERR_EG_CK_FIFO_OVR_ERR_CH0 (0x1 << 4)
#define   NFP_MSFCL_XU_STAT_EG_ERR_EG_RAMFIFO_OVR_ERR_CH3 (0x1 << 3)
#define   NFP_MSFCL_XU_STAT_EG_ERR_EG_RAMFIFO_OVR_ERR_CH2 (0x1 << 2)
#define   NFP_MSFCL_XU_STAT_EG_ERR_EG_RAMFIFO_OVR_ERR_CH1 (0x1 << 1)
#define   NFP_MSFCL_XU_STAT_EG_ERR_EG_RAMFIFO_OVR_ERR_CH0 (0x1)
/* Register Type: MsclXuStatIgErrIntEn */
#define NFP_MSFCL_XU_STAT_IG_ERR_INT_EN 0x023c
#define   NFP_MSFCL_XU_STAT_IG_ERR_INT_EN_IG_ERR_INT_EN_of(_x) ((_x) & 0xffff)
/* Register Type: MsclXuStatEgErrIntEn */
#define NFP_MSFCL_XU_STAT_EG_ERR_INT_EN 0x0240
#define   NFP_MSFCL_XU_STAT_EG_ERR_INT_EN_EG_ERR_INT_EN_of(_x) ((_x) & 0xffffff)
/* Register Type: MsclXuCtlTxTest */
#define NFP_MSFCL_XU_CTL_TX_TEST       0x0244
#define   NFP_MSFCL_XU_CTL_TX_TEST_TX_TEST_SEL(_x)      (((_x) & 0x3) << 1)
#define   NFP_MSFCL_XU_CTL_TX_TEST_TX_TEST_SEL_of(_x)   (((_x) >> 1) & 0x3)
#define   NFP_MSFCL_XU_CTL_TX_TEST_TX_TEST_EN           (0x1)
/* Register Type: MsclPmCtlReg */
#define NFP_MSFCL_PM_CTL_REG           0x0300
#define   NFP_MSFCL_PM_CTL_REG_REG_MODULE_SEL(_x)       (((_x) & 0xffffff) << 6)
#define   NFP_MSFCL_PM_CTL_REG_REG_MODULE_SEL_of(_x)    (((_x) >> 6) & 0xffffff)
#define   NFP_MSFCL_PM_CTL_REG_REG_LANE_SEL(_x)         ((_x) & 0x3f)
#define   NFP_MSFCL_PM_CTL_REG_REG_LANE_SEL_of(_x)      ((_x) & 0x3f)
/* Register Type: MsclMsCtlCfgDisFwRxThdFreelst */
#define NFP_MSFCL_MS_CTL_CFG_DISABLE_FW_RX_THD_FLIST 0x0304
#define   NFP_MSFCL_MS_CTL_CFG_DISABLE_FW_RX_THD_FLIST_DISABLE_FW_RX_THD_FLIST (0x1)
/* Register Type: MsclMsCtlCfgCmdInletFifoThr */
#define NFP_MSFCL_MS_CTL_CFG_CMD_INLET_FIFO_THR 0x0308
#define   NFP_MSFCL_MS_CTL_CFG_CMD_INLET_FIFO_THR_CMD_INLET_FIFO_THR(_x) ((_x) & 0x1f)
#define   NFP_MSFCL_MS_CTL_CFG_CMD_INLET_FIFO_THR_CMD_INLET_FIFO_THR_of(_x) ((_x) & 0x1f)
/* Register Type: MsclMsCtlCfgRcvbufHWM */
#define NFP_MSFCL_MS_CTL_CFG_RCVBUF_HWM 0x030c
#define   NFP_MSFCL_MS_CTL_CFG_RCVBUF_HWM_RCVBUF_HWM(_x) ((_x) & 0x7ff)
#define   NFP_MSFCL_MS_CTL_CFG_RCVBUF_HWM_RCVBUF_HWM_of(_x) ((_x) & 0x7ff)
/* Register Type: MsclMsCtlCfgRdCmdFifoThr */
#define NFP_MSFCL_MS_CTL_CFG_READ_CMD_FIFO_THR 0x0310
#define   NFP_MSFCL_MS_CTL_CFG_READ_CMD_FIFO_THR_READ_CMD_FIFO_THR(_x) ((_x) & 0x1f)
#define   NFP_MSFCL_MS_CTL_CFG_READ_CMD_FIFO_THR_READ_CMD_FIFO_THR_of(_x) ((_x) & 0x1f)
/* Register Type: MsclMsCtlCfgGen2xPullIdFull */
#define NFP_MSFCL_MS_CTL_CFG_GEN_2X_PULL_ID_FULL 0x0314
#define   NFP_MSFCL_MS_CTL_CFG_GEN_2X_PULL_ID_FULL_GEN_2X_PULL_ID_FULL (0x1)
/* Register Type: MsCtlClkGateOvrd */
#define NFP_MSFCL_MS_CTL_CLK_GATE_OVRD 0x0318
#define   NFP_MSFCL_MS_CTL_CLK_GATE_OVRD_MS_CTL_CLK_GATE_OVRD(_x) ((_x) & 0xf)
#define   NFP_MSFCL_MS_CTL_CLK_GATE_OVRD_MS_CTL_CLK_GATE_OVRD_of(_x) ((_x) & 0xf)
/* Register Type: MsclMsCtlCfgGen2xwideToIo */
#define NFP_MSFCL_MS_CTL_CFG_GEN_2XWIDE_TO_IO 0x031c
#define   NFP_MSFCL_MS_CTL_CFG_GEN_2XWIDE_TO_IO_GEN_2XWIDE_TO_IO (0x1)
/* Register Type: MsclMsCtlCfgSpi4ApCswLtstBfSt */
#define NFP_MSFCL_MS_CTL_CFG_SPI4_AP_CSW_WITH_LATEST_BUF_STATUS 0x0320
#define   NFP_MSFCL_MS_CTL_CFG_SPI4_AP_CSW_WITH_LATEST_BUF_STATUS_SPI4_AP_CSW_WITH_LATEST_BUF_STATUS (0x1)


#define NFP_XPB_MSF0_GLOBAL             NFP_XPB_DEST(16,16)
#define NFP_XPB_MSF0_CONFIG             NFP_XPB_DEST(16,17)
#define NFP_XPB_MSF0_ECC                NFP_XPB_DEST(16,18)
#define NFP_XPB_MSF0_MSF_ECC            NFP_XPB_DEST(16,19)
#define NFP_XPB_MSF0_INTMGR             NFP_XPB_DEST(16,20)

#define NFP_XPB_MSF1_GLOBAL             NFP_XPB_DEST(18,16)
#define NFP_XPB_MSF1_CONFIG             NFP_XPB_DEST(18,17)
#define NFP_XPB_MSF1_ECC                NFP_XPB_DEST(18,18)
#define NFP_XPB_MSF1_MSF_ECC            NFP_XPB_DEST(18,19)
#define NFP_XPB_MSF1_INTMGR             NFP_XPB_DEST(18,20)

#endif /* NFP3200_NFP_MSF_H */
