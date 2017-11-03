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

struct fw_table_entry {
  union {
    struct {
      unsigned int reserved:11 ;   
      unsigned int drop_host:1 ;  // do not forward packet from host
      unsigned int parent:4 ;     // parent port of this tree
      uint16_t fw_vector ;        // bit 0 to host
      uint32_t next_lookup[15] ;  // next lookup field for the packet
    };
    uint32_t __raw[16] ; // 64 byte
  };
} __packed ;  

typedef struct fw_table_entry fw_table_entry_t ;
static uint32_t mbox0 = 0 ;
static void show_mailboxes(struct nfp_device *dev, int menum) {
    int i, j ;
    uint32_t val;
    int ret;
    int flag = 0 ;

    for (j = 0; j < 4; j++) {
        ret = nfp_mecsr_read(dev, menum, -1, NFP_ME_MAILBOX_0 + j*4, &val);
        if (ret)
            printf("error reading Mailbox%d: %s\n", j, strerror(errno));
        else {
            if( j==0 ) {
                if( mbox0 != val ) {
                    mbox0 = val ;
                    flag = 1 ;
                }
            }
            if( flag ) {
                printf("Mailbox%d = %08x ", j, val);
                for( i = 0 ; i < 32 ; i++ ) {
                    printf( "%x", val >> (31-i) & 1 ) ;
                    if( (31-i) && ((31-i)%4)==0 ) printf( "_" ) ;
                }
                printf( "\n") ;
            }

        }
    }

}

void step_sim(struct nfp_device *dev, int steps)
{
    printf("stepping %d cycles\n", steps);
    nfp_sal_clock_step(dev, steps, 1);
}

int main(int argc, char **argv)
{
    struct nfp_device *dev;
    fw_table_entry_t entry ;
    int i, j;
    //uint32_t val;
    int menum = NFP6000_MEID(32, 0);
    int p_mbox3 ;
    dev = nfp_device_open(0);
    if (!dev) {
        fprintf(stderr, "error: nfp_device_open failed\n");
        return -1;
    }

    for( i = 0 ; i < 100 ; i++ ) 
    {
        entry.__raw[0] = 0 ;
        entry.fw_vector = 0x1 << (i % 16) ;
        entry.parent = i & 0xf ;
        entry.drop_host = i & 1 ;
        for( j = 0 ; j < 15 ; j++ ) {
            entry.next_lookup[j] = (i << 16) + j ;
        }
        nfp_imem_write(dev, 28, (void*)&entry, sizeof(fw_table_entry_t), (i * sizeof(fw_table_entry_t)) );
    }

    for( i = 0 ; i < 100 ; i++ ) {
        nfp_imem_read(dev, 28, (void*)&entry, sizeof(fw_table_entry_t), (i * sizeof(fw_table_entry_t)) );
        printf( "i:%d fw_vector:%x parent:%d drop_host:%d nxt %x %x %x %x\n", i,  entry.fw_vector, entry.parent, entry.drop_host, entry.next_lookup[0], entry.next_lookup[1], entry.next_lookup[2], entry.next_lookup[3]) ;
    }

    for( i = 0; i < 1000 ; i++ ) {
        step_sim(dev, 100);
        show_mailboxes(dev,menum);
    }
    return 0;
}
