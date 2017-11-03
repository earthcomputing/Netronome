/**
 * Copyright (C) 2013 Netronome Systems, Inc.  All rights reserved.
 *
 * Example configuration/setup of the NBI DMA for MEs and MU
 *
 */

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>

#include <sys/stat.h>

#include <nfp-common/nfp_platform.h>
#include <nfp-common/nfp_chipdata.h>
#include <nfp-common/ns_log.h>
#include <nfp-common/nfp_platform.h>

#include <nfp.h>
#include <nfp_cpp.h>
#include <nfp_sal.h>
#include <nfp_nbi_dma.h>

/***** Defines *****/

/* default args */
#define ARG_DEFAULT_DEVICE      0
#define ARG_DEFAULT_NBI         0
#define ARG_DEFAULT_ME          32
#define ARG_DEFAULT_MU          25
#define ARG_DEFAULT_CTMOFF      1

#define NBI0 0
#define NBI1 1

#define MAX_ISLANDS 64

struct dmasetup_data {
    uint32_t arg_device;
    uint32_t arg_nbi;
    uint32_t arg_nislands;
    uint32_t arg_ctmoff;
    uint32_t arg_mu;
    uint32_t arg_dumponly;
    int arg_verbose;
    int me_list[MAX_ISLANDS];
    int isl_id;
    struct nfp_device *nfp;
    struct nfp_nbi_dev *nfp_nbi;
    const struct nfp_chipdata_chip *chip;
};

/***** Command line argument handling *****/

/**
 * Parse a string of numbers with - and , as separators
 *
 * e.g. 4,28-32,49
 *
 * @param str           Number string to parse, with , and - as separators
 * @param buf           Return buffer
 * @param max_buffer    Maximum size of buffer
 *
 * @return 1, or -1 on failure
 */
int process_num_str(char *str, int *buf, int max_buf)
{
    char ch[64];
    int i;
    int ch_num;
    int ch_prev;
    char chr;
    int error = 0;
    int retval;
    int index;
    int ch_index;
    int min, max;

    index = 0;
    ch_index = 0;
    ch_prev = -1;
    min = max = 0;

    while (((chr = (char) *str++) != '\0') && (error == 0)) {
        switch (chr) {
        case ',':
            if (index == 0) {
                error = 1;
            } else {
                ch[ch_index] = 0;
                ch_num = atoi(ch);
                if (ch_num > max_buf) {
                    error = 1;
                } else {
                    /* Must check if we have previously had a number */
                    if (ch_prev != -1) {
                        /* We go from previous number to the ch_num */
                        min = NFP_MIN(ch_prev, ch_num);
                        max = NFP_MAX(ch_prev, ch_num);
                        for (i = min; i <= max;i++) {
                            buf[i] = 1;
                            ch_prev = -1;
                        }
                    } else {
                        buf[ch_num] = 1;
                    }
                }
                ch_index = 0;
                ch[ch_index] = 0;
            }
            break;
        case '-':
            if (index == 0) {
                error = 1;
            } else {
                ch[ch_index] = 0;
                ch_num = atoi(ch);
                if (ch_num > max_buf) {
                    error = 1;
                } else {
                    ch_prev = ch_num;
                }
                ch_index = 0;
            }
            break;
        case '0': case '1': case '2': case '3':
        case '4': case '5': case '6': case '7':
        case '8': case '9':
            ch[ch_index] = chr;
            ch_index++;
            break;
        default:
            error = 1;
            break;
        }
        index++;
    }

    /* Handle the end of the string! */
    if (ch_index != 0) {
        ch[ch_index] = 0;
        ch_num = atoi(ch);
        if (ch_num > max_buf) {
            error = 1;
        } else {
            /* Must check if we have previously had a number */
            if (ch_prev != -1) {
                /* We go from previous number to the ch_num */
                for (i = ch_prev; i <= ch_num;i++) {
                    buf[i] = 1;
                    ch_prev = -1;
                }
            } else {
                buf[ch_num] = 1;
            }
        }
    }
    if (error == 1) {
        retval = -1;
    } else {
        retval = 1;
    }
    return retval;
}

static void usage(char *progname)
{
    fprintf(stderr,
            "usage: %s [options]\n"
            "\nWhere options are:\n"
            "        -d <device>            -- select NFP device; default 0\n"
            "        -n <nbi>               -- select NBI; default 0\n"
            "        -I <list of islands>   -- list of islands (separated with , or -); default 32\n"
            "        -m <mu island>         -- MU island; default 25\n"
            "        -o <offset_value>      -- value to use as ctm_offset (default 1:64B)\n"
            "        -c                     -- generate cscript output, directed to stdout\n"
            "        -D                     -- dump config and status only\n\n"
            "        -h                     -- print help\n\n"
            "example usage:\n"
            "        %s -n 0 -I 32,33,34 -m 25     -- setup nbi0 dma for ME islands 32,33,34 using MU island 25\n"
            "        %s -n 1 -I 36,37,38 -m 26     -- setup nbi1 dma for ME islands 36,37,38 using MU island 26\n"
            , progname, progname, progname);
}

static int uintarg(const char *s, uint64_t *val)
{
    char *cp;
    uint64_t r;
    r = nfp_strtou64(s, &cp, 0);
    if (cp == s) {
        fprintf(stderr, "Invalid integer value: %s\n", s);
        return -1;
    }
    *val = r;
    return 0;
}

static void parse_options(struct dmasetup_data *data, int argc, char **argv)
{
    int o, i;
    uint64_t val;
    int retval;

    data->arg_device = ARG_DEFAULT_DEVICE;
    data->arg_nbi = ARG_DEFAULT_NBI;
    data->arg_mu = ARG_DEFAULT_MU;
    data->arg_dumponly = 0;
    data->arg_ctmoff = ARG_DEFAULT_CTMOFF;

    /* Default the ME list */
    for (i = 0; i < MAX_ISLANDS; i++)
        data->me_list[i] = 0;
    data->me_list[ARG_DEFAULT_ME] = 1;

    while ((o = getopt(argc, argv, "d:n:I:m:o:cvhD")) != -1) {
        switch(o) {
        case 'D':
            data->arg_dumponly = 1;
            break;
        case 'd':
            if (uintarg(optarg, &val)) {
                usage(argv[0]);
                exit(1);
            }
            data->arg_device = (uint32_t) val;
            break;
        case 'n':
            if (uintarg(optarg, &val)) {
                usage(argv[0]);
                exit(1);
            }
            data->arg_nbi = (uint32_t) val;
            break;
        case 'I':
            data->me_list[ARG_DEFAULT_ME] = 0;
            retval = process_num_str(optarg, data->me_list, MAX_ISLANDS);
            if (retval < 1) {
                usage(argv[0]);
                exit(1);
            }
            break;
        case 'm':
            if (uintarg(optarg, &val)) {
                usage(argv[0]);
                exit(1);
            }
            data->arg_mu = (uint32_t) val;
            break;
        case 'o':
            if (uintarg(optarg, &val)) {
                usage(argv[0]);
                exit(1);
            }
            data->arg_ctmoff = (uint32_t) val;
            break;
        case 'h':
            usage(argv[0]);
            exit(0);
            break;
        default:
            usage(argv[0]);
            exit(1);
            break;
        }
    }
}

/***** NBI DMA Config *****/

int nbi_dma_config(struct dmasetup_data *d)
{
    struct nfp_nbi_dma_config cfg;
    struct nfp_nbi_dma_blq_event_config evt;
    int i;
    int ret;

    cfg.ctm_poll_search_enable = 1;
    cfg.rate_limit_enable = 0;
    cfg.ctm_poll_interval = 3; /* 512us */
    cfg.ctm_poll_enable = 1;
    cfg.nbi_number = d->isl_id;
    cfg.dis_rx_blq_wr_in_err = 0;
    cfg.dis_rx_alloc_in_err = 0;
    cfg.dis_rx_push_last_err = 0;
    cfg.dis_buf_rd_err = 0;
    cfg.dis_bd_ram_err = 0;
    cfg.dis_ds_ram_err = 0;
    cfg.dis_bc_ram_err = 0;
    cfg.dis_push_bus_select = 0;
    cfg.credit_rate0 = 3;
    cfg.credit_rate1 = 3;
    cfg.credit_rate2 = 3;
    cfg.credit_rate3 = 3;

    ret = nfp_nbi_dma_cfg_nbidma(d->nfp_nbi, &cfg);
    if (ret < 0) {
        fprintf(stderr, "nfp_nbi_dma_cfg_nbidma() failed (%s)\n",
                strerror(errno));
        return -1;
    }

    for (i = 0; i < 3; i++) {
        evt.blq = i;
        evt.blq_event_status_enable = 1;
        evt.blq_event_not_full_enable = 1;
        evt.blq_event_threshold = 0;
        ret = nfp_nbi_dma_cfg_blq_event(d->nfp_nbi, &evt);
        if (ret < 0) {
            fprintf(stderr, "nfp_nbi_dma_cfg_blq_event() failed (%s)\n",
                    strerror(errno));
            return -1;
        }
    }

    return 0;
}

void show_bp_config(struct nfp_nbi_dma_bp_config *bp_cfg)
{
    int j;

    printf("Buffer Pool Config %d:\n", bp_cfg->bp);
    printf("    .drop_enable           = %d\n", bp_cfg->drop_enable);
    printf("    .ctm_offset            = %d\n", bp_cfg->ctm_offset);
    printf("    .primary_buffer_list   = %d\n", bp_cfg->primary_buffer_list);
    printf("    .secondary_buffer_list = %d\n", bp_cfg->secondary_buffer_list);
    printf("    .ctm_split_length      = %d\n", bp_cfg->ctm_split_length);
    printf("    .bpe_head              = %d\n", bp_cfg->bpe_head);
    printf("    .bpe_chain_end         = %d\n", bp_cfg->bpe_chain_end);
    //cnt = d->bp_cfg[i].bpe_chain_end - d->bp_cfg[i].bpe_head + 1;
    for (j = 0 ; j <= bp_cfg->bpe_head - bp_cfg->bpe_chain_end + 1; j++) {
        printf("    Buffer Pool Entry [%d]:\n", j);
        printf("        .bpe            = %d\n", bp_cfg->bpe_chain[j].bpe);
        printf("        .ctm_target     = %d\n", bp_cfg->bpe_chain[j].ctm_target);
        printf("        .packet_credits = %d\n", bp_cfg->bpe_chain[j].packet_credits);
        printf("        .buffer_credits = %d\n", bp_cfg->bpe_chain[j].buffer_credits);
    }


}

int nbi_bp_config(struct dmasetup_data *d)
{
    struct nfp_nbi_dma_bp_config bp_cfg;
    struct nfp_nbi_dma_bpe_config bpechain[32];
    int i, j;

    if (nfp_nbi_dma_reset_bpechained(d->nfp_nbi) < 0) {
        fprintf(stderr,
                "nfp_nbi_dma_reset_bpechained() (%s)\n", strerror(errno));
        return -1;
    }

    for (i = 0; i < 7; i++) {
        bp_cfg.bp = i;
        bp_cfg.drop_enable = 0;
        bp_cfg.ctm_offset = d->arg_ctmoff;
        bp_cfg.primary_buffer_list = 0;
        bp_cfg.secondary_buffer_list = 0;
        bp_cfg.ctm_split_length = 3;
        //bp_cfg.bpe_head = 0;
        //bp_cfg.bpe_chain_end = me_isl_cnt - 1;
        bp_cfg.bpe_head = i;
        bp_cfg.bpe_chain_end = i;
        bp_cfg.bpe_chain = &bpechain[i];
        //bp_cfg.bpe_head = 0;
        //bp_cfg.bpe_chain_end = 0;
        //bp_cfg.bpe_chain = &bpechain[0];
        for (j = 0; j < 7; j++) {
            bpechain[j].bpe = j;
            bpechain[j].ctm_target = ARG_DEFAULT_ME + i;
            bpechain[j].packet_credits = 128;
            bpechain[j].buffer_credits = (256 * 1024) / 2048;
        }
        
        show_bp_config(&bp_cfg) ;

        if (nfp_nbi_dma_cfg_bp(d->nfp_nbi, &bp_cfg) < 0) {
            fprintf(stderr, "nfp_nbi_dma_cfg_bp() failed (%s)\n",
                    strerror(errno));
            return -1;
        }
    }

    return 0;
}

int nbi_blq_config(struct dmasetup_data  *d)
{
    struct nfp_nbi_dma_blq_config blq_cfg;
#ifdef INIT_BDSRAM
    struct nfp_nbi_dma_bdsram_config bdsram_cfg;
    uint64_t baseaddr;
    uint32_t basecnt;
    uint32_t basefill;
#endif
    int bufcnt, bufsize = 2048;
    int memtype;

    memtype = nfp_chipdata_island_mu_type(d->chip, d->arg_mu);
    switch (memtype) {
    case NFP_CHIPDATA_MU_TYPE_IMEM:
    case NFP_CHIPDATA_MU_TYPE_EMEM:
        break;
    default:
        fprintf(stderr, "warning island %d for BLQ memory\n", d->arg_mu);
        //return -1;
    }

    bufcnt = 4096;

    /* only one blq */
    blq_cfg.blq = 0;
    blq_cfg.head = 0;
    blq_cfg.tail = bufcnt - 1;
    blq_cfg.size = 3; /* 4k */
    blq_cfg.buffer_size = bufsize / 2048;
#ifdef INIT_BDSRAM
    blq_cfg.bdsram_cfg = &bdsram_cfg;

    /* offset is 0 */
    baseaddr = (0x8000000000 | ((uint64_t) d->arg_mu << 32));
    baseaddr /= 2048;
    basecnt = bufcnt;
    basefill = 0;

    bdsram_cfg.bc_mu_base_addr = &baseaddr;
    bdsram_cfg.bc_total_cnt = &basecnt;
    bdsram_cfg.bc_fill_ratio = &basefill;
    bdsram_cfg.bc_entries = 1;
#else
    blq_cfg.bdsram_cfg = NULL;
#endif

    if (nfp_nbi_dma_cfg_blq(d->nfp_nbi, &blq_cfg) < 0) {
        fprintf(stderr, "nfp_nbi_dma_cfg_blq() failed (%s)\n", strerror(errno));
        return -1;
    }

    return 0;
}

int do_config(struct dmasetup_data *d)
{
    int ret;

    ret = nbi_dma_config(d);
    if (ret < 0) {
        fprintf(stderr, "nbi dma config failed\n");
        return -1;
    }

    ret = nbi_bp_config(d);
    if (ret < 0) {
        fprintf(stderr, "NBI DMA buffer pool config failed\n");
        return -1;
    }

    ret = nbi_blq_config(d);
    if (ret < 0) {
        fprintf(stderr, "NBI buffer list config failed\n");
        return -1;
    }

    return 0;
}

int dump_config(struct dmasetup_data *d)
{
    uint64_t bdsram[4096];
    struct nfp_nbi_dma_blq_status blq_status;
    struct nfp_nbi_dma_bp_config bp_cfg;
    struct nfp_nbi_dma_bpe_config bpe_chain[32];
    int cnt;
    int i, j;

    //for (i = 0; i < 8; i++) {
    for (i = 0; i < 7; i++) {
        bp_cfg.bpe_chain = bpe_chain;
        if (nfp_nbi_dma_read_bp(d->nfp_nbi, i, &bp_cfg) < 0) {
            fprintf(stderr, "nfp_nbi_dma_read_bp failed (%s)", strerror(errno));
            return -1;
        }
        printf("Buffer Pool Config %d:\n", i);
        printf("    .drop_enable           = %d\n", bp_cfg.drop_enable);
        printf("    .ctm_offset            = %d\n", bp_cfg.ctm_offset);
        printf("    .primary_buffer_list   = %d\n", bp_cfg.primary_buffer_list);
        printf("    .secondary_buffer_list = %d\n", bp_cfg.secondary_buffer_list);
        printf("    .ctm_split_length      = %d\n", bp_cfg.ctm_split_length);
        printf("    .bpe_head              = %d\n", bp_cfg.bpe_head);
        printf("    .bpe_chain_end         = %d\n", bp_cfg.bpe_chain_end);
        cnt = bp_cfg.bpe_chain_end - bp_cfg.bpe_head + 1;
        for (j = 0; j < cnt; j++) {
            printf("    Buffer Pool Entry [%d]:\n", j);
            printf("        .bpe            = %d\n", bpe_chain[j].bpe);
            printf("        .ctm_target     = %d\n", bpe_chain[j].ctm_target);
            printf("        .packet_credits = %d\n", bpe_chain[j].packet_credits);
            printf("        .buffer_credits = %d\n", bpe_chain[j].buffer_credits);
        }
    }

    for (i = 0; i < 4; i++) {
        if (nfp_nbi_dma_read_blq_status(d->nfp_nbi, i, &blq_status) < 0) {
            fprintf(stderr,
                    "nfp_nbi_dma_read_blq_status failed (%s)",
                    strerror(errno));
            return -1;
        }
        printf("Buffer List Queue[%d]:\n", i);
        printf("    .blq                  = %d\n", blq_status.blq);
        printf("    .head                 = %d\n", blq_status.head);
        printf("    .tail                 = %d\n", blq_status.tail);
        printf("    .size                 = %d\n", blq_status.size);
        printf("    .blq_evnt_full_cnt    = %d\n", blq_status.blq_evnt_full_cnt);
        printf("    .blq_bdfifo_entry_cnt = %d\n", blq_status.blq_bdfifo_entry_cnt);
        printf("    .blq_entry_cnt        = %d\n", blq_status.blq_entry_cnt);
        printf("    .blq_full             = %d\n", blq_status.blq_full);
        printf("    .blq_empty            = %d\n", blq_status.blq_empty);
    }

    if (nfp_nbi_dma_read_bdsram(d->nfp_nbi, 0, 4096, bdsram) < 0) {
        fprintf(stderr,
                "nfp_nbi_dma_read_bdsram failed (%s)\n", strerror(errno));
        return -1;
    }

    {
        int diff, diff_match, skips;
        uint64_t m = 0x1fffffff;

        printf("Buffer Descriptor RAM:\n");
        skips = 0;
        for (i = 0; i < 4096; i++) {
            diff_match = 0;
            diff = 0;
            for (j = i; j >= 1; j--) {
                if (j == i) {
                    diff = (bdsram[j] & m) - (bdsram[j - 1] & m);
                    continue;
                }
                if ((bdsram[j] & m) - (bdsram[j - 1] & m) == diff)
                    diff_match++;
                else
                    break;
            }

            if (diff_match <= 2) {
                printf("    0x0%4x : %lx\n", i, bdsram[i] & m);
                skips = 0;
            } else {
                if (skips == 0) {
                    printf("...\n");
                }
                skips++;
            }
        }
    }

    return 0;
}

int main(int argc, char **argv)
{
    struct dmasetup_data data;
    int isltype;
    int n;
    int i;

    memset(&data, '\0', sizeof(data));

    parse_options(&data, argc, argv);

    NS_LOG_SET_LEVEL(NS_LOG_LVL_INFO);

    data.nfp = nfp_device_open(data.arg_device);
    if (!data.nfp) {
        fprintf(stderr, "error: nfp_device_open failed (%s)\n",
                strerror(errno));
        return EXIT_FAILURE;
    }

    data.nfp_nbi = nfp_nbi_open(data.nfp, data.arg_nbi);
    if (!data.nfp_nbi) {
        fprintf(stderr, "error: nfp_nbi_open failed (%s)\n", strerror(errno));
        return EXIT_FAILURE;
    }

    data.chip = nfp_device_chip(data.nfp);
    if (!data.chip) {
        fprintf(stderr,
                "error: nfp_device_chip failed (%s)\n",
                strerror(errno));
        return EXIT_FAILURE;
    }

    /* figure out the island ID for the given nbi number */
    data.isl_id = -1;
    n = 0;
    isltype = NFP_CHIPDATA_ISLD_TYPE_NBI;
    for (i = nfp_chipdata_island_id_type_first(data.chip, isltype);
            i != -1;
            i = nfp_chipdata_island_id_type_next(data.chip, i, isltype), n++) {
         if (n == data.arg_nbi) {
             data.isl_id = i;
             break;
         }
    }

    if (data.isl_id == -1) {
        fprintf(stderr, "chip does not contain nbi %d\n", data.arg_nbi);
        return EXIT_FAILURE;
    }

    if (data.arg_dumponly) {
        if (dump_config(&data) < 0) {
            fprintf(stderr, "status dump failed\n");
            return EXIT_FAILURE;
        }
    } else {
        if (do_config(&data) < 0) {
            fprintf(stderr, "configuration failed\n");
            return EXIT_FAILURE;
        }
    }

    nfp_nbi_close(data.nfp_nbi);
    nfp_device_close(data.nfp);

    return EXIT_SUCCESS;
}
