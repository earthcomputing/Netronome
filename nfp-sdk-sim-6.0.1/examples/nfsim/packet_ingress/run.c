/**
 * Copyright (C) 2013 Netronome Systems, Inc.  All rights reserved.
 *
 * @file   run.c
 * @brief  Example code for packet ingress, interacting with the simulator
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

int main()
{
    struct nfp_device *dev;
    short buffer[128*1024];
    int i,j;
    uint32_t val;
    int ret;
    int menum = NFP6000_MEID(32, 0);

    dev = nfp_device_open(0);
    if (!dev) {
        fprintf(stderr, "error: nfp_device_open failed\n");
        return -1;
    }

    printf("injecting %d packet(s)\n", NUMPACK);

    for (i = 0; i < NUMPACK; i++) {
        for (j = 0; j < 1024; j++) {
            buffer[j] = j + 1024*i;
        }
        nfp_sal_packet_ingress(dev, NBI, PORT, (void *) buffer, 200, 0);
        printf("\n%lu: ingress q = %d\n",
               nfp_sal_packet_get_time(dev),
               nfp_sal_packet_ingress_status(dev, NBI, PORT));

    }

    for (i = 0; i < NUMPACK; i++) {
        step_sim(dev, 300);
        printf("\n%lu: ingress q = %d\n",
               nfp_sal_packet_get_time(dev),
               nfp_sal_packet_ingress_status(dev, NBI, PORT));
    }

    printf("Read mailbox after\n");
    for (i = 0; i < 4; i++) {
        ret = nfp_mecsr_read(dev, menum, -1, NFP_ME_MAILBOX_0 + i*4, &val);
        if (ret)
            printf("error reading Mailbox%d: %s\n", i, strerror(errno));
        else
            printf("Mailbox%d = %x\n", i, val);

        if (i == 3 && val == 0)
            fprintf(stderr, "warning: Mailbox3 is 0; no packets arrived\n");
    }

    return 0;
}

