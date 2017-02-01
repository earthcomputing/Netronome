/**
 * Copyright (C) 2014 Netronome Systems, Inc.  All rights reserved.
 */
.begin

    .if (ctx() != 0)
        ctx_arb[kill]
    .endif

    /* loop */
    loop#:

    local_csr_wr[mailbox0, 5]
    local_csr_wr[mailbox1, 6]
    local_csr_wr[mailbox2, 7]
    local_csr_wr[mailbox3, 8]

    br[loop#]

.end

