/*
 * Copyright (C) 2008-2011, Netronome Systems, Inc.  All rights reserved.
 *
 * Disclaimer: this file is provided without any warranty
 * as part of an early-access program, and the content is
 * bound to change before the final release.
 */

#ifndef NFP3200_NFP_UART_H
#define NFP3200_NFP_UART_H

/* HGID: nfp3200/uart.desc = ade0020e9035 */
/* Register Type: UartTxHolding */
#define NFP_UART_THR                   0x0000
#define   NFP_UART_THR_BYTE(_x)                         ((_x) & 0xff)
/* Register Type: UartRxBuffer */
#define NFP_UART_RBR                   0x0000
#define   NFP_UART_RBR_BYTE_of(_x)                      ((_x) & 0xff)
/* Register Type: UartDivisorLow */
#define NFP_UART_DLL                   0x0000
#define   NFP_UART_DLL_LB(_x)                           ((_x) & 0xff)
#define   NFP_UART_DLL_LB_of(_x)                        ((_x) & 0xff)
/* Register Type: UartInterruptEnable */
#define NFP_UART_IER                   0x0004
#define   NFP_UART_IER_UNIT                             (0x1 << 6)
#define   NFP_UART_IER_NRZ                              (0x1 << 5)
#define   NFP_UART_IER_RX_TIMEOUT                       (0x1 << 4)
#define   NFP_UART_IER_RLSI                             (0x1 << 2)
#define   NFP_UART_IER_THRI                             (0x1 << 1)
#define   NFP_UART_IER_RDI                              (0x1)
/* Register Type: UartDivisorHigh */
#define NFP_UART_DLM                   0x0004
#define   NFP_UART_DLM_HB(_x)                           ((_x) & 0xff)
#define   NFP_UART_DLM_HB_of(_x)                        ((_x) & 0xff)
/* Register Type: UartInterruptStatus */
#define NFP_UART_ISR                   0x0008
#define   NFP_UART_ISR_FIFO_MOD_EN_of(_x)               (((_x) >> 6) & 0x3)
#define     NFP_UART_ISR_FIFO_MOD_EN_NON_FIFO           (0)
#define     NFP_UART_ISR_FIFO_MOD_EN_FIFO               (3)
#define   NFP_UART_ISR_TIMEOUT                          (0x1 << 3)
#define   NFP_UART_ISR_SOURCE_of(_x)                    (((_x) >> 1) & 0x3)
#define     NFP_UART_ISR_SOURCE_NOT_USED                (0)
#define     NFP_UART_ISR_SOURCE_THRI                    (1)
#define     NFP_UART_ISR_SOURCE_RDI                     (2)
#define     NFP_UART_ISR_SOURCE_RLSI                    (3)
#define   NFP_UART_ISR_NO_INT                           (0x1)
/* Register Type: UartFifoControl */
#define NFP_UART_FCR                   0x0008
#define   NFP_UART_FCR_R_TRIGGER(_x)                    (((_x) & 0x3) << 6)
#define     NFP_UART_FCR_R_TRIGGER_1                    (0)
#define     NFP_UART_FCR_R_TRIGGER_8                    (1)
#define     NFP_UART_FCR_R_TRIGGER_16                   (2)
#define     NFP_UART_FCR_R_TRIGGER_32                   (3)
#define   NFP_UART_FCR_CLEAR_XMIT                       (0x1 << 2)
#define   NFP_UART_FCR_CLEAR_RCVR                       (0x1 << 1)
#define   NFP_UART_FCR_ENABLE_FIFO                      (0x1)
/* Register Type: UartLineControl */
#define NFP_UART_LCR                   0x000c
#define   NFP_UART_LCR_DLAB                             (0x1 << 7)
#define   NFP_UART_LCR_SBC                              (0x1 << 6)
#define   NFP_UART_LCR_SPAR                             (0x1 << 5)
#define   NFP_UART_LCR_EPAR                             (0x1 << 4)
#define   NFP_UART_LCR_PARITY                           (0x1 << 3)
#define   NFP_UART_LCR_STOP                             (0x1 << 2)
#define     NFP_UART_LCR_STOP_1                         (0x0)
#define     NFP_UART_LCR_STOP_2                         (0x4)
#define   NFP_UART_LCR_WLEN(_x)                         ((_x) & 0x3)
#define   NFP_UART_LCR_WLEN_of(_x)                      ((_x) & 0x3)
#define     NFP_UART_LCR_WLEN_5                         (0)
#define     NFP_UART_LCR_WLEN_6                         (1)
#define     NFP_UART_LCR_WLEN_7                         (2)
#define     NFP_UART_LCR_WLEN_8                         (3)
/* Register Type: UartLineStatus */
#define NFP_UART_LSR                   0x0014
#define   NFP_UART_LSR_FIFO_ERR                         (0x1 << 7)
#define   NFP_UART_LSR_TEMT                             (0x1 << 6)
#define   NFP_UART_LSR_THRE                             (0x1 << 5)
#define   NFP_UART_LSR_BI                               (0x1 << 4)
#define   NFP_UART_LSR_FE                               (0x1 << 3)
#define   NFP_UART_LSR_PE                               (0x1 << 2)
#define   NFP_UART_LSR_OE                               (0x1 << 1)
#define   NFP_UART_LSR_DR                               (0x1)

/* UART base */
#define NFP_XPB_UART			NFP_XPB_DEST(31, 2)	/* UART */
#define NFP_UART_SIZE			SZ_4K

#define NFP_UART_CLOCK			50000000


#endif /* NFP3200_NFP_UART_H */
