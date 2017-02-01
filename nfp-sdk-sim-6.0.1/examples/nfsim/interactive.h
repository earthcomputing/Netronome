/**
 * Copyright (C) 2014 Netronome Systems, Inc.  All rights reserved.
 */
#ifndef __INTERACTIVE_H__
#define __INTERACTIVE_H__

/* run control mode: either sim or hwdbg */
#define MODE_SIM              0
#define MODE_HWDBG            1
#define MODE_ANY              0xff

#define INST_STEP_BREAKID 0xdeadbeef

struct nfp_hwdbg_break_data;
struct command_data;

void interactive_shutdown(void);

struct interactive_data {
    /* command line options */
    int opt_nfp_dev;
    int opt_nfp_hwdbg_dev;
    int opt_forcenosim;
    const char *opt_scriptfile;

    /* mode - either sim or hwdbg */
    int mode;
    int verbose;
    int issim; /* is the server a simulator */

    /* handles */
    struct nfp_device *h_nfp;
    struct nfp_cpp *h_nfp_cpp;
    struct nfp_hwdbg *h_hwdbg;

    /* breakpoint stuff */
    int bp_inst_step_loaded; /* stepping sim requires break */
    int bp_inst_step_busy;

    /* event stuff */
    int evt_display; /* display incoming events */
    int evt_old_clk_space; /* track clock diffs */
    int evt_new_clk_space;
    struct timeval evt_new_time; /* track time between events */
    struct timeval evt_old_time;

    /* hwdbg stuff */
    int64_t hwdbg_breakid;
    int hwdbg_autoclear; /* clear hwdbg breaks automatically */
    int hwdbg_autorun; /* run after autoclear of hwdbg breaks automatically */

    /* readline stuff */
    const char *readline_historyfile;
    char readline_buffer[1024];
    FILE *scriptfile;

    /* command stuff */
    int cmd_meid;
    uint64_t cmd_watchid;
    int cmd_stepctx;

    /* commands */
    struct command_data **cmds;
    int cmds_cnt;

    /* shutdown */
    int shutdown;

    /* connection stuff */
    long int latency_usec; /* Command latency (request/response) to SIM/HW */
};

struct command_data {
    const char *cmdstr;
    int (*cmdfunc) (struct interactive_data *d, const char *argstr);
    int mode;
    const char *help;
};

extern struct interactive_data *interactive_glob;

#endif /* __INTERACTIVE_H__ */

