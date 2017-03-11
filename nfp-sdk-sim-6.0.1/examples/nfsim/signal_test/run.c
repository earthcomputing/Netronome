/**
 * Copyright (C) 2013 Netronome Systems, Inc.  All rights reserved.
 *
 * @file    run.c
 * @brief   Example code for packet egress, interactiving with the simulator
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

#include <nfp-common/nfp_resid.h>
#include <nfp6000/nfp_me.h>
#include <nfp.h>
#include <nfp_sal.h>

/* NBI number to use (0-1) */
#define NBI 0

/* Ethernet/Hydra port to use (0-23) */
#define PORT 0

/* Number of packets to inject */
#define NUMPACK 8

void step_sim(struct nfp_device *dev, int steps)
{
    printf("stepping %d cycles\n", steps);
    nfp_sal_clock_step(dev, steps, 1);
}

int main(int argc, char **argv)
{
    char wmem[1024*8*8];
    struct nfp_device *dev;
    short buffer[128*1024];
    int i,j,n,m;
    uint32_t val;
    uint64_t val64;
    int ret;
    int menum = NFP6000_MEID(32, 0);

    dev = nfp_device_open(0);
    if (!dev) {
        fprintf(stderr, "error: nfp_device_open failed\n");
        return -1;
    }

    /* 
    printf("injecting %d packet(s)\n", NUMPACK);

    for (i = 0; i < NUMPACK; i++) {
        for (j = 0; j < 1024; j++) {
            buffer[j] = j + i * 1024;
        }
        nfp_sal_packet_ingress(dev, 0, 0, (void *) buffer, 200, 0);
        printf("%lu: ingress q = %d; egress q %d\n",
                nfp_sal_packet_get_time(dev),
                nfp_sal_packet_ingress_status(dev, 0, 0),
                nfp_sal_packet_egress_status(dev, 0, 0));
    }
    */

    step_sim(dev, 300);
    for (i = 0; i < NUMPACK; i++) {
        step_sim(dev, 100);
        ret = nfp_mecsr_read(dev, menum, -1, NFP_ME_MAILBOX_0 + i*4, &val);
        if (ret)
            printf("error reading Mailbox%d: %s\n", i, strerror(errno));
        else
            printf("Mailbox%d = %x\n", i, val);
    }

    printf("Read mailbox after\n");
    for (i = 0; i < 4; i++) {
        ret = nfp_mecsr_read(dev, menum, -1, NFP_ME_MAILBOX_0 + i*4, &val);
        if (ret)
            printf("error reading Mailbox%d: %s\n", i, strerror(errno));
        else
            printf("Mailbox%d = %x\n", i, val);
    }

    return 0;
}

