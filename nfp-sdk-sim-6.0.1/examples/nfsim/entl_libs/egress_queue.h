/**
 * Copyright (C) 2017,  Earth Computing Inc.  All rights reserved.
 *
 *
 * @file          egress_queue.h
 * @brief         Egress Queue Access code 
 *
 * Author:        Atsushi Kasuya
 *
 */


#ifndef _EGRESS_QUEUE_H_
#define _EGRESS_QUEUE_H_

#define PACKET_RING_SIZE_LW 256
// #define PACKET_RING_SIZE_LW 8
#define PACKET_RING_NUM_0 2
#define PACKET_RING_NUM_1 3

#define SEQUENCE_NUM_ARRAY_SIZE 12


// Packet info. queued in the Egress Queue
typedef struct packet_data {
    union {
        struct {
          unsigned int island_num:8 ;     // island number 
          unsigned int nbi_num:1 ;        // NBI number to send the packet to
          unsigned int dir:1 ;            // packet direction
          unsigned int bls:2;             // < Buffer list of the MU buffer 
          unsigned int reserved:4 ;       // reserved bits
          unsigned int queue_num:8 ;      // TM queue number
          unsigned int sequencer_num:8 ;  // TM sequencer number / port number
          unsigned int len:32;            // < Length of the packet in bytes 
          unsigned int packet_num:32 ;     // packet number in CTM
          unsigned int seq_num:32 ;        // sequence number
          unsigned int next_lookup_id:32 ; // next lookup index
          unsigned int dummy_word:32 ;     // adjusting length
        };
        unsigned int __raw[6];          /**< Direct access to struct words */
    };
} packet_data_t ;

#ifdef PACKET_RING_EXPORT
__export __shared __cls __align(PACKET_RING_SIZE_LW*sizeof(packet_data_t)) char packet_ring_mem0[PACKET_RING_SIZE_LW*sizeof(packet_data_t)] ;
__export __shared __cls __align(PACKET_RING_SIZE_LW*sizeof(packet_data_t)) char packet_ring_mem1[PACKET_RING_SIZE_LW*sizeof(packet_data_t)] ;
__export __shared __cls __align(uint32_t) uint32_t sequence_num_array0[SEQUENCE_NUM_ARRAY_SIZE] ;
__export __shared __cls __align(uint32_t) uint32_t sequence_num_array1[SEQUENCE_NUM_ARRAY_SIZE] ;

void egress_queue_init() ;

#endif

#ifdef PACKET_RING_IMPORT

__import __shared __cls __align(PACKET_RING_SIZE_LW*sizeof(packet_data_t)) char packet_ring_mem0[PACKET_RING_SIZE_LW*sizeof(packet_data_t)] ;
__import __shared __cls __align(PACKET_RING_SIZE_LW*sizeof(packet_data_t)) char packet_ring_mem1[PACKET_RING_SIZE_LW*sizeof(packet_data_t)] ;
__import __shared __cls __align(uint32_t) uint32_t sequence_num_array0[SEQUENCE_NUM_ARRAY_SIZE] ;
__import __shared __cls __align(uint32_t) uint32_t sequence_num_array1[SEQUENCE_NUM_ARRAY_SIZE] ;

#endif

#ifdef ENTL_FORWARDER_1
#define SEQUENCE_NUM_ARRAY sequence_num_array1

#else
#define SEQUENCE_NUM_ARRAY sequence_num_array0
#endif

void get_packet_data( __xread packet_data_t *pkt_in, uint32_t ring ) ;

void put_packet_data( __xwrite packet_data_t *pkt_out, uint32_t island_ring ) ;

uint32_t get_island_ring( uint32_t port ) ;

uint32_t get_sequencer( uint32_t in_port, uint32_t out_port ) ;

uint32_t get_tm_queue( uint32_t port ) ;

uint32_t get_sequence_num( uint32_t port ) ;

/** Read 32-bit words from Cluster Local Scratch.
 *
 * @param data     Read Transfer Registers the read data will be placed in
 * @param address  Address to read from
 * @param count    Number of 32-bit words to read
 * @param sync     Type of synchronization (sig_done or ctx_swap)
 * @param sig_ptr  Signal to be raised on completion
 *
 * Read count number of 32-bit words from Cluster Local Scratch from address
 * and place them in the transfer registers pointed to by data. The signal
 * sig_ptr will be raised on completion and sync determines if the
 * operation switch context or completes asynchronously.
 *
 * The address is a byte address and does not need to be naturally
 * aligned. However, LW aligned reads will be faster.
 *
 * Data read will be returned either in Little Endian or Big Endian
 * depending on the compiler setting.
 *
 * @note If count is not a constant, the compiler will automatically
 *       generate an instruction with an indirect_ref optional token.
 */
__intrinsic
void cls_read_ptr32(
    __xread void *data,
    volatile void __cls *address,
    unsigned int count,
    sync_t sync,
    SIGNAL *sig_ptr
);
/** Write 32-bit words to Cluster Local Scratch.
 *
 * @param data     Write Transfer Registers to be written
 * @param address  Address to write to
 * @param count    Number of 32-bit words to be written
 * @param sync     Type of synchronization (sig_done or ctx_swap)
 * @param sig_ptr  Signal to be raised on completion
 *
 * Write count number of 32-bit words from the write transfer registers
 * pointed to by data to Cluster Local Scratch at address. The signal
 * sig_ptr will be raised on completion and sync determines if the
 * operation switch context or completes asynchronously.
 *
 * The address has to be long word aligned.
 *
 * Data will be treated as Big Endian or Little Endian depending on the
 * compiler setting.
 */
__intrinsic
void cls_write_ptr32(
    __xwrite void *data,
    volatile void __cls *address,
    unsigned int count,
    sync_t sync,
    SIGNAL *sig_ptr
);
#endif
