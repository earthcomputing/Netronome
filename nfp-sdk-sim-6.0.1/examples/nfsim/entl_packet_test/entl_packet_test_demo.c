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


#include <net/if.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>


#include <nfp-common/nfp_resid.h>
#include <nfp6000/nfp_me.h>
#include <nfp.h>
#include <nfp_sal.h>
#include <nfp_cpp.h>

#include "entl_state_machine.h"

#include "alo_tester.h"

//#define ETH_P_ECLP  0xEAC0    /* Earth Computing Link Protocol [ NOT AN OFFICIALLY REGISTERED ID ] */
#define NET_ETH_ALEN 6

/*
// State definition
#define ENTL_STATE_IDLE     0
#define ENTL_STATE_HELLO    1
#define ENTL_STATE_WAIT     2
#define ENTL_STATE_SEND     3
#define ENTL_STATE_RECEIVE  4
#define ENTL_STATE_RA       5
#define ENTL_STATE_SA       6
#define ENTL_STATE_SB       7
#define ENTL_STATE_RB       8
#define ENTL_STATE_RAL      9
#define ENTL_STATE_SAL      10
#define ENTL_STATE_SBL      11
#define ENTL_STATE_RBL      12
#define ENTL_STATE_ERROR    13
*/
char *entlStateString[] = {"IDLE","HELLO","WAIT","SEND","RECEIVE","RA","SA","SB","RB","RA","SA","SB","RB","ERROR"};
char json_string[512];

FILE *log_file = NULL ;

static int sockfd, w_socket ;
static struct sockaddr_in sockaddr, w_sockaddr ;

static int sin_port ;
#define DEFAULT_DBG_PORT  1337

#define PRINTF printf

static void toServer(char *json) {
  write( w_socket, json, strlen(json) ) ;
  if( log_file ) {
    fprintf( log_file, "%s\n", json ) ;
  }
  //printf( "toServer:%s",json) ;
}

static int open_socket( char *addr ) {
  int flag = 1 ;
  int st = -1 ;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if( sockfd < 0 ) {
      PRINTF( "Can't create socket\n" ) ;
      return 0 ;
  }

  sockaddr.sin_family = AF_INET ;
  sockaddr.sin_addr.s_addr = inet_addr(addr) ; 

  sockaddr.sin_port = htons(DEFAULT_DBG_PORT) ;
  st = connect( sockfd, (struct sockaddr *) &sockaddr, sizeof(struct sockaddr) );
  sin_port = DEFAULT_DBG_PORT ;

  if( st < 0 ) {
      PRINTF( 
        "Can't bind socket to the port %d\n",
        sockaddr.sin_port
      ) ;
      close( sockfd ) ;
      return 0 ;
  }
  else {
      
      PRINTF( 
        "Bind socket to the port %d %d\n",
        sockaddr.sin_port, sin_port
      ) ;
      
  }
  return sin_port ; // sockaddr.sin_port ;
}

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

static entl_state_machine_t state ;

static entl_state_machine_t nfp_state ;

static void gen_json()
{
    char s0r0[20], s0r1[20], s0r2[20], s0r3[20], s0r4[20], buf0[20] ;
    char s1r0[20], s1r1[20], s1r2[20], s1r3[20], s1r4[20], buf1[20] ;
    int s0_state, s1_state ;

    switch( state.state.current_state ) {
        case ENTL_STATE_IDLE: 
        case ENTL_STATE_HELLO:
        case ENTL_STATE_WAIT:
            s0_state = 0 ;
            break ;
        case ENTL_STATE_RECEIVE:
            s0_state = 1 ;
            break ;
        case ENTL_STATE_SEND:
            s0_state = 2 ;
            break ;
        case ENTL_STATE_SA:
        case ENTL_STATE_SAL:
            s0_state = 3 ;
            break ;
        case ENTL_STATE_RA:
        case ENTL_STATE_RAL:
            s0_state = 4 ;
            break ;

        case ENTL_STATE_SB:
        case ENTL_STATE_SBL:
            s0_state = 5 ;
            break ;
        case ENTL_STATE_RB:
        case ENTL_STATE_RBL:
            s0_state = 6 ;
            break ;
        default:
            s0_state = 0 ;
    }

        switch( nfp_state.state.current_state ) {
        case ENTL_STATE_IDLE: 
        case ENTL_STATE_HELLO:
        case ENTL_STATE_WAIT:
            s1_state = 0 ;
            break ;
        case ENTL_STATE_RECEIVE:
            s1_state = 1 ;
            break ;
        case ENTL_STATE_SEND:
            s1_state = 2 ;
            break ;
        case ENTL_STATE_SA:
        case ENTL_STATE_SAL:
            s1_state = 3 ;
            break ;
        case ENTL_STATE_RA:
        case ENTL_STATE_RAL:
            s1_state = 4 ;
            break ;

        case ENTL_STATE_SB:
        case ENTL_STATE_SBL:
            s1_state = 5 ;
            break ;
        case ENTL_STATE_RB:
        case ENTL_STATE_RBL:
            s1_state = 6 ;
            break ;
        default:
            s1_state = 0 ;
    }

    //printf( "sleeping 1 sec on %d\n", count ) ;

    sprintf( s0r0, "%016llx", state.ao.reg[0] ) ;
    sprintf( s0r1, "%016llx", state.ao.reg[1] ) ;
    sprintf( s0r2, "%016llx", state.ao.reg[2] ) ;
    sprintf( s0r3, "%016llx", state.ao.reg[3] ) ;
    sprintf( s0r4, "%016llx", state.ao.reg[4] ) ;
    sprintf( buf0, "%016llx", state.ao.result_buffer ) ;
    sprintf( s1r0, "%016llx", nfp_state.ao.reg[0] ) ;
    sprintf( s1r1, "%016llx", nfp_state.ao.reg[1] ) ;
    sprintf( s1r2, "%016llx", nfp_state.ao.reg[2] ) ;
    sprintf( s1r3, "%016llx", nfp_state.ao.reg[3] ) ;
    sprintf( s1r4, "%016llx", nfp_state.ao.reg[4] ) ;

    sprintf( buf1, "%016llx", nfp_state.ao.result_buffer ) ;    
    sprintf( json_string, "{\"state0\": %d, \"state1\": %d, \"s0r0\": \"%s\", \"s0r1\": \"%s\", \"s0r2\": \"%s\", \"s0r3\": \"%s\", \"s0r4\": \"%s\", \"buf0\": \"%s\", \"s1r0\": \"%s\", \"s1r1\": \"%s\", \"s1r2\": \"%s\", \"s1r3\": \"%s\", \"s1r4\": \"%s\", \"buf1\": \"%s\" }",
       s0_state, s1_state, s0r0, s0r1, s0r2, s0r3, s0r4, buf0, s1r0, s1r1, s1r2, s1r3, s1r4, buf1 );
}

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

typedef struct imem_entry {
    uint32_t __raw[16] ; // 64 byte
} imem_entry_t;  

static map_nfp_state( int i, imem_entry_t *ent ) {
    if( i == 0 ) {
        nfp_state.state.current_state = ent->__raw[0] ;
        nfp_state.ao.reg[0] = ((uint64_t)ent->__raw[4])<<32 | ent->__raw[5] ;
        nfp_state.ao.reg[1] = ((uint64_t)ent->__raw[6])<<32 | ent->__raw[7] ;
        nfp_state.ao.reg[2] = ((uint64_t)ent->__raw[8])<<32 | ent->__raw[9] ;
        nfp_state.ao.reg[3] = ((uint64_t)ent->__raw[10])<<32 | ent->__raw[11] ;
        nfp_state.ao.reg[4] = ((uint64_t)ent->__raw[12])<<32 | ent->__raw[13] ;
    }
    if( i == 4 ) {
        nfp_state.ao.result_buffer = ((uint64_t)ent->__raw[4])<<32 | ent->__raw[5] ;
    }


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
            map_nfp_state( i, &entry ) ;
            printf( "%08lx: ", (i * sizeof(entry) ) ) ;
            for( j = 0 ; j < 16 ; j++ ) {
                printf( "%08x ", entry.__raw[j]) ;
                if( j == 7 ) printf( "\n%08lx: ", (i * sizeof(entry) ) + 32 ) ;
            }
        }
        else printf( "lmem_read error: %d %d",  n, errno ) ;
        printf( "\n" ) ;
    }
    gen_json() ;
    toServer(json_string) ;

}

static void send_packet_json( int m, unsigned char *pkt, int dir )
{
    char buf[20] ;
    char dat[10] ;
    int i , j , k;
    if( log_file ) {
        for( i = 0 ; i < 16 ; i++ ) buf[i] = '_' ;
        buf[16] = 0 ;
        j = 0 ;
        for( i = 0 ; i < m ; i++) {
            if( dir ) {
                for( k = 0 ; k < 16 ; k+=2){
                    buf[k] = buf[k+2] ;
                    buf[k+1] = buf[k+3] ;
                } 
                sprintf( dat, "%02x", pkt[i] ) ;
                buf[14] = dat[0] ;
                buf[15] = dat[1] ;
            }
            else {
                for( k = 15 ; k > 0 ; k-=2) {
                    buf[k] = buf[k-2] ;
                    buf[k-1] = buf[k-3] ;
                }
                sprintf( dat, "%02x", pkt[i] ) ;
                buf[0] = dat[0] ;
                buf[1] = dat[1] ;
            }
            sprintf( json_string, "{ \"packet%d\": \"%s\" }", dir, buf) ;
            fprintf( log_file, "%s\n", json_string ) ;
        }
        for( i = 0 ; i < 16 ; i+=2 ) {
            if( dir ) {
                for( k = 0 ; k < 16 ; k+=2){
                    buf[k] = buf[k+2] ;
                    buf[k+1] = buf[k+3] ;
                } 
                buf[14] = '_' ;
                buf[15] = '_' ;
            }
            else {
                for( k = 15 ; k > 0 ; k-=2) {
                    buf[k] = buf[k-2] ;
                    buf[k-1] = buf[k-3] ;
                }
                buf[0] = '_' ;
                buf[1] = '_' ;
            }
            sprintf( json_string, "{ \"packet%d\": \"%s\" }", dir, buf) ;
            fprintf( log_file, "%s\n", json_string ) ;
        }       
    }

}

static int send_back( struct nfp_device *dev, uint32_t alo_command ) {
    char wmem[1024*8*8];
    int ret, st;
    uint64_t d_addr ;
    uint64_t data ;
    uint32_t val;
    int i ;

    st = entl_next_send( &state, &d_addr, &data, alo_command, 0 ) ; 
    //d_addr |= ENTL_MESSAGE_ONLY ;
    if( st & ENTL_ACTION_SEND ) {
        printf( "entl_next_send %x sending %llx %d %d %llx\n", alo_command, d_addr, state.state.current_state, st, data ) ;
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
        st = nfp_sal_packet_ingress(dev, 0, 0, (void *) wmem, 64, 0);
        send_packet_json(64, wmem, 0) ;

        //step_sim(dev, 300);

        //show_mailboxes(dev, menum) ;
        //ret = nfp_mecsr_read(dev, menum, -1, NFP_ME_MAILBOX_0, &val );
        //while( val != 0){
        //    step_sim(dev, 100);
        //    ret = nfp_mecsr_read(dev, menum, -1, NFP_ME_MAILBOX_0, &val );
        //    if( ret ) break ;
        //}
    }
    else if( st ) {
        printf( "entl_next_send %x %d %d\n", d_addr, state.state.current_state, st) ;
    }
    return st ;
}


static int read_packet( struct nfp_device *dev, int menum ) {
    unsigned char wmem[1024*8*8];
    int i, j, n, m ;
    uint64_t val64;
    uint64_t s_addr ;
    uint64_t d_addr ;
    uint64_t data ;
    uint16_t type ;
    int ret = 0 ;

    m = nfp_sal_packet_egress_status(dev, 0, 0);
    for (j = 0; j < m; j++) {
        n = nfp_sal_packet_egress(dev, 0, 0, (void *) wmem, 8000, &val64);
        printf("new egress packet @ time %lu(size = %d bytes):\n", val64, n);
        send_packet_json(n, wmem, 1) ;
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
        if( type == ETH_P_ECLP ) {
            ret = entl_received( &state, d_addr, data, 0, 0, 0 ) ;
            printf( "ret = %x\n", ret ) ;
        }
        else {
            printf( "  d_addr %llx s_addr %llx  type %0x ETH_P_ECLP %x !! %d \n", d_addr, s_addr, type, ETH_P_ECLP, type == ETH_P_ECLP ) ;
        }
        for (i = 0; i < n; i++) {
            printf("%02x ", (unsigned char) wmem[i]);
            if (i % 16 == 15) {
                printf("\n");
            }
        }
        printf("\n");
        read_lmem(dev, menum) ;

    }
    return ret ;
}

static alo_regs_t src_copy ;

void alo_wr_read(struct nfp_device *dev, struct nfp_cpp *nfp_cpp, int menum, int menum1 ) 
{
    int ret;
    rand_regs(&state.ao) ;

    copy_regs(&state.ao, &src_copy) ;

    printf( "alo_wr_read() test starts\n" ) ;

    if( expect_state_only( &state.state, ENTL_STATE_SEND) == 0 ) {
        printf( "Error, state is not SEND, but %d\n", state.state.current_state) ;
    }
    ret = send_back( dev, ALO_WR ) ; // wr reg 0
    if( expect_state_only( &state.state, ENTL_STATE_RAL) == 0 ) {
        printf( "Error, state is not RAL, but %d\n", state.state.current_state) ;
    }

    printf( "alo_wr_read() test 0\n" ) ;

    ret = 0 ;
    while( ret == 0 ) {
        step_sim(dev, 100);
        ret = read_packet(dev, menum) ;
        read_lmem(dev, menum) ;
        show_mailboxes(dev, menum);
        show_mailboxes(dev, menum1);
        show_packet_stat(nfp_cpp) ;
    }
    printf( "alo_wr_read() test 1\n" ) ;
        read_lmem(dev, menum) ;
        show_mailboxes(dev, menum);
        show_mailboxes(dev, menum1);
        show_packet_stat(nfp_cpp) ;
    if( expect_state_only( &state.state, ENTL_STATE_SAL) == 0 ) {
        printf( "Error, state is not SAL, but %d\n", state.state.current_state) ;
    }
    ret = send_back( dev, 0 ) ;

    if( expect_state_only( &state.state, ENTL_STATE_RECEIVE) == 0 ) {
        printf( "Error, state is not RECEIVE, but %d\n", state.state.current_state) ;
    }
    
    ret = 0 ;
    while( ret == 0 ) {
        step_sim(dev, 100);
        ret = read_packet(dev, menum) ;
        read_lmem(dev, menum) ;
        show_mailboxes(dev, menum);
        show_mailboxes(dev, menum1);
        show_packet_stat(nfp_cpp) ;
    }
    printf( "alo_wr_read() test 2\n" ) ;
        read_lmem(dev, menum) ;
        show_mailboxes(dev, menum);
        show_mailboxes(dev, menum1);
        show_packet_stat(nfp_cpp) ;
    clear_regs(&state.ao) ;
    if( expect_state_only( &state.state, ENTL_STATE_SEND) == 0 ) {
        printf( "Error, state is not SEND, but %d\n", state.state.current_state) ;
    }
    ret = send_back( dev, ALO_RD ) ; // rd reg 0
    if( expect_state_only( &state.state, ENTL_STATE_RAL) == 0 ) {
        printf( "Error, state is not RAL, but %d\n", state.state.current_state) ;
    }
    
    ret = 0 ;
    while( ret == 0 ) {
        step_sim(dev, 100);
        ret = read_packet(dev, menum) ;
        read_lmem(dev, menum) ;
        show_mailboxes(dev, menum);
        show_mailboxes(dev, menum1);
        show_packet_stat(nfp_cpp) ;
    }
    printf( "alo_wr_read() test 3\n" ) ;
        read_lmem(dev, menum) ;
        show_mailboxes(dev, menum);
        show_mailboxes(dev, menum1);
        show_packet_stat(nfp_cpp) ;
    if( expect_state_only( &state.state, ENTL_STATE_SAL) == 0 ) {
        printf( "Error, state is not SAL, but %d\n", state.state.current_state) ;
    }
    ret = send_back( dev, 0 ) ;

    if( expect_state_only( &state.state, ENTL_STATE_RECEIVE) == 0 ) {
        printf( "Error, state is not RECEIVE, but %d\n", state.state.current_state) ;
    }
    
    if( state.ao.reg[0] != src_copy.reg[0]) {
        printf( "Error Read reg[0] %llx is not expected %llx\n", state.ao.reg[0], src_copy.reg[0]) ;
    }
    else {
        printf( "ALO WR -> Read reg[0] %llx passed\n", state.ao.reg[0] ) ;        
    }

}

int main(int argc, char **argv)
{
    int i, j, k;
    //uint32_t val;
    int ret;
    struct nfp_device *dev;
    struct nfp_cpp *nfp_cpp ;
    int menum, menum1  ;
    int port ;
    char *fname ;

    printf( "argc = %d\n", argc ) ;

    menum = NFP6000_MEID(32, 0);
    menum1 = NFP6000_MEID(32, 1);

    entl_state_init( &state ) ;
    alo_regs_init( &state.ao ) ;
    entl_state_init( &nfp_state ) ;
    alo_regs_init( &nfp_state.ao ) ;

    if( argc == 2 ) {
        fname = argv[1] ;
        printf( "fname:%s\n", fname ) ;
        log_file = fopen( fname, "w") ;
        if (! log_file ) {
            printf( "Can't open log file %s\n", log_file ) ;
            exit(1) ;
        }
    }

    state.my_value = 0xbeef - 1 ; 
    state.state.current_state = ENTL_STATE_HELLO ;
    state.name = "DUT" ;

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

    port = open_socket( "127.0.0.1" ) ;

    if( !port ) {
        printf( "Can't open socket\n" ) ;
        return ;
    }
    printf( "got port %d\n", port ) ;

    printf( "listening %d\n", port ) ;

    //if( listen( sockfd, 5 ) < 0 ) {
  //  PRINTF( "listen error on port %d\n", port ) ;
  //  close( sockfd ) ;
  //  return ;
  //}
    //printf( "accepting %d\n", port ) ;

  //a_len = sizeof(w_sockaddr) ;
  w_socket =  sockfd ; // accept( sockfd, (struct sockaddr *)&w_sockaddr, &a_len ) ;



    j = k = 0 ;
    for( i = 0; i < 5000 ; i++ ) {
        step_sim(dev, 100);
        printf( "cycle:%d  k %d\n", i, k++) ;
        read_lmem(dev, menum) ;
        show_mailboxes(dev, menum);
        show_mailboxes(dev, menum1);
        show_packet_stat(nfp_cpp) ;
        ret = read_packet(dev, menum) ;
        if( ret ) {
            k = 0 ;
            if( j++ == 4 ) {
                alo_wr_read(dev, nfp_cpp, menum, menum1) ;
            }            
            else {
                ret = send_back( dev, 0 ) ;
            }
        }

    }
 
    printf("Read mailbox after\n");
    show_mailboxes(dev, menum);


    return 0;
}

