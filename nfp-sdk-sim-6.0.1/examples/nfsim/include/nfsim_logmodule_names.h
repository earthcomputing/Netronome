/*
 * Copyright (C) 2014, Netronome Systems, Inc.  All rights reserved.
 *
 * Disclaimer: this file is provided without any warranty
 * as part of an early-access program, and the content is
 * bound to change before the final release.
 */

#ifndef __NFSIM_LOG_MODULE_NAMES_
#define __NFSIM_LOG_MODULE_NAMES_

#ifndef NULL
#define NULL 0
#endif

struct NFSIM_LOGMODULE_NAMES {
    const char *name;
    int num;
};

struct NFSIM_LOGMODULE_NAMES all_nfsim_logmodule_names[] = {
    {"DEFAULT", 0x0},
    {"RUNCONTROL", 0x1},
    {"NFP", 0x2},
    {"NFP_CPP", 0x3},
    {"NFP_HWDBG", 0x4},
    {"NFP_SAL", 0x5},
    {"SIMBREAK", 0x6},
    {"HWDBG", 0x7},
    {"ACCESS_DRAM", 0x10},
    {NULL, 0}
};
#endif
