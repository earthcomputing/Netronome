/**
 * Copyright (C) 2014 Netronome Systems, Inc.  All rights reserved.
 */
.begin

    .if (ctx() != 0)
        ctx_arb[kill]
    .endif

    /* loop */
    loop#:

    local_csr_wr[mailbox0, 1]
    local_csr_wr[mailbox1, 2]
    local_csr_wr[mailbox2, 3]
    local_csr_wr[mailbox3, 4]

    br[loop#]

.end

