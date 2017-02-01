/*
 * Copyright (C) 2014-2016,  Netronome Systems, Inc.  All rights reserved.
 *
 * @file          cluster_target.c
 * @brief         Cluster target related functions
 */

#include <assert.h>
#include <nfp.h>

#include <nfp6000/nfp_me.h>

#include <cluster_target_c.h>


__intrinsic void __signal_me(unsigned int dst_island, unsigned int dst_me, unsigned int dst_ctx, unsigned int sig_no)
{
    unsigned int addr;

    addr = ((dst_island & 0x3f) << 24 | (dst_me & 15) << 9 | (dst_ctx & 7) << 6 | (sig_no & 15) << 2);

    __asm ct[interthread_signal, --, addr, 0, --];
}

__intrinsic void ct_nn_write(unsigned int dst_island, unsigned int dst_me,  unsigned int sig_no, unsigned int nn_register,
    __xwrite void *data, size_t size, sync_t sync, SIGNAL *sig_ptr)
{
    unsigned int                addr;
    unsigned int                count = (size >> 2);
    struct nfp_mecsr_prev_alu   ind;

    ctassert(__is_ct_const(sync));
    try_ctassert(size != 0);
    ctassert(sync == sig_done || sync == ctx_swap);

    addr = ((dst_island & 0x3f) << 24 | (dst_me & 15) << 17 | (sig_no & 15) << 10 | (1 << 9) | (nn_register & 0x7f) << 2);

    if (__is_ct_const(size) ) {

        if (count <= 8)
        {

            if (sync == sig_done) {
                __asm
                {
                    ct[ctnn_write, *data, addr, 0, __ct_const_val(count)], sig_done[*sig_ptr]
                }
            } else {
                __asm
                {
                    ct[ctnn_write, *data, addr, 0, __ct_const_val(count)], ctx_swap[*sig_ptr]
                }
            }

        } else {

            /* Setup length in PrevAlu for the indirect */
            ind.__raw = 0;
            ind.ov_len = 1;
            ind.length = count - 1;
            if (sync == sig_done) {
                __asm
                {
                    alu[--, --, B, ind.__raw]
                    ct[ctnn_write, *data, addr, 0, __ct_const_val(count)], sig_done[*sig_ptr], indirect_ref
                }
            } else {
                __asm
                {
                    alu[--, --, B, ind.__raw]
                    ct[ctnn_write, *data, addr, 0, __ct_const_val(count)], ctx_swap[*sig_ptr], indirect_ref
                }
            }

        }

    } else {

            /* Setup length in PrevAlu for the indirect */
            ind.__raw = 0;
            ind.ov_len = 1;
            ind.length = count - 1;
            if (sync == sig_done) {
                __asm
                {
                    alu[--, --, B, ind.__raw]
                    ct[ctnn_write, *data, addr, 0, __ct_const_val(count)], sig_done[*sig_ptr], indirect_ref
                }
            } else {
                __asm
                {
                    alu[--, --, B, ind.__raw]
                    ct[ctnn_write, *data, addr, 0, __ct_const_val(count)], ctx_swap[*sig_ptr], indirect_ref
                }
            }

    }
}