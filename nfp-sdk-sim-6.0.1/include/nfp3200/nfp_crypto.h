/*
 * Copyright (C) 2012, Netronome Systems, Inc.  All rights reserved.
 *
 * Disclaimer: this file is provided without any warranty
 * as part of an early-access program, and the content is
 * bound to change before the final release.
 */

#ifndef NFP3200_NFP_CRYPTO_H
#define NFP3200_NFP_CRYPTO_H

/* HGID: nfp3200/crypto_cif.desc = ade0020e9035 */
/* Register Type: CIFConfig */
#define NFP_CRYPTO_CIF_CFG             0x0000
/* Register Type: CIFErrFrst1 */
#define NFP_CRYPTO_CIF_FERR1           0x0010
#define   NFP_CRYPTO_CIF_FERR1_CDS_BLK_UNDFLW           (0x1 << 31)
#define   NFP_CRYPTO_CIF_FERR1_CDS_BLK_OVRFLW           (0x1 << 30)
#define   NFP_CRYPTO_CIF_FERR1_CDS_PKI_UNDFLW           (0x1 << 29)
#define   NFP_CRYPTO_CIF_FERR1_CDS_PKI_OVRFLW           (0x1 << 28)
#define   NFP_CRYPTO_CIF_FERR1_CDS_DMA_UNDFLW           (0x1 << 27)
#define   NFP_CRYPTO_CIF_FERR1_CDS_DMA_OVRFLW           (0x1 << 26)
#define   NFP_CRYPTO_CIF_FERR1_CDS_CBG_UNDFLW           (0x1 << 25)
#define   NFP_CRYPTO_CIF_FERR1_CDS_CBG_OVRFLW           (0x1 << 24)
#define   NFP_CRYPTO_CIF_FERR1_CDS_EVT_UNDFLW           (0x1 << 23)
#define   NFP_CRYPTO_CIF_FERR1_CDS_EVT_OVRFLW           (0x1 << 22)
#define   NFP_CRYPTO_CIF_FERR1_CDS_PRE_UNDFLW           (0x1 << 21)
#define   NFP_CRYPTO_CIF_FERR1_CDS_PRE_OVRFLW           (0x1 << 20)
#define   NFP_CRYPTO_CIF_FERR1_CDS_SGL_UNDFLW           (0x1 << 19)
#define   NFP_CRYPTO_CIF_FERR1_CDS_SGL_OVRFLW           (0x1 << 18)
#define   NFP_CRYPTO_CIF_FERR1_CBG_GRD_UNDFLW           (0x1 << 17)
#define   NFP_CRYPTO_CIF_FERR1_CBG_GRD_OVRFLW           (0x1 << 16)
#define   NFP_CRYPTO_CIF_FERR1_CBG_GPKI_UNDFLW          (0x1 << 15)
#define   NFP_CRYPTO_CIF_FERR1_CBG_GPKI_OVRFLW          (0x1 << 14)
#define   NFP_CRYPTO_CIF_FERR1_CBG_GBLK_UNDFLW          (0x1 << 13)
#define   NFP_CRYPTO_CIF_FERR1_CBG_GBLK_OVRFLW          (0x1 << 12)
#define   NFP_CRYPTO_CIF_FERR1_CBG_GCPP_UNDFLW          (0x1 << 11)
#define   NFP_CRYPTO_CIF_FERR1_CBG_GCPP_OVRFLW          (0x1 << 10)
#define   NFP_CRYPTO_CIF_FERR1_CBG_GDSP_UNDFLW          (0x1 << 9)
#define   NFP_CRYPTO_CIF_FERR1_CBG_GDSP_OVRFLW          (0x1 << 8)
#define   NFP_CRYPTO_CIF_FERR1_CDC_PKI_UNDFLW           (0x1 << 7)
#define   NFP_CRYPTO_CIF_FERR1_CDC_PKI_OVRFLW           (0x1 << 6)
#define   NFP_CRYPTO_CIF_FERR1_CDC_DSP_UNDFLW           (0x1 << 5)
#define   NFP_CRYPTO_CIF_FERR1_CDC_DSP_OVRFLW           (0x1 << 4)
#define   NFP_CRYPTO_CIF_FERR1_CPM_PUSH_UNDFLW          (0x1 << 3)
#define   NFP_CRYPTO_CIF_FERR1_CPM_PUSH_OVRFLW          (0x1 << 2)
#define   NFP_CRYPTO_CIF_FERR1_CPM_PULL_UNDFLW          (0x1 << 1)
#define   NFP_CRYPTO_CIF_FERR1_CPM_PULL_OVRFLW          (0x1)
/* Register Type: CIFErrFrst2 */
#define NFP_CRYPTO_CIF_FERR2           0x0014
#define   NFP_CRYPTO_CIF_FERR2_CTX_UNDFLW_of(_x)        (((_x) >> 16) & 0xffff)
#define   NFP_CRYPTO_CIF_FERR2_CTX_OVRFLW_of(_x)        ((_x) & 0xffff)
/* Register Type: CIFErrFrst3 */
#define NFP_CRYPTO_CIF_FERR3           0x0018
#define   NFP_CRYPTO_CIF_FERR3_BLK_SYNC_ERR             (0x1 << 11)
#define   NFP_CRYPTO_CIF_FERR3_DEC_ERR                  (0x1 << 10)
#define   NFP_CRYPTO_CIF_FERR3_CPP_PS_UNDFLW            (0x1 << 9)
#define   NFP_CRYPTO_CIF_FERR3_CPP_PS_OVRFLW            (0x1 << 8)
#define   NFP_CRYPTO_CIF_FERR3_CPP_DN_UNDFLW            (0x1 << 7)
#define   NFP_CRYPTO_CIF_FERR3_CPP_DN_OVRFLW            (0x1 << 6)
#define   NFP_CRYPTO_CIF_FERR3_CPP_PH_UNDFLW            (0x1 << 5)
#define   NFP_CRYPTO_CIF_FERR3_CPP_PH_OVRFLW            (0x1 << 4)
#define   NFP_CRYPTO_CIF_FERR3_CPP_DTA_ERR              (0x1 << 3)
#define   NFP_CRYPTO_CIF_FERR3_CMD_ERR                  (0x1 << 2)
#define   NFP_CRYPTO_CIF_FERR3_CBG_ERR_of(_x)           ((_x) & 0x3)
/* Register Type: CIFErr1 */
#define NFP_CRYPTO_CIF_ERR1            0x0020
#define   NFP_CRYPTO_CIF_ERR1_CDS_ERR_of(_x)            (((_x) >> 18) & 0x3fff)
#define   NFP_CRYPTO_CIF_ERR1_CBG_ERR_of(_x)            (((_x) >> 8) & 0x3ff)
#define   NFP_CRYPTO_CIF_ERR1_CDC_ERR_of(_x)            (((_x) >> 4) & 0xf)
#define   NFP_CRYPTO_CIF_ERR1_CPM_ERR_of(_x)            ((_x) & 0xf)
/* Register Type: CIFErr2 */
#define NFP_CRYPTO_CIF_ERR2            0x0024
#define   NFP_CRYPTO_CIF_ERR2_CTX_UNDFLW_of(_x)         (((_x) >> 16) & 0xffff)
#define   NFP_CRYPTO_CIF_ERR2_CTX_OVRFLW_of(_x)         ((_x) & 0xffff)
/* Register Type: CIFErr3 */
#define NFP_CRYPTO_CIF_ERR3            0x0028
#define   NFP_CRYPTO_CIF_ERR3_BLK_SYNC_ERR              (0x1 << 11)
#define   NFP_CRYPTO_CIF_ERR3_DEC_ERR                   (0x1 << 10)
#define   NFP_CRYPTO_CIF_ERR3_CPP_PS_UNDFLW             (0x1 << 9)
#define   NFP_CRYPTO_CIF_ERR3_CPP_PS_OVRFLW             (0x1 << 8)
#define   NFP_CRYPTO_CIF_ERR3_CPP_DN_UNDFLW             (0x1 << 7)
#define   NFP_CRYPTO_CIF_ERR3_CPP_DN_OVRFLW             (0x1 << 6)
#define   NFP_CRYPTO_CIF_ERR3_CPP_PH_UNDFLW             (0x1 << 5)
#define   NFP_CRYPTO_CIF_ERR3_CPP_PH_OVRFLW             (0x1 << 4)
#define   NFP_CRYPTO_CIF_ERR3_CPP_DTA_ERR               (0x1 << 3)
#define   NFP_CRYPTO_CIF_ERR3_CMD_ERR                   (0x1 << 2)
#define   NFP_CRYPTO_CIF_ERR3_CBG_ERR_of(_x)            ((_x) & 0x3)
/* Register Type: CIFErrMask1 */
#define NFP_CRYPTO_CIF_ERRMASK1        0x0030
#define   NFP_CRYPTO_CIF_ERRMASK1_CDS_ERR_of(_x)        (((_x) >> 18) & 0x3fff)
#define   NFP_CRYPTO_CIF_ERRMASK1_CBG_ERR_of(_x)        (((_x) >> 8) & 0x3ff)
#define   NFP_CRYPTO_CIF_ERRMASK1_CDC_ERR_of(_x)        (((_x) >> 4) & 0xf)
#define   NFP_CRYPTO_CIF_ERRMASK1_CPM_ERR_of(_x)        ((_x) & 0xf)
/* Register Type: CIFErrMask2 */
#define NFP_CRYPTO_CIF_ERRMASK2        0x0034
#define   NFP_CRYPTO_CIF_ERRMASK2_CTX_UNDFLW_of(_x)     (((_x) >> 16) & 0xffff)
#define   NFP_CRYPTO_CIF_ERRMASK2_CTX_OVRFLW_of(_x)     ((_x) & 0xffff)
/* Register Type: CIFErrMask3 */
#define NFP_CRYPTO_CIF_ERRMASK3        0x0038
#define   NFP_CRYPTO_CIF_ERRMASK3_BLK_SYNC_ERR          (0x1 << 11)
#define   NFP_CRYPTO_CIF_ERRMASK3_DEC_ERR               (0x1 << 10)
#define   NFP_CRYPTO_CIF_ERRMASK3_CPP_PS_UNDFLW         (0x1 << 9)
#define   NFP_CRYPTO_CIF_ERRMASK3_CPP_PS_OVRFLW         (0x1 << 8)
#define   NFP_CRYPTO_CIF_ERRMASK3_CPP_DN_UNDFLW         (0x1 << 7)
#define   NFP_CRYPTO_CIF_ERRMASK3_CPP_DN_OVRFLW         (0x1 << 6)
#define   NFP_CRYPTO_CIF_ERRMASK3_CPP_PH_UNDFLW         (0x1 << 5)
#define   NFP_CRYPTO_CIF_ERRMASK3_CPP_PH_OVRFLW         (0x1 << 4)
#define   NFP_CRYPTO_CIF_ERRMASK3_CPP_DTA_ERR           (0x1 << 3)
#define   NFP_CRYPTO_CIF_ERRMASK3_CMD_ERR               (0x1 << 2)
#define   NFP_CRYPTO_CIF_ERRMASK3_CBG_ERR_of(_x)        ((_x) & 0x3)
/* Register Type: CIFErrClear1 */
#define NFP_CRYPTO_CIF_ERRCLR1         0x0040
#define   NFP_CRYPTO_CIF_ERRCLR1_CDS_ERR_of(_x)         (((_x) >> 18) & 0x3fff)
#define   NFP_CRYPTO_CIF_ERRCLR1_CBG_ERR_of(_x)         (((_x) >> 8) & 0x3ff)
#define   NFP_CRYPTO_CIF_ERRCLR1_CDC_ERR_of(_x)         (((_x) >> 4) & 0xf)
#define   NFP_CRYPTO_CIF_ERRCLR1_CPM_ERR_of(_x)         ((_x) & 0xf)
/* Register Type: CIFErrClear2 */
#define NFP_CRYPTO_CIF_ERRCLR2         0x0044
#define   NFP_CRYPTO_CIF_ERRCLR2_CTX_UNDFLW(_x)         (((_x) & 0xffff) << 16)
#define   NFP_CRYPTO_CIF_ERRCLR2_CTX_UNDFLW_of(_x)      (((_x) >> 16) & 0xffff)
#define   NFP_CRYPTO_CIF_ERRCLR2_CTX_OVRFLW(_x)         ((_x) & 0xffff)
#define   NFP_CRYPTO_CIF_ERRCLR2_CTX_OVRFLW_of(_x)      ((_x) & 0xffff)
/* Register Type: CIFErrClear3 */
#define NFP_CRYPTO_CIF_ERRCLR3         0x0048
#define   NFP_CRYPTO_CIF_ERRCLR3_BLK_SYNC_ERR           (0x1 << 11)
#define   NFP_CRYPTO_CIF_ERRCLR3_DEC_ERR                (0x1 << 10)
#define   NFP_CRYPTO_CIF_ERRCLR3_CPP_PS_UNDFLW          (0x1 << 9)
#define   NFP_CRYPTO_CIF_ERRCLR3_CPP_PS_OVRFLW          (0x1 << 8)
#define   NFP_CRYPTO_CIF_ERRCLR3_CPP_DN_UNDFLW          (0x1 << 7)
#define   NFP_CRYPTO_CIF_ERRCLR3_CPP_DN_OVRFLW          (0x1 << 6)
#define   NFP_CRYPTO_CIF_ERRCLR3_CPP_PH_UNDFLW          (0x1 << 5)
#define   NFP_CRYPTO_CIF_ERRCLR3_CPP_PH_OVRFLW          (0x1 << 4)
#define   NFP_CRYPTO_CIF_ERRCLR3_CPP_DTA_ERR            (0x1 << 3)
#define   NFP_CRYPTO_CIF_ERRCLR3_CMD_ERR                (0x1 << 2)
#define   NFP_CRYPTO_CIF_ERRCLR3_CTX_ERR(_x)            ((_x) & 0x3)
#define   NFP_CRYPTO_CIF_ERRCLR3_CTX_ERR_of(_x)         ((_x) & 0x3)
/* Register Type: CIFPerfSel */
#define NFP_CRYPTO_CIF_PERFSEL         0x0050
#define   NFP_CRYPTO_CIF_PERFSEL_SELECT(_x)             ((_x) & 0xf)
#define   NFP_CRYPTO_CIF_PERFSEL_SELECT_of(_x)          ((_x) & 0xf)
#define     NFP_CRYPTO_CIF_PERFSEL_SELECT_NONE          (0)
#define     NFP_CRYPTO_CIF_PERFSEL_SELECT_FIFO          (1)
#define     NFP_CRYPTO_CIF_PERFSEL_SELECT_DISP          (2)
#define     NFP_CRYPTO_CIF_PERFSEL_SELECT_BUFF          (3)
#define     NFP_CRYPTO_CIF_PERFSEL_SELECT_CPPM          (4)
#define     NFP_CRYPTO_CIF_PERFSEL_SELECT_CPPT          (5)
/* Register Type: CIFDbgRdCtrl */
#define NFP_CRYPTO_CIF_DBGRC           0x0054
#define   NFP_CRYPTO_CIF_DBGRC_BUSY                     (0x1 << 31)
#define     NFP_CRYPTO_CIF_DBGRC_BUSY_READING           (0x80000000)
#define     NFP_CRYPTO_CIF_DBGRC_BUSY_READ_COMPLETE     (0x0)
#define   NFP_CRYPTO_CIF_DBGRC_SRC                      (0x1 << 16)
#define     NFP_CRYPTO_CIF_DBGRC_SRC_DISP               (0x0)
#define     NFP_CRYPTO_CIF_DBGRC_SRC_DMAC               (0x10000)
#define   NFP_CRYPTO_CIF_DBGRC_CNT(_x)                  ((_x) & 0xff)
#define   NFP_CRYPTO_CIF_DBGRC_CNT_of(_x)               ((_x) & 0xff)
/* Register Type: CIFDbgRdAddr */
#define NFP_CRYPTO_CIF_DBGRA           0x0058
#define   NFP_CRYPTO_CIF_DBGRA_CRYPTO_ADDR(_x)          (((_x) & 0x1fff) << 16)
#define   NFP_CRYPTO_CIF_DBGRA_CRYPTO_ADDR_of(_x)       (((_x) >> 16) & 0x1fff)
#define   NFP_CRYPTO_CIF_DBGRA_CTX_INC                  (0x1 << 15)
#define     NFP_CRYPTO_CIF_DBGRA_CTX_INC_ENTRY          (0x0)
#define     NFP_CRYPTO_CIF_DBGRA_CTX_INC_FULL           (0x8000)
#define   NFP_CRYPTO_CIF_DBGRA_SRC_ADDR(_x)             ((_x) & 0xff)
#define   NFP_CRYPTO_CIF_DBGRA_SRC_ADDR_of(_x)          ((_x) & 0xff)
/* Register Type: CIFDalBstRun */
#define NFP_CRYPTO_CIF_DALBSTRN        0x005c
#define   NFP_CRYPTO_CIF_DALBSTRN_RUN(_x)               ((_x) & 0x3)
#define   NFP_CRYPTO_CIF_DALBSTRN_RUN_of(_x)            ((_x) & 0x3)
/* Register Type: CIFDalBstSt */
#define NFP_CRYPTO_CIF_DALBSTST        0x0060
#define   NFP_CRYPTO_CIF_DALBSTST_DONE(_x)              (((_x) & 0xf) << 4)
#define   NFP_CRYPTO_CIF_DALBSTST_DONE_of(_x)           (((_x) >> 4) & 0xf)
#define   NFP_CRYPTO_CIF_DALBSTST_FAIL(_x)              ((_x) & 0xf)
#define   NFP_CRYPTO_CIF_DALBSTST_FAIL_of(_x)           ((_x) & 0xf)

/* HGID: nfp3200/crypto_pki.desc = ade0020e9035 */
/* Register Type: MpuTmoutCtrl */
#define NFP_CRYPTO_PKI_TIMECTRL        0x0000
#define   NFP_CRYPTO_PKI_TIMECTRL_ENABLE_TIMEOUT        (0x1 << 16)
#define   NFP_CRYPTO_PKI_TIMECTRL_TIMEOUT_LIMIT(_x)     ((_x) & 0xffff)
#define   NFP_CRYPTO_PKI_TIMECTRL_TIMEOUT_LIMIT_of(_x)  ((_x) & 0xffff)
/* Register Type: MpuTmdOutMsk */
#define NFP_CRYPTO_PKI_TIMEDOUT        0x0004
#define   NFP_CRYPTO_PKI_TIMEDOUT_TIMED_OUT(_x)         ((_x) & 0xfff)
#define   NFP_CRYPTO_PKI_TIMEDOUT_TIMED_OUT_of(_x)      ((_x) & 0xfff)
/* Register Type: MpuExcldMsk */
#define NFP_CRYPTO_PKI_EXCLUDE         0x0008
#define   NFP_CRYPTO_PKI_EXCLUDE_EXCLUDED(_x)           ((_x) & 0xfff)
#define   NFP_CRYPTO_PKI_EXCLUDE_EXCLUDED_of(_x)        ((_x) & 0xfff)
/* Register Type: MpuRestart */
#define NFP_CRYPTO_PKI_RESTART         0x000c
#define   NFP_CRYPTO_PKI_RESTART_RESTART(_x)            ((_x) & 0xfff)
#define   NFP_CRYPTO_PKI_RESTART_RESTART_of(_x)         ((_x) & 0xfff)
/* Register Type: MpuBusy */
#define NFP_CRYPTO_PKI_BUSY            0x0010
#define   NFP_CRYPTO_PKI_BUSY_SLOT_CNT_of(_x)           (((_x) >> 24) & 0x1f)
#define   NFP_CRYPTO_PKI_BUSY_FREE_of(_x)               (((_x) >> 12) & 0xfff)
#define   NFP_CRYPTO_PKI_BUSY_BUSY_of(_x)               ((_x) & 0xfff)
/* Register Type: DbgMpuIndx */
#define NFP_CRYPTO_PKI_DBG             0x0014
#define   NFP_CRYPTO_PKI_DBG_BLK_IRM_FAULTS             (0x1 << 17)
#define   NFP_CRYPTO_PKI_DBG_HALT_FAULT                 (0x1 << 16)
#define   NFP_CRYPTO_PKI_DBG_PERFBUS_SELECT(_x)         (((_x) & 0xff) << 8)
#define   NFP_CRYPTO_PKI_DBG_PERFBUS_SELECT_of(_x)      (((_x) >> 8) & 0xff)
#define   NFP_CRYPTO_PKI_DBG_ENABLE_IDX                 (0x1 << 7)
#define   NFP_CRYPTO_PKI_DBG_DISABLE_POISON_CB          (0x1 << 6)
#define   NFP_CRYPTO_PKI_DBG_DISABLE_DST_SCRUBBING      (0x1 << 5)
#define   NFP_CRYPTO_PKI_DBG_DISABLE_MPU_SCRUBBING      (0x1 << 4)
#define   NFP_CRYPTO_PKI_DBG_DBG_IDX(_x)                ((_x) & 0xf)
#define   NFP_CRYPTO_PKI_DBG_DBG_IDX_of(_x)             ((_x) & 0xf)
/* Register Type: PrgmCntr */
#define NFP_CRYPTO_PKI_PROGCNTR        0x0018
#define   NFP_CRYPTO_PKI_PROGCNTR_MEM_FAULY_of(_x)      (((_x) >> 23) & 0x1ff)
#define   NFP_CRYPTO_PKI_PROGCNTR_PROG_COUNT_of(_x)     ((_x) & 0x1ff)
/* Register Type: CurrInstr */
#define NFP_CRYPTO_PKI_INSTR           0x001c
#define   NFP_CRYPTO_PKI_INSTR_CURR_INSTR_of(_x)        (_x)
/* Register Type: MpuMemPkCtrl */
#define NFP_CRYPTO_PKI_MPUCTRL         0x0020
#define   NFP_CRYPTO_PKI_MPUCTRL_START_PEEK             (0x1 << 31)
#define     NFP_CRYPTO_PKI_MPUCTRL_START_PEEK_IN_PROGRESS (0x80000000)
#define     NFP_CRYPTO_PKI_MPUCTRL_START_PEEK_DONE      (0x0)
#define   NFP_CRYPTO_PKI_MPUCTRL_UNCORR_ERR             (0x1 << 25)
#define   NFP_CRYPTO_PKI_MPUCTRL_CORR_ERR               (0x1 << 24)
#define   NFP_CRYPTO_PKI_MPUCTRL_PEEK_ADDR(_x)          ((_x) & 0x1ff)
#define   NFP_CRYPTO_PKI_MPUCTRL_PEEK_ADDR_of(_x)       ((_x) & 0x1ff)
/* Register Type: MpuMemPkData */
#define NFP_CRYPTO_PKI_MPUDATA         0x0024
#define   NFP_CRYPTO_PKI_MPUDATA_PEEK_DATA_of(_x)       (_x)
/* Register Type: DstTblPkCtrl */
#define NFP_CRYPTO_PKI_DISTCTRL        0x0028
#define   NFP_CRYPTO_PKI_DISTCTRL_START_PEEK            (0x1 << 31)
#define     NFP_CRYPTO_PKI_DISTCTRL_START_PEEK_IN_PROGRESS (0x80000000)
#define     NFP_CRYPTO_PKI_DISTCTRL_START_PEEK_DONE     (0x0)
#define   NFP_CRYPTO_PKI_DISTCTRL_UNCORR_ERR            (0x1 << 25)
#define   NFP_CRYPTO_PKI_DISTCTRL_CORR_ERR              (0x1 << 24)
#define   NFP_CRYPTO_PKI_DISTCTRL_PEEK_ADDR(_x)         ((_x) & 0x3ff)
#define   NFP_CRYPTO_PKI_DISTCTRL_PEEK_ADDR_of(_x)      ((_x) & 0x3ff)
/* Register Type: DstTblPkDat0 */
#define NFP_CRYPTO_PKI_DISTDATA0       0x002c
#define   NFP_CRYPTO_PKI_DISTDATA0_DATA_of(_x)          (_x)
/* Register Type: DstTblPkDat1 */
#define NFP_CRYPTO_PKI_DISTDATA1       0x0030
#define   NFP_CRYPTO_PKI_DISTDATA1_DATA_of(_x)          (_x)
/* Register Type: PwrsavDis */
#define NFP_CRYPTO_PKI_POWER           0x0034
#define   NFP_CRYPTO_PKI_POWER_DISABLE_POWER_SAVE(_x)   ((_x) & 0xfff)
#define   NFP_CRYPTO_PKI_POWER_DISABLE_POWER_SAVE_of(_x) ((_x) & 0xfff)
/* Register Type: PkiErrFrst1 */
#define NFP_CRYPTO_PKI_FERR1           0x0040
#define   NFP_CRYPTO_PKI_FERR1_UNCORR_MEM               (0x1 << 31)
#define   NFP_CRYPTO_PKI_FERR1_REL_FREE                 (0x1 << 30)
#define   NFP_CRYPTO_PKI_FERR1_ALLOC_NOT_FREE           (0x1 << 29)
#define   NFP_CRYPTO_PKI_FERR1_CLEANUP_NOT_DONE         (0x1 << 28)
#define   NFP_CRYPTO_PKI_FERR1_POP_DONE                 (0x1 << 27)
#define   NFP_CRYPTO_PKI_FERR1_PATH_CNT_UNDFLW          (0x1 << 26)
#define   NFP_CRYPTO_PKI_FERR1_PATH_CNT_OVRFLW          (0x1 << 25)
#define   NFP_CRYPTO_PKI_FERR1_XFER_NOREQ               (0x1 << 23)
#define   NFP_CRYPTO_PKI_FERR1_BAD_PKI_CBG              (0x1 << 21)
#define   NFP_CRYPTO_PKI_FERR1_ACK_NOREQ                (0x1 << 20)
#define   NFP_CRYPTO_PKI_FERR1_RMT_EXEC_NORET           (0x1 << 19)
#define   NFP_CRYPTO_PKI_FERR1_RMT_EXEC_NOEXE           (0x1 << 18)
#define   NFP_CRYPTO_PKI_FERR1_HEAD_GT_TAIL             (0x1 << 17)
#define   NFP_CRYPTO_PKI_FERR1_HEAD_OVRFLW              (0x1 << 16)
#define   NFP_CRYPTO_PKI_FERR1_TAIL_OVERFLW             (0x1 << 15)
#define   NFP_CRYPTO_PKI_FERR1_UNDEF_RMT_INSTR          (0x1 << 14)
#define   NFP_CRYPTO_PKI_FERR1_UNDEF_INSTR              (0x1 << 13)
#define   NFP_CRYPTO_PKI_FERR1_MPU_ASSIGN_ERR           (0x1 << 12)
#define   NFP_CRYPTO_PKI_FERR1_CDS_REP_UNDFLW           (0x1 << 11)
#define   NFP_CRYPTO_PKI_FERR1_CDS_REP_OVRFLW           (0x1 << 10)
#define   NFP_CRYPTO_PKI_FERR1_DMA_REQ_UNDFLW           (0x1 << 9)
#define   NFP_CRYPTO_PKI_FERR1_DMA_REQ_OVRFLW           (0x1 << 8)
#define   NFP_CRYPTO_PKI_FERR1_SIG_REQ_UNDFLW           (0x1 << 7)
#define   NFP_CRYPTO_PKI_FERR1_SIG_REQ_OVRFLW           (0x1 << 6)
#define   NFP_CRYPTO_PKI_FERR1_EVT_REQ_UNDFLW           (0x1 << 5)
#define   NFP_CRYPTO_PKI_FERR1_EVT_REQ_OVRFLW           (0x1 << 4)
#define   NFP_CRYPTO_PKI_FERR1_PROG_UNDFLW              (0x1 << 3)
#define   NFP_CRYPTO_PKI_FERR1_PROG_OVRFLW              (0x1 << 2)
#define   NFP_CRYPTO_PKI_FERR1_CBD_UNDFLW               (0x1 << 1)
#define   NFP_CRYPTO_PKI_FERR1_CBD_OVRFLW               (0x1)
/* Register Type: PkiErr1 */
#define NFP_CRYPTO_PKI_ERR1            0x0044
#define   NFP_CRYPTO_PKI_ERR1_DIST_ERR_of(_x)           (_x)
/* Register Type: PkiErrMsk1 */
#define NFP_CRYPTO_PKI_ERRMASK1        0x0048
#define   NFP_CRYPTO_PKI_ERRMASK1_DIST_ERR_MASK(_x)     (_x)
#define   NFP_CRYPTO_PKI_ERRMASK1_DIST_ERR_MASK_of(_x)  (_x)
/* Register Type: PkiErrClr1 */
#define NFP_CRYPTO_PKI_ERRCLR1         0x004c
#define   NFP_CRYPTO_PKI_ERRCLR1_DIST_ERR_CLR(_x)       (_x)
#define   NFP_CRYPTO_PKI_ERRCLR1_DIST_ERR_CLR_of(_x)    (_x)
/* Register Type: BistRun */
#define NFP_CRYPTO_PKI_BISTRUN         0x0050
#define   NFP_CRYPTO_PKI_BISTRUN_BIST_RUN               (0x1)
/* Register Type: BistStatus */
#define NFP_CRYPTO_PKI_BISTSTAT        0x0054
#define   NFP_CRYPTO_PKI_BISTSTAT_BIST_DONE1            (0x1 << 3)
#define   NFP_CRYPTO_PKI_BISTSTAT_BIST_DONE0            (0x1 << 2)
#define   NFP_CRYPTO_PKI_BISTSTAT_BIST_FL_DONE1         (0x1 << 1)
#define   NFP_CRYPTO_PKI_BISTSTAT_BIST_FL_DONE0         (0x1)

/* HGID: nfp3200/crypto_bulk.desc = ade0020e9035 */
/* Register Type: BasicConfig */
#define NFP_CRYPTO_BULK_CFG            0x0000
#define   NFP_CRYPTO_BULK_CFG_DES_MCP(_x)               (((_x) & 0x3) << 4)
#define   NFP_CRYPTO_BULK_CFG_DES_MCP_of(_x)            (((_x) >> 4) & 0x3)
#define     NFP_CRYPTO_BULK_CFG_DES_MCP_5X              (0)
#define     NFP_CRYPTO_BULK_CFG_DES_MCP_5               (1)
#define     NFP_CRYPTO_BULK_CFG_DES_MCP_4               (2)
#define     NFP_CRYPTO_BULK_CFG_DES_MCP_6               (3)
#define   NFP_CRYPTO_BULK_CFG_ENABLE(_x)                ((_x) & 0xf)
#define   NFP_CRYPTO_BULK_CFG_ENABLE_of(_x)             ((_x) & 0xf)
/* Register Type: PerfBusSel */
#define NFP_CRYPTO_BULK_PERF_BUS_SEL   0x0004
#define   NFP_CRYPTO_BULK_PERF_BUS_SEL_HI_BLK(_x)       (((_x) & 0xf) << 20)
#define   NFP_CRYPTO_BULK_PERF_BUS_SEL_HI_BLK_of(_x)    (((_x) >> 20) & 0xf)
#define     NFP_CRYPTO_BULK_PERF_BUS_SEL_HI_BLK_DIRECTOR (8)
#define     NFP_CRYPTO_BULK_PERF_BUS_SEL_HI_BLK_ENG1    (12)
#define     NFP_CRYPTO_BULK_PERF_BUS_SEL_HI_BLK_ENG2    (13)
#define     NFP_CRYPTO_BULK_PERF_BUS_SEL_HI_BLK_ENG3    (14)
#define     NFP_CRYPTO_BULK_PERF_BUS_SEL_HI_BLK_ENG4    (15)
#define   NFP_CRYPTO_BULK_PERF_BUS_SEL_MID_BLK(_x)      (((_x) & 0xf) << 16)
#define   NFP_CRYPTO_BULK_PERF_BUS_SEL_MID_BLK_of(_x)   (((_x) >> 16) & 0xf)
#define     NFP_CRYPTO_BULK_PERF_BUS_SEL_MID_BLK_DIRECTOR (8)
#define     NFP_CRYPTO_BULK_PERF_BUS_SEL_MID_BLK_ENG0   (12)
#define     NFP_CRYPTO_BULK_PERF_BUS_SEL_MID_BLK_ENG1   (13)
#define     NFP_CRYPTO_BULK_PERF_BUS_SEL_MID_BLK_ENG2   (14)
#define     NFP_CRYPTO_BULK_PERF_BUS_SEL_MID_BLK_ENG3   (15)
#define   NFP_CRYPTO_BULK_PERF_BUS_SEL_LO_BLK(_x)       (((_x) & 0xf) << 12)
#define   NFP_CRYPTO_BULK_PERF_BUS_SEL_LO_BLK_of(_x)    (((_x) >> 12) & 0xf)
#define     NFP_CRYPTO_BULK_PERF_BUS_SEL_LO_BLK_DIRECTOR (8)
#define     NFP_CRYPTO_BULK_PERF_BUS_SEL_LO_BLK_ENG0    (12)
#define     NFP_CRYPTO_BULK_PERF_BUS_SEL_LO_BLK_ENG1    (13)
#define     NFP_CRYPTO_BULK_PERF_BUS_SEL_LO_BLK_ENG2    (14)
#define     NFP_CRYPTO_BULK_PERF_BUS_SEL_LO_BLK_ENG3    (15)
#define   NFP_CRYPTO_BULK_PERF_BUS_SEL_HI_SEL(_x)       (((_x) & 0xf) << 8)
#define   NFP_CRYPTO_BULK_PERF_BUS_SEL_HI_SEL_of(_x)    (((_x) >> 8) & 0xf)
#define   NFP_CRYPTO_BULK_PERF_BUS_SEL_MID_SEL(_x)      (((_x) & 0xf) << 4)
#define   NFP_CRYPTO_BULK_PERF_BUS_SEL_MID_SEL_of(_x)   (((_x) >> 4) & 0xf)
#define   NFP_CRYPTO_BULK_PERF_BUS_SEL_LO_SEL(_x)       ((_x) & 0xf)
#define   NFP_CRYPTO_BULK_PERF_BUS_SEL_LO_SEL_of(_x)    ((_x) & 0xf)
/* Register Type: WtchDgTmrCnf */
#define NFP_CRYPTO_BULK_WATCHDOG_TIMER_CFG 0x0008
#define   NFP_CRYPTO_BULK_WATCHDOG_TIMER_CFG_ENABLE     (0x1 << 30)
#define   NFP_CRYPTO_BULK_WATCHDOG_TIMER_CFG_SIZE(_x)   (((_x) & 0x3) << 28)
#define   NFP_CRYPTO_BULK_WATCHDOG_TIMER_CFG_SIZE_of(_x) (((_x) >> 28) & 0x3)
#define     NFP_CRYPTO_BULK_WATCHDOG_TIMER_CFG_SIZE_28  (0)
#define     NFP_CRYPTO_BULK_WATCHDOG_TIMER_CFG_SIZE_32  (1)
#define     NFP_CRYPTO_BULK_WATCHDOG_TIMER_CFG_SIZE_36  (2)
#define     NFP_CRYPTO_BULK_WATCHDOG_TIMER_CFG_SIZE_40  (3)
#define   NFP_CRYPTO_BULK_WATCHDOG_TIMER_CFG_TRIP(_x)   ((_x) & 0xfffffff)
#define   NFP_CRYPTO_BULK_WATCHDOG_TIMER_CFG_TRIP_of(_x) ((_x) & 0xfffffff)
/* Register Type: ErrorReg */
#define NFP_CRYPTO_BULK_ERR            0x0010
#define   NFP_CRYPTO_BULK_ERR_ENG3                      (0x1 << 22)
#define   NFP_CRYPTO_BULK_ERR_ENG2                      (0x1 << 21)
#define   NFP_CRYPTO_BULK_ERR_ENG1                      (0x1 << 20)
#define   NFP_CRYPTO_BULK_ERR_ENG0                      (0x1 << 19)
#define   NFP_CRYPTO_BULK_ERR_SEQNUM                    (0x1 << 18)
#define   NFP_CRYPTO_BULK_ERR_FIFO_READY                (0x1 << 17)
#define   NFP_CRYPTO_BULK_ERR_FIFO_PARITY               (0x1 << 16)
#define   NFP_CRYPTO_BULK_ERR_FIFO_OVRFLW               (0x1 << 15)
#define   NFP_CRYPTO_BULK_ERR_WRT_FIFO_PARITY           (0x1 << 14)
#define   NFP_CRYPTO_BULK_ERR_WRT_FIFO_OVRFLW           (0x1 << 13)
#define   NFP_CRYPTO_BULK_ERR_WRT_ARB                   (0x1 << 12)
#define   NFP_CRYPTO_BULK_ERR_ALLOC_WATCHDOG3           (0x1 << 11)
#define   NFP_CRYPTO_BULK_ERR_ALLOC_WATCHDOG2           (0x1 << 10)
#define   NFP_CRYPTO_BULK_ERR_ALLOC_WATCHDOG1           (0x1 << 9)
#define   NFP_CRYPTO_BULK_ERR_ALLOC_WATCHDOG0           (0x1 << 8)
#define   NFP_CRYPTO_BULK_ERR_ALLOC_ARB                 (0x1 << 7)
#define   NFP_CRYPTO_BULK_ERR_ALLOC_FIFO_PARITY         (0x1 << 6)
#define   NFP_CRYPTO_BULK_ERR_ALLOC_MEM_PARITY          (0x1 << 5)
#define   NFP_CRYPTO_BULK_ERR_ALLOC_BUSY                (0x1 << 4)
#define   NFP_CRYPTO_BULK_ERR_ALLOC_GET_CTX             (0x1 << 3)
#define   NFP_CRYPTO_BULK_ERR_ALLOC_FREE_CTX            (0x1 << 2)
#define   NFP_CRYPTO_BULK_ERR_ALLOC_GET                 (0x1 << 1)
#define   NFP_CRYPTO_BULK_ERR_ALLOC_CTX                 (0x1)
/* Register Type: WhsOnFrstErr */
#define NFP_CRYPTO_BULK_FERR           0x0014
/* Register Type: ErrorMask */
#define NFP_CRYPTO_BULK_ERRMASK        0x0018
/* Register Type: ClkStopMask */
#define NFP_CRYPTO_BULK_CLK_STOPMASK   0x001c
/* Register Type: TestMode */
#define NFP_CRYPTO_BULK_TESTMODE       0x0020
#define   NFP_CRYPTO_BULK_TESTMODE_TARGET(_x)           (((_x) & 0xff) << 24)
#define   NFP_CRYPTO_BULK_TESTMODE_TARGET_of(_x)        (((_x) >> 24) & 0xff)
#define     NFP_CRYPTO_BULK_TESTMODE_TARGET_ALLOC       (129)
#define     NFP_CRYPTO_BULK_TESTMODE_TARGET_WRT         (130)
#define     NFP_CRYPTO_BULK_TESTMODE_TARGET_OTHER       (131)
#define   NFP_CRYPTO_BULK_TESTMODE_DATA(_x)             ((_x) & 0xffffff)
#define   NFP_CRYPTO_BULK_TESTMODE_DATA_of(_x)          ((_x) & 0xffffff)

/* HGID: nfp3200/crypto_bulkeng.desc = ade0020e9035 */
/* Register Type: ErrRegLo */
#define NFP_CRYPTO_BULKENG_ERR_LO      0x0000
#define   NFP_CRYPTO_BULKENG_ERR_LO_SER_PARITY          (0x1 << 25)
#define   NFP_CRYPTO_BULKENG_ERR_LO_SER_OVRFLW          (0x1 << 24)
#define   NFP_CRYPTO_BULKENG_ERR_LO_DAT_OVRFLOW         (0x1 << 23)
#define   NFP_CRYPTO_BULKENG_ERR_LO_RDYDAT_PARITY       (0x1 << 22)
#define   NFP_CRYPTO_BULKENG_ERR_LO_CFGDAT_PARITY       (0x1 << 21)
#define   NFP_CRYPTO_BULKENG_ERR_LO_CFGDAT_SIZE         (0x1 << 20)
#define   NFP_CRYPTO_BULKENG_ERR_LO_DES_OVRFLW_MASK     (0x1 << 19)
#define   NFP_CRYPTO_BULKENG_ERR_LO_DES_OVRFLW_FLAG     (0x1 << 18)
#define   NFP_CRYPTO_BULKENG_ERR_LO_DES_OVERFLW_DOUT    (0x1 << 17)
#define   NFP_CRYPTO_BULKENG_ERR_LO_DES_OVRFLW_DIN      (0x1 << 16)
#define   NFP_CRYPTO_BULKENG_ERR_LO_DES_CFG_PARTIY      (0x1 << 15)
#define   NFP_CRYPTO_BULKENG_ERR_LO_DES_CFG_SIZE        (0x1 << 14)
#define   NFP_CRYPTO_BULKENG_ERR_LO_AES_OVRFLW_FLAG     (0x1 << 13)
#define   NFP_CRYPTO_BULKENG_ERR_LO_AES_OVRFLW_DATA     (0x1 << 12)
#define   NFP_CRYPTO_BULKENG_ERR_LO_AES_OVRFLW_MASK     (0x1 << 11)
#define   NFP_CRYPTO_BULKENG_ERR_LO_AES_FDF_OVRFLW_FLAG (0x1 << 10)
#define   NFP_CRYPTO_BULKENG_ERR_LO_AES_FDF_OVRFLW_DOUT (0x1 << 9)
#define   NFP_CRYPTO_BULKENG_ERR_LO_AES_FDF_OVRFLW_DIN  (0x1 << 8)
#define   NFP_CRYPTO_BULKENG_ERR_LO_AES_KEYRD_PARITY    (0x1 << 7)
#define   NFP_CRYPTO_BULKENG_ERR_LO_AES_KEYIN_PARITY    (0x1 << 6)
#define   NFP_CRYPTO_BULKENG_ERR_LO_AES_KEY_SIZE        (0x1 << 5)
#define   NFP_CRYPTO_BULKENG_ERR_LO_AES_CFG_PARITY      (0x1 << 4)
#define   NFP_CRYPTO_BULKENG_ERR_LO_AES_CFG_SIZE        (0x1 << 3)
#define   NFP_CRYPTO_BULKENG_ERR_LO_CPC_PKG_HOLE        (0x1 << 2)
#define   NFP_CRYPTO_BULKENG_ERR_LO_CPC_OVRFLW          (0x1 << 1)
#define   NFP_CRYPTO_BULKENG_ERR_LO_CPC_PARITY          (0x1)
/* Register Type: ErrRegHi */
#define NFP_CRYPTO_BULKENG_ERR_HI      0x0004
#define   NFP_CRYPTO_BULKENG_ERR_HI_CRC_MEMRD           (0x1 << 27)
#define   NFP_CRYPTO_BULKENG_ERR_HI_CRC_INSTR_COLLIDE   (0x1 << 26)
#define   NFP_CRYPTO_BULKENG_ERR_HI_CPS_FLAG            (0x1 << 25)
#define   NFP_CRYPTO_BULKENG_ERR_HI_CPS_PKG_HOLE        (0x1 << 24)
#define   NFP_CRYPTO_BULKENG_ERR_HI_CDI_PARITY          (0x1 << 23)
#define   NFP_CRYPTO_BULKENG_ERR_HI_CDI_OVRFLW          (0x1 << 22)
#define   NFP_CRYPTO_BULKENG_ERR_HI_ICF_PTR1_PARITY     (0x1 << 21)
#define   NFP_CRYPTO_BULKENG_ERR_HI_ICF_PTR0_PARITY     (0x1 << 20)
#define   NFP_CRYPTO_BULKENG_ERR_HI_ICF_DATA_PARITY     (0x1 << 19)
#define   NFP_CRYPTO_BULKENG_ERR_HI_RDR_BUSY            (0x1 << 18)
#define   NFP_CRYPTO_BULKENG_ERR_HI_RDR_PARITY          (0x1 << 17)
#define   NFP_CRYPTO_BULKENG_ERR_HI_RDR_OVRFLW          (0x1 << 16)
#define   NFP_CRYPTO_BULKENG_ERR_HI_HASH_OVRFLW         (0x1 << 8)
#define   NFP_CRYPTO_BULKENG_ERR_HI_SHA_CFG_SIZE        (0x1 << 7)
#define   NFP_CRYPTO_BULKENG_ERR_HI_SHA_CFG_PARITY      (0x1 << 6)
#define   NFP_CRYPTO_BULKENG_ERR_HI_GF_FLAGTYPE         (0x1 << 5)
#define   NFP_CRYPTO_BULKENG_ERR_HI_GF_CFG_SIZE         (0x1 << 4)
#define   NFP_CRYPTO_BULKENG_ERR_HI_GF_CFG_PARITY       (0x1 << 3)
#define   NFP_CRYPTO_BULKENG_ERR_HI_CPH_PKG_HOLE        (0x1 << 2)
#define   NFP_CRYPTO_BULKENG_ERR_HI_CPH_OVRFLW          (0x1 << 1)
#define   NFP_CRYPTO_BULKENG_ERR_HI_CPH_PARITY          (0x1)
/* Register Type: WhosOnFrstLo */
#define NFP_CRYPTO_BULKENG_FERR_LO     0x0008
#define   NFP_CRYPTO_BULKENG_FERR_LO_FERR_LO(_x)        (_x)
#define   NFP_CRYPTO_BULKENG_FERR_LO_FERR_LO_of(_x)     (_x)
/* Register Type: WhosOnFrstHi */
#define NFP_CRYPTO_BULKENG_FERR_HI     0x000c
#define   NFP_CRYPTO_BULKENG_FERR_HI_FERR_HI(_x)        (_x)
#define   NFP_CRYPTO_BULKENG_FERR_HI_FERR_HI_of(_x)     (_x)
/* Register Type: ErrMaskLo */
#define NFP_CRYPTO_BULKENG_ERRMASK_LO  0x0010
#define   NFP_CRYPTO_BULKENG_ERRMASK_LO_ERRMASK_LO(_x)  (_x)
#define   NFP_CRYPTO_BULKENG_ERRMASK_LO_ERRMASK_LO_of(_x) (_x)
/* Register Type: ErrMaskHi */
#define NFP_CRYPTO_BULKENG_ERRMASK_HI  0x0014
#define   NFP_CRYPTO_BULKENG_ERRMASK_HI_ERRMASK_HI(_x)  (_x)
#define   NFP_CRYPTO_BULKENG_ERRMASK_HI_ERRMASK_HI_of(_x) (_x)
/* Register Type: ClkStpMskLo */
#define NFP_CRYPTO_BULKENG_CLK_STOPMASK_LO 0x0018
#define   NFP_CRYPTO_BULKENG_CLK_STOPMASK_LO_CLK_STOPMASK_LO(_x) (_x)
#define   NFP_CRYPTO_BULKENG_CLK_STOPMASK_LO_CLK_STOPMASK_LO_of(_x) (_x)
/* Register Type: ClkStpMskHi */
#define NFP_CRYPTO_BULKENG_CLK_STOPMASK_HI 0x001c
#define   NFP_CRYPTO_BULKENG_CLK_STOPMASK_HI_CLK_STOPMASK_HI(_x) (_x)
#define   NFP_CRYPTO_BULKENG_CLK_STOPMASK_HI_CLK_STOPMASK_HI_of(_x) (_x)
/* Register Type: TstMdErrInj */
#define NFP_CRYPTO_BULKENG_TESTMODE_INJ 0x0020
#define   NFP_CRYPTO_BULKENG_TESTMODE_INJ_TARGET(_x)    (((_x) & 0xff) << 24)
#define   NFP_CRYPTO_BULKENG_TESTMODE_INJ_TARGET_of(_x) (((_x) >> 24) & 0xff)
#define     NFP_CRYPTO_BULKENG_TESTMODE_INJ_TARGET_RDR  (129)
#define     NFP_CRYPTO_BULKENG_TESTMODE_INJ_TARGET_ICF_DATA (130)
#define     NFP_CRYPTO_BULKENG_TESTMODE_INJ_TARGET_ICF_PTR0 (131)
#define     NFP_CRYPTO_BULKENG_TESTMODE_INJ_TARGET_ICF_PTR1 (132)
#define     NFP_CRYPTO_BULKENG_TESTMODE_INJ_TARGET_CDI  (133)
#define     NFP_CRYPTO_BULKENG_TESTMODE_INJ_TARGET_AES_KEY (136)
#define     NFP_CRYPTO_BULKENG_TESTMODE_INJ_TARGET_AES_CFG (137)
#define     NFP_CRYPTO_BULKENG_TESTMODE_INJ_TARGET_DES  (138)
#define     NFP_CRYPTO_BULKENG_TESTMODE_INJ_TARGET_CPC  (139)
#define     NFP_CRYPTO_BULKENG_TESTMODE_INJ_TARGET_GF   (140)
#define   NFP_CRYPTO_BULKENG_TESTMODE_INJ_DATA(_x)      ((_x) & 0xffffff)
#define   NFP_CRYPTO_BULKENG_TESTMODE_INJ_DATA_of(_x)   ((_x) & 0xffffff)

#define NFP_XPB_CRYPTO_BULK	NFP_XPB_DEST(19,7)

#endif /* NFP3200_NFP_CRYPTO_H */
