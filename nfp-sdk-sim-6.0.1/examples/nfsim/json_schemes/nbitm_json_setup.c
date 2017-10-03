/**
 * Copyright (C) 2012 Netronome Systems, Inc.  All rights reserved.
 *
 * Perform a simple NBI Traffic Manager setup for packet examples
 *
 */

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <nfp-common/ns_log.h>
#include <nfp-common/nfp_platform.h>

#include <nfp.h>
#include <nfp_cpp.h>
#include <nfp_sal.h>
#include <nfp_nbi_tm.h>

/****** Defines *****/

/* do setup for all queues */
#define QUEUE_CNT 1024
#define QUEUE_SIZE 16

#define ARG_DEFAULT_DEVICE 0
#define ARG_DEFAULT_NBI    0

struct tmsetup_data {
    int arg_device;
    int arg_nbi;
    struct nfp_device *nfp;
    struct nfp_nbi_dev *nfp_nbi;
};

/* convert supported Q sizes into their encoded versions */
int q_size_enc(int qsize)
{
    switch (qsize) {
    case 8    : return 0x3;
    case 16   : return 0x4;
    case 32   : return 0x5;
    case 64   : return 0x6;
    case 128  : return 0x7;
    case 256  : return 0x8;
    case 512  : return 0x9;
    case 1024 : return 0xa;
    case 2048 : return 0xb;
    case 4096 : return 0xc;
    case 8192 : return 0xd;
    case 16384: return 0xe;
    }
    return -1;
}

/* */

int do_config(struct tmsetup_data *d)
{
    struct nfp_nbi_tm_gbl_trf_mngr_cfg gbl_cfg;
    struct nfp_nbi_tm_q_params_cfg qparams;
    struct nfp_nbi_tm_minipkt_crdts_cfg crdt_cfg;
    int i;

    if (q_size_enc(QUEUE_SIZE) < 0) {
        fprintf(stderr, "invalid queue size %d\n", QUEUE_SIZE);
        return -1;
    }


    /* global config */
    gbl_cfg.mac_oobfc    = 0;
    gbl_cfg.l0_chan_map  = 0;
    gbl_cfg.seq0_enable  = 0;
    gbl_cfg.schd_enable  = 1;
    gbl_cfg.shp_enable   = 0;
    gbl_cfg.chan_lvl_sel = 0;
    gbl_cfg.num_seq      = 0;
    gbl_cfg.l1_input_sel = 0;

    if (nfp_nbi_tm_write_gbl_cfg(d->nfp_nbi, &gbl_cfg) < 0) {
        fprintf(stderr,
                "nfp_nbi_tm_write_gbl_cfg failed (%s)\n", strerror(errno));
        return -1;
    }

    /* set up all queue and init the head tail memory */
    for (i = 0; i < QUEUE_CNT; i++) {
        qparams.q_enable = 1;
        qparams.q_size = q_size_enc(QUEUE_SIZE);
        qparams.q_drop_enable = 0;
        qparams.q_red_enable = 0;
        qparams.q_dma_thresh = 0;
        qparams.q_drop_range = 0;
        if (nfp_nbi_tm_write_q_cfg(d->nfp_nbi, i, &qparams) < 0) {
            fprintf(stderr,
                    "nfp_nbi_tm_write_q_cfg failed (%s)\n", strerror(errno));
        }
        if (nfp_nbi_tm_write_headtailsram_cfg(d->nfp_nbi, i, i * QUEUE_SIZE) < 0) {
            fprintf(stderr,
                    "nfp_nbi_tm_write_headtailsram_cfg failed (%s)\n",
                    strerror(errno));
            return -1;
        }
    }

    /* configure credit limits */
    crdt_cfg.minipkt_fc_mode = 1;
    crdt_cfg.fp_crdt_lim = 512;
    crdt_cfg.crdt_lim = 0;

    if (nfp_nbi_tm_write_minipkt_crdt_cfg(d->nfp_nbi, &crdt_cfg) < 0) {
        fprintf(stderr,
                "nfp_nbi_tm_write_minipkt_crdt_cfg failed (%s)\n",
                strerror(errno));
        return -1;
    }

    /* clear the drop counters */
    for (i = 0; i < QUEUE_CNT; i++) {
        uint32_t cnt;
        if (nfp_nbi_tm_read_q_dropcnt(d->nfp_nbi, i, &cnt, 1) < 0) {
            fprintf(stderr,
                    "nfp_nbi_tm_read_q_dropcnt failed (%s)\n",
                    strerror(errno));
            return -1;
        }
    }

    return 0;
}

static void usage(char *progname)
{
    fprintf(stderr,
            "usage: %s [options]\n"
            "\nWhere options are:\n"
            "        -d <device>            -- select NFP device, default 0\n"
            "        -n <nbi>               -- select NBI, default 0\n"
            "        -h                     -- print help\n",
            progname);
}

static int uintarg(const char *s, uint64_t *val)
{
    char *cp;
    uint64_t r;
    r = nfp_strtou64(s, &cp, 0);
    if (cp == s) {
        fprintf(stderr, "Invalid integer value: %s\n", s);
        return -1;
    }
    *val = r;
    return 0;
}

static void parse_options(struct tmsetup_data *d, int argc, char **argv)
{
    int o;
    uint64_t val;

    d->arg_device = ARG_DEFAULT_DEVICE;
    d->arg_nbi = ARG_DEFAULT_NBI;

    while ((o = getopt(argc, argv, "d:n:h")) != -1) {
        switch(o) {
        case 'h':
            usage(argv[0]);
            exit(0);
            break;
        case 'd':
            if (uintarg(optarg, &val)) {
                usage(argv[0]);
                exit(1);
            }
            d->arg_device = val;
            break;
        case 'n':
            if (uintarg(optarg, &val)) {
                usage(argv[0]);
                exit(1);
            }
            d->arg_nbi = val;
            break;
        default:
            usage(argv[0]);
            exit(1);
            break;
        }
    }
}

int main(int argc, char **argv)
{
    struct tmsetup_data data;

    NS_LOG_SET_LEVEL(NS_LOG_LVL_INFO);

    parse_options(&data, argc, argv);

    data.nfp = nfp_device_open(data.arg_device);
    if (!data.nfp) {
        fprintf(stderr, "error: nfp_device_open failed (%s)\n",
                strerror(errno));
        return EXIT_FAILURE;
    }

    data.nfp_nbi = nfp_nbi_open(data.nfp, data.arg_nbi);
    if (!data.nfp_nbi) {
        fprintf(stderr, "error: nfp_nbi_open failed (%s)\n", strerror(errno));
        return EXIT_FAILURE;
    }

    if (do_config(&data) < 0) {
        fprintf(stderr, "error: config failed\n");
        return EXIT_FAILURE;
    }

    nfp_nbi_close(data.nfp_nbi);
    nfp_device_close(data.nfp);

    return EXIT_SUCCESS;
}
