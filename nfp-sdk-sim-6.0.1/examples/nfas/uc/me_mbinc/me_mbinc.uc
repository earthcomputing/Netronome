/**
 * Copyright (C) 2014 Netronome Systems, Inc.  All rights reserved.
 */
/*! @file me_nbinc.uc
 * @brief Test incrementing mailboxes on 4 Contexts
 *
 *
 */

.begin
    // Define shared GPRs across all contexts to be used
    .reg @i0
    .reg @i1
    .reg @i2
    .reg @i3

    .reg il_num
    .reg me_num
    .reg temp

    .if (ctx() == 0)
        /* Initialise the counters */
        immed[@i0,0]
        immed[@i1,1]
        immed[@i2,1]
        immed[@i3,1]

        // Read ActCtxStatus
        local_csr_rd[ACTIVE_CTX_STS]
        immed[il_num,0]
        alu_shf[me_num, 0xF, AND, il_num, >>3]
        alu_shf[il_num, 0x3F, AND, il_num, >>25]
        // MEs are numbered 4-15
        alu[me_num, me_num, -, 4]

        immed[temp, 0]
        alu_shf[temp, temp, OR, il_num, <<24] 
        alu_shf[temp, temp, OR, me_num, <<16]
        local_csr_wr[mailbox0, temp]

        local_csr_wr[mailbox1, 2]
        local_csr_wr[mailbox2, 3]
        local_csr_wr[mailbox3, 4]

        loop0#:    
        local_csr_rd[mailbox0]
        immed[@i0, 0]
        alu[@i0, @i0, +, 1]
        local_csr_wr[mailbox0, @i0]
        ctx_arb[voluntary]
        nop
        br[loop0#]
    .endif

    .if (ctx() == 1)
        loop1#:
        alu[@i1, @i1, +, 1]
        local_csr_wr[mailbox1, @i1]
        ctx_arb[voluntary]        
        nop
        br[loop1#]
    .endif
    
    .if (ctx() == 2)
        loop2#:
        alu[@i2, @i2, +, 1]
        local_csr_wr[mailbox2, @i2]
        ctx_arb[voluntary]        
        nop
        br[loop2#]
    .endif

    .if (ctx() == 3)
        loop3#:
        alu[@i3, @i3, +, 1]
        local_csr_wr[mailbox3, @i3]
        ctx_arb[voluntary]        
        nop
        br[loop3#]
    .endif

    ctx_arb[kill]
   
.end

