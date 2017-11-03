/*
 * Copyright (C) 2017,  Earth Computing Inc.  All rights reserved.
 *
 *
 * @file          entl_packet_forward_test_run.c
 * @brief         Packet Forward Test Host code  
 *
 * Author:        Atsushi Kasuya
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

#include "alo_tester.h"

//#define ETH_P_ECLP  0xEAC0    /* Earth Computing Link Protocol [ NOT AN OFFICIALLY REGISTERED ID ] */
#define NET_ETH_ALEN 6

#define PORT_OFFSET 4

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

static entl_state_machine_t state[2] ;

typedef struct imem_entry {
    uint32_t __raw[16] ; // 64 byte
} imem_entry_t;  


void setup_fw_table( struct nfp_device *dev )
{
    fw_table_entry_t entry ;
    int i, j;

    for( i = 0 ; i < 100 ; i++ ) 
    {
        entry.__raw[0] = 0 ;
        entry.fw_vector = 0x1 << (1 + (i & 1)) ;  // repeat 1, 2, 1, 2 
        entry.parent = i & 0x1 ;
        entry.drop_host = 0 ;
        for( j = 0 ; j < 15 ; j++ ) {
            entry.next_lookup[j] = i ;
        }
        nfp_imem_write(dev, 28, (void*)&entry, sizeof(fw_table_entry_t), (i * sizeof(fw_table_entry_t)) );
    }

    for( i = 0 ; i < 100 ; i++ ) {
        nfp_imem_read(dev, 28, (void*)&entry, sizeof(fw_table_entry_t), (i * sizeof(fw_table_entry_t)) );
        printf( "i:%d fw_vector:%x parent:%d drop_host:%d nxt %x %x %x %x\n", i,  entry.fw_vector, entry.parent, entry.drop_host, entry.next_lookup[0], entry.next_lookup[1], entry.next_lookup[2], entry.next_lookup[3]) ;
    }

}

void step_sim(struct nfp_device *dev, int steps)
{
    printf("stepping %d cycles\n", steps);
    nfp_sal_clock_step(dev, steps, 1);
}

static void show_mailboxes(struct nfp_device *dev, int island, int me) {
    int menum = NFP6000_MEID(island, me);
    int j ;
    uint32_t val;
    int ret;

    for (j = 0; j < 4; j++) {
        ret = nfp_mecsr_read(dev, menum, -1, NFP_ME_MAILBOX_0 + j*4, &val);
        if (ret)
            printf("error reading ME%d Mailbox%d: %s\n", me, j, strerror(errno));
        else {
            printf("NE%d Mailbox%d = %08x\n", me, j, val);
        }
    }

}

static void show_packet_stat(struct nfp_cpp *nfp_cpp) {
    uint32_t val ;
    int ret ;

    ret = nfp_xpb_readl(nfp_cpp, 0x20070400, &val);
    if( ret )
        printf( "error reading mei0.mu_packet_engine.MUPEActivePacketCount\n" ) ;
    else
        printf( "mei0.mu_packet_engine.MUPEActivePacketCount: %08x\n", val ) ;

    ret = nfp_xpb_readl(nfp_cpp, 0x20070404, &val);
    if( ret )
        printf( "error reading mei0.mu_packet_engine.MUPEPeakPacketCount\n" ) ;
    else
        printf( "mei0.mu_packet_engine.MUPEPeakPacketCount: %08x\n", val ) ;    

    ret = nfp_xpb_readl(nfp_cpp, 0x21070400, &val);
    if( ret )
        printf( "error reading mei1.mu_packet_engine.MUPEActivePacketCount\n" ) ;
    else
        printf( "mei1.mu_packet_engine.MUPEActivePacketCount: %08x\n", val ) ;

    ret = nfp_xpb_readl(nfp_cpp, 0x21070404, &val);
    if( ret )
        printf( "error reading mei1.mu_packet_engine.MUPEPeakPacketCount\n" ) ;
    else
        printf( "mei1.mu_packet_engine.MUPEPeakPacketCount: %08x\n", val ) ;    

    ret = nfp_xpb_readl(nfp_cpp, 0x081000c0, &val);
    if( ret )
        printf( "error reading nbi0.nbi_dma_csr.NbiDmaPktCntLo\n" ) ;
    else
        printf( "nbi0.nbi_dma_csr.NbiDmaPktCntLo: %08x\n", val ) ;   

    ret = nfp_xpb_readl(nfp_cpp, 0x081000c8, &val);
    if( ret )
        printf( "error reading nbi0.nbi_dma_csr.NbiDmaByteCntLo\n" ) ;
    else
        printf( "nbi0.nbi_dma_csr.NbiDmaByteCntLo: %08x\n", val ) ;       
}

static void write_imem(struct nfp_device *dev, int islnum, void *buffer, unsigned long long length, unsigned long long offset ) {
    int i, j ;
    //int err ;
    imem_entry_t entry ;

    // ssize_t nfp_imem_write(struct nfp_device *dev, int islnum, const void *buffer, unsigned long long length, unsigned long long offset);

    // ssize_t nfp_imem_read(struct nfp_device *dev, int islnum, void *buffer, unsigned long long length, unsigned long long offset);

    nfp_imem_write(dev, islnum, buffer, length, offset ) ;

    //for( i = 0 ; i < 4 ; i++ ) {
    //    nfp_imem_read(dev, 28, (void*)&entry, sizeof(entry), (i * sizeof(entry)) );
    //    printf( "%08lx: ", (i * sizeof(entry) ) ) ;
    //    for( j = 0 ; j < 16 ; j++ ) {
    //        printf( "%08x ", entry.__raw[j]) ;
    //        if( j == 7 ) printf( "\n%08lx: ", (i * sizeof(entry) ) + 32 ) ;
    //    }
    //    printf( "\n" ) ;
    //}
}

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

static int send_entl( struct nfp_device *dev, uint32_t port, uint32_t alo_command ) {
    char wmem[1024*8*8];
    int ret, st;
    uint64_t d_addr ;
    uint64_t data ;
    uint32_t val;
    int i ;

    st = entl_next_send( &state[port], &d_addr, &data, alo_command, 0 ) ; 
    //d_addr |= ENTL_MESSAGE_ONLY ;
    if( st & ENTL_ACTION_SEND ) {
        printf( "entl_next_send port %d sending %llx %d %d %llx\n", port, d_addr, state[port].state.current_state, st, data ) ;
        wmem[0] = d_addr >> 40 ;
        wmem[1] = d_addr >> 32 ;
        wmem[2] = d_addr >> 24 ;
        wmem[3] = d_addr >> 16 ;
        wmem[4] = d_addr >> 8 ;
        wmem[5] = d_addr ;
        wmem[6] = 0 ;
        wmem[7] = 0 ;
        wmem[8] = 0 ;
        wmem[9] = 0 ;
        wmem[10] = 0 ;
        wmem[11] = 0 ;
        wmem[12] = ETH_P_ECLP >> 8 ;
        wmem[13] = ETH_P_ECLP ;
        wmem[14] = data >> 56 ;
        wmem[15] = data >> 48 ;
        wmem[16] = data >> 40 ;
        wmem[17] = data >> 32 ;
        wmem[18] = data >> 24 ;
        wmem[19] = data >> 16 ;
        wmem[20] = data >> 8 ;
        wmem[21] = data ;

        for( i = 22 ; i < 64 ; i++) {
            wmem[i] = i ;
        }
        for( i = 0 ; i < 64 ; i++ ) {
            printf("%02x ", (unsigned char) wmem[i]);
            if (i % 16 == 15) {
                printf("\n");
            }            
        }
        st = nfp_sal_packet_ingress(dev, 0, port*PORT_OFFSET, (void *) wmem, 64, 0);
        step_sim(dev, 100);

        //show_mailboxes(dev, menum) ;
        //ret = nfp_mecsr_read(dev, menum, -1, NFP_ME_MAILBOX_0, &val );
        //while( val != 0){
        //    step_sim(dev, 100);
        //    ret = nfp_mecsr_read(dev, menum, -1, NFP_ME_MAILBOX_0, &val );
        //    if( ret ) break ;
        //}
    }
    else if( st ) {
        printf( "entl_next_send port %d: %x %d %d\n", port, d_addr, state[port].state.current_state, st) ;
    }
    return st ;
}

static int send_fw_packet( struct nfp_device *dev, uint32_t port, uint64_t s_addr ) {
    char wmem[1024*8*8];
    int ret, st;
    uint64_t d_addr ;
    uint64_t data ;
    uint32_t val;
    int i ;

    d_addr = ECLP_FW_MASK ; 
    data = 0 ;
    //d_addr |= ENTL_MESSAGE_ONLY ;
    printf( "send_fw_packet port %d sending %llx %llx %llx\n", port, d_addr, s_addr, data ) ;
    wmem[0] = d_addr >> 40 ;
    wmem[1] = d_addr >> 32 ;
    wmem[2] = d_addr >> 24 ;
    wmem[3] = d_addr >> 16 ;
    wmem[4] = d_addr >> 8 ;
    wmem[5] = d_addr ;
    wmem[6] = s_addr >> 40 ;
    wmem[7] = s_addr >> 32 ;
    wmem[8] = s_addr >> 24 ;
    wmem[9] = s_addr >> 16 ;
    wmem[10] = s_addr >> 8 ;
    wmem[11] = s_addr ;
    wmem[12] = ETH_P_ECLP >> 8 ;
    wmem[13] = ETH_P_ECLP ;
    wmem[14] = data >> 56 ;
    wmem[15] = data >> 48 ;
    wmem[16] = data >> 40 ;
    wmem[17] = data >> 32 ;
    wmem[18] = data >> 24 ;
    wmem[19] = data >> 16 ;
    wmem[20] = data >> 8 ;
    wmem[21] = data ;

    for( i = 22 ; i < 64 ; i++) {
        wmem[i] = i ;
    }
    for( i = 0 ; i < 64 ; i++ ) {
        printf("%02x ", (unsigned char) wmem[i]);
        if (i % 16 == 15) {
            printf("\n");
        }            
    }
    st = nfp_sal_packet_ingress(dev, 0, port*PORT_OFFSET, (void *) wmem, 64, 0);
    step_sim(dev, 100);

    //show_mailboxes(dev, menum) ;
    //ret = nfp_mecsr_read(dev, menum, -1, NFP_ME_MAILBOX_0, &val );
    //while( val != 0){
    //    step_sim(dev, 100);
    //    ret = nfp_mecsr_read(dev, menum, -1, NFP_ME_MAILBOX_0, &val );
    //    if( ret ) break ;
    //}

}

static int read_packet( struct nfp_device *dev, int port, uint64_t *rs_addr, uint64_t *rd_addr ) {
    unsigned char wmem[1024*8*8];
    int i, j, n, m ;
    uint64_t val64;
    uint64_t s_addr ;
    uint64_t d_addr ;
    uint64_t data ;
    uint16_t type ;
    int ret = 0 ;

    m = nfp_sal_packet_egress_status(dev, 0, port*PORT_OFFSET);
    if ( m > 0 ) {
        n = nfp_sal_packet_egress(dev, 0, port*PORT_OFFSET, (void *) wmem, 8000, &val64);
        printf("new egress packet on port %d @ time %lu(size = %d bytes):\n", port, val64, n);
        d_addr = 0 ;
        s_addr = 0 ;
        type = wmem[12] ;
        for( i = 0 ; i < 6 ; i++ ) {
            d_addr = (d_addr << 8) | wmem[i] ;
            s_addr = (s_addr << 8) | wmem[i+6] ;
            //printf( "    d_addr %llx  s_addr %llx  type %0x\n", d_addr, s_addr, type ) ;
        }
        //d_addr = (uint64_t)wmem[0]<<40 | (uint64_t)wmem[1]<<32 | (uint64_t)wmem[2]<<24 | (uint64_t)wmem[3]<<16 | (uint64_t)wmem[4]<<8 | (uint64_t)wmem[5] ; 
        //s_addr = (uint64_t)wmem[6]<<40 | (uint64_t)wmem[7]<<32 | (uint64_t)wmem[8]<<24 | (uint64_t)wmem[9]<<16 | (uint64_t)wmem[10]<<8 | (uint64_t)wmem[11] ; 
        type = type<<8 | wmem[13] ;
        data = 0 ;
        for( i = 0 ; i < 8 ; i ++ ) {
            data = data << 8 | wmem[i+14] ;
        }
        printf( "  d_addr %llx s_addr %llx  type %0x ETH_P_ECLP %x == %d data %llx \n", d_addr, s_addr, type, ETH_P_ECLP, type == ETH_P_ECLP, data ) ;
        if( type == ETH_P_ECLP && GET_ECLP_ETYPE(d_addr) != ENTL_ETYPE_NOP ) {
            ret = entl_received( &state[port], d_addr, data, 0, 0, 0 ) ;
            printf( "ret = %x\n", ret ) ;
        }
        else {
            printf( "  d_addr %llx s_addr %llx  type %0x ETH_P_ECLP %x !! %d \n", d_addr, s_addr, type, ETH_P_ECLP, type == ETH_P_ECLP ) ;
        }
        if( d_addr & ECLP_FW_MASK ) {
            ret |= ENTL_ACTION_FORWARD ;
        }
        for (i = 0; i < n; i++) {
            printf("%02x ", (unsigned char) wmem[i]);
            if (i % 16 == 15) {
                printf("\n");
            }
        }
        printf("\n");
        *rs_addr = s_addr ;
        *rd_addr = d_addr ;

    }
    return ret ;
}

static alo_regs_t src_copy ;


int main(int argc, char **argv)
{
    int i, j, k;
    //uint32_t val;
    int ret;
    struct nfp_device *dev;
    struct nfp_cpp *nfp_cpp ;
    int menum0, menum1  ;
    uint64_t s_addr ;
    uint64_t d_addr ;

    menum0 = NFP6000_MEID(32, 0);
    menum1 = NFP6000_MEID(33, 0);

    entl_state_init( &state[0] ) ;
    alo_regs_init( &state[0].ao ) ;
    state[0].my_value = 0xbeef - 1 ; 
    state[0].state.current_state = ENTL_STATE_HELLO ;
    state[0].name = "DUT" ;

    entl_state_init( &state[1] ) ;
    alo_regs_init( &state[1].ao ) ;
    state[1].my_value = 0xbeef - 1 ; 
    state[1].state.current_state = ENTL_STATE_HELLO ;
    state[1].name = "DUT" ;

    dev = nfp_device_open(0);
    if (!dev) {
        fprintf(stderr, "error: nfp_device_open failed\n");
        return -1;
    }
    nfp_cpp = nfp_cpp_from_device_id(0);
    if( !nfp_cpp ) {
        fprintf( stderr, "error: nfp_cpp_from_device_id failed\n" ) ;
        return -1 ;
    }

    j = 0 ;
    for( i = 0; i < 5000 ; i++ ) {
        step_sim(dev, 100);
        read_lmem(dev, menum0) ;
        read_lmem(dev, menum1) ;
        show_packet_stat(nfp_cpp) ;

        ret = read_packet(dev, 0, &s_addr, &d_addr) ;
        if( ret & ENTL_ACTION_SEND ) {
            if( j++ == 4 ) {
                s_addr = 2 ;
                ret =  send_fw_packet( dev, 1, s_addr ) ;
                step_sim(dev, 300); // some clock to let packet go
            }
            ret = send_entl( dev, 0, 0 ) ;
        }
        else if( ret & ENTL_ACTION_FORWARD )
        {
            s_addr = 2 ;
            ret =  send_fw_packet( dev, 0, s_addr ) ;
        }
        ret = read_packet(dev, 1, &s_addr, &d_addr) ;
        if( ret & ENTL_ACTION_SEND ) {
            //if( j++ == 4 ) {
            //    fw_packet( dev, 1 ) ;
            //}
            ret = send_entl( dev, 1, 0 ) ;
        }
        else if( ret & ENTL_ACTION_FORWARD )
        {
            s_addr = 3 ;
            ret =  send_fw_packet( dev, 1, s_addr ) ;
        }

        for( k = 0 ; k < 12 ; k++ ) {
            show_mailboxes(dev, 32, k);
        }
        for( k = 0 ; k < 12 ; k++ ) {
            show_mailboxes(dev, 33, k);
        }
        

    }
 
    

    return 0;
}

