/*
 * Copyright (C) 2008-2011, Netronome Systems, Inc.  All rights reserved.
 *
 * Disclaimer: this file is provided without any warranty
 * as part of an early-access program, and the content is
 * bound to change before the final release.
 */

#ifndef NFP3200_NFP_THERM_H
#define NFP3200_NFP_THERM_H

/* HGID: nfp3200/therm.desc = ade0020e9035 */
/* Register Type: ThermalSensor */
#define NFP_THERM_DYNAMIC              0x0008
#define NFP_THERM_STATIC               0x000c
#define   NFP_THERM_TRIP_IND                            (0x1 << 11)
#define     NFP_THERM_TRIP_IND_XXX                      (0x0)
#define     NFP_THERM_TRIP_IND_ENABLE                   (0x800)
#define   NFP_THERM_EN                                  (0x1 << 10)
#define   NFP_THERM_DIN(_x)                             (((_x) & 0xff) << 2)
#define   NFP_THERM_DIN_of(_x)                          (((_x) >> 2) & 0xff)
#define   NFP_THERM_TEST_CONF(_x)                       ((_x) & 0x3)
#define   NFP_THERM_TEST_CONF_of(_x)                    ((_x) & 0x3)

#define NFP_XPB_THERM_START		NFP_XPB_DEST(31, 50)
#define NFP_THERM_SIZE			SZ_4K

#endif /* NFP3200_NFP_THERM_H */
