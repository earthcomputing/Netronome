/*
 * Copyright (C) 2010-2013,  Netronome Systems, Inc.  All rights reserved.
 *
 * Tool for accessing NFP memories using their names
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <getopt.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>

#include <nfp-common/nfp_resid.h>
#include <nfp.h>
#include <nfp_cpp.h>
#include <nfp_sal.h>

#include "version.h"

#define MAX_WORD_SIZE   8
#define MAX_BUFF_SIZE   256

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)	(sizeof(x)/sizeof(x[0]))
#endif

#include "hexrw.c"

enum nfp_memtype {
    MEM_DRAM, MEM_SRAM, MEM_GS, MEM_CLS, MEM_CTM, MEM_LMEM, MEM_USTORE,
    MEM_IMEM, MEM_EMEM, MEM_DMEM,
    MEM_CRYPTO, MEM_ILA,
    MEM_NBI_ALL_SMEM, MEM_NBI_ALL_LMEM, MEM_NBI_PMEM,
    MEM_NBI_TM_TMDESCSRAM, MEM_NBI_TM_TMBDSRAM, MEM_NBI_TM_TMBUFFERLIST,
    MEM_NBI_TM_TMSLOWDESCSTAM, MEM_NBI_TM_TMREORDERBUF, MEM_NBI_TM_TMPKTSRAM,
    MEM_NBI_TM_TMHEADTAILSRAM, MEM_NBI_TM_TMFLOWCONTROL, MEM_NBI_TM_TMBLQREAD,
    MEM_NBI_DMA_BDSRAM, MEM_NBI_DMA_BUFFERLIST,MEM_NBI_DMA_BCSRAM,
    MEM_NBI_MAC_STATSHYD0, MEM_NBI_MAC_STATSHYD1, MEM_NBI_MAC_CHSTATS,
    MEM_PCIE,
    MEM_MAX
};

#if 1

#ifndef MIN
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#endif

struct nfp_cpp *dev_nfp_cpp;

/* cpp define for nbi preclassifier */
#define CPP_NBI_PRECLASSIFIER_OFFSET         (0x300000)
#define CPP_NBI_PRECLASSIFIER_LMEM_OFFSET(x) ((x) == 0 ? 0x30000 : (x)*0x4000)
#define CPP_NBI_PRECLASSIFIER_LMEM_COUNT 12
#define CPP_NBI_PRECLASSIFIER_LMEM_LEN 0x4000
#define CPP_NBI_PRECLASSIFIER_ALL_LMEM_OFFSET (CPP_NBI_PRECLASSIFIER_OFFSET)
#define CPP_NBI_PRECLASSIFIER_ALL_SMEM_OFFSET (CPP_NBI_PRECLASSIFIER_OFFSET + 0x80000)

#define CPP_NBI_PRECLASSIFIER_SMEM_OFFSET(x) ((x) == 0 ? 0x90000 : 0xb0000)
#define CPP_NBI_PRECLASSIFIER_SMEM_COUNT 2
#define CPP_NBI_PRECLASSIFIER_SMEM_LEN 0x10000

#define CPP_NBI_PRECLASSIFIER_PMEM_OFFSET(x) ((x) == 0 ? 0xc0000 : 0xc0000)
#define CPP_NBI_PRECLASSIFIER_PMEM_COUNT 1
#define CPP_NBI_PRECLASSIFIER_PMEM_LEN 0x20000

#define CPP_TGT_NBI 1

/* cpp define for nbi traffic manager */
#define CPP_NBI_TM_OFFSET   (0x200000)

#define CPP_NBI_TM_TMDESCSRAM       0x00
#define CPP_NBI_TM_TMBDSRAM         0x40000
#define CPP_NBI_TM_TMBUFFERLIST     0x48000
#define CPP_NBI_TM_TMSLOWDESCSRAM   0x50000
#define CPP_NBI_TM_TMREORDERBUF     0x58000
#define CPP_NBI_TM_TMPKTSRAM        0x60000
#define CPP_NBI_TM_TMHEADTAILSRAM   0x68000
#define CPP_NBI_TM_TMFLOWCONTROL    0x70000
#define CPP_NBI_TM_TMBLQREAD        0x80000

/* cpp define for nbi dma */
#define CPP_NBI_DMA_OFFSET   (0x000000)

#define CPP_NBI_DMA_BDSRAM      0
#define CPP_NBI_DMA_BUFFERLIST  0x8000
#define CPP_NBI_DMA_BCSRAM      0xa000

/* cpp define for nbi mac */
#define CPP_NBI_MAC_OFFSET   (0x100000)

#define CPP_NBI_MAC_STATSHYD0 0x00000
#define CPP_NBI_MAC_STATSHYD1 0x01000
#define CPP_NBI_MAC_CHSTATS   0x10000


#endif

/* PCIe Shared Memory */
#define CPP_TGT_PCIE 9

#define PCIESHAREDSRAM_BASE (0x000000)
#define PCIESHAREDSRAM_LEN (0x10000)

/* Crypto Memory */
#define CPP_TGT_CRYPTO 12
#define CRYPTOMEM_BASE (0x000000)

/* Ila Memory */
#define CPP_TGT_ILA 6
#define ILAMEM_BASE (0x000000)

/* Functions for read/write access using cpp access */
int cpp_tgt_write(struct nfp_cpp *nfp_cpp,
                        int isl, void * buffer, uint64_t len, uint64_t addr, int tgt)
{
    uint32_t id = NFP_CPP_ISLAND_ID(tgt,
                                  NFP_CPP_ACTION_RW,
                                  0,
                                  isl);
    return nfp_cpp_write(nfp_cpp, id, addr, buffer, len);
}

int cpp_tgt_read(struct nfp_cpp *nfp_cpp,
                        int isl, void * buffer, uint64_t len, uint64_t addr, int tgt)
{
    int retval;
    uint32_t id = NFP_CPP_ISLAND_ID(tgt,
                                  NFP_CPP_ACTION_RW,
                                  0,
                                  isl);
    retval = nfp_cpp_read(nfp_cpp, id, addr, buffer, len);
    return retval;
}

int cpp_tgt_fill(struct nfp_cpp *nfp_cpp, int isl, void *buffer, unsigned long long buf_length, unsigned long long offset, unsigned long long write_length, int tgt)
{
    uint64_t towrite;
    uint64_t written = 0;
    ssize_t ret;
    while (write_length) {
        towrite = MIN(buf_length, write_length);
        /* write out the buffer */
        ret = cpp_tgt_write(nfp_cpp, isl, buffer, towrite, offset, tgt);
        if (ret < 0)
            return -1;

        written += towrite;
        write_length -= towrite;
        offset += towrite;
    }
    return written;
}

struct nfp_memtgt {
    enum nfp_memtype type;
    const char *name;
    const char *alias;
    const char *alias2;
    int wordsize;
    int domain;
    const char *desc;
};

static struct nfp_memtgt targets[] = {
    { .type = MEM_EMEM,
      .name = "dramN", .alias = "ddr", .alias2 = "mem", .wordsize = 4,
      .desc = "External Memory (N island number or 0,1,2)" },
    { .type = MEM_EMEM,
      .name = "ememN", .alias = "ddr", .alias2 = "mem", .wordsize = 4,
      .desc = "External Memory (N island number or 0,1,2)" },
    { .type = MEM_DMEM,
      .name = "dmemN", .alias = "ddr", .alias2 = "mem", .wordsize = 4,
      .desc = "DCache (N island number or 0,1,2)" },
#if 0
    { .type = MEM_SRAM,
      .name = "sram", .alias = "qdr", .alias2 = NULL, .wordsize = 4,
      .desc = "QDR SRAM or VQDR" },
    { .type = MEM_SRAM,
      .name = "sram", .alias = "qdr", .alias2 = NULL, .wordsize = 4,
      .desc = "QDR SRAM or VQDR (channel N)" },
#endif
    { .type = MEM_IMEM,
      .name = "imemN", .alias = "imem", .alias2 = NULL, .wordsize = 4,
      .desc = "Internal Memory (N island number or 0,1)" },
#if 0
    { .type = MEM_GS,
      .name = "scratch", .alias = "gs", .alias2 = NULL, .wordsize = 4,
      .desc = "Global Scratch" },
#endif
    { .type = MEM_CLS,
      .name = "clsN", .alias = NULL, .alias2 = NULL, .wordsize = 4,
      .desc = "Cluster Local Scratch (N island number)" },
    { .type = MEM_CTM,
      .name = "ctmN", .alias = NULL, .alias2 = NULL, .wordsize = 4,
      .desc = "Cluster Target Memory (N island number)", },
    { .type = MEM_LMEM,
      .name = "lmem", .alias = NULL, .alias2 = NULL, .wordsize = 4,
      .desc = "ME Local Memory (microengine, specify ME with -m option)" },
    { .type = MEM_USTORE,
      .name = "ustore", .alias = NULL, .alias2 = NULL, .wordsize = 8,
      .desc = "ME Control Store (microengine, specify ME with -m option)", },
    { .type = MEM_CRYPTO,
      .name = "cryptoN", .alias = NULL, .alias2 = NULL, .wordsize = 4,
      .desc = "Crypto Memory (N island numner)", },
    { .type = MEM_ILA,
      .name = "ilaN", .alias = NULL, .alias2 = NULL, .wordsize = 4,
      .desc = "ILA Shared SRAM Memory (N island number)", },
    { .type = MEM_NBI_ALL_SMEM,
      .name = "pre_smemN", .alias = NULL, .alias2 = NULL, .wordsize = 8,
      .desc = "NBI Preclassifier All Shared Memory (N island number)", },
    { .type = MEM_NBI_ALL_LMEM,
      .name = "pre_lmemN", .alias = NULL, .alias2 = NULL, .wordsize = 8,
      .desc = "NBI Preclassifier All Local Memory (N island number)", },
    { .type = MEM_NBI_TM_TMDESCSRAM,
      .name = "tm_descsN", .alias = NULL, .alias2 = NULL, .wordsize = 8,
      .desc = "NBI TM Descriptor SRAM (N island number)", },
    { .type = MEM_NBI_TM_TMBDSRAM,
      .name = "tm_bdsN", .alias = NULL, .alias2 = NULL, .wordsize = 8,
      .desc = "NBI TM Buffer Descriptor SRAM (N island number)", },
    { .type = MEM_NBI_TM_TMBUFFERLIST,
      .name = "tm_blN", .alias = NULL, .alias2 = NULL, .wordsize = 8,
      .desc = "NBI TM Buffer List Queue Registers (N island number)", },
    { .type = MEM_NBI_TM_TMSLOWDESCSTAM,
      .name = "tm_slowdescsN", .alias = NULL, .alias2 = NULL, .wordsize = 8,
      .desc = "NBI TM Slow Descriptor SRAM (N island number)", },
      { .type = MEM_NBI_TM_TMREORDERBUF,
      .name = "tm_reorderbufsN", .alias = NULL, .alias2 = NULL, .wordsize = 8,
      .desc = "NBI TM Reorder Buffer SRAM (N island number)", },
    { .type = MEM_NBI_TM_TMPKTSRAM,
      .name = "tm_pktsN", .alias = NULL, .alias2 = NULL, .wordsize = 8,
      .desc = "NBI TM Packet SRAM (N island number)", },
    { .type = MEM_NBI_TM_TMHEADTAILSRAM,
      .name = "tm_htsN", .alias = NULL, .alias2 = NULL, .wordsize = 8,
      .desc = "NBI TM Descriptor Head and Tail SRAM (N island number)", },
    { .type = MEM_NBI_TM_TMFLOWCONTROL,
      .name = "tm_fcN", .alias = NULL, .alias2 = NULL, .wordsize = 8,
      .desc = "NBI TM Flow Control Signals (N island number)", },
#if 0
      { .type = MEM_NBI_TM_TMBLQREAD,
      .name = "tm_blqrN", .alias = NULL, .alias2 = NULL, .wordsize = 8,
      .desc = "NBI TM Buffer List Queue Read Addresses (N island number)", },
#endif
    { .type = MEM_NBI_DMA_BDSRAM,
      .name = "dma_bdrN", .alias = NULL, .alias2 = NULL, .wordsize = 8,
      .desc = "NBI DMA Buffer Descriptor RAM (N island number)", },
    { .type = MEM_NBI_DMA_BUFFERLIST,
      .name = "dma_blqN", .alias = NULL, .alias2 = NULL, .wordsize = 8,
      .desc = "NBI DMA Buffer List Queue Register (N island number)", },
    { .type = MEM_NBI_DMA_BCSRAM,
      .name = "dma_bcrN", .alias = NULL, .alias2 = NULL, .wordsize = 8,
      .desc = "NBI DMA Buffer Completion RAM (N island number)", },
    { .type = MEM_NBI_MAC_STATSHYD0,
      .name = "mac_stathyd_zeroN", .alias = NULL, .alias2 = NULL, .wordsize = 4,
      .desc = "NBI MAC Ethernet Port Stats for Hyrda 0 (N island number)", },
    { .type = MEM_NBI_MAC_STATSHYD1,
      .name = "mac_stathyd_oneN", .alias = NULL, .alias2 = NULL, .wordsize = 4,
      .desc = "NBI MAC Ethernet Port Stats for Hyrda 1 (N island number)", },
    { .type = MEM_NBI_MAC_CHSTATS,
      .name = "mac_chstatN", .alias = NULL, .alias2 = NULL, .wordsize = 4,
      .desc = "NBI MAC Channel Stats (N island number)", },
    { .type = MEM_PCIE,
      .name = "pcieN", .alias = NULL, .alias2 = NULL, .wordsize = 4,
      .desc = "PCIe Shared Memory (N island number)", },
};

void usage(const char *program, int err)
{
    FILE *outfd = err ? stderr : stdout;
    int n;

    fprintf(outfd, "Usage: "
            "%s [options] target:address [value...]\n"
            "\n"
            "Read/write contents of NFP memory.  Output will default to "
            "stdout unless\nspecified.  Giving a value on command line "
            "causes memory to be written.\nSpecifying multiple values "
            "will write consecutive memory words.  If a length\nis specified "
            "the values will be repeated to fill the indicated memory region."
            "\nA '+' or '-' in front of the value will trigger a "
            "read-modify-write operation\nthat sets or clears the indicated "
            "bits.\n"
            "\n"
            "Options:\n"
            NFP_HELP
            REMOTE_HOST_HELP
            "  -i FILE, --in=FILE          read from FILE ('-' for stdin)\n"
            "  -o FILE, --out=FILE         write to FILE ('-' for stdout)\n"
            "  -l NUM, --len=NUM           maximum length to read/write "
            "(in bytes)\n"
            "                              (default is file size or word size)\n"
            "  -w NUM, --wordsize=NUM      word size to read/write (4 or 8)\n"
            "  -v, --view-all              don't skip repeated lines with '*'\n"
            "  -R, --raw                   force raw binary mode\n"
            "  -m NAME/NUM, --me=NAME/NUM  ME (island.me or MEID number)\n"
            "\n"
            VERSION_HELP
	        "  -h, --help                  show this help message and exit\n"
            "\n", program);

    fprintf(outfd, "Available targets:\n");
    for (n = 0; n < ARRAY_SIZE(targets); n++)
        fprintf(outfd, "  %-8s  %s\n", targets[n].name, targets[n].desc);
    fprintf(outfd, "\n");

    exit(err ? EXIT_FAILURE : EXIT_SUCCESS);
}

static int mem_read(struct nfp_device *dev, struct nfp_memtgt *tgt,
                    void *buffer, unsigned long long offset, size_t len)
{
    uint64_t addr;
    int err = 0;

    if (offset % tgt->wordsize) {
        errno = EINVAL;
        fprintf(stderr, "offset 0x%lx is not a multiple of wordsize %d\n",
                (uint64_t) offset, tgt->wordsize);
        return -1;
    }

    if (len % tgt->wordsize) {
        errno = EINVAL;
        fprintf(stderr, "length %ld is not a multiple of wordsize %d\n",
                (uint64_t) len, tgt->wordsize);
        return -1;
    }

    switch (tgt->type) {
#if 0
    case MEM_DRAM:
        err = nfp_dram_read(dev, buffer, len, offset);
        break;
#endif
    case MEM_EMEM:
        err = nfp_emem_read(dev, tgt->domain, buffer, len, offset);
        break;
    case MEM_DMEM:
        err = nfp_sal_extmem_dc_read(dev, tgt->domain, buffer, len, offset);
        break;
#if 0
    case MEM_SRAM:
        offset += (0x40000000UL * tgt->domain);
        err = nfp_sram_read(dev, buffer, len, offset);
        break;
    case MEM_GS:
        err = nfp_gs_read(dev, buffer, len, offset);
        break;
#endif
    case MEM_CLS:
        err = nfp_cls_read(dev, tgt->domain, buffer, len, offset);
        break;
    case MEM_CTM:
        err = nfp_ctm_read(dev, tgt->domain, buffer, len, offset);
        break;
    case MEM_LMEM:
        err = nfp_lmem_read(dev, tgt->domain, buffer, len, offset);
        break;
    case MEM_USTORE:
        err = nfp_ustore_read(dev, tgt->domain, buffer, len, offset);
        break;
    case MEM_IMEM:
        err = nfp_imem_read(dev, tgt->domain, buffer, len, offset);
        break;
    case MEM_CRYPTO:
        addr = CRYPTOMEM_BASE  + offset;
        err = cpp_tgt_read(dev_nfp_cpp, tgt->domain, buffer, len, addr, CPP_TGT_CRYPTO);
        break;
    case MEM_ILA:
        addr = ILAMEM_BASE + offset;
        err = cpp_tgt_read(dev_nfp_cpp, tgt->domain, buffer, len, addr, CPP_TGT_ILA);
        break;
    case MEM_NBI_ALL_SMEM:
        addr = CPP_NBI_PRECLASSIFIER_ALL_SMEM_OFFSET + offset;
        err = cpp_tgt_read(dev_nfp_cpp, tgt->domain, buffer, len, addr, CPP_TGT_NBI);
        break;
    case MEM_NBI_ALL_LMEM:
        addr = CPP_NBI_PRECLASSIFIER_ALL_LMEM_OFFSET + offset;
        err = cpp_tgt_read(dev_nfp_cpp, tgt->domain, buffer, len, addr, CPP_TGT_NBI);
        break;
    case MEM_NBI_TM_TMDESCSRAM:
        addr = CPP_NBI_TM_OFFSET + CPP_NBI_TM_TMDESCSRAM + offset;
        err = cpp_tgt_read(dev_nfp_cpp, tgt->domain, buffer, len, addr, CPP_TGT_NBI);
        break;
    case MEM_NBI_TM_TMBDSRAM:
        addr = CPP_NBI_TM_OFFSET + CPP_NBI_TM_TMBDSRAM + offset;
        err = cpp_tgt_read(dev_nfp_cpp, tgt->domain, buffer, len, addr, CPP_TGT_NBI);
        break;
    case MEM_NBI_TM_TMBUFFERLIST:
        addr = CPP_NBI_TM_OFFSET + CPP_NBI_TM_TMBUFFERLIST + offset;
        err = cpp_tgt_read(dev_nfp_cpp, tgt->domain, buffer, len, addr, CPP_TGT_NBI);
        break;
    case MEM_NBI_TM_TMSLOWDESCSTAM:
        addr = CPP_NBI_TM_OFFSET + CPP_NBI_TM_TMSLOWDESCSRAM + offset;
        err = cpp_tgt_read(dev_nfp_cpp, tgt->domain, buffer, len, addr, CPP_TGT_NBI);
        break;
    case MEM_NBI_TM_TMREORDERBUF:
        addr = CPP_NBI_TM_OFFSET + CPP_NBI_TM_TMREORDERBUF + offset;
        err = cpp_tgt_read(dev_nfp_cpp, tgt->domain, buffer, len, addr, CPP_TGT_NBI);
        break;
    case MEM_NBI_TM_TMPKTSRAM:
        addr = CPP_NBI_TM_OFFSET + CPP_NBI_TM_TMPKTSRAM + offset;
        err = cpp_tgt_read(dev_nfp_cpp, tgt->domain, buffer, len, addr, CPP_TGT_NBI);
        break;
    case MEM_NBI_TM_TMHEADTAILSRAM:
        addr = CPP_NBI_TM_OFFSET + CPP_NBI_TM_TMHEADTAILSRAM + offset;
        err = cpp_tgt_read(dev_nfp_cpp, tgt->domain, buffer, len, addr, CPP_TGT_NBI);
        break;
    case MEM_NBI_TM_TMFLOWCONTROL:
        addr = CPP_NBI_TM_OFFSET + CPP_NBI_TM_TMFLOWCONTROL + offset;
        err = cpp_tgt_read(dev_nfp_cpp, tgt->domain, buffer, len, addr, CPP_TGT_NBI);
        break;
#if 0
    case MEM_NBI_TM_TMBLQREAD:
        {
            uint64_t addr = CPP_NBI_TM_OFFSET + CPP_NBI_TM_TMBLQREAD + offset;
            err = cpp_tgt_read(dev_nfp_cpp, tgt->domain, buffer, len, addr, CPP_TGT_NBI);
        }
        break;
#endif
    case MEM_NBI_DMA_BDSRAM:
        addr = CPP_NBI_DMA_OFFSET + CPP_NBI_DMA_BDSRAM + offset;
        err = cpp_tgt_read(dev_nfp_cpp, tgt->domain, buffer, len, addr, CPP_TGT_NBI);
        break;
    case MEM_NBI_DMA_BUFFERLIST:
        addr = CPP_NBI_DMA_OFFSET + CPP_NBI_DMA_BUFFERLIST + offset;
        err = cpp_tgt_read(dev_nfp_cpp, tgt->domain, buffer, len, addr, CPP_TGT_NBI);
        break;
    case MEM_NBI_DMA_BCSRAM:
        addr = CPP_NBI_DMA_OFFSET + CPP_NBI_DMA_BCSRAM + offset;
        err = cpp_tgt_read(dev_nfp_cpp, tgt->domain, buffer, len, addr, CPP_TGT_NBI);
        break;
    case MEM_NBI_MAC_STATSHYD0:
        addr = CPP_NBI_MAC_OFFSET + CPP_NBI_MAC_STATSHYD0 + offset;
        err = cpp_tgt_read(dev_nfp_cpp, tgt->domain, buffer, len, addr, CPP_TGT_NBI);
        break;
    case MEM_NBI_MAC_STATSHYD1:
        addr = CPP_NBI_MAC_OFFSET + CPP_NBI_MAC_STATSHYD1 + offset;
        err = cpp_tgt_read(dev_nfp_cpp, tgt->domain, buffer, len, addr, CPP_TGT_NBI);
        break;
    case MEM_NBI_MAC_CHSTATS:
        addr = CPP_NBI_MAC_OFFSET + CPP_NBI_MAC_CHSTATS + offset;
        err = cpp_tgt_read(dev_nfp_cpp, tgt->domain, buffer, len, addr,CPP_TGT_NBI);
        break;

#if 0
    case MEM_NBI_PMEM:
        {
        uint64_t addr = CPP_NBI_PRECLASSIFIER_OFFSET + CPP_NBI_PRECLASSIFIER_PMEM_OFFSET(0) + offset;
        err = cpp_tgt_read(dev_nfp_cpp, tgt->domain, buffer, len, addr, CPP_TGT_NBI);
        }
        break;
#endif
    case MEM_PCIE:
        addr = PCIESHAREDSRAM_BASE + offset;
        err = cpp_tgt_read(dev_nfp_cpp, tgt->domain, buffer, len, addr, CPP_TGT_PCIE);
        break;
    default:
        fprintf(stderr, "Unknown memory type: %d\n", tgt->type);
        exit(EXIT_FAILURE);
    }

    return err;
}

static int mem_write(struct nfp_device *dev, struct nfp_memtgt *tgt,
                     void *buffer, size_t buflen, unsigned long long offset,
                     size_t len)
{
    uint64_t addr;
    int err = 0;

    if (buflen % tgt->wordsize) {
        fprintf(stderr, "buffer length %ld in not a multiple of wordsize %d\n",
                (uint64_t) buflen, tgt->wordsize);
        errno = EINVAL;
        return -1;
    }

    if (offset % tgt->wordsize) {
        errno = EINVAL;
        fprintf(stderr, "offset 0x%lx is not a multiple of wordsize %d\n",
                (uint64_t) offset, tgt->wordsize);
        return -1;
    }

    if (len % tgt->wordsize) {
        errno = EINVAL;
        fprintf(stderr, "length %ld is not a multiple of wordsize %d\n",
                (uint64_t) len, tgt->wordsize);
        return -1;
    }

    switch (tgt->type) {
#if 0
    case MEM_DRAM:
        err = nfp_dram_fill(dev, buffer, buflen, offset, len);
        break;
#endif
    case MEM_EMEM:
        err = nfp_emem_fill(dev, tgt->domain, buffer, buflen, offset, len);
        break;
    case MEM_DMEM:
        err = nfp_sal_extmem_dc_fill(dev, tgt->domain, buffer, buflen, offset, len);
        break;
#if 0
    case MEM_SRAM:
        offset += (0x40000000UL * tgt->domain);
        err = nfp_sram_fill(dev, buffer, buflen, offset, len);
        break;
    case MEM_GS:
        err = nfp_gs_fill(dev, buffer, buflen, offset, len);
        break;
#endif
    case MEM_CLS:
        err = nfp_cls_fill(dev, tgt->domain, buffer, buflen, offset, len);
        break;
    case MEM_CTM:
        err = nfp_ctm_fill(dev, tgt->domain, buffer, buflen, offset, len);
        break;
    case MEM_LMEM:
        err = nfp_lmem_fill(dev, tgt->domain, buffer, buflen, offset, len);
        break;
    case MEM_USTORE:
        err = nfp_ustore_fill(dev, tgt->domain, buffer, buflen, offset, len);
        break;
    case MEM_IMEM:
        err = nfp_imem_fill(dev, tgt->domain, buffer, buflen, offset, len);
        break;
    case MEM_CRYPTO:
        addr = CRYPTOMEM_BASE  + offset;
        err = cpp_tgt_fill(dev_nfp_cpp, tgt->domain, buffer, buflen, addr, len, CPP_TGT_CRYPTO);
        break;
    case MEM_ILA:
        addr = ILAMEM_BASE + offset;
        err = cpp_tgt_fill(dev_nfp_cpp, tgt->domain, buffer, buflen, addr, len, CPP_TGT_ILA);
        break;
    case MEM_NBI_ALL_SMEM:
        addr = CPP_NBI_PRECLASSIFIER_ALL_SMEM_OFFSET + offset;
        err = cpp_tgt_fill(dev_nfp_cpp, tgt->domain, buffer, buflen, addr, len, CPP_TGT_NBI);
        break;
    case MEM_NBI_ALL_LMEM:
        addr = CPP_NBI_PRECLASSIFIER_ALL_LMEM_OFFSET + offset;
        err = cpp_tgt_fill(dev_nfp_cpp, tgt->domain, buffer, buflen, addr, len, CPP_TGT_NBI);
        break;
    case MEM_NBI_TM_TMDESCSRAM:
        addr = CPP_NBI_TM_OFFSET + CPP_NBI_TM_TMDESCSRAM + offset;
        err = cpp_tgt_fill(dev_nfp_cpp, tgt->domain, buffer, buflen, addr, len, CPP_TGT_NBI);
        break;
    case MEM_NBI_TM_TMBDSRAM:
        addr = CPP_NBI_TM_OFFSET + CPP_NBI_TM_TMBDSRAM + offset;
        err = cpp_tgt_fill(dev_nfp_cpp, tgt->domain, buffer, buflen, addr, len, CPP_TGT_NBI);
        break;
    case MEM_NBI_TM_TMBUFFERLIST:
        addr = CPP_NBI_TM_OFFSET + CPP_NBI_TM_TMBUFFERLIST + offset;
        err = cpp_tgt_fill(dev_nfp_cpp, tgt->domain, buffer, buflen, addr, len, CPP_TGT_NBI);
        break;
    case MEM_NBI_TM_TMSLOWDESCSTAM:
        addr = CPP_NBI_TM_OFFSET + CPP_NBI_TM_TMSLOWDESCSRAM + offset;
        err = cpp_tgt_fill(dev_nfp_cpp, tgt->domain, buffer, buflen, addr, len, CPP_TGT_NBI);
        break;
    case MEM_NBI_TM_TMREORDERBUF:
        addr = CPP_NBI_TM_OFFSET + CPP_NBI_TM_TMREORDERBUF + offset;
        err = cpp_tgt_fill(dev_nfp_cpp, tgt->domain, buffer, buflen, addr, len, CPP_TGT_NBI);
        break;
    case MEM_NBI_TM_TMPKTSRAM:
        addr = CPP_NBI_TM_OFFSET + CPP_NBI_TM_TMPKTSRAM + offset;
        err = cpp_tgt_fill(dev_nfp_cpp, tgt->domain, buffer, buflen, addr, len, CPP_TGT_NBI);
        break;
    case MEM_NBI_TM_TMHEADTAILSRAM:
        addr = CPP_NBI_TM_OFFSET + CPP_NBI_TM_TMHEADTAILSRAM + offset;
        err = cpp_tgt_fill(dev_nfp_cpp, tgt->domain, buffer, buflen, addr, len, CPP_TGT_NBI);
        break;
    case MEM_NBI_TM_TMFLOWCONTROL:
        addr = CPP_NBI_TM_OFFSET + CPP_NBI_TM_TMFLOWCONTROL + offset;
        err = cpp_tgt_fill(dev_nfp_cpp, tgt->domain, buffer, buflen, addr, len, CPP_TGT_NBI);
        break;
    case MEM_NBI_DMA_BDSRAM:
        addr = CPP_NBI_DMA_OFFSET + CPP_NBI_DMA_BDSRAM + offset;
        err = cpp_tgt_fill(dev_nfp_cpp, tgt->domain, buffer, buflen, addr, len, CPP_TGT_NBI);
        break;
    case MEM_NBI_DMA_BUFFERLIST:
        addr = CPP_NBI_DMA_OFFSET + CPP_NBI_DMA_BUFFERLIST + offset;
        err = cpp_tgt_fill(dev_nfp_cpp, tgt->domain, buffer, buflen, addr, len, CPP_TGT_NBI);
        break;
    case MEM_NBI_DMA_BCSRAM:
        addr = CPP_NBI_DMA_OFFSET + CPP_NBI_DMA_BCSRAM + offset;
        err = cpp_tgt_fill(dev_nfp_cpp, tgt->domain, buffer, buflen, addr, len, CPP_TGT_NBI);
        break;
    case MEM_NBI_MAC_STATSHYD0:
        addr = CPP_NBI_MAC_OFFSET + CPP_NBI_MAC_STATSHYD0 + offset;
        err = cpp_tgt_fill(dev_nfp_cpp, tgt->domain, buffer, buflen, addr, len, CPP_TGT_NBI);
        break;
    case MEM_NBI_MAC_STATSHYD1:
        addr = CPP_NBI_MAC_OFFSET + CPP_NBI_MAC_STATSHYD1 + offset;
        err = cpp_tgt_fill(dev_nfp_cpp, tgt->domain, buffer, buflen, addr, len, CPP_TGT_NBI);
        break;
    case MEM_NBI_MAC_CHSTATS:
        addr = CPP_NBI_MAC_OFFSET + CPP_NBI_MAC_CHSTATS + offset;
        err = cpp_tgt_fill(dev_nfp_cpp, tgt->domain, buffer, buflen, addr, len, CPP_TGT_NBI);
        break;
    case MEM_PCIE:
        addr = PCIESHAREDSRAM_BASE + offset;
        err = cpp_tgt_fill(dev_nfp_cpp, tgt->domain, buffer, buflen, addr, len, CPP_TGT_PCIE);
        break;
    default:
        fprintf(stderr, "Unknown memory type: %d\n", tgt->type);
        exit(EXIT_FAILURE);
    }

    return err;
}

static int mem_read_to_fd(struct nfp_device *dev,
                          struct nfp_memtgt *tgt, int fd,
                          unsigned long long offset, size_t len)
{
    char buff[MAX_BUFF_SIZE];
    size_t pos;
    ssize_t rlen = 0;
    int err;

    for (pos = 0; pos < len; pos += rlen) {
        rlen = sizeof(buff);
        if ((pos + rlen) > len)
            rlen = len - pos;

        rlen = mem_read(dev, tgt, buff, offset + pos, rlen);

        if (rlen == 0)
            break;
        if (rlen < 0) {
            perror("Error reading from NFP");
            exit(EXIT_FAILURE);
        }

        err = write(fd, buff, rlen);
        if (err < 0) {
            perror("Error writing to file");
            exit(EXIT_FAILURE);
        }
   }

    return 0;
}

static int mem_write_from_fd(struct nfp_device *dev,
                             struct nfp_memtgt *tgt, int fd,
                             unsigned long long offset, size_t len)
{
    char buff[MAX_BUFF_SIZE];
    size_t pos;
    ssize_t rlen = 0, tot;
    int done = 0;
    int err;

    for (pos = 0; !done && ((len == 0) || (pos < len)); pos += tot) {

        tot = 0;

        /* read a chunk of data up to a max of MAX_BUFF_SIZE */
        while (tot < sizeof(buff)) {
            rlen = read(fd, buff + tot, sizeof(buff) - tot);
            if (rlen < 0) {
                perror("Error reading from pipe");
                exit(EXIT_FAILURE);
            }

            if (rlen == 0) {
                done = 1;
                break;
            }

            tot += rlen;
        }

        if (tot == 0) /* eof with no data so we are done */
            break;

        if ((len > 0) && ((pos + tot) > len))
            tot = len - pos;

        err = mem_write(dev, tgt, buff, tot, offset + pos, tot);
        if (err < 0) {
            perror("Error writing to NFP");
            exit(EXIT_FAILURE);
        } else if (err != tot) {
            fprintf(stderr, "Failed to write %d bytes to NFP\n", (int) tot);
            exit(EXIT_FAILURE);
        }
   }

    return 0;
}


int main(int argc, char **argv)
{
    const struct option options[] = {
        { .name = "nfp", .has_arg = 1, .flag = NULL, .val = 'n' },
        { .name = "help", .has_arg = 0, .flag = NULL, .val = 'h' },
        { .name = "len", .has_arg = 1, .flag = NULL, .val = 'l' },
        { .name = "wordsize", .has_arg = 1, .flag = NULL, .val = 'w' },
        { .name = "raw", .has_arg = 0, .flag = NULL, .val = 'R' },
        { .name = "view-all", .has_arg = 0, .flag = NULL, .val = 'v' },
        { .name = "in", .has_arg = 1, .flag = NULL, .val = 'i' },
        { .name = "out", .has_arg = 1, .flag = NULL, .val = 'o' },
        { .name = "me", .has_arg = 1, .flag = NULL, .val = 'm' },
        VERSION_OPTION,
        { .name = 0 },
    };
    const char *program = argv[0];
    const char *filename = NULL;
    struct nfp_memtgt *tgt = NULL;
    struct nfp_device *nfp;
    unsigned long devnum = 0;
    unsigned long len = 0;
    unsigned long wordsz = 0;
    unsigned long long offset = 0;
    unsigned long domain = 0;
    enum { MODE_UNKNOWN, MODE_READ, MODE_WRITE } mode = MODE_UNKNOWN;
    int is_raw = 0;
    pid_t pid = 0;
    int view_all = 0;
    char *cp, *tgtstr, *offstr, *valstr;
    int option, firstval, n, c, fd, err = 0, modify;
    size_t writesz;
    char *meid_str = NULL;
    int ret;
    int is_frontdoor = 0;

    while ((c = getopt_long(argc, argv, "+hfn:l:w:Rvi:o:m:" VERSION_FLAG, options, &option)) >= 0) {
        switch (c) {
        case 'h':
            usage(program, 0);
            break;
        VERSION_CASE(program);
        case 'n':
            devnum = strtoul(optarg, &cp, 0);
            if ((cp == optarg) || (*cp != 0))
                usage(argv[0], 1);
            break;
        case 'l':
            len = strtoul(optarg, &cp, 0);
            if ((cp == optarg) || (*cp != 0))
                usage(program, 1);
            break;
        case 'w':
            wordsz = strtoul(optarg, &cp, 0);
            if ((cp == optarg) || (*cp != 0))
                usage(program, 1);
            if ((wordsz < 4 || (wordsz & 3) || (wordsz > MAX_WORD_SIZE)))
                usage(program, 1);
            break;
        case 'R':
            is_raw = 1;
            break;
        case 'f':
            is_frontdoor = 1;
            break;
        case 'i':
            if (mode != MODE_UNKNOWN)
                usage(program, 1);
            filename = optarg;
            if (strcmp(filename, "-") == 0)
                filename = NULL;
            mode = MODE_WRITE;
            break;
        case 'o':
            if (mode != MODE_UNKNOWN)
                usage(program, 1);
            filename = optarg;
            if (strcmp(filename, "-") == 0)
                filename = NULL;
            mode = MODE_READ;
            break;
        case 'v':
            view_all = 1;
            break;
        case 'm':
            meid_str = optarg;
            break;
        default:
            usage(argv[0], 1);
            break;
        }
    }

    if (optind == argc)
        usage(program, 1);

    /* Get target offset */
    tgtstr = argv[optind++];
    offstr = strchr(tgtstr, ':');
    if (!offstr)
        usage(program, 1);
    offstr[0] = 0;
    offstr++;
    offset = strtoull(offstr, NULL, 0);

    if (meid_str == NULL) {
        /* Get domain number (if present) */
        cp = tgtstr + strcspn(tgtstr, "0123456789");
        if (cp[0] != 0) {
            domain = strtoul(cp, NULL, 0);
            cp[0] = 'N';
            cp[1] = 0;
        } else {
            fprintf(stderr, "%s: No ME Specified\n", program);
            exit(EXIT_FAILURE);
        }
    } else {
        /* get MEID and set domain */
        if ((meid_str[0] == '0') && (meid_str[1] == 'x')) {
            domain = strtoull(meid_str, NULL, 0);
            /* Validate it's a valid MEID */
            if (NFP6000_MEID_IS_VALID(domain)) {
            } else {
                domain = -1;
            }
        } else {
            ret = nfp6000_idstr2meid(meid_str, NULL);
            if (ret < 0) {
                domain = -1;
            } else {
                domain = ret;
            }
        }
    }

    if (domain == -1) {
        fprintf(stderr, "%s: Invalid MEID: %s\n", program, meid_str);
        exit(EXIT_FAILURE);
    }

    /* Determine memory target */
    for (n = 0; n < ARRAY_SIZE(targets); n++) {
        if (strcmp(tgtstr, targets[n].name) == 0 ||
            (targets[n].alias && strcmp(tgtstr, targets[n].alias) == 0) ||
            (targets[n].alias2 && strcmp(tgtstr, targets[n].alias2) == 0)) {
            tgt = &targets[n];
            break;
        }
    }

    if (!tgt) {
        fprintf(stderr, "%s: Unknown target: %s\n", program, tgtstr);
        exit(EXIT_FAILURE);
    }

    /* XXX allow for shorthand notation for emem and imem
     *     e.g. emem0..2 = emem24..26
     *          imem0..1 = imem28..29
     */
    switch (tgt->type) {
        case MEM_EMEM:
            if (domain < 3) {
                tgt->domain = 24 + domain;
            } else {
                tgt->domain = domain;
            }
            break;
        case MEM_IMEM:
            if (domain < 2) {
                tgt->domain = 28 + domain;
            } else {
                tgt->domain = domain;
            }
            break;
        default:
            tgt->domain = domain;
            break;
    }

    if (wordsz == 0)
        wordsz = tgt->wordsize;

    if (wordsz < tgt->wordsize) {
        fprintf(stderr, "%s: Wordsize must be at least %d for %s target\n",
                program, tgt->wordsize, tgt->name);
        exit(EXIT_FAILURE);
    }

    if (len != 0) {
        if ((len & (wordsz - 1)) != 0) {
            fprintf(stderr, "%s: Length must be a multiple of the word size ("
                    "%ld bytes)\n", program, wordsz);
            exit(EXIT_FAILURE);
        }
    }

    nfp = nfp_device_open(devnum);
    if (!nfp) {
        fprintf(stderr, "Failed to open NFP device %ld: %s\n", devnum,
                strerror(errno));
        exit(EXIT_FAILURE);
    }

    dev_nfp_cpp = nfp_cpp_from_device_id(devnum);
    if (!dev_nfp_cpp) {
        fprintf(stderr, "error: nfp_cpp_from_device_id (%ld) failed\n", devnum);
        return EXIT_FAILURE;
    }

    if (is_frontdoor)
        nfp_sal_cpp_mode(dev_nfp_cpp, NFP_SAL_CPP_MODE_FRONTONLY);

    if (optind == argc) {
        /*
         * No extra parameters.  We are doing read/write on file
         * descriptor (possibly stdin/stdout).
         */

        if (mode == MODE_WRITE) {
            if (filename == NULL) {
                fd = 0;
            } else {
                fd = open(filename, O_RDONLY);
                if (fd < 0) {
                    perror(filename);
                    exit(EXIT_FAILURE);
                }
            }

            if (!is_raw) {
                fd = hexread(fd, &pid);
            } else if (isatty(fd)) {
                fprintf(stderr, "Cowardly refusing to perform binary I/O "
                        "with a tty.\n");
                fprintf(stderr, "Please redirect to a file or pipe.\n");
                exit(EXIT_FAILURE);
            }

            if (len == 0) {
                struct stat st;
                err = fstat(fd, &st);
                if (err == 0)
                    len = st.st_size;
            }

            mem_write_from_fd(nfp, tgt, fd, offset, len);
            if (pid != 0) {
                close(fd);
                waitpid(pid, NULL, 0);
            }
            return EXIT_SUCCESS;

        } else {
            if (filename == NULL) {
                fd = 1;
            } else {
                fd = open(filename, O_WRONLY | O_TRUNC | O_CREAT, 0660);
                if (fd < 0) {
                    perror(filename);
                    exit(EXIT_FAILURE);
                }
            }
            if (!is_raw) {
                fd = hexdump(fd, &pid, view_all, offset, wordsz);
            } else if (isatty(fd)) {
                fprintf(stderr, "Cowardly refusing to perform binary I/O "
                        "with a tty.\n");
                fprintf(stderr, "Please redirect to a file or pipe.\n");
                exit(EXIT_FAILURE);
            }

            if (len == 0)
                len = tgt->wordsize;

            mem_read_to_fd(nfp, tgt, fd, offset, len);
            if (pid != 0) {
                close(fd);
                waitpid(pid, NULL, 0);
            }
            return EXIT_SUCCESS;
        }
    }

    if (mode == MODE_READ)
        usage(program, 1);

    if (len != 0 && (len < (argc-optind) * wordsz)) {
        fprintf(stderr, "%s: Length %ld is less than values on command line\n",
                program, len);
        exit(EXIT_FAILURE);
    }

    firstval = optind;
    if (len == 0)
        len = (argc-optind) * wordsz;

    while (len > 0) {
        valstr = argv[optind++];
        modify = 0;

        if (wordsz == 4) {
            uint32_t val = 0, tmp;

            if (valstr[0] == '-' || valstr[0] == '+') {
                err = mem_read(nfp, tgt, &val, offset, sizeof(val));
                if (err < 0) {
                    perror("Read from NFP device");
                    exit(EXIT_FAILURE);
                } else if (err != sizeof(val)) {
                    fprintf(stderr, "Failed to read %d bytes from NFP device\n",
                            (int) sizeof(val));
                    exit(EXIT_FAILURE);
                }
                tmp = strtoul(valstr + 1, NULL, 0);
                modify = 1;
            } else {
                tmp = strtoul(valstr, NULL, 0);
            }

            switch (valstr[0]) {
            case '-': val &= ~tmp; break;
            case '+': val |= tmp; break;
            default: val = tmp; break;
            }

            if (firstval == argc-1 && !modify)
                writesz = len;
            else
                writesz = wordsz;

            err = mem_write(nfp, tgt, &val, sizeof(val), offset, writesz);
            if (err < 0) {
                perror("Write to NFP device");
                exit(EXIT_FAILURE);
            } else if (err != writesz) {
                fprintf(stderr, "Failed to write %d bytes to NFP device\n",
                        (int) writesz);
                exit(EXIT_FAILURE);
            }

        } else {
            uint64_t val = 0, tmp;

            assert(wordsz == 8);
            if (valstr[0] == '-' || valstr[0] == '+') {
                err = mem_read(nfp, tgt, &val, offset, sizeof(val));
                if (err < 0) {
                    perror("Read from NFP device");
                    exit(EXIT_FAILURE);
                } else if (err != sizeof(val)) {
                    fprintf(stderr, "Failed to read %d bytes from NFP device\n",
                            (int) sizeof(val));
                    exit(EXIT_FAILURE);
                }
                tmp = strtoull(valstr + 1, NULL, 0);
                modify = 1;
            } else {
                tmp = strtoull(valstr, NULL, 0);
            }

            switch (valstr[0]) {
            case '-': val &= ~tmp; break;
            case '+': val |= tmp; break;
            default: val = tmp; break;
            }

            if (firstval == argc-1 && !modify)
                writesz = len;
            else
                writesz = wordsz;

            err = mem_write(nfp, tgt, &val, sizeof(val), offset, writesz);
            if (err < 0) {
                perror("Write to NFP device");
                exit(EXIT_FAILURE);
            } else if (err != writesz) {
                fprintf(stderr, "Failed to write %d bytes to NFP device\n",
                        (int) writesz);
                exit(EXIT_FAILURE);
            }
        }

        offset += writesz;
        len -= writesz;
        if (optind == argc)
            optind = firstval;
    }

    nfp_device_close(nfp);
    return EXIT_SUCCESS;
}
