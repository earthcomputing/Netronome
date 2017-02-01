/**
 * Copyright (C) 2014 Netronome Systems, Inc.  All rights reserved.
 *
 * Dump SDK Simulator MAC statistics
 *
 */

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <nfp-common/ns_log.h>
#include <nfp.h>
#include <nfp_sal.h>

/* enum strings, defined from nfp_sal_net.h */
const char *stat_strs[] = {
    "NFP_SAL_PACKET_STAT_PORT_RATE           ",
    "NFP_SAL_PACKET_STAT_INGRESS_RATE        ",
    "NFP_SAL_PACKET_STAT_EGRESS_RATE         ",
    "NFP_SAL_PACKET_STAT_INGRESS_PKTS        ",
    "NFP_SAL_PACKET_STAT_INGRESS_BYTES       ",
    "NFP_SAL_PACKET_STAT_EGRESS_PKTS         ",
    "NFP_SAL_PACKET_STAT_EGRESS_BYTES        ",
    "NFP_SAL_PACKET_STAT_PAUSE_INGRESS_PKTS  ",
    "NFP_SAL_PACKET_STAT_PAUSE_INGRESS_BYTES ",
    "NFP_SAL_PACKET_STAT_PAUSE_EGRESS_PKTS   ",
    "NFP_SAL_PACKET_STAT_PAUSE_EGRESS_BYTES  ",
    "NFP_SAL_PACKET_STAT_LAST                ", };

/*
 * Argument Parsing
 */
#define ARG_DEFAULT_DEVICE    0
#define ARG_DEFAULT_INTERFACE 0
#define ARG_DEFAULT_PORT      0

struct arg_data {
    int device;
    int intf;
    int port;
};

static void usage(char *progname)
{
    fprintf(stderr,
            "usage: %s [options]\n"
            "\nWhere options are:\n"
            "        -d <device>            -- select NFP device, default 0\n"
            "        -i <intf>              -- select Interface, default 0\n"
            "        -p <port>              -- select Port, default 0\n"
            "        -h                     -- print help\n",
            progname);
}

static int uintarg(const char *s, uint64_t *val)
{
    char *cp;
    uint64_t r;
    r = strtoull(s, &cp, 0);
    if (cp == s) {
        fprintf(stderr, "Invalid integer value: %s\n", s);
        return -1;
    }
    *val = r;
    return 0;
}

static void parse_options(struct arg_data* args, int argc, char *argv[])
{
    int o;
    uint64_t val;

    args->device = ARG_DEFAULT_DEVICE;
    args->intf = ARG_DEFAULT_INTERFACE;
    args->port = ARG_DEFAULT_PORT;

    while ((o = getopt(argc, argv, "d:i:p:h")) != -1) {
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
            args->device = val;
            break;
        case 'i':
            if (uintarg(optarg, &val)) {
                usage(argv[0]);
                exit(1);
            }
            args->intf = val;
            break;
        case 'p':
            if (uintarg(optarg, &val)) {
                usage(argv[0]);
                exit(1);
            }
            args->port = val;
            break;
        default:
            usage(argv[0]);
            exit(1);
            break;
        }
    }
}

int mac_stats(struct nfp_device *nfp, int intf, int port)
{
    uint64_t val64;
    int i;
    int retval;

    for (i = 0; i < NFP_SAL_PACKET_STAT_LAST; i++) {
        retval = nfp_sal_packet_get_stat(nfp, intf, port, i, &val64);
        if (retval < 0) {
            break;
        }
        printf("%s = %ld\n", stat_strs[i], val64);
    }

    return retval;
}

int main(int argc, char **argv)
{
    struct arg_data args;
    struct nfp_device *nfp;
    int retval;

    NS_LOG_SET_LEVEL(NS_LOG_LVL_INFO);

    parse_options(&args, argc, argv);

    nfp = nfp_device_open(args.device);
    if (!nfp) {
        fprintf(stderr, "error: nfp_device_open failed\n");
        exit(EXIT_FAILURE);
    }

    retval = mac_stats(nfp, args.intf, args.port);
    if (retval < 0) {
        fprintf(stderr, "error: reading mac stats\n");
        exit(EXIT_FAILURE);
    }

    nfp_device_close(nfp);

    return EXIT_SUCCESS;
}
