/**
 * Copyright (C) 2013 Netronome Systems, Inc.  All rights reserved.
 *
 * Example using the ME performance stats APIs
 *
 */

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>

#include <nfp-common/ns_log.h>
#include <nfp.h>
#include <nfp_cpp.h>
#include <nfp_sal.h>

/*
 *
 */

void usage(void)
{
    printf("usage: --enable               enable performance stats collection\n"
           "       --disable              disable collection\n"
           "       --is_enabled           check if collection is enabled\n"
           "       --reset                reset collection data\n"
           "       --me meid ctx          print stats for a given ME context\n"
           "       --fifo isl_or_me type  print stats for a given fifo\n");
}

static void getmeid(const char *s, int *meid)
{
    *meid = nfp6000_idstr2meid(s, NULL);
    if (*meid >= 0)
        return;

    *meid = strtol(s, NULL, 0);
    if (*meid >= 0 && *meid <= 0xffff)
        return;

    fprintf(stderr, "invalid MEID string %s", s);
    usage();
    exit(EXIT_FAILURE);
    /* never reached */
}

int main(int argc, char **argv)
{
    struct nfp_device *nfp;
    int enable = 0, disable = 0, is_enabled = 0, reset = 0;
    int getme = 0, ctx = 0, fifo = 0, arg = 0;
    int ret;

    if (argc <= 1) {
        usage();
        return 0;
    }

    if (argc == 2) {
        if (!strcmp(argv[1], "--disable")) {
            disable = 1;
        } else if (!strcmp(argv[1], "--is_enabled")) {
            is_enabled = 1;
        } else if (!strcmp(argv[1], "--enable")) {
            enable = 1;
        } else if (!strcmp(argv[1], "--reset")) {
            reset = 1;
        } else {
            usage();
            return 0;
        }
    }

    if (argc == 4) {
        if (!strcmp(argv[1], "--me")) {
            getmeid(argv[2], &getme);
            ctx = strtol(argv[3], NULL, 0);
        } else if (!strcmp(argv[1], "--fifo")) {
            getmeid(argv[2], &fifo);
            arg = strtol(argv[3], NULL, 0);
        } else {
            usage();
            return 0;
        }
    }

    NS_LOG_SET_LEVEL(NS_LOG_LVL_INFO);

    nfp = nfp_device_open(0);
    if (!nfp) {
        fprintf(stderr, "error: nfp_device_open failed\n");
        return -1;
    }

    if (enable) {
        ret = nfp_sal_pstats_enable(nfp);
        if (ret < 0)
            printf("failed to enable performance stats: %s\n",
                                                 strerror(errno));
    }

    if (disable) {
        ret = nfp_sal_pstats_disable(nfp);
        if (ret < 0)
            printf("failed to disable performance stats: %s\n",
                                                 strerror(errno));
    }

    if (is_enabled) {
        if (nfp_sal_pstats_is_enabled(nfp))
            printf("stats collection is enabled\n");
        else
            printf("stats collection is disabled\n");
    }

    if (reset) {
        ret = nfp_sal_pstats_reset(nfp);
        if (ret < 0)
            printf("failed to reset performance stats: %s\n",
                                                strerror(errno));
    }

    if (fifo > 0) {
#define DATAMAX 1024
        uint64_t cycles;
        uint32_t cnt;
        uint64_t data[DATAMAX];
        unsigned int i;
        ret = nfp_sal_pstats_get_fifo(nfp,
                                      PSTATS_FIFO_ID(fifo, arg),
                                      &cycles,
                                      data,
                                      DATAMAX,
                                      &cnt);
        if (ret < 0) {
            printf("failed to get performance stats: %s\n", strerror(errno));
        } else {
            printf("fifo %x [%ld]:\n", PSTATS_FIFO_ID(fifo, arg), cycles);
            for (i = 0; i < cnt; i++)
                printf(" %ld -> %ld\n", data[i*2], data[i*2 + 1]);
        }
    }

    if (getme > 0) {
        /* get all history */
        uint64_t cycles;
        uint64_t executing;
        uint64_t aborted;
        uint64_t stalled;
        uint64_t idle;
        uint64_t swapped;

        ret = nfp_sal_pstats_get_me(nfp,
                                    getme, ctx,
                                    &cycles,
                                    &executing,
                                    &aborted,
                                    &stalled,
                                    &idle,
                                    &swapped);
        if (ret < 0) {
            printf("failed to get performance stats: %s\n", strerror(errno));
        } else {
            printf("ME 0x%04x:%d [%ld]:", getme, ctx, cycles);
            printf("\nexecuting : %ld", executing);
            printf("\naborted   : %ld", aborted);
            printf("\nstalled   : %ld", stalled);
            printf("\nidle      : %ld", idle);
            printf("\nswapped   : %ld\n", swapped);
        }
    }

    nfp_device_close(nfp);

    return 0;
}
