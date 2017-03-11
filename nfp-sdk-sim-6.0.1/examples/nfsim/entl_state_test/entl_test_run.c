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

static char *state_names[] =
{
    "idle", "hello", "wait", "send", "receive", "AM", "BM", "AH", "BH", "ERROR", "NA", "NA", "NA", "NA", "NA", "NA"
} ;

static entl_state_machine_t state ;

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

static void show_mailboxes(struct nfp_device *dev, int menum) {
    int j ;
    uint32_t val;
    int ret;

    for (j = 0; j < 4; j++) {
        ret = nfp_mecsr_read(dev, menum, -1, NFP_ME_MAILBOX_0 + j*4, &val);
        if (ret)
            printf("error reading Mailbox%d: %s\n", j, strerror(errno));
        else {
            //if( j == 3 ) {
            //    printf("Mailbox%d = %x %s\n", j, val, state_names[(val>>16)&0xf]);
            //}
            //else {
                printf("Mailbox%d = %x\n", j, val);
            //}
        }
    }

}

static int get_state(struct nfp_device *dev, int menum ) 
{
    uint32_t mbox0, mbox1, mbox2, mbox3 ;
    int ret;
    int64_t s_addr, d_addr ;

    ret = nfp_mecsr_read(dev, menum, -1, NFP_ME_MAILBOX_0, &mbox0 );
    ret |= nfp_mecsr_read(dev, menum, -1, NFP_ME_MAILBOX_1, &mbox1 );
    ret |= nfp_mecsr_read(dev, menum, -1, NFP_ME_MAILBOX_2, &mbox2 );
    ret |= nfp_mecsr_read(dev, menum, -1, NFP_ME_MAILBOX_3, &mbox3 );
    if( ret ) {
        printf("error reading Mailbox: %s\n", strerror(errno));
    }

    if( !ret && mbox3 != 0 ) {
        printf( "Got %x %x %x %x\n", mbox0, mbox1, mbox2, mbox3 ) ;
        ret = nfp_mecsr_write(dev, menum, -1, NFP_ME_MAILBOX_3, 0);  // clear command
        s_addr =  mbox2 ;
        d_addr = (((int64_t)mbox3 & 0xffff0000) << 16) | mbox1 ;
        ret = entl_received( &state, s_addr, d_addr, 0, 0 ) ;
        printf( "get_state ret=%d state = %d\n", ret, state.state.current_state ) ;
    }
    else {
        printf( "got %x %x %x %x\n", mbox0, mbox1, mbox2, mbox3 ) ;
        ret = 0 ;
    }
    return ret ;
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

    entl_state_init( &state ) ;
    state.my_addr = 0xbadbeef - 1 ; 
    state.state.current_state = ENTL_STATE_HELLO ;

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

    for( i = 0 ; i < 50000 ; i++ ) {
        ret = get_state(dev, menum);
        if( ret & ENTL_ACTION_SEND ) {
            ret = send_back(dev, menum) ;
        }
        else step_sim(dev, 100);
    }

    printf("Read mailbox after\n");
    show_mailboxes(dev, menum);


    return 0;
}

