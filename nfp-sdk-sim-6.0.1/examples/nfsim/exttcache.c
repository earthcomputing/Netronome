/**
 * Copyright (C) 2013 Netronome Systems, Inc.  All rights reserved.
 *
 * Example using the NFP SDK Simulator Tag Cache API
 *
 */

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
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

/**** ****/

/* Default External Memory Unit */
#define DEFAULT_EMEMNO 24
/* Tag cache search has 512 entries */
#define TCLEN   512
/* Tag entries are split across 3 memory */
#define TCMEMS  3
/* 8 Tag cache banks */
#define TCBANKS 8
/* Tag cache values at reset */
#define TC_CLEAN0 0xfffac6fff9bafff8LL
#define TC_CLEAN1 0xfd4efffc3efffb42LL
#define TC_CLEAN2 0xcaffffb6fffe32ffLL

/* States of tag cache entries */
enum {
    TAG_STATE_LOCKED = 0,
    TAG_STATE_IN_USE,
    TAG_STATE_DIRTY_HLR,
    TAG_STATE_DIRTY_LLR,
    TAG_STATE_CLEAN_HLR,
    TAG_STATE_CLEAN_LLR,
};

struct tag_info {
    uint16_t tag;
    uint8_t state;
    uint8_t ecc;
};

struct global_args {
    int devnum;                 /* -n <NFP Device number> */
    int nfp_debug_level;        /* -d <DEBUG LEVEL> */
    int islnum;                 /* -i <Island number> */
};

/**** ****/

/* Define command line arguments */
static const char *g_optstring = "n:i:d:h?";

static struct global_args g_args;

static const struct option g_longopts[] = {
    { "device", required_argument, NULL, 'n' },
    { "island", required_argument, NULL, 'i' },
    { "debug", required_argument, NULL, 'd' },
    { "help", no_argument, NULL, 'h' },
    { NULL, no_argument, NULL, 0 }
};

/* Display program usage, and exit. */
void usage(const char *program, int err)
{
    fprintf(stderr,
            "Usage: %s [options]      dump tcache entries that have been used\n"
            "\nWhere options are:\n"
            "  -n NUM, --nfp=NUM      select NFP device (default:0)\n"
            "  -i NUM, --island=NUM   island number (default:24)\n"
            "  -d NUM, --debug=NUM    Debug Level\n"
            "  -h, -?, --help         Help\n"
            "\n",
            program);

    exit(err ? EXIT_FAILURE : EXIT_SUCCESS);
}

/* get the tag info from the three memory values */
void get_tag_info(struct tag_info *ti, uint64_t v0, uint64_t v1, uint64_t v2)
{
    int i;
    uint64_t tagvals[8];

    /* pull out info of the 8 tags per tag-cache entry */

    tagvals[0] =  (v2 & 0xffffff0000000000) >> 40;
    tagvals[1] =  (v2 & 0x000000ffffff0000) >> 16;
    tagvals[2] = ((v2 & 0x000000000000ffff) <<  8) |
                 ((v1 & 0xff00000000000000) >> 56);
    tagvals[3] =  (v1 & 0x00ffffff00000000) >> 32;
    tagvals[4] =  (v1 & 0x00000000ffffff00) >> 8;
    tagvals[5] = ((v1 & 0x00000000000000ff) << 16) |
                 ((v0 & 0xffff000000000000) >> 48);
    tagvals[6] =  (v0 & 0x0000ffffff000000) >> 24;
    tagvals[7] =  (v0 & 0x0000000000ffffff) >> 0;

    for (i = 0; i < 8; i++) {
        ti[i].tag = (tagvals[i]) & 0x7fff;
        ti[i].state = (tagvals[i] >> 15) & 0x7;
        ti[i].ecc = tagvals[i] >> 18;
    }
}

/* print out the tag entries that dont match the clean values */
void print_dirty(struct nfp_device *dev, int islnum)
{
    uint64_t buf[TCMEMS][TCLEN];
    int i, j, k;
    int ret;
    struct tag_info ti[8];

    for (i = 0; i < TCLEN; i++)
        for (j = 0; j < TCMEMS; j++)
            buf[j][i] = 0;

    for (i = 0; i < TCBANKS; i++ ) {
        for (j = 0; j < TCMEMS; j++) {
            ret = nfp_sal_extmem_tc_read(dev,
                                          islnum,
                                          3*i + j,
                                          (void *)buf[j],
                                          TCLEN * sizeof(uint64_t),
                                          0);
        }

        if (ret < 0) {
            fprintf(stderr, "failed to read from tcache[%d]: %s\n",
                                                    i, strerror(errno));
            return;
        }

        for (j = 0; j < TCLEN; j++) {
            if (buf[0][j] != TC_CLEAN0 ||
                    buf[1][j] != TC_CLEAN1 ||
                    buf[2][j] != TC_CLEAN2) {
                get_tag_info(ti, buf[0][j], buf[1][j], buf[2][j]);
                printf("tag bank [%d] 0x%08x: \n", i , j);
                for (k = 0; k < 8; k++) {
                    printf("        entry %d: tag = 0x%04x, state = %d\n",
                                                    k, ti[k].tag, ti[k].state);
                }
                printf("\n");
            }
        }
    }
}

int main(int argc, char *argv[])
{
    const char *program = argv[0];
    int opt;
    int longindex = 0;
    struct nfp_device *dev;

    /* Initialise g_args to defaults */
    g_args.nfp_debug_level = NS_LOG_LVL_INFO;
    g_args.devnum = 0;
    g_args.islnum = DEFAULT_EMEMNO;

    /* Process the arguments with getopt.., then populate g_args */
    while (1) {
        opt = getopt_long(argc, argv, g_optstring, g_longopts, &longindex);
        if (opt < 0)
            break;

        switch( opt ) {
            case 'd':
                g_args.nfp_debug_level = atoi(optarg);
                break;
            case 'n':
                g_args.devnum = atoi(optarg);
                break;
            case 'i':
                g_args.islnum = atoi(optarg);
                break;
            case 'h':
            case '?':
                usage(program, EXIT_SUCCESS);
                break;
            default:
                break;
        }
    }

    NS_LOG_SET_LEVEL(g_args.nfp_debug_level);

    dev = nfp_device_open(g_args.devnum);
    if (!dev) {
        fprintf(stderr, "error: nfp_device_open failed\n");
        exit(EXIT_FAILURE);
    }

    printf("printing dirty tcache entries\n");
    print_dirty(dev, g_args.islnum);

    nfp_device_close(dev);

    return EXIT_SUCCESS;
}
