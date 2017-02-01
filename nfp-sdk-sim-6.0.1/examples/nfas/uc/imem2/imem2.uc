/**
 * Copyright (C) 2014 Netronome Systems, Inc.  All rights reserved.
 */
#include <stdmac.uc>

    .reg pattern[4]
    .reg $data[4]
    .xfer_order $data
    .reg addr_hi, @addr_lo, offset
    .sig mem_sig

    .if (ctx() == 0)
        immed[@addr_lo, 0]
        local_csr_wr[MAILBOX_0, @addr_lo]
    .endif

    alu[addr_hi, --, B, ((2 << 6) | (28 << 0 )), <<24]

    move(pattern[0], 0x01234500)
    move(pattern[1], 0x12345604)
    move(pattern[2], 0x23456708)
    move(pattern[3], 0x3456780c)


    .while (1)

        ; Get offset for this thread and increment for the next one
        move(offset, @addr_lo)
        alu[@addr_lo, @addr_lo, +, 16]

        alu[$data[0], pattern[0], +, offset]
        alu[$data[1], pattern[1], +, offset]
        alu[$data[2], pattern[2], +, offset]
        alu[$data[3], pattern[3], +, offset]

        mem[write, $data[0], addr_hi, <<8, offset, 2], ctx_swap[mem_sig]

        alu[offset, offset, +, 16]
        local_csr_wr[MAILBOX_0, offset]

    .endw
