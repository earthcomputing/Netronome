/*
 * Copyright (C) 2008-2011, Netronome Systems, Inc.  All rights reserved.
 *
 * Disclaimer: this file is provided without any warranty
 * as part of an early-access program, and the content is
 * bound to change before the final release.
 */

#ifndef NFP3200_NFP_PCIE_H
#define NFP3200_NFP_PCIE_H

/* HGID: nfp3200/pcie.desc = ade0020e9035 */
#define NFP_PCIE_SRAM_BASE             0x0000
#define NFP_PCIE_SRAM_LEN              0x10000
#define NFP_PCIE_IM                    0x10000
#define NFP_PCIE_EM                    0x20000
/* Register Type: PcieToCppBar */
#define NFP_PCIE_BARCFG_P2C(_bar)      (0x30000 + (0x4 * (_bar)))
#define   NFP_PCIE_BARCFG_P2C_MAPTYPE(_x)               (((_x) & 0x3) << 30)
#define   NFP_PCIE_BARCFG_P2C_MAPTYPE_of(_x)            (((_x) >> 30) & 0x3)
#define     NFP_PCIE_BARCFG_P2C_MAPTYPE_BULK            (0)
#define     NFP_PCIE_BARCFG_P2C_MAPTYPE_CPP             (1)
#define     NFP_PCIE_BARCFG_P2C_MAPTYPE_MIXED           (2)
#define   NFP_PCIE_BARCFG_P2C_TGTACT(_x)                (((_x) & 0x1f) << 25)
#define   NFP_PCIE_BARCFG_P2C_TGTACT_of(_x)             (((_x) >> 25) & 0x1f)
#define   NFP_PCIE_BARCFG_P2C_TOKACTSEL(_x)             (((_x) & 0x3) << 23)
#define   NFP_PCIE_BARCFG_P2C_TOKACTSEL_of(_x)          (((_x) >> 23) & 0x3)
#define     NFP_PCIE_BARCFG_P2C_TOKACTSEL_NONE          (0)
#define     NFP_PCIE_BARCFG_P2C_TOKACTSEL_ADDR          (1)
#define     NFP_PCIE_BARCFG_P2C_TOKACTSEL_BARCFG        (2)
#define   NFP_PCIE_BARCFG_P2C_LEN                       (0x1 << 22)
#define     NFP_PCIE_BARCFG_P2C_LEN_32BIT               (0x0)
#define     NFP_PCIE_BARCFG_P2C_LEN_64BIT               (0x400000)
#define   NFP_PCIE_BARCFG_P2C_BASE(_x)                  ((_x) & 0x3fffff)
#define   NFP_PCIE_BARCFG_P2C_BASE_of(_x)               ((_x) & 0x3fffff)
/* Register Type: CppToPcieBar */
#define NFP_PCIE_BARCFG_C2P(_bar)      (0x30020 + (0x4 * ((_bar) & 0x7)))
#define   NFP_PCIE_BARCFG_C2P_TYPE(_x)                  (((_x) & 0x3) << 30)
#define   NFP_PCIE_BARCFG_C2P_TYPE_of(_x)               (((_x) >> 30) & 0x3)
#define     NFP_PCIE_BARCFG_C2P_TYPE_MEM                (0)
#define     NFP_PCIE_BARCFG_C2P_TYPE_IO                 (1)
#define     NFP_PCIE_BARCFG_C2P_TYPE_CFG                (2)
#define     NFP_PCIE_BARCFG_C2P_TYPE_VIRT               (3)
#define   NFP_PCIE_BARCFG_C2P_ARI_ENABLE                (0x1 << 28)
#define   NFP_PCIE_BARCFG_C2P_ARI(_x)                   (((_x) & 0xff) << 20)
#define   NFP_PCIE_BARCFG_C2P_ARI_of(_x)                (((_x) >> 20) & 0xff)
#define   NFP_PCIE_BARCFG_C2P_ADDR(_x)                  ((_x) & 0x7ffff)
#define   NFP_PCIE_BARCFG_C2P_ADDR_of(_x)               ((_x) & 0x7ffff)
/* Register Type: DMAWritePtr */
#define NFP_PCIE_DMA_WRITEPTR_TPCI_HI  0x40000
#define NFP_PCIE_DMA_WRITEPTR_TPCI_LO  0x40010
#define NFP_PCIE_DMA_WRITEPTR_FPCI_HI  0x40020
#define NFP_PCIE_DMA_WRITEPTR_FPCI_LO  0x40030
#define   NFP_PCIE_DMA_WRITEPTR_PTR_of(_x)              (_x)
/* Register Type: DMACommand */
#define NFP_PCIE_DMA_CMD_TPCI_HI       0x40000
#define NFP_PCIE_DMA_CMD_TPCI_LO       0x40010
#define NFP_PCIE_DMA_CMD_FPCI_HI       0x40020
#define NFP_PCIE_DMA_CMD_FPCI_LO       0x40030
/* Word 3 */
#define   NFP_PCIE_DMA_CMD_LENGTH(_x)                   (((_x) & 0xfff) << 20)
#define   NFP_PCIE_DMA_CMD_LENGTH_of(_x)                (((_x) >> 20) & 0xfff)
#define   NFP_PCIE_DMA_CMD_RID(_x)                      (((_x) & 0xff) << 12)
#define   NFP_PCIE_DMA_CMD_RID_of(_x)                   (((_x) >> 12) & 0xff)
#define   NFP_PCIE_DMA_CMD_RID_OVERRIDE                 (0x1 << 11)
#define     NFP_PCIE_DMA_CMD_RID_OVERRIDE_ENABLE        (0x800)
#define   NFP_PCIE_DMA_CMD_NO_SNOOP                     (0x1 << 10)
#define     NFP_PCIE_DMA_CMD_NO_SNOOP_ENABLE            (0x400)
#define   NFP_PCIE_DMA_CMD_RELAXED_ORDER                (0x1 << 9)
#define     NFP_PCIE_DMA_CMD_RELAXED_ORDER_ENABLE       (0x200)
#define   NFP_PCIE_DMA_CMD_PCIE_ADDR_HI(_x)             ((_x) & 0xff)
#define   NFP_PCIE_DMA_CMD_PCIE_ADDR_HI_of(_x)          ((_x) & 0xff)
/* Word 2 */
#define   NFP_PCIE_DMA_CMD_PCIE_ADDR_LO(_x)             (_x)
#define   NFP_PCIE_DMA_CMD_PCIE_ADDR_LO_of(_x)          (_x)
/* Word 1 */
#define   NFP_PCIE_DMA_CMD_CPL(_x)                      (((_x) & 0xffff) << 16)
#define   NFP_PCIE_DMA_CMD_CPL_of(_x)                   (((_x) >> 16) & 0xffff)
#define   NFP_PCIE_DMA_CMD_TOKEN(_x)                    (((_x) & 0x3) << 14)
#define   NFP_PCIE_DMA_CMD_TOKEN_of(_x)                 (((_x) >> 14) & 0x3)
#define   NFP_PCIE_DMA_CMD_WORD_ONLY                    (0x1 << 13)
#define     NFP_PCIE_DMA_CMD_WORD_ONLY_ENABLE           (0x2000)
#define   NFP_PCIE_DMA_CMD_TARGET64                     (0x1 << 12)
#define     NFP_PCIE_DMA_CMD_TARGET64_ENABLE            (0x1000)
#define   NFP_PCIE_DMA_CMD_CPP_TARGET(_x)               (((_x) & 0xf) << 8)
#define   NFP_PCIE_DMA_CMD_CPP_TARGET_of(_x)            (((_x) >> 8) & 0xf)
#define   NFP_PCIE_DMA_CMD_CPP_ADDR_HI(_x)              ((_x) & 0xff)
#define   NFP_PCIE_DMA_CMD_CPP_ADDR_HI_of(_x)           ((_x) & 0xff)
/* Word 0 */
#define   NFP_PCIE_DMA_CMD_CPP_ADDR_LO(_x)              (_x)
#define   NFP_PCIE_DMA_CMD_CPP_ADDR_LO_of(_x)           (_x)
/* Register Type: DMAControl */
#define NFP_PCIE_DMA_CTRL_TPCI_HI      0x40040
#define NFP_PCIE_DMA_CTRL_TPCI_LO      0x40044
#define NFP_PCIE_DMA_CTRL_FPCI_HI      0x40048
#define NFP_PCIE_DMA_CTRL_FPCI_LO      0x4004c
#define   NFP_PCIE_DMA_CTRL_DESC_ERROR                  (0x1 << 5)
#define     NFP_PCIE_DMA_CTRL_DESC_ERROR_CLEAR          (0x20)
#define   NFP_PCIE_DMA_CTRL_PCIE_ERROR                  (0x1 << 4)
#define     NFP_PCIE_DMA_CTRL_PCIE_ERROR_CLEAR          (0x10)
#define   NFP_PCIE_DMA_CTRL_CPP_ERROR                   (0x1 << 3)
#define     NFP_PCIE_DMA_CTRL_CPP_ERROR_CLEAR           (0x8)
#define   NFP_PCIE_DMA_CTRL_QUEUE_OVERFLOW              (0x1 << 2)
#define     NFP_PCIE_DMA_CTRL_QUEUE_OVERFLOW_CLEAR      (0x4)
#define   NFP_PCIE_DMA_CTRL_CONT_ON_ERROR               (0x1 << 1)
#define     NFP_PCIE_DMA_CTRL_CONT_ON_ERROR_ENABLE      (0x2)
#define   NFP_PCIE_DMA_CTRL_QUEUE_STOP                  (0x1)
#define     NFP_PCIE_DMA_CTRL_QUEUE_STOP_ENABLE         (0x1)
/* Register Type: DMAQueueStatus */
#define NFP_PCIE_DMA_QSTS_TPCI_HI      0x40040
#define NFP_PCIE_DMA_QSTS_TPCI_LO      0x40044
#define NFP_PCIE_DMA_QSTS_FPCI_HI      0x40048
#define NFP_PCIE_DMA_QSTS_FPCI_LO      0x4004c
#define   NFP_PCIE_DMA_QSTS_AVAIL_of(_x)                (((_x) >> 24) & 0xff)
#define   NFP_PCIE_DMA_QSTS_READ_PTR_of(_x)             (((_x) >> 16) & 0xff)
#define   NFP_PCIE_DMA_QSTS_POP_PTR_of(_x)              (((_x) >> 8) & 0xff)
#define   NFP_PCIE_DMA_QSTS_QUEUE_FULL                  (0x1 << 7)
#define   NFP_PCIE_DMA_QSTS_QUEUE_ACTIVE                (0x1 << 6)
#define   NFP_PCIE_DMA_QSTS_DESC_ERROR                  (0x1 << 5)
#define   NFP_PCIE_DMA_QSTS_PCIE_ERROR                  (0x1 << 4)
#define   NFP_PCIE_DMA_QSTS_CPP_ERROR                   (0x1 << 3)
#define   NFP_PCIE_DMA_QSTS_QUEUE_OVERFLOW              (0x1 << 2)
#define   NFP_PCIE_DMA_QSTS_CONT_ON_ERROR               (0x1 << 1)
#define   NFP_PCIE_DMA_QSTS_QUEUE_STOP                  (0x1)
#define NFP_PCIE_QUEUE(_q)             (0x80000 + (0x800 * ((_q) & 0xff)))
/* HGID: nfp3200/pcie_csr.desc = ade0020e9035 */
/* Register Type: PCIECntrlrConfig0 */
#define NFP_PCIE_CSR_CFG0              0x0000
#define   NFP_PCIE_CSR_CFG0_XXX_A                       (0x1 << 31)
#define     NFP_PCIE_CSR_CFG0_XXX_A_XXX_SHORT_0A        (0x0)
#define     NFP_PCIE_CSR_CFG0_XXX_A_XXX_SHORT_1A        (0x80000000)
#define   NFP_PCIE_CSR_CFG0_XXX_B                       (0x1 << 30)
#define     NFP_PCIE_CSR_CFG0_XXX_B_XXX_SHORT_0B        (0x0)
#define     NFP_PCIE_CSR_CFG0_XXX_B_XXX_SHORT_1B        (0x40000000)
#define   NFP_PCIE_CSR_CFG0_XXX_C                       (0x1 << 29)
#define     NFP_PCIE_CSR_CFG0_XXX_C_XXX_SHORT_0C        (0x0)
#define     NFP_PCIE_CSR_CFG0_XXX_C_XXX_SHORT_1C        (0x20000000)
#define   NFP_PCIE_CSR_CFG0_MGMT_REG_ENABLE             (0x1 << 27)
#define     NFP_PCIE_CSR_CFG0_MGMT_REG_ENABLE_XXX_SHORT_0D (0x0)
#define     NFP_PCIE_CSR_CFG0_MGMT_REG_ENABLE_XXX_SHORT_1D (0x8000000)
#define   NFP_PCIE_CSR_CFG0_TARGET_DEVICE(_x)           (((_x) & 0x1f) << 16)
#define   NFP_PCIE_CSR_CFG0_TARGET_DEVICE_of(_x)        (((_x) >> 16) & 0x1f)
#define   NFP_PCIE_CSR_CFG0_TARGET_BUS(_x)              (((_x) & 0xff) << 8)
#define   NFP_PCIE_CSR_CFG0_TARGET_BUS_of(_x)           (((_x) >> 8) & 0xff)
#define   NFP_PCIE_CSR_CFG0_XXX_READS_IN_FLIGHT_LIMIT(_x) (((_x) & 0x3) << 2)
#define   NFP_PCIE_CSR_CFG0_XXX_READS_IN_FLIGHT_LIMIT_of(_x) (((_x) >> 2) & 0x3)
#define     NFP_PCIE_CSR_CFG0_XXX_READS_IN_FLIGHT_LIMIT_32_READS_IN_FLIGHT (0)
#define     NFP_PCIE_CSR_CFG0_XXX_READS_IN_FLIGHT_LIMIT_8_READS_IN_FLIGHT (1)
#define     NFP_PCIE_CSR_CFG0_XXX_READS_IN_FLIGHT_LIMIT_16_READS_IN_FLIGHT (2)
#define     NFP_PCIE_CSR_CFG0_XXX_READS_IN_FLIGHT_LIMIT_XXX_RESERVED_XXX (3)
#define   NFP_PCIE_CSR_CFG0_PCIE_ADDR_MODE              (0x1 << 1)
#define     NFP_PCIE_CSR_CFG0_PCIE_ADDR_MODE_32         (0x0)
#define     NFP_PCIE_CSR_CFG0_PCIE_ADDR_MODE_64         (0x2)
#define   NFP_PCIE_CSR_CFG0_CPP_ADDR_MODE               (0x1)
#define     NFP_PCIE_CSR_CFG0_CPP_ADDR_MODE_40          (0x0)
#define     NFP_PCIE_CSR_CFG0_CPP_ADDR_MODE_32          (0x1)
/* Register Type: PCIESPARE */
#define NFP_PCIE_CSR_SPARE             0x0004
#define   NFP_PCIE_CSR_SPARE_DATA(_x)                   (_x)
#define   NFP_PCIE_CSR_SPARE_DATA_of(_x)                (_x)
/* Register Type: PCIECntrlrConfig1 */
#define NFP_PCIE_CSR_CFG1              0x0008
#define   NFP_PCIE_CSR_CFG1_INT_SEL(_x)                 (((_x) & 0x3) << 10)
#define   NFP_PCIE_CSR_CFG1_INT_SEL_of(_x)              (((_x) >> 10) & 0x3)
#define     NFP_PCIE_CSR_CFG1_INT_SEL_INTA              (0)
#define     NFP_PCIE_CSR_CFG1_INT_SEL_INTB              (1)
#define     NFP_PCIE_CSR_CFG1_INT_SEL_INTC              (2)
#define     NFP_PCIE_CSR_CFG1_INT_SEL_INTD              (3)
#define   NFP_PCIE_CSR_CFG1_PCIE_LANES(_x)              (((_x) & 0x3) << 8)
#define   NFP_PCIE_CSR_CFG1_PCIE_LANES_of(_x)           (((_x) >> 8) & 0x3)
#define     NFP_PCIE_CSR_CFG1_PCIE_LANES_X1             (0)
#define     NFP_PCIE_CSR_CFG1_PCIE_LANES_X2             (1)
#define     NFP_PCIE_CSR_CFG1_PCIE_LANES_X4             (2)
#define     NFP_PCIE_CSR_CFG1_PCIE_LANES_X8             (3)
#define   NFP_PCIE_CSR_CFG1_GEN2_ENABLE                 (0x1 << 7)
#define   NFP_PCIE_CSR_CFG1_BAR2_WINDOW                 (0x1 << 2)
#define     NFP_PCIE_CSR_CFG1_BAR2_WINDOW_30            (0x0)
#define     NFP_PCIE_CSR_CFG1_BAR2_WINDOW_24            (0x4)
#define   NFP_PCIE_CSR_CFG1_BAR1_WINDOW                 (0x1 << 1)
#define     NFP_PCIE_CSR_CFG1_BAR1_WINDOW_30            (0x0)
#define     NFP_PCIE_CSR_CFG1_BAR1_WINDOW_24            (0x2)
#define   NFP_PCIE_CSR_CFG1_BAR0_WINDOW                 (0x1)
#define     NFP_PCIE_CSR_CFG1_BAR0_WINDOW_30            (0x0)
#define     NFP_PCIE_CSR_CFG1_BAR0_WINDOW_24            (0x1)
/* Register Type: PCIECntrlrStat */
#define NFP_PCIE_CSR_STATUS            0x000c
#define   NFP_PCIE_CSR_STATUS_INTERRUPT                 (0x1 << 31)
#define   NFP_PCIE_CSR_STATUS_RCIOCFG_WRCPL             (0x1 << 30)
#define   NFP_PCIE_CSR_STATUS_ERROR_CORRECTABLE         (0x1 << 23)
#define   NFP_PCIE_CSR_STATUS_ERROR_NON_FATAL           (0x1 << 22)
#define   NFP_PCIE_CSR_STATUS_ERROR_FATAL               (0x1 << 21)
#define   NFP_PCIE_CSR_STATUS_MODE_IS_RC                (0x1 << 20)
#define     NFP_PCIE_CSR_STATUS_MODE_IS_RC_FIXME        (0x0)
#define     NFP_PCIE_CSR_STATUS_MODE_IS_RC_FIXME1       (0x100000)
#define   NFP_PCIE_CSR_STATUS_LINK_STATE_of(_x)         (((_x) >> 16) & 0xf)
#define     NFP_PCIE_CSR_STATUS_LINK_STATE_L0           (0)
#define     NFP_PCIE_CSR_STATUS_LINK_STATE_L0S          (1)
#define     NFP_PCIE_CSR_STATUS_LINK_STATE_L1           (2)
#define     NFP_PCIE_CSR_STATUS_LINK_STATE_L2           (3)
#define   NFP_PCIE_CSR_STATUS_POWER_STATE_of(_x)        (((_x) >> 13) & 0x7)
#define     NFP_PCIE_CSR_STATUS_POWER_STATE_DO_UNINITIALIZED (0)
#define     NFP_PCIE_CSR_STATUS_POWER_STATE_D0_ACTIVE   (1)
#define     NFP_PCIE_CSR_STATUS_POWER_STATE_D1          (2)
#define     NFP_PCIE_CSR_STATUS_POWER_STATE_D2          (3)
#define     NFP_PCIE_CSR_STATUS_POWER_STATE_D3_HOT      (4)
#define     NFP_PCIE_CSR_STATUS_POWER_STATE_D3_COLD     (5)
#define   NFP_PCIE_CSR_STATUS_F0_CONFIG_ENABLED         (0x1 << 12)
#define   NFP_PCIE_CSR_STATUS_F0_BUS_MASTER_ENABLED     (0x1 << 11)
#define   NFP_PCIE_CSR_STATUS_F0_MMIO_ENABLED           (0x1 << 10)
#define   NFP_PCIE_CSR_STATUS_F0_IO_ENABLED             (0x1 << 9)
#define   NFP_PCIE_CSR_STATUS_MAX_PAYLOAD_of(_x)        (((_x) >> 6) & 0x7)
#define     NFP_PCIE_CSR_STATUS_MAX_PAYLOAD_128         (0)
#define     NFP_PCIE_CSR_STATUS_MAX_PAYLOAD_256         (1)
#define     NFP_PCIE_CSR_STATUS_MAX_PAYLOAD_FIXME2      (2)
#define     NFP_PCIE_CSR_STATUS_MAX_PAYLOAD_FIXME3      (3)
#define     NFP_PCIE_CSR_STATUS_MAX_PAYLOAD_FIXME4      (4)
#define     NFP_PCIE_CSR_STATUS_MAX_PAYLOAD_FIXME5      (5)
#define     NFP_PCIE_CSR_STATUS_MAX_PAYLOAD_FIXME6      (6)
#define     NFP_PCIE_CSR_STATUS_MAX_PAYLOAD_FIXME7      (7)
#define   NFP_PCIE_CSR_STATUS_MAX_READ_REQ_of(_x)       (((_x) >> 3) & 0x7)
#define     NFP_PCIE_CSR_STATUS_MAX_READ_REQ_128        (0)
#define     NFP_PCIE_CSR_STATUS_MAX_READ_REQ_256        (1)
#define     NFP_PCIE_CSR_STATUS_MAX_READ_REQ_512        (2)
#define     NFP_PCIE_CSR_STATUS_MAX_READ_REQ_1024       (3)
#define     NFP_PCIE_CSR_STATUS_MAX_READ_REQ_2048       (4)
#define     NFP_PCIE_CSR_STATUS_MAX_READ_REQ_4096       (5)
#define     NFP_PCIE_CSR_STATUS_MAX_READ_REQ_FIXME8     (6)
#define     NFP_PCIE_CSR_STATUS_MAX_READ_REQ_FIXME9     (7)
#define   NFP_PCIE_CSR_STATUS_HIGH_SPEED                (0x1 << 2)
#define   NFP_PCIE_CSR_STATUS_LINK_STATUS_of(_x)        ((_x) & 0x3)
#define     NFP_PCIE_CSR_STATUS_LINK_STATUS_DOWN        (0)
#define     NFP_PCIE_CSR_STATUS_LINK_STATUS_TRAIN       (1)
#define     NFP_PCIE_CSR_STATUS_LINK_STATUS_DL_INIT     (2)
#define     NFP_PCIE_CSR_STATUS_LINK_STATUS_UP          (3)
/* Register Type: PCIEPHYConfig */
#define NFP_PCIE_CSR_PHYCFG0           0x0010
#define NFP_PCIE_CSR_PHYCFG1           0x0014
#define NFP_PCIE_CSR_PHYCFG2           0x0018
#define NFP_PCIE_CSR_PHYCFG3           0x001c
#define NFP_PCIE_CSR_PHYCFG4           0x0020
#define NFP_PCIE_CSR_PHYCFG5           0x0024
#define NFP_PCIE_CSR_PHYCFG6           0x0028
#define NFP_PCIE_CSR_PHYCFG7           0x002c
#define NFP_PCIE_CSR_PHYCFG8           0x0030
#define NFP_PCIE_CSR_PHYCFG9           0x0034
#define NFP_PCIE_CSR_PHYCFG10          0x0038
#define NFP_PCIE_CSR_PHYCFG11          0x003c
#define NFP_PCIE_CSR_PHYCFG12          0x0040
#define NFP_PCIE_CSR_PHYCFG13          0x0044
#define NFP_PCIE_CSR_PHYCFG14          0x0048
#define   NFP_PCIE_CSR_PHYCFG_DATA(_x)                  (_x)
#define   NFP_PCIE_CSR_PHYCFG_DATA_of(_x)               (_x)
/* Register Type: PCIEPHYTest */
#define NFP_PCIE_CSR_PHYTST0           0x004c
#define NFP_PCIE_CSR_PHYTST1           0x0050
#define NFP_PCIE_CSR_PHYTST2           0x0054
#define NFP_PCIE_CSR_PHYTST3           0x0058
#define NFP_PCIE_CSR_PHYTST4           0x005c
#define NFP_PCIE_CSR_PHYTST5           0x0060
#define NFP_PCIE_CSR_PHYTST6           0x0064
#define NFP_PCIE_CSR_PHYTST7           0x0068
#define NFP_PCIE_CSR_PHYTST8           0x006c
#define NFP_PCIE_CSR_PHYTST9           0x0070
#define   NFP_PCIE_CSR_PHYTST_DATA(_x)                  (_x)
#define   NFP_PCIE_CSR_PHYTST_DATA_of(_x)               (_x)


/* Generic PCIe BAR CPP mappings. */
#define NFP_PCIE_P2C_CPPMAP_TARGET_OFFSET(x)    ((x) << 20)
#define NFP_PCIE_P2C_CPPMAP_TOKEN_OFFSET(x) ((x) << 18)
#define NFP_PCIE_P2C_CPPMAP_SIZE        (1 << 18)

/* DMA Queue Index Values */
#define NFP_PCIE_DMA_TPCI_HI   0
#define NFP_PCIE_DMA_TPCI_LO   1
#define NFP_PCIE_DMA_FPCI_HI   2
#define NFP_PCIE_DMA_FPCI_LO   3

/* PCIe Interrupt Manager pins
 */
#define NFP_PCIE_IM_IRQ_A       3
#define NFP_PCIE_IM_IRQ_B       2
#define NFP_PCIE_IM_IRQ_C       1
#define NFP_PCIE_IM_IRQ_D       0

#define NFP_XPB_PCIE_CSR        NFP_XPB_DEST(17,1)
#define NFP_XPB_PCIE_MGMT       NFP_XPB_DEST(17,2)
#define NFP_XPB_PCIE_QUEUE_CTL      NFP_XPB_DEST(17,3)
#define NFP_XPB_PCIE_SHARED_CTL     NFP_XPB_DEST(17,4)
#define NFP_XPB_PCIE_IM         NFP_XPB_DEST(17,5)

#endif /* NFP3200_NFP_PCIE_H */
