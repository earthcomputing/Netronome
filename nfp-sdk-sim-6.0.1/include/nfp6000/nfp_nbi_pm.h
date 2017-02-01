/*
 * Copyright (C) 2013, Netronome Systems, Inc.  All rights reserved.
 *
 * Disclaimer: this file is provided without any warranty
 * as part of an early-access program, and the content is
 * bound to change before the final release.
 */


#ifndef NFP6000_NBI_PM_H
#define NFP6000_NBI_PM_H

/*================================================================*/
/* CPP Address Map (CPP_BASE = IMB translation: island + indexes) */

/*----------------------------------------------------------------*/
/* XPB Address Map (XPB_BASE = NFP_XPB_ISLAND(island))          */

#define NFP_NBI_PMX                                         (NBIX_BASE + 0x280000)

/* XPB_BASE + NFP_NBI_PMX_OPCODE + NFP_NBI_PMX_OPCODE_* */
#define NFP_NBI_PMX_OPCODE                                  (NFP_NBI_PMX + 0x00000)
/* XPB_BASE + NFP_NBI_PMX_OPCODE_ECC + (nfp_ecc.h) */
#define NFP_NBI_PMX_OPCODE_ECC                              (NFP_NBI_PMX + 0x10000)
/* XPB_BASE + NFP_NBI_PMX_RDATA + NFP_NBI_PMX_RDATA_* */
#define NFP_NBI_PMX_RDATA                                   (NFP_NBI_PMX + 0x20000)
/* XPB_BASE + NFP_NBI_PMX_RDATA_ECC + (nfp_ecc.h) */
#define NFP_NBI_PMX_LOCAL_RDATA_ECC                         (NFP_NBI_PMX + 0x30000)
/* XPB_BASE + NFP_NBI_PMX_CSR + NFP_NBI_PMX_CSR_* */
#define NFP_NBI_PMX_CSR                                     (NFP_NBI_PMX + 0x40000)

/*================================================================*/



/* HGID: 587d39ed6b9b */

#ifndef NFP6000_LONGNAMES

/*---------------------------------------------------------------- 
  Register: NbiPmCnfg - Packet Modifier Configuration Register.
  Register: NbiPmCnfg - Packet Modifier Configuration Register.
  Register: NbiPmCnfg - Packet Modifier Configuration Register.
    Bits:  1    AddModScrBytes(rw) - The NBI Packet Modifier reports number of deleted bytes (for every packet) to the Traffic Manager. 'add_mo_scr_bytes' enables the addition of modification script bytes into deleted byte-count.
    Bits:  0    AddPadBytes(rw) - The NBI Packet Modifier reports number of inserted bytes (for every packet) to the Traffic Manager. 'add_pad_bytes' enables the addition of pad bytes into inserted byte-count.
*/
#define NFP_NBI_PMX_OPCODE_NBI_PM_CNFG                       0x00000000
#define NFP_NBI_PMX_RDATA_NBI_PM_CNFG                        0x00000000
#define NFP_NBI_PMX_CSR_NBI_PM_CNFG                          0x00000000
#define   NFP_NBI_PMX_OPCODE_NBIPMCNFG_ADD_MOD_SCR_BYTES     (1 << 1)
#define     NFP_NBI_PMX_OPCODE_NBIPMCNFG_ADD_MOD_SCR_BYTES_0 (0 << 1)
#define     NFP_NBI_PMX_OPCODE_NBIPMCNFG_ADD_MOD_SCR_BYTES_1 (1 << 1)
#define   NFP_NBI_PMX_OPCODE_NBIPMCNFG_ADD_PAD_BYTES         (1 << 0)
#define     NFP_NBI_PMX_OPCODE_NBIPMCNFG_ADD_PAD_BYTES_0     (0 << 0)
#define     NFP_NBI_PMX_OPCODE_NBIPMCNFG_ADD_PAD_BYTES_1     (1 << 0)
/*---------------------------------------------------------------- 
  Register: NbiPmPktCntHi - Packet Modifier packet counter, High 32-bit
  Register: NbiPmPktCntHi - Packet Modifier packet counter, High 32-bit
  Register: NbiPmPktCntHi - Packet Modifier packet counter, High 32-bit
    Bits: 31:0  NbiPmPktCntHi(rc) - Upper 32-bit of 64-bit Packet Count.
*/
#define NFP_NBI_PMX_OPCODE_NBI_PM_PKT_CNT_HI                 0x00000004
#define NFP_NBI_PMX_RDATA_NBI_PM_PKT_CNT_HI                  0x00000004
#define NFP_NBI_PMX_CSR_NBI_PM_PKT_CNT_HI                    0x00000004
#define   NFP_NBI_PMX_OPCODE_NBIPMPKTCNTHI_NBI_PM_PKT_CNT_HI(_x) (((_x) & 0xffffffff) << 0)
#define   NFP_NBI_PMX_OPCODE_NBIPMPKTCNTHI_NBI_PM_PKT_CNT_HI_of(_x) (((_x) >> 0) & 0xffffffff)
/*---------------------------------------------------------------- 
  Register: NbiPmPktCntLo - Packet Modifier packet counter, Low 32-bit
  Register: NbiPmPktCntLo - Packet Modifier packet counter, Low 32-bit
  Register: NbiPmPktCntLo - Packet Modifier packet counter, Low 32-bit
    Bits: 31:0  NbiPmPktCntLo(rc) - Lower 32-bit of 64-bit Packet Count.
*/
#define NFP_NBI_PMX_OPCODE_NBI_PM_PKT_CNT_LO                 0x00000008
#define NFP_NBI_PMX_RDATA_NBI_PM_PKT_CNT_LO                  0x00000008
#define NFP_NBI_PMX_CSR_NBI_PM_PKT_CNT_LO                    0x00000008
#define   NFP_NBI_PMX_OPCODE_NBIPMPKTCNTLO_NBI_PM_PKT_CNT_LO(_x) (((_x) & 0xffffffff) << 0)
#define   NFP_NBI_PMX_OPCODE_NBIPMPKTCNTLO_NBI_PM_PKT_CNT_LO_of(_x) (((_x) >> 0) & 0xffffffff)
/*---------------------------------------------------------------- 
  Register: NbiPmModCntHi - Counts number of modifications performed, High 32-bit
  Register: NbiPmModCntHi - Counts number of modifications performed, High 32-bit
  Register: NbiPmModCntHi - Counts number of modifications performed, High 32-bit
    Bits: 31:0  NbiPmModCntHi(rc) - Upper 32-bit of 64-bit Modification Count.
*/
#define NFP_NBI_PMX_OPCODE_NBI_PM_MOD_CNT_HI                 0x0000000c
#define NFP_NBI_PMX_RDATA_NBI_PM_MOD_CNT_HI                  0x0000000c
#define NFP_NBI_PMX_CSR_NBI_PM_MOD_CNT_HI                    0x0000000c
#define   NFP_NBI_PMX_OPCODE_NBIPMMODCNTHI_NBI_PM_MOD_CNT_HI(_x) (((_x) & 0xffffffff) << 0)
#define   NFP_NBI_PMX_OPCODE_NBIPMMODCNTHI_NBI_PM_MOD_CNT_HI_of(_x) (((_x) >> 0) & 0xffffffff)
/*---------------------------------------------------------------- 
  Register: NbiPmModCntLo - Counts number of modifications performed, Low 32-bit
  Register: NbiPmModCntLo - Counts number of modifications performed, Low 32-bit
  Register: NbiPmModCntLo - Counts number of modifications performed, Low 32-bit
    Bits: 31:0  NbiPmModCntLo(rc) - Lower 32-bit of 64-bit Modification Count.
*/
#define NFP_NBI_PMX_OPCODE_NBI_PM_MOD_CNT_LO                 0x00000010
#define NFP_NBI_PMX_RDATA_NBI_PM_MOD_CNT_LO                  0x00000010
#define NFP_NBI_PMX_CSR_NBI_PM_MOD_CNT_LO                    0x00000010
#define   NFP_NBI_PMX_OPCODE_NBIPMMODCNTLO_NBI_PM_MOD_CNT_LO(_x) (((_x) & 0xffffffff) << 0)
#define   NFP_NBI_PMX_OPCODE_NBIPMMODCNTLO_NBI_PM_MOD_CNT_LO_of(_x) (((_x) >> 0) & 0xffffffff)
/*---------------------------------------------------------------- 
  Register: NbiPmPktErrCntHi - Packet Error Count, High 32-bit
  Register: NbiPmPktErrCntHi - Packet Error Count, High 32-bit
  Register: NbiPmPktErrCntHi - Packet Error Count, High 32-bit
    Bits: 31:0  NbiPmPktErrCntHi(rc) - Upper 32-bit of 64-bit Packet Error Count.
*/
#define NFP_NBI_PMX_OPCODE_NBI_PM_PKT_ERR_CNT_HI             0x00000014
#define NFP_NBI_PMX_RDATA_NBI_PM_PKT_ERR_CNT_HI              0x00000014
#define NFP_NBI_PMX_CSR_NBI_PM_PKT_ERR_CNT_HI                0x00000014
#define   NFP_NBI_PMX_OPCODE_NBIPMPKTERRCNTHI_NBI_PM_PKT_ERR_CNT_HI(_x) (((_x) & 0xffffffff) << 0)
#define   NFP_NBI_PMX_OPCODE_NBIPMPKTERRCNTHI_NBI_PM_PKT_ERR_CNT_HI_of(_x) (((_x) >> 0) & 0xffffffff)
/*---------------------------------------------------------------- 
  Register: NbiPmPktErrCntLo - Packet Error Count, Low 32-bit
  Register: NbiPmPktErrCntLo - Packet Error Count, Low 32-bit
  Register: NbiPmPktErrCntLo - Packet Error Count, Low 32-bit
    Bits: 31:0  NbiPmPktErrCntLo(rc) - Lower 32-bit of 64-bit Packet Error Count.
*/
#define NFP_NBI_PMX_OPCODE_NBI_PM_PKT_ERR_CNT_LO             0x00000018
#define NFP_NBI_PMX_RDATA_NBI_PM_PKT_ERR_CNT_LO              0x00000018
#define NFP_NBI_PMX_CSR_NBI_PM_PKT_ERR_CNT_LO                0x00000018
#define   NFP_NBI_PMX_OPCODE_NBIPMPKTERRCNTLO_NBI_PM_PKT_ERR_CNT_LO(_x) (((_x) & 0xffffffff) << 0)
#define   NFP_NBI_PMX_OPCODE_NBIPMPKTERRCNTLO_NBI_PM_PKT_ERR_CNT_LO_of(_x) (((_x) >> 0) & 0xffffffff)
/*---------------------------------------------------------------- 
  Register: NbiPmModscrDecodeErrCntHi - Modification Script Decode Error Count, High 32-bit
  Register: NbiPmModscrDecodeErrCntHi - Modification Script Decode Error Count, High 32-bit
  Register: NbiPmModscrDecodeErrCntHi - Modification Script Decode Error Count, High 32-bit
    Bits: 31:0  NbiPmModscrDecodeErrCntHi(rc) - Upper 32-bit of 64-bit Script Decode Error Count.
*/
#define NFP_NBI_PMX_OPCODE_NBI_PM_MODSCR_DECODE_ERR_CNT_HI   0x0000001c
#define NFP_NBI_PMX_RDATA_NBI_PM_MODSCR_DECODE_ERR_CNT_HI    0x0000001c
#define NFP_NBI_PMX_CSR_NBI_PM_MODSCR_DECODE_ERR_CNT_HI      0x0000001c
#define   NFP_NBI_PMX_OPCODE_NBIPMMODSCRDECODEERRCNTHI_NBI_PM_MODSCR_DECODE_ERR_CNT_HI(_x) (((_x) & 0xffffffff) << 0)
#define   NFP_NBI_PMX_OPCODE_NBIPMMODSCRDECODEERRCNTHI_NBI_PM_MODSCR_DECODE_ERR_CNT_HI_of(_x) (((_x) >> 0) & 0xffffffff)
/*---------------------------------------------------------------- 
  Register: NbiPmModscrDecodeErrCntLo - Modification Script Decode Error Count, Low 32-bit
  Register: NbiPmModscrDecodeErrCntLo - Modification Script Decode Error Count, Low 32-bit
  Register: NbiPmModscrDecodeErrCntLo - Modification Script Decode Error Count, Low 32-bit
    Bits: 31:0  NbiPmModscrDecodeErrCntLo(rw) - lower 32-bit of 64-bit Script Decode Error Count.
*/
#define NFP_NBI_PMX_OPCODE_NBI_PM_MODSCR_DECODE_ERR_CNT_LO   0x00000020
#define NFP_NBI_PMX_RDATA_NBI_PM_MODSCR_DECODE_ERR_CNT_LO    0x00000020
#define NFP_NBI_PMX_CSR_NBI_PM_MODSCR_DECODE_ERR_CNT_LO      0x00000020
#define   NFP_NBI_PMX_OPCODE_NBIPMMODSCRDECODEERRCNTLO_NBI_PM_MODSCR_DECODE_ERR_CNT_LO(_x) (((_x) & 0xffffffff) << 0)
#define   NFP_NBI_PMX_OPCODE_NBIPMMODSCRDECODEERRCNTLO_NBI_PM_MODSCR_DECODE_ERR_CNT_LO_of(_x) (((_x) >> 0) & 0xffffffff)

#else /* NFP6000_LONGNAMES */

/*---------------------------------------------------------------- 
  Register: NbiPmCnfg - Packet Modifier Configuration Register.
  Register: NbiPmCnfg - Packet Modifier Configuration Register.
  Register: NbiPmCnfg - Packet Modifier Configuration Register.
    Bits:  1    AddModScrBytes(rw) - The NBI Packet Modifier reports number of deleted bytes (for every packet) to the Traffic Manager. 'add_mo_scr_bytes' enables the addition of modification script bytes into deleted byte-count.
    Bits:  0    AddPadBytes(rw) - The NBI Packet Modifier reports number of inserted bytes (for every packet) to the Traffic Manager. 'add_pad_bytes' enables the addition of pad bytes into inserted byte-count.
*/
#define NFP_NBI_PMX_OPCODE_NbiPmCnfg                         0x00000000
#define NFP_NBI_PMX_RDATA_NbiPmCnfg                          0x00000000
#define NFP_NBI_PMX_CSR_NbiPmCnfg                            0x00000000
#define   NFP_NBI_PMX_OPCODE_NbiPmCnfg_AddModScrBytes        (1 << 1)
#define     NFP_NBI_PMX_OPCODE_NbiPmCnfg_AddModScrBytes_0    (0 << 1)
#define     NFP_NBI_PMX_OPCODE_NbiPmCnfg_AddModScrBytes_1    (1 << 1)
#define   NFP_NBI_PMX_OPCODE_NbiPmCnfg_AddPadBytes           (1 << 0)
#define     NFP_NBI_PMX_OPCODE_NbiPmCnfg_AddPadBytes_0       (0 << 0)
#define     NFP_NBI_PMX_OPCODE_NbiPmCnfg_AddPadBytes_1       (1 << 0)
/*---------------------------------------------------------------- 
  Register: NbiPmPktCntHi - Packet Modifier packet counter, High 32-bit
  Register: NbiPmPktCntHi - Packet Modifier packet counter, High 32-bit
  Register: NbiPmPktCntHi - Packet Modifier packet counter, High 32-bit
    Bits: 31:0  NbiPmPktCntHi(rc) - Upper 32-bit of 64-bit Packet Count.
*/
#define NFP_NBI_PMX_OPCODE_NbiPmPktCntHi                     0x00000004
#define NFP_NBI_PMX_RDATA_NbiPmPktCntHi                      0x00000004
#define NFP_NBI_PMX_CSR_NbiPmPktCntHi                        0x00000004
#define   NFP_NBI_PMX_OPCODE_NbiPmPktCntHi_NbiPmPktCntHi(_x) (((_x) & 0xffffffff) << 0)
#define   NFP_NBI_PMX_OPCODE_NbiPmPktCntHi_NbiPmPktCntHi_of(_x) (((_x) >> 0) & 0xffffffff)
/*---------------------------------------------------------------- 
  Register: NbiPmPktCntLo - Packet Modifier packet counter, Low 32-bit
  Register: NbiPmPktCntLo - Packet Modifier packet counter, Low 32-bit
  Register: NbiPmPktCntLo - Packet Modifier packet counter, Low 32-bit
    Bits: 31:0  NbiPmPktCntLo(rc) - Lower 32-bit of 64-bit Packet Count.
*/
#define NFP_NBI_PMX_OPCODE_NbiPmPktCntLo                     0x00000008
#define NFP_NBI_PMX_RDATA_NbiPmPktCntLo                      0x00000008
#define NFP_NBI_PMX_CSR_NbiPmPktCntLo                        0x00000008
#define   NFP_NBI_PMX_OPCODE_NbiPmPktCntLo_NbiPmPktCntLo(_x) (((_x) & 0xffffffff) << 0)
#define   NFP_NBI_PMX_OPCODE_NbiPmPktCntLo_NbiPmPktCntLo_of(_x) (((_x) >> 0) & 0xffffffff)
/*---------------------------------------------------------------- 
  Register: NbiPmModCntHi - Counts number of modifications performed, High 32-bit
  Register: NbiPmModCntHi - Counts number of modifications performed, High 32-bit
  Register: NbiPmModCntHi - Counts number of modifications performed, High 32-bit
    Bits: 31:0  NbiPmModCntHi(rc) - Upper 32-bit of 64-bit Modification Count.
*/
#define NFP_NBI_PMX_OPCODE_NbiPmModCntHi                     0x0000000c
#define NFP_NBI_PMX_RDATA_NbiPmModCntHi                      0x0000000c
#define NFP_NBI_PMX_CSR_NbiPmModCntHi                        0x0000000c
#define   NFP_NBI_PMX_OPCODE_NbiPmModCntHi_NbiPmModCntHi(_x) (((_x) & 0xffffffff) << 0)
#define   NFP_NBI_PMX_OPCODE_NbiPmModCntHi_NbiPmModCntHi_of(_x) (((_x) >> 0) & 0xffffffff)
/*---------------------------------------------------------------- 
  Register: NbiPmModCntLo - Counts number of modifications performed, Low 32-bit
  Register: NbiPmModCntLo - Counts number of modifications performed, Low 32-bit
  Register: NbiPmModCntLo - Counts number of modifications performed, Low 32-bit
    Bits: 31:0  NbiPmModCntLo(rc) - Lower 32-bit of 64-bit Modification Count.
*/
#define NFP_NBI_PMX_OPCODE_NbiPmModCntLo                     0x00000010
#define NFP_NBI_PMX_RDATA_NbiPmModCntLo                      0x00000010
#define NFP_NBI_PMX_CSR_NbiPmModCntLo                        0x00000010
#define   NFP_NBI_PMX_OPCODE_NbiPmModCntLo_NbiPmModCntLo(_x) (((_x) & 0xffffffff) << 0)
#define   NFP_NBI_PMX_OPCODE_NbiPmModCntLo_NbiPmModCntLo_of(_x) (((_x) >> 0) & 0xffffffff)
/*---------------------------------------------------------------- 
  Register: NbiPmPktErrCntHi - Packet Error Count, High 32-bit
  Register: NbiPmPktErrCntHi - Packet Error Count, High 32-bit
  Register: NbiPmPktErrCntHi - Packet Error Count, High 32-bit
    Bits: 31:0  NbiPmPktErrCntHi(rc) - Upper 32-bit of 64-bit Packet Error Count.
*/
#define NFP_NBI_PMX_OPCODE_NbiPmPktErrCntHi                  0x00000014
#define NFP_NBI_PMX_RDATA_NbiPmPktErrCntHi                   0x00000014
#define NFP_NBI_PMX_CSR_NbiPmPktErrCntHi                     0x00000014
#define   NFP_NBI_PMX_OPCODE_NbiPmPktErrCntHi_NbiPmPktErrCntHi(_x) (((_x) & 0xffffffff) << 0)
#define   NFP_NBI_PMX_OPCODE_NbiPmPktErrCntHi_NbiPmPktErrCntHi_of(_x) (((_x) >> 0) & 0xffffffff)
/*---------------------------------------------------------------- 
  Register: NbiPmPktErrCntLo - Packet Error Count, Low 32-bit
  Register: NbiPmPktErrCntLo - Packet Error Count, Low 32-bit
  Register: NbiPmPktErrCntLo - Packet Error Count, Low 32-bit
    Bits: 31:0  NbiPmPktErrCntLo(rc) - Lower 32-bit of 64-bit Packet Error Count.
*/
#define NFP_NBI_PMX_OPCODE_NbiPmPktErrCntLo                  0x00000018
#define NFP_NBI_PMX_RDATA_NbiPmPktErrCntLo                   0x00000018
#define NFP_NBI_PMX_CSR_NbiPmPktErrCntLo                     0x00000018
#define   NFP_NBI_PMX_OPCODE_NbiPmPktErrCntLo_NbiPmPktErrCntLo(_x) (((_x) & 0xffffffff) << 0)
#define   NFP_NBI_PMX_OPCODE_NbiPmPktErrCntLo_NbiPmPktErrCntLo_of(_x) (((_x) >> 0) & 0xffffffff)
/*---------------------------------------------------------------- 
  Register: NbiPmModscrDecodeErrCntHi - Modification Script Decode Error Count, High 32-bit
  Register: NbiPmModscrDecodeErrCntHi - Modification Script Decode Error Count, High 32-bit
  Register: NbiPmModscrDecodeErrCntHi - Modification Script Decode Error Count, High 32-bit
    Bits: 31:0  NbiPmModscrDecodeErrCntHi(rc) - Upper 32-bit of 64-bit Script Decode Error Count.
*/
#define NFP_NBI_PMX_OPCODE_NbiPmModscrDecodeErrCntHi         0x0000001c
#define NFP_NBI_PMX_RDATA_NbiPmModscrDecodeErrCntHi          0x0000001c
#define NFP_NBI_PMX_CSR_NbiPmModscrDecodeErrCntHi            0x0000001c
#define   NFP_NBI_PMX_OPCODE_NbiPmModscrDecodeErrCntHi_NbiPmModscrDecodeErrCntHi(_x) (((_x) & 0xffffffff) << 0)
#define   NFP_NBI_PMX_OPCODE_NbiPmModscrDecodeErrCntHi_NbiPmModscrDecodeErrCntHi_of(_x) (((_x) >> 0) & 0xffffffff)
/*---------------------------------------------------------------- 
  Register: NbiPmModscrDecodeErrCntLo - Modification Script Decode Error Count, Low 32-bit
  Register: NbiPmModscrDecodeErrCntLo - Modification Script Decode Error Count, Low 32-bit
  Register: NbiPmModscrDecodeErrCntLo - Modification Script Decode Error Count, Low 32-bit
    Bits: 31:0  NbiPmModscrDecodeErrCntLo(rw) - lower 32-bit of 64-bit Script Decode Error Count.
*/
#define NFP_NBI_PMX_OPCODE_NbiPmModscrDecodeErrCntLo         0x00000020
#define NFP_NBI_PMX_RDATA_NbiPmModscrDecodeErrCntLo          0x00000020
#define NFP_NBI_PMX_CSR_NbiPmModscrDecodeErrCntLo            0x00000020
#define   NFP_NBI_PMX_OPCODE_NbiPmModscrDecodeErrCntLo_NbiPmModscrDecodeErrCntLo(_x) (((_x) & 0xffffffff) << 0)
#define   NFP_NBI_PMX_OPCODE_NbiPmModscrDecodeErrCntLo_NbiPmModscrDecodeErrCntLo_of(_x) (((_x) >> 0) & 0xffffffff)

#endif /* NFP6000_LONGNAMES */


#endif /* NFP6000_NBI_PM_H */
