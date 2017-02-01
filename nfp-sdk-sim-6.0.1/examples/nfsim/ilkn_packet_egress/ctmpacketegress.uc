/**
 * Copyright (C) 2014 Netronome Systems, Inc.  All rights reserved.
 */
// -*- mode: asm; eval: (font-lock-mode t); -*-
// Basic code to
//
//	1. Loop waiting for new packets in CTM via packet_add_thread
//	2. Parse packet metadata for packet/sequence number and ingress port (first 24B)
//	3. Append NBI PM rewrite script to perform NOP
//	4. Use cmd_indirect_ref_0 and indirect_ref to overwrite CPP fields for packet complete cmd to NBI
//	   -- includes sequence#, sequencer#,txqueue#
//  Note: mailbox0 contains packet rx'd count

#include <stdmac.uc>

/*b Defines */

/*b Override Data - (for previous ALU used w/ indirect_ref token */    

;; DATA16[2 ;14] = 0
;; DATA16[2 ;12] = NBI#
;; DATA16[2 ;10] = Retry
;; DATA16[10;0] = TxQueue#
;; LEN[2;3] = 0
;; LEN[3;0] = Rewrite Script Offset

#define_eval  OR_SM    (0x00000001)   // override signal_master (OSM)
#define_eval  OR_DM    (1 << 1)       // override (OM) data_master, data_master_island
#define_eval  OR_DR    (1 << 3)       // override (OD) data ref (see above)
#define_eval  OR_BM    (1 << 6)       // override (OB) byte mask
#define_eval  OR_L     (1 << 7)       // override (OL) length
#define_eval  MD_LEN   (2 << 8)       // (metadata) length (LEN=24bytes 0=8B, 1=16B, 2=24B)
#define_eval  MD_TXQ   (0 << 16)      // (metadata) nbi | retry | txqueue(11b)
#define_eval  OR_INIT  (OR_SM | OR_DM | OR_DR | OR_BM | OR_L | MD_LEN | MD_TXQ)

;; Write no-op rewrite script 64 bits
;; [1;63] = Direct script
;; [8;48] No-op = 3b111(opcode) | 4bxxxx (bytes) | 1bx (modrdata)
;; [8;40] offset 8hxx
;; [3;37] unused
;; [5;32] rdata_len = 5bxxxxx
#define_eval  MDS ((1 << 31) | (7<<21))



/*b Registers */    
.reg $tombox[6]
.xfer_order $tombox
.reg $modscript[2]
.xfer_order $modscript
.reg addr
.reg a
.sig ss
.reg packetnum
.reg csr_data
.reg override_data
.reg txqueue


    /*b Generate No-op Rewrite Script */
	$modscript[0] = MDS  
	$modscript[1] = 0  // pad 4 bytes


.while 1
    immed32(addr, 0) 
    /*b Add Thread */

	//
	// ingress
	//

    // get packet from CTM
	mem[packet_add_thread, $tombox[0], addr, 0, 6], ctx_swap[ss]

    /*b ME Wakes up when signal arrives. It now has the header and metadata in first 6 transfer registers */
	// Header[1 ;63] Split - Indicates the packet is split between
	// Header[2 ;61] Reserved
	// Header[29;32] Mu Ptr - Pointer to the memory buffer    @2KB boundaries.
	// Header[6 ;26] CTM Number - CTM that contains the    packet.
	// Header[10;16] Packet Number - Used to address the packet    in the CTM.
	// Header[2 ;14] Blist - Buffer List Queue associated with the Mu Ptr.
	// Header[14; 0] Packet Length in bytes.

	/*b copy the ingress port/channel to be egress port/channel */
	// channel = $tombox[3] (8;24)
	// >> 8 to move to bit 16 of override data, << 3 to multiply by 8 (8 tx queues per channel)
    alu_op(txqueue, $tombox[3], and, 0xFF000000)	
    alu[txqueue, --, b, txqueue, >>5]	
    alu_op(override_data, OR_INIT, or, txqueue)

	/*b Get Packet Number */
    alu_op(packetnum, $tombox[0], and, 0x03FF0000); leave in upper word


	//
	// egress
	//

	/*b Write Rewrite Script to CTM*/

    // Construct Address for direct write to CTM location
    // [8;32] Don'Care
	// [1;31] AddressMode
	// [6;25] Don'tCare
	// [9;16] Packet Number
	// [5;11] Don'tCare
	// [11;0] Offset in B

    immed32(addr,24);                           ; write PM script to offset 24
    alu[addr, addr, or, packetnum]              ; address correct pkt in CTM
    alu_op(addr, addr, or, (1<<31))             ; CTM indirect mode

	mem[write, $modscript[0], addr, 0, 1], ctx_swap[ss]

    /*b Generate Packet Processing Complete */

    //
    // 1. TO know where the packet starts, one needs to know the BPE from the bottom 3 bits
    //    and then lookup the CTM Offset for that BPE (REMEMBER: different BPEs for the same CTM
    //    can have different offset values - that is LEGAL.
    //
    // 2. Packet length is the actual packet length, not the hdr+packet length written into CTM
    //    So, this needs to be calculated to send the data to egress
    //
    // So per packet, we have 2 lookups (one for BPE number, one to lookup the CTM offset from BPE#),
    // and then a computation of total length. This is not desirable

    // This code assumes hard coded CTM offset of 32B
    
    alu[addr, $tombox[0], +, 32] ; this seems dodgy but it adds 32B to packet length

    /*b Local CSR Wr */
    // write to cmd_indirect_ref_0 to over relevantr CPP cmd fields for packet_complete
    
    alu_op(csr_data, $tombox[2], and, 0x0FFF0000); get sequence# from metadata
    alu_op(csr_data, csr_data, or, 0x0)        ; hardcode sequencer# to 0

    // update cmd_indirect_ref_0 and prime ALU result for packet_complete_unicast command
    local_csr_wr[cmd_indirect_ref_0, csr_data]
    alu[--,--,B,override_data]

    /*b Send Packet Processing Complete */
	mem[packet_complete_unicast, --, addr, 0], indirect_ref

.endw
    
