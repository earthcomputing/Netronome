/*
 * Copyright (C) 2014 Netronome Systems, Inc. All rights reserved.
 */

#ifndef __CRYPTO_LIB_IPSEC_UC__
#define __CRYPTO_LIB_IPSEC_UC__

#ifndef NFP_LIB_ANY_NFAS_VERSION
    #if (!defined(__NFP_TOOL_NFAS) ||                       \
        (__NFP_TOOL_NFAS < NFP_SW_VERSION(5, 0, 0, 0)) ||   \
        (__NFP_TOOL_NFAS > NFP_SW_VERSION(6, 0, 1, 255)))
        #error "This standard library is not supported for the version of \
                  the SDK currently in use."
    #endif
#endif

/** @file crypto_lib_ipsec.uc CRYPTO IPSec Operation Macros
 * @addtogroup cryptoipsec CRYPTO IPSec Operation
 * @{
 *
 * @name CRYPTO IPSec Operation Macros
 * @{
 *
 * This file contains a set of crypto-library sequences.  The sequences are
 * designed to be compatible with Netronome's crypto_support facility. They are
 * implemented as 'compressed' sequences, which can be preloaded to the CIB
 * memory space of the bulk core units at initialization time, and then invoked
 * at run-time on a per-packet basis quickly and efficiently.
 *
 * These sequences implement encryption, decryption, and authentication that is
 * intended to be usable on IPSec-formatted packets encapsulated with ESP or AH.
 * The intention is for the ME to issue a single get_core() at initialization
 * time for a specific Crypto Dispatcher context, then keep that core 'pinned' to
 * that context for the life of the program. The selection of which bulk core
 * unit works on a particular packet is thus pushed to earlier in the process,
 * when the packet is assigned to an ME. This technique has the advantage that it
 * avoids the processing delay associated with executing the get_core() and
 * free_core() instructions for every packet. It also eliminates some of the
 * variables that need to be passed to the CIB code sequences, because each core
 * can use a fixed set of pre-allocated buffers that are statically assigned to
 * the ME's.
 *
 * The source file for these sequences is crypto_lib_ipsec.crypt  The source file
 * is processed by the Netronome utility ca2.py to generate the file
 * crypto_lib_ipsec.uc, which is included in the microcode. The macros defined in
 * crypto_lib_ipsec.uc are used by the macros in crypto_lib.uc
 */
/*
// API Quick Reference:
//
// crypto_load_ipsec_enc_pin_A (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
// crypto_gen_ipsec_enc_pin_A (core, desc)
// crypto_load_ipsec_enc_pin_B (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
// crypto_gen_ipsec_enc_pin_B (core, desc)
// crypto_load_ipsec_enc_pin_C (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
// crypto_gen_ipsec_enc_pin_C (core, desc)
// crypto_load_ipsec_enc_pin_D (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
// crypto_gen_ipsec_enc_pin_D (core, desc)
// crypto_load_ipsec_dec_pin_A (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
// crypto_gen_ipsec_dec_pin_A (core, desc)
// crypto_load_ipsec_dec_pin_B (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
// crypto_gen_ipsec_dec_pin_B (core, desc)
// crypto_load_ipsec_dec_pin_C (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
// crypto_gen_ipsec_dec_pin_C (core, desc)
// crypto_load_ipsec_dec_pin_D (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
// crypto_gen_ipsec_dec_pin_D (core, desc)
// crypto_load_ipsec_enc_strt_pin_A (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
// crypto_gen_ipsec_enc_strt_pin_A (core, desc)
// crypto_load_ipsec_enc_strt_pin_B (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
// crypto_gen_ipsec_enc_strt_pin_B (core, desc)
// crypto_load_ipsec_enc_strt_pin_C (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
// crypto_gen_ipsec_enc_strt_pin_C (core, desc)
// crypto_load_ipsec_enc_strt_pin_D (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
// crypto_gen_ipsec_enc_strt_pin_D (core, desc)
// crypto_load_ipsec_enc_cont (cr_xfr, cr_ctx, packet, in_len, dummy1, dummy2, dummy3)
// crypto_gen_ipsec_enc_cont (core, desc)
// crypto_load_ipsec_enc_end (cr_xfr, cr_ctx, packet, in_len, hmac_resadr, hmac_keyadr, hmac_keylen)
// crypto_gen_ipsec_enc_end (core, desc)
// crypto_load_ipsec_dec_strt_pin_A (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
// crypto_gen_ipsec_dec_strt_pin_A (core, desc)
// crypto_load_ipsec_dec_strt_pin_B (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
// crypto_gen_ipsec_dec_strt_pin_B (core, desc)
// crypto_load_ipsec_dec_strt_pin_C (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
// crypto_gen_ipsec_dec_strt_pin_C (core, desc)
// crypto_load_ipsec_dec_strt_pin_D (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
// crypto_gen_ipsec_dec_strt_pin_D (core, desc)
// crypto_load_ipsec_dec_strt_nw_pin_A (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
// crypto_gen_ipsec_dec_strt_nw_pin_A (core, desc)
// crypto_load_ipsec_dec_strt_nw_pin_B (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
// crypto_gen_ipsec_dec_strt_nw_pin_B (core, desc)
// crypto_load_ipsec_dec_strt_nw_pin_C (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
// crypto_gen_ipsec_dec_strt_nw_pin_C (core, desc)
// crypto_load_ipsec_dec_strt_nw_pin_D (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
// crypto_gen_ipsec_dec_strt_nw_pin_D (core, desc)
// crypto_load_ipsec_dec_cont (cr_xfr, cr_ctx, packet, in_len, dummy1, dummy2, dummy3)
// crypto_gen_ipsec_dec_cont (core, desc)
// crypto_load_ipsec_dec_cont_nw (cr_xfr, cr_ctx, packet, in_len, dummy1, dummy2, dummy3)
// crypto_gen_ipsec_dec_cont_nw (core, desc)
// crypto_load_ipsec_dec_end (cr_xfr, cr_ctx, packet, in_len, hmac_resadr, hmac_keyadr, hmac_keylen)
// crypto_gen_ipsec_dec_end (core, desc)
// crypto_gen_compr_constants (base0, base1, base2, base3, base4)
*/
/**
 * ipsec_enc_pin_A, ipsec_enc_pin_B, ipsec_enc_pin_C, ipsec_enc_pin_D
 *
 * Sequences to do IPSec-compatible encryption of a packet, using bulk cores that
 * stay 'pinned'to a Dispatcher context.  This allows operation without per-packet
 * get_core()/free_core() ops, and also allows preloaded constants to point to
 * fixed addresses within the SA, so that SA-resident parameters do not need to
 * be passed as variables.
 *
 * Supplied in 4 different versions (_A, _B, _C, _D suffix) to handle up to 4
 * buffer sets per core. One context will do a get_core() at init time and will
 * hold onto that core for the life of the program.  As each packet arrives at the
 * ME, it will assign it to a buffer set and then use its Dispatcher context/core
 * to call the appropriate buffer-set-specific sequence.
 *
 * For this set of CIB sequences, we also assume that the 'output' (ciphertext)
 * data is at the same crypto SRAM address as the 'input' data.  This fits
 * architecturally with the idea of having several buffer sets per core, and using
 * ME threads to do the scheduling. It is also important from a performance
 * perspective, because it keeps the number of parameters down to 8, which can
 * all be transferred to CIB in one cycle, which comes for 'free' as part of the
 * setup_load_CIB() opcode that runs the compressed routine.
 *
 * Supports AES, DES, and NO encryption with MD5 or SHA-x HMAC auth.
 *
 * Calling the sequence:
 *
 * crypto_load_ipsec_enc_pin_X (packet, in_len, auth_only_data, iv, auth_length,
 *        hmac_keylen, crypt_select, crypt_modes)
 *
 * @param packet                crypto SRAM address of the start of plaintext to
 *                              be encrypted
 * @param in_len                length of data to be encrypted
 * @param auth_only_data        crypto SRAM address of auth-only data (SPI and
 *                              seq number)
 * @param iv                    crypto SRAM address of the Initialization Vector
 * @param auth_length           byte length of (SPI/Seq
 * @param hmac_keylen           byte length of HMAC key, minus 1 (15 for MD5, 19
 *                              for SHA1, etc)
 * @param crypt_select          config word 1, produced by crypto_setup_configs
 * @param crypt_modes           config word 2, produced by crypto_setup_configs
 **/

/**
 *  Prepare transfer regs to load static (aka 'pinned') encryption sequence
 *
 */
#macro crypto_load_ipsec_enc_pin_A (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
    crypto_build_xfer_reg (cr_xfr[0], packet, in_len)
    crypto_build_xfer_reg (cr_xfr[1], auth_only_data, iv)
    crypto_build_xfer_reg (cr_xfr[2], auth_length, hmac_keylen)
    crypto_build_xfer_reg (cr_xfr[3], crypt_select, crypt_modes)
    #define_eval _crypto_transfer_length 2
#endm


/**
 * Generate cmd sequence as constant data
 *
 *  @param core  crypto bulk core, 0 - 3 for NFP3XXX or 0 - 5 for NFP6XXX
 *  @param desc  sequence 'descriptor', contains sram location address
 *
 *  used in _crypto_library_load_dynamic in crypto_lib.uc
 */
#macro crypto_gen_ipsec_enc_pin_A (core, desc)
    #if core==5
        #define_eval ipsec_enc_pin_A_desc_5 (desc)
    #elif core==4
        #define_eval ipsec_enc_pin_A_desc_4 (desc)
    #elif core==3
        #define_eval ipsec_enc_pin_A_desc_3 (desc)
    #elif core==2
        #define_eval ipsec_enc_pin_A_desc_2 (desc)
    #elif core==1
        #define_eval ipsec_enc_pin_A_desc_1 (desc)
    #elif core==0
        #define_eval ipsec_enc_pin_A_desc_0 (desc)
    #endif
    #define_eval gen_data_0 0
    #define_eval gen_data_1 0
    #define_eval gen_data_2 0
    #define_eval gen_data_3 0
    #define_eval gen_data_4 0x08705e5f
    #define_eval gen_data_5 0x0a13000f
    #define_eval gen_data_6 0x08380c58
    #define_eval gen_data_7 0x01905d0e
    #define_eval gen_data_8 0x01005c5a
    #define_eval gen_data_9 0x0a235c5b
    #define_eval gen_data_10 0x03735958
    #define_eval gen_data_11 0x08280d01
    #define_eval gen_data_12 0x01d05d0e
    #define_eval gen_data_13 0x11725d0c
    #define_eval gen_data_14 0
    #define_eval gen_data_15 0
    #define_eval _cib_xfer_size_ipsec_enc_pin_A 16
    #define_eval _cib_code_size_ipsec_enc_pin_A 10
    #define_eval _cib_vars_base_ipsec_enc_pin_A 20
    #define_eval _cib_vars_size_ipsec_enc_pin_A 4
    #define_eval _cib_vars_xfer_ipsec_enc_pin_A 4
#endm


/**
 *  Prepare transfer regs to load static (aka 'pinned') encryption sequence
 *
 */
#macro crypto_load_ipsec_enc_pin_B (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
    crypto_build_xfer_reg (cr_xfr[0], packet, in_len)
    crypto_build_xfer_reg (cr_xfr[1], auth_only_data, iv)
    crypto_build_xfer_reg (cr_xfr[2], auth_length, hmac_keylen)
    crypto_build_xfer_reg (cr_xfr[3], crypt_select, crypt_modes)
    #define_eval _crypto_transfer_length 2
#endm


/**
 * Generate cmd sequence as constant data
 *
 *  @param core  crypto bulk core, 0 - 3 for NFP3XXX or 0 - 5 for NFP6XXX
 *  @param desc  sequence 'descriptor', contains sram location address
 *
 *  used in _crypto_library_load_dynamic in crypto_lib.uc
 */
#macro crypto_gen_ipsec_enc_pin_B (core, desc)
    #if core==5
        #define_eval ipsec_enc_pin_B_desc_5 (desc)
    #elif core==4
        #define_eval ipsec_enc_pin_B_desc_4 (desc)
    #elif core==3
        #define_eval ipsec_enc_pin_B_desc_3 (desc)
    #elif core==2
        #define_eval ipsec_enc_pin_B_desc_2 (desc)
    #elif core==1
        #define_eval ipsec_enc_pin_B_desc_1 (desc)
    #elif core==0
        #define_eval ipsec_enc_pin_B_desc_0 (desc)
    #endif
    #define_eval gen_data_0 0
    #define_eval gen_data_1 0
    #define_eval gen_data_2 0
    #define_eval gen_data_3 0
    #define_eval gen_data_4 0x08705e5f
    #define_eval gen_data_5 0x0a13001a
    #define_eval gen_data_6 0x08380c58
    #define_eval gen_data_7 0x01905d19
    #define_eval gen_data_8 0x01005c5a
    #define_eval gen_data_9 0x0a235c5b
    #define_eval gen_data_10 0x03735958
    #define_eval gen_data_11 0x08281801
    #define_eval gen_data_12 0x01d05d19
    #define_eval gen_data_13 0x11725d0c
    #define_eval gen_data_14 0
    #define_eval gen_data_15 0
    #define_eval _cib_xfer_size_ipsec_enc_pin_B 16
    #define_eval _cib_code_size_ipsec_enc_pin_B 10
    #define_eval _cib_vars_base_ipsec_enc_pin_B 20
    #define_eval _cib_vars_size_ipsec_enc_pin_B 4
    #define_eval _cib_vars_xfer_ipsec_enc_pin_B 4
#endm


/**
 *  Prepare transfer regs to load static (aka 'pinned') encryption sequence
 *
 */
#macro crypto_load_ipsec_enc_pin_C (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
    crypto_build_xfer_reg (cr_xfr[0], packet, in_len)
    crypto_build_xfer_reg (cr_xfr[1], auth_only_data, iv)
    crypto_build_xfer_reg (cr_xfr[2], auth_length, hmac_keylen)
    crypto_build_xfer_reg (cr_xfr[3], crypt_select, crypt_modes)
    #define_eval _crypto_transfer_length 2
#endm


/**
 * Generate cmd sequence as constant data
 *
 *  @param core  crypto bulk core, 0 - 3 for NFP3XXX or 0 - 5 for NFP6XXX
 *  @param desc  sequence 'descriptor', contains sram location address
 *
 *  used in _crypto_library_load_dynamic in crypto_lib.uc
 */
#macro crypto_gen_ipsec_enc_pin_C (core, desc)
    #if core==5
        #define_eval ipsec_enc_pin_C_desc_5 (desc)
    #elif core==4
        #define_eval ipsec_enc_pin_C_desc_4 (desc)
    #elif core==3
        #define_eval ipsec_enc_pin_C_desc_3 (desc)
    #elif core==2
        #define_eval ipsec_enc_pin_C_desc_2 (desc)
    #elif core==1
        #define_eval ipsec_enc_pin_C_desc_1 (desc)
    #elif core==0
        #define_eval ipsec_enc_pin_C_desc_0 (desc)
    #endif
    #define_eval gen_data_0 0
    #define_eval gen_data_1 0
    #define_eval gen_data_2 0
    #define_eval gen_data_3 0
    #define_eval gen_data_4 0x08705e5f
    #define_eval gen_data_5 0x0a130029
    #define_eval gen_data_6 0x08380c58
    #define_eval gen_data_7 0x01905d28
    #define_eval gen_data_8 0x01005c5a
    #define_eval gen_data_9 0x0a235c5b
    #define_eval gen_data_10 0x03735958
    #define_eval gen_data_11 0x08281b01
    #define_eval gen_data_12 0x01d05d28
    #define_eval gen_data_13 0x11725d0c
    #define_eval gen_data_14 0
    #define_eval gen_data_15 0
    #define_eval _cib_xfer_size_ipsec_enc_pin_C 16
    #define_eval _cib_code_size_ipsec_enc_pin_C 10
    #define_eval _cib_vars_base_ipsec_enc_pin_C 20
    #define_eval _cib_vars_size_ipsec_enc_pin_C 4
    #define_eval _cib_vars_xfer_ipsec_enc_pin_C 4
#endm


/**
 *  Prepare transfer regs to load static (aka 'pinned') encryption sequence
 *
 */
#macro crypto_load_ipsec_enc_pin_D (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
    crypto_build_xfer_reg (cr_xfr[0], packet, in_len)
    crypto_build_xfer_reg (cr_xfr[1], auth_only_data, iv)
    crypto_build_xfer_reg (cr_xfr[2], auth_length, hmac_keylen)
    crypto_build_xfer_reg (cr_xfr[3], crypt_select, crypt_modes)
    #define_eval _crypto_transfer_length 2
#endm


/**
 * Generate cmd sequence as constant data
 *
 *  @param core  crypto bulk core, 0 - 3 for NFP3XXX or 0 - 5 for NFP6XXX
 *  @param desc  sequence 'descriptor', contains sram location address
 *
 *  used in _crypto_library_load_dynamic in crypto_lib.uc
 */
#macro crypto_gen_ipsec_enc_pin_D (core, desc)
    #if core==5
        #define_eval ipsec_enc_pin_D_desc_5 (desc)
    #elif core==4
        #define_eval ipsec_enc_pin_D_desc_4 (desc)
    #elif core==3
        #define_eval ipsec_enc_pin_D_desc_3 (desc)
    #elif core==2
        #define_eval ipsec_enc_pin_D_desc_2 (desc)
    #elif core==1
        #define_eval ipsec_enc_pin_D_desc_1 (desc)
    #elif core==0
        #define_eval ipsec_enc_pin_D_desc_0 (desc)
    #endif
    #define_eval gen_data_0 0
    #define_eval gen_data_1 0
    #define_eval gen_data_2 0
    #define_eval gen_data_3 0
    #define_eval gen_data_4 0x08705e5f
    #define_eval gen_data_5 0x0a130038
    #define_eval gen_data_6 0x08380c58
    #define_eval gen_data_7 0x01905d2b
    #define_eval gen_data_8 0x01005c5a
    #define_eval gen_data_9 0x0a235c5b
    #define_eval gen_data_10 0x03735958
    #define_eval gen_data_11 0x08282a01
    #define_eval gen_data_12 0x01d05d2b
    #define_eval gen_data_13 0x11725d0c
    #define_eval gen_data_14 0
    #define_eval gen_data_15 0
    #define_eval _cib_xfer_size_ipsec_enc_pin_D 16
    #define_eval _cib_code_size_ipsec_enc_pin_D 10
    #define_eval _cib_vars_base_ipsec_enc_pin_D 20
    #define_eval _cib_vars_size_ipsec_enc_pin_D 4
    #define_eval _cib_vars_xfer_ipsec_enc_pin_D 4
#endm

/**
 * ipsec_dec_pin_A, ipsec_dec_pin_B, ipsec_dec_pin_C, ipsec_dec_pin_D
 *
 * Sequences to do IPSec-compatible decryption of a packet, using bulk cores that
 * stay 'pinned'to a Dispatcher context.  This allows operation without per-packet
 * get_core()/free_core() ops, and also allows preloaded constants to point to
 * fixed addresses within the SA, so that SA-resident parameters do not need to
 * be passed as variables.
 *
 * Supplied in 4 different versions (_A, _B, _C, _D suffix) to handle up to 4
 * buffer sets per core. One context will do a get_core() at init time and will
 * hold onto that core for the life of the program.  As each packet arrives at the
 * ME, it will assign it to a buffer set and then use its Dispatcher context/core
 * to call the appropriate buffer-set-specific sequence.
 *
 * For this set of CIB sequences, we also assume that the 'output' (plaintext)
 * data is at the same crypto SRAM address as the 'input' data.  This fits
 * architecturally with the idea of having several buffer sets per core, and using
 * ME threads to do the scheduling. It is also important from a performance
 * perspective, because it keeps the number of parameters down to 8, which can
 * all be transferred to CIB in one cycle, which comes for 'free' as part of the
 * setup_load_CIB() opcode that runs the compressed routine.
 *
 * Supports AES, DES, and NO encryption with MD5 or SHA-x HMAC auth.
 *
 * Calling the sequence:
 *
 * crypto_load_ipsec_dec_pin_X (packet, in_len, auth_only_data, iv, auth_length,
 *        hmac_keylen, crypt_select, crypt_modes)
 *
 * @param packet                crypto SRAM address of the start of ciphertext to
 *                              be decrypted
 * @param in_len                length of data to be decrypted
 * @param auth_only_data        crypto SRAM address of auth-only data (SPI and
 *                              seq number)
 * @param iv                    crypto SRAM address of the Initialization Vector
 * @param auth_length           byte length of (SPI/Seq
 * @param hmac_keylen           byte length of HMAC key, minus 1 (15 for MD5, 19
 *                              for SHA1, etc)
 * @param crypt_select          config word 1, produced by crypto_setup_configs
 * @param crypt_modes           config word 2, produced by crypto_setup_configs
 **/

/**
 *  Prepare transfer regs to load static (aka 'pinned') encryption sequence
 *
 */
#macro crypto_load_ipsec_dec_pin_A (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
    crypto_build_xfer_reg (cr_xfr[0], packet, in_len)
    crypto_build_xfer_reg (cr_xfr[1], auth_only_data, iv)
    crypto_build_xfer_reg (cr_xfr[2], auth_length, hmac_keylen)
    crypto_build_xfer_reg (cr_xfr[3], crypt_select, crypt_modes)
    #define_eval _crypto_transfer_length 2
#endm


/**
 * Generate cmd sequence as constant data
 *
 *  @param core  crypto bulk core, 0 - 3 for NFP3XXX or 0 - 5 for NFP6XXX
 *  @param desc  sequence 'descriptor', contains sram location address
 *
 *  used in _crypto_library_load_dynamic in crypto_lib.uc
 */
#macro crypto_gen_ipsec_dec_pin_A (core, desc)
    #if core==5
        #define_eval ipsec_dec_pin_A_desc_5 (desc)
    #elif core==4
        #define_eval ipsec_dec_pin_A_desc_4 (desc)
    #elif core==3
        #define_eval ipsec_dec_pin_A_desc_3 (desc)
    #elif core==2
        #define_eval ipsec_dec_pin_A_desc_2 (desc)
    #elif core==1
        #define_eval ipsec_dec_pin_A_desc_1 (desc)
    #elif core==0
        #define_eval ipsec_dec_pin_A_desc_0 (desc)
    #endif
    #define_eval gen_data_0 0
    #define_eval gen_data_1 0
    #define_eval gen_data_2 0
    #define_eval gen_data_3 0
    #define_eval gen_data_4 0x08705e5f
    #define_eval gen_data_5 0x0a13000f
    #define_eval gen_data_6 0x08380c58
    #define_eval gen_data_7 0x01905d0e
    #define_eval gen_data_8 0x01005c5a
    #define_eval gen_data_9 0x0a235c5b
    #define_eval gen_data_10 0x00735958
    #define_eval gen_data_11 0x08280d01
    #define_eval gen_data_12 0x01d05d0e
    #define_eval gen_data_13 0x11725d0c
    #define_eval gen_data_14 0
    #define_eval gen_data_15 0
    #define_eval _cib_xfer_size_ipsec_dec_pin_A 16
    #define_eval _cib_code_size_ipsec_dec_pin_A 10
    #define_eval _cib_vars_base_ipsec_dec_pin_A 20
    #define_eval _cib_vars_size_ipsec_dec_pin_A 4
    #define_eval _cib_vars_xfer_ipsec_dec_pin_A 4
#endm


/**
 *  Prepare transfer regs to load static (aka 'pinned') encryption sequence
 *
 */
#macro crypto_load_ipsec_dec_pin_B (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
    crypto_build_xfer_reg (cr_xfr[0], packet, in_len)
    crypto_build_xfer_reg (cr_xfr[1], auth_only_data, iv)
    crypto_build_xfer_reg (cr_xfr[2], auth_length, hmac_keylen)
    crypto_build_xfer_reg (cr_xfr[3], crypt_select, crypt_modes)
    #define_eval _crypto_transfer_length 2
#endm


/**
 * Generate cmd sequence as constant data
 *
 *  @param core  crypto bulk core, 0 - 3 for NFP3XXX or 0 - 5 for NFP6XXX
 *  @param desc  sequence 'descriptor', contains sram location address
 *
 *  used in _crypto_library_load_dynamic in crypto_lib.uc
 */
#macro crypto_gen_ipsec_dec_pin_B (core, desc)
    #if core==5
        #define_eval ipsec_dec_pin_B_desc_5 (desc)
    #elif core==4
        #define_eval ipsec_dec_pin_B_desc_4 (desc)
    #elif core==3
        #define_eval ipsec_dec_pin_B_desc_3 (desc)
    #elif core==2
        #define_eval ipsec_dec_pin_B_desc_2 (desc)
    #elif core==1
        #define_eval ipsec_dec_pin_B_desc_1 (desc)
    #elif core==0
        #define_eval ipsec_dec_pin_B_desc_0 (desc)
    #endif
    #define_eval gen_data_0 0
    #define_eval gen_data_1 0
    #define_eval gen_data_2 0
    #define_eval gen_data_3 0
    #define_eval gen_data_4 0x08705e5f
    #define_eval gen_data_5 0x0a13001a
    #define_eval gen_data_6 0x08380c58
    #define_eval gen_data_7 0x01905d19
    #define_eval gen_data_8 0x01005c5a
    #define_eval gen_data_9 0x0a235c5b
    #define_eval gen_data_10 0x00735958
    #define_eval gen_data_11 0x08281801
    #define_eval gen_data_12 0x01d05d19
    #define_eval gen_data_13 0x11725d0c
    #define_eval gen_data_14 0
    #define_eval gen_data_15 0
    #define_eval _cib_xfer_size_ipsec_dec_pin_B 16
    #define_eval _cib_code_size_ipsec_dec_pin_B 10
    #define_eval _cib_vars_base_ipsec_dec_pin_B 20
    #define_eval _cib_vars_size_ipsec_dec_pin_B 4
    #define_eval _cib_vars_xfer_ipsec_dec_pin_B 4
#endm


/**
 *  Prepare transfer regs to load static (aka 'pinned') encryption sequence
 *
 */
#macro crypto_load_ipsec_dec_pin_C (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
    crypto_build_xfer_reg (cr_xfr[0], packet, in_len)
    crypto_build_xfer_reg (cr_xfr[1], auth_only_data, iv)
    crypto_build_xfer_reg (cr_xfr[2], auth_length, hmac_keylen)
    crypto_build_xfer_reg (cr_xfr[3], crypt_select, crypt_modes)
    #define_eval _crypto_transfer_length 2
#endm


/**
 * Generate cmd sequence as constant data
 *
 *  @param core  crypto bulk core, 0 - 3 for NFP3XXX or 0 - 5 for NFP6XXX
 *  @param desc  sequence 'descriptor', contains sram location address
 *
 *  used in _crypto_library_load_dynamic in crypto_lib.uc
 */
#macro crypto_gen_ipsec_dec_pin_C (core, desc)
    #if core==5
        #define_eval ipsec_dec_pin_C_desc_5 (desc)
    #elif core==4
        #define_eval ipsec_dec_pin_C_desc_4 (desc)
    #elif core==3
        #define_eval ipsec_dec_pin_C_desc_3 (desc)
    #elif core==2
        #define_eval ipsec_dec_pin_C_desc_2 (desc)
    #elif core==1
        #define_eval ipsec_dec_pin_C_desc_1 (desc)
    #elif core==0
        #define_eval ipsec_dec_pin_C_desc_0 (desc)
    #endif
    #define_eval gen_data_0 0
    #define_eval gen_data_1 0
    #define_eval gen_data_2 0
    #define_eval gen_data_3 0
    #define_eval gen_data_4 0x08705e5f
    #define_eval gen_data_5 0x0a130029
    #define_eval gen_data_6 0x08380c58
    #define_eval gen_data_7 0x01905d28
    #define_eval gen_data_8 0x01005c5a
    #define_eval gen_data_9 0x0a235c5b
    #define_eval gen_data_10 0x00735958
    #define_eval gen_data_11 0x08281b01
    #define_eval gen_data_12 0x01d05d28
    #define_eval gen_data_13 0x11725d0c
    #define_eval gen_data_14 0
    #define_eval gen_data_15 0
    #define_eval _cib_xfer_size_ipsec_dec_pin_C 16
    #define_eval _cib_code_size_ipsec_dec_pin_C 10
    #define_eval _cib_vars_base_ipsec_dec_pin_C 20
    #define_eval _cib_vars_size_ipsec_dec_pin_C 4
    #define_eval _cib_vars_xfer_ipsec_dec_pin_C 4
#endm


/**
 *  Prepare transfer regs to load static (aka 'pinned') encryption sequence
 *
 */
#macro crypto_load_ipsec_dec_pin_D (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
    crypto_build_xfer_reg (cr_xfr[0], packet, in_len)
    crypto_build_xfer_reg (cr_xfr[1], auth_only_data, iv)
    crypto_build_xfer_reg (cr_xfr[2], auth_length, hmac_keylen)
    crypto_build_xfer_reg (cr_xfr[3], crypt_select, crypt_modes)
    #define_eval _crypto_transfer_length 2
#endm


/**
 * Generate cmd sequence as constant data
 *
 *  @param core  crypto bulk core, 0 - 3 for NFP3XXX or 0 - 5 for NFP6XXX
 *  @param desc  sequence 'descriptor', contains sram location address
 *
 *  used in _crypto_library_load_dynamic in crypto_lib.uc
 */
#macro crypto_gen_ipsec_dec_pin_D (core, desc)
    #if core==5
        #define_eval ipsec_dec_pin_D_desc_5 (desc)
    #elif core==4
        #define_eval ipsec_dec_pin_D_desc_4 (desc)
    #elif core==3
        #define_eval ipsec_dec_pin_D_desc_3 (desc)
    #elif core==2
        #define_eval ipsec_dec_pin_D_desc_2 (desc)
    #elif core==1
        #define_eval ipsec_dec_pin_D_desc_1 (desc)
    #elif core==0
        #define_eval ipsec_dec_pin_D_desc_0 (desc)
    #endif
    #define_eval gen_data_0 0
    #define_eval gen_data_1 0
    #define_eval gen_data_2 0
    #define_eval gen_data_3 0
    #define_eval gen_data_4 0x08705e5f
    #define_eval gen_data_5 0x0a130038
    #define_eval gen_data_6 0x08380c58
    #define_eval gen_data_7 0x01905d2b
    #define_eval gen_data_8 0x01005c5a
    #define_eval gen_data_9 0x0a235c5b
    #define_eval gen_data_10 0x00735958
    #define_eval gen_data_11 0x08282a01
    #define_eval gen_data_12 0x01d05d2b
    #define_eval gen_data_13 0x11725d0c
    #define_eval gen_data_14 0
    #define_eval gen_data_15 0
    #define_eval _cib_xfer_size_ipsec_dec_pin_D 16
    #define_eval _cib_code_size_ipsec_dec_pin_D 10
    #define_eval _cib_vars_base_ipsec_dec_pin_D 20
    #define_eval _cib_vars_size_ipsec_dec_pin_D 4
    #define_eval _cib_vars_xfer_ipsec_dec_pin_D 4
#endm

/**
 * ipsec_enc_strt_pin_A, B, C, D
 *
 * Similar to ipsec_enc_pin_A,B,C,D, but used to begin an encryption sequence
 * that will span multiple buffers, needed to handle jumbo packets.
 *
 * Calling the sequence:
 *
 * load_ipsec_enc_strt_pin_X (packet, in_len, auth_only_data, iv, auth_length,
 *        hmac_keylen, crypt_select, crypt_modes)
 *
 * @param packet              crypto SRAM addr of start of plaintext to encrypt
 * @param in_len              length of data to be encrypted
 * @param auth_only_data      crypto SRAM address of auth-only data (SPI and
 *                            seq number)
 * @param iv                  crypto SRAM address of the Initialization Vector
 * @param auth_length         byte length of (SPI/Seq
 * @param hmac_keylen         byte length of HMAC key, minus 1
 * @param crypt_select        config word 1, produced by crypto_setup_configs
 * @param crypt_modes         config word 2, produced by crypto_setup_configs
 **/

/**
 *  Prepare transfer regs to load static (aka 'pinned') encryption sequence
 *
 */
#macro crypto_load_ipsec_enc_strt_pin_A (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
    crypto_build_xfer_reg (cr_xfr[0], packet, in_len)
    crypto_build_xfer_reg (cr_xfr[1], auth_only_data, iv)
    crypto_build_xfer_reg (cr_xfr[2], auth_length, hmac_keylen)
    crypto_build_xfer_reg (cr_xfr[3], crypt_select, crypt_modes)
    #define_eval _crypto_transfer_length 2
#endm


/**
 * Generate cmd sequence as constant data
 *
 *  @param core  crypto bulk core, 0 - 3 for NFP3XXX or 0 - 5 for NFP6XXX
 *  @param desc  sequence 'descriptor', contains sram location address
 *
 *  used in _crypto_library_load_dynamic in crypto_lib.uc
 */
#macro crypto_gen_ipsec_enc_strt_pin_A (core, desc)
    #if core==5
        #define_eval ipsec_enc_strt_pin_A_desc_5 (desc)
    #elif core==4
        #define_eval ipsec_enc_strt_pin_A_desc_4 (desc)
    #elif core==3
        #define_eval ipsec_enc_strt_pin_A_desc_3 (desc)
    #elif core==2
        #define_eval ipsec_enc_strt_pin_A_desc_2 (desc)
    #elif core==1
        #define_eval ipsec_enc_strt_pin_A_desc_1 (desc)
    #elif core==0
        #define_eval ipsec_enc_strt_pin_A_desc_0 (desc)
    #endif
    #define_eval gen_data_0 0
    #define_eval gen_data_1 0
    #define_eval gen_data_2 0
    #define_eval gen_data_3 0
    #define_eval gen_data_4 0x08705e5f
    #define_eval gen_data_5 0x0a13000f
    #define_eval gen_data_6 0x08380c58
    #define_eval gen_data_7 0x01905d0e
    #define_eval gen_data_8 0x01005c5a
    #define_eval gen_data_9 0x0a235c5b
    #define_eval gen_data_10 0x13035958
    #define_eval gen_data_11 0
    #define_eval _cib_xfer_size_ipsec_enc_strt_pin_A 12
    #define_eval _cib_code_size_ipsec_enc_strt_pin_A 7
    #define_eval _cib_vars_base_ipsec_enc_strt_pin_A 20
    #define_eval _cib_vars_size_ipsec_enc_strt_pin_A 4
    #define_eval _cib_vars_xfer_ipsec_enc_strt_pin_A 4
#endm


/**
 *  Prepare transfer regs to load static (aka 'pinned') encryption sequence
 *
 */
#macro crypto_load_ipsec_enc_strt_pin_B (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
    crypto_build_xfer_reg (cr_xfr[0], packet, in_len)
    crypto_build_xfer_reg (cr_xfr[1], auth_only_data, iv)
    crypto_build_xfer_reg (cr_xfr[2], auth_length, hmac_keylen)
    crypto_build_xfer_reg (cr_xfr[3], crypt_select, crypt_modes)
    #define_eval _crypto_transfer_length 2
#endm


/**
 * Generate cmd sequence as constant data
 *
 *  @param core  crypto bulk core, 0 - 3 for NFP3XXX or 0 - 5 for NFP6XXX
 *  @param desc  sequence 'descriptor', contains sram location address
 *
 *  used in _crypto_library_load_dynamic in crypto_lib.uc
 */
#macro crypto_gen_ipsec_enc_strt_pin_B (core, desc)
    #if core==5
        #define_eval ipsec_enc_strt_pin_B_desc_5 (desc)
    #elif core==4
        #define_eval ipsec_enc_strt_pin_B_desc_4 (desc)
    #elif core==3
        #define_eval ipsec_enc_strt_pin_B_desc_3 (desc)
    #elif core==2
        #define_eval ipsec_enc_strt_pin_B_desc_2 (desc)
    #elif core==1
        #define_eval ipsec_enc_strt_pin_B_desc_1 (desc)
    #elif core==0
        #define_eval ipsec_enc_strt_pin_B_desc_0 (desc)
    #endif
    #define_eval gen_data_0 0
    #define_eval gen_data_1 0
    #define_eval gen_data_2 0
    #define_eval gen_data_3 0
    #define_eval gen_data_4 0x08705e5f
    #define_eval gen_data_5 0x0a13001a
    #define_eval gen_data_6 0x08380c58
    #define_eval gen_data_7 0x01905d19
    #define_eval gen_data_8 0x01005c5a
    #define_eval gen_data_9 0x0a235c5b
    #define_eval gen_data_10 0x13035958
    #define_eval gen_data_11 0
    #define_eval _cib_xfer_size_ipsec_enc_strt_pin_B 12
    #define_eval _cib_code_size_ipsec_enc_strt_pin_B 7
    #define_eval _cib_vars_base_ipsec_enc_strt_pin_B 20
    #define_eval _cib_vars_size_ipsec_enc_strt_pin_B 4
    #define_eval _cib_vars_xfer_ipsec_enc_strt_pin_B 4
#endm


/**
 *  Prepare transfer regs to load static (aka 'pinned') encryption sequence
 *
 */
#macro crypto_load_ipsec_enc_strt_pin_C (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
    crypto_build_xfer_reg (cr_xfr[0], packet, in_len)
    crypto_build_xfer_reg (cr_xfr[1], auth_only_data, iv)
    crypto_build_xfer_reg (cr_xfr[2], auth_length, hmac_keylen)
    crypto_build_xfer_reg (cr_xfr[3], crypt_select, crypt_modes)
    #define_eval _crypto_transfer_length 2
#endm


/**
 * Generate cmd sequence as constant data
 *
 *  @param core  crypto bulk core, 0 - 3 for NFP3XXX or 0 - 5 for NFP6XXX
 *  @param desc  sequence 'descriptor', contains sram location address
 *
 *  used in _crypto_library_load_dynamic in crypto_lib.uc
 */
#macro crypto_gen_ipsec_enc_strt_pin_C (core, desc)
    #if core==5
        #define_eval ipsec_enc_strt_pin_C_desc_5 (desc)
    #elif core==4
        #define_eval ipsec_enc_strt_pin_C_desc_4 (desc)
    #elif core==3
        #define_eval ipsec_enc_strt_pin_C_desc_3 (desc)
    #elif core==2
        #define_eval ipsec_enc_strt_pin_C_desc_2 (desc)
    #elif core==1
        #define_eval ipsec_enc_strt_pin_C_desc_1 (desc)
    #elif core==0
        #define_eval ipsec_enc_strt_pin_C_desc_0 (desc)
    #endif
    #define_eval gen_data_0 0
    #define_eval gen_data_1 0
    #define_eval gen_data_2 0
    #define_eval gen_data_3 0
    #define_eval gen_data_4 0x08705e5f
    #define_eval gen_data_5 0x0a130029
    #define_eval gen_data_6 0x08380c58
    #define_eval gen_data_7 0x01905d28
    #define_eval gen_data_8 0x01005c5a
    #define_eval gen_data_9 0x0a235c5b
    #define_eval gen_data_10 0x13035958
    #define_eval gen_data_11 0
    #define_eval _cib_xfer_size_ipsec_enc_strt_pin_C 12
    #define_eval _cib_code_size_ipsec_enc_strt_pin_C 7
    #define_eval _cib_vars_base_ipsec_enc_strt_pin_C 20
    #define_eval _cib_vars_size_ipsec_enc_strt_pin_C 4
    #define_eval _cib_vars_xfer_ipsec_enc_strt_pin_C 4
#endm


/**
 *  Prepare transfer regs to load static (aka 'pinned') encryption sequence
 *
 */
#macro crypto_load_ipsec_enc_strt_pin_D (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
    crypto_build_xfer_reg (cr_xfr[0], packet, in_len)
    crypto_build_xfer_reg (cr_xfr[1], auth_only_data, iv)
    crypto_build_xfer_reg (cr_xfr[2], auth_length, hmac_keylen)
    crypto_build_xfer_reg (cr_xfr[3], crypt_select, crypt_modes)
    #define_eval _crypto_transfer_length 2
#endm


/**
 * Generate cmd sequence as constant data
 *
 *  @param core  crypto bulk core, 0 - 3 for NFP3XXX or 0 - 5 for NFP6XXX
 *  @param desc  sequence 'descriptor', contains sram location address
 *
 *  used in _crypto_library_load_dynamic in crypto_lib.uc
 */
#macro crypto_gen_ipsec_enc_strt_pin_D (core, desc)
    #if core==5
        #define_eval ipsec_enc_strt_pin_D_desc_5 (desc)
    #elif core==4
        #define_eval ipsec_enc_strt_pin_D_desc_4 (desc)
    #elif core==3
        #define_eval ipsec_enc_strt_pin_D_desc_3 (desc)
    #elif core==2
        #define_eval ipsec_enc_strt_pin_D_desc_2 (desc)
    #elif core==1
        #define_eval ipsec_enc_strt_pin_D_desc_1 (desc)
    #elif core==0
        #define_eval ipsec_enc_strt_pin_D_desc_0 (desc)
    #endif
    #define_eval gen_data_0 0
    #define_eval gen_data_1 0
    #define_eval gen_data_2 0
    #define_eval gen_data_3 0
    #define_eval gen_data_4 0x08705e5f
    #define_eval gen_data_5 0x0a130038
    #define_eval gen_data_6 0x08380c58
    #define_eval gen_data_7 0x01905d2b
    #define_eval gen_data_8 0x01005c5a
    #define_eval gen_data_9 0x0a235c5b
    #define_eval gen_data_10 0x13035958
    #define_eval gen_data_11 0
    #define_eval _cib_xfer_size_ipsec_enc_strt_pin_D 12
    #define_eval _cib_code_size_ipsec_enc_strt_pin_D 7
    #define_eval _cib_vars_base_ipsec_enc_strt_pin_D 20
    #define_eval _cib_vars_size_ipsec_enc_strt_pin_D 4
    #define_eval _cib_vars_xfer_ipsec_enc_strt_pin_D 4
#endm

/**
 * ipsec_enc_cont
 *
 * Used after ipsec_enc_strt_pin_X, to continue encrypting a packet on a buffer
 * of data following the first part of the packet. Needed to handle jumbo
 * packets. Setup and ending condition from prior use of ipsec_enc_strt_pin_X is
 * required prior to invoking this sequence. In particular, the keys, config
 * registers, hash address, etc. must remain intact when this sequence is
 * started.
 *
 * Note that this sequence does not use or depend on buffer specific constants,
 * since the buffer specific values and setups are done in ipsec_enc_strt_pin_X
 *
 * Calling the sequence:
 *
 * load_ipsec_enc_cont (packet, in_len)
 *
 * @param packet          crypto SRAM addr of the start of plaintext to encrypt
 * @param in_len          length of data to encrypt
 * @param dummy1          placeholder for consistency with other sequences
 * @param dummy2          placeholder for consistency with other sequences
 * @param dummy3          placeholder for consistency with other sequences
 **/

/**
 *  Prepare transfer regs to load static (aka 'pinned') encryption sequence
 *
 */
#macro crypto_load_ipsec_enc_cont (cr_xfr, cr_ctx, packet, in_len, dummy1, dummy2, dummy3)
    crypto_build_xfer_reg (cr_xfr[0], packet, in_len)
    crypto_build_xfer_reg (cr_xfr[1], dummy1, dummy2)
    crypto_build_xfer_reg (cr_xfr[2], dummy3, 0)
    immed [cr_xfr[3], 0]
    #define_eval _crypto_transfer_length 2
#endm


/**
 * Generate cmd sequence as constant data
 *
 *  @param core  crypto bulk core, 0 - 3 for NFP3XXX or 0 - 5 for NFP6XXX
 *  @param desc  sequence 'descriptor', contains sram location address
 *
 *  used in _crypto_library_load_dynamic in crypto_lib.uc
 */
#macro crypto_gen_ipsec_enc_cont (core, desc)
    #if core==5
        #define_eval ipsec_enc_cont_desc_5 (desc)
    #elif core==4
        #define_eval ipsec_enc_cont_desc_4 (desc)
    #elif core==3
        #define_eval ipsec_enc_cont_desc_3 (desc)
    #elif core==2
        #define_eval ipsec_enc_cont_desc_2 (desc)
    #elif core==1
        #define_eval ipsec_enc_cont_desc_1 (desc)
    #elif core==0
        #define_eval ipsec_enc_cont_desc_0 (desc)
    #endif
    #define_eval gen_data_0 0
    #define_eval gen_data_1 0
    #define_eval gen_data_2 0
    #define_eval gen_data_3 0
    #define_eval gen_data_4 0x08180158
    #define_eval gen_data_5 0x13035958
    #define_eval gen_data_6 0
    #define_eval gen_data_7 0
    #define_eval _cib_xfer_size_ipsec_enc_cont 8
    #define_eval _cib_code_size_ipsec_enc_cont 2
    #define_eval _cib_vars_base_ipsec_enc_cont 20
    #define_eval _cib_vars_size_ipsec_enc_cont 4
    #define_eval _cib_vars_xfer_ipsec_enc_cont 4
#endm

/**
 * ipsec_enc_end
 *
 * Used after ipsec_enc_strt_pin_x, and possibly ipsec_enc_cont, to complete
 * encrypting a packet on the last buffer of data of the packet. Needed to
 * handle jumbo packets. Setup and ending condition from prior use of
 * ipsec_enc_strt_pin_x is required prior to invoking this sequence. In
 * particular, the keys, config registers, etc. must remain intact when this
 * sequence is started.
 *
 * The hash key address and result address for the packet must be specified as
 * parameters to this sequence.
 *
 * The key address should be equal to the sram location for the hash key
 * corresponding to the buffer ( A,B,C or D) used for the 1st part of the
 * packet. For e.g., if the 1st part of the packet was loaded using buffer A,
 * the hash key address would be the same as provided in the constant value
 * sa_hmk_a, since that is where the key was loaded.
 *
 * The hash key result address should be the sram address corresponding to the
 * hash result for the buffer being used when this macro ( ipsec_enc_end ) is
 * invoked. For example, if using buffer B, the hash result address would be
 * equal to the constant value set in sa_hmr_b
 *
 * Calling the sequence:
 *
 * crypto_load_ipsec_enc_end (packet, in_len, hmac_resadr, hmac_keyadr,
 *                                                                 hmac_keylen)
 *
 * @param packet         crypto SRAM addr of strt of plaintext to be encrypted
 * @param in_len         length of data to be encrypted
 * @param hmac_resadr    crypto SRAM address of the HMAC key calculation result
 * @param hmac_keyadr    crypto SRAM address of the start of the HMAC key
 * @param hmac_keylen    byte length of HMAC key, minus 1
 **/

/**
 *  Prepare transfer regs to load static (aka 'pinned') encryption sequence
 *
 */
#macro crypto_load_ipsec_enc_end (cr_xfr, cr_ctx, packet, in_len, hmac_resadr, hmac_keyadr, hmac_keylen)
    crypto_build_xfer_reg (cr_xfr[0], packet, in_len)
    crypto_build_xfer_reg (cr_xfr[1], hmac_resadr, hmac_keyadr)
    crypto_build_xfer_reg (cr_xfr[2], hmac_keylen, 0)
    immed [cr_xfr[3], 0]
    #define_eval _crypto_transfer_length 2
#endm


/**
 * Generate cmd sequence as constant data
 *
 *  @param core  crypto bulk core, 0 - 3 for NFP3XXX or 0 - 5 for NFP6XXX
 *  @param desc  sequence 'descriptor', contains sram location address
 *
 *  used in _crypto_library_load_dynamic in crypto_lib.uc
 */
#macro crypto_gen_ipsec_enc_end (core, desc)
    #if core==5
        #define_eval ipsec_enc_end_desc_5 (desc)
    #elif core==4
        #define_eval ipsec_enc_end_desc_4 (desc)
    #elif core==3
        #define_eval ipsec_enc_end_desc_3 (desc)
    #elif core==2
        #define_eval ipsec_enc_end_desc_2 (desc)
    #elif core==1
        #define_eval ipsec_enc_end_desc_1 (desc)
    #elif core==0
        #define_eval ipsec_enc_end_desc_0 (desc)
    #endif
    #define_eval gen_data_0 0
    #define_eval gen_data_1 0
    #define_eval gen_data_2 0
    #define_eval gen_data_3 0
    #define_eval gen_data_4 0x08180158
    #define_eval gen_data_5 0x03735958
    #define_eval gen_data_6 0x08285a01
    #define_eval gen_data_7 0x01d05c5b
    #define_eval gen_data_8 0x11725c0c
    #define_eval gen_data_9 0
    #define_eval gen_data_10 0
    #define_eval gen_data_11 0
    #define_eval _cib_xfer_size_ipsec_enc_end 12
    #define_eval _cib_code_size_ipsec_enc_end 5
    #define_eval _cib_vars_base_ipsec_enc_end 20
    #define_eval _cib_vars_size_ipsec_enc_end 4
    #define_eval _cib_vars_xfer_ipsec_enc_end 4
#endm

/**
 * ipsec_dec_strt_pin_A, B, C, D
 *
 * Similar to ipsec_dec_pin_A,B,C,D, but used to begin a decryption sequence
 * that will span multiple buffers, needed to handle jumbo packets.
 *
 * Calling the sequence:
 *
 * load_ipsec_dec_strt_pin_X (packet, in_len, auth_only_data, iv, auth_length,
 *        hmac_keylen, crypt_select, crypt_modes)
 *
 * @param packet              crypto SRAM addr of start of ciphertext to decrypt
 * @param in_len              length of data to be decrypted
 * @param auth_only_data      crypto SRAM address of auth-only data (SPI and
 *                            seq number)
 * @param iv                  crypto SRAM address of the Initialization Vector
 * @param auth_length         byte length of (SPI/Seq
 * @param hmac_keylen         byte length of HMAC key, minus 1
 * @param crypt_select        config word 1, produced by crypto_setup_configs
 * @param crypt_modes         config word 2, produced by crypto_setup_configs
 **/

/**
 *  Prepare transfer regs to load static (aka 'pinned') encryption sequence
 *
 */
#macro crypto_load_ipsec_dec_strt_pin_A (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
    crypto_build_xfer_reg (cr_xfr[0], packet, in_len)
    crypto_build_xfer_reg (cr_xfr[1], auth_only_data, iv)
    crypto_build_xfer_reg (cr_xfr[2], auth_length, hmac_keylen)
    crypto_build_xfer_reg (cr_xfr[3], crypt_select, crypt_modes)
    #define_eval _crypto_transfer_length 2
#endm


/**
 * Generate cmd sequence as constant data
 *
 *  @param core  crypto bulk core, 0 - 3 for NFP3XXX or 0 - 5 for NFP6XXX
 *  @param desc  sequence 'descriptor', contains sram location address
 *
 *  used in _crypto_library_load_dynamic in crypto_lib.uc
 */
#macro crypto_gen_ipsec_dec_strt_pin_A (core, desc)
    #if core==5
        #define_eval ipsec_dec_strt_pin_A_desc_5 (desc)
    #elif core==4
        #define_eval ipsec_dec_strt_pin_A_desc_4 (desc)
    #elif core==3
        #define_eval ipsec_dec_strt_pin_A_desc_3 (desc)
    #elif core==2
        #define_eval ipsec_dec_strt_pin_A_desc_2 (desc)
    #elif core==1
        #define_eval ipsec_dec_strt_pin_A_desc_1 (desc)
    #elif core==0
        #define_eval ipsec_dec_strt_pin_A_desc_0 (desc)
    #endif
    #define_eval gen_data_0 0
    #define_eval gen_data_1 0
    #define_eval gen_data_2 0
    #define_eval gen_data_3 0
    #define_eval gen_data_4 0x08705e5f
    #define_eval gen_data_5 0x0a13000f
    #define_eval gen_data_6 0x08380c58
    #define_eval gen_data_7 0x01905d0e
    #define_eval gen_data_8 0x01005c5a
    #define_eval gen_data_9 0x0a235c5b
    #define_eval gen_data_10 0x10035958
    #define_eval gen_data_11 0
    #define_eval _cib_xfer_size_ipsec_dec_strt_pin_A 12
    #define_eval _cib_code_size_ipsec_dec_strt_pin_A 7
    #define_eval _cib_vars_base_ipsec_dec_strt_pin_A 20
    #define_eval _cib_vars_size_ipsec_dec_strt_pin_A 4
    #define_eval _cib_vars_xfer_ipsec_dec_strt_pin_A 4
#endm


/**
 *  Prepare transfer regs to load static (aka 'pinned') encryption sequence
 *
 */
#macro crypto_load_ipsec_dec_strt_pin_B (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
    crypto_build_xfer_reg (cr_xfr[0], packet, in_len)
    crypto_build_xfer_reg (cr_xfr[1], auth_only_data, iv)
    crypto_build_xfer_reg (cr_xfr[2], auth_length, hmac_keylen)
    crypto_build_xfer_reg (cr_xfr[3], crypt_select, crypt_modes)
    #define_eval _crypto_transfer_length 2
#endm


/**
 * Generate cmd sequence as constant data
 *
 *  @param core  crypto bulk core, 0 - 3 for NFP3XXX or 0 - 5 for NFP6XXX
 *  @param desc  sequence 'descriptor', contains sram location address
 *
 *  used in _crypto_library_load_dynamic in crypto_lib.uc
 */
#macro crypto_gen_ipsec_dec_strt_pin_B (core, desc)
    #if core==5
        #define_eval ipsec_dec_strt_pin_B_desc_5 (desc)
    #elif core==4
        #define_eval ipsec_dec_strt_pin_B_desc_4 (desc)
    #elif core==3
        #define_eval ipsec_dec_strt_pin_B_desc_3 (desc)
    #elif core==2
        #define_eval ipsec_dec_strt_pin_B_desc_2 (desc)
    #elif core==1
        #define_eval ipsec_dec_strt_pin_B_desc_1 (desc)
    #elif core==0
        #define_eval ipsec_dec_strt_pin_B_desc_0 (desc)
    #endif
    #define_eval gen_data_0 0
    #define_eval gen_data_1 0
    #define_eval gen_data_2 0
    #define_eval gen_data_3 0
    #define_eval gen_data_4 0x08705e5f
    #define_eval gen_data_5 0x0a13001a
    #define_eval gen_data_6 0x08380c58
    #define_eval gen_data_7 0x01905d19
    #define_eval gen_data_8 0x01005c5a
    #define_eval gen_data_9 0x0a235c5b
    #define_eval gen_data_10 0x10035958
    #define_eval gen_data_11 0
    #define_eval _cib_xfer_size_ipsec_dec_strt_pin_B 12
    #define_eval _cib_code_size_ipsec_dec_strt_pin_B 7
    #define_eval _cib_vars_base_ipsec_dec_strt_pin_B 20
    #define_eval _cib_vars_size_ipsec_dec_strt_pin_B 4
    #define_eval _cib_vars_xfer_ipsec_dec_strt_pin_B 4
#endm


/**
 *  Prepare transfer regs to load static (aka 'pinned') encryption sequence
 *
 */
#macro crypto_load_ipsec_dec_strt_pin_C (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
    crypto_build_xfer_reg (cr_xfr[0], packet, in_len)
    crypto_build_xfer_reg (cr_xfr[1], auth_only_data, iv)
    crypto_build_xfer_reg (cr_xfr[2], auth_length, hmac_keylen)
    crypto_build_xfer_reg (cr_xfr[3], crypt_select, crypt_modes)
    #define_eval _crypto_transfer_length 2
#endm


/**
 * Generate cmd sequence as constant data
 *
 *  @param core  crypto bulk core, 0 - 3 for NFP3XXX or 0 - 5 for NFP6XXX
 *  @param desc  sequence 'descriptor', contains sram location address
 *
 *  used in _crypto_library_load_dynamic in crypto_lib.uc
 */
#macro crypto_gen_ipsec_dec_strt_pin_C (core, desc)
    #if core==5
        #define_eval ipsec_dec_strt_pin_C_desc_5 (desc)
    #elif core==4
        #define_eval ipsec_dec_strt_pin_C_desc_4 (desc)
    #elif core==3
        #define_eval ipsec_dec_strt_pin_C_desc_3 (desc)
    #elif core==2
        #define_eval ipsec_dec_strt_pin_C_desc_2 (desc)
    #elif core==1
        #define_eval ipsec_dec_strt_pin_C_desc_1 (desc)
    #elif core==0
        #define_eval ipsec_dec_strt_pin_C_desc_0 (desc)
    #endif
    #define_eval gen_data_0 0
    #define_eval gen_data_1 0
    #define_eval gen_data_2 0
    #define_eval gen_data_3 0
    #define_eval gen_data_4 0x08705e5f
    #define_eval gen_data_5 0x0a130029
    #define_eval gen_data_6 0x08380c58
    #define_eval gen_data_7 0x01905d28
    #define_eval gen_data_8 0x01005c5a
    #define_eval gen_data_9 0x0a235c5b
    #define_eval gen_data_10 0x10035958
    #define_eval gen_data_11 0
    #define_eval _cib_xfer_size_ipsec_dec_strt_pin_C 12
    #define_eval _cib_code_size_ipsec_dec_strt_pin_C 7
    #define_eval _cib_vars_base_ipsec_dec_strt_pin_C 20
    #define_eval _cib_vars_size_ipsec_dec_strt_pin_C 4
    #define_eval _cib_vars_xfer_ipsec_dec_strt_pin_C 4
#endm


/**
 *  Prepare transfer regs to load static (aka 'pinned') encryption sequence
 *
 */
#macro crypto_load_ipsec_dec_strt_pin_D (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
    crypto_build_xfer_reg (cr_xfr[0], packet, in_len)
    crypto_build_xfer_reg (cr_xfr[1], auth_only_data, iv)
    crypto_build_xfer_reg (cr_xfr[2], auth_length, hmac_keylen)
    crypto_build_xfer_reg (cr_xfr[3], crypt_select, crypt_modes)
    #define_eval _crypto_transfer_length 2
#endm


/**
 * Generate cmd sequence as constant data
 *
 *  @param core  crypto bulk core, 0 - 3 for NFP3XXX or 0 - 5 for NFP6XXX
 *  @param desc  sequence 'descriptor', contains sram location address
 *
 *  used in _crypto_library_load_dynamic in crypto_lib.uc
 */
#macro crypto_gen_ipsec_dec_strt_pin_D (core, desc)
    #if core==5
        #define_eval ipsec_dec_strt_pin_D_desc_5 (desc)
    #elif core==4
        #define_eval ipsec_dec_strt_pin_D_desc_4 (desc)
    #elif core==3
        #define_eval ipsec_dec_strt_pin_D_desc_3 (desc)
    #elif core==2
        #define_eval ipsec_dec_strt_pin_D_desc_2 (desc)
    #elif core==1
        #define_eval ipsec_dec_strt_pin_D_desc_1 (desc)
    #elif core==0
        #define_eval ipsec_dec_strt_pin_D_desc_0 (desc)
    #endif
    #define_eval gen_data_0 0
    #define_eval gen_data_1 0
    #define_eval gen_data_2 0
    #define_eval gen_data_3 0
    #define_eval gen_data_4 0x08705e5f
    #define_eval gen_data_5 0x0a130038
    #define_eval gen_data_6 0x08380c58
    #define_eval gen_data_7 0x01905d2b
    #define_eval gen_data_8 0x01005c5a
    #define_eval gen_data_9 0x0a235c5b
    #define_eval gen_data_10 0x10035958
    #define_eval gen_data_11 0
    #define_eval _cib_xfer_size_ipsec_dec_strt_pin_D 12
    #define_eval _cib_code_size_ipsec_dec_strt_pin_D 7
    #define_eval _cib_vars_base_ipsec_dec_strt_pin_D 20
    #define_eval _cib_vars_size_ipsec_dec_strt_pin_D 4
    #define_eval _cib_vars_xfer_ipsec_dec_strt_pin_D 4
#endm

/**
 * ipsec_dec_strt_nw_pin_A, B, C, D
 *
 * Similar to ipsec_dec_strt_pin_A,B,C,D, used to begin a decryption sequence
 * that will span multiple buffers, when no cipher is selected, so the sequence
 * does not have a wait for the cipher to complete. use to handle jumbo packets.
 *
 * Calling the sequence:
 *
 * load_ipsec_dec_strt_nw_pin_X (packet, in_len, auth_only_data, iv, auth_length,
 *        hmac_keylen, crypt_select, crypt_modes)
 *
 * @param packet              crypto SRAM addr of start of ciphertext to decrypt
 * @param in_len              length of data to be decrypted
 * @param auth_only_data      crypto SRAM address of auth-only data (SPI and
 *                            seq number)
 * @param iv                  crypto SRAM address of the Initialization Vector
 * @param auth_length         byte length of (SPI/Seq
 * @param hmac_keylen         byte length of HMAC key, minus 1
 * @param crypt_select        config word 1, produced by crypto_setup_configs
 * @param crypt_modes         config word 2, produced by crypto_setup_configs
 **/

/**
 *  Prepare transfer regs to load static (aka 'pinned') encryption sequence
 *
 */
#macro crypto_load_ipsec_dec_strt_nw_pin_A (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
    crypto_build_xfer_reg (cr_xfr[0], packet, in_len)
    crypto_build_xfer_reg (cr_xfr[1], auth_only_data, iv)
    crypto_build_xfer_reg (cr_xfr[2], auth_length, hmac_keylen)
    crypto_build_xfer_reg (cr_xfr[3], crypt_select, crypt_modes)
    #define_eval _crypto_transfer_length 2
#endm


/**
 * Generate cmd sequence as constant data
 *
 *  @param core  crypto bulk core, 0 - 3 for NFP3XXX or 0 - 5 for NFP6XXX
 *  @param desc  sequence 'descriptor', contains sram location address
 *
 *  used in _crypto_library_load_dynamic in crypto_lib.uc
 */
#macro crypto_gen_ipsec_dec_strt_nw_pin_A (core, desc)
    #if core==5
        #define_eval ipsec_dec_strt_nw_pin_A_desc_5 (desc)
    #elif core==4
        #define_eval ipsec_dec_strt_nw_pin_A_desc_4 (desc)
    #elif core==3
        #define_eval ipsec_dec_strt_nw_pin_A_desc_3 (desc)
    #elif core==2
        #define_eval ipsec_dec_strt_nw_pin_A_desc_2 (desc)
    #elif core==1
        #define_eval ipsec_dec_strt_nw_pin_A_desc_1 (desc)
    #elif core==0
        #define_eval ipsec_dec_strt_nw_pin_A_desc_0 (desc)
    #endif
    #define_eval gen_data_0 0
    #define_eval gen_data_1 0
    #define_eval gen_data_2 0
    #define_eval gen_data_3 0
    #define_eval gen_data_4 0x08705e5f
    #define_eval gen_data_5 0x0a13000f
    #define_eval gen_data_6 0x08380c58
    #define_eval gen_data_7 0x01905d0e
    #define_eval gen_data_8 0x01005c5a
    #define_eval gen_data_9 0x0a235c5b
    #define_eval gen_data_10 0x10015958
    #define_eval gen_data_11 0
    #define_eval _cib_xfer_size_ipsec_dec_strt_nw_pin_A 12
    #define_eval _cib_code_size_ipsec_dec_strt_nw_pin_A 7
    #define_eval _cib_vars_base_ipsec_dec_strt_nw_pin_A 20
    #define_eval _cib_vars_size_ipsec_dec_strt_nw_pin_A 4
    #define_eval _cib_vars_xfer_ipsec_dec_strt_nw_pin_A 4
#endm


/**
 *  Prepare transfer regs to load static (aka 'pinned') encryption sequence
 *
 */
#macro crypto_load_ipsec_dec_strt_nw_pin_B (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
    crypto_build_xfer_reg (cr_xfr[0], packet, in_len)
    crypto_build_xfer_reg (cr_xfr[1], auth_only_data, iv)
    crypto_build_xfer_reg (cr_xfr[2], auth_length, hmac_keylen)
    crypto_build_xfer_reg (cr_xfr[3], crypt_select, crypt_modes)
    #define_eval _crypto_transfer_length 2
#endm


/**
 * Generate cmd sequence as constant data
 *
 *  @param core  crypto bulk core, 0 - 3 for NFP3XXX or 0 - 5 for NFP6XXX
 *  @param desc  sequence 'descriptor', contains sram location address
 *
 *  used in _crypto_library_load_dynamic in crypto_lib.uc
 */
#macro crypto_gen_ipsec_dec_strt_nw_pin_B (core, desc)
    #if core==5
        #define_eval ipsec_dec_strt_nw_pin_B_desc_5 (desc)
    #elif core==4
        #define_eval ipsec_dec_strt_nw_pin_B_desc_4 (desc)
    #elif core==3
        #define_eval ipsec_dec_strt_nw_pin_B_desc_3 (desc)
    #elif core==2
        #define_eval ipsec_dec_strt_nw_pin_B_desc_2 (desc)
    #elif core==1
        #define_eval ipsec_dec_strt_nw_pin_B_desc_1 (desc)
    #elif core==0
        #define_eval ipsec_dec_strt_nw_pin_B_desc_0 (desc)
    #endif
    #define_eval gen_data_0 0
    #define_eval gen_data_1 0
    #define_eval gen_data_2 0
    #define_eval gen_data_3 0
    #define_eval gen_data_4 0x08705e5f
    #define_eval gen_data_5 0x0a13001a
    #define_eval gen_data_6 0x08380c58
    #define_eval gen_data_7 0x01905d19
    #define_eval gen_data_8 0x01005c5a
    #define_eval gen_data_9 0x0a235c5b
    #define_eval gen_data_10 0x10015958
    #define_eval gen_data_11 0
    #define_eval _cib_xfer_size_ipsec_dec_strt_nw_pin_B 12
    #define_eval _cib_code_size_ipsec_dec_strt_nw_pin_B 7
    #define_eval _cib_vars_base_ipsec_dec_strt_nw_pin_B 20
    #define_eval _cib_vars_size_ipsec_dec_strt_nw_pin_B 4
    #define_eval _cib_vars_xfer_ipsec_dec_strt_nw_pin_B 4
#endm


/**
 *  Prepare transfer regs to load static (aka 'pinned') encryption sequence
 *
 */
#macro crypto_load_ipsec_dec_strt_nw_pin_C (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
    crypto_build_xfer_reg (cr_xfr[0], packet, in_len)
    crypto_build_xfer_reg (cr_xfr[1], auth_only_data, iv)
    crypto_build_xfer_reg (cr_xfr[2], auth_length, hmac_keylen)
    crypto_build_xfer_reg (cr_xfr[3], crypt_select, crypt_modes)
    #define_eval _crypto_transfer_length 2
#endm


/**
 * Generate cmd sequence as constant data
 *
 *  @param core  crypto bulk core, 0 - 3 for NFP3XXX or 0 - 5 for NFP6XXX
 *  @param desc  sequence 'descriptor', contains sram location address
 *
 *  used in _crypto_library_load_dynamic in crypto_lib.uc
 */
#macro crypto_gen_ipsec_dec_strt_nw_pin_C (core, desc)
    #if core==5
        #define_eval ipsec_dec_strt_nw_pin_C_desc_5 (desc)
    #elif core==4
        #define_eval ipsec_dec_strt_nw_pin_C_desc_4 (desc)
    #elif core==3
        #define_eval ipsec_dec_strt_nw_pin_C_desc_3 (desc)
    #elif core==2
        #define_eval ipsec_dec_strt_nw_pin_C_desc_2 (desc)
    #elif core==1
        #define_eval ipsec_dec_strt_nw_pin_C_desc_1 (desc)
    #elif core==0
        #define_eval ipsec_dec_strt_nw_pin_C_desc_0 (desc)
    #endif
    #define_eval gen_data_0 0
    #define_eval gen_data_1 0
    #define_eval gen_data_2 0
    #define_eval gen_data_3 0
    #define_eval gen_data_4 0x08705e5f
    #define_eval gen_data_5 0x0a130029
    #define_eval gen_data_6 0x08380c58
    #define_eval gen_data_7 0x01905d28
    #define_eval gen_data_8 0x01005c5a
    #define_eval gen_data_9 0x0a235c5b
    #define_eval gen_data_10 0x10015958
    #define_eval gen_data_11 0
    #define_eval _cib_xfer_size_ipsec_dec_strt_nw_pin_C 12
    #define_eval _cib_code_size_ipsec_dec_strt_nw_pin_C 7
    #define_eval _cib_vars_base_ipsec_dec_strt_nw_pin_C 20
    #define_eval _cib_vars_size_ipsec_dec_strt_nw_pin_C 4
    #define_eval _cib_vars_xfer_ipsec_dec_strt_nw_pin_C 4
#endm


/**
 *  Prepare transfer regs to load static (aka 'pinned') encryption sequence
 *
 */
#macro crypto_load_ipsec_dec_strt_nw_pin_D (cr_xfr, cr_ctx, packet, in_len, auth_only_data, iv, auth_length, hmac_keylen, crypt_select, crypt_modes)
    crypto_build_xfer_reg (cr_xfr[0], packet, in_len)
    crypto_build_xfer_reg (cr_xfr[1], auth_only_data, iv)
    crypto_build_xfer_reg (cr_xfr[2], auth_length, hmac_keylen)
    crypto_build_xfer_reg (cr_xfr[3], crypt_select, crypt_modes)
    #define_eval _crypto_transfer_length 2
#endm


/**
 * Generate cmd sequence as constant data
 *
 *  @param core  crypto bulk core, 0 - 3 for NFP3XXX or 0 - 5 for NFP6XXX
 *  @param desc  sequence 'descriptor', contains sram location address
 *
 *  used in _crypto_library_load_dynamic in crypto_lib.uc
 */
#macro crypto_gen_ipsec_dec_strt_nw_pin_D (core, desc)
    #if core==5
        #define_eval ipsec_dec_strt_nw_pin_D_desc_5 (desc)
    #elif core==4
        #define_eval ipsec_dec_strt_nw_pin_D_desc_4 (desc)
    #elif core==3
        #define_eval ipsec_dec_strt_nw_pin_D_desc_3 (desc)
    #elif core==2
        #define_eval ipsec_dec_strt_nw_pin_D_desc_2 (desc)
    #elif core==1
        #define_eval ipsec_dec_strt_nw_pin_D_desc_1 (desc)
    #elif core==0
        #define_eval ipsec_dec_strt_nw_pin_D_desc_0 (desc)
    #endif
    #define_eval gen_data_0 0
    #define_eval gen_data_1 0
    #define_eval gen_data_2 0
    #define_eval gen_data_3 0
    #define_eval gen_data_4 0x08705e5f
    #define_eval gen_data_5 0x0a130038
    #define_eval gen_data_6 0x08380c58
    #define_eval gen_data_7 0x01905d2b
    #define_eval gen_data_8 0x01005c5a
    #define_eval gen_data_9 0x0a235c5b
    #define_eval gen_data_10 0x10015958
    #define_eval gen_data_11 0
    #define_eval _cib_xfer_size_ipsec_dec_strt_nw_pin_D 12
    #define_eval _cib_code_size_ipsec_dec_strt_nw_pin_D 7
    #define_eval _cib_vars_base_ipsec_dec_strt_nw_pin_D 20
    #define_eval _cib_vars_size_ipsec_dec_strt_nw_pin_D 4
    #define_eval _cib_vars_xfer_ipsec_dec_strt_nw_pin_D 4
#endm

/**
 * ipsec_dec_cont
 *
 * Used after ipsec_dec_strt_pin_X, to continue decrypting a packet on a buffer
 * of data following the first part of the packet. Needed to handle jumbo
 * packets. Setup and ending condition from prior use of ipsec_dec_strt_pin_X is
 * required prior to invoking this sequence. In particular, the keys, config
 * registers, hash address, etc. must remain intact when this sequence is
 * started.
 *
 * Note that this sequence does not use or depend on buffer specific constants,
 * since the buffer specific values and setups are done in ipsec_dec_strt_pin_X
 *
 * Calling the sequence:
 *
 * load_ipsec_dec_cont (packet, in_len)
 *
 * @param packet          crypto SRAM addr of the start of ciphertext to decrypt
 * @param in_len          length of data to decrypt
 * @param dummy1          placeholder for consistency with other sequences
 * @param dummy2          placeholder for consistency with other sequences
 * @param dummy3          placeholder for consistency with other sequences
 **/

/**
 *  Prepare transfer regs to load static (aka 'pinned') encryption sequence
 *
 */
#macro crypto_load_ipsec_dec_cont (cr_xfr, cr_ctx, packet, in_len, dummy1, dummy2, dummy3)
    crypto_build_xfer_reg (cr_xfr[0], packet, in_len)
    crypto_build_xfer_reg (cr_xfr[1], dummy1, dummy2)
    crypto_build_xfer_reg (cr_xfr[2], dummy3, 0)
    immed [cr_xfr[3], 0]
    #define_eval _crypto_transfer_length 2
#endm


/**
 * Generate cmd sequence as constant data
 *
 *  @param core  crypto bulk core, 0 - 3 for NFP3XXX or 0 - 5 for NFP6XXX
 *  @param desc  sequence 'descriptor', contains sram location address
 *
 *  used in _crypto_library_load_dynamic in crypto_lib.uc
 */
#macro crypto_gen_ipsec_dec_cont (core, desc)
    #if core==5
        #define_eval ipsec_dec_cont_desc_5 (desc)
    #elif core==4
        #define_eval ipsec_dec_cont_desc_4 (desc)
    #elif core==3
        #define_eval ipsec_dec_cont_desc_3 (desc)
    #elif core==2
        #define_eval ipsec_dec_cont_desc_2 (desc)
    #elif core==1
        #define_eval ipsec_dec_cont_desc_1 (desc)
    #elif core==0
        #define_eval ipsec_dec_cont_desc_0 (desc)
    #endif
    #define_eval gen_data_0 0
    #define_eval gen_data_1 0
    #define_eval gen_data_2 0
    #define_eval gen_data_3 0
    #define_eval gen_data_4 0x08180158
    #define_eval gen_data_5 0x10035958
    #define_eval gen_data_6 0
    #define_eval gen_data_7 0
    #define_eval _cib_xfer_size_ipsec_dec_cont 8
    #define_eval _cib_code_size_ipsec_dec_cont 2
    #define_eval _cib_vars_base_ipsec_dec_cont 20
    #define_eval _cib_vars_size_ipsec_dec_cont 4
    #define_eval _cib_vars_xfer_ipsec_dec_cont 4
#endm

/**
 * ipsec_dec_cont_nw
 *
 * Similar to ipsec_dec_cont, but used on no cipher selection, so it does
 * not have a wait for cipher. used for jumbo packets
 *
 * Note that this sequence does not use or depend on buffer specific constants,
 * since the buffer specific values and setups are done in ipsec_dec_nw_strt_pin_X
 *
 * Calling the sequence:
 *
 * load_ipsec_dec_cont_nw (packet, in_len)
 *
 * @param packet          crypto SRAM addr of the start of ciphertext to decrypt
 * @param in_len          length of data to decrypt
 * @param dummy1          placeholder for consistency with other sequences
 * @param dummy2          placeholder for consistency with other sequences
 * @param dummy3          placeholder for consistency with other sequences
 **/

/**
 *  Prepare transfer regs to load static (aka 'pinned') encryption sequence
 *
 */
#macro crypto_load_ipsec_dec_cont_nw (cr_xfr, cr_ctx, packet, in_len, dummy1, dummy2, dummy3)
    crypto_build_xfer_reg (cr_xfr[0], packet, in_len)
    crypto_build_xfer_reg (cr_xfr[1], dummy1, dummy2)
    crypto_build_xfer_reg (cr_xfr[2], dummy3, 0)
    immed [cr_xfr[3], 0]
    #define_eval _crypto_transfer_length 2
#endm


/**
 * Generate cmd sequence as constant data
 *
 *  @param core  crypto bulk core, 0 - 3 for NFP3XXX or 0 - 5 for NFP6XXX
 *  @param desc  sequence 'descriptor', contains sram location address
 *
 *  used in _crypto_library_load_dynamic in crypto_lib.uc
 */
#macro crypto_gen_ipsec_dec_cont_nw (core, desc)
    #if core==5
        #define_eval ipsec_dec_cont_nw_desc_5 (desc)
    #elif core==4
        #define_eval ipsec_dec_cont_nw_desc_4 (desc)
    #elif core==3
        #define_eval ipsec_dec_cont_nw_desc_3 (desc)
    #elif core==2
        #define_eval ipsec_dec_cont_nw_desc_2 (desc)
    #elif core==1
        #define_eval ipsec_dec_cont_nw_desc_1 (desc)
    #elif core==0
        #define_eval ipsec_dec_cont_nw_desc_0 (desc)
    #endif
    #define_eval gen_data_0 0
    #define_eval gen_data_1 0
    #define_eval gen_data_2 0
    #define_eval gen_data_3 0
    #define_eval gen_data_4 0x08180158
    #define_eval gen_data_5 0x10015958
    #define_eval gen_data_6 0
    #define_eval gen_data_7 0
    #define_eval _cib_xfer_size_ipsec_dec_cont_nw 8
    #define_eval _cib_code_size_ipsec_dec_cont_nw 2
    #define_eval _cib_vars_base_ipsec_dec_cont_nw 20
    #define_eval _cib_vars_size_ipsec_dec_cont_nw 4
    #define_eval _cib_vars_xfer_ipsec_dec_cont_nw 4
#endm

/**
 * ipsec_dec_end
 *
 * Used after ipsec_dec_strt_pin_x, and possibly ipsec_dec_cont, to complete
 * decrypting a packet on the last buffer of data of the packet. Needed to
 * handle jumbo packets. Setup and ending condition from prior use of
 * ipsec_dec_strt_pin_x is required prior to invoking this sequence. In
 * particular, the keys, config registers, etc. must remain intact when this
 * sequence is started.
 *
 * The hash key address and result address for the packet must be specified as
 * parameters to this sequence.
 *
 * The key address should be equal to the sram location for the hash key
 * corresponding to the buffer ( A,B,C or D) used for the 1st part of the
 * packet. For e.g., if the 1st part of the packet was loaded using buffer A,
 * the hash key address would be the same as provided in the constant value
 * sa_hmk_a, since that is where the key was loaded.
 *
 * The hash key result address should be the sram address corresponding to the
 * hash result for the buffer being used when this macro ( ipsec_dec_end ) is
 * invoked. For example, if using buffer B, the hash result address would be
 * equal to the constant value set in sa_hmr_b
 *
 * Calling the sequence:
 *
 * crypto_load_ipsec_dec_end (packet, in_len, hmac_resadr, hmac_keyadr,
 *                                                             hmac_keylen)
 *
 * @param packet         crypto SRAM addr of strt of ciphertext to be decrypted
 * @param in_len         length of data to be decrypted
 * @param hmac_resadr    crypto SRAM address of the HMAC key calculation result
 * @param hmac_keyadr    crypto SRAM address of the start of the HMAC key
 * @param hmac_keylen    byte length of HMAC key, minus 1
 **/

/**
 *  Prepare transfer regs to load static (aka 'pinned') encryption sequence
 *
 */
#macro crypto_load_ipsec_dec_end (cr_xfr, cr_ctx, packet, in_len, hmac_resadr, hmac_keyadr, hmac_keylen)
    crypto_build_xfer_reg (cr_xfr[0], packet, in_len)
    crypto_build_xfer_reg (cr_xfr[1], hmac_resadr, hmac_keyadr)
    crypto_build_xfer_reg (cr_xfr[2], hmac_keylen, 0)
    immed [cr_xfr[3], 0]
    #define_eval _crypto_transfer_length 2
#endm


/**
 * Generate cmd sequence as constant data
 *
 *  @param core  crypto bulk core, 0 - 3 for NFP3XXX or 0 - 5 for NFP6XXX
 *  @param desc  sequence 'descriptor', contains sram location address
 *
 *  used in _crypto_library_load_dynamic in crypto_lib.uc
 */
#macro crypto_gen_ipsec_dec_end (core, desc)
    #if core==5
        #define_eval ipsec_dec_end_desc_5 (desc)
    #elif core==4
        #define_eval ipsec_dec_end_desc_4 (desc)
    #elif core==3
        #define_eval ipsec_dec_end_desc_3 (desc)
    #elif core==2
        #define_eval ipsec_dec_end_desc_2 (desc)
    #elif core==1
        #define_eval ipsec_dec_end_desc_1 (desc)
    #elif core==0
        #define_eval ipsec_dec_end_desc_0 (desc)
    #endif
    #define_eval gen_data_0 0
    #define_eval gen_data_1 0
    #define_eval gen_data_2 0
    #define_eval gen_data_3 0
    #define_eval gen_data_4 0x08180158
    #define_eval gen_data_5 0x00735958
    #define_eval gen_data_6 0x08285a01
    #define_eval gen_data_7 0x01d05c5b
    #define_eval gen_data_8 0x11725c0c
    #define_eval gen_data_9 0
    #define_eval gen_data_10 0
    #define_eval gen_data_11 0
    #define_eval _cib_xfer_size_ipsec_dec_end 12
    #define_eval _cib_code_size_ipsec_dec_end 5
    #define_eval _cib_vars_base_ipsec_dec_end 20
    #define_eval _cib_vars_size_ipsec_dec_end 4
    #define_eval _cib_vars_xfer_ipsec_dec_end 4
#endm

/**
 * SA layout for IPSec-oriented crypto instruction sequences
 *
 * byte offset   description
 *
 * 0x20-0x3F     cipher key (up to 32 bytes)
 * 0x40-0x7F     authentication key (up to 64 bytes)
 *
 * Based constants:
 * temp0 and temp1        - used for scratch space
 * sa_hmk_X and sa_cik_X  - HMAC and cipher key, fixed locations within the SA
 *
 * Note, for 'pinned' operation, each bulk core (crypto context) is 'assigned'
 * four buffer sets. The buffer sets include the SA and the packet. At
 * initialization time, the four SA base addresses must be passed in as base
 * 1/2/3/4; this set of four addresses is unique to each bulk core.
 * Implementators may choose to use fewer than the four allocated buffer sets.
 **/
#define CRYPTO_NFP_MODE 1  // NFP6xxx mode

#macro crypto_gen_compr_constants (base0, base1, base2, base3, base4)
    #define_eval gen_data_0 0x0000001f
    #define_eval gen_data_1 0x00000000
    #define_eval gen_data_2 0x01800000
    #define_eval gen_data_2 (gen_data_2 + base0)
    #define_eval gen_data_2 (gen_data_2 + (base1 << 16))
    #define_eval gen_data_3 0x00200040
    #define_eval gen_data_3 (gen_data_3 + base1)
    #define_eval gen_data_3 (gen_data_3 + (base1 << 16))
    #define_eval gen_data_4 0x00400180
    #define_eval gen_data_4 (gen_data_4 + base2)
    #define_eval gen_data_4 (gen_data_4 + (base2 << 16))
    #define_eval gen_data_5 0x01800020
    #define_eval gen_data_5 (gen_data_5 + base2)
    #define_eval gen_data_5 (gen_data_5 + (base3 << 16))
    #define_eval gen_data_6 0x00000000
    #define_eval gen_data_7 0x00000000
    #define_eval gen_data_8 0x00200040
    #define_eval gen_data_8 (gen_data_8 + base3)
    #define_eval gen_data_8 (gen_data_8 + (base3 << 16))
    #define_eval gen_data_9 0x00400180
    #define_eval gen_data_9 (gen_data_9 + base4)
    #define_eval gen_data_9 (gen_data_9 + (base4 << 16))
    #define_eval gen_data_10 0x00000000
    #define_eval gen_data_11 0x00000000
    #define_eval gen_data_12 0x00000020
    #define_eval gen_data_12 (gen_data_12 + base4)
    #define_eval gen_data_13 0
    #define_eval _cib_xfer_size_compr_constants 0x000e
    #define_eval _cib_code_size_compr_constants 0x000d
    #define_eval _cib_vars_size_compr_constants 0
    #define_eval _cib_base_addr_compr_constants 0x0000
#endm

/** @}
 * @}
 */

#endif /* __CRYPTO_LIB_IPSEC_UC__ */

