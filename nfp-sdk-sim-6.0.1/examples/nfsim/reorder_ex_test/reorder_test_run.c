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

typedef struct imem_entry {
    uint32_t __raw[16] ; // 64 byte
} imem_entry_t;  

void step_sim(struct nfp_device *dev, int steps)
{
    printf("stepping %d cycles\n", steps);
    nfp_sal_clock_step(dev, steps, 1);
}

static int mbox3 = 0 ;

static void read_imem(struct nfp_device *dev) {
    int i, j ;
    //int err ;
    imem_entry_t entry ;

    // ssize_t nfp_imem_read(struct nfp_device *dev, int islnum, void *buffer, unsigned long long length, unsigned long long offset);

    for( i = 0 ; i < 4 ; i++ ) {
        nfp_imem_read(dev, 28, (void*)&entry, sizeof(entry), (i * sizeof(entry)) );
        printf( "%08lx: ", (i * sizeof(entry) ) ) ;
        for( j = 0 ; j < 16 ; j++ ) {
            printf( "%08x ", entry.__raw[j]) ;
            if( j == 7 ) printf( "\n%08lx: ", (i * sizeof(entry) ) + 32 ) ;
        }
        printf( "\n" ) ;
    }
}

static void show_mailboxes(struct nfp_device *dev, int menum) {
    int j ;
    uint32_t val;
    int ret;

    for (j = 0; j < 4; j++) {
        ret = nfp_mecsr_read(dev, menum, -1, NFP_ME_MAILBOX_0 + j*4, &val);
        if (ret)
            printf("error reading Mailbox%d: %s\n", j, strerror(errno));
        else {
            printf("ME %d Mailbox%d = %08x\n", menum, j, val);
            if( j == 3 ) mbox3 = val ;
        }
    }
}



int main(int argc, char **argv)
{
    struct nfp_device *dev;
    int i;
    //uint32_t val;
    int menum = NFP6000_MEID(32, 0);
    int menum1 = NFP6000_MEID(33, 1);
    int p_mbox3 ;
    dev = nfp_device_open(0);
    if (!dev) {
        fprintf(stderr, "error: nfp_device_open failed\n");
        return -1;
    }

    for( i = 0; i < 5000 ; i++ ) {
        p_mbox3 = mbox3 ;
        step_sim(dev, 100);
        show_mailboxes(dev, menum);
        show_mailboxes(dev, menum1);
        if( p_mbox3 != mbox3 ) {
            read_imem( dev ) ;
        }
    }
    printf("Read mailbox after\n");
    show_mailboxes(dev, menum);
    show_mailboxes(dev, menum1);
    return 0;
}
