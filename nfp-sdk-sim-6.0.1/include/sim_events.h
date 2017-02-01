/** @file sim_events.h
 * Copyright (C) 2012 Netronome Systems, Inc.  All rights reserved.
 *
 * NFP-6XXX Simulator events header:
 * Defines event numbers and event parameter constants used by IPC
 * servers and clients
 *
 */

#ifndef __SIM_EVENTS_H__
#define __SIM_EVENTS_H__

#include <stdint.h>

enum SIMEVENT {
    SIMEVENT_SIM_SHUTDOWN    = 0,
    SIMEVENT_SIM_RESTART     = 1,
    SIMEVENT_RUN_START       = 2,
    SIMEVENT_RUN_UPDATE      = 3,
    SIMEVENT_RUN_STOP        = 4,
    SIMEVENT_RUN_RESET       = 5,
    SIMEVENT_RUN_RESETDONE   = 6,
    SIMEVENT_WATCH           = 7,
    SIMEVENT_PKT_INSENT      = 8,
    SIMEVENT_PKT_OUTRECV     = 9,
    SIMEVENT_PKT_INLEVEL     = 10,
    SIMEVENT_PKT_OUTLEVEL    = 11,
    SIMEVENT_ILKN_INSENT     = 12,
    SIMEVENT_ILKN_OUTRECV    = 13,
    SIMEVENT_ILKN_INLEVEL    = 14,
    SIMEVENT_ILKN_OUTLEVEL   = 15,
    SIMEVENT_USER            = 16,
    SIMEVENT_USER_RUN_UPDATE = 17,
    SIMEVENT_LAST,
};

#ifdef _WIN32
    #define __PACKED__
    #pragma pack(push, 1)
#else
    #define __PACKED__ __attribute__ ((__packed__))
#endif
struct simevent_sim_shutdown {
    uint64_t me_clock_count;
    uint32_t stop_reason;
} __PACKED__;

struct simevent_sim_restart {
    uint64_t dummy; /* FIXME: delete me at some point */
} __PACKED__;

struct simevent_run_start {
    uint64_t me_clock_count;
    uint64_t me_cycles_remaining;
} __PACKED__;

struct simevent_run_update {
    uint64_t me_clock_count;
    uint64_t me_cycles_remaining;
} __PACKED__;

/* enums for stop_reason */
enum SIMEVENT_RUN {
    SIMEVENT_RUN_STOP_FINISHED  = 32,
    SIMEVENT_RUN_STOP_USER      = 33,
    SIMEVENT_RUN_STOP_BREAK     = 34,
    SIMEVENT_RUN_STOP_RESET     = 35,
    SIMEVENT_RUN_STOP_PACKET    = 36,
};

struct simevent_run_stop {
    uint64_t me_clock_count;
    uint32_t stop_reason;
} __PACKED__;

struct simevent_run_reset {
    uint64_t me_clock_count;
} __PACKED__;

struct simevent_run_resetdone {
    uint64_t me_clock_count;
} __PACKED__;

struct simevent_user {
    uint64_t type;
    uint64_t subtype;
} __PACKED__;

/* Sub event types for SIMEVENT_WATCH */
enum SIMEVENT_WATCH_CLASS {
    SIMEVENT_WATCH_CLASS_MEM    = 64,
    SIMEVENT_WATCH_CLASS_REG    = 65,
    SIMEVENT_WATCH_CLASS_CTRK   = 66,
    SIMEVENT_WATCH_CLASS_PC     = 67,
};

/* payload format for SIMEVENT_WATCH_CLASS_CTRK */
struct simevent_watch_ctrk_payload {
    uint16_t level;
    uint16_t credits;
    uint16_t hwlimit;
    uint16_t swlimit;
    uint16_t throttle;
    uint16_t send;
    uint16_t taken;
} __PACKED__ ;

struct simevent_watch {
    uint64_t watchid;           /* the watchid assigned by the user */
    uint16_t watchtype;         /* the watchtype assigned by the user */
    uint64_t me_clock_count;    /* me cycle when event occurred */
    uint8_t  watchclass;        /* the class of watch MEM/REG/CTRK etc. */
    /* three 64-bit words with various meanings depending on watchclass */
    union {
        uint64_t generic_arg0;
        uint64_t reg_value;     /* the new value of the register that changed */
        uint64_t mem_addr;      /* the memory write address */
        uint64_t pc_addr;       /* the pc watch address */
    };
    union {
        uint64_t generic_arg1;
        uint64_t ctrk_level;    /* the current ctracker level */
        uint64_t pc_ctx;        /* the pc watch context */
        uint64_t reg_offset;    /* applicable when an offset is used eg lmem */
    };
    union {
        uint64_t generic_arg2;
        uint64_t pc_id;         /* the me id */
        uint64_t ctrk_id;       /* the ctracker id */
        uint64_t reg_oldvalue;  /* the previous value for register breaks */
    };
    int payload_len;
    /* payload follows if present */
} __PACKED__;

struct simevent_pkt_insent {
    uint64_t me_clock_count;
    uint64_t level;
} __PACKED__;

struct simevent_pkt_outrecv {
    uint64_t me_clock_count;
    uint64_t level;
} __PACKED__;

struct simevent_pkt_inlevel {
    uint64_t me_clock_count;
    uint64_t level;
} __PACKED__;

struct simevent_pkt_outlevel {
    uint64_t me_clock_count;
    uint64_t level;
} __PACKED__;

struct simevent_ilkn_insent {
    uint64_t me_clock_count;
    uint64_t level;
} __PACKED__;

struct simevent_ilkn_outrecv {
    uint64_t me_clock_count;
    uint64_t level;
} __PACKED__;

struct simevent_ilkn_inlevel {
    uint64_t me_clock_count;
    uint64_t level;
} __PACKED__;

struct simevent_ilkn_outlevel {
    uint64_t me_clock_count;
    uint64_t level;
} __PACKED__;

struct simevent_user_run_update {
    uint64_t me_clock_count;
} __PACKED__;

#ifdef _WIN32
    #pragma pack(pop)
#endif

#define MAX_SUBS_PER_EVENT_TYPE 8

#endif /* __SIM_EVENTS_H__ */
