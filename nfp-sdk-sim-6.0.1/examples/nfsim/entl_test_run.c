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

// State definition
#define ENTL_STATE_IDLE     0
#define ENTL_STATE_HELLO    1
#define ENTL_STATE_WAIT     2
#define ENTL_STATE_SEND     3
#define ENTL_STATE_RECEIVE  4
#define ENTL_STATE_AM       5
#define ENTL_STATE_BM       6
#define ENTL_STATE_AH       7
#define ENTL_STATE_BH       8
#define ENTL_STATE_ERROR    9

#define ENTL_MESSAGE_HELLO  0x0000
#define ENTL_MESSAGE_EVENT  0x0001
#define ENTL_MESSAGE_NOP    0x0002
#define ENTL_MESSAGE_AIT    0x0003
#define ENTL_MESSAGE_ACK    0x0004
#define ENTL_MESSAGE_ONLY   0x8000

static char *state_names[] =
{
    "idle", "hello", "wait", "send", "receive", "AM", "BM", "AH", "BH", "ERROR"
} ;

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
    //char wmem[1024*8*8];
    struct nfp_device *dev;
    //short buffer[128*1024];
    int i;
    uint32_t val;
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
    for( i = 0; i < 2 ; i++ ) {
        step_sim(dev, 100);
        ret = nfp_mecsr_read(dev, menum, -1, NFP_ME_MAILBOX_0+ (4*3), &val);
        if (ret)
            printf("error reading Mailbox%d: %s\n", 3, strerror(errno));
        else
            printf("Mailbox%d = %x %s\n", 3, val, state_names[(val>>16)&0xf] );

    }
    val = 0 ;
    ret = nfp_mecsr_write(dev, menum, -1, NFP_ME_MAILBOX_0 + (4*2), val);
    val = 0xbadbeef - 1 ;
    ret = nfp_mecsr_write(dev, menum, -1, NFP_ME_MAILBOX_0 + (4*1), val);
    val = (ENTL_MESSAGE_ONLY | ENTL_MESSAGE_HELLO) << 16 ;
    ret = nfp_mecsr_write(dev, menum, -1, NFP_ME_MAILBOX_0, val);
    for( i = 0; i < 5 ; i++ ) {
        step_sim(dev, 100);
        ret = nfp_mecsr_read(dev, menum, -1, NFP_ME_MAILBOX_0+ (4*3), &val);
        if (ret)
            printf("error reading Mailbox%d: %s\n", 3, strerror(errno));
        else
            printf("  Mailbox%d = %x %s\n", 3, val, state_names[(val>>16)&0xf] );

    }
 
    /*
    for (i = 0; i < 100; i++) {
        ret = nfp_mecsr_read(dev, menum, -1, NFP_ME_MAILBOX_0+ (4*3), &val);
        if (ret)
            printf("error reading Mailbox%d: %s\n", 3, strerror(errno));
        else
            printf("Mailbox%d = %x\n", 3, val);
        ret = nfp_mecsr_write(dev, menum, -1, NFP_ME_MAILBOX_0 + (4*1), val);
        val = (ENTL_MESSAGE_ONLY | ENTL_MESSAGE_HELLO) << 16 ;
        ret = nfp_mecsr_write(dev, menum, -1, NFP_ME_MAILBOX_0 , val);
        step_sim(dev, 100);
    }
    */

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

