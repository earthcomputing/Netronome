/**
 * Copyright (C) 2014 Netronome Systems, Inc.  All rights reserved.
 */
// -*- mode: asm; eval: (font-lock-mode t); -*-
// Basic test to forward packets from NBI ingress to NBI egress
//

/*b Registers */    
.reg $tombox[6]
.xfer_order $tombox
.reg $modscript[2]
.xfer_order $modscript
.reg moddata
.reg addr
.sig ss
.reg packetnum
.reg seqnum
.reg mask
.reg csr_data
.reg override_data
.reg tmp
.reg @cnt

/*b Override Data */    
;; DATA16[2 ;14] = 0
;; DATA16[2 ;12] = NBI#
;; DATA16[2 ;10] = Retry
;; DATA16[10;0] = TxQueue#
;; OTAT = 0
;; OSC = 0
;; OSN = 0
;; LEN[2;3] = 0
;; LEN[3;0] = Rewrite Script Offset
;; OL = 1
;; OB = 1
;; OD = 001
;; OM = 1
;; OSM=1

override_data =  1 ;override signal_master (OSM)
override_data = override_data | (1<<1) ;override (OM) data_master, data_master_island
override_data = override_data | (1<<3) ;override (OD)
override_data = override_data | (1<<6) ;override (OB)
override_data = override_data | (1<<7) ;override (OL)
override_data = override_data | (2<<8) ;override (LEN=24bytes 0=8B, 1=16B, 2=24B)
override_data = override_data | (0<<16); txqueue=0, retry=0, nbi=0

/*b Generate Rewrite Script */
// Write no-op rewrite script 64 bits
// [1;63] = Direct script
moddata = (1 << 31) 
// [8;48] No-op = 3b111(opcode) | 4bxxxx (bytes) | 1bx (modrdata)
// [8;40] offset 8hxx
// [3;37] unused
// [5;32] rdata_len = 5bxxxxx
moddata = moddata | (7<<21)
$modscript[0] = moddata
// pad 4 bytes
$modscript[1] = 0 

.if (ctx() == 0)
    @cnt = 0 
.endif

.while 1
    addr = 0x0
    /*b Add Thread */
    
    mem[packet_add_thread, $tombox[0], addr, 0, 6], ctx_swap[ss]

    /*b ME Wakes up when signal arrives. It now has the header and metadata in first 6 transfer registers */
    //Header[1 ;63] Split - Indicates the packet is split between
    //Header[2 ;61] Reserved
    //Header[29;32] Mu Ptr - Pointer to the memory buffer    @2KB boundaries.
    //Header[6 ;26] CTM Number - CTM that contains the    packet.
    //Header[10;16] Packet Number - Used to address the packet    in the CTM.
    //Header[2 ;14] Blist - Buffer List Queue associated with the Mu Ptr.
    //Header[14; 0] Packet Length in bytes.

    /*b Get Packet Number */
    packetnum = $tombox[0] >> 16
    mask = 0x3ff
    packetnum = packetnum & mask

    //
    // egress
    //

    /*b Write Rewrite Script */
    //[8;32] Don'Care
    //[1;31] AddressMode
    //[6;25] Don'tCare
    //[9;16] Packet Number
    //[5;11] Don'tCare
    //[11;0] Offset in B
    addr = 24 
    tmp  = packetnum << 16
    addr = addr | tmp
    addr = addr | (1<<31)
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

    // This code assumes hard coded CTM offset of 64B
    addr = $tombox[0] + 64

    //[3 cycles] X = (SEQ#<<16) | SQR#
    //[1 cycle ] Local CSR Wr (X)
    /*b Local CSR Wr */

    alu[@cnt, @cnt, +, 1]
    local_csr_wr[mailbox3, @cnt]
        
    seqnum = $tombox[2] >> 16
    mask = 0xfff
    seqnum = seqnum & mask
    local_csr_wr[mailbox0, seqnum]
    csr_data = (seqnum << 16)
    local_csr_wr[cmd_indirect_ref_0, csr_data]
    /*b LMem */
    //[1 cycle ] ALU [--,--,B, LMem]
    alu[--,--,B,override_data]

    /*b Send Packet Processing Complete */
    //[2 cycles] MEM[packet_complete,--,$M[1]], Indirect
    mem[packet_complete_unicast, --, addr, 0], indirect_ref
.endw
