/**
 * Copyright (C) 2014 Netronome Systems, Inc.  All rights reserved.
 *
 * Example which retreives MAC port configuration
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
#include <nfp_cpp.h>

#include "xpbcsr_regdesc_addr_map.h"

/*
 * Argument Parsing
 */
#define ARG_DEFAULT_DEVICE    0
#define ARG_DEFAULT_NBI       0

struct arg_data {
    int device;
    int nbi;
};

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
    args->nbi = ARG_DEFAULT_NBI;

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
            args->device = val;
            break;
        case 'n':
            if (uintarg(optarg, &val)) {
                usage(argv[0]);
                exit(1);
            }
            args->nbi = val;
            break;
        default:
            usage(argv[0]);
            exit(1);
            break;
        }
    }
}

int mac_ports(struct nfp_cpp *nfp_cpp,
              uint32_t ig_offset, uint32_t eg_offset,
              uint32_t pcp_remap_offset)
{
    uint32_t val32;
    uint32_t val32a;
    int i, j, k;
    int retval;
    int ig_base_chan;
    int ig_base_num;
    int eg_base_chan;
    int eg_base_num;
    int port = 0;
    uint32_t val32b;

    printf("\t\tPCP Remap");
    for (k = 0; k < 8; k++) {
        printf("\t%d", k);
    }
    printf("\tuntagged\n");
    for (i = 0; i < 8; i++) {
        retval = nfp_xpb_readl(nfp_cpp, ig_offset + i * 4, &val32);
        if (retval < 0) {
            fprintf(stderr, "xpb readl failed (%s)", strerror(errno));
            exit(EXIT_FAILURE);
        }
        retval = nfp_xpb_readl(nfp_cpp, eg_offset + i * 4, &val32a);
        if (retval < 0) {
            fprintf(stderr, "xpb readl failed (%s)", strerror(errno));
            exit(EXIT_FAILURE);
        }
        for (j = 0; j < 3; j++) {
            ig_base_chan = (val32 >> 0) & 0x3f;
            ig_base_num = (val32 >> 6) & 0xf;
            val32 = val32 >> 10;
            eg_base_chan = (val32a >> 0) & 0x3f;
            eg_base_num = (val32a >> 6) & 0xf;
            val32a = val32a >> 10;
            if (port > 11) {
                ig_base_chan += 64;
                eg_base_chan += 64;
            }
            if (ig_base_num != 0) {
                retval = nfp_xpb_readl(nfp_cpp,
                                       pcp_remap_offset + port * 4,
                                       &val32b);
                if (retval < 0) {
                    fprintf(stderr, "xpb readl failed (%s)", strerror(errno));
                    exit(EXIT_FAILURE);
                }

                printf("port %2d\tIg Chan %3d-%d (%d)\t",
                       port, ig_base_chan,
                       ig_base_chan + ig_base_num - 1, ig_base_num);

                for (k=0; k<9; k++) {
                    printf("%d\t", ig_base_chan + (int) (val32b & 7));
                    val32b = val32b >> 3;
                }
                printf("\n");
            }
            if (eg_base_num != 0) {
                printf("     \tEg Chan %3d-%d (%d)\n",
                       eg_base_chan, eg_base_chan + eg_base_num - 1,
                       eg_base_num);
            }
            port += 1;
        }
    }

    return retval;
}

int main(int argc, char **argv)
{
    struct arg_data args;
    struct nfp_device *nfp;
    struct nfp_cpp *nfp_cpp;
    int retval;

    NS_LOG_SET_LEVEL(NS_LOG_LVL_INFO);

    parse_options(&args, argc, argv);

    nfp = nfp_device_open(args.device);
    if (!nfp) {
        fprintf(stderr, "error: nfp_device_open failed\n");
        exit(EXIT_FAILURE);
    }

    nfp_cpp = nfp_cpp_from_device_id(args.device);
    if (!nfp_cpp) {
        fprintf(stderr,
                "error: nfp_cpp_from_device_id (%d) failed\n",
                args.device);
        exit(EXIT_FAILURE);
    }

    printf("MAC Port to Channel Assignment (NBI:%d)\n", args.nbi);
    if (args.nbi == 0) {
        retval = mac_ports(nfp_cpp,
                           XPB_mci0_mac_csr_MacPort2to0ChanAssign,
                           XPB_mci0_mac_csr_MacEgPort2to0ChanAssign,
                           XPB_mci0_mac_csr_MacPcpReMap0);
    } else if (args.nbi == 1) {
        retval = mac_ports(nfp_cpp,
                           XPB_mci1_mac_csr_MacPort2to0ChanAssign,
                           XPB_mci1_mac_csr_MacEgPort2to0ChanAssign,
                           XPB_mci1_mac_csr_MacPcpReMap0);
    } else {
        fprintf(stderr,"error: invalid nbi:%d\n", args.nbi);
        exit(EXIT_FAILURE);
    }

    if (retval < 0) {
        fprintf(stderr, "error: reading mac ports\n");
        exit(EXIT_FAILURE);
    }

    nfp_device_close(nfp);
    nfp_cpp_free(nfp_cpp);

    return EXIT_SUCCESS;
}
