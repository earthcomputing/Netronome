/*
 * Copyright (C) 2012-2015,  Netronome Systems, Inc.  All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file          src/npe_hash.c
 * @brief         Hash operations
 */

/* Compilation for Netronome NFP */
#if defined(__NFP_TOOL_NFCC)
    #ifndef NFP_LIB_ANY_NFCC_VERSION
        #if (__NFP_TOOL_NFCC < NFP_SW_VERSION(5, 0, 0, 0)) ||   \
            (__NFP_TOOL_NFCC > NFP_SW_VERSION(6, 255, 255, 255))
            #error "This software is not supported for the version of the SDK currently in use."
         #endif
    #endif
#endif

/* flowenv code */
#ifdef __NFP_TOOL_NFCC
#include <assert.h>
#include <nfp/mem_bulk.h>
#include <nfp/me.h>
#endif

/* npe code */
#include <npe_types.h>
#include <npe_hash.h>


enum npe_hash_crc_op {
    CRC_32_BE,
    CRC_32_BE_SWAP,
    CRC_32_LE,
    CRC_32_LE_SWAP,
    CRC_CCITT_BE,
    CRC_CCITT_BE_SWAP,
    CRC_CCITT_LE,
    CRC_CCITT_LE_SWAP,
    CRC_ISCSI_BE,
    CRC_ISCSI_BE_SWAP,
    CRC_ISCSI_LE,
    CRC_ISCSI_LE_SWAP,
    CRC_10_BE,
    CRC_10_BE_SWAP,
    CRC_10_LE,
    CRC_10_LE_SWAP,
    CRC_5_BE,
    CRC_5_BE_SWAP,
    CRC_5_LE,
    CRC_5_LE_SWAP,
    CRC_NONE,
    CRC_NONE_SWAP
};

enum npe_hash_crc_bytes {
    BYTES_0_3,
    BYTES_0_2,
    BYTES_0_1,
    BYTE_0,
    BYTES_1_3,
    BYTES_2_3,
    BYTE_3,
};


#ifndef __NFP_TOOL_NFCC
static const unsigned int crc32_table[256] = {
    0x00000000U,0x04C11DB7U,0x09823B6EU,0x0D4326D9U,
    0x130476DCU,0x17C56B6BU,0x1A864DB2U,0x1E475005U,
    0x2608EDB8U,0x22C9F00FU,0x2F8AD6D6U,0x2B4BCB61U,
    0x350C9B64U,0x31CD86D3U,0x3C8EA00AU,0x384FBDBDU,
    0x4C11DB70U,0x48D0C6C7U,0x4593E01EU,0x4152FDA9U,
    0x5F15ADACU,0x5BD4B01BU,0x569796C2U,0x52568B75U,
    0x6A1936C8U,0x6ED82B7FU,0x639B0DA6U,0x675A1011U,
    0x791D4014U,0x7DDC5DA3U,0x709F7B7AU,0x745E66CDU,
    0x9823B6E0U,0x9CE2AB57U,0x91A18D8EU,0x95609039U,
    0x8B27C03CU,0x8FE6DD8BU,0x82A5FB52U,0x8664E6E5U,
    0xBE2B5B58U,0xBAEA46EFU,0xB7A96036U,0xB3687D81U,
    0xAD2F2D84U,0xA9EE3033U,0xA4AD16EAU,0xA06C0B5DU,
    0xD4326D90U,0xD0F37027U,0xDDB056FEU,0xD9714B49U,
    0xC7361B4CU,0xC3F706FBU,0xCEB42022U,0xCA753D95U,
    0xF23A8028U,0xF6FB9D9FU,0xFBB8BB46U,0xFF79A6F1U,
    0xE13EF6F4U,0xE5FFEB43U,0xE8BCCD9AU,0xEC7DD02DU,
    0x34867077U,0x30476DC0U,0x3D044B19U,0x39C556AEU,
    0x278206ABU,0x23431B1CU,0x2E003DC5U,0x2AC12072U,
    0x128E9DCFU,0x164F8078U,0x1B0CA6A1U,0x1FCDBB16U,
    0x018AEB13U,0x054BF6A4U,0x0808D07DU,0x0CC9CDCAU,
    0x7897AB07U,0x7C56B6B0U,0x71159069U,0x75D48DDEU,
    0x6B93DDDBU,0x6F52C06CU,0x6211E6B5U,0x66D0FB02U,
    0x5E9F46BFU,0x5A5E5B08U,0x571D7DD1U,0x53DC6066U,
    0x4D9B3063U,0x495A2DD4U,0x44190B0DU,0x40D816BAU,
    0xACA5C697U,0xA864DB20U,0xA527FDF9U,0xA1E6E04EU,
    0xBFA1B04BU,0xBB60ADFCU,0xB6238B25U,0xB2E29692U,
    0x8AAD2B2FU,0x8E6C3698U,0x832F1041U,0x87EE0DF6U,
    0x99A95DF3U,0x9D684044U,0x902B669DU,0x94EA7B2AU,
    0xE0B41DE7U,0xE4750050U,0xE9362689U,0xEDF73B3EU,
    0xF3B06B3BU,0xF771768CU,0xFA325055U,0xFEF34DE2U,
    0xC6BCF05FU,0xC27DEDE8U,0xCF3ECB31U,0xCBFFD686U,
    0xD5B88683U,0xD1799B34U,0xDC3ABDEDU,0xD8FBA05AU,
    0x690CE0EEU,0x6DCDFD59U,0x608EDB80U,0x644FC637U,
    0x7A089632U,0x7EC98B85U,0x738AAD5CU,0x774BB0EBU,
    0x4F040D56U,0x4BC510E1U,0x46863638U,0x42472B8FU,
    0x5C007B8AU,0x58C1663DU,0x558240E4U,0x51435D53U,
    0x251D3B9EU,0x21DC2629U,0x2C9F00F0U,0x285E1D47U,
    0x36194D42U,0x32D850F5U,0x3F9B762CU,0x3B5A6B9BU,
    0x0315D626U,0x07D4CB91U,0x0A97ED48U,0x0E56F0FFU,
    0x1011A0FAU,0x14D0BD4DU,0x19939B94U,0x1D528623U,
    0xF12F560EU,0xF5EE4BB9U,0xF8AD6D60U,0xFC6C70D7U,
    0xE22B20D2U,0xE6EA3D65U,0xEBA91BBCU,0xEF68060BU,
    0xD727BBB6U,0xD3E6A601U,0xDEA580D8U,0xDA649D6FU,
    0xC423CD6AU,0xC0E2D0DDU,0xCDA1F604U,0xC960EBB3U,
    0xBD3E8D7EU,0xB9FF90C9U,0xB4BCB610U,0xB07DABA7U,
    0xAE3AFBA2U,0xAAFBE615U,0xA7B8C0CCU,0xA379DD7BU,
    0x9B3660C6U,0x9FF77D71U,0x92B45BA8U,0x9675461FU,
    0x8832161AU,0x8CF30BADU,0x81B02D74U,0x857130C3U,
    0x5D8A9099U,0x594B8D2EU,0x5408ABF7U,0x50C9B640U,
    0x4E8EE645U,0x4A4FFBF2U,0x470CDD2BU,0x43CDC09CU,
    0x7B827D21U,0x7F436096U,0x7200464FU,0x76C15BF8U,
    0x68860BFDU,0x6C47164AU,0x61043093U,0x65C52D24U,
    0x119B4BE9U,0x155A565EU,0x18197087U,0x1CD86D30U,
    0x029F3D35U,0x065E2082U,0x0B1D065BU,0x0FDC1BECU,
    0x3793A651U,0x3352BBE6U,0x3E119D3FU,0x3AD08088U,
    0x2497D08DU,0x2056CD3AU,0x2D15EBE3U,0x29D4F654U,
    0xC5A92679U,0xC1683BCEU,0xCC2B1D17U,0xC8EA00A0U,
    0xD6AD50A5U,0xD26C4D12U,0xDF2F6BCBU,0xDBEE767CU,
    0xE3A1CBC1U,0xE760D676U,0xEA23F0AFU,0xEEE2ED18U,
    0xF0A5BD1DU,0xF464A0AAU,0xF9278673U,0xFDE69BC4U,
    0x89B8FD09U,0x8D79E0BEU,0x803AC667U,0x84FBDBD0U,
    0x9ABC8BD5U,0x9E7D9662U,0x933EB0BBU,0x97FFAD0CU,
    0xAFB010B1U,0xAB710D06U,0xA6322BDFU,0xA2F33668U,
    0xBCB4666DU,0xB8757BDAU,0xB5365D03U,0xB1F740B4U
};

static const uint16_t crc_ccitt_table[256] = {
    0x0000,  0x1021,  0x2042,  0x3063,  0x4084,  0x50A5,  0x60C6,  0x70E7,
    0x8108,  0x9129,  0xA14A,  0xB16B,  0xC18C,  0xD1AD,  0xE1CE,  0xF1EF,
    0x1231,  0x0210,  0x3273,  0x2252,  0x52B5,  0x4294,  0x72F7,  0x62D6,
    0x9339,  0x8318,  0xB37B,  0xA35A,  0xD3BD,  0xC39C,  0xF3FF,  0xE3DE,
    0x2462,  0x3443,  0x0420,  0x1401,  0x64E6,  0x74C7,  0x44A4,  0x5485,
    0xA56A,  0xB54B,  0x8528,  0x9509,  0xE5EE,  0xF5CF,  0xC5AC,  0xD58D,
    0x3653,  0x2672,  0x1611,  0x0630,  0x76D7,  0x66F6,  0x5695,  0x46B4,
    0xB75B,  0xA77A,  0x9719,  0x8738,  0xF7DF,  0xE7FE,  0xD79D,  0xC7BC,
    0x48C4,  0x58E5,  0x6886,  0x78A7,  0x0840,  0x1861,  0x2802,  0x3823,
    0xC9CC,  0xD9ED,  0xE98E,  0xF9AF,  0x8948,  0x9969,  0xA90A,  0xB92B,
    0x5AF5,  0x4AD4,  0x7AB7,  0x6A96,  0x1A71,  0x0A50,  0x3A33,  0x2A12,
    0xDBFD,  0xCBDC,  0xFBBF,  0xEB9E,  0x9B79,  0x8B58,  0xBB3B,  0xAB1A,
    0x6CA6,  0x7C87,  0x4CE4,  0x5CC5,  0x2C22,  0x3C03,  0x0C60,  0x1C41,
    0xEDAE,  0xFD8F,  0xCDEC,  0xDDCD,  0xAD2A,  0xBD0B,  0x8D68,  0x9D49,
    0x7E97,  0x6EB6,  0x5ED5,  0x4EF4,  0x3E13,  0x2E32,  0x1E51,  0x0E70,
    0xFF9F,  0xEFBE,  0xDFDD,  0xCFFC,  0xBF1B,  0xAF3A,  0x9F59,  0x8F78,
    0x9188,  0x81A9,  0xB1CA,  0xA1EB,  0xD10C,  0xC12D,  0xF14E,  0xE16F,
    0x1080,  0x00A1,  0x30C2,  0x20E3,  0x5004,  0x4025,  0x7046,  0x6067,
    0x83B9,  0x9398,  0xA3FB,  0xB3DA,  0xC33D,  0xD31C,  0xE37F,  0xF35E,
    0x02B1,  0x1290,  0x22F3,  0x32D2,  0x4235,  0x5214,  0x6277,  0x7256,
    0xB5EA,  0xA5CB,  0x95A8,  0x8589,  0xF56E,  0xE54F,  0xD52C,  0xC50D,
    0x34E2,  0x24C3,  0x14A0,  0x0481,  0x7466,  0x6447,  0x5424,  0x4405,
    0xA7DB,  0xB7FA,  0x8799,  0x97B8,  0xE75F,  0xF77E,  0xC71D,  0xD73C,
    0x26D3,  0x36F2,  0x0691,  0x16B0,  0x6657,  0x7676,  0x4615,  0x5634,
    0xD94C,  0xC96D,  0xF90E,  0xE92F,  0x99C8,  0x89E9,  0xB98A,  0xA9AB,
    0x5844,  0x4865,  0x7806,  0x6827,  0x18C0,  0x08E1,  0x3882,  0x28A3,
    0xCB7D,  0xDB5C,  0xEB3F,  0xFB1E,  0x8BF9,  0x9BD8,  0xABBB,  0xBB9A,
    0x4A75,  0x5A54,  0x6A37,  0x7A16,  0x0AF1,  0x1AD0,  0x2AB3,  0x3A92,
    0xFD2E,  0xED0F,  0xDD6C,  0xCD4D,  0xBDAA,  0xAD8B,  0x9DE8,  0x8DC9,
    0x7C26,  0x6C07,  0x5C64,  0x4C45,  0x3CA2,  0x2C83,  0x1CE0,  0x0CC1,
    0xEF1F,  0xFF3E,  0xCF5D,  0xDF7C,  0xAF9B,  0xBFBA,  0x8FD9,  0x9FF8,
    0x6E17,  0x7E36,  0x4E55,  0x5E74,  0x2E93,  0x3EB2,  0x0ED1,  0x1EF0
};

#endif


#ifdef __NFP_TOOL_NFCC
#define _MULTI_REG_OP( reg_op, reg_op_last, s )                     \
{                                                                   \
    int last_reg;                                                   \
                                                                    \
    switch ( s & ~3) {                                              \
                                                                    \
    case 64:                                                        \
        reg_op(0); reg_op(1); reg_op(2); reg_op(3); reg_op(4);      \
        reg_op(5); reg_op(6); reg_op(7); reg_op(8); reg_op(9);      \
        reg_op(10); reg_op(11); reg_op(12); reg_op(13); reg_op(14); \
        reg_op(15);                                                 \
        last_reg = 16;                                              \
        break;                                                      \
                                                                    \
    case 60:                                                        \
        reg_op(0); reg_op(1); reg_op(2); reg_op(3); reg_op(4);      \
        reg_op(5); reg_op(6); reg_op(7); reg_op(8); reg_op(9);      \
        reg_op(10); reg_op(11); reg_op(12); reg_op(13); reg_op(14); \
        last_reg = 15;                                              \
        break;                                                      \
                                                                    \
    case 56:                                                        \
        reg_op(0); reg_op(1); reg_op(2); reg_op(3); reg_op(4);      \
        reg_op(5); reg_op(6); reg_op(7); reg_op(8); reg_op(9);      \
        reg_op(10); reg_op(11); reg_op(12); reg_op(13);             \
        last_reg = 14;                                              \
        break;                                                      \
                                                                    \
    case 52:                                                        \
        reg_op(0); reg_op(1); reg_op(2); reg_op(3); reg_op(4);      \
        reg_op(5); reg_op(6); reg_op(7); reg_op(8); reg_op(9);      \
        reg_op(10); reg_op(11); reg_op(12);                         \
        last_reg = 13;                                              \
        break;                                                      \
                                                                    \
    case 48:                                                        \
        reg_op(0); reg_op(1); reg_op(2); reg_op(3); reg_op(4);      \
        reg_op(5); reg_op(6); reg_op(7); reg_op(8); reg_op(9);      \
        reg_op(10); reg_op(11);                                     \
        last_reg = 12;                                              \
        break;                                                      \
                                                                    \
    case 44:                                                        \
        reg_op(0); reg_op(1); reg_op(2); reg_op(3); reg_op(4);      \
        reg_op(5); reg_op(6); reg_op(7); reg_op(8); reg_op(9);      \
        reg_op(10);                                                 \
        last_reg = 11;                                              \
        break;                                                      \
                                                                    \
    case 40:                                                        \
        reg_op(0); reg_op(1); reg_op(2); reg_op(3); reg_op(4);      \
        reg_op(5); reg_op(6); reg_op(7); reg_op(8); reg_op(9);      \
        last_reg = 10;                                              \
        break;                                                      \
                                                                    \
    case 36:                                                        \
        reg_op(0); reg_op(1); reg_op(2); reg_op(3); reg_op(4);      \
        reg_op(5); reg_op(6); reg_op(7); reg_op(8);                 \
        last_reg = 9;                                               \
        break;                                                      \
                                                                    \
    case 32:                                                        \
        reg_op(0); reg_op(1); reg_op(2); reg_op(3); reg_op(4);      \
        reg_op(5); reg_op(6); reg_op(7);                            \
        last_reg = 8;                                               \
        break;                                                      \
                                                                    \
    case 28:                                                        \
        reg_op(0); reg_op(1); reg_op(2); reg_op(3); reg_op(4);      \
        reg_op(5); reg_op(6);                                       \
        last_reg = 7;                                               \
        break;                                                      \
                                                                    \
    case 24:                                                        \
       reg_op(0); reg_op(1); reg_op(2); reg_op(3); reg_op(4);       \
       reg_op(5);                                                   \
       last_reg = 6;                                                \
       break;                                                       \
                                                                    \
    case 20:                                                        \
        reg_op(0); reg_op(1); reg_op(2); reg_op(3); reg_op(4);      \
        last_reg = 5;                                               \
        break;                                                      \
                                                                    \
    case 16:                                                        \
        reg_op(0);  reg_op(1);  reg_op(2);  reg_op(3);              \
        last_reg = 4;                                               \
        break;                                                      \
                                                                    \
    case 12:                                                        \
        reg_op(0);  reg_op(1);  reg_op(2);                          \
        last_reg = 3;                                               \
        break;                                                      \
                                                                    \
    case 8:                                                         \
        reg_op(0);  reg_op(1);                                      \
        last_reg = 2;                                               \
        break;                                                      \
                                                                    \
    case 4:                                                         \
        reg_op(0);                                                  \
        last_reg = 1;                                               \
        break;                                                      \
                                                                    \
    case 0:                                                         \
        last_reg = 0;                                               \
        break;                                                      \
                                                                    \
    default:                                                        \
        break;                                                      \
    }                                                               \
                                                                    \
    switch ( size & 3 ) {                                           \
                                                                    \
    case 1:                                                         \
        reg_op_last(last_reg, BYTE_0);                              \
        break;                                                      \
    case 2:                                                         \
        reg_op_last(last_reg, BYTES_0_1);                           \
        break;                                                      \
    case 3:                                                         \
        reg_op_last(last_reg, BYTES_0_2);                           \
        break;                                                      \
    default:                                                        \
        break;                                                      \
    }                                                               \
}
#endif


#ifdef __NFP_TOOL_NFCC
__NPE_INTRINSIC void
_npe_hash_crc_op(enum npe_hash_crc_op op, enum npe_hash_crc_bytes b,
                 uint32_t d)
{
    uint32_t res;

    if ( op == CRC_32_BE ) {
        if (b == BYTES_0_3)
            __asm crc_be[crc_32, res, d], bytes_0_3;
        else if (b == BYTES_0_2)
            __asm crc_be[crc_32, res, d], bytes_0_2;
        else if (b == BYTES_0_1)
            __asm crc_be[crc_32, res, d], bytes_0_1;
        else if (b == BYTE_0)
            __asm crc_be[crc_32, res, d], byte_0;
        else if (b == BYTES_1_3)
            __asm crc_be[crc_32, res, d], bytes_1_3;
        else if (b == BYTES_2_3)
            __asm crc_be[crc_32, res, d], bytes_2_3;
        else if (b == BYTE_3)
            __asm crc_be[crc_32, res, d], byte_3;
    } else if ( op == CRC_32_BE_SWAP ) {
        if (b == BYTES_0_3)
            __asm crc_be[crc_32, res, d], bytes_0_3, bit_swap;
        else if (b == BYTES_0_2)
            __asm crc_be[crc_32, res, d], bytes_0_2, bit_swap;
        else if (b == BYTES_0_1)
            __asm crc_be[crc_32, res, d], bytes_0_1, bit_swap;
        else if (b == BYTE_0)
            __asm crc_be[crc_32, res, d], byte_0, bit_swap;
        else if (b == BYTES_1_3)
            __asm crc_be[crc_32, res, d], bytes_1_3, bit_swap;
        else if (b == BYTES_2_3)
            __asm crc_be[crc_32, res, d], bytes_2_3, bit_swap;
        else if (b == BYTE_3)
            __asm crc_be[crc_32, res, d], byte_3, bit_swap;
    } else if ( op == CRC_32_LE ) {
        if (b == BYTES_0_3)
            __asm crc_le[crc_32, res, d], bytes_0_3;
        else if (b == BYTES_0_2)
            __asm crc_le[crc_32, res, d], bytes_0_2;
        else if (b == BYTES_0_1)
            __asm crc_le[crc_32, res, d], bytes_0_1;
        else if (b == BYTE_0)
            __asm crc_le[crc_32, res, d], byte_0;
        else if (b == BYTES_1_3)
            __asm crc_le[crc_32, res, d], bytes_1_3;
        else if (b == BYTES_2_3)
            __asm crc_le[crc_32, res, d], bytes_2_3;
        else if (b == BYTE_3)
            __asm crc_le[crc_32, res, d], byte_3;
    } else if ( op == CRC_32_LE_SWAP ) {
        if (b == BYTES_0_3)
            __asm crc_le[crc_32, res, d], bytes_0_3, bit_swap;
        else if (b == BYTES_0_2)
            __asm crc_le[crc_32, res, d], bytes_0_2, bit_swap;
        else if (b == BYTES_0_1)
            __asm crc_le[crc_32, res, d], bytes_0_1, bit_swap;
        else if (b == BYTE_0)
            __asm crc_le[crc_32, res, d], byte_0, bit_swap;
        else if (b == BYTES_1_3)
            __asm crc_le[crc_32, res, d], bytes_1_3, bit_swap;
        else if (b == BYTES_2_3)
            __asm crc_le[crc_32, res, d], bytes_2_3, bit_swap;
        else if (b == BYTE_3)
            __asm crc_le[crc_32, res, d], byte_3, bit_swap;
    } else if ( op == CRC_CCITT_BE ) {
        if (b == BYTES_0_3)
            __asm crc_be[crc_ccitt, res, d], bytes_0_3;
        else if (b == BYTES_0_2)
            __asm crc_be[crc_ccitt, res, d], bytes_0_2;
        else if (b == BYTES_0_1)
            __asm crc_be[crc_ccitt, res, d], bytes_0_1;
        else if (b == BYTE_0)
            __asm crc_be[crc_ccitt, res, d], byte_0;
        else if (b == BYTES_1_3)
            __asm crc_be[crc_ccitt, res, d], bytes_1_3;
        else if (b == BYTES_2_3)
            __asm crc_be[crc_ccitt, res, d], bytes_2_3;
        else if (b == BYTE_3)
            __asm crc_be[crc_ccitt, res, d], byte_3;
    } else if ( op == CRC_CCITT_BE_SWAP ) {
        if (b == BYTES_0_3)
            __asm crc_be[crc_ccitt, res, d], bytes_0_3, bit_swap;
        else if (b == BYTES_0_2)
            __asm crc_be[crc_ccitt, res, d], bytes_0_2, bit_swap;
        else if (b == BYTES_0_1)
            __asm crc_be[crc_ccitt, res, d], bytes_0_1, bit_swap;
        else if (b == BYTE_0)
            __asm crc_be[crc_ccitt, res, d], byte_0, bit_swap;
        else if (b == BYTES_1_3)
            __asm crc_be[crc_ccitt, res, d], bytes_1_3, bit_swap;
        else if (b == BYTES_2_3)
            __asm crc_be[crc_ccitt, res, d], bytes_2_3, bit_swap;
        else if (b == BYTE_3)
            __asm crc_be[crc_ccitt, res, d], byte_3, bit_swap;
    } else if ( op == CRC_CCITT_LE ) {
        if (b == BYTES_0_3)
            __asm crc_le[crc_ccitt, res, d], bytes_0_3;
        else if (b == BYTES_0_2)
            __asm crc_le[crc_ccitt, res, d], bytes_0_2;
        else if (b == BYTES_0_1)
            __asm crc_le[crc_ccitt, res, d], bytes_0_1;
        else if (b == BYTE_0)
            __asm crc_le[crc_ccitt, res, d], byte_0;
        else if (b == BYTES_1_3)
            __asm crc_le[crc_ccitt, res, d], bytes_1_3;
        else if (b == BYTES_2_3)
            __asm crc_le[crc_ccitt, res, d], bytes_2_3;
        else if (b == BYTE_3)
            __asm crc_le[crc_ccitt, res, d], byte_3;
    } else if ( op == CRC_CCITT_LE_SWAP ) {
        if (b == BYTES_0_3)
            __asm crc_le[crc_ccitt, res, d], bytes_0_3, bit_swap;
        else if (b == BYTES_0_2)
            __asm crc_le[crc_ccitt, res, d], bytes_0_2, bit_swap;
        else if (b == BYTES_0_1)
            __asm crc_le[crc_ccitt, res, d], bytes_0_1, bit_swap;
        else if (b == BYTE_0)
            __asm crc_le[crc_ccitt, res, d], byte_0, bit_swap;
        else if (b == BYTES_1_3)
            __asm crc_le[crc_ccitt, res, d], bytes_1_3, bit_swap;
        else if (b == BYTES_2_3)
            __asm crc_le[crc_ccitt, res, d], bytes_2_3, bit_swap;
        else if (b == BYTE_3)
            __asm crc_le[crc_ccitt, res, d], byte_3, bit_swap;
    }
 }
#endif

#ifdef __NFP_TOOL_NFCC
__NPE_INTRINSIC uint32_t
_npe_hash_crc(enum npe_hash_crc_op op, void *p, size_t size, uint32_t init)
{
    crc_write(init);

    if (__is_in_lmem(p)) {
        #undef _REG_OP
        #define _REG_OP(_x) _npe_hash_crc_op(op, BYTES_0_3, \
                                             ((__lmem uint32_t *)p)[_x])
        #undef _REG_OP_LAST
        #define _REG_OP_LAST(_x,_b) _npe_hash_crc_op(op, _b, \
                                             ((__lmem uint32_t *)p)[_x])
    } else {
        #undef _REG_OP
        #define _REG_OP(_x) _npe_hash_crc_op(op, BYTES_0_3, \
                                             ((__gpr uint32_t *)p)[_x])
        #undef _REG_OP_LAST
        #define _REG_OP_LAST(_x,_b) _npe_hash_crc_op(op, _b, \
                                             ((__gpr uint32_t *)p)[_x])
    }

    _MULTI_REG_OP( _REG_OP, _REG_OP_LAST, size);

    return crc_read();
}
#endif


#ifdef __NFP_TOOL_NFCC
__NPE_INTRINSIC uint32_t
_npe_hash_crc_rem(enum npe_hash_crc_op op, npe_mem_ptr_t p,
                  size_t size, uint32_t init)
{
    __xread uint32_t xfr[8];
    int bytes_left = size;
    uint64_t curr_byte = (uint64_t)p;

    crc_write(init);

    while(1) {

        if ( bytes_left >= 32) {
            mem_read64( &xfr[0], (__mem void *)curr_byte, 32);
            _npe_hash_crc_op(op, BYTES_0_3, xfr[0]);
            _npe_hash_crc_op(op, BYTES_0_3, xfr[1]);
            _npe_hash_crc_op(op, BYTES_0_3, xfr[2]);
            _npe_hash_crc_op(op, BYTES_0_3, xfr[3]);
            _npe_hash_crc_op(op, BYTES_0_3, xfr[4]);
            _npe_hash_crc_op(op, BYTES_0_3, xfr[5]);
            _npe_hash_crc_op(op, BYTES_0_3, xfr[6]);
            _npe_hash_crc_op(op, BYTES_0_3, xfr[7]);
            bytes_left -= 32;
            curr_byte += 32;
        } else if ( bytes_left >= 24) {
            mem_read64( &xfr[0], (__mem void *)curr_byte, 24);
            _npe_hash_crc_op(op, BYTES_0_3, xfr[0]);
            _npe_hash_crc_op(op, BYTES_0_3, xfr[1]);
            _npe_hash_crc_op(op, BYTES_0_3, xfr[2]);
            _npe_hash_crc_op(op, BYTES_0_3, xfr[3]);
            _npe_hash_crc_op(op, BYTES_0_3, xfr[4]);
            _npe_hash_crc_op(op, BYTES_0_3, xfr[5]);
            bytes_left -= 24;
            curr_byte += 24;
        } else if ( bytes_left >= 16) {
            mem_read64( &xfr[0], (__mem void *)curr_byte, 16);
            _npe_hash_crc_op(op, BYTES_0_3, xfr[0]);
            _npe_hash_crc_op(op, BYTES_0_3, xfr[1]);
            _npe_hash_crc_op(op, BYTES_0_3, xfr[2]);
            _npe_hash_crc_op(op, BYTES_0_3, xfr[3]);
            bytes_left -= 16;
            curr_byte += 16;
        } else if ( bytes_left >= 8) {
            mem_read64( &xfr[0], (__mem void *)curr_byte, 8);
            _npe_hash_crc_op(op, BYTES_0_3, xfr[0]);
            _npe_hash_crc_op(op, BYTES_0_3, xfr[1]);
            bytes_left -= 8;
            curr_byte += 8;
        } else if ( bytes_left >= 4) {
            mem_read32( &xfr[0], (__mem void *)curr_byte, 4);
            _npe_hash_crc_op(op, BYTES_0_3, xfr[0]);
            bytes_left -= 4;
            curr_byte += 4;
        } else {
            mem_read8( &xfr[0], (__mem void *)curr_byte, bytes_left);
            switch ( bytes_left ) {
            case 1:
                _npe_hash_crc_op(op, BYTE_0, xfr[0]);
                bytes_left -= 1;
                break;
            case 2:
                _npe_hash_crc_op(op, BYTES_0_1, xfr[0]);
                bytes_left -= 2;
                break;
            case 3:
                _npe_hash_crc_op(op, BYTES_0_2, xfr[0]);
                bytes_left -= 3;
                break;
            default:
                break;
            }
        }

        if ( bytes_left == 0 )
            break;
    }

    return crc_read();
}
#endif


__NPE_INTRINSIC uint32_t
npe_hash_crc_32(void *p, size_t size, uint32_t init,
                enum npe_endian endian, enum npe_bit_swap swap)
{

#ifdef __NFP_TOOL_NFCC

    /* N.B. In order to support both lmem and reg data,
       need to have constant values for size, endian, swap
       due to various compiler nuances. Also limiting
       max data size for similar reason. */

    ctassert(__is_ct_const(size));
    ctassert(size <= 67);
    ctassert(__is_ct_const(endian));
    ctassert(__is_ct_const(swap));

    #if ( endian == NPE_BIG_ENDIAN )
        #if ( swap == NPE_BIT_SWAP_DISABLE )
            return _npe_hash_crc(CRC_32_BE, p, size, init);
        #else
            return _npe_hash_crc(CRC_32_BE_SWAP, p, size, init);
        #endif
    #else
        #if ( swap == NPE_BIT_SWAP_DISABLE )
            return _npe_hash_crc(CRC_32_LE, p, size, init);
        #else
            return _npe_hash_crc(CRC_32_LE_SWAP, p, size, init);
        #endif
    #endif

#else

    // crc-32 polynomial: 0x104C11DB7

    uint32_t crc = init;
    int len = size;
    char *data = (char *)p;

    if ( endian == NPE_BIG_ENDIAN ) {

        for (;;) {

            char d;

            d = *(data+3);
            crc = crc32_table[(d & 0xff) ^ ((crc >> 24) & 0xff)] ^ (crc << 8);
            len--;
            if ( len <= 0 ) break;

            d = *(data+2);
            crc = crc32_table[(d & 0xff) ^ ((crc >> 24) & 0xff)] ^ (crc << 8);
            len--;
            if ( len <= 0 ) break;

            d = *(data+1);
            crc = crc32_table[(d & 0xff) ^ ((crc >> 24) & 0xff)] ^ (crc << 8);
            len--;
            if ( len <= 0 ) break;

            d = *(data+0);
            crc = crc32_table[(d & 0xff) ^ ((crc >> 24) & 0xff)] ^ (crc << 8);
            len--;
            if ( len <= 0 ) break;

            data+=4;
        }
    } else {
        for (;;) {
            crc = crc32_table[(*data & 0xff) ^ ((crc >> 24) & 0xff)] ^ (crc << 8);
            data++;
            len--;
            if ( len <= 0 ) break;
        }
    }

    return crc;

#endif
}


uint32_t
npe_hash_crc_32_rem(npe_mem_ptr_t p, size_t size, uint32_t init,
                    enum npe_endian endian, enum npe_bit_swap swap)
{
#ifdef __NFP_TOOL_NFCC
    if ( endian == NPE_BIG_ENDIAN ) {
        if ( swap == NPE_BIT_SWAP_DISABLE )
            return _npe_hash_crc_rem(CRC_32_BE, p, size, init);
        else
            return _npe_hash_crc_rem(CRC_32_BE_SWAP, p, size, init);
    } else {
        if ( swap == NPE_BIT_SWAP_DISABLE )
            return _npe_hash_crc_rem(CRC_32_LE, p, size, init);
        else
            return _npe_hash_crc_rem(CRC_32_LE_SWAP, p, size, init);
    }
#else
    return npe_hash_crc_32(p, size, init, endian, swap);
#endif
}


__NPE_INTRINSIC uint32_t
npe_hash_crc_ccitt(void *p, size_t size, uint32_t init,
                enum npe_endian endian, enum npe_bit_swap swap)
{

#ifdef __NFP_TOOL_NFCC

    /* N.B. In order to support both lmem and reg data,
       need to have constant values for size, endian, swap
       due to various compiler nuances. Also limiting
       max data size for similar reason. */

    ctassert(__is_ct_const(size));
    ctassert(size <= 67);
    ctassert(__is_ct_const(endian));
    ctassert(__is_ct_const(swap));

    #if ( endian == NPE_BIG_ENDIAN )
        #if ( swap == NPE_BIT_SWAP_DISABLE )
            return _npe_hash_crc(CRC_CCITT_BE, p, size, init);
        #else
            return _npe_hash_crc(CRC_CCITT_BE_SWAP, p, size, init);
        #endif
    #else
        #if ( swap == NPE_BIT_SWAP_DISABLE )
            return _npe_hash_crc(CRC_CCITT_LE, p, size, init);
        #else
            return _npe_hash_crc(CRC_CCITT_LE_SWAP, p, size, init);
        #endif
    #endif

#else

    // crc-ccitt polynomial: 0x11021

    uint32_t crc = init & 0xffff;
    int len = size;
    char *data = (char *)p;

    if ( endian == NPE_BIG_ENDIAN ) {

        for (;;) {

            char d;

            d = *(data+3);
            crc = crc_ccitt_table[(crc >> 8 ^ d) & 0xffU] ^ (crc << 8);
            len--;
            if ( len <= 0 ) break;

            d = *(data+2);
            crc = crc_ccitt_table[(crc >> 8 ^ d) & 0xffU] ^ (crc << 8);
            len--;
            if ( len <= 0 ) break;

            d = *(data+1);
            crc = crc_ccitt_table[(crc >> 8 ^ d) & 0xffU] ^ (crc << 8);
            len--;
            if ( len <= 0 ) break;

            d = *(data+0);
            crc = crc_ccitt_table[(crc >> 8 ^ d) & 0xffU] ^ (crc << 8);
            len--;
            if ( len <= 0 ) break;

            data+=4;
        }
    } else {
        for (;;) {
            crc = crc_ccitt_table[(crc >> 8 ^ *data) & 0xffU] ^ (crc << 8);
            data++;
            len--;
            if ( len <= 0 ) break;
        }
    }

    return crc & 0xffffU;

#endif
}


uint32_t
npe_hash_crc_ccitt_rem(npe_mem_ptr_t p, size_t size, uint32_t init,
                       enum npe_endian endian, enum npe_bit_swap swap)
{
#ifdef __NFP_TOOL_NFCC
    if ( endian == NPE_BIG_ENDIAN ) {
        if ( swap == NPE_BIT_SWAP_DISABLE )
            return _npe_hash_crc_rem(CRC_CCITT_BE, p, size, init);
        else
            return _npe_hash_crc_rem(CRC_CCITT_BE_SWAP, p, size, init);
    } else {
        if ( swap == NPE_BIT_SWAP_DISABLE )
            return _npe_hash_crc_rem(CRC_CCITT_LE, p, size, init);
        else
            return _npe_hash_crc_rem(CRC_CCITT_LE_SWAP, p, size, init);
    }
#else /* GCC */
    return npe_hash_crc_ccitt(p, size, init, endian, swap);
#endif
}


#define _npe_hash_fletcher_16_op(s1, s2, b, d)  \
{                                               \
    if (b == BYTES_0_3)  {                      \
        s1 += ((d >> 24) & 0xff);               \
        s2 += s1;                               \
        s1 += ((d >> 16) & 0xff);               \
        s2 += s1;                               \
        s1 += ((d >> 8) & 0xff);                \
        s2 += s1;                               \
        s1 += ((d >> 0) & 0xff);                \
        s2 += s1;                               \
    } else if (b == BYTES_0_2) {                \
        s1 += ((d  >> 24) & 0xff);              \
        s2 += s1;                               \
        s1 += ((d >> 16) & 0xff);               \
        s2 += s1;                               \
        s1 += ((d >> 8) & 0xff);                \
        s2 += s1;                               \
    } else if (b == BYTES_0_1) {                \
        s1 += ((d  >> 24) & 0xff);              \
        s2 += s1;                               \
        s1 += ((d >> 16) & 0xff);               \
        s2 += s1;                               \
    } else if (b == BYTE_0) {                   \
        s1 += ((d  >> 24) & 0xff);              \
        s2 += s1;                               \
    }                                           \
                                                \
    s1 = (s1 & 0xff) + (s1 >> 8);               \
    s2 = (s2 & 0xff) + (s2 >> 8);               \
}


__NPE_INTRINSIC uint16_t
npe_hash_fletcher_16(void *p, size_t size)
{

#ifdef __NFP_TOOL_NFCC

    __gpr uint32_t sum1 = 0xff;
    __gpr uint32_t sum2 = 0xff;

    /* N.B. In order to support both lmem and reg data,
       need to have constant values for size
       due to various compiler nuances. Also limiting
       max data size for similar reason. */

    ctassert(__is_ct_const(size));
    ctassert(size <= 67);

    if (__is_in_lmem(p)) {
        #undef _REG_OP
        #define _REG_OP(_x) _npe_hash_fletcher_16_op(sum1, sum2, BYTES_0_3, \
                                             ((__lmem uint32_t *)p)[_x])
        #undef _REG_OP_LAST
        #define _REG_OP_LAST(_x,_b) _npe_hash_fletcher_16_op(sum1, sum2, _b, \
                                             ((__lmem uint32_t *)p)[_x])
    } else {
        #undef _REG_OP
        #define _REG_OP(_x) _npe_hash_fletcher_16_op(sum1, sum2, BYTES_0_3, \
                                             ((__gpr uint32_t *)p)[_x])
        #undef _REG_OP_LAST
        #define _REG_OP_LAST(_x,_b) _npe_hash_fletcher_16_op(sum1, sum2, _b, \
                                             ((__gpr uint32_t *)p)[_x])
    }

    _MULTI_REG_OP( _REG_OP, _REG_OP_LAST, size );

#else /* GCC */

    uint32_t *curr_byte = (uint32_t *)p;
    uint32_t sum1 = 0xff;
    uint32_t sum2 = 0xff;
    int bytes_left = size;

    while(1) {

        uint32_t val = *curr_byte++;

        if ( bytes_left >= 4) {
            _npe_hash_fletcher_16_op(sum1, sum2, BYTES_0_3, val);
            bytes_left -= 4;
        } else {
            switch ( bytes_left ) {
            case 1:
                _npe_hash_fletcher_16_op(sum1, sum2, BYTE_0, val);
                bytes_left -= 1;
                break;
            case 2:
                _npe_hash_fletcher_16_op(sum1, sum2, BYTES_0_1, val);
                bytes_left -= 2;
                break;
            case 3:
                _npe_hash_fletcher_16_op(sum1, sum2, BYTES_0_2, val);
                bytes_left -= 3;
                break;
            default:
                break;
            }
        }

        if ( bytes_left == 0 )
            break;
    }

    #endif

 done:
    return ( sum2 << 8 | sum1 );
}


uint16_t
npe_hash_fletcher_16_rem(npe_mem_ptr_t p, size_t size)
{
#ifdef __NFP_TOOL_NFCC

    __xread uint32_t xfr[8];
    int bytes_left = size;
    uint64_t curr_byte = (uint64_t)p;
    __gpr uint32_t sum1 = 0xff;
    __gpr uint32_t sum2 = 0xff;

    while(1) {

        if ( bytes_left >= 32) {
            mem_read64( &xfr[0], (__mem void *)curr_byte, 32);
            _npe_hash_fletcher_16_op(sum1, sum2, BYTES_0_3, xfr[0]);
            _npe_hash_fletcher_16_op(sum1, sum2, BYTES_0_3, xfr[1]);
            _npe_hash_fletcher_16_op(sum1, sum2, BYTES_0_3, xfr[2]);
            _npe_hash_fletcher_16_op(sum1, sum2, BYTES_0_3, xfr[3]);
            _npe_hash_fletcher_16_op(sum1, sum2, BYTES_0_3, xfr[4]);
            _npe_hash_fletcher_16_op(sum1, sum2, BYTES_0_3, xfr[5]);
            _npe_hash_fletcher_16_op(sum1, sum2, BYTES_0_3, xfr[6]);
            _npe_hash_fletcher_16_op(sum1, sum2, BYTES_0_3, xfr[7]);
            bytes_left -= 32;
            curr_byte += 32;
        } else if ( bytes_left >= 24) {
            mem_read64( &xfr[0], (__mem void *)curr_byte, 24);
            _npe_hash_fletcher_16_op(sum1, sum2, BYTES_0_3, xfr[0]);
            _npe_hash_fletcher_16_op(sum1, sum2, BYTES_0_3, xfr[1]);
            _npe_hash_fletcher_16_op(sum1, sum2, BYTES_0_3, xfr[2]);
            _npe_hash_fletcher_16_op(sum1, sum2, BYTES_0_3, xfr[3]);
            _npe_hash_fletcher_16_op(sum1, sum2, BYTES_0_3, xfr[4]);
            _npe_hash_fletcher_16_op(sum1, sum2, BYTES_0_3, xfr[5]);
            bytes_left -= 24;
            curr_byte += 24;
        } else if ( bytes_left >= 16) {
            mem_read64( &xfr[0], (__mem void *)curr_byte, 16);
            _npe_hash_fletcher_16_op(sum1, sum2, BYTES_0_3, xfr[0]);
            _npe_hash_fletcher_16_op(sum1, sum2, BYTES_0_3, xfr[1]);
            _npe_hash_fletcher_16_op(sum1, sum2, BYTES_0_3, xfr[2]);
            _npe_hash_fletcher_16_op(sum1, sum2, BYTES_0_3, xfr[3]);
            bytes_left -= 16;
            curr_byte += 16;
        } else if ( bytes_left >= 8) {
            mem_read64( &xfr[0], (__mem void *)curr_byte, 8);
            _npe_hash_fletcher_16_op(sum1, sum2, BYTES_0_3, xfr[0]);
            _npe_hash_fletcher_16_op(sum1, sum2, BYTES_0_3, xfr[1]);
            bytes_left -= 8;
            curr_byte += 8;
        } else if ( bytes_left >= 4) {
            mem_read32( &xfr[0], (__mem void *)curr_byte, 4);
            _npe_hash_fletcher_16_op(sum1, sum2, BYTES_0_3, xfr[0]);
            bytes_left -= 4;
            curr_byte += 4;
        } else {
            mem_read8( &xfr[0], (__mem void *)curr_byte, bytes_left);
            switch ( bytes_left ) {
            case 1:
                _npe_hash_fletcher_16_op(sum1, sum2, BYTE_0, xfr[0]);
                bytes_left -= 1;
                break;
            case 2:
                _npe_hash_fletcher_16_op(sum1, sum2, BYTES_0_1, xfr[0]);
                bytes_left -= 2;
                break;
            case 3:
                _npe_hash_fletcher_16_op(sum1, sum2, BYTES_0_2, xfr[0]);
                bytes_left -= 3;
                break;
            default:
                break;
            }
        }

        if ( bytes_left == 0 )
            break;
    }

    return ( sum2 << 8 | sum1 );
#else /* GCC */
    return npe_hash_fletcher_16(p, size);
#endif
}


#define _npe_hash_fletcher_32_op(s1, s2, b, d ) \
{                                               \
    if ( b == BYTES_0_3 ) {                     \
        s1 += ((d  >> 16) & 0xffff);            \
        s2 += s1;                               \
        s1 += ((d >>   0) & 0xffff);            \
        s2 += s1;                               \
    } else {                                    \
        s1 += ((d  >> 16) & 0xffff);            \
        s2 += s1;                               \
    }                                           \
    s1 = (s1 & 0xffff) + (s1 >> 16);            \
    s2 = (s2 & 0xffff) + (s2 >> 16);            \
}


__NPE_INTRINSIC uint32_t
npe_hash_fletcher_32(void *p, size_t size)
{

#ifdef __NFP_TOOL_NFCC

    __gpr uint32_t sum1 = 0xffff;
    __gpr uint32_t sum2 = 0xffff;

    /* N.B. In order to support both lmem and reg data,
       need to have constant values for size
       due to various compiler nuances. Also limiting
       max data size for similar reason. */

    ctassert(__is_ct_const(size));
    ctassert(size <= 67);
    ctassert(!(size & 1));

    if (__is_in_lmem(p)) {
        #undef _REG_OP
        #define _REG_OP(_x) _npe_hash_fletcher_32_op(sum1, sum2, BYTES_0_3, \
                                             ((__lmem uint32_t *)p)[_x])
        #undef _REG_OP_LAST
        #define _REG_OP_LAST(_x, _b) _npe_hash_fletcher_32_op(sum1, sum2,     \
                                                          BYTES_0_1,  \
                                             ((__lmem uint32_t *)p)[_x])
    } else {
        #undef _REG_OP
        #define _REG_OP(_x) _npe_hash_fletcher_32_op(sum1, sum2, BYTES_0_3, \
                                             ((__gpr uint32_t *)p)[_x])
        #undef _REG_OP_LAST
        #define _REG_OP_LAST(_x, _b) _npe_hash_fletcher_32_op(sum1, sum2,    \
                                              BYTES_0_1,                     \
                                             ((__gpr uint32_t *)p)[_x])
    }

    _MULTI_REG_OP(_REG_OP, _REG_OP_LAST, size);

#else /* GCC */

    uint32_t *curr_byte = (uint32_t *)p;
    uint32_t sum1 = 0xffff;
    uint32_t sum2 = 0xffff;
    int bytes_left = size & ~1;

    while(1) {

        uint32_t val = *curr_byte++;

        if ( bytes_left >= 4) {
            _npe_hash_fletcher_32_op(sum1, sum2, BYTES_0_3, val);
            bytes_left -= 4;
        } else if ( bytes_left >= 2 ) {
            _npe_hash_fletcher_32_op(sum1, sum2, BYTES_0_1, val);
            bytes_left -= 2;
        }

        if ( bytes_left == 0 )
            break;
    }

#endif

    return ( sum2 << 16 | sum1 );
}


uint32_t
npe_hash_fletcher_32_rem(npe_mem_ptr_t p, size_t size)
{
#ifdef __NFP_TOOL_NFCC

    __xread uint32_t xfr[8];
    int bytes_left = size & ~1;
    uint64_t curr_byte = (uint64_t)p;
    __gpr uint32_t sum1 = 0xffff;
    __gpr uint32_t sum2 = 0xffff;

    while(1) {

        if ( bytes_left >= 32) {
            mem_read64( &xfr[0], (__mem void *)curr_byte, 32);
            _npe_hash_fletcher_32_op(sum1, sum2, BYTES_0_3, xfr[0]);
            _npe_hash_fletcher_32_op(sum1, sum2, BYTES_0_3, xfr[1]);
            _npe_hash_fletcher_32_op(sum1, sum2, BYTES_0_3, xfr[2]);
            _npe_hash_fletcher_32_op(sum1, sum2, BYTES_0_3, xfr[3]);
            _npe_hash_fletcher_32_op(sum1, sum2, BYTES_0_3, xfr[4]);
            _npe_hash_fletcher_32_op(sum1, sum2, BYTES_0_3, xfr[5]);
            _npe_hash_fletcher_32_op(sum1, sum2, BYTES_0_3, xfr[6]);
            _npe_hash_fletcher_32_op(sum1, sum2, BYTES_0_3, xfr[7]);
            bytes_left -= 32;
            curr_byte += 32;
        } else if ( bytes_left >= 24) {
            mem_read64( &xfr[0], (__mem void *)curr_byte, 24);
            _npe_hash_fletcher_32_op(sum1, sum2, BYTES_0_3, xfr[0]);
            _npe_hash_fletcher_32_op(sum1, sum2, BYTES_0_3, xfr[1]);
            _npe_hash_fletcher_32_op(sum1, sum2, BYTES_0_3, xfr[2]);
            _npe_hash_fletcher_32_op(sum1, sum2, BYTES_0_3, xfr[3]);
            _npe_hash_fletcher_32_op(sum1, sum2, BYTES_0_3, xfr[4]);
            _npe_hash_fletcher_32_op(sum1, sum2, BYTES_0_3, xfr[5]);
            bytes_left -= 24;
            curr_byte += 24;
        } else if ( bytes_left >= 16) {
            mem_read64( &xfr[0], (__mem void *)curr_byte, 16);
            _npe_hash_fletcher_32_op(sum1, sum2, BYTES_0_3, xfr[0]);
            _npe_hash_fletcher_32_op(sum1, sum2, BYTES_0_3, xfr[1]);
            _npe_hash_fletcher_32_op(sum1, sum2, BYTES_0_3, xfr[2]);
            _npe_hash_fletcher_32_op(sum1, sum2, BYTES_0_3, xfr[3]);
            bytes_left -= 16;
            curr_byte += 16;
        } else if ( bytes_left >= 8) {
            mem_read64( &xfr[0], (__mem void *)curr_byte, 8);
            _npe_hash_fletcher_32_op(sum1, sum2, BYTES_0_3, xfr[0]);
            _npe_hash_fletcher_32_op(sum1, sum2, BYTES_0_3, xfr[1]);
            bytes_left -= 8;
            curr_byte += 8;
        } else if ( bytes_left >= 4) {
            mem_read32( &xfr[0], (__mem void *)curr_byte, 4);
            _npe_hash_fletcher_32_op(sum1, sum2, BYTES_0_3, xfr[0]);
            bytes_left -= 4;
            curr_byte += 4;
        } else if (bytes_left >= 2) {
            mem_read8( &xfr[0], (__mem void *)curr_byte, 2);
            _npe_hash_fletcher_32_op(sum1, sum2, BYTES_0_1, xfr[0]);
            bytes_left -= 2;
            curr_byte += 2;
        }

        if ( bytes_left == 0 )
            break;
    }

    return ( sum2 << 16 | sum1 );
#else
    return npe_hash_fletcher_32(p, size);
#endif
}


#define _npe_hash_jenkins_8_op(hash, b, d) \
{                                       \
    if (b == BYTES_0_3)  {              \
        hash += d >> 24 & 0xff;         \
        hash += hash << 10;             \
        hash ^= hash >> 6;              \
        hash += d >> 16 & 0xff;         \
        hash += hash << 10;             \
        hash ^= hash >> 6;              \
        hash += d >>  8 & 0xff;         \
        hash += hash << 10;             \
        hash ^= hash >> 6;              \
        hash += d >>  0 & 0xff;         \
        hash += hash << 10;             \
        hash ^= hash >> 6;              \
    } else if ( b == BYTES_0_2 ) {      \
        hash += d >> 24 & 0xff;         \
        hash += hash << 10;             \
        hash ^= hash >> 6;              \
        hash += d >> 16 & 0xff;         \
        hash += hash << 10;             \
        hash ^= hash >> 6;              \
        hash += d >>  8 & 0xff;         \
        hash += hash << 10;             \
        hash ^= hash >> 6;              \
    } else if ( b == BYTES_0_1 ) {      \
        hash += d >> 24 & 0xff;         \
        hash += hash << 10;             \
        hash ^= hash >> 6;              \
        hash += d >> 16 & 0xff;         \
        hash += hash << 10;             \
        hash ^= hash >> 6;              \
    } else if ( b == BYTE_0 ) {         \
        hash += d >> 24 & 0xff;         \
        hash += hash << 10;             \
        hash ^= hash >> 6;              \
    }                                   \
}


__NPE_INTRINSIC uint32_t
npe_hash_jenkins_8(void *p, size_t size)
{
#ifdef __NFP_TOOL_NFCC

    __gpr uint32_t hash = 0;

    /* N.B. In order to support both lmem and reg data,
       need to have constant values for size
       due to various compiler nuances. Also limiting
       max data size for similar reason. */

    ctassert(__is_ct_const(size));
    ctassert(size <= 67);

    if (__is_in_lmem(p)) {
        #undef _REG_OP
        #define _REG_OP(_x) _npe_hash_jenkins_8_op(hash, BYTES_0_3, \
                                             ((__lmem uint32_t *)p)[_x])
        #undef _REG_OP_LAST
        #define _REG_OP_LAST(_x,_b) _npe_hash_jenkins_8_op(hash, _b, \
                                             ((__lmem uint32_t *)p)[_x])
    } else {
        #undef _REG_OP
        #define _REG_OP(_x) _npe_hash_jenkins_8_op(hash, BYTES_0_3, \
                                             ((__gpr uint32_t *)p)[_x])
        #undef _REG_OP_LAST
        #define _REG_OP_LAST(_x,_b) _npe_hash_jenkins_8_op(hash, _b, \
                                             ((__gpr uint32_t *)p)[_x])
    }

    _MULTI_REG_OP( _REG_OP, _REG_OP_LAST, size );

#else /* GCC */

    uint32_t *curr_byte = (uint32_t *)p;
    uint32_t hash = 0;
    int bytes_left = size;

    while(1) {

        uint32_t val = *curr_byte++;

        if ( bytes_left >= 4) {
            _npe_hash_jenkins_8_op(hash, BYTES_0_3, val);
            bytes_left -= 4;
        } else {
            switch ( bytes_left ) {
            case 1:
                _npe_hash_jenkins_8_op(hash, BYTE_0, val);
                bytes_left -= 1;
                break;
            case 2:
                _npe_hash_jenkins_8_op(hash, BYTES_0_1, val);
                bytes_left -= 2;
                break;
            case 3:
                _npe_hash_jenkins_8_op(hash, BYTES_0_2, val);
                bytes_left -= 3;
                break;
            default:
                break;
            }
        }

        if ( bytes_left == 0 )
            break;
    }

    #endif

    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;

    return hash;
 }


uint32_t
npe_hash_jenkins_8_rem(npe_mem_ptr_t p, size_t size)
{
#ifdef __NFP_TOOL_NFCC

    __xread uint32_t xfr[8];
    int bytes_left = size;
    uint64_t curr_byte = (uint64_t)p;
    __gpr uint32_t hash = 0;

    while(1) {

        if ( bytes_left >= 32) {
            mem_read64( &xfr[0], (__mem void *)curr_byte, 32);
            _npe_hash_jenkins_8_op(hash, BYTES_0_3, xfr[0]);
            _npe_hash_jenkins_8_op(hash, BYTES_0_3, xfr[1]);
            _npe_hash_jenkins_8_op(hash, BYTES_0_3, xfr[2]);
            _npe_hash_jenkins_8_op(hash, BYTES_0_3, xfr[3]);
            _npe_hash_jenkins_8_op(hash, BYTES_0_3, xfr[4]);
            _npe_hash_jenkins_8_op(hash, BYTES_0_3, xfr[5]);
            _npe_hash_jenkins_8_op(hash, BYTES_0_3, xfr[6]);
            _npe_hash_jenkins_8_op(hash, BYTES_0_3, xfr[7]);
            bytes_left -= 32;
            curr_byte += 32;
        } else if ( bytes_left >= 24) {
            mem_read64( &xfr[0], (__mem void *)curr_byte, 24);
            _npe_hash_jenkins_8_op(hash, BYTES_0_3, xfr[0]);
            _npe_hash_jenkins_8_op(hash, BYTES_0_3, xfr[1]);
            _npe_hash_jenkins_8_op(hash, BYTES_0_3, xfr[2]);
            _npe_hash_jenkins_8_op(hash, BYTES_0_3, xfr[3]);
            _npe_hash_jenkins_8_op(hash, BYTES_0_3, xfr[4]);
            _npe_hash_jenkins_8_op(hash, BYTES_0_3, xfr[5]);
            bytes_left -= 24;
            curr_byte += 24;
        } else if ( bytes_left >= 16) {
            mem_read64( &xfr[0], (__mem void *)curr_byte, 16);
            _npe_hash_jenkins_8_op(hash, BYTES_0_3, xfr[0]);
            _npe_hash_jenkins_8_op(hash, BYTES_0_3, xfr[1]);
            _npe_hash_jenkins_8_op(hash, BYTES_0_3, xfr[2]);
            _npe_hash_jenkins_8_op(hash, BYTES_0_3, xfr[3]);
            bytes_left -= 16;
            curr_byte += 16;
        } else if ( bytes_left >= 8) {
            mem_read64( &xfr[0], (__mem void *)curr_byte, 8);
            _npe_hash_jenkins_8_op(hash, BYTES_0_3, xfr[0]);
            _npe_hash_jenkins_8_op(hash, BYTES_0_3, xfr[1]);
            bytes_left -= 8;
            curr_byte += 8;
        } else if ( bytes_left >= 4) {
            mem_read32( &xfr[0], (__mem void *)curr_byte, 4);
            _npe_hash_jenkins_8_op(hash, BYTES_0_3, xfr[0]);
            bytes_left -= 4;
            curr_byte += 4;
        } else {
            mem_read8( &xfr[0], (__mem void *)curr_byte, bytes_left);
            switch ( bytes_left ) {
            case 1:
                _npe_hash_jenkins_8_op(hash, BYTE_0, xfr[0]);
                bytes_left -= 1;
                break;
            case 2:
                _npe_hash_jenkins_8_op(hash, BYTES_0_1, xfr[0]);
                bytes_left -= 2;
                break;
            case 3:
                _npe_hash_jenkins_8_op(hash, BYTES_0_2, xfr[0]);
                bytes_left -= 3;
                break;
            default:
                break;
            }
        }

        if ( bytes_left == 0 )
            break;
    }

    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;

    return ( hash );
#else
    return npe_hash_jenkins_8(p, size);
#endif
}


#define _npe_hash_jenkins_32_op(hash, b, d) \
{                                           \
    hash += d;                              \
    hash += hash << 10;                     \
    hash ^= hash >> 6;                      \
}


__NPE_INTRINSIC uint32_t
npe_hash_jenkins_32(void *p, size_t size)
{
#ifdef __NFP_TOOL_NFCC

    __gpr uint32_t hash = 0;

    /* N.B. In order to support both lmem and reg data,
       need to have constant values for size
       due to various compiler nuances. Also limiting
       max data size for similar reason. */

    ctassert(__is_ct_const(size));
    ctassert(size <= 67);
    ctassert(!(size & 3));

    if (__is_in_lmem(p)) {
        #undef _REG_OP
        #define _REG_OP(_x) _npe_hash_jenkins_32_op(hash, BYTES_0_3, \
                                             ((__lmem uint32_t *)p)[_x])
        #undef _REG_OP_LAST
        #define _REG_OP_LAST(_x,_b) _npe_hash_jenkins_32_op(hash, _b, \
                                             ((__lmem uint32_t *)p)[_x])
    } else {
        #undef _REG_OP
        #define _REG_OP(_x) _npe_hash_jenkins_32_op(hash, BYTES_0_3, \
                                             ((__gpr uint32_t *)p)[_x])
        #undef _REG_OP_LAST
        #define _REG_OP_LAST(_x,_b) _npe_hash_jenkins_32_op(hash, _b, \
                                             ((__gpr uint32_t *)p)[_x])
    }

    _MULTI_REG_OP( _REG_OP, _REG_OP_LAST, size );

#else /* GCC */

    uint32_t *curr_byte = (uint32_t *)p;
    uint32_t hash = 0;
    int bytes_left = size & ~3;

    while(1) {

        uint32_t val = *curr_byte++;

        _npe_hash_jenkins_32_op(hash, BYTES_0_3, val);

        bytes_left -= 4;

        if ( bytes_left == 0 )
            break;
    }

#endif

    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;

    return hash;
}


uint32_t
npe_hash_jenkins_32_rem(npe_mem_ptr_t p, size_t size)
{
#ifdef __NFP_TOOL_NFCC

    __xread uint32_t xfr[8];
    int bytes_left = size & ~3;
    uint64_t curr_byte = (uint64_t)p;
    __gpr uint32_t hash = 0;

    while(1) {

        if ( bytes_left >= 32) {
            mem_read64( &xfr[0], (__mem void *)curr_byte, 32);
            _npe_hash_jenkins_32_op(hash, BYTES_0_3, xfr[0]);
            _npe_hash_jenkins_32_op(hash, BYTES_0_3, xfr[1]);
            _npe_hash_jenkins_32_op(hash, BYTES_0_3, xfr[2]);
            _npe_hash_jenkins_32_op(hash, BYTES_0_3, xfr[3]);
            _npe_hash_jenkins_32_op(hash, BYTES_0_3, xfr[4]);
            _npe_hash_jenkins_32_op(hash, BYTES_0_3, xfr[5]);
            _npe_hash_jenkins_32_op(hash, BYTES_0_3, xfr[6]);
            _npe_hash_jenkins_32_op(hash, BYTES_0_3, xfr[7]);
            bytes_left -= 32;
            curr_byte += 32;
        } else if ( bytes_left >= 24) {
            mem_read64( &xfr[0], (__mem void *)curr_byte, 24);
            _npe_hash_jenkins_32_op(hash, BYTES_0_3, xfr[0]);
            _npe_hash_jenkins_32_op(hash, BYTES_0_3, xfr[1]);
            _npe_hash_jenkins_32_op(hash, BYTES_0_3, xfr[2]);
            _npe_hash_jenkins_32_op(hash, BYTES_0_3, xfr[3]);
            _npe_hash_jenkins_32_op(hash, BYTES_0_3, xfr[4]);
            _npe_hash_jenkins_32_op(hash, BYTES_0_3, xfr[5]);
            bytes_left -= 24;
            curr_byte += 24;
        } else if ( bytes_left >= 16) {
            mem_read64( &xfr[0], (__mem void *)curr_byte, 16);
            _npe_hash_jenkins_32_op(hash, BYTES_0_3, xfr[0]);
            _npe_hash_jenkins_32_op(hash, BYTES_0_3, xfr[1]);
            _npe_hash_jenkins_32_op(hash, BYTES_0_3, xfr[2]);
            _npe_hash_jenkins_32_op(hash, BYTES_0_3, xfr[3]);
            bytes_left -= 16;
            curr_byte += 16;
        } else if ( bytes_left >= 8) {
            mem_read64( &xfr[0], (__mem void *)curr_byte, 8);
            _npe_hash_jenkins_32_op(hash, BYTES_0_3, xfr[0]);
            _npe_hash_jenkins_32_op(hash, BYTES_0_3, xfr[1]);
            bytes_left -= 8;
            curr_byte += 8;
        } else {
            mem_read32( &xfr[0], (__mem void *)curr_byte, 4);
            _npe_hash_jenkins_32_op(hash, BYTES_0_3, xfr[0]);
            bytes_left -= 4;
            curr_byte += 4;
        }

        if ( bytes_left == 0 )
            break;
    }

    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;

    return ( hash );
#else
    return npe_hash_jenkins_32(p, size);
#endif
}
