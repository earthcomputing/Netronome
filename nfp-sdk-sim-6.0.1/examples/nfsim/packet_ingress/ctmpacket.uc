/**
 * Copyright (C) 2014 Netronome Systems, Inc.  All rights reserved.
 */
#include <stdmac.uc>

// Basic test to
//
//	1. Prime local mailboxes0,1,2 with 0x0
//	2. Read CTM location via packet add threads
//	3. Write tombox contents to mailbox0,1,2
//	4. Count packets in mailbox3

.reg $tombox[6]
.xfer_order $tombox
.reg addr 
.sig ss
.reg count
.reg packetnum
.reg len

.if (ctx() == 0)
	// Prime the mailboxes
    immed32(count, 0)
	local_csr_wr[mailbox0, 0]
	local_csr_wr[mailbox1, 0]
	local_csr_wr[mailbox2, 0]
  .while 1
    immed32(addr, 0)

    // Wait for a packet
	mem[packet_add_thread, $tombox[0], addr, 0, 6], ctx_swap[ss]

	// Write mailbox0 with contents of tombox
	local_csr_wr[mailbox0,$tombox[0]]
	local_csr_wr[mailbox1,$tombox[1]]
	local_csr_wr[mailbox2,$tombox[2]]
    alu[count, count, +, 1]

	local_csr_wr[mailbox3,count]

	packetnum = $tombox[1] >> 15

	// To free a packet
	mem[packet_free_and_signal, --, packetnum, 0], ctx_swap[ss]

  .endw
.endif

ctx_arb[kill]
