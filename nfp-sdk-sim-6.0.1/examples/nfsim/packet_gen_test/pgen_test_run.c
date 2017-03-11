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

#include "entl_state_machine.h"

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

/* NBI number to use (0-1) */
#define NBI 0

/* Ethernet/Hydra port to use (0-23) */
#define PORT 0

/* Number of packets to inject */
#define NUMPACK 8

static entl_state_machine_t state ;

void step_sim(struct nfp_device *dev, int steps)
{
    printf("stepping %d cycles\n", steps);
    nfp_sal_clock_step(dev, steps, 1);
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
            printf("Mailbox%d = %x\n", j, val);
        }
    }

}

static int send_back( struct nfp_device *dev, int menum ) {
    int ret, st;
    uint64_t d_addr ;
    uint32_t val;

    st = entl_next_send( &state, &d_addr, 0 ) ; 
    if( st & ENTL_ACTION_SEND ) {
        printf( "entl_next_send sending %x %d %d\n", d_addr, state.state.current_state, st) ;
        val = 0 ;
        ret = nfp_mecsr_write(dev, menum, -1, NFP_ME_MAILBOX_2, d_addr);
        val = state.my_addr ;
        ret = nfp_mecsr_write(dev, menum, -1, NFP_ME_MAILBOX_1, val);
        val = (ENTL_MESSAGE_ONLY >> 16) | (d_addr >> 16) | ((state.my_addr & 0xffff00000000 )>> 32) ;
        ret = nfp_mecsr_write(dev, menum, -1, NFP_ME_MAILBOX_0, val);
        show_mailboxes(dev, menum) ;
        step_sim(dev, 100);
        ret = nfp_mecsr_read(dev, menum, -1, NFP_ME_MAILBOX_0, &val );
        while( val != 0){
            step_sim(dev, 100);
            ret = nfp_mecsr_read(dev, menum, -1, NFP_ME_MAILBOX_0, &val );
            if( ret ) break ;
        }
    }
    else if( st ) {
        printf( "entl_next_send %x %d %d\n", d_addr, state.state.current_state, st) ;
    }
    return st ;
}

static int read_packet( struct nfp_device *dev ) {
    char wmem[1024*8*8];
    int i, j, n, m ;
    uint64_t val64;

    m = nfp_sal_packet_egress_status(dev, 0, 0);
    for (j = 0; j < m; j++) {
        n = nfp_sal_packet_egress(dev, 0, 0, (void *) wmem, 8000, &val64);
        printf("new egress packet @ time %lu(size = %d bytes):\n", val64, n);
        for (i = 0; i < n; i++) {
            printf("%02x ", (unsigned char) wmem[i]);
            if (i % 16 == 15) {
                printf("\n");
            }
        }
        printf("\n");
    }

}

int main(int argc, char **argv)
{
    struct nfp_device *dev;
    int i;
    //uint32_t val;
    //int ret;
    int menum = NFP6000_MEID(32, 0);

    dev = nfp_device_open(0);
    if (!dev) {
        fprintf(stderr, "error: nfp_device_open failed\n");
        return -1;
    }

    for( i = 0; i < 5000 ; i++ ) {
        step_sim(dev, 100);
        read_packet(dev) ;

        show_mailboxes(dev, menum);
    }
 
    printf("Read mailbox after\n");
    show_mailboxes(dev, menum);


    return 0;
}

