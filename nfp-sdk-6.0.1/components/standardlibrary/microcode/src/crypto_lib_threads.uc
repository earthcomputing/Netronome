/*
 * Copyright (C) 2014 Netronome Systems, Inc. All rights reserved.
 */


#ifndef __CRYPTO_LIB_THREADS_UC__
#define __CRYPTO_LIB_THREADS_UC__

#ifndef NFP_LIB_ANY_NFAS_VERSION
    #if (!defined(__NFP_TOOL_NFAS) ||                       \
        (__NFP_TOOL_NFAS < NFP_SW_VERSION(5, 0, 0, 0)) ||   \
        (__NFP_TOOL_NFAS > NFP_SW_VERSION(6, 0, 1, 255)))
        #error "This standard library is not supported for the version of \
                  the SDK currently in use."
    #endif
#endif

/** @file crypto_lib_threads.uc CRYPTO Threads Operation Macros
 * @addtogroup cryptothreads CRYPTO Threads Operation
 * @{
 *
 * @name CRYPTO Threads Operation Macros
 * @{
 *
 *  Crypto logic support macros facilitating encryption, decryption, and
 *  authentication of ip packets. Utilizes multiple crypto cores, packet buffers,
 *  and threads in a pipeline in order to maximize utilization of crypto hardware
 *  and maximize packet processing performance.
 *
 *  Intended to be run on one or more microengines in the crypto island of
 *  NFP6000
 */

// API Quick Reference:
//
// crypto_threads_input(_me_ctx, _crypto_ctx, _in_ring_type, _in_ring_num )
// crypto_threads_output(_me_ctx, _crypto_ctx )
// crypto_threads_init()


#include "aggregate.uc"
#include "crypto_lib.uc"
#include "crypto_lib_ipsec.uc"
#include "localmem.uc"
#include "limit.uc"
#include "sig_macros.uc"
#include "thread.uc"
#include "threads.uc"
#include "ring_utils.uc"
#include "stdmac.uc"

#include "crypto_lib_threads.h" // include shared defines


//
// When using crypto_threads_init, the build system or another file should define
// CRYPTO_NUM_THREADS to be the number of threads to run on the micro engine, and
// CRYPTO_START_CTX to be the 1st crypto core/ctx to be used by the micro engine.
// Refer to description of crypto_threads_init for details.
//

#ifndef CRYPTO_NUM_THREADS
#define CRYPTO_NUM_THREADS 6
#endif

#ifndef CRYPTO_START_CTX
#define CRYPTO_START_CTX 0
#endif

// ring type
#define CRYPTO_RING_CTM  0
#define CRYPTO_RING_EMU0 1
#define CRYPTO_RING_EMU1 2
#define CRYPTO_RING_EMU2 3


// ring size ( in lwords )
#define CRYPTO_RING_SIZE_128   128
#define CRYPTO_RING_SIZE_256   256
#define CRYPTO_RING_SIZE_512   512
#define CRYPTO_RING_SIZE_1K   1024
#define CRYPTO_RING_SIZE_2K   2048
#define CRYPTO_RING_SIZE_4K   4096
#define CRYPTO_RING_SIZE_8K   8192
#define CRYPTO_RING_SIZE_16K 16384
#define CRYPTO_RING_SIZE_32K 32768
#define CRYPTO_RING_SIZE_64K 64536


// Note: below configuration definitions can be overriden by user by adding
// a #define for the definition prior to inclusion of this file

// override the default response ring code, in order to customize the behavior
// for a specific application. user should #define OVERRIDE_RESPONSE_RING 1 and
// define a #macro crypto_threads_send_response () to handle output thread
// generation of response status
#ifndef OVERRIDE_RESPONSE_RING
#define OVERRIDE_RESPONSE_RING 0
#endif
// response ring type
#ifndef RESPONSE_RING_TYPE
#define RESPONSE_RING_TYPE CRYPTO_RING_CTM
#endif
// buffer ring type (for return of 'start of packet' buffers used to
// send re-generated beginning of packet during encryption operations )
// buffer ring is allocated and initialized by user
#ifndef BUF_RING_TYPE
#define BUF_RING_TYPE CRYPTO_RING_CTM
#endif
// buffer ring number
#ifndef BUF_RING_NUM
#define BUF_RING_NUM 0
#endif
// buffer ring island id. set to island id of
// island containing ctm based buffer ring.
// 0x00 means same island crypto_lib_threads is running on.
#ifndef BUF_RING_ISLAND_ID
#define BUF_RING_ISLAND_ID 0x00
#endif


// INIT_FLAG_USE_ALLOC_MEM == 1 causes the nfas/loader directive to be used
// to allocate init flag memory. Note that init flag is now in CLS.
#ifndef INIT_FLAG_USE_ALLOC_MEM
#define INIT_FLAG_USE_ALLOC_MEM 1
#endif
// when using alloc_mem, user can specify a fixed memory location at which to
// locate the init flag. otherwise loader will pick location
#ifndef INIT_FLAG_ALLOCATION_FIXED
#define INIT_FLAG_ALLOCATION_FIXED 0
#endif
// Set the initialization flag location
#ifndef INIT_FLAG_ADDR
#define INIT_FLAG_ADDR 0x00000020
#endif
// For linker, need to manually specify island here
#ifndef INIT_FLAG_ISLAND
#define INIT_FLAG_ISLAND i12
#endif
#if ( INIT_FLAG_USE_ALLOC_MEM == 1 )
    #if (INIT_FLAG_ALLOCATION_FIXED == 1 )
        .alloc_mem crypto_init_flag INIT_FLAG_ISLAND.cls+INIT_FLAG_ADDR global 4 4 addr40
    #else
        .alloc_mem crypto_init_flag INIT_FLAG_ISLAND.cls global 4 4 addr40
    #endif
#else
    #define crypto_init_flag INIT_FLAG_ADDR
#endif

// enable use of SA 'flush' mechanism
#ifndef ENABLE_SA_FLUSH
#define ENABLE_SA_FLUSH     0
#endif
#if ( ENABLE_SA_FLUSH == 1 )
// SA_FLUSH_FLAGS_USE_ALLOC_MEM == 1 causes the nfas/loader directive to be used
// to allocate sa flush flags memory. Note that flags are now in CLS.
#ifndef SA_FLUSH_FLAGS_USE_ALLOC_MEM
#define SA_FLUSH_FLAGS_USE_ALLOC_MEM 1
#endif
// when using alloc_mem, user can specify a fixed memory location at which to
// locate the sa flush flags. otherwise loader will pick location
#ifndef SA_FLUSH_FLAGS_ALLOCATION_FIXED
#define SA_FLUSH_FLAGS_ALLOCATION_FIXED 0
#endif
// Set the sa flush flags location
#ifndef SA_FLUSH_FLAGS_ADDR
#define SA_FLUSH_FLAGS_ADDR 0x00000024
#endif
#if ( SA_FLUSH_FLAGS_USE_ALLOC_MEM == 1 )
    #if (SA_FLUSH_FLAGS_ALLOCATION_FIXED == 1 )
        .alloc_mem crypto_sa_flush_flags cls+SA_FLUSH_FLAGS_ADDR island 4 4 addr32
    #else
        .alloc_mem crypto_sa_flush_flags cls island 4 4 addr32
    #endif
#else
    #define crypto_sa_flush_flags SA_FLUSH_FLAGS_ADDR
#endif
#endif // ENABLE_SA_FLUSH == 1


// REQ_RING_TYPE is one of CRYPTO_RING_CTM, EMU0, EMU1, or EMU2
#ifndef REQ_RING_TYPE
#define REQ_RING_TYPE CRYPTO_RING_CTM
#endif
// REQ_RING_USE_ONE_EMU_RING is for running with multiple
// crypto islands, all using a single req ring in emu
#ifndef REQ_RING_USE_ONE_EMU_RING
#define REQ_RING_USE_ONE_EMU_RING 0
#endif
// REQ_RING_SIZE is one of CRYPTO_RING_SIZE_N, in lwords
#ifndef REQ_RING_SIZE
#define REQ_RING_SIZE CRYPTO_RING_SIZE_2K
#endif
// REQ_RING_NUM is the ring number, 0-13 for CTM or 0-1024 for EMU
#ifndef REQ_RING_NUM
#define REQ_RING_NUM 0
#endif
// REQ_RING_USE_ALLOC_MEM == 1 causes the nfas/loader directive to be used
// to allocate request ring memory
#ifndef REQ_RING_USE_ALLOC_MEM
#define REQ_RING_USE_ALLOC_MEM 1
#endif
// when using alloc_mem, user can specify a fixed memory location at which to
// locate the request ring. otherwise loader will pick location. has to be ring
// size aligned
#ifndef REQ_RING_ALLOCATION_FIXED
#define REQ_RING_ALLOCATION_FIXED 0
#endif
// Set the request ring location
#ifndef REQ_RING_ADDR
#define REQ_RING_ADDR 0x00000000
#endif
#ifndef REQ_RING_DESC_ADDR
#define_eval REQ_RING_DESC_ADDR (REQ_RING_ADDR + (REQ_RING_SIZE << 2))
#endif
#if ( REQ_RING_USE_ALLOC_MEM == 1 )
    #define_eval _SIZE (REQ_RING_SIZE << 2) // in bytes here
    #if (REQ_RING_ALLOCATION_FIXED == 1 )
        #define_eval _ADDR REQ_RING_ADDR
        #if ( REQ_RING_TYPE == CRYPTO_RING_CTM )
            .alloc_mem crypto_req_ring ctm+_ADDR island \
                        _SIZE _SIZE addr32
        #elif ( REQ_RING_TYPE == CRYPTO_RING_EMU0 )
            .alloc_mem crypto_req_ring emem0+_ADDR global \
                        _SIZE _SIZE addr40
            .alloc_mem crypto_req_ring_desc emem0+_ADDR global \
                        16 16 addr40
        #elif ( REQ_RING_TYPE == CRYPTO_RING_EMU1 )
            .alloc_mem crypto_req_ring emem1+_ADDR global \
                        _SIZE _SIZE addr40
            .alloc_mem crypto_req_ring_desc emem1+_ADDR global \
                        16 16 addr40
        #else ( REQ_RING_TYPE == CRYPTO_RING_EMU2 )
            .alloc_mem crypto_req_ring emem2+_ADDR global \
                        _SIZE _SIZE addr40
            .alloc_mem crypto_req_ring_desc emem2+_ADDR global \
                        16 16 addr40
        #endif
        #undef _ADDR
    #else
        #if ( REQ_RING_TYPE == CRYPTO_RING_CTM )
            .alloc_mem crypto_req_ring ctm island \
                        _SIZE _SIZE addr32
        #elif ( REQ_RING_TYPE == CRYPTO_RING_EMU0 )
            .alloc_mem crypto_req_ring emem0 global \
                        _SIZE _SIZE addr40
            .alloc_mem crypto_req_ring_desc emem0 global \
                        16 16 addr40
        #elif ( REQ_RING_TYPE == CRYPTO_RING_EMU1 )
            .alloc_mem crypto_req_ring emem1 global \
                        _SIZE _SIZE addr40
            .alloc_mem crypto_req_ring_desc emem1 global \
                        16 16 addr40
        #else ( REQ_RING_TYPE == CRYPTO_RING_EMU2 )
            .alloc_mem crypto_req_ring emem2 global \
                        _SIZE _SIZE addr40
            .alloc_mem crypto_req_ring_desc emem2 global \
                        16 16 addr40
        #endif
    #endif
#else
    #define crypto_req_ring REQ_RING_ADDR
    #define crypto_req_ring_desc REQ_RING_DESC_ADDR
#endif


// override the default counter update code, in order to customize the behavior
// for a specific application. user should #define OVERRIDE_UPDATE_COUNTER 1 and
// define a #macro crypto_threads_update_counter () to handle counter updates,
// and a #macro crypto_threads_clear_counters() to clear the counters.
#ifndef OVERRIDE_UPDATE_COUNTER
#define OVERRIDE_UPDATE_COUNTER 0
#endif
// when using default counter update code, user can specify location of counters
// in either crypto island cls or ctm using #define COUNTER_MEMORY_TYPE.
// default value is cls.
#define COUNTER_MEMORY_TYPE_CLS 0
#define COUNTER_MEMORY_TYPE_CTM 1
#ifndef COUNTER_MEMORY_TYPE
#define COUNTER_MEMORY_TYPE COUNTER_MEMORY_TYPE_CLS
#endif
// COUNTER_USE_ALLOC_MEM == 1 causes the nfas/loader directive to be used
// to allocate counter memory
#ifndef COUNTER_USE_ALLOC_MEM
#define COUNTER_USE_ALLOC_MEM 1
#endif
// when using alloc_mem, user can specify a fixed memory location at which to
// locate the counters. otherwise loader will pick location
#ifndef COUNTER_ALLOCATION_FIXED
#define COUNTER_ALLOCATION_FIXED 0
#endif
// counter base address, when using COUNTER_ALLOCATION_FIXED, or when not using
// alloc_mem
#ifndef COUNTER_BASE_ADDR
#define COUNTER_BASE_ADDR 0x1000 // default starting location of counters
#endif                           // has to be aligned to counter memory size
// counters can be set to be 64 or 32 bits long, in crypto_lib_threads.h
// ( but there seems to be some problem with cls / 64 bit cntrs )
#if ( COUNTER_USE_ALLOC_MEM == 1 )
    #define_eval COUNTER_MEM_SIZE ((CRYPTO_COUNTER_LENGTH/8) * CRYPTO_CNTR_MAX)
    #if (COUNTER_ALLOCATION_FIXED == 1 )
        #if (COUNTER_MEMORY_TYPE == COUNTER_MEMORY_TYPE_CLS)
            .alloc_mem ces_mem_cntrs cls+COUNTER_BASE_ADDR island \
                COUNTER_MEM_SIZE COUNTER_MEM_SIZE addr32
        #elif (COUNTER_MEMORY_TYPE == COUNTER_MEMORY_TYPE_CTM)
            .alloc_mem ces_mem_cntrs ctm+COUNTER_BASE_ADDR island \
                COUNTER_MEM_SIZE COUNTER_MEM_SIZE addr32
        #endif
    #else
        #if (COUNTER_MEMORY_TYPE == COUNTER_MEMORY_TYPE_CLS)
            .alloc_mem ces_mem_cntrs cls island \
                COUNTER_MEM_SIZE COUNTER_MEM_SIZE addr32
        #elif (COUNTER_MEMORY_TYPE == COUNTER_MEMORY_TYPE_CTM)
            .alloc_mem ces_mem_cntrs ctm island \
                COUNTER_MEM_SIZE COUNTER_MEM_SIZE addr32
        #endif
    #endif
#else
    #define ces_mem_cntrs COUNTER_BASE_ADDR
#endif


// enable handling for jumbo packets
#ifndef ENABLE_JUMBO_PKTS
#define ENABLE_JUMBO_PKTS  1
#endif


// enable error checking
#ifndef ENABLE_ERROR_CHECKS
#define ENABLE_ERROR_CHECKS  0
#endif


// enable anti-replay handling
#ifndef ENABLE_ANTI_REPLAY
#define ENABLE_ANTI_REPLAY  1
#endif


// 'detailed' counter ( i.e. pkt types )
#ifndef ENABLE_DETAILED_COUNTERS
#define ENABLE_DETAILED_COUNTERS 0
#endif


// debug
#ifndef ENABLE_DEBUG_COUNTERS
#define ENABLE_DEBUG_COUNTERS 0
#endif

#ifndef HALT_ON_ERROR
#define HALT_ON_ERROR         0
#endif

/// @cond INTERNAL_MACROS


// min/max crypto ctx, used to reduce code size if not all in use
#define_eval CRYPTO_MIN_CTX CRYPTO_START_CTX
#define_eval CRYPTO_MAX_CTX (CRYPTO_START_CTX + (CRYPTO_NUM_THREADS / 2 ) - 1)


// enable chained commands to crypto cores
#define ENABLE_CHAIN_MODE 1
#if (ENABLE_CHAIN_MODE == 1 )
#define CHAINED          1
#define CHAINED_LAST     1
#define CHAINED_NOT_LAST 0
#else
#define CHAINED          0
#define CHAINED_LAST     0
#define CHAINED_NOT_LAST 0
#endif



/**
 *  Request ring entry format
 *
 *   Lword Bits  Description
 *
 *   0     31:24 Status Return Queue Address bits 39:32
 *   0     23:16 Packet Write Back Address 39:32
 *   0        15 Must be set to 1
 *   0        14 Decryption Option
 *               0 - transfer entire packet
 *               1 - transfer only encrypted/decrypted payload data
 *            13 Free Start of Packet Buffer
 *               0 - do not free start of packet buffer
 *               1 - free buffer on cmd completion
 *                   ( writes Start of Packet Address into ring
 *                     passed via parameters to crypto_threads_init )
 *   0        12 Test Flag
 *               1 - test in progress
 *               0 - test not in progress
 *   0     11:10 CTM Buffer Size
 *               0 - 256
 *               1 - 512
 *               2 - 1024
 *               3 - 2048
 *   0       9:0 Status Return Queue Que Number
 *
 *   1     31:24 SA (Security Association ) Address 39:32
 *   1     23:16 Start of Packet Address Bits 39:32
 *   1      15:8 Continuation of Packet Address Bits 39:32
 *   1       7:0  End of Packet Address Bits 39:32
 *
 *   2      31:9 SA Addr 31:9, bits 8:0 implied 0
 *   2       8:0 SA Version
 *
 *   3      31:0 Start of Packet Addr 31:0
 *
 *   4      31:0 Continuation of Packet Addr 31:0
 *
 *   5      31:0 End of Packet Addr 31:0
 *
 *   6      31:0 Packet Write Back Addr 31:0
 *
 *   7     31:16 Start of Packet Length ( Bytes )
 *   7      15:0 Continuation of Packet Length ( Bytes )
 *
 *   8     31:16 End of Packet Length ( Bytes )
 *   8      15:0 Packet Write Back Length ( Bytes )
 *
 *   9     31:16 Packet Write Back End of Packet Length ( Bytes )
 *   9      15:0 Packet Plaintext or Encrypted Text Length (Bytes)
 *                Including Pad Bytes
 *
 *   10    31:16 Authentication Header ( AH or ESP ) Offset
 *   10     15:0 IV Offset
 *
 *   11    31:16 Plaintext or Encrypted Text Offset
 *   11     15:0 Authentication Data Offset
 *
 *   12    31:0  SA Control Word
 *
 *   Offsets are byte offset from start of packet. If offset is > start of
 *   packet length, byte offset refers to a byte in the continuation of packet,
 *   or in end of packet if offset is > start of packet length + continuation of
 *   packet length
 */

// request ring entry constants

#define CRYPTO_REQ_LENGTH                    13    // 32 bit words
#define CRYPTO_REQ_ENTRY_VALID               $req[0]
#define CRYPTO_REQ_ENTRY_VALID_BIT           15
#define CRYPTO_REQ_DECRYPT_OPTION            $req[0]
#define CRYPTO_REQ_DECRYPT_OPTION_BIT        14
#define CRYPTO_REQ_FREE_BUF                  $req[0]
#define CRYPTO_REQ_FREE_BUF_BIT              13
#define CRYPTO_REQ_TEST_FLAG                 $req[0]
#define CRYPTO_REQ_TEST_FLAG_BIT             12
#define CRYPTO_REQ_SA_ADDR                   $req[2]
#define CRYPTO_REQ_SA_ADDR_UPPER             $req[1]
#define CRYPTO_REQ_SA_ADDR_UPPER_SHIFT       24
#define CRYPTO_REQ_SA_VERSION_MASK           0x1ff
#define CRYPTO_REQ_PKT_STRT_ADDR             $req[3]
#define CRYPTO_REQ_PKT_STRT_ADDR_UPPER       $req[1]
#define CRYPTO_REQ_PKT_STRT_ADDR_UPPER_SHIFT 16
#define CRYPTO_REQ_PKT_CONT_ADDR             $req[4]
#define CRYPTO_REQ_PKT_CONT_ADDR_UPPER       $req[1]
#define CRYPTO_REQ_PKT_CONT_ADDR_UPPER_SHIFT 8
#define CRYPTO_REQ_PKT_END_ADDR              $req[5]
#define CRYPTO_REQ_PKT_END_ADDR_UPPER        $req[1]
#define CRYPTO_REQ_PKT_END_ADDR_UPPER_SHIFT  0
#define CRYPTO_REQ_PKT_WRBK_ADDR             $req[6]
#define CRYPTO_REQ_PKT_WRBK_ADDR_UPPER       $req[0]
#define CRYPTO_REQ_PKT_WRBK_ADDR_UPPER_SHIFT 16
#define CRYPTO_REQ_PKT_STRT_LENGTH           $req[7]
#define CRYPTO_REQ_PKT_STRT_LENGTH_SHIFT     16
#define CRYPTO_REQ_PKT_CONT_LENGTH           $req[7]
#define CRYPTO_REQ_PKT_CONT_LENGTH_SHIFT     0
#define CRYPTO_REQ_PKT_END_LENGTH            $req[8]
#define CRYPTO_REQ_PKT_END_LENGTH_SHIFT      16
#define CRYPTO_REQ_PKT_WRBK_LENGTH           $req[8]
#define CRYPTO_REQ_PKT_WRBK_LENGTH_SHIFT     0
#define CRYPTO_REQ_PKT_WRBK_END_LENGTH       $req[9]
#define CRYPTO_REQ_PKT_WRBK_END_LENGTH_SHIFT 16
#define CRYPTO_REQ_PTEXT_LENGTH              $req[9]
#define CRYPTO_REQ_PTEXT_LENGTH_SHIFT        0
#define CRYPTO_REQ_AUTH_HDR_OFFSET           $req[10]
#define CRYPTO_REQ_AUTH_HDR_OFFSET_SHIFT     16
#define CRYPTO_REQ_IV_OFFSET                 $req[10]
#define CRYPTO_REQ_IV_OFFSET_SHIFT           0
#define CRYPTO_REQ_PTEXT_OFFSET              $req[11]
#define CRYPTO_REQ_PTEXT_OFFSET_SHIFT        16
#define CRYPTO_REQ_AUTH_DATA_OFFSET          $req[11]
#define CRYPTO_REQ_AUTH_DATA_OFFSET_SHIFT    0
#define CRYPTO_REQ_SA_CONTROL_WORD           $req[12]

/**
 *  Response ring entry format
 *
 *   Lword 0: 31    Must be 1
 *   Lword 0: 30    Direction
 *                  0 = Encrypt, 1 = Decrypt
 *   Lword 0: 29:26 Spare
 *   Lword 0: 25:24 CTM Buffer Size, pass through from request
 *   Lword 0: 23:16 Packet Write Back Address Bits 39:32
 *   Lword 0: 15:8  End of Packet Address Bits 39:32
 *   Lword 0: 7:0   Status Return Value one of CRYPTO_RSP_STATUS_*
 *   Lword 1: 31:0  Packet Write Back Addr 31:0
 *   Lword 2: 31:0  End of Packet Addr 31:0
 *   Lword 3: 31:16 End of Packet Length ( Bytes )
 *   Lword 3: 15:0  Packet Write Back Length ( Bytes )
 *   Lword 4: 31:9  SA Address 31:9, Bits 8:0 implied 0
 *   Lword 4: 8:0   SA Version
 *   Lword 5: 31:8  Spare
 *   Lword 5: 7:0   Next Header Value
 */


// response ring entry constants

#define CRYPTO_RSP_LENGTH 6

// response status codes in crypto_lib_threads.h


/**
 *  SA (Security Association) Format
 *
 *   Lword 0-7:      Cipher Key ( 8 lwords, up to 32 bytes )
 *   Lword 8-23:     Authentication Key ( 16 lwords, up to 64 bytes )
 *   Lword 24:       SA Control Word
 *   Lword 24: 31:22 Spare
 *   Lword 24: 21:   Anti-replay window size 1:64 0:32
 *   Lword 24: 20:   Sequence Number Size 1:64 0:32
 *   Lword 24: 19:   Enable Anti-replay handling if 1
 *   Lword 24: 18:17 Unused by this code
 *   Lword 24: 16    Direction, one of CRYPTO_DIR_*
 *   Lword 24: 15:14 Protocol, one of CRYPTO_PROTOCOL_*
 *   Lword 24: 13:12 Mode, one of CRYPTO_PROTMODE_*
 *   Lword 24: 11:8  Cipher, one of CRYPTO_CIPHER_*
 *   Lword 24:  7:4  Cipher Mode, one of CRYPTO_CIMODE_*
 *   Lword 24:  3:0  Hash, one of CRYPTO_HASH_*
 *   Lword 25: 31:0  SPI (Security Parameters Index)
 *   Lword 26-63    Unused by this code
 *   Lword 64       Sequence Number 31:0
 *   Lword 65       Sequence Number 63:32
 *   Lword 66:      Anti-replay window counter 31:0
 *   Lword 67:      Anti-replay window counter 63:32
 *   Lword 68:      Anti-replay window bitmap 31:0
 *   Lword 69:      Anti-replay window bitmap 63:32
 *   Lword 70: 31:1 Unused by this code
 *   Lword 70: 0:   Anti-replay SA lock bit
 *   Lword 71-72    Unused by this code
 *   Lword 73:      Authentication Check Failure Counter
 *   Lword 74:      Mode, Cipher, or Hash Selection Error Counter
 *   Lword 75:      Other Internally Detected Error Counter
 *   Lword 76-78:   Unused by this code
 *   Lword 79:      Anti-replay sequence number too small
 *   Lword 80:      Anti-replay sequence number already received
 *   Lword 81-128:  Unused by this code
 *
 *   NOTE:
 *   Lwords 0-63   may only be accessed using bulk memory operations
 *   Lwords 64-128 may only be accessed using atomic memory operations
 *
 *   CRYPTO_DIR_*, CRYPTO_CIPHER_*, CRYPTO_CIMODE_*, CRYPTO_HASH_*
 *   from crypto_lib.uc
 */

// SA Constants

// field values
#define CRYPTO_PROTOCOL_AH  0
#define CRYPTO_PROTOCOL_ESP 1
#define CRYPTO_PROTMODE_TRANSPORT 0
#define CRYPTO_PROTMODE_TUNNEL    1

// bit positions within lwords
#define CRYPTO_SA_AR_WIN_64_MASK 0x1
#define CRYPTO_SA_AR_WIN_64_SHIFT 21
#define CRYPTO_SA_SEQNUM_64_MASK 0x1
#define CRYPTO_SA_SEQNUM_64_SHIFT 20
#define CRYPTO_SA_AR_ENA_MASK    0x1
#define CRYPTO_SA_AR_ENA_SHIFT    19
#define CRYPTO_SA_DIR_MASK       0x1
#define CRYPTO_SA_DIR_SHIFT      16
#define CRYPTO_SA_PROTOCOL_MASK  0x3
#define CRYPTO_SA_PROTOCOL_SHIFT 14
#define CRYPTO_SA_PROTMODE_MASK  0x3
#define CRYPTO_SA_PROTMODE_SHIFT 12
#define CRYPTO_SA_CIPHER_MASK    0xf
#define CRYPTO_SA_CIPHER_SHIFT   8
#define CRYPTO_SA_CIMODE_MASK    0xf
#define CRYPTO_SA_CIMODE_SHIFT   4
#define CRYPTO_SA_HASH_MASK      0xf
#define CRYPTO_SA_HASH_SHIFT     0
#define CRYPTO_SA_AR_LOCK_MASK   0x1
#define CRYPTO_SA_AR_LOCK_SHIFT  0

// Lword offsets to words in the SA
// these should be accessed only with bulk memory ops
#define CRYPTO_SA_CTLWRD_OFFSET   24
#define CRYPTO_SA_KEYS_OFFSET      0

// these should be accessed only with atomic memory ops
#define CRYPTO_SA_AR_CNTR_LOW     66
#define CRYPTO_SA_AR_CNTR_HIGH    67
#define CRYPTO_SA_AR_BMAP_LOW     68
#define CRYPTO_SA_AR_BMAP_HIGH    69
#define CRYPTO_SA_AR_LOCK_OFFSET  70
#define CRYPTO_SA_AUTH_FAIL_CNTR  73
#define CRYPTO_SA_SELECT_ERR_CNTR 74
#define CRYPTO_SA_OTHER_ERR_CNTR  75
#define CRYPTO_SA_AR_SEQLOW_CNTR  79
#define CRYPTO_SA_AR_SEQRCV_CNTR  80


/**
 *  Crypto SRAM Layout ( location in hex):
 *
 *  Core/Ctx      0     1     2     3     4     5
 *  Temp       0100  0200  0300  0400  0500  0600  // temp vars
 *  SA 0       0800  1000  1800  2000  2800  3000  // SA Data ( 256B )
 *  Vars 0     0900  1100  1900  2100  2900  3100  // Variables ( 128B )
 *  ICV/HMac 0 0980  1180  1980  2180  2980  3180  // ICV/HMAC ( 128B )
 *  SA 1       0A00  1200  1A00  2200  2A00  3200  // SA Data ( 256B )
 *  Vars 1     0B00  1300  1B00  2300  2B00  3300  // Variables ( 128B )
 *  ICV/HMac 1 0B80  1380  1B80  2380  2B80  3380  // ICV/HMAC ( 128B )
 *  SA 2       0C00  1400  1C00  2400  2C00  3400  // SA Data ( 256B )
 *  Vars 2     0D00  1500  1D00  2500  2D00  3500  // Variables ( 128B )
 *  ICV/HMac 2 0D80  1580  1D80  2580  2D80  3580  // ICV/HMAC ( 128B )
 *  SA 3       0E00  1600  1E00  2600  2E00  3600  // SA Data ( 256B )
 *  Vars 3     0F00  1700  1F00  2700  2F00  3700  // Variables ( 128B )
 *  ICV/HMac 3 0F80  1780  1F80  2780  2F80  3780  // ICV/HMAC ( 128B )
 *  Pkt Buff 0 4000  6000  8000  A000  C000  E000  // packet data
 *  Pkt Buff 1 4800  6800  8800  A800  C800  E800  // packet data
 *  Pkt Buff 2 5000  7000  9000  B000  D000  F000  // packet data
 *  Pkt Buff 3 5800  7800  9800  B800  D800  F800  // packet data
 */

// Sram Constants

#define CRYPTO_SA_LEN             0x100 // reserved area in sram
#define CRYPTO_SA_DMA_KEYS_LEN    0x060 // key data dma len ( 32 + 64 bytes )
#define CRYPTO_SA_DMA_KEYS_OFFSET 0x020 // key data offset in sram sa
#define CRYPTO_SA_START           0x800 // start of sa area for core/ctx 0
#define CRYPTO_SA_BLOCK_SIZE      0x200 // length between start of sa's
#define CRYPTO_TEMP_START         0x100 // start of temp area for core/ctx 0
#define CRYPTO_TEMP_LEN           0x100 // length of temp area
#define CRYPTO_NUM_BUFS               4 // number of buffers / core used (1-4)
#ifndef CRYPTO_MAX_SRAM_BUF_BYTES       // ( enable overide of max bytes for testing )
#define CRYPTO_MAX_SRAM_BUF_BYTES  2048 // maximum bytes to use in sram pkt buf
#endif                                  // needs to be an 8 byte multiple

#define crypto_threads_get_temp_addr_const(_crypto_ctx) (CRYPTO_TEMP_START + \
                                                 (_crypto_ctx * CRYPTO_TEMP_LEN))
#define crypto_threads_get_SA_addr_const(_crypto_ctx)   (CRYPTO_SA_START   + \
                                        (_crypto_ctx * (4*CRYPTO_SA_BLOCK_SIZE)))

/**
 *  Local Memory Use ( hex addr, 64 lwords per buffer allocated ):
 *
 *  ME Threads  0/1   2/3   4/5   6/7
 *  Buffer 0   0000  0400  0800  0C00  // buffer set 0
 *  Buffer 1   0100  0500  0900  0D00  // buffer set 1
 *  Buffer 2   0200  0600  0A00  0E00  // buffer set 2
 *  Buffer 3   0300  0700  0B00  0F00  // buffer set 3
 *
 *  Local Memory format:
 *
 *  0x0000: Request Ring Entry Lword 0   ( Addr bits 39:32, stat rtrn que # )
 *  0x0004: Request Ring Entry Lword 1   ( Address bits 39:32, see above )
 *  0x0008: Request Ring Entry Lword 2   ( SA Addr )
 *  0x000C: Request Ring Entry Lword 3   ( Start of Packet Addr )
 *  0x0010: Request Ring Entry Lword 4   ( Continuation of Packet Addr )
 *  0x0014: Request Ring Entry Lword 5   ( End of Packet Addr )
 *  0x0018: Request Ring Entry Lword 6   ( Write Back Packet Addr )
 *  0x001C: Request Ring Entry Lword 7   ( Start/Continue Packet Length )
 *  0x0020: Request Ring Entry Lword 8   ( End/Write Back Packet Length )
 *  0x0024: Request Ring Entry Lword 9   ( Write Back End Length/Text Length )
 *  0x0028: Request Ring Entry Lword 10  ( Auth Hdr Offset/ IV Offset )
 *  0x002C: Request Ring Entry Lword 11  ( Text Offset / Auth Data Offset )
 *  0x0030: Security Association Ctl Wrd ( see above for format )
 *  0x0034: ICV/Auth Data Length         ( a.k.a. hash length )
 *  0x0038: Jumbo State                  ( jumbo processing state )
 *  0x003C: Jumbo Pkt Plaintext Addr     ( jumbo 'plaintext' addr in sram )
 *  0x0040: Jumbo Pkt Auth Data Addr     ( jumbo pkt auth data addr in sram )
 *  0x0044: Jumbo Pkt End Addr           ( jumbo pkt end addr in memory )
 *  0x0048: Pkt Sequence Num 31:0        ( sequence # from auth hdr low )
 *  0x004C: Pkt Sequence Num 63:32       ( sequence # from auth hdr high )
 *
 *  Common Local Memory Locations used across all 4 buffers,
 *  offsets are from start of Buffer 0 ( i.e. 0x000, 0x400, 0x800, 0xC00 )
 *
 *  Following are used in the input thread loop while processing a jumbo
 *  packet; mainly because ran out of GPRs to do this. The equivalent var
 *  in the buffer specific local mem area is for output thread use
 *  0x00C0: Jumbo State                  ( jumbo processing state )
 *  0x00C4: Jumbo Pkt End Offset         ( jumbo working end of pkt offset )
 *  0x00C8: Jumbo Pkt Plaintext Offset   ( jumbo 'plaintext' working offset )
 *  0x00CC: Jumbo Pkt Wrtback End Offset ( jumbo working wrtback end offset )
 *  0x00D0: Jumbo Pkt Hash Key Addr      ( jumbo addr of hash key in use )
 *
 *  Following used to retain value in case packet runs across multiple
 *  buffers ( i.e. jumbo pkt )
 *  0x00E0: Packet Next Hdr Value        ( next header field from packet )
 */
#define CRYPTO_LM_SA_ADDR                   0x08
#define CRYPTO_LM_PKT_STRT_ADDR             0x0C
#define CRYPTO_LM_PKT_CONT_ADDR             0x10
#define CRYPTO_LM_PKT_CONT_ADDR_UPPER       0x04
#define CRYPTO_LM_PKT_CONT_ADDR_UPPER_SHIFT 8
#define CRYPTO_LM_PKT_END_ADDR              0x14
#define CRYPTO_LM_PKT_END_ADDR_UPPER        0x04
#define CRYPTO_LM_PKT_END_ADDR_UPPER_SHIFT  0
#define CRYPTO_LM_PKT_WRBK_ADDR             0x18
#define CRYPTO_LM_PKT_WRBK_ADDR_UPPER       0x00
#define CRYPTO_LM_PKT_WRBK_ADDR_UPPER_SHIFT 16
#define CRYPTO_LM_PKT_STRT_LENGTH           0x1C
#define CRYPTO_LM_PKT_STRT_LENGTH_SHIFT     16
#define CRYPTO_LM_PKT_CONT_LENGTH           0x1C
#define CRYPTO_LM_PKT_CONT_LENGTH_SHIFT     0
#define CRYPTO_LM_PKT_END_LENGTH            0x20
#define CRYPTO_LM_PKT_END_LENGTH_SHIFT      16
#define CRYPTO_LM_PKT_WRBK_LENGTH           0x20
#define CRYPTO_LM_PKT_WRBK_LENGTH_SHIFT     0
#define CRYPTO_LM_PKT_WRBK_END_LENGTH       0x24
#define CRYPTO_LM_PKT_WRBK_END_LENGTH_SHIFT 16
#define CRYPTO_LM_PKT_TEXT_LENGTH           0x24
#define CRYPTO_LM_PKT_TEXT_LENGTH_SHIFT     0
#define CRYPTO_LM_AUTH_HDR_OFFSET           0x28
#define CRYPTO_LM_AUTH_HDR_OFFSET_SHIFT     16
#define CRYPTO_LM_IV_OFFSET                 0x28
#define CRYPTO_LM_IV_OFFSET_SHIFT           0
#define CRYPTO_LM_PTEXT_OFFSET              0x2C
#define CRYPTO_LM_PTEXT_OFFSET_SHIFT        16
#define CRYPTO_LM_AUTH_DATA_OFFSET          0x2C
#define CRYPTO_LM_AUTH_DATA_OFFSET_SHIFT    0
#define CRYPTO_LM_SA_CTLWRD                 0x30
#define CRYPTO_LM_HASH_LENGTH               0x34
#define CRYPTO_LM_STATUS_QUE_SHIFT          0
#define CRYPTO_LM_STATUS_QUE_MASK           0x3ff
#define CRYPTO_LM_JUMBO_STATE               0x38
#define CRYPTO_LM_JUMBO_BUF_SRAM_PTEXT      0x3C
#define CRYPTO_LM_JUMBO_BUF_SRAM_AUTH_DATA  0x40
#define CRYPTO_LM_JUMBO_BUF_END_ADDR        0x44
#define CRYPTO_LM_PKT_SEQ_NUM_LOW           0x48
#define CRYPTO_LM_PKT_SEQ_NUM_HIGH          0x4C
#define CRYPTO_LM_COMM_JUMBO_STATE          0xC0
#define CRYPTO_LM_COMM_JUMBO_PKT_END_OFF    0xC4
#define CRYPTO_LM_COMM_JUMBO_PKT_PTEXT_OFF  0xC8
#define CRYPTO_LM_COMM_JUMBO_PKT_WRBK_END_OFF 0xCC
#define CRYPTO_LM_COMM_JUMBO_HKEY_ADDR      0xD0
#define CRYPTO_LM_COMM_NXT_HDR_VAL          0xE0

// jumbo packet 'state' values
#define CRYPTO_LM_JUMBO_STATE_NON_JUMBO     0x00
#define CRYPTO_LM_JUMBO_STATE_FIRST_BUF     0x01
#define CRYPTO_LM_JUMBO_STATE_CONTINUE_BUF  0x02
#define CRYPTO_LM_JUMBO_STATE_END_BUF       0x03


// counter offsets are in crypto_lib_threads.h

#if (OVERRIDE_UPDATE_COUNTER == 0 )

#macro crypto_threads_clear_counters()
.begin
    .sig s
    .reg r_cnt, r_xfr_cnt, r_addr, r_addr_inc
    .reg $xfr[4]
    .xfer_order $xfr

    move($xfr[0], 0)
    move($xfr[1], 0)
    move($xfr[2], 0)
    move($xfr[3], 0)

    #if ( CRYPTO_COUNTER_LENGTH == 64 )
    #define_eval _LWORD_COUNT ( CRYPTO_CNTR_LAST << 1 )
    #else
    #define_eval _LWORD_COUNT ( CRYPTO_CNTR_LAST << 0 )
    #endif
    move(r_cnt, _LWORD_COUNT)
    #undef _LWORD_COUNT
    move(r_xfr_cnt, 4)
    move(r_addr, ces_mem_cntrs)

    .repeat

        limit_min(r_xfr_cnt, r_xfr_cnt, r_cnt)
        #if ( COUNTER_MEMORY_TYPE == COUNTER_MEMORY_TYPE_CLS )
            alu[--, 0x80, OR, r_xfr_cnt, <<8]
            cls[write, $xfr[0], r_addr, 0, MAX_4], indirect_ref, ctx_swap[s]
        #else
            alu[--, 0x80, OR, r_xfr_cnt, <<8]
            mem[write32, $xfr[0], 0, <<8, r_addr, MAX_4], indirect_ref, ctx_swap[s]
        #endif
        alu[r_addr_inc, --, B, r_xfr_cnt, <<2]
        alu[r_addr, r_addr, +, r_addr_inc]
        alu[r_cnt, r_cnt, -, r_xfr_cnt]

    .until ( r_cnt <= 0 )

.end
#endm

#macro crypto_threads_update_counter(_cntr_num)
    #if ( is_ct_const(_cntr_num) && (_cntr_num < CRYPTO_CNTR_DEBUG_LAST) && \
                                               (ENABLE_DEBUG_COUNTERS == 0))
       // don't build in debug counters
    #elif ( is_ct_const(_cntr_num) )
        .begin
            #if ( CRYPTO_COUNTER_LENGTH == 64 )
                #define_eval _CNTR_OFFSET (_cntr_num << 3)
                #define_eval _INCR_CMD incr64
            #else
                #define_eval _CNTR_OFFSET (_cntr_num << 2)
                #define_eval _INCR_CMD incr
            #endif

            .reg r_ca
            #if ( COUNTER_MEMORY_TYPE == COUNTER_MEMORY_TYPE_CLS )
                immed[r_ca, (ces_mem_cntrs + _CNTR_OFFSET)] // ok since cls only 64K
                cls[_INCR_CMD, --, r_ca, 0]
            #else
                move(r_ca, (ces_mem_cntrs + _CNTR_OFFSET))
                mem[_INCR_CND, --, 0, <<8, r_ca]
            #endif
            #undef _CNTR_OFFSET
            #undef _INCR_CMD
        .end
    #else
        .begin
            #if ( CRYPTO_COUNTER_LENGTH == 64 )
                #define_eval _CNTR_OFFSET_SHIFT 3
                #define_eval _INCR_CMD incr64
            #else
                #define_eval _CNTR_OFFSET_SHIFT 2
                #define_eval _INCR_CMD incr
            #endif

            .reg r_ca
             move(r_ca, ces_mem_cntrs)
             alu[r_ca, r_ca, OR, _cntr_num, <<_CNTR_OFFSET_SHIFT]

            #if ( COUNTER_MEMORY_TYPE == COUNTER_MEMORY_TYPE_CLS )
                cls[_INCR_CMD, --, r_ca, 0]
            #else
                mem[_INCR_CND, --, 0, <<8, r_ca]
            #endif

            #undef _CNTR_OFFSET_SHIFT
            #undef _INCR_CMD
        .end
    #endif
#endm

#endif // OVERRIDE_UPDATE_COUNTER


/**
 *  Update error counters in SA ( security association )
 *
 *  @param  _sa_addr_upper GPR,CONST bits 39:32 of sa base address
 *                         in bits 7:0
 *  @param  _sa_addr       GPR bits 31:0 of sa base address
 *                         ( bits 8:0 are masked off to 0 )
 *  @param  _cntr_num      GPR, CONST counter lword offset in sa
 *
 *  assumes sa_addr is on an sa length alignment ( currently 32 lwords )
 */
#macro crypto_threads_update_sa_counter(_sa_addr_upper, _sa_addr, _cntr_num)
    .begin
        .reg r_tmpa, r_tmpb

        move(r_tmpa, CRYPTO_REQ_SA_VERSION_MASK) // mask off version bits
        alu[r_tmpb, _sa_addr, AND~, r_tmpa]
        alu[r_tmpb, r_tmpb, OR, _cntr_num, <<2] // offset to counter
        alu[r_tmpa, --, B, _sa_addr_upper, <<24]
        mem[incr, --, r_tmpa, <<8, r_tmpb] // don't wait for completion
    .end
#endm


/**
 *  Update packet counters for specific modes,
 *  ciphers, hashes.
 *
 *  @param  _sa_ctlwrd GPR, Control Word of SA used for packet
 */
#macro crypto_threads_update_pkt_counter(_sa_ctlwrd)
.begin

    .reg r_cntr, r_val

    // increment the protocol/prot mode counter
    move(r_cntr, CRYPTO_CNTR_PKTS_ENCRYPT_ESP_TUNNEL)
    // pick protocol
    ld_field_w_clr[r_val, 0001, _sa_ctlwrd, >>CRYPTO_SA_PROTOCOL_SHIFT]
    alu[r_val, r_val, AND, CRYPTO_SA_PROTOCOL_MASK]
    .if ( r_val == CRYPTO_PROTOCOL_AH )
        alu[r_cntr, r_cntr, +, 2]
    .endif
    // pick protocol mode
    ld_field_w_clr[r_val, 0001, _sa_ctlwrd, >>CRYPTO_SA_PROTMODE_SHIFT]
    alu[r_val, r_val, AND, CRYPTO_SA_PROTMODE_MASK]
    .if ( r_val == CRYPTO_PROTMODE_TRANSPORT )
        alu[r_cntr, r_cntr, +, 1]
    .endif
    // pick encrypt vs decrypt
    ld_field_w_clr[r_val, 0001, _sa_ctlwrd, >>CRYPTO_SA_DIR_SHIFT]
    alu[r_val, r_val, AND, CRYPTO_SA_DIR_MASK]
    .if ( r_val == CRYPTO_DIR_DECRYPT )
        alu[r_cntr, r_cntr, +, 4]
    .endif
    // update the counter
    crypto_threads_update_counter(r_cntr)


    // increment the cipher counter
    move(r_cntr, CRYPTO_CNTR_PKTS_ENCRYPT_NONE)
    // pick cipher
    ld_field_w_clr[r_val, 0001, _sa_ctlwrd, >>CRYPTO_SA_CIPHER_SHIFT]
    alu[r_val, r_val, AND, CRYPTO_SA_CIPHER_MASK]
    .if ( r_val == CRYPTO_CIPHER_AES_128 )
        move(r_cntr, CRYPTO_CNTR_PKTS_ENCRYPT_AES128)
    .elif ( r_val == CRYPTO_CIPHER_AES_192 )
        move(r_cntr, CRYPTO_CNTR_PKTS_ENCRYPT_AES192)
    .elif ( r_val == CRYPTO_CIPHER_AES_256 )
        move(r_cntr, CRYPTO_CNTR_PKTS_ENCRYPT_AES256)
    .elif ( r_val == CRYPTO_CIPHER_3DES )
        move(r_cntr, CRYPTO_CNTR_PKTS_ENCRYPT_3DES)
    .endif
    // pick encrypt vs decrypt
    ld_field_w_clr[r_val, 0001, _sa_ctlwrd, >>CRYPTO_SA_DIR_SHIFT]
    alu[r_val, r_val, AND, CRYPTO_SA_DIR_MASK]
    .if ( r_val == CRYPTO_DIR_DECRYPT )
        alu[r_cntr, r_cntr, +, 9]
    .endif
    // update the counter
    crypto_threads_update_counter(r_cntr)


    // increment the hash counter
    move(r_cntr, CRYPTO_CNTR_PKTS_ENCRYPT_HASH_NONE)
    // pick hash
    ld_field_w_clr[r_val, 0001, _sa_ctlwrd, >>CRYPTO_SA_HASH_SHIFT]
    alu[r_val, r_val, AND, CRYPTO_SA_HASH_MASK]
    .if ( r_val == CRYPTO_HASH_SHA_1 )
        move(r_cntr, CRYPTO_CNTR_PKTS_ENCRYPT_SHA1)
    .elif ( r_val == CRYPTO_HASH_SHA_256 )
        move(r_cntr, CRYPTO_CNTR_PKTS_ENCRYPT_SHA256)
    .elif ( r_val == CRYPTO_HASH_SHA_512 )
        move(r_cntr, CRYPTO_CNTR_PKTS_ENCRYPT_SHA512)
    .endif
    // pick encrypt vs decrypt
    ld_field_w_clr[r_val, 0001, _sa_ctlwrd, >>CRYPTO_SA_DIR_SHIFT]
    alu[r_val, r_val, AND, CRYPTO_SA_DIR_MASK]
    .if ( r_val == CRYPTO_DIR_DECRYPT )
        alu[r_cntr, r_cntr, +, 9]
    .endif
    // update the counter
    crypto_threads_update_counter(r_cntr)

.end
#endm


/**
 *  Dump debug vars to cls location 0
 *
 *  @param  _p1   1st reg param to dump
 *  @param  _p2   2nd reg param to dump
 *  @param  _p3   3rd reg param to dump
 *  @param  _p4   4th reg param to dump
 */
#macro cls_debug_dump4(_p1, _p2, _p3, _p4)
.begin
    .reg $x[4]
    .xfer_order $x
    .reg t
    .sig z
    move(t,0)
    move($x[0], _p1)
    move($x[1], _p2)
    move($x[2], _p3)
    move($x[3], _p4)
    cls[write, $x[0], 0, t, 4], ctx_swap[z]
.end
#endm

#macro cls_debug_dump2(_p1, _p2)
.begin
    .reg $x[2]
    .xfer_order $x
    .reg t
    .sig z
    move(t,0)
    move($x[0], _p1)
    move($x[1], _p2)
    cls[write, $x[0], 0, t, 2], ctx_swap[z]
.end
#endm


/**
 *  Get Sram Temp address, based on sram layout shown above
 *
 *  @param  _temp_addr  GPR, returned sram address of start of temp area
 *  @param  _crypto_ctx GPR, crypto context ( 0 through 5 )
 */
#macro crypto_threads_get_temp_addr(_temp_addr, _crypto_ctx)
    alu[_temp_addr, _crypto_ctx, +, 1]
    alu[_temp_addr, --, B, _temp_addr, <<8]
#endm


/**
 *  Get Sram SA Buffer address, based on sram layout shown above
 *
 *  @param  _sa_addr    GPR, returned sram address of start of sa
 *  @param  _crypto_ctx GPR, crypto context ( 0 through 5 )
 *  @param  _buf_sel    GPR, buffer ( 0 through 3 )
 */
#macro crypto_threads_get_sa_addr(_sa_addr, _crypto_ctx, _buf_sel)
    alu[_sa_addr, _crypto_ctx, +, 1]
    alu[_sa_addr, --, B, _sa_addr, <<11]
    alu[_sa_addr, _sa_addr, OR, _buf_sel, <<9]
#endm


/**
 *  Get Sram Vars buffer address, based on sram layout shown above
 *
 *  @param  _vars_addr  GPR, returned sram address of start of vars
 *  @param  _crypto_ctx GPR, crypto context ( 0 through 5 )
 *  @param  _buf_sel    GPR, buffer ( 0 through 3 )
 */
#macro crypto_threads_get_vars_addr(_vars_addr, _crypto_ctx, _buf_sel)
    crypto_threads_get_sa_addr(_vars_addr, _crypto_ctx, _buf_sel)
    alu[_vars_addr, _vars_addr, OR, 1, <<8]
#endm


/**
 *  Get Sram ICV/Auth Data ( a.k.a. hash value) buffer address, based on sram
 *  layout shown above
 *
 *  @param  _hash_addr  GPR, returned sram address of start of icv/auth data/hash
 *  @param  _crypto_ctx GPR, crypto context ( 0 through 5 )
 *  @param  _buf_sel    GPR, buffer ( 0 through 3 )
 */
#macro crypto_threads_get_hash_addr(_hash_addr, _crypto_ctx, _buf_sel)
    crypto_threads_get_sa_addr(_hash_addr, _crypto_ctx, _buf_sel)
    alu[_hash_addr, _hash_addr, OR, 0x18, <<4]
#endm


/**
 *  Get Sram Authentication Key buffer address, based on sram
 *  layout shown above
 *
 *  @param  _hash_key_addr GPR, returned sram address of icv/auth data/hash
 *  @param  _crypto_ctx    GPR, crypto context ( 0 through 5 )
 *  @param  _buf_sel       GPR, buffer ( 0 through 3 )
 */
#macro crypto_threads_get_hash_key_addr(_hash_key_addr, _crypto_ctx, _buf_sel)
    crypto_threads_get_sa_addr(_hash_key_addr, _crypto_ctx, _buf_sel)
    alu[_hash_key_addr, _hash_key_addr, OR, 0x0C, <<2]
#endm


/**
 *  Get Sram Packet Buffer address, based on sram layout shown above
 *
 *  @param  _pkt_addr   GPR, returned sram address of start of pkt buffer
 *  @param  _crypto_ctx GPR, crypto context ( 0 through 5 )
 *  @param  _buf_sel    GPR, buffer ( 0 through 3 )
 */
#macro crypto_threads_get_pkt_addr(_pkt_addr, _crypto_ctx, _buf_sel)
    alu[_pkt_addr, --, B, _crypto_ctx, <<1]
    alu[_pkt_addr, _pkt_addr, +, 4]
    alu[_pkt_addr, --, B, _pkt_addr, <<12]
    alu[_pkt_addr, _pkt_addr, OR, _buf_sel, <<11]
#endm


/**
 *  Get Local Memory Temp address, based on local memory layout shown above
 *
 *  @param  _temp_addr  GPR, returned local memory address of start of temp area
 *  @param  _me_ctx     GPR, me context ( 0 through 7 )
 *  @param  _buf_sel    GPR, buffer ( 0 through 3 )
 */
#macro crypto_threads_get_lm_temp_addr(_temp_addr, _me_ctx, _buf_sel)
    alu[_temp_addr, 0xc, AND, _me_ctx, <<1]
    alu[_temp_addr, _temp_addr, +, _buf_sel]
    alu[_temp_addr, --, B, _temp_addr, <<8]
#endm


/**
 *  Save Next Hdr value from sram to Local Memory 'Common' location
 *
 *  @param  _me_ctx     GPR, me context ( 0 through 7 )
 *  @param  _sram_addr  GPR, byte address in sram with next hdr
 */
#macro crypto_threads_save_nxt_hdr( _me_ctx, _sram_addr )
.begin

    .sig snhs
    .reg r_temp, r_temp1
    .reg $xd[2]
    .xfer_order $xd

    // read 8 byte block containing nxt hdr field
    alu[r_temp, _sram_addr, AND~, 7]
    crypto[read, $xd[0], 0, r_temp, 1], ctx_swap[snhs]

    // extract the next hdr byte
    alu[r_temp, _sram_addr, AND, 7]
    jump[r_temp, enhjt#], targets [enhj0#, enhj1#, enhj2#, enhj3#, \
                                     enhj4#, enhj5#, enhj6#, enhj7# ]
    enhjt#:
    enhj0#: br[enhjt0#]
    enhj1#: br[enhjt1#]
    enhj2#: br[enhjt2#]
    enhj3#: br[enhjt3#]
    enhj4#: br[enhjt4#]
    enhj5#: br[enhjt5#]
    enhj6#: br[enhjt6#]
    enhj7#: br[enhjt7#]

    enhjt0#:
        ld_field_w_clr[r_temp, 0001, $xd[0], >>24]
        br[enh_done#]
    enhjt1#:
        ld_field_w_clr[r_temp, 0001, $xd[0], >>16]
        br[enh_done#]
    enhjt2#:
        ld_field_w_clr[r_temp, 0001, $xd[0], >>8 ]
        br[enh_done#]
    enhjt3#:
        ld_field_w_clr[r_temp, 0001, $xd[0], >>0 ]
        br[enh_done#]
    enhjt4#:
        ld_field_w_clr[r_temp, 0001, $xd[1], >>24]
        br[enh_done#]
    enhjt5#:
        ld_field_w_clr[r_temp, 0001, $xd[1], >>16]
        br[enh_done#]
    enhjt6#:
        ld_field_w_clr[r_temp, 0001, $xd[1], >>8 ]
        br[enh_done#]
    enhjt7#:
        ld_field_w_clr[r_temp, 0001, $xd[1], >>0 ]

    enh_done#:
        crypto_threads_get_lm_temp_addr(r_temp1, _me_ctx, 0)
        localmem_write1(r_temp, \
                        r_temp1, \
                        CRYPTO_LM_COMM_NXT_HDR_VAL)
.end
#endm


/**
 *  Compare memory ranges in crypto sram
 *
 *  @param  _res     GPR, returns 1 if mismatch, 0 if match
 *  @param  _addr1   GPR, start of memory area 1 ( byte address, may be 1 byte
 *                        aligned )
 *  @param  _addr2   GPR, start of memory area 2 ( byte address, must be 8 byte
                          aligned )
 *  @param  _len     GPR, byte length to compare
 *
 *  Crypto read only operates on 8 byte aligned address, 8 byte length transfer.
 *  So need to assemble aligned longwords at _addr1 for compare to aligned
 *  data at _addr2
 */
#macro crypto_threads_compare(_res, _addr1, _addr2, _len)
.begin
    .sig ctcs
    .reg r_cnt, r_addra, r_da[2], r_db[2], r_msk[2], r_temp
    .reg $xfra[4]
    .xfer_order $xfra
    .reg $xfrb[16]
    .xfer_order $xfrb

    // read entire _addr2 ( aligned ) sram compare region
    // into transfer registers in one cmd
    .if ( _len & 0x7 )
        alu[r_temp, _len, +, 8] // round up # 8 byte xfrs
    .else
        move(r_temp, _len)
    .endif
    alu[r_temp, --, B, r_temp, >>3] // 8 bytes per transfer
    alu[r_temp, r_temp, -, 1] // 0 based for indirect
    alu[--, 0x80, OR, r_temp, <<8]
    crypto[read, $xfrb[0], 0, _addr2, MAX_8], indirect_ref, ctx_swap[ctcs]

    move(r_cnt, _len)
    move(r_addra, _addr1)

    .repeat

    // get next 16 bytes from addr a into 4 GPR's
    alu[r_tmp, r_addra, AND~,  0x7]
    crypto[read, $xfra[0], 0, r_tmp, 2], ctx_swap[ctcs]


    // align into GPR's, based on starting address byte
    alu[r_tmp, r_addra, AND, 7]
    jump[r_tmp, ctcjt#], targets [j0#, j1#, j2#, j3#, j4#, j5#, j6#, j7#]

    ctcjt#:
        j0#: br[ctcjt0#]
        j1#: br[ctcjt1#]
        j2#: br[ctcjt2#]
        j3#: br[ctcjt3#]
        j4#: br[ctcjt4#]
        j5#: br[ctcjt5#]
        j6#: br[ctcjt6#]
        j7#: br[ctcjt7#]

    ctcjt0#:
        move(r_da[0], $xfra[0])
        move(r_da[1], $xfra[1])
        br[ctc_rdb#]

    ctcjt1#:
        alu[r_da[0], --, B, $xfra[0], <<8]
        ld_field[r_da[0], 0001, $xfra[1], >>24]
        alu[r_da[1], --, B, $xfra[1], <<8]
        ld_field[r_da[1], 0001, $xfra[2], >>24]
        br[ctc_rdb#]

    ctcjt2#:
        alu[r_da[0], --, B, $xfra[0], <<16]
        ld_field[r_da[0], 0011, $xfra[1], >>16]
        alu[r_da[1], --, B, $xfra[1], <<16]
        ld_field[r_da[1], 0011, $xfra[2], >>16]
        br[ctc_rdb#]

    ctcjt3#:
        alu[r_da[0], --, B, $xfra[0], <<24]
        ld_field[r_da[0], 0111, $xfra[1], >>8]
        alu[r_da[1], --, B, $xfra[1], <<24]
        ld_field[r_da[1], 0111, $xfra[2], >>8]
        br[ctc_rdb#]

    ctcjt4#:
        alu[r_da[0], --, B, $xfra[1]]
        alu[r_da[1], --, B, $xfra[2]]
        br[ctc_rdb#]

    ctcjt5#:
        alu[r_da[0], --, B, $xfra[1], <<8]
        ld_field[r_da[0], 0001, $xfra[2], >>24]
        alu[r_da[1], --, B, $xfra[2], <<8]
        ld_field[r_da[1], 0001, $xfra[3], >>24]
        br[ctc_rdb#]

    ctcjt6#:
        alu[r_da[0], --, B, $xfra[1], <<16]
        ld_field[r_da[0], 0011, $xfra[2], >>16]
        alu[r_da[1], --, B, $xfra[2], <<16]
        ld_field[r_da[1], 0011, $xfra[3], >>16]
        br[ctc_rdb#]

    ctcjt7#:
        alu[r_da[0], --, B, $xfra[1], <<24]
        ld_field[r_da[0], 0111, $xfra[2], >>8]
        alu[r_da[1], --, B, $xfra[2], <<24]
        ld_field[r_da[1], 0111, $xfra[3], >>8]
        br[ctc_rdb#]


    // move 2 lwords read for aligned region b from xfr regs
    // into gprs for compare
    ctc_rdb#:

    alu[r_temp, _len, -, r_cnt]
    alu[r_temp, --, B, r_temp, >>3]
    jump[r_temp, ctcbjt#], targets [k0#, k1#, k2#, k3#, k4#, k5#, k6#, k7#]

    ctcbjt#:
        k0#: br[ctcbjt0#]
        k1#: br[ctcbjt1#]
        k2#: br[ctcbjt2#]
        k3#: br[ctcbjt3#]
        k4#: br[ctcbjt4#]
        k5#: br[ctcbjt5#]
        k6#: br[ctcbjt6#]
        k7#: br[ctcbjt7#]

    ctcbjt0#:
        move(r_db[0], $xfrb[0])
        move(r_db[1], $xfrb[1])
        br[ctc_genm#]

    ctcbjt1#:
        move(r_db[0], $xfrb[2])
        move(r_db[1], $xfrb[3])
        br[ctc_genm#]

    ctcbjt2#:
        move(r_db[0], $xfrb[4])
        move(r_db[1], $xfrb[5])
        br[ctc_genm#]

    ctcbjt3#:
        move(r_db[0], $xfrb[6])
        move(r_db[1], $xfrb[7])
        br[ctc_genm#]

    ctcbjt4#:
        move(r_db[0], $xfrb[8])
        move(r_db[1], $xfrb[9])
        br[ctc_genm#]

    ctcbjt5#:
        move(r_db[0], $xfrb[10])
        move(r_db[1], $xfrb[11])
        br[ctc_genm#]

    ctcbjt6#:
        move(r_db[0], $xfrb[12])
        move(r_db[1], $xfrb[13])
        br[ctc_genm#]

    ctcbjt7#:
        move(r_db[0], $xfrb[14])
        move(r_db[1], $xfrb[15])
        br[ctc_genm#]


    // generate mask to mask off unused bytes based on
    // remaining length
    ctc_genm#:

    .if ( r_cnt > 7 ) // compare all 8 bytes
        move(r_msk[0], 0xffffffff)
        move(r_msk[1], 0xffffffff)
    .else
        jump[r_cnt, ctcmjt#], targets [mj0#, mj1#, mj2#, mj3#, mj4#, mj5#, \
                                                                 mj6#, mj7#]

        ctcmjt#:
            mj0#: br[ctcmjt0#]
            mj1#: br[ctcmjt1#]
            mj2#: br[ctcmjt2#]
            mj3#: br[ctcmjt3#]
            mj4#: br[ctcmjt4#]
            mj5#: br[ctcmjt5#]
            mj6#: br[ctcmjt6#]
            mj7#: br[ctcmjt7#]

        ctcmjt0#:  // this shouldn't happen
            move(r_msk[0], 0x00000000)
            move(r_msk[1], 0x00000000)
            br[ctc_cmp#]

        ctcmjt1#:
            move(r_msk[0], 0xff000000)
            move(r_msk[1], 0x00000000)
            br[ctc_cmp#]

        ctcmjt2#:
            move(r_msk[0], 0xffff0000)
            move(r_msk[1], 0x00000000)
            br[ctc_cmp#]

        ctcmjt3#:
            move(r_msk[0], 0xffffff00)
            move(r_msk[1], 0x00000000)
            br[ctc_cmp#]

        ctcmjt4#:
            move(r_msk[0], 0xffffffff)
            move(r_msk[1], 0x00000000)
            br[ctc_cmp#]

        ctcmjt5#:
            move(r_msk[0], 0xffffffff)
            move(r_msk[1], 0xff000000)
            br[ctc_cmp#]

        ctcmjt6#:
            move(r_msk[0], 0xffffffff)
            move(r_msk[1], 0xffff0000)
            br[ctc_cmp#]

        ctcmjt7#:
            move(r_msk[0], 0xffffffff)
            move(r_msk[1], 0xffffff00)
    .endif

    // compare the two lwords
    ctc_cmp#:
    alu[r_da[0], r_da[0], XOR, r_db[0]]
    alu[--, r_da[0], AND, r_msk[0]]
    bne[crypto_threads_compare_failed#]

    alu[r_da[1], r_da[1], XOR, r_db[1]]
    alu[--, r_da[1], AND, r_msk[1]]
    bne[crypto_threads_compare_failed#]


    // next address, count
    alu[r_addra, r_addra, +, 8]
    alu[r_cnt, r_cnt, -, 8]

    .until ( r_cnt <= 0)


crypto_threads_compare_ok#:
    immed[_res, 0]
    br[crypto_threads_compare_done#]


crypto_threads_compare_failed#:
    immed[_res, 1]


crypto_threads_compare_done#:

.end
#endm


/**
 *  Clear memory range in local ctm
 *
 *  @param  _addr    GPR, start of memory area
 *  @param  _len     GPR, length to clear, # lwords
 *
 *  _len has to be a multiple of 32
 */
#macro crypto_threads_clr_ctm(_addr, _len)
.begin
    .reg $xfr[32]
    .xfer_order $xfr
    .sig z
    .reg r_addr, r_cnt, r_xfr_cnt

    move(r_cnt, 0)
    move(r_addr, _addr)
    move(r_xfr_cnt, 15)
    aggregate_zero($xfr, 32)

    .repeat
        alu[--, 0x80, OR, r_xfr_cnt, <<8]
        mem[write, $xfr[0], 0, <<8, r_addr, MAX_16], \
            indirect_ref, ctx_swap[z]

        r_cnt = r_cnt + 32
        r_addr = r_addr + 128
    .until ( r_cnt >= _len )
.end
#endm


/**
 *  Clear memory range in emem
 *
 *  @param  _addr    GPR, start of memory area
 *  @param  _len     GPR, length to clear, # lwords
 *
 *  _len has to be a multiple of 32
 */
#macro crypto_threads_clr_emem(_addr, _len)
.begin
    .reg $xfr[32]
    .xfer_order $xfr
    .sig z
    .reg r_addr_hi, r_addr_low, r_cnt, r_xfr_cnt

    move(r_cnt, 0)
    move(r_xfr_cnt, 15)
    aggregate_zero($xfr, 32)
    immed40(r_addr_hi, r_addr_low, _addr)

    .repeat
        alu[--, 0x80, OR, r_xfr_cnt, <<8]
        mem[write, $xfr[0], r_addr_hi, <<8, r_addr_low, MAX_16], \
            indirect_ref, ctx_swap[z]

        r_cnt = r_cnt + 32
        r_addr_low = r_addr_low + 128
    .until ( r_cnt >= _len )
.end
#endm


/**
 *  Clear Initialization complete flag.
 *  For now, supports a flag in CLS only
 */
#macro crypto_threads_clear_init_flag()
.begin
    .reg addr
    .reg $x
    .sig z
    move(addr, crypto_init_flag)
    move($x, 0x0)
    cls[write, $x, addr, 0, 1], ctx_swap[z]
.end
#endm


/**
 *  Set Initialization complete flag.
 *  For now, supports a flag in CLS only
 */
#macro crypto_threads_set_init_flag()
.begin
    .reg addr
    .reg $x
    .sig z
    move(addr, crypto_init_flag)
    move($x, 0x05170522)
    cls[write, $x, addr, 0, 1], ctx_swap[z]
.end
#endm


/**
 *  Wait for Initialization complete flag
 *  For now, supports a flag in CLS only
 */
#macro crypto_threads_wait_init_flag()
.begin
    .reg addr_lo, addr_hi
    .reg $x
    .sig z
    immed40(addr_hi, addr_lo, crypto_init_flag)
    ctw_loop#:
    cls[read, $x, addr_hi, <<8, addr_lo, 1], ctx_swap[z]
    .if ($x != 0x05170522)
        br[ctw_loop#]
    .endif
.end
#endm


#if ( ENABLE_SA_FLUSH == 1 )
/**
 *  Set SA Flush flags.
 *  For now, supports a flag word in CLS only
 */
#macro crypto_threads_set_sa_flush_flags()
.begin
    .reg addr
    .reg $x
    .sig z
    move(addr, crypto_sa_flush_flags)
    move($x, 0x00ffffff) // 1 bit / sram pkt buffer
    cls[write, $x, addr, 0, 1], ctx_swap[z]
.end
#endm


/**
 *  Clear SA Flush flags.
 *  For now, supports a flag word in CLS only
 */
#macro crypto_threads_clear_sa_flush_flags()
.begin
    .reg addr
    .reg $x
    .sig z
    move(addr, crypto_sa_flush_flags)
    move($x, 0)
    cls[write, $x, addr, 0, 1], ctx_swap[z]
.end
#endm


/**
 *  Test/Clr SA Flush flag.
 *  For now, supports a flag word in CLS only
 *  Clears a single bit in the flag word and
 *  returns original value, using atomic operation
 */
#macro crypto_threads_tst_clr_sa_flush_flag(_flag, _crypto_ctx, _buf_num)
.begin
    .reg addr, bit_mask, t
    .reg $x
    .sig z

    alu[--, _buf_num, OR, 0]
    alu[bit_mask, --, B, 1, <<indirect]
    alu[t, --, B, _crypto_ctx, <<2]
    alu[ --, t, OR, 0]
    alu[bit_mask, --, B, bit_mask, <<indirect]

    move($x, bit_mask) // 1 bit / sram pkt buffer
    move(addr, crypto_sa_flush_flags)
    cls[test_clr, $x, addr, 0, 1], ctx_swap[z]
    alu[_flag, $x, AND, bit_mask]
.end
#endm
#endif // ENABLE_SA_FLUSH == 1

/**
 *  Check SA control word for valid/supported values
 *
 *  @param  _result   GPR, set to 1 on error or 0 if ok
 *  @param  _sa_ctlwrd GPR, control word of SA to be checked
 */
#macro crypto_threads_check_sa(_result, _sa_ctlwrd)
.begin

    .reg r_val

    move(_result, 0)

    // check 'protocol'
    ld_field_w_clr[r_val, 0001, _sa_ctlwrd, >>CRYPTO_SA_PROTOCOL_SHIFT]
    alu[r_val, r_val, AND, CRYPTO_SA_PROTOCOL_MASK]
    .if ( r_val > CRYPTO_PROTOCOL_ESP )
        crypto_threads_update_counter(CRYPTO_CNTR_SA_PROTOCOL_SEL_ERR)
        br[ctcsa_failed#]
    .endif

    // check 'protocol mode'
    ld_field_w_clr[r_val, 0001, _sa_ctlwrd, >>CRYPTO_SA_PROTMODE_SHIFT]
    alu[r_val, r_val, AND, CRYPTO_SA_PROTMODE_MASK]
    .if ( r_val > CRYPTO_PROTMODE_TUNNEL )
        crypto_threads_update_counter(CRYPTO_CNTR_SA_PROT_MODE_SEL_ERR)
        br[ctcsa_failed#]
    .endif

    // check cipher
    ld_field_w_clr[r_val, 0001, _sa_ctlwrd, >>CRYPTO_SA_CIPHER_SHIFT]
    alu[r_val, r_val, AND, CRYPTO_SA_CIPHER_MASK]
    .if ( r_val > CRYPTO_CIPHER_3DES )
        crypto_threads_update_counter(CRYPTO_CNTR_SA_CIPHER_SEL_ERR)
        br[ctcsa_failed#]
    .elif ( r_val < CRYPTO_CIPHER_AES_128 )
        .if ( r_val != CRYPTO_CIPHER_NONE )
            crypto_threads_update_counter(CRYPTO_CNTR_SA_CIPHER_SEL_ERR)
            br[ctcsa_failed#]
        .endif
    .endif

    // check cipher mode
    ld_field_w_clr[r_val, 0001, _sa_ctlwrd, >>CRYPTO_SA_CIMODE_SHIFT]
    alu[r_val, r_val, AND, CRYPTO_SA_CIMODE_MASK]
    .if ( r_val < CRYPTO_CIMODE_CBC )
        crypto_threads_update_counter(CRYPTO_CNTR_SA_CIPHER_MODE_SEL_ERR)
        br[ctcsa_failed#]
    .elif ( r_val > CRYPTO_CIMODE_OFB )
        crypto_threads_update_counter(CRYPTO_CNTR_SA_CIPHER_MODE_SEL_ERR)
        br[ctcsa_failed#]
    .endif

    // check hash
    ld_field_w_clr[r_val, 0001, _sa_ctlwrd, >>CRYPTO_SA_HASH_SHIFT]
    alu[r_val, r_val, AND, CRYPTO_SA_HASH_MASK]
    .if ( r_val != CRYPTO_HASH_SHA_1 )
        .if ( r_val != CRYPTO_HASH_SHA_256 )
            .if ( r_val != CRYPTO_HASH_SHA_512 )
                .if ( r_val != CRYPTO_HASH_NONE )
                    crypto_threads_update_counter(CRYPTO_CNTR_SA_HASH_SEL_ERR)
                    br[ctcsa_failed#]
                .endif
            .endif
        .endif
    .endif

    // check ok
    br[ctcsa_done#]

    // check failed
    ctcsa_failed#:
        move(_result, 1)

    ctcsa_done#:

.end
#endm


/**
 *  Send Response to Response Ring
 *
 *  @param  _me_ctx  GPR, me context ( 0 through 7 )
 *  @param  _buf_sel GPR, buffer select 0-3
 *  @param  _status  CONST or GPR, one of CRYPTO_RSP_STATUS_*
 *
 *  Default implementation, can be overridden by user using OVERIDE_RESPONSE_RING
 *  Sends response into response ring, increments error counters in sa, plus
 *  sends start of packet address into buffer free ring if free is enabled in
 *  cmd packet.
 *
 */
#if (OVERRIDE_RESPONSE_RING == 0 )
#macro crypto_threads_send_response(_me_ctx, _buf_sel, _status)

.begin

    .sig srs
    .reg r_temp, r_temp1, r_temp2, r_lm_base, r_rsp_ring_num, r_poll_count

    .reg $rsp[CRYPTO_RSP_LENGTH]
    .xfer_order $rsp

    // setup direct index/offset access to lm vars here
    crypto_threads_get_lm_temp_addr(r_lm_base, _me_ctx, _buf_sel)
    localmem_set_address(r_lm_base, 0, LM_HANDLE_0)
    #define_eval CRYPTO_LM_PKT_WRBK_ADDR_UPPER_LW (CRYPTO_LM_PKT_WRBK_ADDR_UPPER >> 2)
    #define_eval CRYPTO_LM_PKT_END_ADDR_UPPER_LW  (CRYPTO_LM_PKT_END_ADDR_UPPER >> 2)
    #define_eval CRYPTO_LM_SA_ADDR_LW             (CRYPTO_LM_SA_ADDR >> 2)
    #define_eval CRYPTO_LM_PKT_STRT_ADDR_LW       (CRYPTO_LM_PKT_STRT_ADDR >> 2)
    #define_eval CRYPTO_LM_PKT_CONT_ADDR_LW       (CRYPTO_LM_PKT_CONT_ADDR >> 2)
    #define_eval CRYPTO_LM_PKT_END_ADDR_LW        (CRYPTO_LM_PKT_END_ADDR >> 2)
    #define_eval CRYPTO_LM_PKT_WRBK_ADDR_LW       (CRYPTO_LM_PKT_WRBK_ADDR >> 2)
    #define_eval CRYPTO_LM_PKT_STRT_LENGTH_LW     (CRYPTO_LM_PKT_STRT_LENGTH >> 2)
    #define_eval CRYPTO_LM_PKT_END_LENGTH_LW      (CRYPTO_LM_PKT_END_LENGTH >> 2)
    #define_eval CRYPTO_LM_PKT_WRBK_END_LENGTH_LW (CRYPTO_LM_PKT_WRBK_END_LENGTH >> 2)
    #define_eval CRYPTO_LM_SA_CTLWRD_LW           (CRYPTO_LM_SA_CTLWRD >> 2)
    #define_eval r_pkt_upper_addr1 *l$index0[CRYPTO_LM_PKT_WRBK_ADDR_UPPER_LW]
    #define_eval r_pkt_upper_addr2 *l$index0[CRYPTO_LM_PKT_END_ADDR_UPPER_LW]
    #define_eval r_pkt_strt_addr   *l$index0[CRYPTO_LM_PKT_STRT_ADDR_LW]
    #define_eval r_pkt_cont_addr   *l$index0[CRYPTO_LM_PKT_CONT_ADDR_LW]
    #define_eval r_pkt_end_addr    *l$index0[CRYPTO_LM_PKT_END_ADDR_LW]
    #define_eval r_pkt_wrbk_addr   *l$index0[CRYPTO_LM_PKT_WRBK_ADDR_LW]
    #define_eval r_pkt_lengths1    *l$index0[CRYPTO_LM_PKT_STRT_LENGTH_LW]
    #define_eval r_pkt_lengths2    *l$index0[CRYPTO_LM_PKT_END_LENGTH_LW]
    #define_eval r_pkt_lengths3    *l$index0[CRYPTO_LM_PKT_WRBK_END_LENGTH_LW]
    #define_eval r_sa_addr         *l$index0[CRYPTO_LM_SA_ADDR_LW]
    #define_eval r_sa_ctlwrd       *l$index0[CRYPTO_LM_SA_CTLWRD_LW]


    // if response is an error code, update the sa error
    // counters
    move(r_temp2, _status)
    .if ( r_temp2 )

        ld_field_w_clr[r_temp, 0001, \
            r_pkt_upper_addr2, >>CRYPTO_REQ_SA_ADDR_UPPER_SHIFT]

        .if ( r_temp2 == CRYPTO_RSP_STATUS_AUTH_CHECK_FAIL )
            move(r_temp2, CRYPTO_SA_AUTH_FAIL_CNTR)
        .elif ( r_temp2 == CRYPTO_RSP_STATUS_SA_SELECTION_ERR )
            move(r_temp2, CRYPTO_SA_SELECT_ERR_CNTR)
        .else
            move(r_temp2, CRYPTO_SA_OTHER_ERR_CNTR)
        .endif

        crypto_threads_update_sa_counter(r_temp, r_sa_addr, r_temp2)

        // reload idx reg in case of use in other ctx
        localmem_set_address(r_lm_base, 0, LM_HANDLE_0)
    .endif


    // for decrypt option 1, ESP, tunnel mode, the write back address is
    // the pointer to the plaintext we are writing back and the actual start
    // of packet is in the continuation of packet address

    move(r_temp, r_sa_ctlwrd)
    alu[r_temp1, CRYPTO_SA_PROTOCOL_MASK, AND, r_temp, \
                                  >>CRYPTO_SA_PROTOCOL_SHIFT]
    alu[r_temp2, CRYPTO_SA_PROTMODE_MASK, AND, r_temp, \
                                  >>CRYPTO_SA_PROTMODE_SHIFT]
    .if ( (r_temp1 == CRYPTO_PROTOCOL_ESP) && \
          (r_temp2 == CRYPTO_PROTMODE_TUNNEL) && \
          (r_pkt_upper_addr1 & (1 << CRYPTO_REQ_DECRYPT_OPTION_BIT)))

        ld_field_w_clr[r_temp, 0110, r_pkt_upper_addr2, <<8] // upper addrs
        ld_field[r_temp, 0001, _status, >>0] // status
        alu[r_temp, r_temp, OR, 1, <<31] // must be 1
        move(r_temp1, r_sa_ctlwrd)
        alu[r_temp1, CRYPTO_SA_DIR_MASK, AND, r_temp1, \
                         >>CRYPTO_SA_DIR_SHIFT]
        alu[r_temp, r_temp, OR, r_temp1, <<30] // dir

        alu[r_temp1, --, B, r_pkt_upper_addr1, >>10]
        alu[r_temp1, r_temp1, AND, 3]
        alu[r_temp, r_temp, OR, r_temp1, <<24] //CTM buff size

        // status, etc. in rsp[0]
        alu[$rsp[0], --, B, r_temp]

        // cont addr in rsp[1]
        move($rsp[1], r_pkt_cont_addr)

        // end addr in rsp[2]
        move($rsp[2], r_pkt_end_addr)

        ld_field_w_clr[r_temp, 0011, r_pkt_lengths1, <<0]
        move(r_temp1, r_pkt_lengths3)
        ld_field[r_temp, 1100, r_temp1, <<0]

        // cont, end lengths in rsp[3]
        alu[$rsp[3], --, B, r_temp]

    .else

        ld_field_w_clr[r_temp, 0100, r_pkt_upper_addr1, >>0]
        ld_field[r_temp, 0010, r_pkt_upper_addr2, <<8] // upper addrs
        ld_field[r_temp, 0001, _status, >>0] // status
        alu[r_temp, r_temp, OR, 1, <<31] // must be 1
        move(r_temp1, r_sa_ctlwrd)
        alu[r_temp1, CRYPTO_SA_DIR_MASK, AND, r_temp1, \
                         >>CRYPTO_SA_DIR_SHIFT]
        alu[r_temp, r_temp, OR, r_temp1, <<30] // dir

        alu[r_temp1, --, B, r_pkt_upper_addr1, >>10]
        alu[r_temp1, r_temp1, AND, 3]
        alu[r_temp, r_temp, OR, r_temp1, <<24] //CTM buff size

        // status, etc. in rsp[0]
        alu[$rsp[0], --, B, r_temp]

        // writeback addr in rsp[1]
        move($rsp[1], r_pkt_wrbk_addr)

        // end addr in rsp[2]
        move($rsp[2], r_pkt_end_addr)

        move(r_temp1, r_pkt_lengths2)
        ld_field_w_clr[r_temp, 0011, r_temp1, <<0]
        move(r_temp1, r_pkt_lengths3)
        ld_field[r_temp, 1100, r_temp1, <<0]

        // writeback, end lengths in rsp[3]
        alu[$rsp[3], --, B, r_temp]

    .endif

    // sa addr in rsp[4]
    move($rsp[4], r_sa_addr)


    // for decrypt, put the next hdr field into rsp[5]
    move($rsp[5], 0) // default rsp[5] to 0

    move(r_temp, r_sa_ctlwrd)
    alu[r_temp, CRYPTO_SA_DIR_MASK, AND, r_temp,
                                 >>CRYPTO_SA_DIR_SHIFT]
    .if ( r_temp == CRYPTO_DIR_DECRYPT )

        crypto_threads_get_lm_temp_addr(r_temp, _me_ctx, 0)
        localmem_read1($rsp[5], \
                       r_temp, \
                       CRYPTO_LM_COMM_NXT_HDR_VAL)

        // restore value in lm idx reg
        localmem_set_address(r_lm_base, 0, LM_HANDLE_0)
        nop
        nop
        nop
    .endif


    // write the response ring

    ld_field_w_clr[r_rsp_ring_num, 0011, r_pkt_upper_addr1, \
                                   >>CRYPTO_LM_STATUS_QUE_SHIFT]
    move(r_temp, CRYPTO_LM_STATUS_QUE_MASK)
    alu[r_rsp_ring_num, r_rsp_ring_num, AND, r_temp]

    #if ( RESPONSE_RING_TYPE == CRYPTO_RING_EMU0 \
          || RESPONSE_RING_TYPE == CRYPTO_RING_EMU1 \
          || RESPONSE_RING_TYPE == CRYPTO_RING_EMU2 )

        move(r_poll_count, 100)

        ld_field_w_clr[r_temp, 1000, r_pkt_upper_addr1]

        .repeat

            #define_eval _PUT_LENGTH CRYPTO_RSP_LENGTH
            mem[put, $rsp[0], r_temp, <<8, r_rsp_ring_num, _PUT_LENGTH], \
                                                                sig_done[srs]
            #undef _PUT_LENGTH

            ctx_arb[srs]

            alu[--, $rsp[0], AND, 1, <<31]
            bne[crypto_threads_send_response_done#]

            alu[r_poll_count, r_poll_count, -, 1]

        .until ( r_poll_count <= 0 )

        // failed to put to ring, incr counter & go on without writing ring
        crypto_threads_update_counter(CRYPTO_CNTR_RSP_RING_WRITE_ERR)
        br[crypto_threads_send_response_done#]


    #elif ( RESPONSE_RING_TYPE == CRYPTO_RING_CTM )

        move(r_poll_count, 100)
       .repeat

            jump [r_rsp_ring_num, ctsrjt#], targets [j0#, j1#, j2#, j3#, j4#, \
                                                     j5#, j6#, j7#, j8#, j9#, \
                                                     j10#, j11#, j12#, j13#]
            ctsrjt#:
                j0#: br [ctsr0#]
                j1#: br [ctsr1#]
                j2#: br [ctsr2#]
                j3#: br [ctsr3#]
                j4#: br [ctsr4#]
                j5#: br [ctsr5#]
                j6#: br [ctsr6#]
                j7#: br [ctsr7#]
                j8#: br [ctsr8#]
                j9#: br [ctsr9#]
                j10#: br [ctsr10#]
                j11#: br [ctsr11#]
                j12#: br [ctsr12#]
                j13#: br [ctsr13#]

            ctsr0#:
                br_!inp_state[CTM_Ring0_Status, crypto_threads_rsp_ring_write#]
                br[ctsrpi#]
            ctsr1#:
                br_!inp_state[CTM_Ring1_Status, crypto_threads_rsp_ring_write#]
                br[ctsrpi#]
            ctsr2#:
                br_!inp_state[CTM_Ring2_Status, crypto_threads_rsp_ring_write#]
                br[ctsrpi#]
            ctsr3#:
                br_!inp_state[CTM_Ring3_Status, crypto_threads_rsp_ring_write#]
                br[ctsrpi#]
            ctsr4#:
                br_!inp_state[CTM_Ring4_Status, crypto_threads_rsp_ring_write#]
                br[ctsrpi#]
            ctsr5#:
                br_!inp_state[CTM_Ring5_Status, crypto_threads_rsp_ring_write#]
                br[ctsrpi#]
            ctsr6#:
                br_!inp_state[CTM_Ring6_Status, crypto_threads_rsp_ring_write#]
                br[ctsrpi#]
            ctsr7#:
                br_!inp_state[CTM_Ring7_Status, crypto_threads_rsp_ring_write#]
                br[ctsrpi#]
            ctsr8#:
                br_!inp_state[CTM_Ring8_Status, crypto_threads_rsp_ring_write#]
                br[ctsrpi#]
            ctsr9#:
                br_!inp_state[CTM_Ring9_Status, crypto_threads_rsp_ring_write#]
                br[ctsrpi#]
            ctsr10#:
                br_!inp_state[CTM_Ring10_Status, crypto_threads_rsp_ring_write#]
                br[ctsrpi#]
            ctsr11#:
                br_!inp_state[CTM_Ring11_Status, crypto_threads_rsp_ring_write#]
                br[ctsrpi#]
            ctsr12#:
                br_!inp_state[CTM_Ring12_Status, crypto_threads_rsp_ring_write#]
                br[ctsrpi#]
            ctsr13#:
                br_!inp_state[CTM_Ring13_Status, crypto_threads_rsp_ring_write#]

            ctsrpi#:
            alu[r_poll_count, r_poll_count, -, 1]

        .until ( r_poll_count <= 0 )

        // failed to see ring not full, incr counter & go on without writing ring
        crypto_threads_update_counter(CRYPTO_CNTR_RSP_RING_WRITE_ERR)
        br[crypto_threads_send_response_done#]

        crypto_threads_rsp_ring_write#:
        #define_eval _PUT_LENGTH CRYPTO_RSP_LENGTH
        ru_ctm_ring_put($rsp[0], r_rsp_ring_num, --, _PUT_LENGTH, srs, SIG_WAIT)
        #undef _PUT_LENGTH
    #else
        #error "crypto_threads_send_response: invalid response ring type."
    #endif

    crypto_threads_send_response_done#:


    // send response done, now free start of packet buffer
    .if (r_pkt_upper_addr1 & (1 << CRYPTO_REQ_FREE_BUF_BIT))

        // reload idx reg in case of use in other ctx
        localmem_set_address(r_lm_base, 0, LM_HANDLE_0)

        #if (BUF_RING_TYPE == CRYPTO_RING_EMU0 \
                || BUF_RING_TYPE == CRYPTO_RING_EMU1 \
                || BUF_RING_TYPE == CRYPTO_RING_EMU2 )

        move(r_poll_count, 100)
        move($rsp[0], r_pkt_strt_addr)

        .repeat

            #define_eval _PUT_LENGTH 1
            move(r_temp, BUF_RING_NUM)
            mem[put, $rsp[0], BUF_RING_ISLAND_ID, <<8, r_temp, _PUT_LENGTH], \
                                                                   sig_done[srs]
            #undef _PUT_LENGTH

            ctx_arb[srs]

            alu[--, $rsp[0], AND, 1, <<31]
            bne[crypto_threads_free_buf_done#]

            alu[r_poll_count, r_poll_count, -, 1]

        .until ( r_poll_count <= 0 )

        // failed to put to ring, incr counter & go on without writing ring
        crypto_threads_update_counter(CRYPTO_CNTR_BUF_RING_WRITE_ERR)
        br[crypto_threads_free_buf_done#]


        #elif ( BUF_RING_TYPE == CRYPTO_RING_CTM )

            #if ( BUF_RING_ISLAND_ID == 0x00 )
            move(r_poll_count, 100)
            .repeat

                move(r_temp, BUF_RING_NUM)
                jump [r_temp, ctfbjt#], targets [jt0#, jt1#, jt2#, jt3#, jt4#, jt5#, \
                                                jt6#, jt7#, jt8#, jt9#, jt10#, \
                                                jt11#, jt12#, jt13#]
                ctfbjt#:
                    jt0#: br [ctfb0#]
                    jt1#: br [ctfb1#]
                    jt2#: br [ctfb2#]
                    jt3#: br [ctfb3#]
                    jt4#: br [ctfb4#]
                    jt5#: br [ctfb5#]
                    jt6#: br [ctfb6#]
                    jt7#: br [ctfb7#]
                    jt8#: br [ctfb8#]
                    jt9#: br [ctfb9#]
                    jt10#: br [ctfb10#]
                    jt11#: br [ctfb11#]
                    jt12#: br [ctfb12#]
                    jt13#: br [ctfb13#]

                ctfb0#:
                    br_!inp_state[CTM_Ring0_Status, crypto_threads_buf_ring_write#]
                    br[ctfbpi#]
                ctfb1#:
                    br_!inp_state[CTM_Ring1_Status, crypto_threads_buf_ring_write#]
                    br[ctfbpi#]
                ctfb2#:
                    br_!inp_state[CTM_Ring2_Status, crypto_threads_buf_ring_write#]
                    br[ctfbpi#]
                ctfb3#:
                    br_!inp_state[CTM_Ring3_Status, crypto_threads_buf_ring_write#]
                    br[ctfbpi#]
                ctfb4#:
                    br_!inp_state[CTM_Ring4_Status, crypto_threads_buf_ring_write#]
                    br[ctfbpi#]
                ctfb5#:
                    br_!inp_state[CTM_Ring5_Status, crypto_threads_buf_ring_write#]
                    br[ctfbpi#]
                ctfb6#:
                    br_!inp_state[CTM_Ring6_Status, crypto_threads_buf_ring_write#]
                    br[ctfbpi#]
                ctfb7#:
                    br_!inp_state[CTM_Ring7_Status, crypto_threads_buf_ring_write#]
                    br[ctfbpi#]
                ctfb8#:
                    br_!inp_state[CTM_Ring8_Status, crypto_threads_buf_ring_write#]
                        br[ctfbpi#]
                ctfb9#:
                    br_!inp_state[CTM_Ring9_Status, crypto_threads_buf_ring_write#]
                    br[ctfbpi#]
                ctfb10#:
                    br_!inp_state[CTM_Ring10_Status, crypto_threads_buf_ring_write#]
                    br[ctfbpi#]
                ctfb11#:
                    br_!inp_state[CTM_Ring11_Status, crypto_threads_buf_ring_write#]
                    br[ctfbpi#]
                ctfb12#:
                    br_!inp_state[CTM_Ring12_Status, crypto_threads_buf_ring_write#]
                    br[ctfbpi#]
                ctfb13#:
                    br_!inp_state[CTM_Ring13_Status, crypto_threads_buf_ring_write#]

                ctfbpi#:
                alu[r_poll_count, r_poll_count, -, 1]

            .until ( r_poll_count <= 0 )

            // failed to see ring not full, incr counter & go on without writing ring
            crypto_threads_update_counter(CRYPTO_CNTR_BUF_RING_WRITE_ERR)
            br[crypto_threads_free_buf_done#]

            crypto_threads_buf_ring_write#:
            #define_eval _PUT_LENGTH 1
            move($rsp[0], r_pkt_strt_addr)
            move(r_temp, BUF_RING_NUM)
            ru_ctm_ring_put($rsp[0], r_temp, --, _PUT_LENGTH, srs, SIG_WAIT)
            #undef _PUT_LENGTH

            #else

            // CTM ring in external island, can't check the status.
            // Assume can't become full. Note the <<8 for 40 bit
            // addressing to the island not used with ct instruction.
            move($rsp[0], r_pkt_strt_addr)
            move(r_temp, BUF_RING_ISLAND_ID << 24)
            move(r_temp1, BUF_RING_NUM << 2)
            ct[ring_put, $rsp[0], r_temp,  r_temp1, 1], ctx_swap[srs]

            #endif
    #else
        #error "crypto_threads_send_response: invalid buf ring type."
    #endif

    crypto_threads_free_buf_done#:

    .endif


    #if ( HALT_ON_ERROR == 1 ) // for DEBUG
        .if ( _status != CRYPTO_RSP_STATUS_OK )
            ctx_arb[bpt]
        .endif
    #endif

    #undef r_pkt_upper_addr1
    #undef r_pkt_upper_addr2
    #undef r_pkt_strt_addr
    #undef r_pkt_cont_addr
    #undef r_pkt_end_addr
    #undef r_pkt_wrbk_addr
    #undef r_pkt_lengths1
    #undef r_pkt_lengths2
    #undef r_pkt_lengths3
    #undef r_sa_addr
    #undef r_sa_ctlwrd

.end

#endm
#endif // OVERRIDE_RESPONSE_RING == 0


#if ( ENABLE_ANTI_REPLAY == 1 )
/**
 *  Processing for anti-replay
 *  Invoked in input thread for prelimary check, and at end
 *  of output thread on good decrypts
 *
 *  Does the following:
 *
 *  a) Checks sequence number in packet against anti replay window for the
 *     current SA. If to the left of window ( i.e. sequence number in packet
 *     is < lowest number in window ), marks status as bad and updates sa
 *     resident error counter
 *
 *  b) If to the right of window ( i.e. sequence number in packet is > highest
 *     number in window ), updates highest number in window, and updates window
 *     status bit mask
 *
 *  c) If in the window and already received, marks status as bad and updates
 *     sa resident error counter
 *
 *  d) If in the window and not already recieved, leaves status as good and
 *     updates the window status bit mask.
 *
 *
 *  NOTE: handles 32 bit sequence numbers only
 *
 *  @param  _me_ctx  GPR,   me context ( 0 through 7 )
 *  @param  _buf_sel GPR,   buffer select 0-3
 *  @param  _status  GPR,   one of CRYPTO_RSP_STATUS_* to be
 *                          sent in response, may be modified
 *                          here on ar check failure
 *  @param  _UPDATE  CONST, if 1, will do the replay check plus update
 *                          replay window data as needed. if 0, will only
 *                          do the check. Check only is done in input
 *                          thread, Check plus update in output thread.
 */
#macro crypto_threads_anti_replay (_me_ctx, _buf_sel, _status, _UPDATE )
.begin

    .sig ars
    .reg r_temp, r_temp1, r_temp2
    .reg r_seq_num, r_window_size
    .reg $xd[4]
    .xfer_order $xd

    // setup direct index/offset access to lm vars here
    crypto_threads_get_lm_temp_addr(r_temp, _me_ctx, _buf_sel)
    localmem_set_address(r_temp, 0, LM_HANDLE_0)
    nop
    nop
    nop
    #define_eval CRYPTO_LM_PKT_END_ADDR_UPPER_LW (CRYPTO_LM_PKT_END_ADDR_UPPER >> 2)
    #define_eval CRYPTO_LM_SA_ADDR_LW            (CRYPTO_LM_SA_ADDR >> 2)
    #define_eval CRYPTO_LM_SA_CTLWRD_LW          (CRYPTO_LM_SA_CTLWRD >> 2)
    #define_eval r_pkt_upper_addr2        *l$index0[CRYPTO_LM_PKT_END_ADDR_UPPER_LW]
    #define_eval r_sa_addr                *l$index0[CRYPTO_LM_SA_ADDR_LW]
    #define_eval r_sa_ctlwrd              *l$index0[CRYPTO_LM_SA_CTLWRD_LW]

    // if anti replay is enabled in SA..
    move(r_tmp1, r_sa_ctlwrd)
    .if ( r_tmp1 & ( 1 << CRYPTO_SA_AR_ENA_SHIFT ))

        // move sequence num into gpr
        localmem_read1(r_seq_num, r_temp, \
                        CRYPTO_LM_PKT_SEQ_NUM_LOW)
        // fix index reg for later use
        localmem_set_address(r_temp, 0, LM_HANDLE_0)
        nop
        nop
        nop

        // lock access to the ar data in sa
        #if ( _UPDATE == 1 )
        move(r_temp1, CRYPTO_REQ_SA_VERSION_MASK) // mask off version bits
        alu[r_temp2, r_sa_addr, AND~, r_temp1]
        alu[r_temp2, r_temp2, OR, CRYPTO_SA_AR_LOCK_OFFSET, <<2]
        ld_field_w_clr[r_temp1, 0001, \
            r_pkt_upper_addr2, >>CRYPTO_REQ_SA_ADDR_UPPER_SHIFT]
        alu[r_temp1, --, B, r_temp1, <<24] // sa addr upper byte
        move($xd[0], (1 << CRYPTO_SA_AR_LOCK_SHIFT))
       .repeat
            mem[test_and_set, $xd[0], r_temp1, <<8, r_temp2, 1], sig_done[ars]
            ctx_arb[ars]
            // potential for hang here
       .until ( ($xd[0] & (1 << CRYPTO_SA_AR_LOCK_SHIFT)) == 0)
        #endif


        // read the ar data from the sa
        move(r_temp1, CRYPTO_REQ_SA_VERSION_MASK) // mask off version bits
        alu[r_temp2, r_sa_addr, AND~, r_temp1]
        alu[r_temp2, r_temp2, OR, CRYPTO_SA_AR_CNTR_LOW, <<2]
        ld_field_w_clr[r_temp1, 0001, \
            r_pkt_upper_addr2, >>CRYPTO_REQ_SA_ADDR_UPPER_SHIFT]
        alu[r_temp1, --, B, r_temp1, <<24] // sa addr upper byte
        #pragma warning(disable:5008) // $xd[1] not used now...
        mem[read_atomic, $xd[0], r_temp1, <<8, r_temp2, 4], ctx_swap[ars]
        #pragma warning(default:5008)

        #define r_ar_ctr_lo $xd[0]
        #define r_ar_ctr_hi $xd[1]
        #define r_ar_bm_lo  $xd[2]
        #define r_ar_bm_hi  $xd[3]


        // bit map stores bits for sequence numbers "r_ar_ctr_lo" through
        //  "r_ar_ctr_lo - r_window_size + 1"

        //
        // if rcv'd seq num is to 'left' of window, mark as bad
        //
        move(r_temp1, r_sa_ctlwrd)
        .if ( r_temp1 & ( 1 << CRYPTO_SA_AR_WIN_64_SHIFT ))
            move(r_window_size, 64) // window size 64
        .else
            move(r_window_size, 32) // window size 32
        .endif

        // cntr has to be > window size to do this check
        .if_unsigned ( r_ar_ctr_lo > r_window_size )

            alu[r_temp2, r_ar_ctr_lo, -, r_window_size]

            .if_unsigned ( r_temp2 >= r_seq_num )

                move(_status, CRYPTO_RSP_STATUS_REPLAY_TOO_SMALL)

                move(r_temp1, CRYPTO_REQ_SA_VERSION_MASK) // mask off version bits
                alu[r_temp2, r_sa_addr, AND~, r_temp1]
                ld_field_w_clr[r_temp1, 0001, \
                    r_pkt_upper_addr2, >>CRYPTO_REQ_SA_ADDR_UPPER_SHIFT]

                crypto_threads_update_sa_counter(r_temp1, r_temp2, \
                                CRYPTO_SA_AR_SEQLOW_CNTR )

                br[car_done#]

            .endif
        .endif


        //
        // if rcv'd seq num is to 'right' of window, make right of window
        // equal to the sequence number, shift the window bit mask, and
        // write out the window data
        //
        .if_unsigned ( r_seq_num > r_ar_ctr_lo ) // n.b. for 32 bit seq num only

            #if ( _UPDATE == 1 )

            move($xd[0], r_seq_num)    // ar cntr lo
            move($xd[1], r_ar_ctr_hi)  // ar cntr hi (not used, 32 bit seq num)

            #define r_shift r_temp
            alu[r_shift, r_seq_num, -, r_ar_ctr_lo]

            .if ( r_window_size == 32 )

                .if_unsigned ( r_shift > 32 )
                    move($xd[2], 1 << 31) // bm lo
                .else
                    alu[--, r_shift, OR, 0]
                    alu[r_temp2, --, B, r_ar_bm_lo, >>indirect] // bm lo
                    alu[$xd[2], r_temp2, OR, 1, <<31]
                .endif
                move($xd[3], 0)

            .else // (r_window_size == 64 )

                .if_unsigned ( r_shift > 64 )
                    move($xd[2], 0) // bm lo
                    move($xd[3], 1 << 31) // bm hi
                .elif ( r_shift >= 32 )
                    alu[r_shift, r_shift, -, 32]
                    alu[--, r_shift, OR, 0]
                    alu[$xd[2], --, B, r_ar_bm_hi, >>indirect] // bm lo
                    alu[$xd[3], --, B, 1, <<31] // bm hi
                .else // ( r_shift < 32 )
                    alu[r_temp2, --, B, r_ar_bm_hi]
                    alu[--, r_shift, OR, 0]
                    dbl_shf[$xd[2], r_temp2, r_ar_bm_lo, >>indirect] // bm lo
                    alu[--, r_shift, OR, 0]
                    alu[r_temp2, --, B, r_temp2, >>indirect]
                    alu[$xd[3], r_temp2, OR, 1, <<31] // bm hi
                .endif
            .endif


            // write the ar data back to the sa
            ld_field_w_clr[r_temp1, 0001, \
                r_pkt_upper_addr2, >>CRYPTO_REQ_SA_ADDR_UPPER_SHIFT]
            alu[r_temp1, --, B, r_temp1, <<24] // sa addr upper byte
            move(r_temp2, (CRYPTO_SA_AR_CNTR_LOW << 2))
            alu[r_temp2, r_temp2, OR, r_sa_addr]
            alu[r_temp2, r_temp2, AND~, 3] // mask of ver bits
            mem[write_atomic, $xd[0], r_temp1, <<8, r_temp2, 4], ctx_swap[ars]

            #endif // _UPDATE == 1

        // otherwise, sequence number is within the window
        .else

            #define r_bit_num r_temp
            alu[r_bit_num, r_ar_ctr_lo, -, r_seq_num]

            #define r_rcvd_flag r_temp1
            #pragma warning(disable:5008) // not sure why nfas flags this...
            move(r_rcvd_flag, 0 )
            #pragma warning(default:5008)

            .if ( r_bit_num < 32 )

                alu[r_temp2, 31, -, r_bit_num]
                .if ( r_window_size == 64 )
                    alu[--, r_temp2, OR, 0]
                    alu[r_rcvd_flag, r_ar_bm_hi, AND, 1, <<indirect]
                .else
                    alu[--, r_temp2, OR, 0]
                    alu[r_rcvd_flag, r_ar_bm_lo, AND, 1, <<indirect]
                .endif

            .else // (r_bit_num >= 32)

                alu[r_temp2, r_bit_num, -, 32]
                alu[r_temp2, 31, -, r_temp2]
                alu[--, r_temp2, OR, 0]
                alu[r_rcvd_flag, r_ar_bm_lo, AND, 1, <<indirect]

            .endif

            // if pkt already rcvd, mark it bad and increment
            // sa counter
            .if ( r_rcvd_flag )

                move(_status, CRYPTO_RSP_STATUS_REPLAY_ALREADY_USED)

                move(r_temp1, CRYPTO_REQ_SA_VERSION_MASK) // mask off version bits
                alu[r_temp2, r_sa_addr, AND~, r_temp1]
                ld_field_w_clr[r_temp1, 0001, \
                    r_pkt_upper_addr2, >>CRYPTO_REQ_SA_ADDR_UPPER_SHIFT]

                crypto_threads_update_sa_counter(r_temp1, r_temp2, \
                                CRYPTO_SA_AR_SEQRCV_CNTR )


            // if pkt not already rcvd, update ar bitmap and write it
            // out to sa
            .else

                #if ( _UPDATE == 1 )

                .if ( r_window_size == 64 )
                .if ( r_bit_num < 32 )
                        alu[r_temp2, 31, -, r_bit_num]
                        alu[--, r_temp2, OR, 0]
                        alu[$xd[0], r_ar_bm_hi, OR, 1, <<indirect]

                        move(r_temp2, (CRYPTO_SA_AR_BMAP_HIGH << 2))

                .else // (r_bit_num >= 32)

                    alu[r_temp2, r_bit_num, -, 32]
                        alu[r_temp2, 31, -, r_temp2]
                    alu[--, r_temp2, OR, 0]
                        alu[$xd[0], r_ar_bm_lo, OR, 1, <<indirect]

                        move(r_temp2, (CRYPTO_SA_AR_BMAP_LOW << 2))
                    .endif

                .else
                    alu[r_temp2, 31, -, r_bit_num]
                    alu[--, r_temp2, OR, 0]
                    alu[$xd[0], r_ar_bm_lo, OR, 1, <<indirect]

                    move(r_temp2, (CRYPTO_SA_AR_BMAP_LOW << 2))
                .endif

                // write the ar bmap word back to the sa
                ld_field_w_clr[r_temp1, 0001, \
                    r_pkt_upper_addr2, >>CRYPTO_REQ_SA_ADDR_UPPER_SHIFT]
                alu[r_temp1, --, B, r_temp1, <<24] // sa addr upper byte

                alu[r_temp2, r_temp2, OR, r_sa_addr]
                alu[r_temp2, r_temp2, AND~, 3] // mask of ver bits
                mem[write_atomic, $xd[0], r_temp1, <<8, r_temp2, 1], ctx_swap[ars]

                #endif // _UPDATE == 1

            .endif

        .endif



        // crypto anti replay handling done
        car_done#:



        // release access to the ar data in sa

        #if ( _UPDATE == 1 )

        move(r_temp1, CRYPTO_REQ_SA_VERSION_MASK) // mask off version bits
        alu[r_temp2, r_sa_addr, AND~, r_temp1]
        alu[r_temp2, r_temp2, OR, CRYPTO_SA_AR_LOCK_OFFSET, <<2]
        ld_field_w_clr[r_temp1, 0001, \
            r_pkt_upper_addr2, >>CRYPTO_REQ_SA_ADDR_UPPER_SHIFT]
        alu[r_temp1, --, B, r_temp1, <<24] // sa addr upper byte
        move($xd[0], (1 << CRYPTO_SA_AR_LOCK_SHIFT))
        mem[clr, $xd[0], r_temp1, <<8, r_temp2, 1], ctx_swap[ars]
        #endif // _UPDATE == 1

        #undef r_pkt_upper_addr2
        #undef r_sa_addr
        #undef r_sa_ctlwrd
        #undef r_ar_ctr_lo
        #undef r_ar_ctr_hi
        #undef r_ar_bm_lo
        #undef r_ar_bm_hi

    .endif

.end
#endm
#endif // ENABLE_ANTI_REPLAY

/**
 *  Load core specific addresses of sram buffer areas to CIB for use by
 *  CIB sequences
 *
 *  @param  _cr          XFR transfer registers to use
 *  @param  _crypto_ctx  CONST, crypto context ( 0 through 5 )
 */
#macro crypto_threads_setup_CIB_constants (_cr, _crypto_ctx)
    #define_eval TBASE  crypto_threads_get_temp_addr_const(_crypto_ctx)
    #define_eval BASE1  crypto_threads_get_SA_addr_const(_crypto_ctx)
    #define_eval BASE2  (BASE1+CRYPTO_SA_BLOCK_SIZE)
    #define_eval BASE3  (BASE2+CRYPTO_SA_BLOCK_SIZE)
    #define_eval BASE4  (BASE3+CRYPTO_SA_BLOCK_SIZE)
    crypto_library_load_constants (_cr, _crypto_ctx, TBASE, \
                                    BASE1, BASE2, BASE3, BASE4)
#endm


/**
 *  Allocate a bulk crypto core and load it with library sequences
 *
 *  @param  _crypto_ctx  GPR, crypto core/context ( 0 through 5 )
 */
#macro crypto_threads_init_core (_crypto_ctx)
.begin

    .reg r_temp_strt
    .reg $cr[16]
    .xfer_order $cr

    // enable the bulk core ( toggle the enable bit in bulk cfg reg )
    #if (IS_NFPTYPE(__NFP6000)) // won't work as is for NFP3200
        .sig z
        .reg r_tmp, r_addr, r_offset

        #define CRYPTO_BULK_XPB_DEVICE_ID 23
        // EAS says 0x70000 for offset; must include 4 bits of XPB ID
        #define CRYPTO_BULK_BASIC_CONFIG_OFFSET 0x0000

        // using local, self-island address ( i.e. 0 )
        alu_shf[r_addr, --, B, CRYPTO_BULK_XPB_DEVICE_ID, <<16]
        move(r_offset, CRYPTO_BULK_BASIC_CONFIG_OFFSET)
        ct[xpb_read, $cr[0], r_addr, r_offset, 1], ctx_swap[z]
        alu[--, _crypto_ctx, OR, 0]
        alu[r_tmp, --, B, 1, <<indirect]
        alu[$cr[0], $cr[0], AND~, r_tmp] // clear the enable bit
        ct[xpb_write, $cr[0], r_addr, r_offset, 1], ctx_swap[z]
        alu[$cr[0], $cr[0], OR, r_tmp] // set the enable bit
        ct[xpb_write, $cr[0], r_addr, r_offset, 1], ctx_swap[z]
        nop // time for enable to take effect..?
        nop
        nop
        nop
        nop

        #undef CRYPTO_BULK_XPB_DEVICE_ID
        #undef CRYPTO_BULK_BASIC_CONFIG_OFFSET
    #endif


    // Get the bulk core and assign it to crypto dispatcher context.
    crypto_get_core ($cr, _crypto_ctx)
    crypto_send ($cr)


    // Calculate temp address for this crypto context/core
    crypto_threads_get_temp_addr(r_temp_strt, _crypto_ctx)

    #pragma warning(disable:5151) // not sure why nfas warns here..

    // switching on context is needed because of crypto lib macros setup
    .if ( _crypto_ctx == 0 )
        #if ( CRYPTO_MIN_CTX <= 0 && CRYPTO_MAX_CTX >= 0 )
        // address within CIB buffer where we start loading code sequences;
        // variables & constants come first
        #define_eval CIB_static_start_core_address  32
        // init for loading sequences, use temp var area as staging memory
        crypto_init_CIB_setup (r_temp_strt, 0, CIB_static_start_core_address)
        // Load the library constants into the core's CIB
        crypto_threads_setup_CIB_constants ($cr, 0)
        // Send compressed crypto sequences to the core for use during
        // run-time packet processing.
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 0, ipsec_enc_pin_A, 0)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 0, ipsec_enc_pin_B, 0)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 0, ipsec_enc_pin_C, 0)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 0, ipsec_enc_pin_D, 0)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 0, ipsec_dec_pin_A, 0)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 0, ipsec_dec_pin_B, 0)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 0, ipsec_dec_pin_C, 0)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 0, ipsec_dec_pin_D, 0)
        #if ( ENABLE_JUMBO_PKTS == 1 )
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 0, ipsec_enc_strt_pin_A, 0)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 0, ipsec_enc_strt_pin_B, 0)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 0, ipsec_enc_strt_pin_C, 0)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 0, ipsec_enc_strt_pin_D, 0)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 0, ipsec_enc_cont, 0)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 0, ipsec_enc_end, 0)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 0, ipsec_dec_strt_pin_A, 0)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 0, ipsec_dec_strt_pin_B, 0)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 0, ipsec_dec_strt_pin_C, 0)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 0, ipsec_dec_strt_pin_D, 0)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 0, ipsec_dec_strt_nw_pin_A, 0)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 0, ipsec_dec_strt_nw_pin_B, 0)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 0, ipsec_dec_strt_nw_pin_C, 0)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 0, ipsec_dec_strt_nw_pin_D, 0)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 0, ipsec_dec_cont, 0)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 0, ipsec_dec_cont_nw, 0)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 0, ipsec_dec_end, 0)
        #endif
        #endif
    .elif ( _crypto_ctx == 1 )
        #if ( CRYPTO_MIN_CTX <= 1 && CRYPTO_MAX_CTX >= 1 )
        #define_eval CIB_static_start_core_address  32
        crypto_init_CIB_setup (r_temp_strt, 0, CIB_static_start_core_address)
        crypto_threads_setup_CIB_constants ($cr, 1)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 1, ipsec_enc_pin_A, 1)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 1, ipsec_enc_pin_B, 1)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 1, ipsec_enc_pin_C, 1)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 1, ipsec_enc_pin_D, 1)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 1, ipsec_dec_pin_A, 1)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 1, ipsec_dec_pin_B, 1)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 1, ipsec_dec_pin_C, 1)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 1, ipsec_dec_pin_D, 1)
        #if ( ENABLE_JUMBO_PKTS == 1 )
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 1, ipsec_enc_strt_pin_A, 1)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 1, ipsec_enc_strt_pin_B, 1)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 1, ipsec_enc_strt_pin_C, 1)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 1, ipsec_enc_strt_pin_D, 1)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 1, ipsec_enc_cont, 1)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 1, ipsec_enc_end, 1)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 1, ipsec_dec_strt_pin_A, 1)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 1, ipsec_dec_strt_pin_B, 1)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 1, ipsec_dec_strt_pin_C, 1)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 1, ipsec_dec_strt_pin_D, 1)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 1, ipsec_dec_strt_nw_pin_A, 1)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 1, ipsec_dec_strt_nw_pin_B, 1)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 1, ipsec_dec_strt_nw_pin_C, 1)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 1, ipsec_dec_strt_nw_pin_D, 1)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 1, ipsec_dec_cont, 1)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 1, ipsec_dec_cont_nw, 1)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 1, ipsec_dec_end, 1)
        #endif
        #endif
    .elif ( _crypto_ctx == 2 )
        #if ( CRYPTO_MIN_CTX <= 2 && CRYPTO_MAX_CTX >= 2 )
        #define_eval CIB_static_start_core_address  32
        crypto_init_CIB_setup (r_temp_strt, 0, CIB_static_start_core_address)
        crypto_threads_setup_CIB_constants ($cr, 2)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 2, ipsec_enc_pin_A, 2)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 2, ipsec_enc_pin_B, 2)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 2, ipsec_enc_pin_C, 2)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 2, ipsec_enc_pin_D, 2)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 2, ipsec_dec_pin_A, 2)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 2, ipsec_dec_pin_B, 2)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 2, ipsec_dec_pin_C, 2)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 2, ipsec_dec_pin_D, 2)
        #if ( ENABLE_JUMBO_PKTS == 1 )
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 2, ipsec_enc_strt_pin_A, 2)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 2, ipsec_enc_strt_pin_B, 2)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 2, ipsec_enc_strt_pin_C, 2)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 2, ipsec_enc_strt_pin_D, 2)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 2, ipsec_enc_cont, 2)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 2, ipsec_enc_end, 2)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 2, ipsec_dec_strt_pin_A, 2)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 2, ipsec_dec_strt_pin_B, 2)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 2, ipsec_dec_strt_pin_C, 2)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 2, ipsec_dec_strt_pin_D, 2)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 2, ipsec_dec_strt_nw_pin_A, 2)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 2, ipsec_dec_strt_nw_pin_B, 2)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 2, ipsec_dec_strt_nw_pin_C, 2)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 2, ipsec_dec_strt_nw_pin_D, 2)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 2, ipsec_dec_cont, 2)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 2, ipsec_dec_cont_nw, 2)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 2, ipsec_dec_end, 2)
        #endif
        #endif
    .elif ( _crypto_ctx == 3 )
        #if ( CRYPTO_MIN_CTX <= 3 && CRYPTO_MAX_CTX >= 3 )
        #define_eval CIB_static_start_core_address  32
        crypto_init_CIB_setup (r_temp_strt, 0, CIB_static_start_core_address)
        crypto_threads_setup_CIB_constants ($cr, 3)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 3, ipsec_enc_pin_A, 3)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 3, ipsec_enc_pin_B, 3)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 3, ipsec_enc_pin_C, 3)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 3, ipsec_enc_pin_D, 3)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 3, ipsec_dec_pin_A, 3)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 3, ipsec_dec_pin_B, 3)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 3, ipsec_dec_pin_C, 3)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 3, ipsec_dec_pin_D, 3)
        #if ( ENABLE_JUMBO_PKTS == 1 )
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 3, ipsec_enc_strt_pin_A, 3)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 3, ipsec_enc_strt_pin_B, 3)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 3, ipsec_enc_strt_pin_C, 3)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 3, ipsec_enc_strt_pin_D, 3)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 3, ipsec_enc_cont, 3)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 3, ipsec_enc_end, 3)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 3, ipsec_dec_strt_pin_A, 3)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 3, ipsec_dec_strt_pin_B, 3)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 3, ipsec_dec_strt_pin_C, 3)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 3, ipsec_dec_strt_pin_D, 3)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 3, ipsec_dec_strt_nw_pin_A, 3)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 3, ipsec_dec_strt_nw_pin_B, 3)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 3, ipsec_dec_strt_nw_pin_C, 3)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 3, ipsec_dec_strt_nw_pin_D, 3)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 3, ipsec_dec_cont, 3)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 3, ipsec_dec_cont_nw, 3)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 3, ipsec_dec_end, 3)
        #endif
        #endif
    .elif ( _crypto_ctx == 4 )
        #if ( CRYPTO_MIN_CTX <= 4 && CRYPTO_MAX_CTX >= 4 )
        #define_eval CIB_static_start_core_address  32
        crypto_init_CIB_setup (r_temp_strt, 0, CIB_static_start_core_address)
        crypto_threads_setup_CIB_constants ($cr, 4)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 4, ipsec_enc_pin_A, 4)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 4, ipsec_enc_pin_B, 4)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 4, ipsec_enc_pin_C, 4)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 4, ipsec_enc_pin_D, 4)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 4, ipsec_dec_pin_A, 4)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 4, ipsec_dec_pin_B, 4)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 4, ipsec_dec_pin_C, 4)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 4, ipsec_dec_pin_D, 4)
        #if ( ENABLE_JUMBO_PKTS == 1 )
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 4, ipsec_enc_strt_pin_A, 4)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 4, ipsec_enc_strt_pin_B, 4)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 4, ipsec_enc_strt_pin_C, 4)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 4, ipsec_enc_strt_pin_D, 4)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 4, ipsec_enc_cont, 4)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 4, ipsec_enc_end, 4)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 4, ipsec_dec_strt_pin_A, 4)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 4, ipsec_dec_strt_pin_B, 4)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 4, ipsec_dec_strt_pin_C, 4)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 4, ipsec_dec_strt_pin_D, 4)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 4, ipsec_dec_strt_nw_pin_A, 4)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 4, ipsec_dec_strt_nw_pin_B, 4)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 4, ipsec_dec_strt_nw_pin_C, 4)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 4, ipsec_dec_strt_nw_pin_D, 4)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 4, ipsec_dec_cont, 4)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 4, ipsec_dec_cont_nw, 4)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 4, ipsec_dec_end, 4)
        #endif
        #endif
    .else // _crypto_ctx == 5
        #if ( CRYPTO_MIN_CTX <= 5 && CRYPTO_MAX_CTX >= 5 )
        #define_eval CIB_static_start_core_address  32
        crypto_init_CIB_setup (r_temp_strt, 0, CIB_static_start_core_address)
        crypto_threads_setup_CIB_constants ($cr, 5)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 5, ipsec_enc_pin_A, 5)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 5, ipsec_enc_pin_B, 5)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 5, ipsec_enc_pin_C, 5)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 5, ipsec_enc_pin_D, 5)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 5, ipsec_dec_pin_A, 5)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 5, ipsec_dec_pin_B, 5)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 5, ipsec_dec_pin_C, 5)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 5, ipsec_dec_pin_D, 5)
        #if ( ENABLE_JUMBO_PKTS == 1 )
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 5, ipsec_enc_strt_pin_A, 5)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 5, ipsec_enc_strt_pin_B, 5)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 5, ipsec_enc_strt_pin_C, 5)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 5, ipsec_enc_strt_pin_D, 5)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 5, ipsec_enc_cont, 5)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 5, ipsec_enc_end, 5)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 5, ipsec_dec_strt_pin_A, 5)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 5, ipsec_dec_strt_pin_B, 5)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 5, ipsec_dec_strt_pin_C, 5)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 5, ipsec_dec_strt_pin_D, 5)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 5, ipsec_dec_strt_nw_pin_A, 5)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 5, ipsec_dec_strt_nw_pin_B, 5)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 5, ipsec_dec_strt_nw_pin_C, 5)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 5, ipsec_dec_strt_nw_pin_D, 5)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 5, ipsec_dec_cont, 5)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 5, ipsec_dec_cont_nw, 5)
        crypto_library_load (CRYPTO_LOAD_STATIC, $cr, 5, ipsec_dec_end, 5)
        #endif
        #endif
    .endif

    #pragma warning(default:5151)

    crypto_threads_update_counter(CRYPTO_CNTR_INIT_CORE)

.end

#endm


/**
 *  Simplification macro for use in crypto_threads_encrypt_strt, below
 */
#macro crypto_threads_encrypt_s ( _buf_letter, _params, _runcmd, \
                                    _crypto_ctx, _sram_vars, _BR_TARGET)
    #if ( CRYPTO_MIN_CTX <= _crypto_ctx  && CRYPTO_MAX_CTX >= _crypto_ctx )
         #define_eval _crypto_transfer_length 2
         crypto_load_library_sequence (_params, \
                                       ipsec_enc_pin_/**/_buf_letter/**/, \
                                       _crypto_ctx, _sram_vars)
         crypto_run_library_sequence (_runcmd, _crypto_ctx, \
                ipsec_enc_pin_/**/_buf_letter/**/, \
                _crypto_ctx, _sram_vars, 0, *nosend*)
     #endif
     br[_BR_TARGET]
#endm


/**
 *  Load Encrypt Sequence params to sram, and Generate Encrypt Sequence cmd
 *  in xfr regs
 *
 *  @param  _buf_letter  CONSTANT, buffer selection, one of A,B,C,D
 *  @param  _crypto_ctx  GPR, crypto context ( 0 through 5 )
 *  @param  _runcmd      XFR, transfer register array (2 lwords) returned with
 *                            encrypt sequence cmd
 *  @param  _sram_ptext  GPR, crypto sram address of plaintext start
 *  @param  _ptext_size  GPR, length of plaintext, in bytes - 1
 *  @param  _sram_spi    GPR, crypto sram address of spi
 *  @param  _sram_iv     GPR, crypto sram address of iv
 *  @param  _sram_vars   GPR, crypto sram address of sequence variables
 *  @param  _auth_length GPR, length of authentication header data, in bytes - 1
 *  @param  _cfg_hlen    GPR, icv/hash length, -1 byte
 *  @param  _cfg_select  GPR, cfg_pulse_R and cfg_sel_R fields for
 *                            Crypt_Setup_Regs0
 *  @param  _cfg_modes   GPR, cfg_Crypt0_R field for use in a Crypt_Setup_Regs_0
 *                            instruction
 *
 */
#macro crypto_threads_encrypt (_buf_letter, _crypto_ctx, _runcmd, \
                              _sram_ptext, _ptext_size, _sram_spi, _sram_iv, \
                              _sram_vars, _auth_length, _cfg_hlen, _cfg_select, \
                              _cfg_modes )
.begin

    .reg $params[4]
    .xfer_order $params

    .reg r_ptext_size_m1

    alu[r_ptext_size_m1, _ptext_size, -, 1]
    crypto_load_ipsec_enc_pin_/**/_buf_letter/**/($params, _crypto_ctx, \
                          _sram_ptext, r_ptext_size_m1, _sram_spi, _sram_iv, \
                          _auth_length, _cfg_hlen, _cfg_select, _cfg_modes)

    // switching on context needed because of crypto lib macros setup

    jump [_crypto_ctx, ctejt#], targets [je0#, je1#, je2#, je3#, je4#, je5#]
    ctejt#:
        je0#: br[cte0#]
        je1#: br[cte1#]
        je2#: br[cte2#]
        je3#: br[cte3#]
        je4#: br[cte4#]
        je5#: br[cte5#]

    cte0#:
        crypto_threads_encrypt_s ( _buf_letter, $params, _runcmd, 0, \
                             _sram_vars, crypto_threads_encrypt_done#)

    cte1#:
        crypto_threads_encrypt_s ( _buf_letter, $params, _runcmd, 1, \
                             _sram_vars, crypto_threads_encrypt_done#)

    cte2#:
        crypto_threads_encrypt_s ( _buf_letter, $params, _runcmd, 2, \
                             _sram_vars, crypto_threads_encrypt_done#)

    cte3#:
        crypto_threads_encrypt_s ( _buf_letter, $params, _runcmd, 3, \
                             _sram_vars, crypto_threads_encrypt_done#)

    cte4#:
        crypto_threads_encrypt_s ( _buf_letter, $params, _runcmd, 4, \
                             _sram_vars, crypto_threads_encrypt_done#)

    cte5#:
        crypto_threads_encrypt_s ( _buf_letter, $params, _runcmd, 5, \
                             _sram_vars, crypto_threads_encrypt_done#)

    crypto_threads_encrypt_done#:
.end
#endm


/**
 *  Simplification macro for use in crypto_threads_decrypt, below
 */
#macro crypto_threads_decrypt_s ( _buf_letter, _params, _runcmd, \
                                     _crypto_ctx, _sram_vars, _BR_TARGET)
    #if ( CRYPTO_MIN_CTX <= _crypto_ctx  && CRYPTO_MAX_CTX >= _crypto_ctx )
         #define_eval _crypto_transfer_length 2
         crypto_load_library_sequence (_params, \
                                       ipsec_dec_pin_/**/_buf_letter/**/, \
                                       _crypto_ctx, _sram_vars)
         crypto_run_library_sequence (_runcmd, _crypto_ctx, \
                ipsec_dec_pin_/**/_buf_letter/**/, \
                _crypto_ctx, _sram_vars, 0, *nosend*)
     #endif
     br[_BR_TARGET]
#endm


/**
 *  Load Decrypt Sequence params to sram, and Generate Decrypt Sequence cmd
 *  in xfr regs
 *
 *  @param  _buf_letter  CONSTANT, buffer selection, one of A,B,C,D
 *  @param  _crypto_ctx  GPR, crypto context ( 0 through 5 )
 *  @param  _runcmd      XFR, transfer register array (2 lwords) returned with
 *                            decrypt sequence cmd
 *  @param  _sram_ptext  GPR, crypto sram address of plaintext start
 *  @param  _ptext_size  GPR, length of plaintext, in bytes - 1
 *  @param  _sram_spi    GPR, crypto sram address of spi
 *  @param  _sram_iv     GPR, crypto sram address of iv
 *  @param  _sram_vars   GPR, crypto sram address of sequence variables
 *  @param  _auth_length GPR, length of authentication header data, in bytes - 1
 *  @param  _cfg_hlen    GPR, icv/hash length, -1 byte
 *  @param  _cfg_select  GPR, cfg_pulse_R and cfg_sel_R fields for
 *                            Crypt_Setup_Regs0
 *  @param  _cfg_modes   GPR, cfg_Crypt0_R field for use in a Crypt_Setup_Regs_0
 *                            instruction
 *
 */
#macro crypto_threads_decrypt (_buf_letter, _crypto_ctx, _runcmd, \
                              _sram_ptext, _ptext_size, _sram_spi, _sram_iv, \
                              _sram_vars, _auth_length, _cfg_hlen, _cfg_select, \
                              _cfg_modes )
.begin

    .reg $params[4]
    .xfer_order $params

    .reg r_ptext_size_m1

    alu[r_ptext_size_m1, _ptext_size, -, 1]
    crypto_load_ipsec_dec_pin_/**/_buf_letter/**/($params, _crypto_ctx, \
                          _sram_ptext, r_ptext_size_m1, _sram_spi, _sram_iv, \
                          _auth_length, _cfg_hlen, _cfg_select, _cfg_modes)

    // switching on context needed because of crypto lib macros setup

    jump [_crypto_ctx, ctdjt#], targets [jd0#, jd1#, jd2#, jd3#, jd4#, jd5#]
    ctdjt#:
        jd0#: br[ctd0#]
        jd1#: br[ctd1#]
        jd2#: br[ctd2#]
        jd3#: br[ctd3#]
        jd4#: br[ctd4#]
        jd5#: br[ctd5#]

    ctd0#:
        crypto_threads_decrypt_s ( _buf_letter, $params, _runcmd, 0, \
                             _sram_vars, crypto_threads_decrypt_done#)

    ctd1#:
        crypto_threads_decrypt_s ( _buf_letter, $params, _runcmd, 1, \
                             _sram_vars, crypto_threads_decrypt_done#)

    ctd2#:
        crypto_threads_decrypt_s ( _buf_letter, $params, _runcmd, 2, \
                             _sram_vars, crypto_threads_decrypt_done#)

    ctd3#:
        crypto_threads_decrypt_s ( _buf_letter, $params, _runcmd, 3, \
                             _sram_vars, crypto_threads_decrypt_done#)

    ctd4#:
        crypto_threads_decrypt_s ( _buf_letter, $params, _runcmd, 4, \
                             _sram_vars, crypto_threads_decrypt_done#)

    ctd5#:
        crypto_threads_decrypt_s ( _buf_letter, $params, _runcmd, 5, \
                             _sram_vars, crypto_threads_decrypt_done#)

    crypto_threads_decrypt_done#:
.end
#endm


#if ( ENABLE_JUMBO_PKTS == 1 )

/**
 *  Simplification macro for use in crypto_threads_encrypt_strt, below
 */
#macro crypto_threads_encrypt_strt_s ( _buf_letter, _params, _runcmd, \
                                       _crypto_ctx, _sram_vars, _BR_TARGET)
    #if ( CRYPTO_MIN_CTX <= _crypto_ctx  && CRYPTO_MAX_CTX >= _crypto_ctx )
         #define_eval _crypto_transfer_length 2
         crypto_load_library_sequence (_params, \
                                       ipsec_enc_strt_pin_/**/_buf_letter/**/, \
                                       _crypto_ctx, _sram_vars)
         crypto_run_library_sequence (_runcmd, _crypto_ctx, \
                ipsec_enc_strt_pin_/**/_buf_letter/**/, \
                _crypto_ctx, _sram_vars, 0, *nosend*)
     #endif
     br[_BR_TARGET]
#endm


/**
 *  Load Encrypt Sequence params to sram, and Generate Encrypt Sequence cmd
 *  in xfr regs, for start of jumbo packet
 *
 *  @param  _buf_letter  CONSTANT, buffer selection, one of A,B,C,D
 *  @param  _crypto_ctx  GPR, crypto context ( 0 through 5 )
 *  @param  _runcmd      XFR, transfer register array (2 lwords) returned with
 *                            encrypt sequence cmd
 *  @param  _sram_ptext  GPR, crypto sram address of plaintext start
 *  @param  _ptext_size  GPR, length of plaintext, in bytes - 1
 *  @param  _sram_spi    GPR, crypto sram address of spi
 *  @param  _sram_iv     GPR, crypto sram address of iv
 *  @param  _sram_vars   GPR, crypto sram address of sequence variables
 *  @param  _auth_length GPR, length of authentication header data, in bytes - 1
 *  @param  _cfg_hlen    GPR, icv/hash length, -1 byte
 *  @param  _cfg_select  GPR, cfg_pulse_R and cfg_sel_R fields for
 *                            Crypt_Setup_Regs0
 *  @param  _cfg_modes   GPR, cfg_Crypt0_R field for use in a Crypt_Setup_Regs_0
 *                            instruction
 *
 */
#macro crypto_threads_encrypt_strt (_buf_letter, _crypto_ctx, _runcmd, \
                              _sram_ptext, _ptext_size, _sram_spi, _sram_iv, \
                              _sram_vars, _auth_length, _cfg_hlen, _cfg_select, \
                              _cfg_modes )
.begin

    .reg $params[4]
    .xfer_order $params

    .reg r_ptext_size_m1

    alu[r_ptext_size_m1, _ptext_size, -, 1]
    crypto_load_ipsec_enc_strt_pin_/**/_buf_letter/**/($params, _crypto_ctx, \
                          _sram_ptext, r_ptext_size_m1, _sram_spi, _sram_iv, \
                          _auth_length, _cfg_hlen, _cfg_select, _cfg_modes)

    // switching on context needed because of crypto lib macros setup

    jump [_crypto_ctx, ctesjt#], targets [jes0#, jes1#, jes2#, jes3#, jes4#, jes5#]
    ctesjt#:
        jes0#: br[ctes0#]
        jes1#: br[ctes1#]
        jes2#: br[ctes2#]
        jes3#: br[ctes3#]
        jes4#: br[ctes4#]
        jes5#: br[ctes5#]

    ctes0#:
        crypto_threads_encrypt_strt_s ( _buf_letter, $params, _runcmd, 0, \
                             _sram_vars, crypto_threads_encrypt_strt_done#)

    ctes1#:
        crypto_threads_encrypt_strt_s ( _buf_letter, $params, _runcmd, 1, \
                             _sram_vars, crypto_threads_encrypt_strt_done#)

    ctes2#:
        crypto_threads_encrypt_strt_s ( _buf_letter, $params, _runcmd, 2, \
                             _sram_vars, crypto_threads_encrypt_strt_done#)

    ctes3#:
        crypto_threads_encrypt_strt_s ( _buf_letter, $params, _runcmd, 3, \
                             _sram_vars, crypto_threads_encrypt_strt_done#)

    ctes4#:
        crypto_threads_encrypt_strt_s ( _buf_letter, $params, _runcmd, 4, \
                             _sram_vars, crypto_threads_encrypt_strt_done#)

    ctes5#:
        crypto_threads_encrypt_strt_s ( _buf_letter, $params, _runcmd, 5, \
                             _sram_vars, crypto_threads_encrypt_strt_done#)

     crypto_threads_encrypt_strt_done#:
.end
#endm


/**
 *  Simplification macro for use in crypto_threads_encrypt_cont, below
 */
#macro crypto_threads_encrypt_cont_s ( _params, _runcmd, _crypto_ctx, \
                                                    _sram_vars, _BR_TARGET)
    #if ( CRYPTO_MIN_CTX <= _crypto_ctx  && CRYPTO_MAX_CTX >= _crypto_ctx )
         #define_eval _crypto_transfer_length 2
         crypto_load_library_sequence (_params, ipsec_enc_cont, \
                                            _crypto_ctx, _sram_vars)
         crypto_run_library_sequence (_runcmd, _crypto_ctx, \
                ipsec_enc_cont, _crypto_ctx, _sram_vars, 0, *nosend*)
     #endif
     br[_BR_TARGET]
#endm

/**
 *  Load Encrypt Sequence params to sram, and Generate Encrypt Sequence cmd
 *  in xfr regs, for continuation of jumbo packet
 *
 *  @param  _crypto_ctx  GPR, crypto context ( 0 through 5 )
 *  @param  _runcmd      XFR, transfer register array (2 lwords) returned with
 *                            encrypt sequence cmd
 *  @param  _sram_ptext  GPR, crypto sram address of plaintext start
 *  @param  _ptext_size  GPR, length of plaintext, in bytes - 1
 *  @param  _sram_vars   GPR, crypto sram address of sequence variables
 *
 */
#macro crypto_threads_encrypt_cont (_crypto_ctx, _runcmd, _sram_ptext, \
                                    _ptext_size, _sram_vars )
.begin

    .reg $params[4]
    .xfer_order $params

    .reg r_ptext_size_m1
    .reg r_d1, r_d2, r_d3

    alu[r_ptext_size_m1, _ptext_size, -, 1]

    #pragma warning(disable:4700)
    crypto_load_ipsec_enc_cont($params, _crypto_ctx, \
                          _sram_ptext, r_ptext_size_m1, \
                          r_d1, r_d2, r_d3)
    #pragma warning(default:4700)

    // switching on context needed because of crypto lib macros setup

    jump [_crypto_ctx, ctecjt#], targets [jec0#, jec1#, jec2#, jec3#, jec4#, jec5#]
    ctecjt#:
        jec0#: br[ctec0#]
        jec1#: br[ctec1#]
        jec2#: br[ctec2#]
        jec3#: br[ctec3#]
        jec4#: br[ctec4#]
        jec5#: br[ctec5#]

    ctec0#:
        crypto_threads_encrypt_cont_s ( $params, _runcmd, 0, \
                            _sram_vars, crypto_threads_encrypt_cont_done#)

    ctec1#:
        crypto_threads_encrypt_cont_s ( $params, _runcmd, 1, \
                            _sram_vars, crypto_threads_encrypt_cont_done#)

    ctec2#:
        crypto_threads_encrypt_cont_s ( $params, _runcmd, 2, \
                            _sram_vars, crypto_threads_encrypt_cont_done#)

    ctec3#:
        crypto_threads_encrypt_cont_s ( $params, _runcmd, 3, \
                            _sram_vars, crypto_threads_encrypt_cont_done#)

    ctec4#:
        crypto_threads_encrypt_cont_s ( $params, _runcmd, 4, \
                            _sram_vars, crypto_threads_encrypt_cont_done#)

    ctec5#:
        crypto_threads_encrypt_cont_s ( $params, _runcmd, 5, \
                            _sram_vars, crypto_threads_encrypt_cont_done#)

    crypto_threads_encrypt_cont_done#:
.end
#endm


/**
 *  Simplification macro for use in crypto_threads_encrypt_end, below
 */
#macro crypto_threads_encrypt_end_s ( _params, _runcmd, _crypto_ctx, \
                                                    _sram_vars, _BR_TARGET)
    #if ( CRYPTO_MIN_CTX <= _crypto_ctx  && CRYPTO_MAX_CTX >= _crypto_ctx )
         #define_eval _crypto_transfer_length 2
         crypto_load_library_sequence (_params, ipsec_enc_end, \
                                            _crypto_ctx, _sram_vars)
         crypto_run_library_sequence (_runcmd, _crypto_ctx, \
                ipsec_enc_end, _crypto_ctx, _sram_vars, 0, *nosend*)
     #endif
     br[_BR_TARGET]
#endm

/**
 *  Load Encrypt Sequence params to sram, and Generate Encrypt Sequence cmd
 *  in xfr regs, for end of jumbo packet
 *
 *  @param  _crypto_ctx  GPR, crypto context ( 0 through 5 )
 *  @param  _runcmd      XFR, transfer register array (2 lwords) returned with
 *                            encrypt sequence cmd
 *  @param  _sram_ptext  GPR, crypto sram address of plaintext start
 *  @param  _ptext_size  GPR, length of plaintext, in bytes - 1
 *  @param  _hres_addr   GPR, sram address of hash result
 *  @param  _hkey_addr   GPR, sram address of hash key
 *  @param  _hkey_len    GPR, hash/authentication key length
 *  @param  _sram_vars   GPR, crypto sram address of sequence variables
 */
#macro crypto_threads_encrypt_end (_crypto_ctx, _runcmd, _sram_ptext, \
                                    _ptext_size, _hres_addr, _hkey_addr, \
                                    _hkey_len, _sram_vars )
.begin

    .reg $params[4]
    .xfer_order $params

    .reg r_ptext_size_m1

    alu[r_ptext_size_m1, _ptext_size, -, 1]
    crypto_load_ipsec_enc_end($params, _crypto_ctx, \
                          _sram_ptext, r_ptext_size_m1, \
                          _hres_addr, _hkey_addr, _hkey_len)

    // switching on context needed because of crypto lib macros setup

    jump [_crypto_ctx, cteejt#], targets [jee0#, jee1#, jee2#, jee3#, jee4#, jee5#]
    cteejt#:
        jee0#: br[ctee0#]
        jee1#: br[ctee1#]
        jee2#: br[ctee2#]
        jee3#: br[ctee3#]
        jee4#: br[ctee4#]
        jee5#: br[ctee5#]

    ctee0#:
        crypto_threads_encrypt_end_s ( $params, _runcmd, 0, \
                            _sram_vars, crypto_threads_encrypt_end_done#)

    ctee1#:
        crypto_threads_encrypt_end_s ( $params, _runcmd, 1, \
                            _sram_vars, crypto_threads_encrypt_end_done#)

    ctee2#:
        crypto_threads_encrypt_end_s ( $params, _runcmd, 2, \
                            _sram_vars, crypto_threads_encrypt_end_done#)

    ctee3#:
        crypto_threads_encrypt_end_s ( $params, _runcmd, 3, \
                            _sram_vars, crypto_threads_encrypt_end_done#)

    ctee4#:
        crypto_threads_encrypt_end_s ( $params, _runcmd, 4, \
                            _sram_vars, crypto_threads_encrypt_end_done#)

    ctee5#:
        crypto_threads_encrypt_end_s ( $params, _runcmd, 5, \
                            _sram_vars, crypto_threads_encrypt_end_done#)

    crypto_threads_encrypt_end_done#:
.end
#endm


/**
 *  Simplification macro for use in crypto_threads_decrypt_strt, below
 */
#macro crypto_threads_decrypt_strt_s ( _buf_letter, _params, _runcmd, \
                                       _crypto_ctx, _sram_vars, _BR_TARGET)
    #if ( CRYPTO_MIN_CTX <= _crypto_ctx  && CRYPTO_MAX_CTX >= _crypto_ctx )
         #define_eval _crypto_transfer_length 2
         crypto_load_library_sequence (_params, \
                                       ipsec_dec_strt_pin_/**/_buf_letter/**/, \
                                       _crypto_ctx, _sram_vars)
         crypto_run_library_sequence (_runcmd, _crypto_ctx, \
                ipsec_dec_strt_pin_/**/_buf_letter/**/, \
                _crypto_ctx, _sram_vars, 0, *nosend*)
     #endif
     br[_BR_TARGET]
#endm


/**
 *  Load Decrypt Sequence params to sram, and Generate Decrypt Sequence cmd
 *  in xfr regs, for start of jumbo packet
 *
 *  @param  _buf_letter  CONSTANT, buffer selection, one of A,B,C,D
 *  @param  _crypto_ctx  GPR, crypto context ( 0 through 5 )
 *  @param  _runcmd      XFR, transfer register array (2 lwords) returned with
 *                            decrypt sequence cmd
 *  @param  _sram_ptext  GPR, crypto sram address of plaintext start
 *  @param  _ptext_size  GPR, length of plaintext, in bytes - 1
 *  @param  _sram_spi    GPR, crypto sram address of spi
 *  @param  _sram_iv     GPR, crypto sram address of iv
 *  @param  _sram_vars   GPR, crypto sram address of sequence variables
 *  @param  _auth_length GPR, length of authentication header data, in bytes - 1
 *  @param  _cfg_hlen    GPR, icv/hash length, -1 byte
 *  @param  _cfg_select  GPR, cfg_pulse_R and cfg_sel_R fields for
 *                            Crypt_Setup_Regs0
 *  @param  _cfg_modes   GPR, cfg_Crypt0_R field for use in a Crypt_Setup_Regs_0
 *                            instruction
 *
 */
#macro crypto_threads_decrypt_strt (_buf_letter, _crypto_ctx, _runcmd, \
                              _sram_ptext, _ptext_size, _sram_spi, _sram_iv, \
                              _sram_vars, _auth_length, _cfg_hlen, _cfg_select, \
                              _cfg_modes )
.begin

    .reg $params[4]
    .xfer_order $params

    .reg r_ptext_size_m1

    alu[r_ptext_size_m1, _ptext_size, -, 1]
    crypto_load_ipsec_dec_strt_pin_/**/_buf_letter/**/($params, _crypto_ctx, \
                          _sram_ptext, r_ptext_size_m1, _sram_spi, _sram_iv, \
                          _auth_length, _cfg_hlen, _cfg_select, _cfg_modes)

    // switching on context needed because of crypto lib macros setup

    jump [_crypto_ctx, ctdsjt#], targets [jds0#, jds1#, jds2#, jds3#, jds4#, jds5#]
    ctdsjt#:
        jds0#: br[ctds0#]
        jds1#: br[ctds1#]
        jds2#: br[ctds2#]
        jds3#: br[ctds3#]
        jds4#: br[ctds4#]
        jds5#: br[ctds5#]

    ctds0#:
        crypto_threads_decrypt_strt_s ( _buf_letter, $params, _runcmd, 0, \
                             _sram_vars, crypto_threads_decrypt_strt_done#)

    ctds1#:
        crypto_threads_decrypt_strt_s ( _buf_letter, $params, _runcmd, 1, \
                             _sram_vars, crypto_threads_decrypt_strt_done#)

    ctds2#:
        crypto_threads_decrypt_strt_s ( _buf_letter, $params, _runcmd, 2, \
                             _sram_vars, crypto_threads_decrypt_strt_done#)

    ctds3#:
        crypto_threads_decrypt_strt_s ( _buf_letter, $params, _runcmd, 3, \
                             _sram_vars, crypto_threads_decrypt_strt_done#)

    ctds4#:
        crypto_threads_decrypt_strt_s ( _buf_letter, $params, _runcmd, 4, \
                             _sram_vars, crypto_threads_decrypt_strt_done#)

    ctds5#:
        crypto_threads_decrypt_strt_s ( _buf_letter, $params, _runcmd, 5, \
                             _sram_vars, crypto_threads_decrypt_strt_done#)

     crypto_threads_decrypt_strt_done#:
.end
#endm


/**
 *  Simplification macro for use in crypto_threads_decrypt_strt_nw, below
 */
#macro crypto_threads_decrypt_strt_nw_s ( _buf_letter, _params, _runcmd, \
                                       _crypto_ctx, _sram_vars, _BR_TARGET)
    #if ( CRYPTO_MIN_CTX <= _crypto_ctx  && CRYPTO_MAX_CTX >= _crypto_ctx )
         #define_eval _crypto_transfer_length 2
         crypto_load_library_sequence (_params, \
                                       ipsec_dec_strt_nw_pin_/**/_buf_letter/**/, \
                                       _crypto_ctx, _sram_vars)
         crypto_run_library_sequence (_runcmd, _crypto_ctx, \
                ipsec_dec_strt_nw_pin_/**/_buf_letter/**/, \
                _crypto_ctx, _sram_vars, 0, *nosend*)
     #endif
     br[_BR_TARGET]
#endm


/**
 *  Load Decrypt Sequence params to sram, and Generate Decrypt Sequence cmd
 *  in xfr regs, for start of jumbo packet with no wait
 *
 *  @param  _buf_letter  CONSTANT, buffer selection, one of A,B,C,D
 *  @param  _crypto_ctx  GPR, crypto context ( 0 through 5 )
 *  @param  _runcmd      XFR, transfer register array (2 lwords) returned with
 *                            decrypt sequence cmd
 *  @param  _sram_ptext  GPR, crypto sram address of plaintext start
 *  @param  _ptext_size  GPR, length of plaintext, in bytes - 1
 *  @param  _sram_spi    GPR, crypto sram address of spi
 *  @param  _sram_iv     GPR, crypto sram address of iv
 *  @param  _sram_vars   GPR, crypto sram address of sequence variables
 *  @param  _auth_length GPR, length of authentication header data, in bytes - 1
 *  @param  _cfg_hlen    GPR, icv/hash length, -1 byte
 *  @param  _cfg_select  GPR, cfg_pulse_R and cfg_sel_R fields for
 *                            Crypt_Setup_Regs0
 *  @param  _cfg_modes   GPR, cfg_Crypt0_R field for use in a Crypt_Setup_Regs_0
 *                            instruction
 *
 */
#macro crypto_threads_decrypt_strt_nw (_buf_letter, _crypto_ctx, _runcmd, \
                              _sram_ptext, _ptext_size, _sram_spi, _sram_iv, \
                              _sram_vars, _auth_length, _cfg_hlen, _cfg_select, \
                              _cfg_modes )
.begin

    .reg $params[4]
    .xfer_order $params

    .reg r_ptext_size_m1

    alu[r_ptext_size_m1, _ptext_size, -, 1]
    crypto_load_ipsec_dec_strt_nw_pin_/**/_buf_letter/**/($params, _crypto_ctx, \
                          _sram_ptext, r_ptext_size_m1, _sram_spi, _sram_iv, \
                          _auth_length, _cfg_hlen, _cfg_select, _cfg_modes)

    // switching on context needed because of crypto lib macros setup

    jump [_crypto_ctx, ctdsnwjt#], targets [jdsnw0#, jdsnw1#, jdsnw2#, jdsnw3#, jdsnw4#, jdsnw5#]
    ctdsnwjt#:
        jdsnw0#: br[ctdsnw0#]
        jdsnw1#: br[ctdsnw1#]
        jdsnw2#: br[ctdsnw2#]
        jdsnw3#: br[ctdsnw3#]
        jdsnw4#: br[ctdsnw4#]
        jdsnw5#: br[ctdsnw5#]

    ctdsnw0#:
        crypto_threads_decrypt_strt_nw_s ( _buf_letter, $params, _runcmd, 0, \
                             _sram_vars, crypto_threads_decrypt_strt_nw_done#)

    ctdsnw1#:
        crypto_threads_decrypt_strt_nw_s ( _buf_letter, $params, _runcmd, 1, \
                             _sram_vars, crypto_threads_decrypt_strt_nw_done#)

    ctdsnw2#:
        crypto_threads_decrypt_strt_nw_s ( _buf_letter, $params, _runcmd, 2, \
                             _sram_vars, crypto_threads_decrypt_strt_nw_done#)

    ctdsnw3#:
        crypto_threads_decrypt_strt_nw_s ( _buf_letter, $params, _runcmd, 3, \
                             _sram_vars, crypto_threads_decrypt_strt_nw_done#)

    ctdsnw4#:
        crypto_threads_decrypt_strt_nw_s ( _buf_letter, $params, _runcmd, 4, \
                             _sram_vars, crypto_threads_decrypt_strt_nw_done#)

    ctdsnw5#:
        crypto_threads_decrypt_strt_nw_s ( _buf_letter, $params, _runcmd, 5, \
                             _sram_vars, crypto_threads_decrypt_strt_nw_done#)

     crypto_threads_decrypt_strt_nw_done#:
.end
#endm


/**
 *  Simplification macro for use in crypto_threads_decrypt_cont, below
 */
#macro crypto_threads_decrypt_cont_s ( _params, _runcmd, _crypto_ctx, \
                                                    _sram_vars, _BR_TARGET)
    #if ( CRYPTO_MIN_CTX <= _crypto_ctx  && CRYPTO_MAX_CTX >= _crypto_ctx )
         #define_eval _crypto_transfer_length 2
         crypto_load_library_sequence (_params, ipsec_dec_cont, \
                                            _crypto_ctx, _sram_vars)
         crypto_run_library_sequence (_runcmd, _crypto_ctx, \
                ipsec_dec_cont, _crypto_ctx, _sram_vars, 0, *nosend*)
     #endif
     br[_BR_TARGET]
#endm

/**
 *  Load Decrypt Sequence params to sram, and Generate Decrypt Sequence cmd
 *  in xfr regs, for continuation of jumbo packet
 *
 *  @param  _crypto_ctx  GPR, crypto context ( 0 through 5 )
 *  @param  _runcmd      XFR, transfer register array (2 lwords) returned with
 *                            decrypt sequence cmd
 *  @param  _sram_ptext  GPR, crypto sram address of plaintext start
 *  @param  _ptext_size  GPR, length of plaintext, in bytes - 1
 *  @param  _sram_vars   GPR, crypto sram address of sequence variables
 *
 */
#macro crypto_threads_decrypt_cont (_crypto_ctx, _runcmd, _sram_ptext, \
                                    _ptext_size, _sram_vars )
.begin

    .reg $params[4]
    .xfer_order $params

    .reg r_ptext_size_m1
    .reg r_d1, r_d2, r_d3

    alu[r_ptext_size_m1, _ptext_size, -, 1]

    #pragma warning(disable:4700)
    crypto_load_ipsec_dec_cont($params, _crypto_ctx, \
                          _sram_ptext, r_ptext_size_m1, \
                          r_d1, r_d2, r_d3)
    #pragma warning(default:4700)

    // switching on context needed because of crypto lib macros setup

    jump [_crypto_ctx, ctdcjt#], targets [jdc0#, jdc1#, jdc2#, jdc3#, jdc4#, jdc5#]
    ctdcjt#:
        jdc0#: br[ctdc0#]
        jdc1#: br[ctdc1#]
        jdc2#: br[ctdc2#]
        jdc3#: br[ctdc3#]
        jdc4#: br[ctdc4#]
        jdc5#: br[ctdc5#]

    ctdc0#:
        crypto_threads_decrypt_cont_s ( $params, _runcmd, 0, \
                            _sram_vars, crypto_threads_decrypt_cont_done#)

    ctdc1#:
        crypto_threads_decrypt_cont_s ( $params, _runcmd, 1, \
                            _sram_vars, crypto_threads_decrypt_cont_done#)

    ctdc2#:
        crypto_threads_decrypt_cont_s ( $params, _runcmd, 2, \
                            _sram_vars, crypto_threads_decrypt_cont_done#)

    ctdc3#:
        crypto_threads_decrypt_cont_s ( $params, _runcmd, 3, \
                            _sram_vars, crypto_threads_decrypt_cont_done#)

    ctdc4#:
        crypto_threads_decrypt_cont_s ( $params, _runcmd, 4, \
                            _sram_vars, crypto_threads_decrypt_cont_done#)

    ctdc5#:
        crypto_threads_decrypt_cont_s ( $params, _runcmd, 5, \
                            _sram_vars, crypto_threads_decrypt_cont_done#)

    crypto_threads_decrypt_cont_done#:

.end
#endm


/**
 *  Simplification macro for use in crypto_threads_decrypt_cont_nw, below
 */
#macro crypto_threads_decrypt_cont_nw_s ( _params, _runcmd, _crypto_ctx, \
                                                    _sram_vars, _BR_TARGET)
    #if ( CRYPTO_MIN_CTX <= _crypto_ctx  && CRYPTO_MAX_CTX >= _crypto_ctx )
         #define_eval _crypto_transfer_length 2
         crypto_load_library_sequence (_params, ipsec_dec_cont_nw, \
                                            _crypto_ctx, _sram_vars)
         crypto_run_library_sequence (_runcmd, _crypto_ctx, \
                ipsec_dec_cont_nw, _crypto_ctx, _sram_vars, 0, *nosend*)
     #endif
     br[_BR_TARGET]
#endm

/**
 *  Load Decrypt Sequence params to sram, and Generate Decrypt Sequence cmd
 *  in xfr regs, for continuation of jumbo packet, without wait
 *
 *  @param  _crypto_ctx  GPR, crypto context ( 0 through 5 )
 *  @param  _runcmd      XFR, transfer register array (2 lwords) returned with
 *                            decrypt sequence cmd
 *  @param  _sram_ptext  GPR, crypto sram address of plaintext start
 *  @param  _ptext_size  GPR, length of plaintext, in bytes - 1
 *  @param  _sram_vars   GPR, crypto sram address of sequence variables
 *
 */
#macro crypto_threads_decrypt_cont_nw (_crypto_ctx, _runcmd, _sram_ptext, \
                                    _ptext_size, _sram_vars )
.begin

    .reg $params[4]
    .xfer_order $params

    .reg r_ptext_size_m1
    .reg r_d1, r_d2, r_d3

    alu[r_ptext_size_m1, _ptext_size, -, 1]

    #pragma warning(disable:4700)
    crypto_load_ipsec_dec_cont_nw($params, _crypto_ctx, \
                          _sram_ptext, r_ptext_size_m1, \
                          r_d1, r_d2, r_d3)
    #pragma warning(default:4700)

    // switching on context needed because of crypto lib macros setup

    jump [_crypto_ctx, ctdcnwjt#], targets [jdcnw0#, jdcnw1#, jdcnw2#, jdcnw3#, jdcnw4#, jdcnw5#]
    ctdcnwjt#:
        jdcnw0#: br[ctdcnw0#]
        jdcnw1#: br[ctdcnw1#]
        jdcnw2#: br[ctdcnw2#]
        jdcnw3#: br[ctdcnw3#]
        jdcnw4#: br[ctdcnw4#]
        jdcnw5#: br[ctdcnw5#]

    ctdcnw0#:
        crypto_threads_decrypt_cont_nw_s ( $params, _runcmd, 0, \
                            _sram_vars, crypto_threads_decrypt_cont_nw_done#)

    ctdcnw1#:
        crypto_threads_decrypt_cont_nw_s ( $params, _runcmd, 1, \
                            _sram_vars, crypto_threads_decrypt_cont_nw_done#)

    ctdcnw2#:
        crypto_threads_decrypt_cont_nw_s ( $params, _runcmd, 2, \
                            _sram_vars, crypto_threads_decrypt_cont_nw_done#)

    ctdcnw3#:
        crypto_threads_decrypt_cont_nw_s ( $params, _runcmd, 3, \
                            _sram_vars, crypto_threads_decrypt_cont_nw_done#)

    ctdcnw4#:
        crypto_threads_decrypt_cont_nw_s ( $params, _runcmd, 4, \
                            _sram_vars, crypto_threads_decrypt_cont_nw_done#)

    ctdcnw5#:
        crypto_threads_decrypt_cont_nw_s ( $params, _runcmd, 5, \
                            _sram_vars, crypto_threads_decrypt_cont_nw_done#)

    crypto_threads_decrypt_cont_nw_done#:

.end
#endm


/**
 *  Simplification macro for use in crypto_threads_decrypt_end, below
 */
#macro crypto_threads_decrypt_end_s ( _params, _runcmd, _crypto_ctx, \
                                                    _sram_vars, _BR_TARGET)
    #if ( CRYPTO_MIN_CTX <= _crypto_ctx  && CRYPTO_MAX_CTX >= _crypto_ctx )
         #define_eval _crypto_transfer_length 2
         crypto_load_library_sequence (_params, ipsec_dec_end, \
                                            _crypto_ctx, _sram_vars)
         crypto_run_library_sequence (_runcmd, _crypto_ctx, \
                ipsec_dec_end, _crypto_ctx, _sram_vars, 0, *nosend*)
     #endif
     br[_BR_TARGET]
#endm

/**
 *  Load Decrypt Sequence params to sram, and Generate Decrypt Sequence cmd
 *  in xfr regs, for end of jumbo packet
 *
 *  @param  _crypto_ctx  GPR, crypto context ( 0 through 5 )
 *  @param  _runcmd      XFR, transfer register array (2 lwords) returned with
 *                            decrypt sequence cmd
 *  @param  _sram_ptext  GPR, crypto sram address of plaintext start
 *  @param  _ptext_size  GPR, length of plaintext, in bytes - 1
 *  @param  _hres_addr   GPR, sram address of hash result
 *  @param  _hkey_addr   GPR, sram address of hash key
 *  @param  _hkey_len    GPR, hash/authentication key length
 *  @param  _sram_vars   GPR, crypto sram address of sequence variables
 */
#macro crypto_threads_decrypt_end (_crypto_ctx, _runcmd, _sram_ptext, \
                                    _ptext_size, _hres_addr, _hkey_addr, \
                                    _hkey_len, _sram_vars )
.begin

    .reg $params[4]
    .xfer_order $params

    .reg r_ptext_size_m1

    alu[r_ptext_size_m1, _ptext_size, -, 1]
    crypto_load_ipsec_dec_end($params, _crypto_ctx, \
                          _sram_ptext, r_ptext_size_m1, \
                          _hres_addr, _hkey_addr, _hkey_len)

    // switching on context needed because of crypto lib macros setup

    jump [_crypto_ctx, ctdejt#], targets [jde0#, jde1#, jde2#, jde3#, jde4#, jde5#]
    ctdejt#:
        jde0#: br[ctde0#]
        jde1#: br[ctde1#]
        jde2#: br[ctde2#]
        jde3#: br[ctde3#]
        jde4#: br[ctde4#]
        jde5#: br[ctde5#]

    ctde0#:
        crypto_threads_decrypt_end_s ( $params, _runcmd, 0, \
                            _sram_vars, crypto_threads_decrypt_end_done#)

    ctde1#:
        crypto_threads_decrypt_end_s ( $params, _runcmd, 1, \
                            _sram_vars, crypto_threads_decrypt_end_done#)

    ctde2#:
        crypto_threads_decrypt_end_s ( $params, _runcmd, 2, \
                            _sram_vars, crypto_threads_decrypt_end_done#)

    ctde3#:
        crypto_threads_decrypt_end_s ( $params, _runcmd, 3, \
                            _sram_vars, crypto_threads_decrypt_end_done#)

    ctde4#:
        crypto_threads_decrypt_end_s ( $params, _runcmd, 4, \
                            _sram_vars, crypto_threads_decrypt_end_done#)

    ctde5#:
        crypto_threads_decrypt_end_s ( $params, _runcmd, 5, \
                            _sram_vars, crypto_threads_decrypt_end_done#)

    crypto_threads_decrypt_end_done#:
.end
#endm

#endif /* ENABLE_JUMBO_PKTS */


/**
 *  Re-enable 'ready' signal for buffer
 *
 *  @param  _bufr  GPR, buffer selection, one of 0-3
 */
#macro crypto_threads_reenable_buf(_buf)
.begin
    .reg r_temp
    alu[r_temp, _buf, OR, 0x8] // bufs using sigs 8-11
    alu[r_temp, r_me_ctx, OR, r_temp, <<3]
    local_csr_wr[SAME_ME_SIGNAL, r_temp]
.end
#endm


/**
 *  Helper macro to invoke send_status_response as a
 *  'subroutine' within the input thread. Note that
 *  crypto_threads_reenable_buf is also invoked because,
 *  whenever the input thread does send_response, it is
 *  because of an error and the buffer won't be passed
 *  along to the output thread, so we have to 'reuse' it
 */
#define SEND_RESPONSE_SUBROUTINE 1 // use subroutine here for send response
#macro crypto_threads_input_send_response(_ctx, _buf, _stat)
    #if ( SEND_RESPONSE_SUBROUTINE == 1 )
        move(r_stat, _stat )
        load_addr[r_rtn, cti_rtn#]
        br[send_response_sub#]
        cti_rtn#:
    #else
        crypto_threads_send_response( _ctx, _buf, _stat )
        crypto_threads_reenable_buf(_buf) // reuse this buffer
    #endif
#endm


/// @endcond


/**
 *  Crypto input thread
 *
 *  @param  _me_ctx       GPR, me context of this thread
 *  @param  _crypto_ctx   GPR, crypto core context to be used by this thread.
 *                        One context is used per core, so context will be equal
 *                        to the core # in use by this thread
 *  @param  _in_ring_type CONST, ring type. one of CRYTPO_RING_CTM,
 *                        CRYPTO_RING_IMU, CRYPTO_RING_EMU
 *  @param  _in_ring_num  CONST or GPR, ring number of input request ring.
 *
 *  Input thread dedicated to a single crypto context / core. Typically run on
 *  even #'d thread, i.e. me threads 0,2,4,6 while corresponding output thread
 *  is run on odd #'d threads, i.e. me threads 1,3,5,7
 *
 *  Utilizes 1/6 of crypto sram buffer; each crypto core/ctx is allocated 1/6 of
 *  the sram buffer. Within each 1/6 of the sram buffer, space is allocated for
 *  4x (2KB packet buffer, SA struct, and temp area)
 *
 *  The 4 packet buffers are used to create a pipeline to keep the crypto core
 *  active. The 4 buffers are in one of the following states:
 *  - data being dma'd from system memory into crypto sram buffer
 *  - ready to be operated on by crypto core when current crypto operation done
 *  - being operated on by crypto core
 *  - being dma'd from crypto sram to system memory
 *
 *  Source packet data may be stored contiguously in one buffer, or split into
 *  two or three buffers. One, two, or three dma transfers will be performed to
 *  move the buffers into crypt sram for encryption/decryption. The buffers
 *  will be placed in crypto sram contiguously, starting with data at Start of
 *  Packet Address, followed by Continuation of Packet Address, followed by End
 *  of Packet Address. If a length field is 0, no dma will occur for the
 *  corresponding buffer. ( addresses and length are part of the request ring
 *  entry, refer to request ring entry in code file for format)
 */
#macro crypto_threads_input(_me_ctx, _crypto_ctx, _in_ring_type, _in_ring_num )

    .begin

    //
    // When split into two buffers the intended use is as follows:
    // Start of Packet Address refers to a byte address in a CTM buffer, not in
    // a Crypto Island, where the start of a received packet is stored. End of
    // Packet Address refers to an 8 byte aligned address in Emu/Imu Memory,
    // which is the rest of the packet, following the portion in ctm. Note that
    // the ctm start/length must result in an 8 byte aligned ending address in
    // ctm if the end of packet length is non-zero. The start byte need not be
    // 8 byte aligned, and the dma will begin at the nearest 8 byte aligned
    // address in ctm which contains the starting byte; hence, extra bytes may
    // be read but will not be used. The 2 buffer split is likely to be used
    // for decryption operations. In the case of decrypt option 1, in
    // which only the decrypted plain text is written back out, the continue
    // address is used to pass the address of the start of the output packet,
    // and is passed through into the response ring entry.
    //
    // When split into three buffers, the intended use is as follows:
    // Start of Packet Address refers to a byte address in a Crypto Island
    // resident CTM buffer (although it does not have to) where the start of a
    // received packet is stored after having been read from it's original
    // location, modified to insert the various ipsec related fields to convert
    // a non-ipsec packet to an ipsec packet, and re-written into Crypto CTM.
    // Continuation of Packet Address refers to the portion of the original
    // received packet, in the non-Crypto Island CTM where it was received,
    // which follows the beginning of the packet which was modified. End of
    // Packet Address refers to an 8 byte aligned address in Emu/Imu Memory,
    // which is the rest of the packet, following the portion in ctm. Note that
    // the Start of Packet start/length must result in an 8 byte aligned ending
    // address in ctm if either Continue or End of Packet length is non-zero.
    // The start byte need not be 8 byte aligned, and the dma will begin at
    // the nearest 8 byte aligned address in ctm which contains the starting
    // byte; hence, extra bytes may be read but will not be used. Continuation
    // and End Addresses must be 8 byte aligned. The Continuation Length must be
    // an 8 byte multiple if End of Packet Length is non-zero. The 3 buffer
    // split is expected to be used for encryption operations.
    //
    // In all cases, the resulting output packet is written back at Packet Write
    // Back address, from the Request Ring entry, The intended use is that
    // Packet Write Back address is in the original CTM receive buffer, and may
    // be the original CTM packet starting address, offset by 1-7 bytes as
    // needed to align the portion of the packet written back to CTM such that
    // the starting address and length results in an 8 byte aligned ending
    // address in the CTM buffer. The balance of the packet, if any will be
    // written starting at End of Packet address which is typically in dram
    // memory. The actual addresses and byte counts will be returned in the
    // output response. In the case of Decrypt option 1, in which only the
    // decrypted plain text is written back out, the write back address
    // corresponds to the address of the start of the plain text data, rather
    // than the start of the packet.
    //
    // Thread processing is as follows:
    //  1) wait for next buffer section ( one of 4 ) to be available
    //  2) read next request from input ring
    //  3) read in Security Association data from memory to crypto sram
    //  4) read in packet data to be encrypted or decrypted to crypto sram
    //  5) generate vars for CIB based crypto sequence
    //  6) queue execution of crypto sequence
    //  7) go back to step 1
    //
    // For jumbo packets > 2KB, processing is altered because the crypto hardware
    // can only process 2KB at a time, and the sram packet buffer allocation
    // is only 2KB. Therefore, processing is sequenced so that the 1st 2KB
    // of packet data is loaded and processed using buffer n, the next up to
    // 2KB using buffer n+1, etc. Processing actions specific to the start
    // of the packet are carried out only on the 1st part of the packet,
    // and actions specific to the last part of the packet ( i.e. hash
    // transfer and checking ) are carried out only on the last part of the
    // packet.

    .sig s
    .reg r_buf, r_sram_sa, r_sram_vars, r_sram_pkt
    .reg r_pkt_end_addr, r_pkt_wrbk_addr
    .reg r_pkt_strt_addr_hi, r_pkt_cont_addr_hi, r_pkt_end_addr_hi
    .reg r_pkt_strt_len, r_pkt_cont_len, r_pkt_end_len
    .reg r_pkt_wrbk_len, r_pkt_wrbk_end_len
    .reg r_tmp, r_tmp1, r_tmp2
    .reg r_test_flag
    .reg r_auth_data_addr

    .reg r_sa_ctlwrd
    .reg r_dir, r_sram_ptext, r_ptext_size, r_sram_spi, r_sram_iv
    .reg r_cipher, r_cimode, r_hash, r_auth_len
    .reg r_iv_len, r_cfg_hlen, r_cfg_select, r_cfg_modes

    #if ( SEND_RESPONSE_SUBROUTINE == 1 )
    .reg r_stat, r_rtn
    #endif

    .reg $req[13]
    .xfer_order $req


    .sig volatile buf_sig_A, buf_sig_B, buf_sig_C, buf_sig_D
    .addr buf_sig_A 8
    .addr buf_sig_B 9
    .addr buf_sig_C 10
    .addr buf_sig_D 11

    .set_sig buf_sig_A
    .set_sig buf_sig_B
    .set_sig buf_sig_C
    .set_sig buf_sig_D


    crypto_threads_update_counter(CRYPTO_CNTR_INPUT_THREAD_FLAG)

    // init internal vars
    move(r_test_flag, 0)
    move(r_buf, 0)

    // init sa addr store in lm because read it before it is written
    // same for jumbo state
    move(r_tmp1, 0xffffffff)
    .repeat
        crypto_threads_get_lm_temp_addr(r_tmp, _me_ctx, r_buf)
        localmem_write1(r_tmp1, r_tmp, CRYPTO_LM_SA_ADDR)
        #if ( ENABLE_JUMBO_PKTS == 1 )
        localmem_write1(CRYPTO_LM_JUMBO_STATE_NON_JUMBO, \
                        r_tmp, CRYPTO_LM_JUMBO_STATE)
        .if ( r_buf == 0 )
            localmem_write1(CRYPTO_LM_JUMBO_STATE_NON_JUMBO, \
                            r_tmp, CRYPTO_LM_COMM_JUMBO_STATE)
        .endif
        #endif
        alu[r_buf, r_buf, +, 1]
    .until ( r_buf > 3 )


    // set all buffer ready signals to start
    alu[r_tmp, r_me_ctx, OR, (&buf_sig_A<<3)]
    local_csr_wr[SAME_ME_SIGNAL, r_tmp]
    alu[r_tmp, r_me_ctx, OR, (&buf_sig_B<<3)]
    local_csr_wr[SAME_ME_SIGNAL, r_tmp]
    alu[r_tmp, r_me_ctx, OR, (&buf_sig_C<<3)]
    local_csr_wr[SAME_ME_SIGNAL, r_tmp]
    alu[r_tmp, r_me_ctx, OR, (&buf_sig_D<<3)]
    local_csr_wr[SAME_ME_SIGNAL, r_tmp]


    // start with buffer A
    immed[r_buf, 0]



    //
    // input processing loop
    //

    crypto_input_thread_loop#:


    // wait for next buffer to be available. sigs 8,9,10,11, for buf_sig_A,B,C,D
    // are set when output thread is done with respective buffer
    move(r_tmp, 0x100)
    alu[--, r_buf, OR, 0]
    alu[r_tmp, --, B, r_tmp, <<indirect]
    ctx_arb[--], defer[1]
    local_csr_wr[ACTIVE_CTX_WAKEUP_EVENTS, r_tmp]



    //
    // handling when in midst of a jumbo packet
    //
    #if ( ENABLE_JUMBO_PKTS == 1 )
    crypto_threads_get_lm_temp_addr(r_tmp, _me_ctx, 0)
    localmem_read1(r_tmp1, r_tmp, CRYPTO_LM_COMM_JUMBO_STATE)
    .if ( r_tmp1 == CRYPTO_LM_JUMBO_STATE_FIRST_BUF \
           || r_tmp1 == CRYPTO_LM_JUMBO_STATE_CONTINUE_BUF )
        // last buffer was 1st or continuation part of jumbo. this
        // buffer will be either a continuation, if the packet goes beyond
        // another 2KB, or the end, if the packet end is within next 2KB
        // skip reading request ring
        br[crypto_input_thread_gen_jumbo_params#]
    .endif
    #endif



    //
    // read the request ring
    //

    crypto_threads_input_read_req_ring#:

    crypto_threads_update_counter(CRYPTO_CNTR_CMD_RING_READ)

    #if ( _in_ring_type == CRYPTO_RING_EMU0 \
            || _in_ring_type == CRYPTO_RING_EMU1 \
            || _in_ring_type == CRYPTO_RING_EMU2 )


        // this code polls request ring continuously. could add mechanism,
        // potentially using a work queue, to stop polling request ring when
        // found empty for n polls in a row, such as n == 3, in order to reduce
        // cpp bus accesses.


        #pragma warning(disable:5008,disable:5004)
        ru_emem_ring_get(crypto_req_ring, $req[0], _in_ring_num, CRYPTO_REQ_LENGTH, s)
        #pragma warning(default:5008,default:5004)
        br_signal[s[1],crypto_threads_input_req_ring_empty#] // not enough words in ring
        br_bset[CRYPTO_REQ_ENTRY_VALID, CRYPTO_REQ_ENTRY_VALID_BIT, \
                         crypto_threads_input_req_ring_not_empty#] // valid cmd

        // increment empty ring seen counter while a test is running
        crypto_threads_input_req_ring_empty#:
        .if ( r_test_flag )
            crypto_threads_update_counter(CRYPTO_CNTR_CMD_RING_EMPTY)
        .endif
        br[crypto_threads_input_read_req_ring#]

    #elif ( _in_ring_type == CRYPTO_RING_CTM )

        #define_eval _REQ_LENGTH CRYPTO_REQ_LENGTH
        #pragma warning(disable:5008)
        ru_ctm_ring_get($req[0], _in_ring_num, --, _REQ_LENGTH, s, SIG_WAIT)
        #pragma warning(default:5008)
        br_bset[CRYPTO_REQ_ENTRY_VALID, CRYPTO_REQ_ENTRY_VALID_BIT, \
                         crypto_threads_input_req_ring_not_empty#] // valid cmd
        // increment empty ring seen counter while a test is running
        .if ( r_test_flag )
            crypto_threads_update_counter(CRYPTO_CNTR_CMD_RING_EMPTY)
        .endif
        br[crypto_threads_input_read_req_ring#]

    #else
        #error "crypto_threads_input: invalid input ring type."
    #endif


    crypto_threads_input_req_ring_not_empty#:

    aggregate_directive(.set, $req, 13) // for nfas warning

    alu[r_test_flag, CRYPTO_REQ_TEST_FLAG, AND, 1, <<CRYPTO_REQ_TEST_FLAG_BIT]

    crypto_threads_update_counter(CRYPTO_CNTR_CMD_READ)


    // Read sa addr, for re-use check below, before re-write it
    crypto_threads_get_lm_temp_addr(r_tmp, _me_ctx, r_buf)
    localmem_read1(r_tmp1, r_tmp, CRYPTO_LM_SA_ADDR)

    // Save request params needed by output thread in local memory
    localmem_write8(CRYPTO_REQ_PKT_WRBK_ADDR_UPPER, \
                    CRYPTO_REQ_PKT_END_ADDR_UPPER, \
                    CRYPTO_REQ_SA_ADDR, \
                    CRYPTO_REQ_PKT_STRT_ADDR, \
                    CRYPTO_REQ_PKT_CONT_ADDR, \
                    CRYPTO_REQ_PKT_END_ADDR, \
                    CRYPTO_REQ_PKT_WRBK_ADDR, \
                    CRYPTO_REQ_PKT_STRT_LENGTH, \
                    r_tmp, CRYPTO_LM_PKT_WRBK_ADDR_UPPER)
    localmem_write_next(CRYPTO_REQ_PKT_END_LENGTH)
    localmem_write_next(CRYPTO_REQ_PKT_WRBK_END_LENGTH)
    localmem_write_next(CRYPTO_REQ_AUTH_HDR_OFFSET)
    localmem_write_next(CRYPTO_REQ_AUTH_DATA_OFFSET)
    localmem_write_next(CRYPTO_REQ_SA_CONTROL_WORD)

    // move sa control word into gpr
    move(r_sa_ctlwrd, CRYPTO_REQ_SA_CONTROL_WORD)


    // tell compiler scope of $xfr,$xfrw
    .begin
    .reg $xfr[16]
    .reg $xfrw[4]
    .xfer_order $xfr
    .xfer_order $xfrw


    // If SA already in the current buffer is same as needed for this packet,
    // skip reading it in again. Assuming all sa's will be provided from same
    // memory island, so not comparing upper address byte. Bits 8:0 of SA
    // Address are the SA 'version' and are part of comparison but not used
    // for addressing. If host re-uses SA table location for a new SA, it
    // must change the version so we know the cached SA, if any, is not
    // valid anymore. Note version bits located in the sa itself are not used.
    // Optionally, the 'sa flush' flag word in cls provides a way for user code
    // to 'flush' any saved sa data by invoking crypto_threads_set_sa_flush_flags().
    // This flush mechanism should be used if an sa may be updated 512 times
    // without a packet being transmitted, as the sa 'version' field could wrap
    // and the change missed by this code. To enable the flush mechanism, the file
    // should be built with #define ENABLE_SA_FLUSH 1. By default it is disabled.
    #if ( ENABLE_SA_FLUSH == 1 )
    crypto_threads_tst_clr_sa_flush_flag(r_tmp2, _crypto_ctx, r_buf)
    .if ( r_tmp2 == 0 && \
            CRYPTO_REQ_SA_ADDR == r_tmp1 )
    #else
    .if (CRYPTO_REQ_SA_ADDR == r_tmp1 )
    #endif
        // re-use sa
        localmem_read1(r_sa_ctlwrd, r_tmp, CRYPTO_LM_SA_CTLWRD)
        crypto_threads_update_counter(CRYPTO_CNTR_SA_REUSE)
        br[crypto_input_thread_replay_check#]

    .else

        // TBD implement mechanism to cache SA data in crypto island CTM to avoid
        // reading it from MU on every packet. or user of this code may do it

        // check sa ctlwrd for invalid cipher, mode, etc.
        #if ( ENABLE_ERROR_CHECKS == 1 )
        crypto_threads_check_sa(r_tmp, r_sa_ctlwrd)
        .if ( r_tmp != 0 )
            // trash saved value of sa addr so won't get a match
            // on next packet
            crypto_threads_get_lm_temp_addr(r_tmp, _me_ctx, r_buf)
            move(r_tmp1, 0xffffffff)
            localmem_write1(r_tmp1, r_tmp, CRYPTO_LM_SA_ADDR)
            // send failure response
            crypto_threads_input_send_response( _me_ctx, r_buf, \
                     CRYPTO_RSP_STATUS_SA_SELECTION_ERR )
            br[crypto_input_thread_loop#]
        .endif
        #endif

        // read SA key data to crypto sram
        crypto_threads_get_sa_addr(r_sram_sa, _crypto_ctx, r_buf)
        alu[r_sram_sa, r_sram_sa, +, CRYPTO_SA_DMA_KEYS_OFFSET]
        move(r_tmp, CRYPTO_REQ_SA_VERSION_MASK) // mask off version bits
        alu[r_tmp1, CRYPTO_REQ_SA_ADDR, AND~, r_tmp]
        alu[r_tmp1, r_tmp1, OR, CRYPTO_SA_KEYS_OFFSET, <<2]
        ld_field_w_clr[r_tmp, 0001, \
            CRYPTO_REQ_SA_ADDR_UPPER, >>CRYPTO_REQ_SA_ADDR_UPPER_SHIFT]
        crypto_dma_in ($xfr, _crypto_ctx, CHAINED, CHAINED_NOT_LAST, \
                                               r_sram_sa, r_tmp1, \
                                                r_tmp, CRYPTO_SA_DMA_KEYS_LEN, --)
    .endif



    //
    // do preliminary anti replay check here
    //
    crypto_input_thread_replay_check#:
    #if ( ENABLE_ANTI_REPLAY == 1 )
    alu[r_dir, CRYPTO_SA_DIR_MASK, AND, r_sa_ctlwrd, >>CRYPTO_SA_DIR_SHIFT]
    .if ( r_dir == CRYPTO_DIR_DECRYPT )
        .if ( r_sa_ctlwrd & ( 1 << CRYPTO_SA_AR_ENA_SHIFT ))
            .begin
            .sig z
            .reg r_status
            .reg $x

            // read the 32 bit sequence number from the packet.
            // assuming it is in the start of packet area. if not,
            // need to fix this
            ld_field_w_clr[r_tmp, 0001, \
                    CRYPTO_REQ_PKT_STRT_ADDR_UPPER, \
                    >>CRYPTO_REQ_PKT_STRT_ADDR_UPPER_SHIFT]
            alu[r_tmp, --, B, r_tmp, <<24] // upper byte
            ld_field_w_clr[r_tmp1, 0011, CRYPTO_REQ_AUTH_HDR_OFFSET, \
                                     >>CRYPTO_REQ_AUTH_HDR_OFFSET_SHIFT]
            alu[r_tmp1, r_tmp1, +, CRYPTO_REQ_PKT_STRT_ADDR]
            alu[r_tmp2, CRYPTO_SA_PROTOCOL_MASK, AND, r_sa_ctlwrd, \
                                  >>CRYPTO_SA_PROTOCOL_SHIFT]
            .if ( r_tmp2 == CRYPTO_PROTOCOL_ESP )
                alu[r_tmp1, r_tmp1, +, 4] // seq is 2nd lword in auth hdr
            .else
                alu[r_tmp1, r_tmp1, +, 8] // seq is 3rd lword in auth hdr
            .endif
            mem[read32, $x, r_tmp, <<8, r_tmp1, 1], ctx_swap[z]

            // save the seq number in lm for processing in output thread
            crypto_threads_get_lm_temp_addr(r_tmp, _me_ctx, r_buf)
            localmem_write1($x, r_tmp, CRYPTO_LM_PKT_SEQ_NUM_LOW)

            // now do the anti replay check
            move(r_status, CRYPTO_RSP_STATUS_OK)
            crypto_threads_anti_replay(_me_ctx, r_buf, r_status, 0)
            .if ( r_status != CRYPTO_RSP_STATUS_OK )
                crypto_threads_input_send_response( _me_ctx, r_buf, r_status)
                br[crypto_input_thread_loop#]
            .endif
            .end
        .endif
    .endif
    #endif



    //
    // generate params for use by crypto sequence
    //
    crypto_input_thread_gen_params#:

    crypto_threads_get_pkt_addr(r_sram_pkt, _crypto_ctx, r_buf)
    alu[r_tmp, CRYPTO_REQ_PKT_STRT_ADDR, AND, 7]
    // actual pkt start in sram is offset for non-8 byte alignment
    alu[r_sram_pkt, r_sram_pkt, +, r_tmp]

    alu[r_tmp, CRYPTO_SA_PROTOCOL_MASK, AND, r_sa_ctlwrd, \
                                  >>CRYPTO_SA_PROTOCOL_SHIFT]

    // ESP, either transport or tunnel
    .if ( r_tmp == CRYPTO_PROTOCOL_ESP )
        ld_field_w_clr[r_tmp, 0011, CRYPTO_REQ_PTEXT_OFFSET, \
                                 >>CRYPTO_REQ_PTEXT_OFFSET_SHIFT]
        alu[r_sram_ptext, r_sram_pkt, +, r_tmp]
        ld_field_w_clr[r_ptext_size, 0011, CRYPTO_REQ_PTEXT_LENGTH, \
                                 >>CRYPTO_REQ_PTEXT_LENGTH_SHIFT]
        ld_field_w_clr[r_tmp, 0011, CRYPTO_REQ_AUTH_HDR_OFFSET, \
                              >>CRYPTO_REQ_AUTH_HDR_OFFSET_SHIFT]
        alu[r_sram_spi, r_sram_pkt, +, r_tmp]
        ld_field_w_clr[r_tmp, 0011, CRYPTO_REQ_IV_OFFSET, \
                                    >>CRYPTO_REQ_IV_OFFSET_SHIFT]
        alu[r_sram_iv, r_sram_pkt, +, r_tmp]
        alu[r_dir, CRYPTO_SA_DIR_MASK, AND, r_sa_ctlwrd, >>CRYPTO_SA_DIR_SHIFT]
        alu[r_cipher, CRYPTO_SA_CIPHER_MASK, AND, r_sa_ctlwrd, \
                                        >>CRYPTO_SA_CIPHER_SHIFT]
        alu[r_cimode, CRYPTO_SA_CIMODE_MASK, AND, r_sa_ctlwrd, \
                                        >>CRYPTO_SA_CIMODE_SHIFT]
        alu[r_hash, CRYPTO_SA_HASH_MASK, AND, r_sa_ctlwrd, >>CRYPTO_SA_HASH_SHIFT]
        .if ( r_hash == CRYPTO_HASH_NONE ) // hash none hangs crypto sequence
            move(r_hash, CRYPTO_HASH_SHA_1)
        .endif
        #pragma warning(disable:5008)
        crypto_setup_configs(r_cfg_hlen, r_iv_len, r_cfg_select, r_cfg_modes, \
                               r_dir, r_cipher, r_cimode, r_hash)
        #pragma warning(default:5008)
        alu[r_auth_len, r_iv_len, +, 7] // spi + seq + iv - 1
        crypto_threads_get_vars_addr(r_sram_vars, _crypto_ctx, r_buf)
        // save icv/auth data length needed by output thread in local memory
        crypto_threads_get_lm_temp_addr(r_tmp, _me_ctx, r_buf)
        localmem_write1(r_cfg_hlen, r_tmp, CRYPTO_LM_HASH_LENGTH)

    // AH, either transport or tunnel
    .elif ( r_tmp == CRYPTO_PROTOCOL_AH )

        // The crypto cmd sequence is setup for esp mode, with the
        // packet order of:
        // ip hdr - auth hdr - iv - plaintext data - auth data.
        // But for AH mode the actual packet order is:
        // ip hdr - auth hdr - auth data - ip payload and no iv.
        // And the plain text offset param in the cmd is pointing at the
        // outer ip hdr. so in order to use the same crypto sequence, doing
        // some fudging around of the paramters to the sequence here.
        ld_field_w_clr[r_tmp, 0011, CRYPTO_REQ_PTEXT_OFFSET, \
                                 >>CRYPTO_REQ_PTEXT_OFFSET_SHIFT]
        alu[r_sram_spi, r_sram_pkt, +, r_tmp] // tell crypto sequence auth hdr
                                              // is at the start of the data
                                              // to be hashed
        move(r_auth_len, 27) // tell crypto sequence the auth length is 28 ( -1 )
                             // 'spi' = 4, 'seq' = 8, 'iv' = 16

        alu[r_sram_iv, r_sram_spi, +, 12] // tell crypto sequence 'iv' is after
                                          // 'esp hdr' ( spi + seq ).
                                          // doesn't really exist, but the crypto
                                          // sequence loads it only for the cipher
                                          // (cipher is also unused for ah )

        // tell crypto sequence the 'text' is after the 'iv'
        move(r_sram_ptext, 16)
        alu[r_sram_ptext, r_sram_ptext, +, r_sram_iv]

        // tell crypto sequence the 'text size' is the actual length to be hashed,
        // less the size of the pretend 'esp hdr' + 'iv'
        ld_field_w_clr[r_ptext_size, 0011, CRYPTO_REQ_PTEXT_LENGTH, \
                                 >>CRYPTO_REQ_PTEXT_LENGTH_SHIFT]
        move(r_tmp, 28)
        alu[r_ptext_size, r_ptext_size, -, r_tmp]

        alu[r_dir, CRYPTO_SA_DIR_MASK, AND, r_sa_ctlwrd, >>CRYPTO_SA_DIR_SHIFT]
        move(r_cipher, CRYPTO_CIPHER_NONE) // no cipher for AH mode
        move(r_cimode, CRYPTO_CIMODE_CBC)  // not used
        alu[r_hash, CRYPTO_SA_HASH_MASK, AND, r_sa_ctlwrd, >>CRYPTO_SA_HASH_SHIFT]
        .if ( r_hash == CRYPTO_HASH_NONE ) // hash none hangs crypto sequence
            move(r_hash, CRYPTO_HASH_SHA_1)
        .endif
        #pragma warning(disable:5008) // r_iv_len is not used here
        crypto_setup_configs(r_cfg_hlen, r_iv_len, r_cfg_select, r_cfg_modes, \
                               r_dir, r_cipher, r_cimode, r_hash)
        #pragma warning(default:5008)
        crypto_threads_get_vars_addr(r_sram_vars, _crypto_ctx, r_buf)
        // save icv/auth data length needed by output thread in local memory
        crypto_threads_get_lm_temp_addr(r_tmp, _me_ctx, r_buf)
        localmem_write1(r_cfg_hlen, r_tmp, CRYPTO_LM_HASH_LENGTH)
    .else
        // shouldn't get here
        crypto_threads_input_send_response(  _me_ctx, r_buf, \
                                      CRYPTO_RSP_STATUS_SA_SELECTION_ERR )
        br[crypto_input_thread_loop#]
    .endif


    #if ( ENABLE_JUMBO_PKTS == 1 )
    // adjust ptext_size if needed, will only come through
    // here on the 1st buffer of a jumbo pkt. this is 1st
    // spot that the jumbo pkt is detected, and the jumbo
    // 'state' is updated here
    alu[r_tmp1, r_sram_ptext, +, r_ptext_size]
    crypto_threads_get_pkt_addr(r_tmp, _crypto_ctx, r_buf)
    alu[r_tmp1, r_tmp1, -, r_tmp] // byte count to end of ptext
    move( r_tmp, CRYPTO_MAX_SRAM_BUF_BYTES )
    .if ( r_tmp1 > r_tmp )
        // must be a jumbo, ptext runs past end of buffer
        crypto_threads_get_pkt_addr(r_tmp1, _crypto_ctx, r_buf)
        alu[r_ptext_size, r_sram_ptext, -, r_tmp1]
        alu[r_ptext_size, r_tmp, -, r_ptext_size]
        .if ( r_hash < CRYPTO_HASH_SHA_384 )
            // text length has to be multiple of 512 bits for
            // MD5, SHA1, SHA256 hashes
            alu[r_ptext_size, r_ptext_size, AND~, 0x3f]
        .else
            // ..or a multiple of 1024 bits for
            // SHA384 or SHA512 hashes
            alu[r_ptext_size, r_ptext_size, AND~, 0x7f]
        .endif

        // save working ptext offset for use on next buffer
        crypto_threads_get_lm_temp_addr(r_tmp, _me_ctx, 0)
        localmem_write1(r_ptext_size, r_tmp, \
                    CRYPTO_LM_COMM_JUMBO_PKT_PTEXT_OFF)

        // save address of hash key for use on next buffer
        crypto_threads_get_hash_key_addr(r_tmp1, _crypto_ctx, r_buf)
        localmem_write1(r_tmp1, r_tmp, \
                    CRYPTO_LM_COMM_JUMBO_HKEY_ADDR)

        // set jumbo state to 1st buffer of jumbo pkt
        localmem_write1(CRYPTO_LM_JUMBO_STATE_FIRST_BUF, \
                        r_tmp, CRYPTO_LM_COMM_JUMBO_STATE)

        // set jumbo state in the buffer specific lm
        crypto_threads_get_lm_temp_addr(r_tmp, _me_ctx, r_buf)
        localmem_write1(CRYPTO_LM_JUMBO_STATE_FIRST_BUF, \
                        r_tmp, CRYPTO_LM_JUMBO_STATE)

        crypto_threads_update_counter(CRYPTO_CNTR_JUMBO_FIRST)

    .endif

    br[crypto_input_thread_gjp_end#] // normal flow skips next section


    // code to generate crypto sequence parameters for jumbo packets,
    // when processing 'continuation' or 'end' packet buffer
    // top of input loop branches here when previous buffer processed
    // was for a jumbo packet. the jumbo state is also updated here.
    crypto_input_thread_gen_jumbo_params#:

    // update sequence vars temp address for this buffer
    crypto_threads_get_vars_addr(r_sram_vars, _crypto_ctx, r_buf)

    // 'plaintext' location will be at start of sram, adjusted for
    // alignment of end of previous buffer
    #pragma warning(disable:4701) // r_sram_ptext, r_ptext_size set from prev. buffer
    alu[r_tmp, r_sram_ptext, +, r_ptext_size]
    #pragma warning(default:4701)
    alu[r_tmp, r_tmp, AND, 7]
    crypto_threads_get_pkt_addr(r_sram_ptext, _crypto_ctx, r_buf)
    alu[r_sram_ptext, r_sram_ptext, +, r_tmp]

    // 'plaintext' size is rest of packet that fits in sram buffer
    #pragma warning(disable:4701) // CRYPTO_REQ_PTEXT_LENGTH still set from
                                  // previous buffer
    ld_field_w_clr[r_ptext_size, 0011, CRYPTO_REQ_PTEXT_LENGTH, \
                                 >>CRYPTO_REQ_PTEXT_LENGTH_SHIFT]
    #pragma warning(default:4701)
    crypto_threads_get_lm_temp_addr(r_tmp, _me_ctx, 0)
    localmem_read1(r_tmp1, r_tmp, CRYPTO_LM_COMM_JUMBO_PKT_PTEXT_OFF)
    alu[r_ptext_size, r_ptext_size, -, r_tmp1] // bytes left to go

    // figure max buffer available
    move(r_tmp, CRYPTO_MAX_SRAM_BUF_BYTES)
    alu[r_tmp1, r_sram_ptext, AND, 7]
    alu[r_tmp, r_tmp, -, r_tmp1]

    .if ( r_ptext_size > r_tmp )
        // this is a 'continuation', still more to go after
        // this buffer, set the ptext size to the max
        move(r_ptext_size, r_tmp)

        #pragma warning(disable:4701) // r_hash still set from previous buffer
        .if ( r_hash < CRYPTO_HASH_SHA_384 )
            #pragma warning(default:4701)
            // text length has to be multiple of 512 bits for
            // MD5, SAH1, SHA256 hashes
            alu[r_ptext_size, r_ptext_size, AND~, 0x3f]
        .else
            // ..or a multiple of 1024 bits for
            // SHA384 or SHA512 hashes
            alu[r_ptext_size, r_ptext_size, AND~, 0x7f]
        .endif

        // save working ptext offset for use on next buffer
        crypto_threads_get_lm_temp_addr(r_tmp, _me_ctx, 0)
        localmem_read1(r_tmp1, r_tmp, \
                    CRYPTO_LM_COMM_JUMBO_PKT_PTEXT_OFF)
        alu[r_tmp1, r_tmp1, +, r_ptext_size]
        localmem_write1(r_tmp1, r_tmp, \
                    CRYPTO_LM_COMM_JUMBO_PKT_PTEXT_OFF)

        // set the new jumbo state
        localmem_write1(CRYPTO_LM_JUMBO_STATE_CONTINUE_BUF, \
                        r_tmp, CRYPTO_LM_COMM_JUMBO_STATE)
        crypto_threads_get_lm_temp_addr(r_tmp, _me_ctx, r_buf)
        localmem_write1(CRYPTO_LM_JUMBO_STATE_CONTINUE_BUF, \
                        r_tmp, CRYPTO_LM_JUMBO_STATE)
        // also need to pass along req params for output thread
        #pragma warning(disable:4701) // REQ_* still set from previous buffer
        localmem_write8(CRYPTO_REQ_PKT_WRBK_ADDR_UPPER, \
                        CRYPTO_REQ_PKT_END_ADDR_UPPER, \
                        CRYPTO_REQ_SA_ADDR, \
                        CRYPTO_REQ_PKT_STRT_ADDR, \
                        CRYPTO_REQ_PKT_CONT_ADDR, \
                        CRYPTO_REQ_PKT_END_ADDR, \
                        CRYPTO_REQ_PKT_WRBK_ADDR, \
                        CRYPTO_REQ_PKT_STRT_LENGTH, \
                        r_tmp, CRYPTO_LM_PKT_WRBK_ADDR_UPPER)
        localmem_write_next(CRYPTO_REQ_PKT_END_LENGTH)
        localmem_write_next(CRYPTO_REQ_PKT_WRBK_END_LENGTH)
        localmem_write_next(CRYPTO_REQ_AUTH_HDR_OFFSET)
        localmem_write_next(CRYPTO_REQ_AUTH_DATA_OFFSET)
        localmem_write_next(r_sa_ctlwrd)
        localmem_write_next(r_cfg_hlen)
        #pragma warning(default:4701)

        crypto_threads_update_counter(CRYPTO_CNTR_JUMBO_CONTINUE)
    .else
        // this is the last buffer of the packet,
        // ptext_size is fine as is

        // update jumbo state
        crypto_threads_get_lm_temp_addr(r_tmp, _me_ctx, 0)
        localmem_write1(CRYPTO_LM_JUMBO_STATE_END_BUF, \
                        r_tmp, CRYPTO_LM_COMM_JUMBO_STATE)

        // update buffer specific jumbo state
        crypto_threads_get_lm_temp_addr(r_tmp, _me_ctx, r_buf)
        localmem_write1(CRYPTO_LM_JUMBO_STATE_END_BUF, \
                        r_tmp, CRYPTO_LM_JUMBO_STATE)
        // also need to pass along req params for output thread
        #pragma warning(disable:4701) // REQ_* still set from previous buffer
        localmem_write8(CRYPTO_REQ_PKT_WRBK_ADDR_UPPER, \
                        CRYPTO_REQ_PKT_END_ADDR_UPPER, \
                        CRYPTO_REQ_SA_ADDR, \
                        CRYPTO_REQ_PKT_STRT_ADDR, \
                        CRYPTO_REQ_PKT_CONT_ADDR, \
                        CRYPTO_REQ_PKT_END_ADDR, \
                        CRYPTO_REQ_PKT_WRBK_ADDR, \
                        CRYPTO_REQ_PKT_STRT_LENGTH, \
                        r_tmp, CRYPTO_LM_PKT_WRBK_ADDR_UPPER)
        localmem_write_next(CRYPTO_REQ_PKT_END_LENGTH)
        localmem_write_next(CRYPTO_REQ_PKT_WRBK_END_LENGTH)
        localmem_write_next(CRYPTO_REQ_AUTH_HDR_OFFSET)
        localmem_write_next(CRYPTO_REQ_AUTH_DATA_OFFSET)
        localmem_write_next(r_sa_ctlwrd)
        localmem_write_next(r_cfg_hlen)
        #pragma warning(default:4701)

        crypto_threads_update_counter(CRYPTO_CNTR_JUMBO_END)

    .endif

    // skip down to the dma in section
    br[crypto_input_thread_dma_in#]
    crypto_input_thread_gjp_end#:

    #endif // ENABLE_JUMBO_PKTS


    // For AH 'decrypt' ( a.k.a. 'inbound') need to read in the
    // auth data to temp var area in sram now, then zero it in memory
    // for read in to sram for hash calculation. N.B. zero'ing it in
    // memory because dma in is chained back to back with 'decrypt'
    // operation and me won't get control back until 'decrypt' is
    // completed, so can't zero the auth data directly in crypto sram,
    // and can't skip the dma around it because of h/w alignment issues.
    alu[r_tmp, CRYPTO_SA_PROTOCOL_MASK, AND, r_sa_ctlwrd, \
                                  >>CRYPTO_SA_PROTOCOL_SHIFT]
    .if ( r_tmp == CRYPTO_PROTOCOL_AH )
        .if ( r_dir == CRYPTO_DIR_DECRYPT )
            // but don't do any of this for HASH_NONE. This is a nonsense
            // case ( i.e. AH with no hash ) but it has come up in testing
            // note that to get around hardware hang on hash none, the
            // h/w will be set to do SHA1, but we will ignore the hash code
            // in the output thread
            alu[r_tmp, CRYPTO_SA_HASH_MASK, AND, r_sa_ctlwrd,\
                                         >>CRYPTO_SA_HASH_SHIFT]
            .if ( r_tmp != CRYPTO_HASH_NONE )

                // read the auth data in from memory. Assuming it is all
                // in the start of packet area. if not need to fix this
                ld_field_w_clr[r_pkt_strt_addr_hi, 0001, \
                            CRYPTO_REQ_PKT_STRT_ADDR_UPPER, \
                        >>CRYPTO_REQ_PKT_STRT_ADDR_UPPER_SHIFT]
                alu[r_pkt_strt_addr_hi, --, B, r_pkt_strt_addr_hi, <<24] // upper byte
                ld_field_w_clr[r_auth_data_addr, 0011, \
                        CRYPTO_REQ_AUTH_DATA_OFFSET, \
                        >>CRYPTO_REQ_AUTH_DATA_OFFSET_SHIFT]
                alu[r_auth_data_addr, r_auth_data_addr, +, \
                        CRYPTO_REQ_PKT_STRT_ADDR] // addr bits 31:0
                alu[r_tmp, r_cfg_hlen, +, 1] // auth data byte length
                alu[r_tmp, --, B, r_tmp, >>2] // length in lwords
                alu[--, 0x80, OR, r_tmp, <<8] // indirect ref
                #pragma warning(disable:5003)
                mem[read32, $xfr[0], r_pkt_strt_addr_hi, <<8, r_auth_data_addr, \
                    MAX_16], indirect_ref, ctx_swap[s]
                #pragma warning(default:5003)
                // write the auth data into the 2nd 1/2 of the sram 'vars' location
                // crypto writes in 8 byte chunks. maximum auth data is 16 lwords
                alu[r_tmp1, r_sram_vars, +, 0x40]
                // lwords 0 - 3
                aggregate_copy($xfrw, 0, $xfr, 0, 4)
                crypto[write, $xfrw[0], 0, r_tmp1, 2], ctx_swap[s]
                .if ( r_tmp > 4 ) // lwords 4-7
                    aggregate_copy($xfrw, 0, $xfr, 4, 4)
                    crypto[write, $xfrw[0], 0x10, r_tmp1, 2], ctx_swap[s]
                    .if ( r_tmp > 8 ) // lwords 8-11
                        aggregate_copy($xfrw, 0, $xfr, 8, 4)
                        crypto[write, $xfrw[0], 0x20, r_tmp1, 2], ctx_swap[s]
                        .if ( r_tmp > 12 ) // lwords 12-15
                            aggregate_copy($xfrw, 0, $xfr, 12, 4)
                            crypto[write, $xfrw[0], 0x30, r_tmp1, 2], ctx_swap[s]
                        .endif
                    .endif
                .endif


                // now zero out the auth data in the packet
                // currently valid auth data lengths are 4,5,8,12,16 lwords
                aggregate_zero($xfrw, 4)
                mem[write32, $xfrw[0], r_pkt_strt_addr_hi, <<8, \
                    r_auth_data_addr, 4], ctx_swap[s]
                .if ( r_tmp > 4 )
                    alu[r_auth_data_addr, r_auth_data_addr, +, 0x10]
                    mem[write32, $xfrw[0], r_pkt_strt_addr_hi, <<8, \
                        r_auth_data_addr, 1], ctx_swap[s]
                    .if ( r_tmp > 5 )
                        alu[r_auth_data_addr, r_auth_data_addr, +, 0x4]
                        mem[write32, $xfrw[0], r_pkt_strt_addr_hi, <<8, \
                            r_auth_data_addr, 3], ctx_swap[s]
                        .if ( r_tmp > 8 )
                            alu[r_auth_data_addr, r_auth_data_addr, +, 0xC]
                            mem[write32, $xfrw[0], r_pkt_strt_addr_hi, <<8, \
                                r_auth_data_addr, 4], ctx_swap[s]
                            .if ( r_tmp > 12 )
                                alu[r_auth_data_addr, r_auth_data_addr, +, 0x10]
                                mem[write32, $xfrw[0], r_pkt_strt_addr_hi, <<8, \
                                    r_auth_data_addr, 4], ctx_swap[s]
                            .endif
                        .endif
                    .endif
                .endif
            .endif
        .endif
    .endif



    //
    // read packet data to crypto sram
    //

    // move lengths to gprs
    ld_field_w_clr[r_pkt_strt_len, 0011, CRYPTO_REQ_PKT_STRT_LENGTH, \
                                           >>CRYPTO_REQ_PKT_STRT_LENGTH_SHIFT]

    ld_field_w_clr[r_pkt_cont_len, 0011, CRYPTO_REQ_PKT_CONT_LENGTH, \
                                           >>CRYPTO_REQ_PKT_CONT_LENGTH_SHIFT]

    ld_field_w_clr[r_pkt_end_len, 0011, CRYPTO_REQ_PKT_END_LENGTH, \
                                           >>CRYPTO_REQ_PKT_END_LENGTH_SHIFT]

    ld_field_w_clr[r_pkt_wrbk_end_len, 0011, CRYPTO_REQ_PKT_WRBK_END_LENGTH, \
                                          >>CRYPTO_REQ_PKT_WRBK_END_LENGTH_SHIFT]

    ld_field_w_clr[r_pkt_end_addr_hi, 0001, CRYPTO_REQ_PKT_END_ADDR_UPPER, \
                                         >>CRYPTO_REQ_PKT_END_ADDR_UPPER_SHIFT]


    crypto_input_thread_dma_in#: // br point for jumbo handling

    #pragma warning(disable:4701) // CRYPTO_REQ_* still set from previous buffer
    move(r_pkt_end_addr, CRYPTO_REQ_PKT_END_ADDR)
    #pragma warning(default:4701)

    #if ( ENABLE_JUMBO_PKTS == 1 )
    // adjust dma in operations for handling jumbo packets

    crypto_threads_get_lm_temp_addr(r_tmp, _me_ctx, 0)
    localmem_read1(r_tmp1, r_tmp, CRYPTO_LM_COMM_JUMBO_STATE)

    .if ( r_tmp1 == CRYPTO_LM_JUMBO_STATE_FIRST_BUF )

        // processing 1st buffer of a jumbo. we already know the
        // entire packet can't fit in one buffer.
        // for simplicity, assuming that r_pkt_strt_len + r_pkt_cont_len < 2K
        // so, to go over 2K, has to be in the end of packet area ( i.e. dram )

        // adjust end length to be processed with this sram buffer
        crypto_threads_get_pkt_addr(r_tmp1, _crypto_ctx, r_buf)
        #pragma warning(disable:4701) // CRYPTO_REQ_* are valid from read in
        alu[r_tmp, CRYPTO_REQ_PKT_STRT_ADDR, AND, 7]
        #pragma warning(default:4701)
        alu[r_tmp1, r_tmp1, +, r_tmp] // r_tmp1 is 1st used addr in sram
        alu[r_tmp1, r_sram_ptext, -, r_tmp1] // r_tmp1 is #bytes up to ptext
        alu[r_pkt_end_len, r_tmp1, +, r_ptext_size]  // total used in buf
        #pragma warning(disable:4701) // r_pkt_strt,cont_len valid from read in
        alu[r_pkt_end_len, r_pkt_end_len, -, r_pkt_strt_len] // less strt
        // for decrypt option 1, ESP the continuation of packet address is
        // used to 'pass through' the writeback address for the response,
        // so in that case do not use it here
        alu[r_tmp, CRYPTO_SA_PROTOCOL_MASK, AND, r_sa_ctlwrd, \
                                  >>CRYPTO_SA_PROTOCOL_SHIFT]
        .if ( (r_tmp == CRYPTO_PROTOCOL_ESP) && \
              (CRYPTO_REQ_DECRYPT_OPTION & (1 << CRYPTO_REQ_DECRYPT_OPTION_BIT)))
            // do nothing
        .else
            alu[r_pkt_end_len, r_pkt_end_len, -, r_pkt_cont_len] // less cont
        .endif
        #pragma warning(default:4701)

        // save current end of packet area offset in 'common' lm
        crypto_threads_get_lm_temp_addr(r_tmp, _me_ctx, 0)
        localmem_write1(r_pkt_end_len, r_tmp, \
                         CRYPTO_LM_COMM_JUMBO_PKT_END_OFF)

    .elif ( r_tmp1 == CRYPTO_LM_JUMBO_STATE_CONTINUE_BUF )

        // processing a jumbo 'continue' buffer,
        // size of buffer for start/continue is 0
        move(r_pkt_strt_len, 0)
        move(r_pkt_cont_len, 0)
        // and end length is same as ptext size
        move(r_pkt_end_len, r_ptext_size)

        // reset sram addr to start of buffer
        crypto_threads_get_pkt_addr(r_sram_pkt, _crypto_ctx, r_buf)

        // advance the end addr by the amount of 'end' data
        // already read in
        crypto_threads_get_lm_temp_addr(r_tmp, _me_ctx, 0)
        localmem_read1(r_tmp1, r_tmp, CRYPTO_LM_COMM_JUMBO_PKT_END_OFF)
        #pragma warning(disable:4701) // CRYPTO_REQ_* are valid from prev. buffer
        alu[r_pkt_end_addr, CRYPTO_REQ_PKT_END_ADDR, +, r_tmp1]
        #pragma warning(default:4701)
        // and save current end of packet area offset
        alu[r_tmp1, r_tmp1, +, r_pkt_end_len]
        localmem_write1(r_tmp1, r_tmp, \
            CRYPTO_LM_COMM_JUMBO_PKT_END_OFF)

        // save the end addr & ptext addr  in buffer specific
        // lm for output thread to copy out non-8 byte aligned data
        crypto_threads_get_lm_temp_addr(r_tmp, _me_ctx, r_buf)
        localmem_write1(r_pkt_end_addr, r_tmp, \
            CRYPTO_LM_JUMBO_BUF_END_ADDR)
        localmem_write1(r_sram_ptext, r_tmp, \
            CRYPTO_LM_JUMBO_BUF_SRAM_PTEXT)

        // if the new end addr is not 8 byte aligned,
        // need to back it up to previous 8 byte address
        .if ( r_pkt_end_addr & 7 )
            alu[r_pkt_end_addr, r_pkt_end_addr, AND~, 7]
            // and add one more 8 byte dma in to compensate
            alu[r_pkt_end_len, r_pkt_end_len, +, 8]
        .endif


    .elif ( r_tmp1 == CRYPTO_LM_JUMBO_STATE_END_BUF )

        // processing a jumbo 'end' buffer, so it has the remaining
        // size of buffer for 'end' and 0 for start/continue
        move(r_pkt_strt_len, 0)
        move(r_pkt_cont_len, 0)

        // end length is original total end length less amount
        // already read in ( this includes auth data for esp decrypt,
        // which is not included in 'plaintext' length)
        #pragma warning(disable:4701) // CRYPTO_REQ_* are valid from prev. buffer
        ld_field_w_clr[r_pkt_end_len, 0011, CRYPTO_REQ_PKT_END_LENGTH, \
                                           >>CRYPTO_REQ_PKT_END_LENGTH_SHIFT]
        #pragma warning(default:4701)
        crypto_threads_get_lm_temp_addr(r_tmp, _me_ctx, 0)
        localmem_read1(r_tmp1, r_tmp, CRYPTO_LM_COMM_JUMBO_PKT_END_OFF)
        alu[r_pkt_end_len, r_pkt_end_len, -, r_tmp1]

        // reset sram address to start of buffer
        crypto_threads_get_pkt_addr(r_sram_pkt, _crypto_ctx, r_buf)

        // advance the end addr by the amount of 'end' data
        // already read in
        #pragma warning(disable:4701) // CRYPTO_REQ_* are valid from prev. buffer
        alu[r_pkt_end_addr, CRYPTO_REQ_PKT_END_ADDR, +, r_tmp1]
        #pragma warning(default:4701)

        // save the memory end addr and ptext sram addr in buffer
        // specific lm for output thread to copy out non-8 byte aligned data
        crypto_threads_get_lm_temp_addr(r_tmp, _me_ctx, r_buf)
        localmem_write1(r_pkt_end_addr, r_tmp, \
            CRYPTO_LM_JUMBO_BUF_END_ADDR)
        localmem_write1(r_sram_ptext, r_tmp, \
            CRYPTO_LM_JUMBO_BUF_SRAM_PTEXT)

        // save the auth data sram addr in buffer specific
        // lm for output thread to compare with calculated value
        // during esp decrypts
        alu[r_tmp1, r_sram_ptext, +, r_pkt_end_len]
        #pragma warning(disable:4701) // r_cfg_hlen valid from prev. buffer
        alu[r_tmp1, r_tmp1, -, r_cfg_hlen]
        #pragma warning(default:4701)
        alu[r_tmp1, r_tmp1, -, 1]
        localmem_write1(r_tmp1, r_tmp, \
            CRYPTO_LM_JUMBO_BUF_SRAM_AUTH_DATA)


        // if the new end addr is not 8 byte aligned,
        // need to back it up to previous 8 byte address
        .if ( r_pkt_end_addr & 7 )
            alu[r_pkt_end_addr, r_pkt_end_addr, AND~, 7]
            // and add one more 8 byte dma in to compensate
            alu[r_pkt_end_len, r_pkt_end_len, +, 8]
        .endif

    .endif
    #endif

    #if ( ENABLE_JUMBO_PKTS == 1 )
    #pragma warning(disable:4701) // r_pkt_strt_len, etc. are set from read in.
                                  // problem flagged due to jump to
                                  // crypto_input_thread_dma_in# for jumbos
    #endif

    // start of packet ( typically in ctm )
    .if ( r_pkt_strt_len > 0 )
        alu[r_tmp, r_pkt_strt_len, AND, 7]
        alu[r_tmp1, CRYPTO_REQ_PKT_STRT_ADDR, AND, 7]
        .if ( r_tmp == 0 ) // 8 byte multiple length
            .if ( r_tmp1 == 0 ) // 8 byte aligned address
                // do nothing
            .else
                // non 8 byte aligned addr, add another 8 byte block to length
                alu[r_pkt_strt_len, r_pkt_strt_len, +, 8]
            .endif
        .else // non 8 byte multiple length
            .if ( r_tmp1 == 0 ) // 8 byte aligned address
                // round up length to next 8 byte multiple
                alu[r_pkt_strt_len, r_pkt_strt_len, +, 8]
                alu[r_pkt_strt_len, r_pkt_strt_len, -, r_tmp]
            .else
                // non 8 byte aligned address,
                // and non 8 byte length, round up length to next 8 byte multiple
                alu[r_pkt_strt_len, r_pkt_strt_len, +, 8]
                alu[r_pkt_strt_len, r_pkt_strt_len, -, r_tmp]
            .endif
        .endif
        crypto_threads_get_pkt_addr(r_sram_pkt, _crypto_ctx, r_buf)
        // N.B. the hardware accepts system mem addr bits 2:0 but zeros them out
        ld_field_w_clr[r_pkt_strt_addr_hi, 0001, \
          CRYPTO_REQ_PKT_STRT_ADDR_UPPER, >>CRYPTO_REQ_PKT_STRT_ADDR_UPPER_SHIFT]

        // for decrypt option 1, ESP, tunnel mode, the continuation of packet address
        // is used to 'pass through' the writeback address for the response,
        // so do not do a continue of packet dma in in this case
        alu[r_tmp, CRYPTO_SA_PROTOCOL_MASK, AND, r_sa_ctlwrd, \
                                  >>CRYPTO_SA_PROTOCOL_SHIFT]
        .if (r_tmp == CRYPTO_PROTOCOL_ESP)
            alu[r_tmp, CRYPTO_SA_PROTMODE_MASK, AND, r_sa_ctlwrd, \
                                  >>CRYPTO_SA_PROTMODE_SHIFT]
            .if ( (r_tmp == CRYPTO_PROTMODE_TUNNEL) && \
                  (CRYPTO_REQ_DECRYPT_OPTION & (1 << CRYPTO_REQ_DECRYPT_OPTION_BIT)))
                move(r_tmp, 0)
            .else
                move(r_tmp, 1)
            .endif
        .else
            move(r_tmp, 1)
        .endif

        .if ( r_pkt_end_len ) // have an end of packet transfer
            crypto_dma_in ($xfr, _crypto_ctx, CHAINED, CHAINED_NOT_LAST, \
                                    r_sram_pkt, CRYPTO_REQ_PKT_STRT_ADDR, \
                                    r_pkt_strt_addr_hi, r_pkt_strt_len, --)
        .elif ( r_pkt_cont_len && r_tmp ) // have a continue of packet transfer
            crypto_dma_in ($xfr, _crypto_ctx, CHAINED, CHAINED_NOT_LAST, \
                                    r_sram_pkt, CRYPTO_REQ_PKT_STRT_ADDR, \
                                    r_pkt_strt_addr_hi, r_pkt_strt_len, --)
        .else // only have the start of packet transfer
            // need 1 more block if was odd alignment
            alu[r_pkt_strt_len, r_pkt_strt_len, +, 8]
            crypto_dma_in ($xfr, _crypto_ctx, CHAINED, CHAINED_LAST, \
                                    r_sram_pkt, CRYPTO_REQ_PKT_STRT_ADDR, \
                                    r_pkt_strt_addr_hi, r_pkt_strt_len, --)
        .endif
        alu[r_sram_pkt, r_sram_pkt, +, r_pkt_strt_len]
    .endif


    // continuation of packet ( typically in ctm ) must be 8 byte aligned start
    // ( not used for dma in for decrypt option 1 )
    .if (r_tmp) // as above re esp, decrypt option 1
        .if ( r_pkt_cont_len > 0 )
            ld_field_w_clr[r_pkt_cont_addr_hi, 0001, \
                      CRYPTO_REQ_PKT_CONT_ADDR_UPPER, \
                       >>CRYPTO_REQ_PKT_CONT_ADDR_UPPER_SHIFT]
            .if ( r_pkt_end_len )
                crypto_dma_in ($xfr, _crypto_ctx, CHAINED, CHAINED_NOT_LAST, \
                                r_sram_pkt, CRYPTO_REQ_PKT_CONT_ADDR, \
                                r_pkt_cont_addr_hi, r_pkt_cont_len, --)
            .else
                // no end of packet, so ok if continue is not an 8 byte multiple
                // but need to round up the dma to get the last 1-7 bytes
                .if ( r_pkt_cont_len & 7 )
                    alu[r_tmp, r_pkt_cont_len, +, 8]
                    alu[r_tmp, r_tmp, AND~, 7]
                .else
                    alu[r_tmp, --, B, r_pkt_cont_len]
                .endif

                crypto_dma_in ($xfr, _crypto_ctx, CHAINED, CHAINED_LAST, \
                                r_sram_pkt, CRYPTO_REQ_PKT_CONT_ADDR, \
                                r_pkt_cont_addr_hi, r_tmp, --)
            .endif
            alu[r_sram_pkt, r_sram_pkt, +, r_pkt_cont_len]
        .endif
    .endif

    // end of packet ( typically in dram ) must be 8 byte aligned start address
    .if ( r_pkt_end_len > 0 )

        // if not an 8 byte multiple, round up dma to get last
        // 1-7 bytes
        .if ( r_pkt_end_len & 7 )
            alu[r_tmp, r_pkt_end_len, +, 8]
            alu[r_tmp, r_tmp, AND~, 7]
        .else
            alu[r_tmp, --, B, r_pkt_end_len]
        .endif

        crypto_dma_in ($xfr, _crypto_ctx, CHAINED, CHAINED_LAST, \
                                     r_sram_pkt, r_pkt_end_addr, \
                                          r_pkt_end_addr_hi, r_tmp, --)
    .endif

    #if ( ENABLE_JUMBO_PKTS == 1 )
    #pragma warning(default:4701) // r_pkt_strt_len, etc. set from read in
    #endif

    // end of tell compiler scope of $xfr,$xfrw
    .end


    // tell compiler scope of $runcmd, $dma_pkt_wrbk_out, etc.
    .begin
    .reg $runcmd[2], $dma_pkt_wrbk_out[4], $dma_pkt_end_out[4], $int_sig_msg[2]
    .xfer_order $runcmd, $dma_pkt_wrbk_out, $dma_pkt_end_out, $int_sig_msg


    //
    // load sequence params and generate crypto sequence cmd
    //
    crypto_input_thread_gen_crypto_sequence#:
/// @cond INTERNAL CRYPTO_PARAMS
    #ifdef SIMULATE_AES_GCM
    alu[r_tmp, --, B, r_ptext_size, >>1]
    #define CRYPTO_PARAMS r_sram_ptext, r_tmp, r_sram_spi, r_sram_iv, \
                          r_sram_vars,  r_auth_len, r_cfg_hlen, r_cfg_select, \
                          r_cfg_modes
    #else
    #define CRYPTO_PARAMS r_sram_ptext, r_ptext_size, r_sram_spi, r_sram_iv, \
                          r_sram_vars,  r_auth_len, r_cfg_hlen, r_cfg_select, \
                          r_cfg_modes
    #endif
/// @endcond
    #if ( ENABLE_JUMBO_PKTS == 1 )
    #pragma warning(disable:4701) // the CRYPTO_PARAMS are setup during the read in
                                  // of the 1st buffer, and are retained or adjusted
                                  // for subsequent buffers for jumbo pkts...
    #endif

    #pragma warning(disable:5008) // $runcmd may be setup but not used in case of
                                  // errors

    #if ( ENABLE_JUMBO_PKTS == 1 )
    crypto_threads_get_lm_temp_addr(r_tmp, _me_ctx, 0)
    localmem_read1(r_tmp1, r_tmp, CRYPTO_LM_COMM_JUMBO_STATE)
    #endif


    .if ( r_dir ==  CRYPTO_DIR_ENCRYPT )

        #if ( ENABLE_JUMBO_PKTS == 1 )
        .if ( (r_tmp1 == CRYPTO_LM_JUMBO_STATE_NON_JUMBO) \
              || (r_tmp1 == CRYPTO_LM_JUMBO_STATE_FIRST_BUF ))
            crypto_threads_update_counter(CRYPTO_CNTR_CMD_READ_ENCRYPT)
            alu[r_tmp, CRYPTO_CNTR_PKTS_ENCRYPT_CORE_0, +, _crypto_ctx]
            crypto_threads_update_counter(r_tmp)
        .endif
        #else
        crypto_threads_update_counter(CRYPTO_CNTR_CMD_READ_ENCRYPT)
        alu[r_tmp, CRYPTO_CNTR_PKTS_ENCRYPT_CORE_0, +, _crypto_ctx]
        crypto_threads_update_counter(r_tmp)
        #endif

        .if ( r_cipher == CRYPTO_CIPHER_NONE )
            // CIPHER_NONE hangs if use route code cipher then hash in serial.
            // Instead, have to use cipher and hash, parallel, which does not
            // hang. As it turns out, this is the same as the decrypt sequence.
            // So to work around this problem, do decrypt instead of encrypt.
            // Also note that while r_dir ( encrypt vs. decrypt ) figures into
            // the r_cfg_modes param value, it does not have any effect for
            // CIPHER_NONE.
            #if ( ENABLE_JUMBO_PKTS == 1 )
            .if ( r_tmp1 == CRYPTO_LM_JUMBO_STATE_NON_JUMBO )
            #endif
                .if ( r_buf == 0 )
                    crypto_threads_decrypt(A, _crypto_ctx, $runcmd, CRYPTO_PARAMS)
                .elif ( r_buf == 1 )
                    crypto_threads_decrypt(B, _crypto_ctx, $runcmd, CRYPTO_PARAMS)
                .elif ( r_buf == 2 )
                    crypto_threads_decrypt(C, _crypto_ctx, $runcmd, CRYPTO_PARAMS)
                .else // ( r_buf == 3 )
                    crypto_threads_decrypt(D, _crypto_ctx, $runcmd, CRYPTO_PARAMS)
                .endif
            #if ( ENABLE_JUMBO_PKTS == 1 )
            .elif ( r_tmp1 == CRYPTO_LM_JUMBO_STATE_FIRST_BUF )
                .if ( r_buf == 0 )
                    crypto_threads_decrypt_strt_nw(A, _crypto_ctx, $runcmd, \
                                                                  CRYPTO_PARAMS)
                .elif ( r_buf == 1 )
                    crypto_threads_decrypt_strt_nw(B, _crypto_ctx, $runcmd, \
                                                                  CRYPTO_PARAMS)
                .elif ( r_buf == 2 )
                    crypto_threads_decrypt_strt_nw(C, _crypto_ctx, $runcmd, \
                                                                  CRYPTO_PARAMS)
                .else // ( r_buf == 3 )
                    crypto_threads_decrypt_strt_nw(D, _crypto_ctx, $runcmd, \
                                                                  CRYPTO_PARAMS)
                .endif
            .elif ( r_tmp1 == CRYPTO_LM_JUMBO_STATE_CONTINUE_BUF )
                crypto_threads_decrypt_cont_nw(_crypto_ctx, $runcmd, \
                                      r_sram_ptext, r_ptext_size, r_sram_vars )
            .elif ( r_tmp1 == CRYPTO_LM_JUMBO_STATE_END_BUF )
                #define r_hres r_tmp
                #define r_hkey r_tmp1
                localmem_read1(r_hkey, r_tmp, CRYPTO_LM_COMM_JUMBO_HKEY_ADDR)
                crypto_threads_get_hash_addr(r_hres, _crypto_ctx, r_buf)
                crypto_threads_decrypt_end(_crypto_ctx, $runcmd, \
                                      r_sram_ptext, r_ptext_size, r_hres, r_hkey, \
                                      r_cfg_hlen, r_sram_vars )
                #undef r_hres
                #undef r_hkey
            .endif
            #endif
        .else
            #if ( ENABLE_JUMBO_PKTS == 1 )
            .if ( r_tmp1 == CRYPTO_LM_JUMBO_STATE_NON_JUMBO )
            #endif
                .if ( r_buf == 0 )
                    crypto_threads_encrypt(A, _crypto_ctx, $runcmd, CRYPTO_PARAMS)
                .elif ( r_buf == 1 )
                    crypto_threads_encrypt(B, _crypto_ctx, $runcmd, CRYPTO_PARAMS)
                .elif ( r_buf == 2 )
                    crypto_threads_encrypt(C, _crypto_ctx, $runcmd, CRYPTO_PARAMS)
                .else // ( r_buf == 3 )
                    crypto_threads_encrypt(D, _crypto_ctx, $runcmd, CRYPTO_PARAMS)
                .endif
            #if ( ENABLE_JUMBO_PKTS == 1 )
            .elif ( r_tmp1 == CRYPTO_LM_JUMBO_STATE_FIRST_BUF )
                .if ( r_buf == 0 )
                    crypto_threads_encrypt_strt(A, _crypto_ctx, $runcmd, \
                                                                    CRYPTO_PARAMS)
                .elif ( r_buf == 1 )
                    crypto_threads_encrypt_strt(B, _crypto_ctx, $runcmd, \
                                                                    CRYPTO_PARAMS)
                .elif ( r_buf == 2 )
                    crypto_threads_encrypt_strt(C, _crypto_ctx, $runcmd, \
                                                                    CRYPTO_PARAMS)
                .else // ( r_buf == 3 )
                    crypto_threads_encrypt_strt(D, _crypto_ctx, $runcmd, \
                                                                    CRYPTO_PARAMS)
                .endif
            .elif ( r_tmp1 == CRYPTO_LM_JUMBO_STATE_CONTINUE_BUF )
                crypto_threads_encrypt_cont(_crypto_ctx, $runcmd, \
                                      r_sram_ptext, r_ptext_size, r_sram_vars )
            .elif ( r_tmp1 == CRYPTO_LM_JUMBO_STATE_END_BUF )
                #define r_hres r_tmp
                #define r_hkey r_tmp1
                localmem_read1(r_hkey, r_tmp, CRYPTO_LM_COMM_JUMBO_HKEY_ADDR)
                crypto_threads_get_hash_addr(r_hres, _crypto_ctx, r_buf)
                crypto_threads_encrypt_end(_crypto_ctx, $runcmd, \
                                      r_sram_ptext, r_ptext_size, r_hres, r_hkey, \
                                      r_cfg_hlen, r_sram_vars )
                #undef r_hres
                #undef r_hkey
            .endif
            #endif
        .endif

    .else // decrypt

        #if ( ENABLE_JUMBO_PKTS == 1 )
        .if ( (r_tmp1 == CRYPTO_LM_JUMBO_STATE_NON_JUMBO) \
              || (r_tmp1 == CRYPTO_LM_JUMBO_STATE_FIRST_BUF ))
            crypto_threads_update_counter(CRYPTO_CNTR_CMD_READ_DECRYPT)
            alu[r_tmp, CRYPTO_CNTR_PKTS_DECRYPT_CORE_0, +, _crypto_ctx]
            crypto_threads_update_counter(r_tmp)
        .endif
        #else
        crypto_threads_update_counter(CRYPTO_CNTR_CMD_READ_DECRYPT)
        alu[r_tmp, CRYPTO_CNTR_PKTS_DECRYPT_CORE_0, +, _crypto_ctx]
        crypto_threads_update_counter(r_tmp)
        #endif

        #if ( ENABLE_JUMBO_PKTS == 1 )
        .if ( r_tmp1 == CRYPTO_LM_JUMBO_STATE_NON_JUMBO )
        #endif
            .if ( r_buf == 0 )
                crypto_threads_decrypt(A, _crypto_ctx, $runcmd, CRYPTO_PARAMS)
            .elif ( r_buf == 1 )
                crypto_threads_decrypt(B, _crypto_ctx, $runcmd, CRYPTO_PARAMS)
            .elif ( r_buf == 2 )
                crypto_threads_decrypt(C, _crypto_ctx, $runcmd, CRYPTO_PARAMS)
            .elif ( r_buf == 3 )
                crypto_threads_decrypt(D, _crypto_ctx, $runcmd, CRYPTO_PARAMS)
            .endif
        #if ( ENABLE_JUMBO_PKTS == 1 )
        .elif ( r_tmp1 == CRYPTO_LM_JUMBO_STATE_FIRST_BUF )
            // see above re crypto engine hangs on ciper none if have a wait
            // coded; these sequences do not have a wait
            .if ( r_cipher == CRYPTO_CIPHER_NONE )
                .if ( r_buf == 0 )
                    crypto_threads_decrypt_strt_nw(A, _crypto_ctx, $runcmd, CRYPTO_PARAMS)
                .elif ( r_buf == 1 )
                    crypto_threads_decrypt_strt_nw(B, _crypto_ctx, $runcmd, CRYPTO_PARAMS)
                .elif ( r_buf == 2 )
                    crypto_threads_decrypt_strt_nw(C, _crypto_ctx, $runcmd, CRYPTO_PARAMS)
                .elif ( r_buf == 3 )
                    crypto_threads_decrypt_strt_nw(D, _crypto_ctx, $runcmd, CRYPTO_PARAMS)
                .endif
            .else
                .if ( r_buf == 0 )
                    crypto_threads_decrypt_strt(A, _crypto_ctx, $runcmd, CRYPTO_PARAMS)
                .elif ( r_buf == 1 )
                    crypto_threads_decrypt_strt(B, _crypto_ctx, $runcmd, CRYPTO_PARAMS)
                .elif ( r_buf == 2 )
                    crypto_threads_decrypt_strt(C, _crypto_ctx, $runcmd, CRYPTO_PARAMS)
                .elif ( r_buf == 3 )
                    crypto_threads_decrypt_strt(D, _crypto_ctx, $runcmd, CRYPTO_PARAMS)
                .endif
            .endif
        .elif ( r_tmp1 == CRYPTO_LM_JUMBO_STATE_CONTINUE_BUF )
            // see above re crypto engine hangs on ciper none if have a wait
            // coded; thos sequence does not have a wait
            .if ( r_cipher == CRYPTO_CIPHER_NONE )
                crypto_threads_decrypt_cont_nw(_crypto_ctx, $runcmd, \
                                 r_sram_ptext, r_ptext_size, r_sram_vars )
            .else
                crypto_threads_decrypt_cont(_crypto_ctx, $runcmd, \
                                 r_sram_ptext, r_ptext_size, r_sram_vars )
            .endif
        .elif ( r_tmp1 == CRYPTO_LM_JUMBO_STATE_END_BUF )
            #define r_hres r_tmp
            #define r_hkey r_tmp1
            localmem_read1(r_hkey, r_tmp, CRYPTO_LM_COMM_JUMBO_HKEY_ADDR)
            crypto_threads_get_hash_addr(r_hres, _crypto_ctx, r_buf)
            crypto_threads_decrypt_end(_crypto_ctx, $runcmd, \
                                  r_sram_ptext, r_ptext_size, r_hres, r_hkey, \
                                  r_cfg_hlen, r_sram_vars )
            #undef r_hres
            #undef r_hkey
        .endif
        #endif
    .endif

    #if ( ENABLE_JUMBO_PKTS == 1 )
    #pragma warning(default:4701) // see above
    #endif

    #pragma warning(default:5008) // see above

    .set $runcmd[0], $runcmd[1]


    //
    // generate cmds to dma the en/decrypted data back out, send a done signal,
    // and then send the cmds to the crypto core
    //

    #if ( ENABLE_JUMBO_PKTS == 1 )
    // for jumbo 'continue' or 'end' buffers, skip past the
    // dma for the 'writeback' area, which was already done with
    // the 1st jumbo buffer
    crypto_threads_get_lm_temp_addr(r_tmp, _me_ctx, 0)
    localmem_read1(r_tmp1, r_tmp, CRYPTO_LM_COMM_JUMBO_STATE)
    .if ( r_tmp1 == CRYPTO_LM_JUMBO_STATE_CONTINUE_BUF \
           || r_tmp1 == CRYPTO_LM_JUMBO_STATE_END_BUF )
        br[crypto_input_thread_dma_wrback_end#]
    .endif
    #endif

    #if ( ENABLE_JUMBO_PKTS == 1 )
    #pragma warning(disable:4701) // r_sa_ctlwrd, etc. are set from read in.
                                  // problem flagged due to jump to
                                  // crypto_input_thread_dma_in# for jumbos
    #endif


    alu[r_tmp, CRYPTO_SA_PROTOCOL_MASK, AND, r_sa_ctlwrd, \
                                  >>CRYPTO_SA_PROTOCOL_SHIFT]

    // ESP, either transport or tunnel
    .if ( r_tmp == CRYPTO_PROTOCOL_ESP )

      // encrypt, or decrypt option 0. writes back entire packet
      .if (!(CRYPTO_REQ_DECRYPT_OPTION & (1 << CRYPTO_REQ_DECRYPT_OPTION_BIT)))

        // figure starting address in sram
        crypto_threads_get_pkt_addr(r_sram_pkt, _crypto_ctx, r_buf)

        // if pkt start address not 8 byte aligned, dma has to start at next
        // 8 byte aligned address
        alu[r_tmp, CRYPTO_REQ_PKT_STRT_ADDR, AND, 7]
        .if ( r_tmp != 0 )
            alu[r_sram_pkt, r_sram_pkt, +, 8]
        .endif

        // make sure write back address is aligned same as start packet address
        alu[r_pkt_wrbk_addr, --, B, CRYPTO_REQ_PKT_WRBK_ADDR]
        alu[r_tmp1, r_pkt_wrbk_addr, AND, 7]
        #if ( ENABLE_ERROR_CHECKS == 1 )
        .if ( r_tmp1 != r_tmp )
            crypto_threads_input_send_response( _me_ctx, r_buf, \
                     CRYPTO_RSP_STATUS_WRITE_BACK_ALIGN_ERR )
            br[crypto_input_thread_loop#]
        .endif
        #endif

        // if pkt write back address not 8 byte aligned, dma has to start at
        // next 8 byte aligned address
        .if ( r_tmp1 != 0 )
            alu[r_pkt_wrbk_addr, r_pkt_wrbk_addr, +, 8]
            alu[r_pkt_wrbk_addr, r_pkt_wrbk_addr, -, r_tmp1]
        .endif

        // figure write back length,  make sure the alignment matches the address
        ld_field_w_clr[r_pkt_wrbk_len, 0011, CRYPTO_REQ_PKT_WRBK_LENGTH, \
                                       >>CRYPTO_REQ_PKT_WRBK_LENGTH_SHIFT]

        // if end of packet buffer, write back must end on 8 byte alignment
        .if ( r_pkt_wrbk_end_len > 0 )

            #if ( ENABLE_ERROR_CHECKS == 1 )
            alu[r_tmp1, r_pkt_wrbk_len, AND, 7]
            alu[r_tmp1, 8, -, r_tmp1]
            alu[r_tmp1, r_tmp1, AND, 7] // need in case of already 8 byte aligned
            .if ( r_tmp != r_tmp1 )
                crypto_threads_input_send_response( _me_ctx, r_buf, \
                                       CRYPTO_RSP_STATUS_WRITE_BACK_LENGTH_ERR )
                br[crypto_input_thread_loop#]
            .endif
            #endif

            // handle encrypt case where end of packet buffer only contains
            // authentication data. adjust r_pkt_wrbk_end_len so that no dma
            // will be started for the end of packet area. ( output thread writes
            // auth data using non-dma out )
            .if ( r_dir ==  CRYPTO_DIR_ENCRYPT )

                 alu[r_tmp1, CRYPTO_SA_HASH_MASK, AND, \
                     r_sa_ctlwrd, >>CRYPTO_SA_HASH_SHIFT]
                .if ( r_tmp1 != CRYPTO_HASH_NONE )
                     alu[r_tmp1, r_cfg_hlen, +, 1]
                    .if ( r_pkt_wrbk_end_len <= r_tmp1 )
                        move(r_pkt_wrbk_end_len, 0)
                    .endif
                .endif
            .endif

        .else // r_pkt_wrbk_end_len <= 0

            // if pkt write back address not 8 byte aligned, dma has to start at
            // next 8 byte aligned address, so reduce dma length
            .if ( r_tmp1 != 0 )
                alu[r_pkt_wrbk_len, r_pkt_wrbk_len, -, 8]
                alu[r_pkt_wrbk_len, r_pkt_wrbk_len, +, r_tmp1]
            .endif

            // handle case where transfer ends in writeback area
            // and does not end on 8 byte alignment
            alu[r_tmp1, r_pkt_wrbk_len, +, r_pkt_wrbk_addr]
            alu[r_tmp1, r_tmp1, AND, 7]
            .if ( r_tmp1 != 0 )
                alu[r_pkt_wrbk_len, r_pkt_wrbk_len, +, 8]
            .endif

        .endif


        // if pkt write back address not 8 byte aligned, dma has to skip
        // 1st non-8 byte aligned bytes
        .if ( r_tmp != 0 )
            alu[r_pkt_wrbk_len, r_pkt_wrbk_len, AND~, 7]
        .endif


      .else

        // decrypt option 1. writes back only the decrypted plain text data

        // figure starting address in sram
        crypto_threads_get_pkt_addr(r_sram_pkt, _crypto_ctx, r_buf)

        // advance to 1st byte of packet in sram ( for non 8-byte alignment )
        alu[r_tmp, CRYPTO_REQ_PKT_STRT_ADDR, AND, 7]
        alu[r_sram_pkt, r_sram_pkt, +, r_tmp]

        // advance to start of text offset, skipping over fields prior to
        // decrypted text
        ld_field_w_clr[r_tmp, 0011, CRYPTO_REQ_PTEXT_OFFSET, \
                                >>CRYPTO_REQ_PTEXT_OFFSET_SHIFT]
        alu[r_sram_pkt, r_sram_pkt, +, r_tmp]


        alu[r_pkt_wrbk_addr, --, B, CRYPTO_REQ_PKT_WRBK_ADDR]
        // for transport mode, mem transfer begins at auth hdr offset
        alu[r_tmp, CRYPTO_SA_PROTMODE_MASK, AND, r_sa_ctlwrd, \
                                  >>CRYPTO_SA_PROTMODE_SHIFT]
        .if ( r_tmp == CRYPTO_PROTMODE_TRANSPORT)
            ld_field_w_clr[r_tmp, 0011, CRYPTO_REQ_AUTH_HDR_OFFSET, \
                                     >>CRYPTO_REQ_AUTH_HDR_OFFSET_SHIFT]
            alu[r_pkt_wrbk_addr, r_pkt_wrbk_addr, +, r_tmp]
        .endif


        // make sure write back address is aligned the same as plain text address
        alu[r_tmp, r_sram_pkt, AND, 7]
        alu[r_tmp1, r_pkt_wrbk_addr, AND, 7]
        #if ( ENABLE_ERROR_CHECKS == 1 )
        .if ( r_tmp1 != r_tmp )
            crypto_threads_input_send_response( _me_ctx, r_buf, \
                       CRYPTO_RSP_STATUS_WRITE_BACK_ALIGN_ERR )
            br[crypto_input_thread_loop#]
        .endif
        #endif

        // if pkt write back address not 8 byte aligned, dma has to start at
        // next 8 byte aligned address
        .if ( r_tmp1 != 0 )
            alu[r_pkt_wrbk_addr, r_pkt_wrbk_addr, +, 8]
            alu[r_pkt_wrbk_addr, r_pkt_wrbk_addr, -, r_tmp1]

            // and same thing for the sram address, which we already know is
            // aligned the same
            alu[r_sram_pkt, r_sram_pkt, +, 8]
            alu[r_sram_pkt, r_sram_pkt, -, r_tmp1]
        .endif

        // figure the write back length and make sure the alignment matches the
        // address
        ld_field_w_clr[r_pkt_wrbk_len, 0011, CRYPTO_REQ_PKT_WRBK_LENGTH, \
                                          >>CRYPTO_REQ_PKT_WRBK_LENGTH_SHIFT]


        // for transport mode, mem transfer begins at auth hdr offset
        alu[r_tmp1, CRYPTO_SA_PROTMODE_MASK, AND, r_sa_ctlwrd, \
                                  >>CRYPTO_SA_PROTMODE_SHIFT]
        .if ( r_tmp1 == CRYPTO_PROTMODE_TRANSPORT)
            ld_field_w_clr[r_tmp1, 0011, CRYPTO_REQ_AUTH_HDR_OFFSET, \
                                     >>CRYPTO_REQ_AUTH_HDR_OFFSET_SHIFT]
            alu[r_pkt_wrbk_len, r_pkt_wrbk_len, -, r_tmp1]
        .endif


        // if end of packet buffer, write back must end on 8 byte alignment
        #if ( ENABLE_ERROR_CHECKS == 1 )
        .if ( r_pkt_wrbk_end_len > 0 )
            alu[r_tmp1, r_pkt_wrbk_len, AND, 7]
            alu[r_tmp1, 8, -, r_tmp1]
            alu[r_tmp1, r_tmp1, AND, 7] // need in case of already 8 byte aligned
            .if ( r_tmp != r_tmp1 )
                crypto_threads_input_send_response( _me_ctx, r_buf, \
                        CRYPTO_RSP_STATUS_WRITE_BACK_LENGTH_ERR )
                br[crypto_input_thread_loop#]
            .endif
        .endif
        #endif


        // handle case where transfer ends in writeback area
        // and does not end on 8 byte alignment
        .if ( r_pkt_wrbk_end_len == 0 )
            alu[r_tmp1, r_pkt_wrbk_len, +, r_pkt_wrbk_addr]
            alu[r_tmp1, r_tmp1, AND, 7]
            .if ( r_tmp1 != 0 )
                alu[r_pkt_wrbk_len, r_pkt_wrbk_len, +, 8]
            .endif
        .endif


        // if pkt write back address not 8 byte aligned, dma has to skip
        // 1st non-8 byte aligned bytes
        .if ( r_tmp != 0 )
            alu[r_pkt_wrbk_len, r_pkt_wrbk_len, AND~, 7]
        .endif

      .endif


     // AH, either transport or tunnel
    .elif ( r_tmp == CRYPTO_PROTOCOL_AH )

        // 'encrypt' ( a.k.a. 'Outbound' )
        .if ( r_dir ==  CRYPTO_DIR_ENCRYPT )

            // figure starting address in sram
            crypto_threads_get_pkt_addr(r_sram_pkt, _crypto_ctx, r_buf)

            // advance to 1st byte of packet in sram ( for non 8-byte alignment )
            alu[r_tmp, CRYPTO_REQ_PKT_STRT_ADDR, AND, 7]
            alu[r_sram_pkt, r_sram_pkt, +, r_tmp]

            // sram transfer will begin with the auth header
            ld_field_w_clr[r_tmp, 0011, CRYPTO_REQ_AUTH_HDR_OFFSET, \
                                     >>CRYPTO_REQ_AUTH_HDR_OFFSET_SHIFT]
            alu[r_sram_pkt, r_sram_pkt, +, r_tmp]


            // memory transfer will begin with the auth header
            alu[r_pkt_wrbk_addr, --, B, CRYPTO_REQ_PKT_WRBK_ADDR]
            alu[r_pkt_wrbk_addr, r_pkt_wrbk_addr, +, r_tmp]


            // make sure write back address is aligned same as the auth hdr.
            alu[r_tmp, r_pkt_wrbk_addr, AND, 7]
            #if ( ENABLE_ERROR_CHECKS == 1 )
            alu[r_tmp1, r_sram_pkt, AND, 7]
            .if ( r_tmp1 != r_tmp )
                crypto_threads_input_send_response( _me_ctx, r_buf, \
                          CRYPTO_RSP_STATUS_WRITE_BACK_ALIGN_ERR )
                br[crypto_input_thread_loop#]
            .endif
            #endif


            // if pkt write back address not 8 byte aligned, dma has to start at
            // next 8 byte aligned address
            .if ( r_tmp != 0 )
                alu[r_pkt_wrbk_addr, r_pkt_wrbk_addr, +, 8]
                alu[r_pkt_wrbk_addr, r_pkt_wrbk_addr, -, r_tmp]

                // same for sram address
                alu[r_sram_pkt, r_sram_pkt, +, 8]
                alu[r_sram_pkt, r_sram_pkt, -, r_tmp]
            .endif


            // figure write back length
            ld_field_w_clr[r_pkt_wrbk_len, 0011, CRYPTO_REQ_PKT_WRBK_LENGTH, \
                                           >>CRYPTO_REQ_PKT_WRBK_LENGTH_SHIFT]

            // transfer will begin with the auth header
            ld_field_w_clr[r_tmp1, 0011, CRYPTO_REQ_AUTH_HDR_OFFSET, \
                                     >>CRYPTO_REQ_AUTH_HDR_OFFSET_SHIFT]
            alu[r_pkt_wrbk_len, r_pkt_wrbk_len, -, r_tmp1]


            // if end of packet buffer, write back must end on 8 byte alignment
            #if ( ENABLE_ERROR_CHECKS == 1 )
            .if ( r_pkt_wrbk_end_len > 0 )
                alu[r_tmp1, r_pkt_wrbk_len, AND, 7]
                alu[r_tmp1, 8, -, r_tmp1]
                alu[r_tmp1, r_tmp1, AND, 7] // need if already 8 byte aligned
                .if ( r_tmp1 != r_tmp )
                    crypto_threads_input_send_response( _me_ctx, r_buf, \
                            CRYPTO_RSP_STATUS_WRITE_BACK_LENGTH_ERR )
                    br[crypto_input_thread_loop#]
                .endif
            .endif
            #endif


            // if pkt write back address not 8 byte aligned, dma has to start at
            // next 8 byte aligned address, so reduce dma length
            .if ( r_tmp != 0 )
                alu[r_pkt_wrbk_len, r_pkt_wrbk_len, -, 8]
                alu[r_pkt_wrbk_len, r_pkt_wrbk_len, +, r_tmp]
            .endif


            // handle case where transfer ends in writeback area
            // and does not end on 8 byte alignment
            .if ( r_pkt_wrbk_end_len == 0 )
                alu[r_tmp1, r_pkt_wrbk_len, +, r_pkt_wrbk_addr]
                alu[r_tmp1, r_tmp1, AND, 7]
                .if ( r_tmp1 != 0 )
                    alu[r_pkt_wrbk_len, r_pkt_wrbk_len, +, 8]
                .endif
            .endif


            // if pkt write back address not 8 byte aligned, dma has to skip
            // 1st non-8 byte aligned bytes
            .if ( r_tmp != 0 )
                alu[r_pkt_wrbk_len, r_pkt_wrbk_len, AND~, 7]
            .endif

        // 'decrypt' ( a.k.a. 'Inbound' )
        .else

            // decrypt option 0. does not write anything back
            .if (!(CRYPTO_REQ_DECRYPT_OPTION & \
                    (1 << CRYPTO_REQ_DECRYPT_OPTION_BIT)))

                // use the dma out xfr regs for the done sig cmd
                #define r_island r_tmp1
                #define r_ppid7 r_tmp2
                crypto_get_island_and_PPID7 (r_island, r_ppid7)
                alu[r_tmp, _me_ctx, +, 1] // send sig to correspondng out thread
                .if ( r_buf == 0 )
                    crypto_send_done_signal ($dma_pkt_wrbk_out, _crypto_ctx, \
                          r_tmp, CHAINED, r_island, r_ppid7, buf_sig_A, *nosend*)
                .elif ( r_buf == 1 )
                    crypto_send_done_signal ($dma_pkt_wrbk_out, _crypto_ctx, \
                          r_tmp, CHAINED, r_island, r_ppid7, buf_sig_B, *nosend*)
                .elif ( r_buf == 2 )
                    crypto_send_done_signal ($dma_pkt_wrbk_out, _crypto_ctx, \
                          r_tmp, CHAINED, r_island, r_ppid7, buf_sig_C, *nosend*)
                .elif ( r_buf == 3 )
                    crypto_send_done_signal ($dma_pkt_wrbk_out, _crypto_ctx, \
                          r_tmp, CHAINED, r_island, r_ppid7, buf_sig_D, *nosend*)
                .endif
                #undef r_island
                #undef r_ppid7
                #define_eval _xfer_len 2
                #pragma warning(disable:5151) // not sure why nfas warns here..
                crypto_send (write_fifo, $runcmd, --, 0, _xfer_len, --)
                #pragma warning(default:5151)
                crypto_threads_update_counter(CRYPTO_CNTR_CMD_SEND_1)

                br[crypto_input_thread_next_buf#]

            // 'decrypt' option 1. writes back packet data, minus
            // authentication header & data
            .else

                // figure starting address in sram
                crypto_threads_get_pkt_addr(r_sram_pkt, _crypto_ctx, r_buf)

                // advance to 1st byte of packet in sram (for non 8-byte alignment)
                alu[r_tmp, CRYPTO_REQ_PKT_STRT_ADDR, AND, 7]
                alu[r_sram_pkt, r_sram_pkt, +, r_tmp]

                // advance to start of inner ip header ( tunnel mode ), or to
                // start of ip payload ( transport mode ) which follows auth data
                ld_field_w_clr[r_tmp, 0011, CRYPTO_REQ_AUTH_DATA_OFFSET, \
                                    >>CRYPTO_REQ_AUTH_DATA_OFFSET_SHIFT]
                alu[r_sram_pkt, r_sram_pkt, +, r_tmp]

                // N.B. have to check which hash is selcted by sa; if it
                // selected hash none, have to skip over adding in the
                // hash length, because we have really set the h/w to do SHA1
                // since it hangs on hash none. (r_cfg_hlen would be non-zero)
                // The output thread will ignore the hash for hash none.
                alu[r_tmp, CRYPTO_SA_HASH_MASK, AND, r_sa_ctlwrd,\
                                         >>CRYPTO_SA_HASH_SHIFT]
                .if ( r_tmp != CRYPTO_HASH_NONE )
                    alu[r_sram_pkt, r_sram_pkt, +, r_cfg_hlen]
                    alu[r_sram_pkt, r_sram_pkt, +, 1]
                .endif

                // figure starting address for dma in memory
                alu[r_pkt_wrbk_addr, --, B, CRYPTO_REQ_PKT_WRBK_ADDR]

                alu[r_tmp, CRYPTO_SA_PROTMODE_MASK, AND, r_sa_ctlwrd, \
                                  >>CRYPTO_SA_PROTMODE_SHIFT]
                // for tunnel mode, start at 'plaintext' offset
                .if ( r_tmp == CRYPTO_PROTMODE_TUNNEL )
                    ld_field_w_clr[r_tmp, 0011, CRYPTO_REQ_PTEXT_OFFSET, \
                                            >>CRYPTO_REQ_PTEXT_OFFSET_SHIFT]
                // for transport mode, start at 'auth header' offset, which
                // is just after the ip header
                .else
                    ld_field_w_clr[r_tmp, 0011, CRYPTO_REQ_AUTH_HDR_OFFSET, \
                                            >>CRYPTO_REQ_AUTH_HDR_OFFSET_SHIFT]
                .endif
                alu[r_pkt_wrbk_addr, r_pkt_wrbk_addr, +, r_tmp]


                // make sure write back dma address is aligned the same as
                // starting address in sram
                alu[r_tmp1, r_pkt_wrbk_addr, AND, 7]
                #if ( ENABLE_ERROR_CHECKS == 1 )
                alu[r_tmp, r_sram_pkt, AND, 7]
                .if ( r_tmp1 != r_tmp )
                    crypto_threads_input_send_response( _me_ctx, r_buf, \
                               CRYPTO_RSP_STATUS_WRITE_BACK_ALIGN_ERR )
                    br[crypto_input_thread_loop#]
                .endif
                #endif

                // if pkt write back address not 8 byte aligned, dma has to \
                // start at next 8 byte aligned address
                .if ( r_tmp1 != 0 )
                    alu[r_pkt_wrbk_addr, r_pkt_wrbk_addr, +, 8]
                    alu[r_pkt_wrbk_addr, r_pkt_wrbk_addr, -, r_tmp1]

                    // and same thing for the sram address, which we already \
                    // know is aligned the same
                    alu[r_sram_pkt, r_sram_pkt, +, 8]
                    alu[r_sram_pkt, r_sram_pkt, -, r_tmp1]
                .endif

                // figure the write back length and make sure the alignment \
                // matches the address
                ld_field_w_clr[r_pkt_wrbk_len, 0011, CRYPTO_REQ_PKT_WRBK_LENGTH, \
                                              >>CRYPTO_REQ_PKT_WRBK_LENGTH_SHIFT]

                alu[r_tmp, CRYPTO_SA_PROTMODE_MASK, AND, r_sa_ctlwrd, \
                                  >>CRYPTO_SA_PROTMODE_SHIFT]
                // for tunnel mode, the dma skips ahead by the amount of the
                // 'plaintext offset' to leave room to copy out the mac header
                .if ( r_tmp == CRYPTO_PROTMODE_TUNNEL )
                    ld_field_w_clr[r_tmp, 0011, CRYPTO_REQ_PTEXT_OFFSET, \
                                            >>CRYPTO_REQ_PTEXT_OFFSET_SHIFT]
                // for transport mode, the dma skips ahead by the amount of the
                // auth header offset so it starts just after the ip header
                .else
                    ld_field_w_clr[r_tmp, 0011, CRYPTO_REQ_AUTH_HDR_OFFSET, \
                                            >>CRYPTO_REQ_AUTH_HDR_OFFSET_SHIFT]

                .endif
                alu[r_pkt_wrbk_len, r_pkt_wrbk_len, -, r_tmp]

                 // if end of packet buffer, write back must end on 8 byte alignment
                #if ( ENABLE_ERROR_CHECKS == 1 )
                .if ( r_pkt_wrbk_end_len > 0 )
                    alu[r_tmp, r_pkt_wrbk_len, AND, 7]
                    alu[r_tmp, 8, -, r_tmp]
                    alu[r_tmp, r_tmp, AND, 7] // need if already 8 byte aligned
                    .if ( r_tmp != r_tmp1 )
                        crypto_threads_input_send_response( _me_ctx, r_buf, \
                                CRYPTO_RSP_STATUS_WRITE_BACK_LENGTH_ERR )
                        br[crypto_input_thread_loop#]
                    .endif
                .endif
                #endif


                // handle case where transfer ends in writeback area
                // and does not end on 8 byte alignment
                .if ( r_pkt_wrbk_end_len == 0 )
                    alu[r_tmp, r_pkt_wrbk_len, +, r_pkt_wrbk_addr]
                    alu[r_tmp, r_tmp, AND, 7]
                    .if ( r_tmp != 0 )
                        alu[r_pkt_wrbk_len, r_pkt_wrbk_len, +, 8]
                    .endif
                .endif


                // if pkt write back address not 8 byte aligned, dma has to skip
                // 1st non-8 byte aligned bytes
                .if ( r_tmp1 != 0 )
                    alu[r_pkt_wrbk_len, r_pkt_wrbk_len, AND~, 7]
                .endif

            .endif
        .endif

    .else
        // shouldn't get here now due to check_sa. removing to save code/regs
        /*
        crypto_threads_input_send_response(  _me_ctx, r_buf, \
                                      CRYPTO_RSP_STATUS_SA_SELECTION_ERR )
        br[crypto_input_thread_loop#]
        */

    .endif

    #define r_pkt_wrbk_addr_hi r_tmp
    ld_field_w_clr[r_pkt_wrbk_addr_hi, 0001, CRYPTO_REQ_PKT_WRBK_ADDR_UPPER, \
                                         >>CRYPTO_REQ_PKT_WRBK_ADDR_UPPER_SHIFT]

    // finally, generate the dma out for the 1st part of the packet
    .if ( r_pkt_wrbk_end_len > 0 )
        crypto_dma_out ($dma_pkt_wrbk_out, _crypto_ctx, \
                         CHAINED, CHAINED_NOT_LAST, \
                         r_sram_pkt, r_pkt_wrbk_addr, r_pkt_wrbk_addr_hi, \
                         r_pkt_wrbk_len, *nosend*)
    .else
        crypto_dma_out ($dma_pkt_wrbk_out, _crypto_ctx, CHAINED, CHAINED_LAST, \
                         r_sram_pkt, r_pkt_wrbk_addr, r_pkt_wrbk_addr_hi, \
                         r_pkt_wrbk_len, *nosend*)
    .endif
    #undef r_pkt_wrbk_addr_hi


    .if ( r_pkt_wrbk_end_len > 0 )

        // br point for jumbo handling
        crypto_input_thread_dma_wrback_end#:

        // split packet, do two dma outs
        move(r_pkt_end_addr, CRYPTO_REQ_PKT_END_ADDR)
        alu[r_sram_pkt, r_sram_pkt, +, r_pkt_wrbk_len]

        #if ( ENABLE_JUMBO_PKTS == 1 )
            // if processing a jumbo packet, determine writeback end of
            // packet address and length, and starting sram address
            crypto_threads_get_lm_temp_addr(r_tmp, _me_ctx, 0)
            localmem_read1(r_tmp1, r_tmp, CRYPTO_LM_COMM_JUMBO_STATE)

            .if ( r_tmp1 == CRYPTO_LM_JUMBO_STATE_FIRST_BUF)
                // r_sram_pkt and r_pkt_end_addr stay as is

                // figure total packet bytes in this buffer
                // (adjusted for max buffer size already via ptext_size)
                // if decrypt option 1, only decrypted data is xfrd
                .if (CRYPTO_REQ_DECRYPT_OPTION & \
                        (1 << CRYPTO_REQ_DECRYPT_OPTION_BIT))
                    move(r_tmp, r_ptext_size)
                .else // otherwise, transferring all data
                    crypto_threads_get_pkt_addr(r_tmp1, _crypto_ctx, r_buf)
                    alu[r_tmp, CRYPTO_REQ_PKT_STRT_ADDR, AND, 7]
                    alu[r_tmp1, r_tmp1, +, r_tmp]
                    alu[r_tmp, r_sram_ptext, -, r_tmp1]
                    alu[r_tmp, r_tmp, +, r_ptext_size]
                .endif

                // wrbk end length is total, less how many transfered
                // in writeback area
                ld_field_w_clr[r_tmp1, 0011, CRYPTO_REQ_PKT_WRBK_LENGTH, \
                                    >>CRYPTO_REQ_PKT_WRBK_LENGTH_SHIFT]
                alu[r_pkt_wrbk_end_len, r_tmp, -, r_tmp1]


                // save the writeback end offset for next buffer
                crypto_threads_get_lm_temp_addr(r_tmp, _me_ctx, 0)
                localmem_write1(r_pkt_wrbk_end_len, r_tmp, \
                        CRYPTO_LM_COMM_JUMBO_PKT_WRBK_END_OFF)


                // if non 8 byte multiple length, round up, so all
                // bytes transferred. extra bytes transferred will
                // be overwritten by jumbo continue or end transfer
                alu[r_tmp, r_pkt_wrbk_end_len, AND, 7]
                .if ( r_tmp )
                    alu[r_pkt_wrbk_end_len, r_pkt_wrbk_end_len, +, 8]
                    alu[r_pkt_wrbk_end_len, r_pkt_wrbk_end_len, AND~, 7]
                .endif

            .elif ( (r_tmp1 == CRYPTO_LM_JUMBO_STATE_CONTINUE_BUF) \
                    || ( r_tmp1 == CRYPTO_LM_JUMBO_STATE_END_BUF) )

                // transfer starts from beginning of sram
                crypto_threads_get_pkt_addr(r_sram_pkt, _crypto_ctx, r_buf)
                // but if not 8-byte aligned start addr, advance to next
                // 8 byte block; the first non-byte aligned bytes will be
                // transferred after the dma by the output thread
                .if ( r_sram_ptext & 7 )
                    alu[r_sram_pkt, r_sram_pkt, +, 8]
                .endif

                .if (r_tmp1 == CRYPTO_LM_JUMBO_STATE_CONTINUE_BUF)

                    // advance memory address by working writeback offset
                    localmem_read1(r_tmp1, r_tmp, \
                            CRYPTO_LM_COMM_JUMBO_PKT_WRBK_END_OFF)
                    alu[r_pkt_end_addr, r_pkt_end_addr, +, r_tmp1]

                    // if non 8 byte aligned, advance to next 8 byte
                    // block, output thread will write back the 1st
                    // non 8 byte aligned bytes
                    .if ( r_pkt_end_addr & 7 )
                        alu[r_pkt_end_addr, r_pkt_end_addr, +, 8]
                        alu[r_pkt_end_addr, r_pkt_end_addr, AND~, 7]
                    .endif

                    // transfer length is ptext size for this buffer
                    move(r_pkt_wrbk_end_len, r_ptext_size)

                    // save updated working writeback offset
                    alu[r_tmp1, r_tmp1, +, r_pkt_wrbk_end_len]
                    localmem_write1(r_tmp1, r_tmp, \
                            CRYPTO_LM_COMM_JUMBO_PKT_WRBK_END_OFF)

                .else // ( r_tmp1 == CRYPTO_LM_JUMBO_STATE_END_BUF)

                    // advance memory address by working writeback offset
                    localmem_read1(r_tmp1, r_tmp, \
                            CRYPTO_LM_COMM_JUMBO_PKT_WRBK_END_OFF)
                    alu[r_pkt_end_addr, r_pkt_end_addr, +, r_tmp1]

                    // if non 8 byte aligned, advance to next 8 byte
                    // block, output thread will write back the 1st
                    // non 8 byte aligned bytes
                    .if ( r_pkt_end_addr & 7 )
                        alu[r_pkt_end_addr, r_pkt_end_addr, +, 8]
                        alu[r_pkt_end_addr, r_pkt_end_addr, AND~, 7]
                    .endif


                    // length is remainder of end length
                    ld_field_w_clr[r_pkt_wrbk_end_len, 0011, \
                                    CRYPTO_REQ_PKT_WRBK_END_LENGTH, \
                                    >>CRYPTO_REQ_PKT_WRBK_END_LENGTH_SHIFT]
                    alu[r_pkt_wrbk_end_len, r_pkt_wrbk_end_len, -, r_tmp1]


                    // if non 8 byte multiple length, round up
                    alu[r_tmp, r_pkt_wrbk_end_len, AND, 7]
                    .if ( r_tmp )
                        alu[r_pkt_wrbk_end_len, r_pkt_wrbk_end_len, +, 8]
                    .endif


                    // finished with this packet in input thread, reset jumbo state
                    crypto_threads_get_lm_temp_addr(r_tmp, _me_ctx, 0)
                    localmem_write1(CRYPTO_LM_JUMBO_STATE_NON_JUMBO, \
                            r_tmp, CRYPTO_LM_COMM_JUMBO_STATE)

                .endif

                // for jumbo continue or end, we didn't do a writeback
                // transfer. so need to use the writeback and writeback end
                // transfer regs for the writeback end and signal cmds

                crypto_dma_out ($dma_pkt_wrbk_out, _crypto_ctx, \
                                 CHAINED, CHAINED_LAST, \
                                 r_sram_pkt, r_pkt_end_addr, \
                                 r_pkt_end_addr_hi, r_pkt_wrbk_end_len, *nosend*)

                #define r_island r_tmp1
                #define r_ppid7 r_tmp2
                crypto_get_island_and_PPID7 (r_island, r_ppid7)
                alu[r_tmp, _me_ctx, +, 1] // send sig to corresponding output thread
                .if ( r_buf == 0 )
                    crypto_send_done_signal ($dma_pkt_end_out, _crypto_ctx, r_tmp, \
                                    CHAINED, r_island, r_ppid7, buf_sig_A, *nosend*)
                .elif ( r_buf == 1 )
                    crypto_send_done_signal ($dma_pkt_end_out, _crypto_ctx, r_tmp, \
                                    CHAINED, r_island, r_ppid7, buf_sig_B, *nosend*)
                .elif ( r_buf == 2 )
                    crypto_send_done_signal ($dma_pkt_end_out, _crypto_ctx, r_tmp, \
                                    CHAINED, r_island, r_ppid7, buf_sig_C, *nosend*)
                        .elif ( r_buf == 3 )
                    crypto_send_done_signal ($dma_pkt_end_out, _crypto_ctx, r_tmp, \
                                    CHAINED, r_island, r_ppid7, buf_sig_D, *nosend*)
                .endif
                #undef r_island
                #undef r_ppid7

                #define_eval _xfer_len 4
                #pragma warning(disable:5151) // not sure why nfas warns here..
                crypto_send (write_fifo, $runcmd, --, 0, _xfer_len, --)
                #pragma warning(default:5151)
                crypto_threads_update_counter(CRYPTO_CNTR_CMD_SEND_2)

                br[crypto_input_thread_next_buf#]

            .else // not handling a jumbo
                // if non 8 byte multiple length, round up
                alu[r_tmp, r_pkt_wrbk_end_len, AND, 7]
                .if ( r_tmp )
                    alu[r_pkt_wrbk_end_len, r_pkt_wrbk_end_len, +, 8]
                .endif
            .endif
        #else
        // if non 8 byte multiple length, round up
        alu[r_tmp, r_pkt_wrbk_end_len, AND, 7]
        .if ( r_tmp )
            alu[r_pkt_wrbk_end_len, r_pkt_wrbk_end_len, +, 8]
        .endif
        #endif

        crypto_dma_out ($dma_pkt_end_out, _crypto_ctx, CHAINED, CHAINED_LAST, \
                         r_sram_pkt, r_pkt_end_addr, \
                         r_pkt_end_addr_hi, r_pkt_wrbk_end_len, *nosend*)

        #define r_island r_tmp1
        #define r_ppid7 r_tmp2
        crypto_get_island_and_PPID7 (r_island, r_ppid7)
        alu[r_tmp, _me_ctx, +, 1] // send sig to corresponding output thread
        .if ( r_buf == 0 )
            crypto_send_done_signal ($int_sig_msg, _crypto_ctx, r_tmp, CHAINED, \
                                      r_island, r_ppid7, buf_sig_A, *nosend*)
        .elif ( r_buf == 1 )
            crypto_send_done_signal ($int_sig_msg, _crypto_ctx, r_tmp, CHAINED, \
                                      r_island, r_ppid7, buf_sig_B, *nosend*)
        .elif ( r_buf == 2 )
            crypto_send_done_signal ($int_sig_msg, _crypto_ctx, r_tmp, CHAINED, \
                                      r_island, r_ppid7, buf_sig_C, *nosend*)
        .elif ( r_buf == 3 )
            crypto_send_done_signal ($int_sig_msg, _crypto_ctx, r_tmp, CHAINED, \
                                      r_island, r_ppid7, buf_sig_D, *nosend*)
        .endif
        #undef r_island
        #undef r_ppid7

        // send the crypto sequence into the crypto input fifo. per EAS,
        // will block until sufficient space available in input fifo
        #define_eval _xfer_len 6
        #pragma warning(disable:5151) // not sure why nfas warns here..
        crypto_send (write_fifo, $runcmd, --, 0, _xfer_len, --)
        #pragma warning(default:5151)
        crypto_threads_update_counter(CRYPTO_CNTR_CMD_SEND_3)

    .else

        // not a split packet, use the 2nd dma out xfr regs for the done sig cmd
        #define r_island r_tmp1
        #define r_ppid7 r_tmp2
        crypto_get_island_and_PPID7 (r_island, r_ppid7)
        alu[r_tmp, _me_ctx, +, 1] // send sig to corresponding output thread
        .if ( r_buf == 0 )
            crypto_send_done_signal ($dma_pkt_end_out, _crypto_ctx, r_tmp, \
                               CHAINED, r_island, r_ppid7, buf_sig_A, *nosend*)
        .elif ( r_buf == 1 )
            crypto_send_done_signal ($dma_pkt_end_out, _crypto_ctx, r_tmp, \
                               CHAINED, r_island, r_ppid7, buf_sig_B, *nosend*)
        .elif ( r_buf == 2 )
            crypto_send_done_signal ($dma_pkt_end_out, _crypto_ctx, r_tmp, \
                               CHAINED, r_island, r_ppid7, buf_sig_C, *nosend*)
        .elif ( r_buf == 3 )
            crypto_send_done_signal ($dma_pkt_end_out, _crypto_ctx, r_tmp, \
                               CHAINED, r_island, r_ppid7, buf_sig_D, *nosend*)
        .endif
        #undef r_island
        #undef r_ppid7

        #define_eval _xfer_len 4
        #pragma warning(disable:5151) // not sure why nfas warns here..
        crypto_send (write_fifo, $runcmd, --, 0, _xfer_len, --)
        #pragma warning(default:5151)
        crypto_threads_update_counter(CRYPTO_CNTR_CMD_SEND_4)
    .endif

    // end of tell compiler scope of $runcmd, $dma_pkt_wrbk_out, etc.
    .end


    // next buffer
    crypto_input_thread_next_buf#:
    #define_eval _MAX_BUF ( CRYPTO_NUM_BUFS - 1 )
    .if ( r_buf == _MAX_BUF )
        immed[r_buf, 0]
    .else
        alu[r_buf, r_buf, +, 1]
    .endif



    br[crypto_input_thread_loop#]


    // subroutine for send response to reduce code size
    // invoked via helper macro crypto_threads_input_send_response
    #if ( SEND_RESPONSE_SUBROUTINE == 1 )
    .subroutine
    send_response_sub#:
        crypto_threads_send_response( _me_ctx, r_buf, r_stat)
        crypto_threads_reenable_buf(r_buf) // reuse this buffer
        rtn[r_rtn]
    .endsub
    #endif

    .end

#endm


/**
 *  Crypto output thread
 *
 *  @param  _me_ctx       GPR, me context of this thread
 *  @param  _crypto_ctx   GPR, crypto core context to be used by this thread.
 *                        One context is used per core, so context will be equal
 *                        to the core # in use by this thread
 *
 *  Output thread dedicated to a single crypto context / core. Typically run on
 *  odd #'d thread, i.e. me threads 1,3,5,7, while corresponding input thread is
 *  run on even #'d threads, i.e. me threads 0,2,4,6.
 *
 *  See above re crypto_threads_input for buffer utilization description
 */
#macro crypto_threads_output(_me_ctx, _crypto_ctx)

    .begin

    //
    // Thread processing is as follows:
    // 1) wait for signal from crypto core indicating next buffer is processed
    // 2) if decrypt:
    //   a) write non-8 byte aligned start of packet bytes into memory
    //   b) compare calculated ICV / authentication data to received value
    //   c) increment stats
    //   d) write output status into status ring
    //   e) free start of packet buffer if requested in request
    //   f) go to step 1
    // 3) if encrypt:
    //   a) write non-8 byte aligned start of packet bytes into memory
    //   b) transfer ICV / authentication data from sram to memory
    //   c) increment stats
    //   d) write output status into status ring
    //   e) free start of packet buffer if requested in request
    //   f) go to step 1


    .reg r_buf, r_tmp, r_tmp1, r_sram_pkt_addr, r_status
    .reg r_pkt_addr_upper1, r_pkt_addr_upper2, r_sa_addr, r_pkt_strt_addr
    .reg r_pkt_cont_addr, r_pkt_end_addr, r_pkt_wrbk_addr
    .reg r_pkt_lengths1, r_pkt_lengths2, r_pkt_lengths3
    .reg r_pkt_offsets1, r_pkt_offsets2, r_sa_ctlwrd, r_hlen, r_dir
    .reg r_addr_lo, r_addr_hi, r_pkt_wrbk_len, r_pkt_wrbk_end_len
    .reg r_auth_hdr_offset, r_auth_data_offset
    .reg r_sram_hash_addr, r_sram_pkt_auth_addr
    .reg r_protocol, r_hash

     #if ( ENABLE_JUMBO_PKTS == 1 )
    .reg r_jumbo_state
     #endif


    .reg $xfr[2]
    .xfer_order $xfr
    .sig s

    .sig volatile buf_sig_A, buf_sig_B, buf_sig_C, buf_sig_D
    .addr buf_sig_A 8
    .addr buf_sig_B 9
    .addr buf_sig_C 10
    .addr buf_sig_D 11

    .set_sig buf_sig_A
    .set_sig buf_sig_B
    .set_sig buf_sig_C
    .set_sig buf_sig_D


    crypto_threads_update_counter(CRYPTO_CNTR_OUTPUT_THREAD_FLAG)

    // start with buffer A
    immed[r_buf, 0]



    //
    // output processing loop
    //

    crypto_output_thread_loop#:


    // wait for next buffer to be ready. sigs 8,9,10,11, for buf_sig_A,B,C,D
    // are set when crypto core is done with respective buffer
    move(r_tmp, 0x100)
    alu[--, r_buf, OR, 0]
    alu[r_tmp, --, B, r_tmp, <<indirect]
    ctx_arb[--], defer[1]
    local_csr_wr[ACTIVE_CTX_WAKEUP_EVENTS, r_tmp]


    crypto_threads_update_counter(CRYPTO_CNTR_OUTPUT_PKT_SIG)

    move(r_status, CRYPTO_RSP_STATUS_OK) // default status to OK


    // move data in local memory to GPR's
    #pragma warning(disable:5008) // r_sa_addr & r_pkt_cont_addr are not used
    crypto_threads_get_lm_temp_addr(r_tmp, _me_ctx, r_buf)
    localmem_read8(r_pkt_addr_upper1, \
                   r_pkt_addr_upper2, \
                   r_sa_addr, \
                   r_pkt_strt_addr, \
                   r_pkt_cont_addr, \
                   r_pkt_end_addr, \
                   r_pkt_wrbk_addr, \
                   r_pkt_lengths1, \
                   r_tmp, CRYPTO_LM_PKT_WRBK_ADDR_UPPER)
    #pragma warning(default:5008)
    localmem_read6(r_pkt_lengths2, \
                   r_pkt_lengths3, \
                   r_pkt_offsets1, \
                   r_pkt_offsets2, \
                   r_sa_ctlwrd, \
                   r_hlen, \
                   r_tmp, CRYPTO_LM_PKT_WRBK_LENGTH)

    #if ( ENABLE_JUMBO_PKTS == 1 )
    // read and clear this buffer's jumbo state var
    localmem_read1(r_jumbo_state, \
                   r_tmp, CRYPTO_LM_JUMBO_STATE)
    localmem_write1(CRYPTO_LM_JUMBO_STATE_NON_JUMBO, \
                   r_tmp, CRYPTO_LM_JUMBO_STATE)
    #endif


    alu[r_protocol, CRYPTO_SA_PROTOCOL_MASK, AND, r_sa_ctlwrd, \
                                    >>CRYPTO_SA_PROTOCOL_SHIFT]

    alu[r_dir, CRYPTO_SA_DIR_MASK, AND, r_sa_ctlwrd, >>CRYPTO_SA_DIR_SHIFT]

    alu[r_hash, CRYPTO_SA_HASH_MASK, AND, r_sa_ctlwrd, >>CRYPTO_SA_HASH_SHIFT]


    // for jumbo continue or end buffers, transfer start of buffer
    // which was not dma'd due to non-8 byte alignment
    #if ( ENABLE_JUMBO_PKTS == 1 )
    .if ( (r_jumbo_state == CRYPTO_LM_JUMBO_STATE_CONTINUE_BUF) \
         || (r_jumbo_state == CRYPTO_LM_JUMBO_STATE_END_BUF ))

        localmem_read1(r_sram_pkt_addr, \
                       r_tmp, CRYPTO_LM_JUMBO_BUF_SRAM_PTEXT)

        .if ( r_sram_pkt_addr & 7 ) // was not 8 byte aligned

            localmem_read1(r_tmp1, \
                       r_tmp, CRYPTO_LM_JUMBO_BUF_END_ADDR)

            // length is from 1 to 7 bytes depending on starting address offset
            alu[r_tmp, r_sram_pkt_addr, AND, 7]
            alu[r_tmp, 8, -, r_tmp]
            ld_field_w_clr[r_addr_hi, 0001, r_pkt_addr_upper1, \
                              >>CRYPTO_LM_PKT_WRBK_ADDR_UPPER_SHIFT]
            crypto_non_dma_out(r_sram_pkt_addr, r_tmp1, r_addr_hi, r_tmp, \
                                CRYPTO_NDO_7B)

        .endif

        .if ( r_jumbo_state == CRYPTO_LM_JUMBO_STATE_CONTINUE_BUF )
            br[crypto_output_thread_buf_done#] // done with this buffer
        .elif ( r_jumbo_state == CRYPTO_LM_JUMBO_STATE_END_BUF )
            br[crypto_output_thread_hash_processing#] // move or check hash
        .endif
    .endif
    #endif


    // ESP, either transport or tunnel
    .if ( r_protocol == CRYPTO_PROTOCOL_ESP )

        // transfer start of packet which was not dma'd due to non-8 byte
        // alignment

        // for decrypt option 1, transport mode, mem transfer begins at
        // auth hdr offset
        alu[r_tmp, CRYPTO_SA_PROTMODE_MASK, AND, r_sa_ctlwrd, \
                                      >>CRYPTO_SA_PROTMODE_SHIFT]
        .if ((r_pkt_addr_upper1 & (1 << CRYPTO_REQ_DECRYPT_OPTION_BIT)) && \
             ( r_tmp == CRYPTO_PROTMODE_TRANSPORT))
            ld_field_w_clr[r_auth_hdr_offset, 0011, r_pkt_offsets1, \
                                        >>CRYPTO_LM_AUTH_HDR_OFFSET_SHIFT]
            alu[r_pkt_wrbk_addr, r_pkt_wrbk_addr, +, r_auth_hdr_offset]
        .endif

        alu[r_tmp, r_pkt_wrbk_addr, AND, 7]
        .if ( r_tmp != 0 )
            crypto_threads_get_pkt_addr(r_sram_pkt_addr, _crypto_ctx, r_buf)

            // advance to 1st byte of packet in sram ( for non 8-byte alignment )
            alu[r_tmp1, r_pkt_strt_addr, AND, 7]
            alu[r_sram_pkt_addr, r_sram_pkt_addr, +, r_tmp1]

            // if decrypt option 1..
            .if (r_pkt_addr_upper1 & (1 << CRYPTO_REQ_DECRYPT_OPTION_BIT))
                // advance to start of text offset, skipping over fields prior to
                // decrypted text
                ld_field_w_clr[r_tmp1, 0011, r_pkt_offsets2, \
                                 >>CRYPTO_LM_PTEXT_OFFSET_SHIFT]
                alu[r_sram_pkt_addr, r_sram_pkt_addr, +, r_tmp1]
            .endif

            // length is from 1 to 7 bytes depending on starting address offset
            alu[r_tmp, 8, -, r_tmp]
            ld_field_w_clr[r_addr_hi, 0001, r_pkt_addr_upper1, \
                              >>CRYPTO_LM_PKT_WRBK_ADDR_UPPER_SHIFT]
            crypto_non_dma_out(r_sram_pkt_addr, r_pkt_wrbk_addr, r_addr_hi, \
                                r_tmp, CRYPTO_NDO_7B)
        .endif

     // AH, either transport or tunnel

    .elif ( r_protocol == CRYPTO_PROTOCOL_AH )

        .if ( r_dir ==  CRYPTO_DIR_ENCRYPT ) // a.k.a. 'Outbound'

            // transfer start of packet which was not dma'd due to non-8 byte
            // alignment. transfer starts at the auth header

            ld_field_w_clr[r_auth_hdr_offset, 0011, r_pkt_offsets1, \
                                     >>CRYPTO_LM_AUTH_HDR_OFFSET_SHIFT]
            alu[r_pkt_wrbk_addr, r_pkt_wrbk_addr, +, r_auth_hdr_offset]
            alu[r_tmp, r_pkt_wrbk_addr, AND, 7]
            .if ( r_tmp != 0 )
                crypto_threads_get_pkt_addr(r_sram_pkt_addr, _crypto_ctx, r_buf)

                // advance to 1st byte of packet in sram
                // ( for non 8-byte alignment )
                alu[r_tmp1, r_pkt_strt_addr, AND, 7]
                alu[r_sram_pkt_addr, r_sram_pkt_addr, +, r_tmp1]

                // advance to auth header offset
                alu[r_sram_pkt_addr, r_sram_pkt_addr, +, r_auth_hdr_offset]

                // length is from 1 to 7 bytes depending on wrback address offset
                alu[r_tmp, 8, -, r_tmp]
                ld_field_w_clr[r_addr_hi, 0001, r_pkt_addr_upper1, \
                                  >>CRYPTO_LM_PKT_WRBK_ADDR_UPPER_SHIFT]
                crypto_non_dma_out(r_sram_pkt_addr, r_pkt_wrbk_addr, \
                                    r_addr_hi, r_tmp, CRYPTO_NDO_7B)
            .endif

        .elif ( r_dir ==  CRYPTO_DIR_DECRYPT ) // a.k.a. 'Inbound'

            // if 'decrypt' option 1, ...
            .if (r_pkt_addr_upper1 & (1 << CRYPTO_REQ_DECRYPT_OPTION_BIT))


                alu[r_tmp, CRYPTO_SA_PROTMODE_MASK, AND, r_sa_ctlwrd, \
                                    >>CRYPTO_SA_PROTMODE_SHIFT]

                // for inbound AH tunnel mode..
                .if ( r_tmp == CRYPTO_PROTMODE_TUNNEL )

                    // transfer start of packet up to the 'plaintext offset',
                    // which is the MAC hdr, up to the start of the ip tunnel
                    // hdr, which was not transfered by the dma

                    // get sram addr
                    crypto_threads_get_pkt_addr(r_sram_pkt_addr, _crypto_ctx, r_buf)
                    // advance to 1st byte of packet in sram
                    // ( for non 8-byte alignment )
                    alu[r_tmp, r_pkt_strt_addr, AND, 7]
                    alu[r_sram_pkt_addr, r_sram_pkt_addr, +, r_tmp]


                    // save next hdr field, which is 1st byte in auth hdr,
                    // from sram to to local memory so send_response can put
                    // it into the response. saving it now because for AH,
                    // AH Hdr is in 1st buffer in case of jumbo packet
                    ld_field_w_clr[r_tmp, 0011, r_pkt_offsets1, \
                                     >>CRYPTO_LM_AUTH_HDR_OFFSET_SHIFT]
                    alu[r_tmp, r_tmp, +, r_sram_pkt_addr]
                    crypto_threads_save_nxt_hdr( _me_ctx, r_tmp )


                    // length is equal to 'plaintext' offset
                    ld_field_w_clr[r_tmp, 0011, r_pkt_offsets2, \
                                     >>CRYPTO_LM_PTEXT_OFFSET_SHIFT]

                    ld_field_w_clr[r_addr_hi, 0001, r_pkt_addr_upper1, \
                                      >>CRYPTO_LM_PKT_WRBK_ADDR_UPPER_SHIFT]

                    crypto_non_dma_out(r_sram_pkt_addr, r_pkt_wrbk_addr, \
                                        r_addr_hi, r_tmp, CRYPTO_NDO_DEFAULT)

                    // now transfer start of packet which was not dma'd due to
                    // non-8 byte alignment. transfer starts at the inner ip header
                    // ( after the auth data in sram, after the MAC hdr in memory )
                    ld_field_w_clr[r_auth_data_offset, 0011, r_pkt_offsets2, \
                                     >>CRYPTO_LM_AUTH_DATA_OFFSET_SHIFT]
                    alu[r_sram_pkt_addr, r_sram_pkt_addr, +, r_auth_data_offset]
                    // don't do next addition for HASH_NONE; in that case the
                    // hash was set to SHA1, to get around h/w hang, so hlen will
                    // have the value for SHA1
                    .if ( r_hash != CRYPTO_HASH_NONE )
                        alu[r_sram_pkt_addr, r_sram_pkt_addr, +, r_hlen]
                        alu[r_sram_pkt_addr, r_sram_pkt_addr, +, 1]
                    .endif

                    alu[r_tmp1, r_sram_pkt_addr, AND, 7]
                    .if ( r_tmp1 ) // if not aligned on 8 byte boundary

                        // skip to 'plaintext' offset in output buffer, which
                        // is just past the MAC header
                        alu[r_pkt_wrbk_addr, r_pkt_wrbk_addr, +, r_tmp]

                        // length is from 1 to 7 bytes depending on address offset
                        alu[r_tmp1, 8, -, r_tmp1]
                        crypto_non_dma_out(r_sram_pkt_addr, r_pkt_wrbk_addr, \
                                             r_addr_hi, r_tmp1, CRYPTO_NDO_7B)
                    .endif

                // for inbound AH transport mode..
                .else

                    // get sram addr
                    crypto_threads_get_pkt_addr(r_sram_pkt_addr, _crypto_ctx, r_buf)
                    // advance to 1st byte of packet in sram
                    // ( for non 8-byte alignment )
                    alu[r_tmp, r_pkt_strt_addr, AND, 7]
                    alu[r_sram_pkt_addr, r_sram_pkt_addr, +, r_tmp]


                    // save next hdr field, which is 1st byte in auth hdr,
                    // from sram to to local memory so send_response can put
                    // it into the response. saving it now because for AH,
                    // AH Hdr is in 1st buffer in case of jumbo packet
                    ld_field_w_clr[r_tmp, 0011, r_pkt_offsets1, \
                                     >>CRYPTO_LM_AUTH_HDR_OFFSET_SHIFT]
                    alu[r_tmp, r_tmp, +, r_sram_pkt_addr]
                    crypto_threads_save_nxt_hdr( _me_ctx, r_tmp )


                    // transfer start of packet which was not dma'd due to
                    // non-8 byte alignment. transfer starts at the ip payload
                    // ( after the auth data in sram, after the ip hdr in memory )

                    ld_field_w_clr[r_auth_data_offset, 0011, r_pkt_offsets2, \
                                     >>CRYPTO_LM_AUTH_DATA_OFFSET_SHIFT]
                    alu[r_sram_pkt_addr, r_sram_pkt_addr, +, r_auth_data_offset]
                    // don't do next addition for HASH_NONE; in that case the
                    // hash was set to SHA1, to get around h/w hang, so hlen will
                    // have the value for SHA1
                    .if ( r_hash != CRYPTO_HASH_NONE )
                        alu[r_sram_pkt_addr, r_sram_pkt_addr, +, r_hlen]
                        alu[r_sram_pkt_addr, r_sram_pkt_addr, +, 1]
                    .endif

                    alu[r_tmp1, r_sram_pkt_addr, AND, 7]
                    .if ( r_tmp1 ) // if not aligned on 8 byte boundary

                        // skip to start of ip payload in output buffer
                        // is just past the ip header and where the auth
                        // hdr starts in the received packet
                        ld_field_w_clr[r_auth_hdr_offset, 0011, r_pkt_offsets1, \
                                         >>CRYPTO_LM_AUTH_HDR_OFFSET_SHIFT]
                        alu[r_pkt_wrbk_addr, r_pkt_wrbk_addr, +, r_auth_hdr_offset]

                        ld_field_w_clr[r_addr_hi, 0001, r_pkt_addr_upper1, \
                                      >>CRYPTO_LM_PKT_WRBK_ADDR_UPPER_SHIFT]

                        // length is from 1 to 7 bytes depending on address offset
                        alu[r_tmp1, 8, -, r_tmp1]
                        crypto_non_dma_out(r_sram_pkt_addr, r_pkt_wrbk_addr, \
                                            r_addr_hi, r_tmp1, CRYPTO_NDO_7B)
                    .endif
                .endif
            .endif
        .endif

        // restore r_pkt_wrbk_addr from AH mode edits
	crypto_threads_get_lm_temp_addr(r_tmp, _me_ctx, r_buf)
        localmem_read1(r_pkt_wrbk_addr,	r_tmp, \
		       CRYPTO_LM_PKT_WRBK_ADDR)

    .else
        move(r_status, CRYPTO_RSP_STATUS_SA_SELECTION_ERR )
        br[crypto_output_thread_send_status#]
    .endif



    // alter flow for jumbo packets
    #if ( ENABLE_JUMBO_PKTS == 1 )
    .if ( r_jumbo_state == CRYPTO_LM_JUMBO_STATE_FIRST_BUF )
        br[crypto_output_thread_buf_done#] // done with this buffer
    .endif
    #endif


    crypto_output_thread_hash_processing#:

    // encrypt / a.k.a. 'outbound'
    .if ( r_dir == CRYPTO_DIR_ENCRYPT )

        // ESP, either transport or tunnel
        .if ( r_protocol == CRYPTO_PROTOCOL_ESP )
            #if ( ENABLE_DETAILED_COUNTERS == 1 )
            crypto_threads_update_counter(CRYPTO_CNTR_OUTPUT_ENCRYPT_ESP)
            #endif
            move(r_tmp, 1)
        // AH, either transport or tunnel
        .elif ( r_protocol == CRYPTO_PROTOCOL_AH )
            #if ( ENABLE_DETAILED_COUNTERS == 1 )
            crypto_threads_update_counter(CRYPTO_CNTR_OUTPUT_ENCRYPT_AH)
            #endif
            move(r_tmp, 1)
        .else
            move(r_tmp, 0)
        .endif

        .if ( r_tmp == 1 )

            .if ( r_hash != CRYPTO_HASH_NONE ) // unless HASH_NONE was selected...
                // transfer icv/auth data from sram to system memory

                ld_field_w_clr[r_auth_data_offset, 0011, r_pkt_offsets2, \
                                         >>CRYPTO_LM_AUTH_DATA_OFFSET_SHIFT]
                ld_field_w_clr[r_pkt_wrbk_len, 0011, r_pkt_lengths2, \
                                          >>CRYPTO_LM_PKT_WRBK_LENGTH_SHIFT]
                alu[r_hlen, r_hlen, +, 1]
                crypto_threads_get_hash_addr(r_sram_hash_addr, _crypto_ctx, r_buf)

                // auth/icv data located entirely in end of packet
                .if ( r_auth_data_offset >= r_pkt_wrbk_len )
                    ld_field_w_clr[r_addr_hi, 0001, r_pkt_addr_upper2, \
                                       >>CRYPTO_LM_PKT_END_ADDR_UPPER_SHIFT]
                    alu[r_addr_lo, r_pkt_end_addr, +, r_auth_data_offset]
                    alu[r_addr_lo, r_addr_lo, -, r_pkt_wrbk_len]
                    crypto_non_dma_out(r_sram_hash_addr, r_addr_lo, r_addr_hi, \
                                             r_hlen, CRYPTO_NDO_16LW)

                .else
                    // handle part of auth data located in packet write back area
                    ld_field_w_clr[r_addr_hi, 0001, r_pkt_addr_upper1, \
                                      >>CRYPTO_LM_PKT_WRBK_ADDR_UPPER_SHIFT]
                    alu[r_addr_lo, r_pkt_wrbk_addr, +, r_auth_data_offset]
                    alu[r_tmp, r_pkt_wrbk_len, -, r_auth_data_offset]
                    limit_min(r_tmp, r_tmp, r_hlen)
                    .if ( r_tmp & 0x3 ) // not even # lwords, do default non dma out
                        crypto_non_dma_out(r_sram_hash_addr, r_addr_lo, r_addr_hi, \
                                            r_tmp, CRYPTO_NDO_DEFAULT)
                    .else // otherwise can use the lword non dma out
                        crypto_non_dma_out(r_sram_hash_addr, r_addr_lo, r_addr_hi, \
                                            r_tmp, CRYPTO_NDO_16LW)
                    .endif

                    // handle part of auth data located in end of packet area
                    .if ( r_tmp < r_hlen )
                        // start addr of remainder of icv/auth data
                        alu[r_sram_hash_addr, r_sram_hash_addr, +, r_tmp]
                        // length of remainder of icv/auth data
                        alu[r_tmp, r_hlen, -, r_tmp]
                        ld_field_w_clr[r_addr_hi, 0001, r_pkt_addr_upper2, \
                                      >>CRYPTO_LM_PKT_END_ADDR_UPPER_SHIFT]
                        .if (( r_tmp & 0x7 ) || ( r_sram_hash_addr & 0x7 ))
                            // if not on 8-byte boundary, or if not
                            // even # lwords, or if not even #
                            // of 8 bytes, ( which makes sram addr
                            // non 8 byte aligned ), have to use
                            // default non-dma out
                            crypto_non_dma_out(r_sram_hash_addr, r_pkt_end_addr, \
                                               r_addr_hi, r_tmp, CRYPTO_NDO_DEFAULT)
                        .else
                            crypto_non_dma_out(r_sram_hash_addr, r_pkt_end_addr, \
                                               r_addr_hi, r_tmp, CRYPTO_NDO_16LW)
                        .endif
                    .endif
                .endif
            .endif
        .else
            move(r_status, CRYPTO_RSP_STATUS_SA_SELECTION_ERR )
        .endif

    // decrypt
    .else

        // ESP transport or tunnel
        .if ( r_protocol == CRYPTO_PROTOCOL_ESP )

            #if ( ENABLE_DETAILED_COUNTERS == 1 )
            crypto_threads_update_counter(CRYPTO_CNTR_OUTPUT_DECRYPT_ESP)
            #endif

            #if ( ENABLE_JUMBO_PKTS == 1 )
            // sram addr of auth data saved by input thread
            .if ( r_jumbo_state == CRYPTO_LM_JUMBO_STATE_END_BUF )
                crypto_threads_get_lm_temp_addr(r_tmp, _me_ctx, r_buf)
                localmem_read1(r_sram_pkt_auth_addr, \
                           r_tmp, CRYPTO_LM_JUMBO_BUF_SRAM_AUTH_DATA)
            .else
            #endif
            crypto_threads_get_pkt_addr(r_sram_pkt_auth_addr, _crypto_ctx, \
                                                                          r_buf)
            alu[r_tmp, r_pkt_strt_addr, AND, 7] // pkt start offset in sram
            alu[r_sram_pkt_auth_addr, r_sram_pkt_auth_addr, +, r_tmp]
            ld_field_w_clr[r_auth_data_offset, 0011, r_pkt_offsets2, \
                                      >>CRYPTO_LM_AUTH_DATA_OFFSET_SHIFT]
                alu[r_sram_pkt_auth_addr, r_sram_pkt_auth_addr, +, \
                                                      r_auth_data_offset]
            #if ( ENABLE_JUMBO_PKTS == 1 )
            .endif
            #endif

            // save next hdr field, which is 1 byte before the
            // auth data, from sram to local memory
            // so send_response can put it into the response
            alu[r_tmp, r_sram_pkt_auth_addr, -, 1]
            crypto_threads_save_nxt_hdr( _me_ctx, r_tmp )


            .if ( r_hash != CRYPTO_HASH_NONE ) // unless HASH_NONE was selected...
                // compare calculated icv/auth data to received value

                crypto_threads_get_hash_addr(r_sram_hash_addr, _crypto_ctx, \
                                                                        r_buf)

                alu[r_hlen, r_hlen, +, 1]

                crypto_threads_compare(r_tmp, r_sram_pkt_auth_addr, \
                                          r_sram_hash_addr, r_hlen)

                .if ( r_tmp != 0 )
                    move(r_status, CRYPTO_RSP_STATUS_AUTH_CHECK_FAIL )
                .endif
            .endif


            // if decrypt option 1..
            .if (r_pkt_addr_upper1 & (1 << CRYPTO_REQ_DECRYPT_OPTION_BIT))

                // adjust writeback and/or end of packet length to account for
                // removal of padding bytes. ( can't do this until after
                // decryption, so have to do it here )

                // pad length is 2 bytes before authentication data
                // read 8 byte block containing pad length from sram
                alu[r_tmp, r_sram_pkt_auth_addr, -, 2]
                alu[r_tmp1, r_tmp, AND~, 7]
                crypto[read, $xfr[0], 0, r_tmp1, 1], ctx_swap[s]

                // extract the pad length byte
                alu[r_tmp1, r_tmp, AND, 7]
                jump[r_tmp1, epljt#], targets [eplj0#, eplj1#, eplj2#, eplj3#, \
                                               eplj4#, eplj5#, eplj6#, eplj7# ]
                epljt#:
                eplj0#: br[epljt0#]
                eplj1#: br[epljt1#]
                eplj2#: br[epljt2#]
                eplj3#: br[epljt3#]
                eplj4#: br[epljt4#]
                eplj5#: br[epljt5#]
                eplj6#: br[epljt6#]
                eplj7#: br[epljt7#]

                epljt0#:
                    ld_field_w_clr[r_tmp, 0001, $xfr[0], >>24]
                    br[epl_done#]
                epljt1#:
                    ld_field_w_clr[r_tmp, 0001, $xfr[0], >>16]
                    br[epl_done#]
                epljt2#:
                    ld_field_w_clr[r_tmp, 0001, $xfr[0], >>8 ]
                    br[epl_done#]
                epljt3#:
                    ld_field_w_clr[r_tmp, 0001, $xfr[0], >>0 ]
                    br[epl_done#]
                epljt4#:
                    ld_field_w_clr[r_tmp, 0001, $xfr[1], >>24]
                    br[epl_done#]
                epljt5#:
                    ld_field_w_clr[r_tmp, 0001, $xfr[1], >>16]
                    br[epl_done#]
                epljt6#:
                    ld_field_w_clr[r_tmp, 0001, $xfr[1], >>8 ]
                    br[epl_done#]
                epljt7#:
                    ld_field_w_clr[r_tmp, 0001, $xfr[1], >>0 ]

                epl_done#:

                // pad length byte is in r_tmp. Now subtract r_tmp + 2 from
                // end of packet length and/or writeback length. send_response
                // will pick that up from lm and write it into response ring
                alu[r_tmp, r_tmp, +, 2]

                // for tunnel mode, the actual 'writeback' length is
                // sent in the 'continue' length
                alu[r_tmp1, CRYPTO_SA_PROTMODE_MASK, AND, r_sa_ctlwrd, \
                                      >>CRYPTO_SA_PROTMODE_SHIFT]
                .if ( r_tmp1 == CRYPTO_PROTMODE_TUNNEL)
                    ld_field_w_clr[r_pkt_wrbk_len, 0011, r_pkt_lengths1, \
                                          >>CRYPTO_LM_PKT_CONT_LENGTH_SHIFT]
                .else
                    ld_field_w_clr[r_pkt_wrbk_len, 0011, r_pkt_lengths2, \
                                          >>CRYPTO_LM_PKT_WRBK_LENGTH_SHIFT]
                .endif

                ld_field_w_clr[r_pkt_wrbk_end_len, 0011, r_pkt_lengths3, \
                                      >>CRYPTO_LM_PKT_WRBK_END_LENGTH_SHIFT]

                .if ( r_pkt_wrbk_end_len >= r_tmp )

                    // length of pad + 2 is contained within the end of packet
                    // area, so just reduce it from there
                    alu[r_pkt_wrbk_end_len, r_pkt_wrbk_end_len, -, r_tmp]
                    ld_field[r_pkt_lengths3, 1100, r_pkt_wrbk_end_len, \
                                      <<CRYPTO_LM_PKT_WRBK_END_LENGTH_SHIFT]
                    // save it in local mem for crypto_threads_send_response
                    crypto_threads_get_lm_temp_addr(r_tmp, _me_ctx, r_buf)
                    localmem_write1(r_pkt_lengths3, r_tmp, \
                                     CRYPTO_LM_PKT_WRBK_END_LENGTH)

                .elif ( r_pkt_wrbk_end_len )

                    // length of pad + 2 is contained partially  within the end
                    // of packet area, and partially in the writeback area.

                    // subtract out of write back length
                    // what is not included in the end of packet area
                    alu[r_tmp, r_tmp, -, r_pkt_wrbk_end_len]
                    alu[r_pkt_wrbk_len, r_pkt_wrbk_len, -, r_tmp]

                    .if ( r_tmp1 == CRYPTO_PROTMODE_TUNNEL) //tnl mode in cont len
                        ld_field_w_clr[r_pkt_lengths1, 0011, r_pkt_wrbk_len, \
                                          <<CRYPTO_LM_PKT_CONT_LENGTH_SHIFT]
                    .else
                        ld_field_w_clr[r_pkt_lengths2, 0011, r_pkt_wrbk_len, \
                                          <<CRYPTO_LM_PKT_WRBK_LENGTH_SHIFT]
                    .endif

                    // write back end of packet length is now 0
                    move(r_pkt_wrbk_end_len, 0)
                    ld_field[r_pkt_lengths3, 1100, r_pkt_wrbk_end_len, \
                                      <<CRYPTO_LM_PKT_WRBK_END_LENGTH_SHIFT]

                    // save to local mem for crypto_threads_send_response
                    crypto_threads_get_lm_temp_addr(r_tmp, _me_ctx, r_buf)
                    localmem_write3(r_pkt_lengths1, \
                                    r_pkt_lengths2, \
                                    r_pkt_lengths3, \
                                    r_tmp, \
                                    CRYPTO_LM_PKT_CONT_LENGTH)

                .else // length pad + 2 contained entirely in write back area

                    alu[r_pkt_wrbk_len, r_pkt_wrbk_len, -, r_tmp]

                    .if ( r_tmp1 == CRYPTO_PROTMODE_TUNNEL) //tnl mode in cont len
                        ld_field_w_clr[r_pkt_lengths1, 0011, r_pkt_wrbk_len, \
                                          <<CRYPTO_LM_PKT_CONT_LENGTH_SHIFT]
                    .else
                        ld_field_w_clr[r_pkt_lengths2, 0011, r_pkt_wrbk_len, \
                                          <<CRYPTO_LM_PKT_WRBK_LENGTH_SHIFT]
                    .endif


                    // save it in local mem for crypto_threads_send_response
                    crypto_threads_get_lm_temp_addr(r_tmp, _me_ctx, r_buf)
                    localmem_write2(r_pkt_lengths1, \
                                    r_pkt_lengths2, \
                                    r_tmp, \
                                    CRYPTO_LM_PKT_CONT_LENGTH)
                .endif

            .endif


         // AH, either transport or tunnel
        .elif ( r_protocol == CRYPTO_PROTOCOL_AH )

            #if ( ENABLE_DETAILED_COUNTERS == 1 )
            crypto_threads_update_counter(CRYPTO_CNTR_OUTPUT_DECRYPT_AH)
            #endif

            .if ( r_hash != CRYPTO_HASH_NONE ) // unless HASH_NONE was selected...
                // compare calculated icv/auth data to received value

                // calculated value
                crypto_threads_get_hash_addr(r_sram_hash_addr, _crypto_ctx, r_buf)
                // received value was stored in sram vars area by input thread
                crypto_threads_get_vars_addr(r_tmp, _crypto_ctx, r_buf)
                alu[r_tmp, r_tmp, +, 0x40]

                alu[r_hlen, r_hlen, +, 1]

                crypto_threads_compare(r_tmp1, r_tmp, \
                                 r_sram_hash_addr, r_hlen)

                .if ( r_tmp1 != 0 )
                    move(r_status, CRYPTO_RSP_STATUS_AUTH_CHECK_FAIL )
                .endif
            .endif
        .else
            move(r_status, CRYPTO_RSP_STATUS_SA_SELECTION_ERR )
        .endif

    .endif


    // do anti-replay processing
    #if ( ENABLE_ANTI_REPLAY == 1 )
    .if ( r_status == CRYPTO_RSP_STATUS_OK )
        .if ( r_dir == CRYPTO_DIR_DECRYPT )
            crypto_threads_anti_replay(_me_ctx, r_buf, r_status, 1)
        .endif
    .endif
    #endif


    // send status to response ring
    crypto_output_thread_send_status#:
    crypto_threads_send_response( _me_ctx, r_buf, r_status )


    #if ( ENABLE_JUMBO_PKTS == 1 )
    // for jumbo continue & end buffers, invalidate saved sa addr
    // in lm, because we haven't really read in sa info ( the
    // the keys and sa ctlwrd ), and we don't want the input thread
    // sa re-use code to think the info can be re-used
    .if ( (r_jumbo_state == CRYPTO_LM_JUMBO_STATE_CONTINUE_BUF) \
         || (r_jumbo_state == CRYPTO_LM_JUMBO_STATE_END_BUF ))
        crypto_threads_get_lm_temp_addr(r_tmp, _me_ctx, r_buf)
        move(r_tmp1, 0xffffffff)
        localmem_write1(r_tmp1, r_tmp, CRYPTO_LM_SA_ADDR)
    .endif
    #endif


    // update prot/protmode/cipher/hash/etc. specific packet counter
    #if ( ENABLE_DETAILED_COUNTERS == 1 )
    crypto_threads_update_pkt_counter(r_sa_ctlwrd)
    #endif

    // set signal to input thread, done with buffer
    crypto_output_thread_buf_done#:
    alu[r_tmp, _me_ctx, AND, 0x6] // previous ctx
    alu[r_tmp, r_tmp, OR, r_buf, <<3] // signal = buffer + 8
    alu[r_tmp, r_tmp, +, (8<<3)]
    local_csr_wr[SAME_ME_SIGNAL, r_tmp]


    // next buffer
    #define_eval _MAX_BUF ( CRYPTO_NUM_BUFS - 1 )
    .if ( r_buf == _MAX_BUF )
        immed[r_buf, 0]
    .else
        alu[r_buf, r_buf, +, 1]
    .endif


    br[crypto_output_thread_loop#]

    .end

#endm


/**
 *  Initialize crypto threads
 *
 *  @b Example:
 *  @code
 *  crypto_threads_init()
 *  @endcode

 *  Initializes input request ring, initializes crypto cores, and initializes and
 *  starts crypto input and output threads. CRYPTO_NUM_THREADS, CRYPTO_START_CTX
 *  determine how many threads are started and what contexts they are started on.
 *
 *  The Request Ring params are defined as REQ_RING_xyz, above.
 *
 *  Typically used via a wrapper microcode file that may override some of the
 *  parameters defined above, invokes this macro, and does nothing else.
 *
 *  Threads that wish to send requests to the request ring may use macro:
 *  crypto_threads_wait_init_flag()
 *  which loops waiting for crypto_threads_init to complete.
 */
#macro crypto_threads_init()

    .begin

    // Threads in use start with thread 0 through thread CRYPTO_NUM_THREADS - 1
    // To run 6 crypto cores in the NFP6000 crypto island, the 1st me could have
    // the default value CRYPTO_NUM_THREADS 6, and the 2nd me could have the value
    // CRYPTO_NUM_THREADS 6, for a total of 12 threads, 2/core
    //
    // Similarly, CRYPTO_START_CTX could use the default value 0 for the 1st micro
    // engine and the value 3 for the 2nd micro engine, assuming the first me is
    // running 3 crypto cores and the 2nd me is running 3
    //
    // In order to initialize the input request ring before all the input threads
    // start, Thread 0 on the me with CRYPTO_START_CTX == 0 will initialize the
    // crypto input request ring, and the remaining threads are held off until the
    // ring is initialized.
    //
    // If the _ring_size param to crypto_threads_init is zero, the ring is assumed
    // to be already initialized and will not be initialized by crypto_threads_init.
    //
    // Thread 0 on the me with CRYPTO_START_CTX == 0 will also clear the crypto
    // counters if COUNTER_USE_ALLOC_MEM == 0
    //
    // Threads on an me are sequenced such that the init_core is done one core
    // at a time.
    //
    // Thread 7 on the me with CRYPTO_START_CTX == 0 will signal the neighbor me.
    // Thread 0 on the me  with CRYPTO_START_CTX != 0 will wait to be signaled by
    // the previous neighbor me. Thus when using crypto_threads_init for
    // initialization and startup, it is required that the 1st me at a minimum
    // uses crypto core/ctx 0, and that if a 2nd me is used, it must be the next
    // neighbor of the 1st me and must not use crypto core/ctx 0.


    .sig _ss, _st
    .addr _ss 12
    .addr _st 13
    .reg r_me_ctx, r_crypto_ctx


    // figure me and crypto contexts for this thread
    thread_id(r_me_ctx, 0)
    alu [ r_crypto_ctx, --, B, r_me_ctx, >>1 ]
    alu [ r_crypto_ctx, r_crypto_ctx, +, CRYPTO_START_CTX]


    //
    // initialize input request ring, clear counters, clear sa flush flags
    //
    .if ( r_me_ctx == 0 )
        .if ( r_crypto_ctx == 0 )
            #if (REQ_RING_SIZE > 0 )
                #if ( REQ_RING_TYPE == CRYPTO_RING_EMU0 \
                      || REQ_RING_TYPE == CRYPTO_RING_EMU1 \
                      || REQ_RING_TYPE == CRYPTO_RING_EMU2 )
                    #if ( REQ_RING_USE_ONE_EMU_RING == 1 )
                    // to run with 2 islands & 1 req ring, don't
                    // init ring if on 2nd island
                    .begin
                    .reg r_isld
                    local_csr_rd[ACTIVE_CTX_STS]
                    immed[r_isld, 0]
                    alu[r_isld, 0x3f, AND, r_isld, >>25]
                    .if ( r_isld != 12 )
                        // instead, wait for init flag from
                        // other crypto island init
                        crypto_threads_wait_init_flag()
                    .else
                    #endif /* REQ_RING_USE_ONE_EMU_RING */
                        #if ( REQ_RING_USE_ALLOC_MEM == 0 )
                            crypto_threads_clr_emem(crypto_req_ring, REQ_RING_SIZE)
                        #endif
                        #pragma warning(disable:5008)
                        ru_emem_ring_setup(crypto_req_ring_desc, crypto_req_ring, \
                                            REQ_RING_NUM, REQ_RING_SIZE, 0)
                        #pragma warning(default:5008)
                    #if ( REQ_RING_USE_ONE_EMU_RING == 1 )
                    .endif
                    .end
                    #endif
                #elif ( REQ_RING_TYPE == CRYPTO_RING_CTM )
                    #if ( REQ_RING_USE_ALLOC_MEM == 0 )
                        crypto_threads_clr_ctm(crypto_req_ring, REQ_RING_SIZE)
                    #endif
                    ru_ctm_ring_setup(REQ_RING_NUM, crypto_req_ring, \
                                                  REQ_RING_SIZE, FULL )
                #else
                    #error "crypto_threads_init: invalid input ring type."
                #endif
            #endif
            #if ( INIT_FLAG_USE_ALLOC_MEM == 0 )
                crypto_threads_clear_init_flag()
            #endif
            #if ( COUNTER_USE_ALLOC_MEM == 0 )
                crypto_threads_clear_counters()
            #endif
            #if ( ENABLE_SA_FLUSH && SA_FLUSH_FLAGS_USE_ALLOC_MEM == 0 )
                crypto_threads_clear_sa_flush_flags()
            #endif
        .endif
    .endif


    //
    // synchronize two me's
    //

    .if ( r_me_ctx == 0 )
        .if ( r_crypto_ctx )
            // this is the 1st thread on the 2nd me.
            // wait for signal from 1st me
            #pragma warning(disable:4700)
            signal_wait(_ss, THD_SWAP)
            #pragma warning(default:4700)
        .endif
    .endif


    //
    // do the init cores for this me one core at a time
    //

    .if ( r_me_ctx == 7 ) // signal ctx 0 to start
        signal_next_ctx(&_st)
    .endif

    #pragma warning(disable:4700)
    signal_wait(_st, THD_SWAP) // wait for prior thread to signal
    #pragma warning(default:4700)

    .if ( r_me_ctx < CRYPTO_NUM_THREADS )
        .if ( (r_me_ctx & 1) == 0 )
            // init core
            crypto_threads_init_core (r_crypto_ctx)
        .endif
    .endif

    // signal next thread to go
    signal_next_ctx(&_st)

    // wait again for prior thread to be done with init core
    #pragma warning(disable:4700)
    signal_wait(_st, THD_SWAP)
    #pragma warning(default:4700)

    // signal next thread to go
    signal_next_ctx(&_st)


    //
    // synchronize two me's
    //
    .if ( r_me_ctx == 7 )
        // this is the last thread on the 1st me.
        // signal thread 0 in the next neighbor
         signal_next_me(&_ss)
    .endif


    //
    // flag init done, first me will do this. could update
    // this logic so if 2 me's in use, will wait for 2nd me
    // to be done. however, it will work as is.
    //
    .if ( r_me_ctx == 7 )
        crypto_threads_update_counter(CRYPTO_CNTR_INIT_FLAG)
        crypto_threads_set_init_flag()
    .endif



    //
    // now, run the input or output thread if enabled
    //
    .if ( r_me_ctx < CRYPTO_NUM_THREADS )

        .if ( (r_me_ctx & 1) == 0 )
            //init input thread
            crypto_threads_input(r_me_ctx, r_crypto_ctx, REQ_RING_TYPE, \
                                                           REQ_RING_NUM )
        .else
            // init output thread
            crypto_threads_output(r_me_ctx, r_crypto_ctx)
        .endif

    .else
        // this thread is not going to run the crypto thread code;
        // if want to continue to run other code in this thread,
        // comment out next line
        ctx_arb[kill]

    .endif

     nop // need to stop nfas error

    .end

#endm

/** @}
 * @}
 */

#endif /* __CRYPTO_LIB_THREADS_UC__ */
