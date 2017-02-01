/*
 * Copyright (C) 2013, Netronome Systems, Inc.  All rights reserved.
 *
 * Disclaimer: this file is provided without any warranty
 * as part of an early-access program, and the content is
 * bound to change before the final release.
 */

#ifndef __SIM_EVENT_NAMES_
#define __SIM_EVENT_NAMES_

#ifndef NULL
#define NULL 0
#endif

struct SIMEVENT_NAMES {
    const char *name;
    int type;
};

struct SIMEVENT_NAMES all_simevent_names[] = {
    {"SIMEVENT_SIM_SHUTDOWN", 0x0},
    {"SIMEVENT_SIM_RESTART", 0x1},
    {"SIMEVENT_RUN_START", 0x2},
    {"SIMEVENT_RUN_UPDATE", 0x3},
    {"SIMEVENT_RUN_STOP", 0x4},
    {"SIMEVENT_RUN_RESET", 0x5},
    {"SIMEVENT_RUN_RESETDONE", 0x6},
    {"SIMEVENT_WATCH", 0x7},
    {"SIMEVENT_PKT_INSENT", 0x8},
    {"SIMEVENT_PKT_OUTRECV", 0x9},
    {"SIMEVENT_PKT_INLEVEL", 0xa},
    {"SIMEVENT_PKT_OUTLEVEL", 0xb},
    {"SIMEVENT_ILKN_INSENT", 0xc},
    {"SIMEVENT_ILKN_OUTRECV", 0xd},
    {"SIMEVENT_ILKN_INLEVEL", 0xe},
    {"SIMEVENT_ILKN_OUTLEVEL", 0xf},
    {"SIMEVENT_USER", 0x10},
    {"SIMEVENT_USER_RUN_UPDATE", 0x11},
    {"SIMEVENT_LAST", 0x12},
    {"SIMEVENT_RUN_STOP_FINISHED", 0x20},
    {"SIMEVENT_RUN_STOP_USER", 0x21},
    {"SIMEVENT_RUN_STOP_BREAK", 0x22},
    {"SIMEVENT_RUN_STOP_RESET", 0x23},
    {"SIMEVENT_RUN_STOP_PACKET", 0x24},
    {"SIMEVENT_WATCH_CLASS_MEM", 0x40},
    {"SIMEVENT_WATCH_CLASS_REG", 0x41},
    {"SIMEVENT_WATCH_CLASS_CTRK", 0x42},
    {"SIMEVENT_WATCH_CLASS_PC", 0x43},
    {NULL, 0}
};
#endif
