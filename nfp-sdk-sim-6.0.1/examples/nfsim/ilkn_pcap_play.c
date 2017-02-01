/**
 * Copyright (C) 2013 Netronome Systems, Inc.  All rights reserved.
 *
 * Example code to Play/Inject a PCAP file into an Ingress Interlaken port
 *
 */

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <byteswap.h>
#include <inttypes.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <pcap/pcap.h>

#include <nfp-common/nfp_chipdata.h>
#include <nfp-common/nfp_regdata.h>
#include <nfp-common/ns_log.h>
#include <nfp.h>
#include <nfp_sal.h>

#include <ilkn_bus.h>

#define ARG_DEFAULT_DEVICE   0
#define ARG_DEFAULT_NBI      0
#define ARG_DEFAULT_PORT     0
#define ARG_DEFAULT_CHNL     0
#define ARG_DEFAULT_LOOPS    1
#define ARG_DEFAULT_USETIME  0
#define ARG_DEFAULT_WAITTIME 0
#define ARG_DEFAULT_USASNS   0
#define ARG_DEFAULT_USERATE  0
#define ARG_DEFAULT_RATEMBPS 0

#undef MIN
#undef MAX
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) < (y) ? (y) : (x))

#define SERDES_RATE 12.5e9
#define ILKN_FRM_OH_NUM 64
#define ILKN_FRM_OH_DEN 67

struct pcap_play_data {
    int arg_device; /* which nfp_device to use */
    int arg_nbi;
    int arg_port;
    int arg_chnl;
    int arg_loops;
    int arg_usetime;
    int arg_usasns;
    uint64_t arg_waittime;
    int arg_userate;
    int arg_ratembps;

    int bmax;
    int bmin;
    int bshort;
    int lanes_active;
    double rx_rate_act_bpps;
    uint64_t first_time;
    uint64_t start_time;

    int total_real_bytes;
    int total_oh_bytes;

    struct nfp_device *nfp;
    struct nfp_cpp *nfp_cpp;
    struct nfp_chipdata *chipdata;
    struct nfp_regdata *reg_data;

    char *pcap_filename;
    struct pcap_pkthdr ph;

};

static void usage(char *progname)
{
    fprintf(stderr,
            "usage: %s [options] <pcapfile>\n"
            "\nWhere options are:\n"
            "        -d <device>   -- select NFP device\n"
            "        -n <nbi>      -- select NBI\n"
            "        -p <port>     -- select NBI Interlaken port {0|1}\n"
            "        -c <channel>  -- select Interlaken channel {0..63}\n"
            "        -l <loops>    -- number of times pcap file is played\n"
            "        -w <time>     -- sim time in picoseconds to wait before\n"
            "                         first packet is sent\n"
            "        -t            -- use pcap time for sending\n"
            "        -u            -- interpret microsecond as nanoseconds\n"
            "                         for the pcap file\n"
            "        -r <rate>     -- specify ingress rate in mbit/s\n"
            "and arguments are:\n"
            "        <pcapfile>  -- the pcap file to play\n",
            progname);
}

static int uintarg(const char *s, uint64_t *val)
{
    char *cp;
    uint64_t r;
    r = strtoull(s, &cp, 0);
    if (cp == s) {
        fprintf(stderr, "Invalid integer value: %s\n", s);
        return -1;
    }
    *val = r;
    return 0;
}

static void parse_options(struct pcap_play_data* pp_data, int argc, char *argv[])
{
    int o;
    uint64_t val;

    pp_data->arg_device = ARG_DEFAULT_DEVICE;
    pp_data->arg_nbi = ARG_DEFAULT_NBI;
    pp_data->arg_port = ARG_DEFAULT_PORT;
    pp_data->arg_chnl = ARG_DEFAULT_CHNL;
    pp_data->arg_loops = ARG_DEFAULT_LOOPS;
    pp_data->arg_usetime = ARG_DEFAULT_USETIME;
    pp_data->arg_waittime = ARG_DEFAULT_WAITTIME;
    pp_data->arg_usasns = ARG_DEFAULT_USASNS;
    pp_data->arg_ratembps = ARG_DEFAULT_RATEMBPS;
    pp_data->arg_userate = ARG_DEFAULT_USERATE;

    while ((o = getopt(argc, argv, "w:d:n:p:c:l:r:htu")) != -1) {
        switch(o) {
        case 'h':
            usage(argv[0]);
            exit(0);
            break;
        case 't':
            pp_data->arg_usetime = 1;
            break;
        case 'u':
            pp_data->arg_usasns = 1;
            break;
        case 'w':
            if (uintarg(optarg, &val)) {
                usage(argv[0]);
                exit(1);
            }
            pp_data->arg_waittime = val;
            break;
        case 'd':
            if (uintarg(optarg, &val)) {
                usage(argv[0]);
                exit(1);
            }
            pp_data->arg_device = val;
            break;
        case 'n':
            if (uintarg(optarg, &val)) {
                usage(argv[0]);
                exit(1);
            }
            pp_data->arg_nbi = val;
            break;
        case 'p':
            if (uintarg(optarg, &val)) {
                usage(argv[0]);
                exit(1);
            }
            pp_data->arg_port = val;
            break;
        case 'c':
            if (uintarg(optarg, &val)) {
                usage(argv[0]);
                exit(1);
            }
            pp_data->arg_chnl = val;
            break;
        case 'l':
            if (uintarg(optarg, &val)) {
                usage(argv[0]);
                exit(1);
            }
            pp_data->arg_loops = val;
            break;
        case 'r':
            if (uintarg(optarg, &val)) {
                usage(argv[0]);
                exit(1);
            }
            pp_data->arg_ratembps = val;
            pp_data->arg_userate = 1;
            break;
        default:
            usage(argv[0]);
            exit(1);
            break;
        }
    }

    if ((pp_data->arg_usetime == 1) && (pp_data->arg_userate == 1)) {
        printf("WARNING: -t and -r specified!\n"
                "-t will be ignored\n");
        pp_data->arg_usetime = 0;
        pp_data->arg_usasns = 0;
    }

    if ((pp_data->arg_ratembps == 0) && (pp_data->arg_userate == 1)) {
        fprintf(stderr, "must specify non-zero rate with option -r\n");
        usage(argv[0]);
        exit(1);
    }

    if (argc  - optind != 1) {
        fprintf(stderr, "must specify a pcap filename\n");
        usage(argv[0]);
        exit(1);
    }

    pp_data->pcap_filename = argv[optind];
}



/*
 * populate the interlaken buffer structure for nfp_sal call
 */
static int ilkn_buff_popl(struct pcap_play_data *pp_data,
                          const unsigned char *pktdata,
                          int start,
                          struct ilkn_ctrl frame,
                          struct ilkn_buffer * buffer)
{
    // interlaken interface is big endian over 32B data word (data_3..data_0)
    // reverse bytes for host LE storage in uint64_t's
    // ** warning this works only because data words are at start of packed structure
    union ilkn_buffer_u ilkn_buff;
    int bytes_remaining = frame.sz, bytes = 0;
    int i,j=0;

    while (bytes_remaining > 0)
    {
        for (i=7; i>=0 && bytes_remaining > 0; i--)
        {
            ilkn_buff.parray[j+i] = *(pktdata + start + bytes);
            bytes_remaining--;
            bytes++;
        }
        j += 8;
    }

    ilkn_buff.pstruct.valid = 1;
    ilkn_buff.pstruct.sop = frame.sop;
    ilkn_buff.pstruct.eop = frame.eop;
    ilkn_buff.pstruct.sob = frame.sob;
    ilkn_buff.pstruct.eob = frame.eob;
    ilkn_buff.pstruct.chan = pp_data->arg_chnl;

    // sz of 0 indicates all bytes valid
    ilkn_buff.pstruct.sz = frame.sz % ILKN_WORD_SZ;

    // reset remaining fields
    ilkn_buff.pstruct.app0 = 0;
    ilkn_buff.pstruct.app1 = 0;
    ilkn_buff.pstruct.chanext = 0;
    ilkn_buff.pstruct.eop_err = 0;
    ilkn_buff.pstruct.multiple_use = 0;

    *buffer = ilkn_buff.pstruct;

    return 0;
}


/*
 * print the interlaken buffer structure used in nfp_sal call
 */
#if 0
static void ilkn_buff_print(struct ilkn_buffer * ilkn_buff)
{

    printf("sop %d, sob %d, eob %d, eop %d, chan %d, \
        data %016"PRIx64"%016"PRIx64"%016"PRIx64"%016"PRIx64", sz = %d\n",
        ilkn_buff->sop, ilkn_buff->sob, ilkn_buff->eob, ilkn_buff->eop,
        ilkn_buff->chan, ilkn_buff->data_3, ilkn_buff->data_2, ilkn_buff->data_1,
        ilkn_buff->data_0, ilkn_buff->sz);

}
#endif


/*
 * calculate true chip rx rate after all overhead compensated
 */
void ilkn_calc_start_time(struct pcap_play_data *pp_data)
{


    if (pp_data->arg_usetime) {
        // use pcap timing as start time
        // (note this will mean bursts are back to back, but pkts spaced correctly)
        if (pp_data->arg_usasns) {
            // interpret us as ns
            pp_data->start_time = pp_data->ph.ts.tv_sec * (1e12/1e3) +
                        pp_data->ph.ts.tv_usec * (1e6/1e3);
        } else {
            pp_data->start_time = pp_data->ph.ts.tv_sec * 1e12 +
                        pp_data->ph.ts.tv_usec * 1e6;
        }
    } else if (pp_data->arg_userate) {
        // baseline start time @ first transmission
        pp_data->start_time = pp_data->first_time;

        // use non-overhead bytes and rate to calc start time
        if (pp_data->total_real_bytes > 0) {
            // start time in ps is, baseline + bits tx'd / rate in bits per "ps"
            pp_data->start_time += (8 * pp_data->total_real_bytes) /
                                   (pp_data->arg_ratembps / 1e6);
        }
    } else
        pp_data->start_time = 0; // send immediately

#if 0
    printf("first_time = %lu ps, start_time = %lu ps, OHB %d, RB %d \n",
        pp_data->first_time, pp_data->start_time, pp_data->total_oh_bytes,
        pp_data->total_real_bytes);
#endif
}


/*
 * generate ilkn bursts from pcap packet
 */
static int ilkn_gen_brsts(struct pcap_play_data *pp_data,
                          const unsigned char *pktdata,
                          int bytes)
{
    int bytes_sent = 0;
    struct ilkn_buffer buffer;
    struct ilkn_ctrl frame;
    int i;
    int full_brst_cnt = bytes / pp_data->bmax;
    int pen_brst_sz = 0;
    int last_brst_sz = bytes % pp_data->bmax;
    int total_brst_cnt;

    frame.sop = 1;

    // split packet into bursts
    // adjust last & penultimate burst if necessary/possible to burst min to
    // prevent burst short word padding overhead
    if (last_brst_sz < pp_data->bmin && last_brst_sz > 0 && full_brst_cnt > 0) {
        pen_brst_sz = pp_data->bmax - (pp_data->bmin - last_brst_sz);
        last_brst_sz = pp_data->bmin;
    }

    total_brst_cnt = full_brst_cnt + (last_brst_sz > 0);
    for (i = 0; i < total_brst_cnt; i++) {
        int burst_size;
        int j, iwords;
        // each loop is a burst
        frame.sob = 1;
        frame.eob = 0;
        frame.eop = 0;

        burst_size = pp_data->bmax;

        // if penultimate burst, adjust burst size if needed
        if (i == total_brst_cnt - 2 && pen_brst_sz > 0)
            burst_size = pen_brst_sz;

        // if final burst, adjust burst size as need
        if (i == total_brst_cnt - 1 && last_brst_sz > 0)
            burst_size = last_brst_sz;

        // calculate number of nfp interlaken words per burst
        iwords = burst_size / ILKN_WORD_SZ + ((burst_size % ILKN_WORD_SZ) > 0);
        for (j = 0; j < iwords; j++) {
            int ret = 0;
            // each loop is an nfp ilkn word (up to 32B)
            if (j == iwords - 1) {
                frame.eob = 1;
                if (i == total_brst_cnt - 1)
                    frame.eop = 1;
            }

            frame.sz = MIN(ILKN_WORD_SZ,burst_size);
            ilkn_buff_popl(pp_data, pktdata, bytes_sent, frame, &buffer);
            // ilkn_buff_print(&buffer);
            ilkn_calc_start_time(pp_data);

            // send ilkn buffer to the server
            ret = nfp_sal_ilkn_ingress(pp_data->nfp, pp_data->arg_nbi,
                pp_data->arg_port, (const void *) &buffer, sizeof(buffer),
                pp_data->start_time);

            if (ret < 0) {
                fprintf(stderr, "nfp_sal_ilkn_ingress() failed (%s)\n",
                        strerror(errno));
                exit(1);
            }

            burst_size -= frame.sz;
            bytes_sent += frame.sz;

            // update real/overhead bytes sent
            pp_data->total_real_bytes += frame.sz;
            // 1x8B word per burst
            if(frame.sob)
                pp_data->total_oh_bytes += 8;
            // last burst needs to be padded to multiple of 8B
            pp_data->total_oh_bytes += (8 - (frame.sz % 8)) & 7;

            // clear start flags after first burst
            frame.sop = 0;
            frame.sob = 0;
        }
    }

    return 0;
}

/*
 * XPB CSR Read using Full Qualified Name
 */
int xpb_fqn_rd(struct nfp_cpp *nfp_cpp, struct nfp_regdata *reg_data,
    const char *reg_name, uint32_t * data)
{
    int match_cnt;
    char *matches = NULL;
    struct nfp_regdata_node reg;

    // check if register string matches a chip CSR
    match_cnt = nfp_regdata_matches(reg_data, reg_name,
                                    &matches, NFP_REGDATA_FLG_FQN_ONLY);
    if (match_cnt <= 0) {
        fprintf(stderr, "ERROR: CSR FQN Match error for: %s.\n", reg_name);
        return 0;
    }

    // get register structure for this csr
    nfp_regdata_lookup(reg_data, reg_name, NFP_REGDATA_FLG_FQN_ONLY, &reg);

    nfp_xpb_readl(nfp_cpp, reg.offset, data);

    return 1;
}


/*
 * Get handle to Sim + other useful structures
 */
int acquire_sim_hndls(struct pcap_play_data * pp_data)
{
    const struct nfp_chipdata_chip *chip = NULL;
    const char * chip_id = "nfp-6xxxc";

    pp_data->nfp = nfp_device_open(pp_data->arg_device);
    if (!pp_data->nfp) {
        fprintf(stderr, "failed to open nfp device: %s\n", strerror(errno));
        exit(1);
    }

    printf("opening nfp_cpp\n");
    pp_data->nfp_cpp = nfp_cpp_from_device_id(0);
    if (!pp_data->nfp_cpp) {
        fprintf(stderr, "ERROR:  error: nfp_cpp_from_device_id failed\n");
        return 0;
    }

    // Load chip data from install location
    pp_data->chipdata = nfp_chipdata_load(NULL, NULL);
    if (pp_data->chipdata == NULL) {
        fprintf(stderr, "ERROR: Could not load chip data.\n");
        return 0;
    }

    chip = nfp_chipdata_select(pp_data->chipdata, chip_id);
    if (chip == NULL) {
        fprintf(stderr, "ERROR:  Could not select chip %s.\n",
            nfp_chipdata_chip_id(chip));
        return 0;
    }

    // load reg database for chip
    pp_data->reg_data = nfp_regdata_load(chip, 0x00, NULL);
    if (pp_data->reg_data == NULL) {
        fprintf(stderr, "ERROR:  Could not load reg data for chip %s.\n",
            nfp_chipdata_chip_id(chip));
        return 0;
    }

    return 1;

}


/*
 * Get configured ILKN Burst sizes for the nbi/port
 */
void ilkn_burst_sizes(struct pcap_play_data * pp_data)
{
    uint32_t rx_burst_cfg;
    char fqn[128];

    // read rx burst size csr and translate to max/min/short burst sizes
    sprintf(fqn, "xpb:Nbi%dIsldXpbMap.NbiTopXpbMap.MacGlbAdrMap.MacInterlaken%d.LkRxBurstConfig",
        pp_data->arg_nbi, pp_data->arg_port);

    xpb_fqn_rd(pp_data->nfp_cpp, pp_data->reg_data, fqn, &rx_burst_cfg);

    // MAX|MIN|SHORT are 3bit fields @ offsets 24|16|8
    pp_data->bmax = (rx_burst_cfg >> 24) & 7;
    pp_data->bmin = (rx_burst_cfg >> 16) & 7;
    pp_data->bshort = (rx_burst_cfg >> 8) & 7;

    // convert 3bit field to byte value
    pp_data->bmax = (pp_data->bmax+1)*64;
    pp_data->bmin = (pp_data->bmin+1)*32;

    // bshort is awkward!
    switch(pp_data->bshort) {
        case 1: pp_data->bshort = 64; break;
        case 2: pp_data->bshort = 16; break;
        case 4: pp_data->bshort = 24; break;
        case 7: pp_data->bshort = 8; break;
        default: pp_data->bshort = 32;
    }

    //printf("csr %x, burst max %d, burst min %d, burst short %d\n", rx_burst_cfg, pp_data->bmax, pp_data->bmin, pp_data->bshort);
}

int main(int argc, char **argv)
{
    struct pcap_play_data pp_data;
    pcap_t *pcap = NULL;
    const unsigned char *pktdata;
    char pcap_errbuf[PCAP_ERRBUF_SIZE];
    int pkt_cnt = 0;
    int loops;

    if (argc < 2) {
        usage(argv[0]);
        return 0;
    }

    parse_options(&pp_data, argc, argv);

    // get handles to sim and other useful structures
    if (!acquire_sim_hndls(&pp_data))
        exit(1);

    // get burst sizes from csr, convert to byte values
    ilkn_burst_sizes(&pp_data);

    // initial start time (i.e. when 1st packet can start)
    // either time supplied as argument or current simulator time
    pp_data.first_time = MAX(pp_data.arg_waittime,
                             nfp_sal_packet_get_time(pp_data.nfp));

    pp_data.total_oh_bytes = 0;
    pp_data.total_real_bytes = 0;


    loops = pp_data.arg_loops;
    while (loops > 0) {
        // open pcap file to load to ilkn queue
        pcap = pcap_open_offline(pp_data.pcap_filename, pcap_errbuf);
        if (!pcap) {
            fprintf(stderr, "pcap_open_offline() failed for file %s: %s\n",
                    pp_data.pcap_filename, pcap_errbuf);
            exit(1);
        }

        while (1) {
            pktdata = pcap_next(pcap, &pp_data.ph);
            if (!pktdata)
                break;

            pkt_cnt++;

            ilkn_gen_brsts(&pp_data, pktdata, pp_data.ph.len);
        }

        pcap_close(pcap);

        loops--;
    }

    nfp_device_close(pp_data.nfp);

    printf("sent %d packets\n", pkt_cnt);

    return 0;
}
