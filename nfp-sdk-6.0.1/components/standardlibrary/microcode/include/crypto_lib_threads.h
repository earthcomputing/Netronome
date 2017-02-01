/*
 * Copyright (C) 2014 Netronome Systems, Inc. All rights reserved.
 */


#ifndef __CRYPTO_LIB_THREADS_H__
#define __CRYPTO_LIB_THREADS_H__


/**
 *  Shared definitions used in crypto_lib_threads.uc
 */


/**
 *  Response status codes
 *
 *  All codes other than CRYPTO_RSP_STATUS_OK (0) signify an
 *  error condition.
 */

// returned from crypto_lib_threads ucode
#define CRYPTO_RSP_STATUS_OK                                   0
#define CRYPTO_RSP_STATUS_AUTH_CHECK_FAIL                      1
#define CRYPTO_RSP_STATUS_SA_SELECTION_ERR                     2
#define CRYPTO_RSP_STATUS_WRITE_BACK_ALIGN_ERR                 3
#define CRYPTO_RSP_STATUS_WRITE_BACK_LENGTH_ERR                4
#define CRYPTO_RSP_STATUS_REPLAY_TOO_SMALL                     5
#define CRYPTO_RSP_STATUS_REPLAY_ALREADY_USED                  6

// returned from pkt processing ucode, when error detected
// prior to forwarding packet/request to crypto_lib_threads
// microcode. not used directly in crypto_lib_threads.uc
#define CRYPTO_RSP_STATUS_NEED_EOP_NONE_SUPPLIED                  16
#define CRYPTO_RSP_STATUS_HARD_BYTE_CNT_EXCEEDED                  17
#define CRYPTO_RSP_STATUS_SEQ_CNTR_WRAPPED                        18
#define CRYPTO_RSP_STATUS_PP_REPLAY_TOO_SMALL                     19
#define CRYPTO_RSP_STATUS_PP_REPLAY_ALREADY_USED                  20
#define CRYPTO_RSP_STATUS_PMTU_EXCEEDED                           21
#define CRYPTO_RSP_STATUS_L2_HDR_LEN_TOO_BIG                      22
#define CRYPTO_RSP_STATUS_SA_CTRL_WORD_CONFIG_ERROR               23
#define CRYPTO_RSP_STATUS_MAX_NUM_IPV6_EXT_HDRS_EXCEEDED          24
#define CRYPTO_RSP_STATUS_IPV6_DECRYPT_IPSEC_HDR_NOT_FOUND        25
#define CRYPTO_RSP_STATUS_IPV6_EXT_HDRS_END_NOT_FOUND_IN_SOP_BUF  26
#define CRYPTO_RSP_STATUS_IPV6_IP_HDR_TOO_BIG_4_LOCAL_CTM_BUF     27
#define CRYPTO_RSP_STATUS_LAST                                    28


/**
 * Stats/Counters
 *
 * By default, counters are maintained in the crypto island CLS
 * starting at COUNTER_BASE_ADDR.
 *
 * Each counter definition defines the counter number. The counters are
 * stored in CLS or CTM at 32 or 64 bit offsets based on the counter number
 * and the value of COUNTER_LENGTH
 */

// counter length
#ifndef CRYPTO_COUNTER_LENGTH
#define CRYPTO_COUNTER_LENGTH 32
#endif

// debug counters
#define CRYPTO_CNTR_INIT_FLAG               0
#define CRYPTO_CNTR_INIT_CORE               1
#define CRYPTO_CNTR_INPUT_THREAD_FLAG       2
#define CRYPTO_CNTR_OUTPUT_THREAD_FLAG      3
#define CRYPTO_CNTR_CMD_RING_READ           4
#define CRYPTO_CNTR_CMD_READ                5
#define CRYPTO_CNTR_CMD_READ_ENCRYPT        6
#define CRYPTO_CNTR_CMD_READ_DECRYPT        7
#define CRYPTO_CNTR_SA_REUSE                8
#define CRYPTO_CNTR_CMD_SEND_1              9
#define CRYPTO_CNTR_CMD_SEND_2              10
#define CRYPTO_CNTR_CMD_SEND_3              11
#define CRYPTO_CNTR_CMD_SEND_4              12
#define CRYPTO_CNTR_OUTPUT_PKT_SIG          13
#define CRYPTO_CNTR_OUTPUT_ENCRYPT_ESP      14
#define CRYPTO_CNTR_OUTPUT_DECRYPT_ESP      15
#define CRYPTO_CNTR_OUTPUT_ENCRYPT_AH       16
#define CRYPTO_CNTR_OUTPUT_DECRYPT_AH       17
#define CRYPTO_CNTR_JUMBO_FIRST             18
#define CRYPTO_CNTR_JUMBO_CONTINUE          19
#define CRYPTO_CNTR_JUMBO_END               20
#define CRYPTO_CNTR_DBG_0                   21
#define CRYPTO_CNTR_DBG_1                   22
#define CRYPTO_CNTR_DBG_2                   23
#define CRYPTO_CNTR_DBG_3                   24
#define CRYPTO_CNTR_DBG_4                   25
#define CRYPTO_CNTR_DBG_5                   26
#define CRYPTO_CNTR_DEBUG_LAST              31

// real counters
#define CRYPTO_CNTR_PKTS_ENCRYPT_CORE_0     32
#define CRYPTO_CNTR_PKTS_ENCRYPT_CORE_1     33
#define CRYPTO_CNTR_PKTS_ENCRYPT_CORE_2     34
#define CRYPTO_CNTR_PKTS_ENCRYPT_CORE_3     35
#define CRYPTO_CNTR_PKTS_ENCRYPT_CORE_4     36
#define CRYPTO_CNTR_PKTS_ENCRYPT_CORE_5     37
#define CRYPTO_CNTR_PKTS_DECRYPT_CORE_0     38
#define CRYPTO_CNTR_PKTS_DECRYPT_CORE_1     39
#define CRYPTO_CNTR_PKTS_DECRYPT_CORE_2     40
#define CRYPTO_CNTR_PKTS_DECRYPT_CORE_3     41
#define CRYPTO_CNTR_PKTS_DECRYPT_CORE_4     42
#define CRYPTO_CNTR_PKTS_DECRYPT_CORE_5     43
#define CRYPTO_CNTR_PKTS_ENCRYPT_ESP_TUNNEL 44
#define CRYPTO_CNTR_PKTS_ENCRYPT_ESP_TRANS  45
#define CRYPTO_CNTR_PKTS_ENCRYPT_AH_TUNNEL  46
#define CRYPTO_CNTR_PKTS_ENCRYPT_AH_TRANS   47
#define CRYPTO_CNTR_PKTS_DECRYPT_ESP_TUNNEL 48
#define CRYPTO_CNTR_PKTS_DECRYPT_ESP_TRANS  49
#define CRYPTO_CNTR_PKTS_DECRYPT_AH_TUNNEL  50
#define CRYPTO_CNTR_PKTS_DECRYPT_AH_TRANS   51
#define CRYPTO_CNTR_PKTS_ENCRYPT_NONE       52
#define CRYPTO_CNTR_PKTS_ENCRYPT_AES128     53
#define CRYPTO_CNTR_PKTS_ENCRYPT_AES192     54
#define CRYPTO_CNTR_PKTS_ENCRYPT_AES256     55
#define CRYPTO_CNTR_PKTS_ENCRYPT_3DES       56
#define CRYPTO_CNTR_PKTS_ENCRYPT_HASH_NONE  57
#define CRYPTO_CNTR_PKTS_ENCRYPT_SHA1       58
#define CRYPTO_CNTR_PKTS_ENCRYPT_SHA256     59
#define CRYPTO_CNTR_PKTS_ENCRYPT_SHA512     60
#define CRYPTO_CNTR_PKTS_DECRYPT_NONE       61
#define CRYPTO_CNTR_PKTS_DECRYPT_AES128     62
#define CRYPTO_CNTR_PKTS_DECRYPT_AES192     63
#define CRYPTO_CNTR_PKTS_DECRYPT_AES256     64
#define CRYPTO_CNTR_PKTS_DECRYPT_3DES       65
#define CRYPTO_CNTR_PKTS_DECRYPT_HASH_NONE  66
#define CRYPTO_CNTR_PKTS_DECRYPT_SHA1       67
#define CRYPTO_CNTR_PKTS_DECRYPT_SHA256     68
#define CRYPTO_CNTR_PKTS_DECRYPT_SHA512     69
#define CRYPTO_CNTR_PKTS_AUTH_CHECK_FAIL    70
#define CRYPTO_CNTR_SA_MODE_SEL_ERR         71
#define CRYPTO_CNTR_SA_PROTOCOL_SEL_ERR     72
#define CRYPTO_CNTR_SA_PROT_MODE_SEL_ERR    73
#define CRYPTO_CNTR_SA_CIPHER_SEL_ERR       74
#define CRYPTO_CNTR_SA_CIPHER_MODE_SEL_ERR  75
#define CRYPTO_CNTR_SA_HASH_SEL_ERR         76
#define CRYPTO_CNTR_INTERNAL_ERR            77
#define CRYPTO_CNTR_RSP_RING_WRITE_ERR      78
#define CRYPTO_CNTR_BUF_RING_WRITE_ERR      79
#define CRYPTO_CNTR_CMD_RING_EMPTY          80
#define CRYPTO_CNTR_LAST                    81
/* Need a power of 2 >= last counter */
#define CRYPTO_CNTR_MAX                     128


#endif /* __CRYPTO_LIB_THREADS_H__ */
