/**
 * Copyright (C) 2014 Netronome Systems, Inc.  All rights reserved.
 */
/*! @file cls.uc
 * @brief Test writting to CLS
 *
 *
 */

.begin

    .sig s
    .reg $x[2]
    .xfer_order $x
    .reg addrh
    .reg addrl

    .reg cls_addr

    immed[addrl, 0]
    immed[addrh, 0]
    immed_w1[addrh, 0x2000]
    immed[cls_addr, 0]

    .if ctx() == 0

        alu[$x[1], --, B, addrl]
loop1#:
        mem[write, $x[0], addrh, <<8, addrl, 1], sig_done[s]
        ctx_arb[s]
        alu[$x[0], addrl, +, 8]
        cls[incr, --, cls_addr, <<8, 0x00]
        cls[incr, --, cls_addr, <<8, 0x10]
        br[loop1#]
        nop
        nop
        nop
    .endif

    ctx_arb[kill]
    nop
    nop
    nop

.end
