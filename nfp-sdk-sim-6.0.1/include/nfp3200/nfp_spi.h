/*
 * Copyright (C) 2008-2011, Netronome Systems, Inc.  All rights reserved.
 *
 * Disclaimer: this file is provided without any warranty
 * as part of an early-access program, and the content is
 * bound to change before the final release.
 */

#ifndef NFP3200_NFP_SPI_H
#define NFP3200_NFP_SPI_H

/* HGID: nfp3200/spi.desc = ade0020e9035 */
/* Register Type: SerialConfigSpiA */
#define NFP_CAP_SPI_CTL0_DEV0_CFG0_SPI 0x0aa0
#define NFP_CAP_SPI_CTL0_DEV1_CFG0_SPI 0x0aa4
#define NFP_CAP_SPI_CTL0_DEV2_CFG0_SPI 0x0aa8
#define NFP_CAP_SPI_CTL0_DEV3_CFG0_SPI 0x0aac
#define NFP_CAP_SPI_CTL0_DEV0_CFG1_SPI 0x0ac0
#define NFP_CAP_SPI_CTL0_DEV1_CFG1_SPI 0x0ac4
#define NFP_CAP_SPI_CTL0_DEV2_CFG1_SPI 0x0ac8
#define NFP_CAP_SPI_CTL0_DEV3_CFG1_SPI 0x0acc
#define NFP_CAP_SPI_CTL0_DEV0_CFG2_SPI 0x0ae0
#define NFP_CAP_SPI_CTL0_DEV1_CFG2_SPI 0x0ae4
#define NFP_CAP_SPI_CTL0_DEV2_CFG2_SPI 0x0ae8
#define NFP_CAP_SPI_CTL0_DEV3_CFG2_SPI 0x0aec
#define   NFP_CAP_SPI_CTL0_SPI_DATA_LEN(_x)             (((_x) & 0x3f) << 26)
#define   NFP_CAP_SPI_CTL0_SPI_DATA_LEN_of(_x)          (((_x) >> 26) & 0x3f)
#define   NFP_CAP_SPI_CTL0_SPI_CMD_LEN(_x)              (((_x) & 0xf) << 22)
#define   NFP_CAP_SPI_CTL0_SPI_CMD_LEN_of(_x)           (((_x) >> 22) & 0xf)
#define   NFP_CAP_SPI_CTL0_SPI_ADDR_LEN(_x)             (((_x) & 0x1f) << 16)
#define   NFP_CAP_SPI_CTL0_SPI_ADDR_LEN_of(_x)          (((_x) >> 16) & 0x1f)
#define   NFP_CAP_SPI_CTL0_SPI_ALT_READ_CMD(_x)         (((_x) & 0xff) << 8)
#define   NFP_CAP_SPI_CTL0_SPI_ALT_READ_CMD_of(_x)      (((_x) >> 8) & 0xff)
#define   NFP_CAP_SPI_CTL0_SPI_ALT_WRITE_CMD(_x)        ((_x) & 0xff)
#define   NFP_CAP_SPI_CTL0_SPI_ALT_WRITE_CMD_of(_x)     ((_x) & 0xff)
/* Register Type: SerialConfigMdioA */
#define NFP_CAP_SPI_CTL0_DEV0_CFG0_MDIO 0x0aa0
#define NFP_CAP_SPI_CTL0_DEV1_CFG0_MDIO 0x0aa4
#define NFP_CAP_SPI_CTL0_DEV2_CFG0_MDIO 0x0aa8
#define NFP_CAP_SPI_CTL0_DEV3_CFG0_MDIO 0x0aac
#define NFP_CAP_SPI_CTL0_DEV0_CFG1_MDIO 0x0ac0
#define NFP_CAP_SPI_CTL0_DEV1_CFG1_MDIO 0x0ac4
#define NFP_CAP_SPI_CTL0_DEV2_CFG1_MDIO 0x0ac8
#define NFP_CAP_SPI_CTL0_DEV3_CFG1_MDIO 0x0acc
#define NFP_CAP_SPI_CTL0_DEV0_CFG2_MDIO 0x0ae0
#define NFP_CAP_SPI_CTL0_DEV1_CFG2_MDIO 0x0ae4
#define NFP_CAP_SPI_CTL0_DEV2_CFG2_MDIO 0x0ae8
#define NFP_CAP_SPI_CTL0_DEV3_CFG2_MDIO 0x0aec
#define   NFP_CAP_SPI_CTL0_MDIO_DATA_LEN(_x)            (((_x) & 0x3f) << 26)
#define   NFP_CAP_SPI_CTL0_MDIO_DATA_LEN_of(_x)         (((_x) >> 26) & 0x3f)
#define   NFP_CAP_SPI_CTL0_MDIO_TA_LEN(_x)              (((_x) & 0x3) << 24)
#define   NFP_CAP_SPI_CTL0_MDIO_TA_LEN_of(_x)           (((_x) >> 24) & 0x3)
#define   NFP_CAP_SPI_CTL0_MDIO_HDR_LEN(_x)             (((_x) & 0x1f) << 16)
#define   NFP_CAP_SPI_CTL0_MDIO_HDR_LEN_of(_x)          (((_x) >> 16) & 0x1f)
#define   NFP_CAP_SPI_CTL0_MDIO_HDR_HI(_x)              (((_x) & 0xff) << 8)
#define   NFP_CAP_SPI_CTL0_MDIO_HDR_HI_of(_x)           (((_x) >> 8) & 0xff)
#define   NFP_CAP_SPI_CTL0_MDIO_HDR_LO(_x)              ((_x) & 0xff)
#define   NFP_CAP_SPI_CTL0_MDIO_HDR_LO_of(_x)           ((_x) & 0xff)
/* Register Type: SerialConfigMdioB */
#define NFP_CAP_SPI_CTL1_DEV0_CFG0_MDIO 0x0ab0
#define NFP_CAP_SPI_CTL1_DEV1_CFG0_MDIO 0x0ab4
#define NFP_CAP_SPI_CTL1_DEV2_CFG0_MDIO 0x0ab8
#define NFP_CAP_SPI_CTL1_DEV3_CFG0_MDIO 0x0abc
#define NFP_CAP_SPI_CTL1_DEV0_CFG1_MDIO 0x0ad0
#define NFP_CAP_SPI_CTL1_DEV1_CFG1_MDIO 0x0ad4
#define NFP_CAP_SPI_CTL1_DEV2_CFG1_MDIO 0x0ad8
#define NFP_CAP_SPI_CTL1_DEV3_CFG1_MDIO 0x0adc
#define NFP_CAP_SPI_CTL1_DEV0_CFG2_MDIO 0x0af0
#define NFP_CAP_SPI_CTL1_DEV1_CFG2_MDIO 0x0af4
#define NFP_CAP_SPI_CTL1_DEV2_CFG2_MDIO 0x0af8
#define NFP_CAP_SPI_CTL1_DEV3_CFG2_MDIO 0x0afc
#define   NFP_CAP_SPI_CTL1_MDIO_SELECT                  (0x1 << 31)
#define     NFP_CAP_SPI_CTL1_MDIO_SELECT_SPI            (0x0)
#define     NFP_CAP_SPI_CTL1_MDIO_SELECT_MDIO           (0x80000000)
#define   NFP_CAP_SPI_CTL1_MDIO_PREAMBLE                (0x1 << 30)
#define     NFP_CAP_SPI_CTL1_MDIO_PREAMBLE_DISABLE      (0x0)
#define     NFP_CAP_SPI_CTL1_MDIO_PREAMBLE_ENABLE       (0x40000000)
#define   NFP_CAP_SPI_CTL1_MDIO_TA_BITS(_x)             (((_x) & 0x7) << 24)
#define   NFP_CAP_SPI_CTL1_MDIO_TA_BITS_of(_x)          (((_x) >> 24) & 0x7)
#define   NFP_CAP_SPI_CTL1_MDIO_BUSY                    (0x1 << 21)
#define     NFP_CAP_SPI_CTL1_MDIO_BUSY_NOT_BUSY         (0x0)
#define     NFP_CAP_SPI_CTL1_MDIO_BUSY_BUSY             (0x200000)
#define   NFP_CAP_SPI_CTL1_MDIO_HDR_23_16(_x)           (((_x) & 0xff) << 12)
#define   NFP_CAP_SPI_CTL1_MDIO_HDR_23_16_of(_x)        (((_x) >> 12) & 0xff)
#define   NFP_CAP_SPI_CTL1_MDIO_DATA_DRIVE_CTRL         (0x1 << 10)
#define     NFP_CAP_SPI_CTL1_MDIO_DATA_DRIVE_CTRL_OPEN_DRAIN (0x0)
#define     NFP_CAP_SPI_CTL1_MDIO_DATA_DRIVE_CTRL_PUSH_PULL (0x400)
#define   NFP_CAP_SPI_CTL1_MDIO_DEV_PCLK_DIV(_x)        ((_x) & 0x3ff)
#define   NFP_CAP_SPI_CTL1_MDIO_DEV_PCLK_DIV_of(_x)     ((_x) & 0x3ff)
/* Register Type: SerialConfigSpiB */
#define NFP_CAP_SPI_CTL1_DEV0_CFG0_SPI 0x0ab0
#define NFP_CAP_SPI_CTL1_DEV1_CFG0_SPI 0x0ab4
#define NFP_CAP_SPI_CTL1_DEV2_CFG0_SPI 0x0ab8
#define NFP_CAP_SPI_CTL1_DEV3_CFG0_SPI 0x0abc
#define NFP_CAP_SPI_CTL1_DEV0_CFG1_SPI 0x0ad0
#define NFP_CAP_SPI_CTL1_DEV1_CFG1_SPI 0x0ad4
#define NFP_CAP_SPI_CTL1_DEV2_CFG1_SPI 0x0ad8
#define NFP_CAP_SPI_CTL1_DEV3_CFG1_SPI 0x0adc
#define NFP_CAP_SPI_CTL1_DEV0_CFG2_SPI 0x0af0
#define NFP_CAP_SPI_CTL1_DEV1_CFG2_SPI 0x0af4
#define NFP_CAP_SPI_CTL1_DEV2_CFG2_SPI 0x0af8
#define NFP_CAP_SPI_CTL1_DEV3_CFG2_SPI 0x0afc
#define   NFP_CAP_SPI_CTL1_SPI_SELECT                   (0x1 << 31)
#define     NFP_CAP_SPI_CTL1_SPI_SELECT_SPI             (0x0)
#define     NFP_CAP_SPI_CTL1_SPI_SELECT_MDIO            (0x80000000)
#define   NFP_CAP_SPI_CTL1_SPI_ALT_READ                 (0x1 << 30)
#define     NFP_CAP_SPI_CTL1_SPI_ALT_READ_DISABLED      (0x0)
#define     NFP_CAP_SPI_CTL1_SPI_ALT_READ_ENABLED       (0x40000000)
#define   NFP_CAP_SPI_CTL1_SPI_ALT_WRITE                (0x1 << 29)
#define     NFP_CAP_SPI_CTL1_SPI_ALT_WRITE_DISABLED     (0x0)
#define     NFP_CAP_SPI_CTL1_SPI_ALT_WRITE_ENABLED      (0x20000000)
#define   NFP_CAP_SPI_CTL1_SPI_ENABLE_DELAY(_x)         (((_x) & 0x1f) << 24)
#define   NFP_CAP_SPI_CTL1_SPI_ENABLE_DELAY_of(_x)      (((_x) >> 24) & 0x1f)
#define   NFP_CAP_SPI_CTL1_SPI_ADDR_TRANS_BITS(_x)      (((_x) & 0x3) << 22)
#define   NFP_CAP_SPI_CTL1_SPI_ADDR_TRANS_BITS_of(_x)   (((_x) >> 22) & 0x3)
#define   NFP_CAP_SPI_CTL1_SPI_BUSY                     (0x1 << 21)
#define     NFP_CAP_SPI_CTL1_SPI_BUSY_NOT_BUSY          (0x0)
#define     NFP_CAP_SPI_CTL1_SPI_BUSY_BUSY              (0x200000)
#define   NFP_CAP_SPI_CTL1_SPI_SERIAL_BOOT_PROM_WRITE   (0x1 << 20)
#define     NFP_CAP_SPI_CTL1_SPI_SERIAL_BOOT_PROM_WRITE_DISCARD (0x0)
#define     NFP_CAP_SPI_CTL1_SPI_SERIAL_BOOT_PROM_WRITE_ENABLE (0x100000)
#define   NFP_CAP_SPI_CTL1_SPI_MODE_SELECT(_x)          (((_x) & 0x3) << 18)
#define   NFP_CAP_SPI_CTL1_SPI_MODE_SELECT_of(_x)       (((_x) >> 18) & 0x3)
#define     NFP_CAP_SPI_CTL1_SPI_MODE_SELECT_FIRST_EDGE_CLK0 (0)
#define     NFP_CAP_SPI_CTL1_SPI_MODE_SELECT_SECOND_EDGE_CLK0 (1)
#define     NFP_CAP_SPI_CTL1_SPI_MODE_SELECT_FIRST_EDGE_CLK1 (2)
#define     NFP_CAP_SPI_CTL1_SPI_MODE_SELECT_SECOND_EDGE_CLK1 (3)
#define   NFP_CAP_SPI_CTL1_SPI_READ_MODE(_x)            (((_x) & 0x3) << 16)
#define   NFP_CAP_SPI_CTL1_SPI_READ_MODE_of(_x)         (((_x) >> 16) & 0x3)
#define     NFP_CAP_SPI_CTL1_SPI_READ_MODE_SLOW         (0)
#define     NFP_CAP_SPI_CTL1_SPI_READ_MODE_FAST_SINGLE  (1)
#define     NFP_CAP_SPI_CTL1_SPI_READ_MODE_FAST_DUAL    (3)
#define   NFP_CAP_SPI_CTL1_SPI_HOLD                     (0x1 << 15)
#define     NFP_CAP_SPI_CTL1_SPI_HOLD_IGNORE            (0x0)
#define     NFP_CAP_SPI_CTL1_SPI_HOLD_ALLOW             (0x8000)
#define   NFP_CAP_SPI_CTL1_SPI_HOLD_TIMEOUT             (0x1 << 14)
#define     NFP_CAP_SPI_CTL1_SPI_HOLD_TIMEOUT_USE       (0x0)
#define     NFP_CAP_SPI_CTL1_SPI_HOLD_TIMEOUT_IGNORE    (0x4000)
#define   NFP_CAP_SPI_CTL1_SPI_DUMMY_LEN(_x)            (((_x) & 0x3f) << 8)
#define   NFP_CAP_SPI_CTL1_SPI_DUMMY_LEN_of(_x)         (((_x) >> 8) & 0x3f)
#define   NFP_CAP_SPI_CTL1_SPI_PCLK_DIV(_x)             ((_x) & 0xff)
#define   NFP_CAP_SPI_CTL1_SPI_PCLK_DIV_of(_x)          ((_x) & 0xff)

#define NFP_XPB_SPI_TARGET(x)		NFP_XPB_DEST(31, 8 + ((x) & 3))
#define NFP_XPB_SPI_CFG0(x)		NFP_XPB_DEST(31, 12 + ((x) & 3))
#define NFP_XPB_SPI_CFG1(x)		NFP_XPB_DEST(31, 28 + ((x) & 3))
#define NFP_XPB_SPI_CFG2(x)		NFP_XPB_DEST(31, 44 + ((x) & 3))

#endif /* NFP3200_NFP_SPI_H */
