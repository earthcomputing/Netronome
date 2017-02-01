/**
 * Copyright (C) 2014 Netronome Systems, Inc.  All rights reserved.
 *
 * Example to watch All ME ATTN signals using the NFP APIs
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <inttypes.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include <nfp-common/nfp_chipdata.h>
#include <nfp-common/nfp_resid.h>
#include <nfp-common/ns_log.h>
#include <nfp.h>
#include <nfp_cpp.h>
#include <nfp_sal.h>

#include <simevent_names.h>
#include "read_mecsr.h"

/***** Defines *****/

/* Masks used for watch events */
#define WATCHID_MASK_MEATTN                     (1<<16)
#define WATCHID_MASK_ACTCTXSTATUS               (1<<17)
#define WATCHID_MASK_PCBREAKPOINTSTATUS         (1<<18)
#define WATCHID_MASK_MAILBOX0                   (1<<19)
#define WATCHID_MASK_MAILBOX1                   (1<<20)
#define WATCHID_MASK_MAILBOX2                   (1<<21)
#define WATCHID_MASK_MAILBOX3                   (1<<22)

struct meattn_info {
    uint64_t watchid;
    uint32_t meid;
    uint32_t mask;
    uint64_t me_cycle_count0;
    uint64_t me_cycle_count1;
    uint32_t ctx;
    uint32_t pc;
    uint32_t pcb_status;
    uint32_t mailbox0;
    uint32_t mailbox1;
    uint32_t mailbox2;
    uint32_t mailbox3;
};

struct globalargs_data {
    int verbose;                /* -v verbose */
    const char *meid_str;       /* -m <MEID name> */
    int nfp_debug_level;        /* -d <DEBUG LEVEL> */
    int devnum;                 /* -n <NFP Device number> */
    int steps;                  /* -s <steps> */
    int sim_break;              /* -b break (stop) sim */
};

/***** Globals *****/

struct meattn_info *g_all_meattn_info = NULL;
int g_mecnt;

/* Define command line arguments */
static const char *g_optstring = "n:d:s:bvh?";

struct globalargs_data globalargs;

/* Display program usage, and exit. */
void usage(const char *program, int err)
{
    fprintf(stderr,
            "Usage: %s [options]    Watch All ME ATTN signals using the island Cluster Scratch Interrupt Status\n"
            "\nWhere options are:\n"
            "  -n <dev no>          Device number (default: 0)\n"
            "  -d <debug level>     Debug Level\n"
            "  -s <steps>           Number of steps to simulate (optional)\n"
            "  -b                   Break (Stop) simulator on any ME ATTN change\n"
            "  -v                   Verbose output\n"
            "  -h, -?               Help\n\n"
            "\n"
            "output:\n"
            "<meid> 1:<me cycle count>  0:<me cycle count>\n"
            "...\n\n"
            "where 1: is the cycle count when ME ATTN asserted\n"
            "      0: is the cycle count when ME ATTN de-asserted\n",
            program);

    exit(err ? EXIT_FAILURE : EXIT_SUCCESS);
}

/* Based on the watchid, get the index of the g_all_meattn_info[] */
int get_watch_index(uint64_t watchid)
{
    int i;

    i = 0;
    while (i < g_mecnt) {
        if (g_all_meattn_info[i].watchid == watchid)
            return i;
        i += 1;
    }
    return -1;
}

void ievent_handler(struct nfp_device *dev,
                    uint32_t ev_type,
                    void *ev_data,
                    uint32_t ev_data_len)
{
    const char *meid_str_ptr;
    char meid_str[NFP_MEID_STR_SZ];
    struct meattn_info *meinfo;
    struct simevent_watch *evt_watch = ev_data;
    struct simevent_sim_shutdown *evt_shutdown = ev_data;
    uint64_t me_clock_count;
    uint64_t watch_id = -1;
    uint64_t value = -1;
    int meid;
    int index;

    /* Get the ME clock count with certain events */
    switch (ev_type) {
    case SIMEVENT_WATCH:
        me_clock_count = evt_watch->me_clock_count;
        watch_id = evt_watch->watchid;
        value = evt_watch->generic_arg0;

        index = get_watch_index(watch_id & 0xffff);
        if (index < 0) {
            fprintf(stderr, "failed to get ME watch index\n");
            exit(EXIT_FAILURE);
        }

        /* set pointer to current me info */
        meinfo = &g_all_meattn_info[index];

        if (watch_id & WATCHID_MASK_ACTCTXSTATUS) {
            if (value & (uint64_t) (1<<31)) {
                meinfo->ctx = (uint32_t) ((uint64_t) value & 0x7);
                meinfo->pc = (uint32_t) ((uint64_t) (value >> 8) & 0x1ffff);
            }
        }

        if (watch_id & WATCHID_MASK_MAILBOX0)
            g_all_meattn_info[index].mailbox0 = value;

        if (watch_id & WATCHID_MASK_MAILBOX1)
            g_all_meattn_info[index].mailbox1 = value;

        if (watch_id & WATCHID_MASK_MAILBOX2)
            g_all_meattn_info[index].mailbox2 = value;

        if (watch_id & WATCHID_MASK_MAILBOX3)
            g_all_meattn_info[index].mailbox3 = value;

        if (watch_id & WATCHID_MASK_MEATTN) {
            if ((value & meinfo->mask) == meinfo->mask) {
                meinfo->me_cycle_count1 = me_clock_count;
            } else {
                if (meinfo->me_cycle_count1 != 0) {
                    meinfo->me_cycle_count0 = me_clock_count;
                    meinfo->ctx = 0;
                    meinfo->pc = 0;
                }
            }
            meid = meinfo->meid;
            /* Get the alt name for the MEID */
            meid_str_ptr = nfp6000_meid2str(meid_str, meid);
            if (meid_str_ptr == NULL) {
                sprintf(meid_str, "%04x", meid);
            }

            if (g_all_meattn_info[index].pc == 0) {
                printf("%s  1:%lu  0:%lu\n",
                       meid_str,
                       meinfo->me_cycle_count1,
                       meinfo->me_cycle_count0);

            } else {
                printf("%s  1:%lu  0:%lu  ctx:%d  pc:%d?  "
                       "mb0:0x%08x  mb1:0x%08x  mb2:0x%08x  mb3:0x%08x\n",
                       meid_str,
                       meinfo->me_cycle_count1,
                       meinfo->me_cycle_count0,
                       meinfo->ctx,
                       meinfo->pc,
                       meinfo->mailbox0,
                       meinfo->mailbox1,
                       meinfo->mailbox2,
                       meinfo->mailbox3);
           }
        }
        break;
    case SIMEVENT_SIM_SHUTDOWN:
        me_clock_count = evt_shutdown->me_clock_count;
        printf("Got sim shutdown event @ %lu cycles, closing application\n",
               me_clock_count);
        kill(getpid(), SIGINT);
        break;
    }
    fflush(stdout);
}

int main(int argc, char **argv)
{
    struct nfp_device *nfp;
    const struct nfp_chipdata_chip *chip;
    const char *program = argv[0];
    int meid;
    int opt = 0;
    int devnum;
    int watchtype;
    uint64_t watchid;
    int retval;
    int n;
    int i;

    /* Initialise globalargs to defaults */
    globalargs.devnum = 0;
    globalargs.nfp_debug_level = NS_LOG_LVL_INFO;
    globalargs.verbose = 0;
    globalargs.steps = -1;
    globalargs.sim_break = 0;

    /* Process the arguments with getopt(), then populate globalargs */
    opt = getopt( argc, argv, g_optstring );

    while( opt != -1 ) {
        switch( opt ) {
            case 'n':
                globalargs.devnum = atoi(optarg);
                break;
            case 'd':
                globalargs.nfp_debug_level = atoi(optarg);
                break;
            case 's':
                globalargs.steps = atoi(optarg);
                break;
            case 'b':
                globalargs.sim_break = 1;
                break;
            case 'v':
                globalargs.verbose++;
                break;
            case 'h':
            case '?':
                usage(program, 0);
                break;
            default:
                break;
        }
        opt = getopt( argc, argv, g_optstring );
    }

    devnum = globalargs.devnum;

    /* Init and Set NS log level */
    ns_log_init(NS_LOG_FLAG_STRIPFILE);
    NS_LOG_SET_LEVEL(NS_LOG_LVL_INFO);

    /* Open NFP Device */
    if (globalargs.verbose) {
        printf("opening nfp_device (%d)\n", devnum);
    }
    nfp = nfp_device_open(devnum);
    if (!nfp) {
        fprintf(stderr,
                "%s: ERROR: nfp_device_open (%d) failed\n", program, devnum);
        exit(EXIT_FAILURE);
    }

    /* get chipdata for the connected nfp so we know how many MEs there are */
    chip = nfp_device_chip(nfp);

    g_mecnt = nfp_chipdata_me_count(chip);
    if (g_mecnt == 0) {
        fprintf(stderr, "chip has no MEs\n");
        exit(EXIT_FAILURE);
    }

    g_all_meattn_info = malloc(sizeof(g_all_meattn_info[0]) * g_mecnt);
    if (!g_all_meattn_info) {
        fprintf(stderr, "failed to allocate ME data\n");
        exit(EXIT_FAILURE);
    }

    /* Setup watchs on all ME ATTN signals */
    n = 0;
    for (meid = nfp_chipdata_meid_first(chip);
                meid != -1; nfp_chipdata_meid_next(chip, meid)) {
        if (n >= g_mecnt)
            break;

        /* Initialise the ME ATTN struct */
        g_all_meattn_info[n].watchid = meid;
        g_all_meattn_info[n].meid = meid;
        g_all_meattn_info[n].mask = 1 << (NFP6000_MEID_MENUM_of(meid) + 16);
        g_all_meattn_info[n].me_cycle_count0 = 0;
        g_all_meattn_info[n].me_cycle_count1 = 0;
        g_all_meattn_info[n].ctx = 0;
        g_all_meattn_info[n].pc = 0;
        g_all_meattn_info[n].mailbox0 = 0xdeadbeef;
        g_all_meattn_info[n].mailbox1 = 0xdeadbeef;
        g_all_meattn_info[n].mailbox2 = 0xdeadbeef;
        g_all_meattn_info[n].mailbox3 = 0xdeadbeef;
        n++;
    }

    if (n != g_mecnt) {
        fprintf(stderr, "chipdata reported conflicting ME count\n");
        exit(EXIT_FAILURE);
    }

    if (globalargs.verbose) {
        printf("Setup %d watches on ME ATTNs\n", g_mecnt);
    }

    if (globalargs.sim_break == 1) {
        watchtype = NFP_SAL_WATCH_WTYPE(NFP_SAL_WATCH_ACTION_BREAK,
                                        NFP_SAL_WATCH_TYPE_CHANGE);
    } else {
        watchtype = NFP_SAL_WATCH_WTYPE(NFP_SAL_WATCH_ACTION_NOTIFY,
                                        NFP_SAL_WATCH_TYPE_CHANGE);
    }

    /* Setup Watch Events for all ME ATTNs */
    for (i = 0; i < g_mecnt; i++) {
        meid = g_all_meattn_info[i].meid;
        watchid = g_all_meattn_info[i].watchid;

        retval = nfp_sal_watch_add_xpb(nfp, watchtype,
                                       watchid | WATCHID_MASK_MEATTN,
                                       g_all_meattn_info[i].mask, /* mask */
                                       0,                       /* value */
                                       (NFP6000_MEID_ISLAND_of(meid) << 24) | 0x000b0000);
        if (retval < 0) {
            fprintf(stderr, "failed to add watch (%s)", strerror(errno));
            exit(EXIT_FAILURE);
        }

        retval = nfp_sal_watch_add_mecsr(nfp,
                                         watchtype,
                                         watchid | WATCHID_MASK_ACTCTXSTATUS,
                                         0xffffffff,     /* mask */
                                         0,              /* value */
                                         meid,
                                         -1, /* ctx */
                                         NFP_ME_ACTCTXSTATUS);
        if (retval < 0) {
            fprintf(stderr, "failed to add watch (%s)", strerror(errno));
            exit(EXIT_FAILURE);
        }

        retval = nfp_sal_watch_add_mecsr(nfp,
                                         watchtype,
                                         watchid | WATCHID_MASK_PCBREAKPOINTSTATUS,
                                         0xffffffff,     /* mask */
                                         0,              /* value */
                                         meid,
                                         -1, /* ctx */
                                         NFP_ME_PCBREAKPOINTSTATUS);
        if (retval < 0) {
            fprintf(stderr, "failed to add watch (%s)", strerror(errno));
            exit(EXIT_FAILURE);
        }

        retval = nfp_sal_watch_add_mecsr(nfp,
                                         watchtype,
                                         watchid | WATCHID_MASK_MAILBOX0,
                                         0xffffffff,     /* mask */
                                         0,              /* value */
                                         meid,
                                         -1, /* ctx */
                                         NFP_ME_MAILBOX0);
        if (retval < 0) {
            fprintf(stderr, "failed to add watch (%s)", strerror(errno));
            exit(EXIT_FAILURE);
        }

        retval = nfp_sal_watch_add_mecsr(nfp,
                                         watchtype,
                                         watchid | WATCHID_MASK_MAILBOX1,
                                         0xffffffff,     /* mask */
                                         0,              /* value */
                                         meid,
                                         -1, /* ctx */
                                         NFP_ME_MAILBOX1);
        if (retval < 0) {
            fprintf(stderr, "failed to add watch (%s)", strerror(errno));
            exit(EXIT_FAILURE);
        }

        retval = nfp_sal_watch_add_mecsr(nfp,
                                         watchtype,
                                         watchid | WATCHID_MASK_MAILBOX2,
                                         0xffffffff,     /* mask */
                                         0,              /* value */
                                         meid,
                                         -1, /* ctx */
                                         NFP_ME_MAILBOX2);
        if (retval < 0) {
            fprintf(stderr, "failed to add watch (%s)", strerror(errno));
            exit(EXIT_FAILURE);
        }

        retval = nfp_sal_watch_add_mecsr(nfp,
                                         watchtype,
                                         watchid | WATCHID_MASK_MAILBOX3,
                                         0xffffffff,     /* mask */
                                         0,              /* value */
                                         meid,
                                         -1, /* ctx */
                                         NFP_ME_MAILBOX3);
        if (retval < 0) {
            fprintf(stderr, "failed to add watch (%s)", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    /* Setup event handler */
    nfp_sal_subs_simevent_all(nfp, &ievent_handler);

    if (globalargs.steps != -1) {
        retval = nfp_sal_clock_step(nfp, globalargs.steps, 1);
        if (retval < 0) {
            fprintf(stderr,
                    "%s: ERROR: unable to step simulator (%d)\n",
                    program, devnum);
            exit(EXIT_FAILURE);
        }
    } else {
        /* Continious loop...Ctrl-C to break out */
        if (globalargs.verbose) {
            printf("Press Ctrl+C to break\n");
        }
        while (1) {
            sleep(1);
        }
    }

    /* Close/Free NFP handles */
    nfp_device_close(nfp);
    free(g_all_meattn_info);

    return 0;
}
