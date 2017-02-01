/*
 * Copyright (C) 2008-2011, Netronome Systems, Inc.  All rights reserved.
 *
 * Disclaimer: this file is provided without any warranty
 * as part of an early-access program, and the content is
 * bound to change before the final release.
 */

#ifndef NFP3200_NFP_SHAC_H
#define NFP3200_NFP_SHAC_H

/* SHaC IM Base */
#define NFP_XPB_SHAC_IM		NFP_XPB_DEST(31, 5)

/* Interrupt manager IM pins.
 */
#define NFP_SHAC_IM_TSA_DYNAMIC	0
#define NFP_SHAC_IM_TSA_STATIC	1
#define NFP_SHAC_IM_XPB_WDOG	2	/* XPB bus timeout */
#define NFP_SHAC_IM_SPI_WDOG	3	/* SPI HOLD timeout */


#define NFP_SHAC_IM_SHAC_ERR	6	/* CPP SHAC error */
#define NFP_SHAC_IM_GPIO	7	/* GPIO trigger */
#define NFP_SHAC_IM_UART	8	/* UART IRQ */
#define NFP_SHAC_IM_TIMER_1	9	/* Timer 1 */
#define NFP_SHAC_IM_TIMER_2	10	/* Timer 2 */
#define NFP_SHAC_IM_TIMER_3	11	/* Timer 3 */
#define NFP_SHAC_IM_TIMER_4	12	/* Timer 4 */
#define NFP_SHAC_IM_SRAM0_ERR	13	/* QDR SRAM0 error */
#define NFP_SHAC_IM_SRAM1_ERR	14	/* QDR SRAM1 error */

#endif /* NFP3200_NFP_SHAC_H */
