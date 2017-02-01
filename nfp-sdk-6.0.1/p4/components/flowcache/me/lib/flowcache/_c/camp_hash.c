
/*
 * Copyright (C) 2014-2016,  Netronome Systems, Inc.  All rights reserved.
 *
 * @file        camp_hash.c
 * @brief       Implement camp hash, write pad values to imem and on initialisation,
 *              read from imem and put into NN registers.
 *              All threads on the ME use these NN registers when calculating the hash value
 *              from the local key values, local key length and pad values. Use crc_be for hashing.
 *              Result are two 32-bit hash values.
 */

#ifndef _CAMP_HASH_C_
#define _CAMP_HASH_C_

#include <nfp.h>
#include <stdint.h>
#include <nfp/me.h>
#include <nfp/mem_bulk.h>
#include <nfp6000/nfp_me.h>

#include "camp_hash_c.h"
#include "rotate_c.h"
#include "cluster_target_c.h"


#define CAMP_HASH_PAD_NN_IDX            96
#define CAMP_HASH_PAD_NUM_ELEMENTS      32

__mem __addr40 __declspec(scope(global), export, imem0) uint32_t camp_hash_pad[CAMP_HASH_PAD_NUM_ELEMENTS] =
    {
        0x05e37dc7, 0x820bfd7f, 0xc0a8d1d3, 0x1e7433f4,
        0xa4e17724, 0x22efe253, 0xd04ea6bd, 0xae676609,
        0xe4354df6, 0xa76b008c, 0xa8dec911, 0x4a573ca8,
        0x2a85c196, 0x31d0a299, 0x5dae2d36, 0x5ba9ea1f,
        0x6c37b38f, 0x3aee5498, 0x2421f934, 0x01f1ae08,
        0x993a96e7, 0x9a1a6bf3, 0x0bbc6452, 0x57d85210,
        0xb1b60e52, 0xee86be25, 0x51cdac42, 0x22d6cd1d,
        0x15b6fa5f, 0xc56d11f3, 0xc8a2c9bb, 0xe13a69d5
    };


/*
* Read from NN registers and increment after each read.
* Index should have been set before calling this function otherwise
* zero (0) is assumed.
*/
__intrinsic
unsigned int nn_ring_dequeue_incr(void)
{
    unsigned result;
    __asm alu[result, --, B, *n$index++]
    return result;
}

__intrinsic
void nn_ring_enqueue_incr(
    unsigned int val
)
{
    __asm alu[*n$index++, --, B, val]
}


__intrinsic
unsigned int dbl_shr(
    unsigned int srcA,          /* high 32 bits */
    unsigned int srcB,          /* low 32 bits */
    unsigned int shift_cnt      /* shift count */
)
{
    unsigned result;
    {
        if (__is_ct_const(shift_cnt))
        {
            __asm dbl_shf[result, srcA, srcB, >>__ct_const_val(shift_cnt)]
        }
        else
        {
            __asm
            {
                alu[--, shift_cnt, OR, 0]
                dbl_shf[result, srcA, srcB, >>indirect]
            }
        }
    }
    return result;
}


void camp_hash_init(void)
{
    SIGNAL              sig;
    SIGNAL              nn_write_sig;

    __xread uint32_t    camp_pad[16];
    __xwrite uint32_t   camp_pad_nn[16];
    __gpr uint32_t      i = 0;

    /*
     * Set the next neighbour register start index.
     */
   local_csr_write(local_csr_nn_put, CAMP_HASH_PAD_NN_IDX);

    for (i = 0; i < CAMP_HASH_PAD_NUM_ELEMENTS; i += 16)
    {
        /*
         * Read the pad values from imem and store in NN registers starting at index CAMP_HASH_PAD_NN_IDX.
         * This is to be done once per ME and all contexts use values from NN registers.
         */
        mem_read32(camp_pad, (__mem void *)(camp_hash_pad + i), 16 << 2);

        nn_ring_enqueue_incr(camp_pad[0]);
        nn_ring_enqueue_incr(camp_pad[1]);
        nn_ring_enqueue_incr(camp_pad[2]);
        nn_ring_enqueue_incr(camp_pad[3]);
        nn_ring_enqueue_incr(camp_pad[4]);
        nn_ring_enqueue_incr(camp_pad[5]);
        nn_ring_enqueue_incr(camp_pad[6]);
        nn_ring_enqueue_incr(camp_pad[7]);
        nn_ring_enqueue_incr(camp_pad[8]);
        nn_ring_enqueue_incr(camp_pad[9]);
        nn_ring_enqueue_incr(camp_pad[10]);
        nn_ring_enqueue_incr(camp_pad[11]);
        nn_ring_enqueue_incr(camp_pad[12]);
        nn_ring_enqueue_incr(camp_pad[13]);
        nn_ring_enqueue_incr(camp_pad[14]);
        nn_ring_enqueue_incr(camp_pad[15]);
    }
}



__gpr hash_result_t  camp_hash(__lmem uint32_t *in_key_index, __gpr uint32_t in_key_length)
 {
    __gpr hash_result_t                 out_hash;
    __gpr uint32_t                      value_0, value_1, copy, rotate, crc;
    __gpr uint32_t                      i;


    /*
     * Set the next neighbour register start index.
     */
   local_csr_write(local_csr_nn_get, CAMP_HASH_PAD_NN_IDX);


   /*
    * Set the crc local_csr_crc_remainder to the key length
    */
   local_csr_write(local_csr_crc_remainder, in_key_length);


   /*
    * Start reading NN register from CAMP_HASH_PAD_NN_IDX and
    * increment after each read.
    */

    __asm alu[value_0, in_key_length, +, *n$index++]    // value_0 = in_key_length + nn_ring_dequeue_incr();
    __asm alu[value_1, in_key_length, XOR, *n$index++]  // value_1 = in_key_length ^ nn_ring_dequeue_incr();

    for (i = in_key_length; i > 0; i--)
    {
        /*
        * Process all elements in key
        */
        value_0 = value_0 + *in_key_index;
        in_key_index++;

        __asm alu[value_0, value_0, XOR, *n$index++]    // value_0 = value_0 ^ nn_ring_dequeue_incr();

        __asm crc_be[crc_32, copy, value_0]

        //  value_0 = dbl_shr(value_0, value_0, value_1);
        //  value_1 = value_1 ^ copy;
        __asm alu[value_1, value_1, xor, value_0]
        __asm dbl_shf[value_0, copy, value_0, >>indirect]
    }


    rotate = dbl_shr(value_1, value_1, value_0);
    value_0 = value_0 ^ rotate;

    crc = local_csr_read(local_csr_crc_remainder);
    value_0 = value_0 + crc;


    rotate = dbl_shr(value_0, value_0, value_1);
    value_1 = value_1 ^ rotate;

    value_1 = value_1 ^ crc;

    /*
     * Result
     */
    out_hash.value_0 = value_0;
    out_hash.value_1 = value_1;

    return out_hash;
 }



#endif  // _CAMP_HASH_C_