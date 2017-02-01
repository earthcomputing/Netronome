/*
 * Copyright (C) 2008-2011, Netronome Systems, Inc.  All rights reserved.
 *
 * Disclaimer: this file is provided without any warranty
 * as part of an early-access program, and the content is
 * bound to change before the final release.
 */

#ifndef NFP3200_NFP_TIMER_H
#define NFP3200_NFP_TIMER_H

#define NFP_TIMER_HZ		(50000000)	/* 50Mhz ticks */

#define NFP_XPB_TIMER		NFP_XPB_DEST(31,3)
#define NFP_TIMER_SIZE		SZ_4K

#define NFP_XPB_TIMER_1		(NFP_XPB_TIMER + 0x00)
#define NFP_XPB_TIMER_2		(NFP_XPB_TIMER + 0x04)
#define NFP_XPB_TIMER_3		(NFP_XPB_TIMER + 0x08)
#define NFP_XPB_TIMER_4		(NFP_XPB_TIMER + 0x0c)

#define NFP_TIMER_CTL		(0x00)
#define  NFP_TIMER_CTL_ACTIVATE		(1 << 7)	/* Activate timer */
#define  NFP_TIMER_CTL_PSS_CLK		(0 << 2)	/* Prescale to: clock */
#define  NFP_TIMER_CTL_PSS_CLK_DIV_16	(1 << 2)	/*    clock / 16      */
#define  NFP_TIMER_CTL_PSS_CLK_DIV_256	(2 << 2)	/*    clock / 256     */
#define  NFP_TIMER_CTL_PSS_GPIO		(3 << 2)	/*    40ns GPIO pulse */
#define NFP_TIMER_CLD		(0x10)			/* Initial value */
#define NFP_TIMER_CSR		(0x20)			/* Current value */
#define NFP_TIMER_CLR		(0x30)			/* Interrupt clear */
#define  NFP_TIMER_CLR_ICL		(1 << 0)	/* Clear interrupt */
#define NFP_TIMER_TWDE		(0x40)
#define  NFP_TIMER_TWDE_WDE		(1 << 0)	/* Enable watchdog on Timer 4 */

#endif /* NFP3200_NFP_TIMER_H */
