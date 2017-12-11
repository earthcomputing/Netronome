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


//#define ETH_P_ECLP  0xEAC0    /* Earth Computing Link Protocol [ NOT AN OFFICIALLY REGISTERED ID ] */
#define NET_ETH_ALEN 6



/**
 * Ethernet header structure
 */
struct __attribute__((__packed__))  eth_addr {
    char a[NET_ETH_ALEN];
} ;


struct __attribute__((__packed__)) eth_hdr {
    struct eth_addr dst;                /** Destination address */
    struct eth_addr src;                /** Source address */
    uint16_t type;                      /** Protocol type */
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

static void show_mailboxes(struct nfp_device *dev, int menum) {
    int j ;
    uint32_t val;
    int ret;

    for (j = 0; j < 4; j++) {
        ret = nfp_mecsr_read(dev, menum, -1, NFP_ME_MAILBOX_0 + j*4, &val);
        if (ret)
            printf("error reading Mailbox%d: %s\n", j, strerror(errno));
        else {
            printf("Mailbox%d = %08x\n", j, val);
        }
    }

}

typedef struct imem_entry {
    uint32_t __raw[16] ; // 64 byte
} imem_entry_t;  

static void read_lmem(struct nfp_device *dev, int menum) {
    int i, j ;
    int err ;
    imem_entry_t entry ;

/**
 * Read memory from ME Local Memory.
 *
 * @param dev           NFP device
 * @param meid          ME ID
 * @param buffer        output buffer
 * @param length        number of bytes to read
 * @param offset        offset into ME Local Memory
 *
 * Read contents of ME Local Memory into @a buffer.  @a length and @a offset
 * must both be 4-byte aligned.
 *
 * @return number of bytes actually read, or -1 on error (and set
 * errno accordingly).
 */
//NFP_API
//ssize_t nfp_lmem_read(struct nfp_device *dev, int meid, void *buffer, unsigned long long length, unsigned long long offset);

    for( i = 0 ; i < 8 ; i++ ) {
        int n = nfp_lmem_read(dev, menum, (void*)&entry, sizeof(entry), (i * sizeof(entry)) );
        if( n > 0 ) {
            printf( "%08lx: ", (i * sizeof(entry) ) ) ;
            for( j = 0 ; j < 16 ; j++ ) {
                printf( "%08x ", entry.__raw[j]) ;
                if( j == 7 ) printf( "\n%08lx: ", (i * sizeof(entry) ) + 32 ) ;
            }
        }
        else printf( "lmem_read error: %d %d",  n, errno ) ;
        printf( "\n" ) ;
    }
}



int main(int argc, char **argv)
{
    int i, j;
    //uint32_t val;
    int ret;
    struct nfp_device *dev;
    int menum, menum1  ;
    menum = NFP6000_MEID(32, 0);
    menum1 = NFP6000_MEID(32, 1);

    dev = nfp_device_open(0);
    if (!dev) {
        fprintf(stderr, "error: nfp_device_open failed\n");
        return -1;
    }
    j = 0 ;
    for( i = 0; i < 5000 ; i++ ) {
        step_sim(dev, 100);
        //read_lmem(dev, menum) ;
        show_mailboxes(dev, menum);
        show_mailboxes(dev, menum1);
    }
 
    printf("Read mailbox after\n");
    show_mailboxes(dev, menum);


    return 0;
}

