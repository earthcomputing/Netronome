#include <nfp.h>
#include <me.h>
#include <mutexlv.h>
#include <stdint.h>
#include <pkt/pkt.h>
#include <net/eth.h>
#include <nfp/mem_bulk.h>
//#include "system_init_c.h"
#include "entl_state_machine.h"

#define ETH_P_ECLP  0xEAC0    /* Earth Computing Link Protocol [ NOT AN OFFICIALLY REGISTERED ID ] */

#define ENTL_STATE_HELLO    1
#define ENTL_MESSAGE_ONLY     0x800000000000

#define BACKOFF_SLEEP           256

#ifndef PKT_NBI_OFFSET
#define PKT_NBI_OFFSET 64
#endif

#define MAC_PREPEND_BYTES       8

#ifndef NBI
#define NBI 0
#endif

#if (__REVISION_MIN < __REVISION_B0)
#define MS_MAX_OFF  64
#else
#define MS_MAX_OFF  128
#endif

struct pkt_hdr {
    struct {
        uint32_t mac_timestamp;
        uint32_t mac_prepend;
    };
    struct eth_hdr pkt;
};

struct pkt_rxed {
    struct nbi_meta_catamaran nbi_meta;
    struct pkt_hdr            pkt_hdr;
};

#define CTM_ALLOC_ERR 0xffffffff
#define MY_MAC_ADDR 0xbadbeef
#define RETRY_CYCLE 1000

volatile uint32_t  ctm_pkt_num ;

__shared uint64_t  keep_pkt_num ;

__shared __lmem entl_state_machine_t state;
MUTEXLV state_lock = 0;
#define STATE_LOCK_BIT 0

SIGNAL entl_send_sig;

#define MAX_CREDITS         255

__export __shared __cls struct ctm_pkt_credits ctm_credits =
    {MAX_CREDITS, MAX_CREDITS};

__intrinsic int wait_with_timeout( unsigned int cycle ) {
    SIGNAL entl_timeout_sig;
    set_alarm( cycle, &entl_timeout_sig );
    wait_sig_mask(__signals(&entl_send_sig) | __signals(&entl_timeout_sig));
    return signal_test(&entl_timeout_sig);
}

__intrinsic struct pkt_ms_info
__my_pkt_msd_write(__addr40 void *pbuf, unsigned char off,
                __xwrite uint32_t xms[2], size_t size, sync_t sync,
                SIGNAL *sig)
{
    __gpr struct pkt_ms_info msi;
    unsigned int mbox2 ;
    /* Check for an illegal packet offset for direct modification script */
    //__RT_ASSERT((off >= 16) && (off <= (MS_MAX_OFF + 16)));

    /* Check if a no-op modification script is possible */
    if (off <= MS_MAX_OFF && (off & 7) == 0) {
        /* Write a no-op modification script right before the packet start */
        msi.off_enc = (off >> 3) - 2;

        xms[0] =  (NBI_PM_TYPE(NBI_PM_TYPE_DIRECT) |
                  NBI_PM_OPCODE(NBI_PKT_MS_INSTRUCT_NOOP, 0, 0));
        xms[1] = 0;
        mbox2 = NBI_PKT_MS_INSTRUCT_NOOP ;
        __mem_write64(xms, (__addr40 unsigned char *)pbuf + off - 8, size,
                      size, sync, sig);
    } else {
        /* Determine a starting offset for the 8-byte modification script that
         * is closest to the start of packet, that is 8-byte aligned, and that
         * is still within the 120-byte (56-byte for A0) offset limit */
        unsigned char ms_off = MS_MAX_OFF - 8;

        if (off < MS_MAX_OFF)
            ms_off = (off & ~0x7) - 8;

        /* write a delete modification script to remove any excess bytes */
        msi.off_enc = (ms_off >> 3) - 1;

        xms[0] = (NBI_PM_TYPE(NBI_PM_TYPE_DIRECT) |
                  NBI_PM_OPCODE(NBI_PKT_MS_INSTRUCT_DELETE,
                                off - ms_off - 4, 0));
        xms[1] = 0;
        mbox2 = msi.off_enc << 24 | ms_off << 16 | off - ms_off - 4 ;
        __mem_write64(xms, (__addr40 unsigned char *) pbuf + ms_off, size,
                      size, sync, sig);
    }
    //mbox2 = msi.off_enc << 16 | msi.len_adj ;
    local_csr_write(local_csr_mailbox2, mbox2);

    /* Set the length adjustment to point to the start of packet. */
    msi.len_adj = off;

    return msi;
}


__intrinsic struct pkt_ms_info
my_pkt_msd_write(__addr40 void *pbuf, unsigned char off)
{
    SIGNAL sig;
    __xwrite uint32_t ms[2];

    return __my_pkt_msd_write(pbuf, off, ms, sizeof(ms), ctx_swap, &sig);
}

struct eth_hdr_padded {
    union {
        struct {
          struct eth_hdr hdr ;
          uint8_t padd[2] ;
        }  ;
      };
      uint32_t __raw[8];
} __PACKED;


int receive_packet( struct pkt_rxed *pkt_rxed, size_t size )
{
    __xread struct pkt_rxed pkt_rxed_in;
    int island, pnum;
    int pkt_off;
    __mem struct pkt_hdr *pkt_hdr;
    uint8_t *wmem ;
    int64_t s_addr ;
    int64_t d_addr ;
    int ret ;
    unsigned int mbox0, mbox1, mbox2 ;

    pkt_nbi_recv(&pkt_rxed_in, sizeof(pkt_rxed->nbi_meta));
    pkt_rxed->nbi_meta = pkt_rxed_in.nbi_meta;

    pkt_off  = PKT_NBI_OFFSET;
    island   = pkt_rxed->nbi_meta.pkt_info.isl;
    pnum     = pkt_rxed->nbi_meta.pkt_info.pnum;
    pkt_hdr  = pkt_ctm_ptr40(island, pnum, pkt_off);

    mem_read32(&(pkt_rxed_in.pkt_hdr), pkt_hdr, sizeof(pkt_rxed_in.pkt_hdr));
    pkt_rxed->pkt_hdr = pkt_rxed_in.pkt_hdr;
    wmem = &pkt_rxed->pkt_hdr.pkt ;
    d_addr = (uint64_t)wmem[0]<<40 | (uint64_t)wmem[1]<<32 | (uint64_t)wmem[2]<<24 | (uint64_t)wmem[3]<<16 | (uint64_t)wmem[4]<<8 | (uint64_t)wmem[5] ; 
    s_addr = (uint64_t)wmem[6]<<40 | (uint64_t)wmem[7]<<32 | (uint64_t)wmem[8]<<24 | (uint64_t)wmem[9]<<16 | (uint64_t)wmem[10]<<8 | (uint64_t)wmem[11] ; 
    mbox0 = d_addr >> 32 ;
    mbox1 = d_addr ;
    mbox2 = s_addr ;
    local_csr_write(local_csr_mailbox0, mbox0 );
    local_csr_write(local_csr_mailbox1, mbox1 );
    local_csr_write(local_csr_mailbox2, mbox2 );
    MUTEXLV_lock(state_lock,STATE_LOCK_BIT) ;
    ret = entl_received( &state, s_addr, d_addr, 0, 0 ) ;
    MUTEXLV_unlock(state_lock,STATE_LOCK_BIT) ;
    if( d_addr & ENTL_MESSAGE_ONLY )
    {
      keep_pkt_num = 0x800000000000 | pnum ;
    }


    return ret;
}

void
rewrite_packet( __addr40 char *pbuf, uint64_t s_addr, uint64_t d_addr )
{
  SIGNAL sig;
  __xread uint32_t pkt_buf[4];
  volatile __declspec(write_reg) uint32_t w_pkt[4] ;
  uint8_t pkt[16] ;

  mem_read32( &pkt_buf[0], pbuf, 16 ) ;
  pkt[0] = d_addr >> 40 ;
  pkt[1] = d_addr >> 32 ;
  pkt[2] = d_addr >> 24 ;
  pkt[3] = d_addr >> 16 ;
  pkt[4] = d_addr >> 8 ;
  pkt[5] = d_addr ;
  pkt[6] = s_addr >> 40 ;
  pkt[7] = s_addr >> 32 ;
  pkt[8] = s_addr >> 24 ;
  pkt[9] = s_addr >> 16 ;
  pkt[10] = s_addr >> 8 ;
  pkt[11] = s_addr ;
  pkt[12] = ETH_P_ECLP >> 8 ;
  pkt[13] = ETH_P_ECLP ;
  w_pkt[0] = pkt[0] << 24 | pkt[1] << 16 | pkt[2] << 8 | pkt[3] ;
  w_pkt[1] = pkt[4] << 24 | pkt[5] << 16 | pkt[6] << 8 | pkt[7] ;
  w_pkt[2] = pkt[8] << 24 | pkt[9] << 16 | pkt[10] << 8 | pkt[11] ;
  w_pkt[3] = pkt[12] << 24 | pkt[13] << 16 | pkt_buf[3] & 0xffff ;

  mem_write32( (void*)w_pkt, (__addr40 uint8_t *)pbuf , 16 );

}

void
send_packet( int island, int pnum, int plen, unsigned int seq_num, int flag )
{
    int pkt_off;
    __gpr struct pkt_ms_info msi;
    __addr40 char *pbuf;
    //unsigned int mbox2 ;
    /* Write the MAC egress CMD and adjust offset and len accordingly */
    pkt_off = PKT_NBI_OFFSET;  // 64
    pbuf   = pkt_ctm_ptr40(island, pnum, 0);

    pkt_off += MAC_PREPEND_BYTES; // 64 + 8 = 72
    plen -= MAC_PREPEND_BYTES;
    pkt_mac_egress_cmd_write(pbuf, pkt_off, 0, 0); // Write 4byte to make the packet MAC egress not generate L3 and L4 checksums

    pkt_off -= 4;  // rewind back 4 byte for egress cmd
    plen += 8;
    msi = my_pkt_msd_write(pbuf, pkt_off); // Write a packet modification script of NULL
    //mbox2 = msi.off_enc << 16 | msi.len_adj ;
    //local_csr_write(local_csr_mailbox2, mbox2);

    if( flag ) {
      pkt_nbi_send(island, pnum, &msi, plen, NBI, 0,
                 0, seq_num, PKT_CTM_SIZE_256);
    }
    else {
      pkt_nbi_send_dont_free(island, pnum, &msi, plen, NBI, 0,
                 0, seq_num, PKT_CTM_SIZE_256);
    }

}

int
main(void)
{

    if (__ctx() == 0) {
        int ret ;
        int64_t s_addr ;
        int64_t d_addr ;
        unsigned int mbox3, mbox2, mbox1 ;
        __addr40 char *pkt_hdr;    /* The packet in the CTM */
        int pkt_off = PKT_NBI_OFFSET + MAC_PREPEND_BYTES ;
        unsigned int seq_num = 0 ;
        int pnum ;
        local_csr_write(local_csr_mailbox0, 0);
        state.my_addr = MY_MAC_ADDR ;
        ctm_pkt_num = pkt_ctm_alloc( &ctm_credits, __ISLAND, PKT_CTM_SIZE_256, 1, 0);
        while (ctm_pkt_num == CTM_ALLOC_ERR)
        {
          sleep(BACKOFF_SLEEP);
          //PIF_COUNT(ERROR_WAIT_CTM_PKT_ALLOC);
          ctm_pkt_num = pkt_ctm_alloc( &ctm_credits, __ISLAND, PKT_CTM_SIZE_256, 1, 0);
        }
        keep_pkt_num = 0 ; 
        MUTEXLV_lock(state_lock,STATE_LOCK_BIT) ;
        entl_state_init( &state ) ;
        //entl_set_random_addr( &state ) ;
        state.my_addr = MY_MAC_ADDR ; //((addr << 32 ) | l_rand()) & 0xffffffffffff ;
        state.state.current_state = ENTL_STATE_HELLO ;
        MUTEXLV_unlock(state_lock,STATE_LOCK_BIT) ;
        mbox3 = state.my_addr ;
        for (;;) {
          int flag ;
          ret = wait_with_timeout(RETRY_CYCLE);
          MUTEXLV_lock(state_lock,STATE_LOCK_BIT) ;
          ret = entl_next_send( &state, &d_addr, 0 ) ; 
          MUTEXLV_unlock(state_lock,STATE_LOCK_BIT) ;
          local_csr_write(local_csr_mailbox3, mbox3);
          if( ret ) {
            d_addr |= ENTL_MESSAGE_ONLY ;
            mbox3 = (d_addr >>16 &0xffff0000) | 0x8000 | state.state.current_state ;
            //local_csr_write(local_csr_mailbox3, mbox3);
            if( keep_pkt_num ) {
              pnum = keep_pkt_num & 0xffffffff ;
              keep_pkt_num = 0 ;
              flag = 1 ;
            }
            else {
              pnum = ctm_pkt_num ;
              flag = 0 ;
            }
            pkt_hdr  = pkt_ctm_ptr40(__ISLAND, pnum, pkt_off);
            s_addr = state.my_addr ;
            rewrite_packet( pkt_hdr, s_addr, d_addr ) ;
            send_packet( __ISLAND, pnum, 64 + 4, seq_num++, flag ) ;
            //local_csr_write(local_csr_mailbox2, seq_num );
          }

        }
    }
    else {
      struct pkt_rxed pkt_rxed; /* The packet header received by the thread */
      //__mem struct pkt_hdr *pkt_hdr;    /* The packet in the CTM */
        unsigned int mbox0, mbox1, mbox2 ;
        int ret ;
        int i = 0 ;
        sleep(100) ;
        while( state.my_addr != MY_MAC_ADDR ) {
            sleep(100) ;            
        }
        mbox2 = state.my_addr ;
        local_csr_write(local_csr_mailbox2, mbox2 );
        sleep(100) ;             
        signal_ctx(0, __signal_number(&entl_send_sig)) ;  // send hello first       
        sleep(100) ;             
        for (;;) {
          ret = receive_packet(&pkt_rxed, sizeof(pkt_rxed));
          mbox0 = (ret << 16) | 0x8000 | state.state.current_state ;
          //local_csr_write(local_csr_mailbox0, mbox0 );
          if( ret & ENTL_ACTION_SEND ) {
            //sleep(100) ;
            signal_ctx(0, __signal_number(&entl_send_sig)) ;                    
              
          }            
          // sleep(100) ;
    
        }
    }
    return 0;
}	
