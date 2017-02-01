/**
 * Copyright (C) 2013 Netronome Systems, Inc.  All rights reserved.
 *
 * Example using the NFP SDK Simulator Execution Stats API
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
    printf("usage: --enable meid       enable execution coverage for an ME\n"
           "       --disable           disable all execution coverage \n"
           "       --is_enabled meid   check if execution coverage is enabled\n"
           "       --get meid ctx      retrieve and print execution coverage\n"
           "                           for a specific ME context\n");
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
    int enable = 0, is_enabled = 0, disable = 0, getme = 0, ctx = 0;
    int ret;

    NS_LOG_SET_LEVEL(NS_LOG_LVL_INFO);

    nfp = nfp_device_open(0);
    if (!nfp) {
        fprintf(stderr, "error: nfp_device_open failed\n");
        return EXIT_FAILURE;
    }

    if (argc <= 1) {
        usage();
        return EXIT_FAILURE;
    }

    if (argc == 2) {
        if (!strcmp(argv[1], "--disable")) {
            disable = 1;
        } else {
            usage();
            return EXIT_FAILURE;
        }
    }

    if (argc == 3) {
        if (!strcmp(argv[1], "--enable")) {
            getmeid(argv[2], &enable);
        } else if (!strcmp(argv[1], "--is_enabled")) {
            getmeid(argv[2], &is_enabled);
        } else {
            usage();
            return EXIT_FAILURE;
        }
    }

    if (argc == 4) {
        if (!strcmp(argv[1], "--get")) {
            getmeid(argv[2], &getme);
            ctx = strtol(argv[3], NULL, 0);
        } else {
            usage();
            return EXIT_FAILURE;
        }
    }

    if (enable) {
        ret = nfp_sal_excover_enable(nfp, enable);
        if (ret < 0)
            printf("failed to enable execution coverage: %s\n",
                                                 strerror(errno));
    }

    if (is_enabled) {
        if (nfp_sal_excover_is_enabled(nfp, is_enabled))
            printf("stats collection is enabled\n");
        else
            printf("stats collection is disabled\n");
    }

    if (disable) {
        ret = nfp_sal_excover_disable(nfp);
        if (ret < 0)
            printf("failed to disable execution coverage: %s\n",
                                                 strerror(errno));
    }

    if (getme > 0) {
#define CS_SZ (1024*8*2)
        /* enough space for all ustore for all contexts */
        uint8_t buf[CS_SZ*8];
        int count = CS_SZ;
        int i;
        /* get all history */
        ret = nfp_sal_excover_get(nfp, getme, ctx, buf, CS_SZ*8);
        if (ret < 0) {
            printf("failed to get execution coverage: %s\n", strerror(errno));
            count = 0;
        }
        for (i = 0; i < count; i++) {
            if (!((uint64_t *)buf)[i])
                continue;
            printf("%8d: 0x%08"PRIx64"\n", i, ((uint64_t *)buf)[i]);
        }
    }

    nfp_device_close(nfp);

    return EXIT_SUCCESS;
}
