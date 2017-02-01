/**
 * Copyright (C) 2014 Netronome Systems, Inc.  All rights reserved.
 *
 * Example IPC client which uses the NFHAL/NFSAL APIs
 *
 * @file          intractv_hwdbg.c
 * @brief         Support functions for interactive client hw debug
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <inttypes.h>

#include <nfp-common/nfp_resid.h>
#include <nfp_hwdbg.h>

#include "interactive.h"
#include "intractv_hwdbg.h"

/*********************************************************************
 * Globals                                                           *
 *********************************************************************/

/* use one scratch for all calls, safe for single-threaded */
static uint64_t uint64args[128];

/*********************************************************************
 * Hardware debug events                                             *
 *********************************************************************/

void hwdbg_event_handler(struct nfp_hwdbg *nfp_hwdbg,
                         uint32_t ev_type,
                         void *ev_data,
                         uint32_t ev_data_len)
{
    struct interactive_data *d = interactive_glob;
    struct nfp_hwdbg_break_data *data;
    char meid_str[NFP_MEID_STR_SZ];
    const char *meid_str_ptr;

    if (ev_type == NFP_HWDBG_EVENT_TYPE_SHUTDOWN) {
        printf("server shutdown\n");
        interactive_shutdown();
        return;
    }

    if (ev_type == NFP_HWDBG_EVENT_TYPE_GROUPSTOP) {
        struct nfp_hwdbg_event_pl_groupstop *gs = ev_data;
        printf("groupstop me %x\n", gs->meid);
        return;
    }

    if (ev_type != NFP_HWDBG_EVENT_TYPE_BREAK) {
        fprintf(stderr, "got invalid hwdbg event type %d\n", ev_type);
        return;
    }
    data = (struct nfp_hwdbg_break_data *) ev_data;

    meid_str_ptr = nfp6000_meid2altstr(meid_str, data->meid);
    if (meid_str_ptr == NULL)
        sprintf(meid_str, "%04x", data->meid);

    if (data->breakid < 0) {
        printf("got event[NFP_HWDBG_EVENT_TYPE_BREAK] : "
               "Software Breakpoint (0x%lx) - %s PC:%d CTX:%d\n",
               data->breakid, meid_str, data->pc, data->ctx);
    } else {
        printf("got event[NFP_HWDBG_EVENT_TYPE_BREAK] : "
               "Code Breakpoint (0x%lx) - %s PC:%d CTX:%d\n",
               data->breakid, meid_str, data->pc, data->ctx);
    }

    /* Auto clear the break the ME */
    if (d->hwdbg_autoclear) {
        nfp_hwdbg_brk_clear(nfp_hwdbg, data->breakid);
        /* Auto run after clearing the break */
        if (d->hwdbg_autorun) {
            /* Delay before sending the run, to give the hwdbg server time to clear the breakpoint */
            usleep(d->latency_usec>>2);
            nfp_hwdbg_me_run(nfp_hwdbg, data->meid, 0);
        }
    }

    return;
}

/*********************************************************************
 * Support functions                                                 *
 *********************************************************************/

/* Convert a string to an array of uin64ts */
static int get_args_uint64(const char *str, uint64_t *out, int max)
{
    const char *s;
    char val_str[64];
    int val_off = 0;
    int got_val = 0;
    int n = 0;
    int done = 0;

    s = str;
    while (!done) {
        switch (*s) {
        case '\0':
        case '\r':
        case '\n':
            done = 1;
        case '\t':
        case ' ':
            if (got_val) {
                val_str[val_off] = '\0';
                val_off = 0;
                got_val = 0;
                out[n++] = strtoull(val_str, NULL, 0);
            }

            if (max == n) {
                done = 1;
            }
            break;
        default:
            if (val_off < 63) {
                val_str[val_off++] = *s;
                got_val = 1;
            }
            break;
        }
        s++;
    }
    return n;
}


/*********************************************************************
 * Exported Command functions                                        *
 *********************************************************************/

static int del_pcb(struct interactive_data *d, const char *argstr)
{
    int64_t bp;
    int cnt;

    cnt = get_args_uint64(argstr, uint64args, 128);
    if (cnt != 1) {
        fprintf(stderr, "invalid arguments\n");
        return -1;
    }

    bp = uint64args[0];

    if (nfp_hwdbg_brk_del(d->h_hwdbg, bp) < 0) {
        fprintf(stderr, "failed to del %ld breakpoint (%s)\n",
                bp, strerror(errno));
        return -1;
    }

    return 0;
}

static int add_pcb(struct interactive_data *d, const char *argstr)
{
    char meid_str[NFP_MEID_STR_SZ];
    const char *meid_str_ptr;
    int64_t breakid;
    int ctx = 0xff, pc;
    int cnt;
    int ret;

    cnt = get_args_uint64(argstr, uint64args, 128);
    if (cnt > 2 || cnt < 1) {
        fprintf(stderr, "invalid arguments\n");
        return -1;
    }

    if (cnt == 2)
        ctx = uint64args[1];
    pc = uint64args[0];

    breakid = d->hwdbg_breakid;

    meid_str_ptr = nfp6000_meid2altstr(meid_str, d->cmd_meid);
    if (meid_str_ptr == NULL) {
        sprintf(meid_str, "%04x", d->cmd_meid);
    }

    printf("adding break ID 0x%lx to %s pc:%d ctx mask:0x%x\n",
           breakid, meid_str, pc, ctx);

    ret = nfp_hwdbg_brk_add_code(d->h_hwdbg, breakid, d->cmd_meid, pc, ctx);
    if (ret < 0) {
        fprintf(stderr,
                "nfp_hwdbg_brk_add_code failed (%s)\n", strerror(errno));
        return -1;
    }

    d->hwdbg_breakid++;

    return 0;
}

static int reset_data_all(struct interactive_data *d, const char *argstr)
{
    printf("reseting all debug server data\n");
    nfp_hwdbg_srv_reset(d->h_hwdbg, NFP_HWDBG_SRV_RESET_TYPE_HARDRESET);
    return 0;
}

static int reset_data_nobp(struct interactive_data *d, const char *argstr)
{
    printf("reseting debug server data - keeping code breaks\n");
    nfp_hwdbg_srv_reset(d->h_hwdbg, NFP_HWDBG_SRV_RESET_TYPE_SOFTRESET);
    return 0;
}

static int toggle_autoclear(struct interactive_data *d, const char *argstr)
{
    if (d->hwdbg_autoclear)
        d->hwdbg_autoclear = 0;
    else
        d->hwdbg_autoclear = 1;
    printf("autoclear:%d\n", d->hwdbg_autoclear);
    return 0;
}

static int toggle_autorun(struct interactive_data *d, const char *argstr)
{
    if (d->hwdbg_autorun)
        d->hwdbg_autorun = 0;
    else
        d->hwdbg_autorun = 1;
    printf("autorun:%d\n", d->hwdbg_autorun);
    return 0;
}

static int me_status(struct interactive_data *d, const char *argstr)
{
    char meid_str[NFP_MEID_STR_SZ];
    const char *meid_str_ptr;
    struct nfp_hwdbg_me_status_data status;
    int meid = d->cmd_meid;
    int i;
    int stopped;

    meid_str_ptr = nfp6000_meid2altstr(meid_str, meid);
    if (meid_str_ptr == NULL)
        sprintf(meid_str, "%04x", meid);

    if (nfp_hwdbg_me_status(d->h_hwdbg, &meid, &status, 1) < 0) {
        fprintf(stderr,
                "failed to get me status (%s)\n", strerror(errno));
    } else {
        stopped = status.state != NFP_HWDBG_ME_STATUS_ACTIVE;
        printf("%s (0x%x) status - stopped:%d (0x%x)  PC:%d  CTX:%d\n",
                meid_str, meid, stopped, status.state, status.pc, status.ctx);
        printf("                   ctx_en:0x%x enables:0x%x\n",
               status.ctx_enables, status.enables);
        printf("                   ts_low:0x%08x  ts_high:0x%08x  profile_cnt:%d (0x%x)\n",
               status.timestamp_low, status.timestamp_high, status.profile_cnt, status.profile_cnt);
        for (i = 0; i < 8; i++)
            printf("                   ctxpc%d:0x%04x\n", i, status.ctxpc[i]);
    }
    return 0;
}

static int hwdbg_run(struct interactive_data *d, const char *argstr)
{
    if (nfp_hwdbg_me_run(d->h_hwdbg, d->cmd_meid, 0)) {
        fprintf(stderr, "running me%04x failed (%s)\n",
                d->cmd_meid, strerror(errno));
        return -1;
    }
    return 0;
}

static int hwdbg_stop(struct interactive_data *d, const char *argstr)
{
    if (nfp_hwdbg_me_stop(d->h_hwdbg, d->cmd_meid) < 0) {
        fprintf(stderr, "stopping me%04x failed (%s)\n",
                d->cmd_meid, strerror(errno));
        return -1;
    }
    return 0;
}

static int hwdbg_step(struct interactive_data *d, const char *argstr)
{
    struct nfp_hwdbg_me_status_data status;
    int meid = d->cmd_meid;
    int stopped;
    int ctx;

    if (strlen(argstr) == 0)
        ctx = -1;
    else {
        ctx = atoi(argstr);
        if (ctx >= 8) {
            fprintf(stderr, "invalid context for step\n");
            return -1;
        }
    }

    if (nfp_hwdbg_me_step(d->h_hwdbg, meid, ctx)) {
        fprintf(stderr, "stepping me:0x%04x failed (%s)\n",
                        d->cmd_meid, strerror(errno));
        return -1;
    }

    if (nfp_hwdbg_me_status(d->h_hwdbg, &meid, &status, 1) < 0) {
        fprintf(stderr,
                "failed to get me status (%s)\n", strerror(errno));
        return -1;
    } else {
        stopped = status.state != NFP_HWDBG_ME_STATUS_ACTIVE;
        printf("me:0x%04x status - stopped %d PC:%d CTX:%d\n",
                meid, stopped, status.pc, status.ctx);
    }
    return 0;
}

static int hwdbg_dump_clear(struct interactive_data *d, int clear)
{
    struct nfp_hwdbg_break_data *data;
    int i;
    int ret;
    int cnt;

    /* check for new breaks */
    ret = nfp_hwdbg_brk_check(d->h_hwdbg, NULL, 0, &cnt);
    if (ret < 0) {
        printf("failed to check breaks\n");
        return -1;
    }
    if (cnt != 0) {
        if (clear) {
            printf("clearing %d breaks:\n", cnt);
        }
        data = malloc(sizeof(*data) * cnt);
        if (!data) {
            fprintf(stderr, "failed to allocate memory for break status\n");
            return -1;
        }
        if (nfp_hwdbg_brk_check(d->h_hwdbg, data, cnt, &cnt) < 0) {
            fprintf(stderr, "failed to get break status\n");
            free(data);
            return -1;
        }
        for (i = 0; i < cnt; i++) {
            if (data[i].breakid < 0) {
                printf("%d : %ld - me%04x.%d:%d\n", i,
                        data[i].breakid, data[i].meid,
                        data[i].ctx, data[i].pc);
            } else {
                printf("%d : %lx - me%04x.%d:%d\n", i,
                        data[i].breakid, data[i].meid,
                        data[i].ctx, data[i].pc);
            }
            if (clear) {
                nfp_hwdbg_brk_clear(d->h_hwdbg, data[i].breakid);
            }
        }
        free(data);
    }
    return 0;
}

static int hwdbg_dump(struct interactive_data *d, const char *argstr)
{
    return hwdbg_dump_clear(d, 0);
}

static int hwdbg_clear(struct interactive_data *d, const char *argstr)
{
    return hwdbg_dump_clear(d, 1);
}


/*********************************************************************
 * Exported Command list                                             *
 *********************************************************************/

struct command_data hwdbg_cmds[] = {
    { "R",  hwdbg_run,        MODE_HWDBG, "Run ME"},
    { "S",  hwdbg_stop,       MODE_HWDBG, "Stop ME"},
    { "I",  hwdbg_step,       MODE_HWDBG, "Step a single ME instruction <ctx>, if no ctx then step current"},
    { "bp", add_pcb,          MODE_HWDBG, "Add HW PC breakpoint (pc ctxmask)"},
    { "BN", del_pcb,          MODE_HWDBG, "Del HW PC breakpoint"},
    { "BR", reset_data_all,   MODE_HWDBG, "Reset all server data"},
    { "BS", reset_data_nobp,  MODE_HWDBG, "Reset server data, keep BPs"},
    { "Ba", toggle_autoclear, MODE_HWDBG, "Toggle autoclear breaks (default:No autoclear)"},
    { "BA", toggle_autorun,   MODE_HWDBG, "Toggle autorun; RUN after autoclear (default:No autorun)"},
    { "Bl", hwdbg_dump,       MODE_HWDBG, "List active breaks"},
    { "Bc", hwdbg_clear,      MODE_HWDBG, "Clear active breaks"},
    { "ps", me_status,        MODE_HWDBG, "Display HWDBG ME status"},
    { NULL } /* end of list */
    };
