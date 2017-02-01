/**
 * Copyright (C) 2013 Netronome Systems, Inc.  All rights reserved.
 *
 * Example code to play/inject a PCAP file into a Simulated Ingress port
 *
 */

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <pcap/pcap.h>

#include <nfp.h>
#include <nfp_sal.h>

#undef DEBUG

#ifdef DEBUG
#define DEBUG_PRINT(...) do{ fprintf( stdout, __VA_ARGS__ ); } while( 0 )
#else
#define DEBUG_PRINT(...) do{ } while ( 0 )
#endif

/* This includes 7 bytes preamble and 1 byte start of frame */
#define ETH_PREAMBLE_LEN        8
/* Frame check sequence */
#define ETH_FCS_LEN             4
/* Inter frame gap */
#define ETH_IFG_LEN             12

#define ARG_DEFAULT_DEVICE   0
#define ARG_DEFAULT_NBI      0
#define ARG_DEFAULT_PORT     0
#define ARG_DEFAULT_LOOPS    1
#define ARG_DEFAULT_USETIME  0
#define ARG_DEFAULT_USERATE  0
#define ARG_DEFAULT_WAITTIME 0
#define ARG_DEFAULT_USASNS   0
#define ARG_DEFAULT_IPG      ETH_PREAMBLE_LEN + ETH_IFG_LEN
#define ARG_DEFAULT_RATE     10u
#define ARG_DEFAULT_MINPKT   64u

#define PACKET_MAX 16000

#define GIGA            (1000*1000*1000)
#define PS_IN_S         (1000*1000*1000*1000)
#define _1_GBPS         (1000)
#define BITS_PER_BYTE   (8)

struct pcap_play_data {
    int arg_device; /* which nfp_device to use */
    int arg_nbi;
    int arg_port;
    int arg_loops;
    int arg_usetime;
    int arg_usasns;
    int arg_userate;
    int arg_minpkt;
    uint64_t arg_waittime;
    uint32_t arg_linerate;
    uint32_t arg_interpacketgap;
    struct nfp_device *nfp;

    char *pcap_filename;
};

static void usage(char *progname)
{
    fprintf(stderr,
            "usage: %s [options] <pcapfile>\n"
            "\nWhere options are:\n"
            "        -t                     -- use pcap time for sending\n"
            "        -d <device>            -- select NFP device\n"
            "        -n <nbi>               -- select NBI\n"
            "        -p <port>              -- select NBI port\n"
            "        -w <time>              -- sim time in picoseconds to wait before\n"
            "                                   first packet is sent\n"
            "        -u                     -- interpret microsecond as nanoseconds\n"
            "                                   for the pcap file\n"
            "        -r <rate>              -- specify line rate in Gbps, default 10. Will override -t and -u\n"
            "        -g <interpacket gap>   -- specify interpacket gap in bytes, default 20b. Will override -t and -u\n"
            "        -m <min packet size>   -- specify min packet size, default 64b. Will override -t and -u (does not modify packet)\n"
            "        -l <loops>             -- Loop through the pcap file X times\n"
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

static void parse_options(struct pcap_play_data* pp_data,
                          int argc,
                          char *argv[])
{
    int o;
    uint64_t val;

    pp_data->arg_device = ARG_DEFAULT_DEVICE;
    pp_data->arg_nbi = ARG_DEFAULT_NBI;
    pp_data->arg_port = ARG_DEFAULT_PORT;
    pp_data->arg_loops = ARG_DEFAULT_LOOPS;
    pp_data->arg_usetime = ARG_DEFAULT_USETIME;
    pp_data->arg_waittime = ARG_DEFAULT_WAITTIME;
    pp_data->arg_usasns = ARG_DEFAULT_USASNS;
    pp_data->arg_interpacketgap = ARG_DEFAULT_IPG;
    pp_data->arg_linerate = ARG_DEFAULT_RATE;
    pp_data->arg_userate = ARG_DEFAULT_USERATE;
    pp_data->arg_minpkt = ARG_DEFAULT_MINPKT;
    while ((o = getopt(argc, argv, "w:d:n:p:l:r:g:m:htu")) != -1) {
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
            pp_data->arg_linerate = val;
            pp_data->arg_userate = 1;
            break;
        case 'g':
            if (uintarg(optarg, &val)) {
                usage(argv[0]);
                exit(1);
            }
            pp_data->arg_interpacketgap = val;
            pp_data->arg_userate = 1;
            break;
        case 'm':
            if (uintarg(optarg, &val)) {
                usage(argv[0]);
                exit(1);
            }
            pp_data->arg_minpkt = val;
            pp_data->arg_userate = 1;
            break;
        default:
            usage(argv[0]);
            exit(1);
            break;
        }
    }

    if (((pp_data->arg_usetime == 1) || (pp_data->arg_usasns == 1))
            && (pp_data->arg_userate == 1)) {
        printf("WARNING: (-t or -u) and (-r, -g or -m) specified!\n"
                "-t and -u will be ignored\n");
        pp_data->arg_usetime = 0;
        pp_data->arg_usasns = 0;
    }

    if (argc  - optind != 1) {
        fprintf(stderr, "must specify a pcap file\n");
        usage(argv[0]);
        exit(1);
    }

    pp_data->pcap_filename = argv[optind];
}

static int play_pcap(struct pcap_play_data *pp_data)
{
    pcap_t *pcap = NULL;
    struct pcap_pkthdr ph;
    char pcap_errbuf[PCAP_ERRBUF_SIZE];
    const unsigned char *pktdata;
    int ret;
    int n = 0;
    int first = 1;
    uint64_t simstart_ps = 0;
    uint64_t timein_ps;
    uint64_t prev_timein_ps = 0;
    uint64_t prev_size = 0;
    uint64_t timeoffset_ps;
    uint64_t timetosend_ps;
    uint64_t bit_time_ps;

    bit_time_ps = _1_GBPS/pp_data->arg_linerate;
    DEBUG_PRINT("bit_time_ps: %ld\n",bit_time_ps);

    /* get the current sim time */
    simstart_ps = nfp_sal_packet_get_time(pp_data->nfp);

    DEBUG_PRINT("simstart_ps: %ld\n",simstart_ps);
    pcap = pcap_open_offline(pp_data->pcap_filename, pcap_errbuf);
    if (!pcap) {
        fprintf(stderr, "pcap_open_offline() failed for file %s: %s\n",
                                        pp_data->pcap_filename, pcap_errbuf);
        exit(1);
    }

    while (1) {
        pktdata = pcap_next(pcap, &ph);
        if (!pktdata)
            break;

        /* calculate time to send */
        if (pp_data->arg_usasns) { /* interpret us as ns */
            timein_ps = ph.ts.tv_sec * 1000 * 1000 * 1000 * 1000 +
                        ph.ts.tv_usec * 1000;
        } else {
            timein_ps = ph.ts.tv_sec * 1000 * 1000 * 1000 * 1000 +
                        ph.ts.tv_usec * 1000 * 1000;
        }
        if (pp_data->arg_userate) {
            timein_ps =
                    prev_timein_ps
                            + (pp_data->arg_interpacketgap * BITS_PER_BYTE
                                    * bit_time_ps * !first)
                            + (prev_size * BITS_PER_BYTE * bit_time_ps);
        }
        DEBUG_PRINT("timestamp: %lu.%lu\r\n",(uint64_t)ph.ts.tv_sec, (uint64_t)ph.ts.tv_usec);
        prev_size = ph.caplen;
        if (prev_size < pp_data->arg_minpkt)
            prev_size = pp_data->arg_minpkt;
        prev_timein_ps = timein_ps;
        DEBUG_PRINT("timein_ps 1: %ld\n",timein_ps);

        /* for the first packet set a zero reference */
        if (first)
            timeoffset_ps = timein_ps;

        DEBUG_PRINT("timeoffset_ps: %ld\n",timeoffset_ps);
        timein_ps -= timeoffset_ps;
        DEBUG_PRINT("timein_ps 2: %ld\n",timein_ps);

        if ((pp_data->arg_usetime) || (pp_data->arg_userate))
            timetosend_ps = pp_data->arg_waittime + timein_ps + simstart_ps;
        else
            timetosend_ps = pp_data->arg_waittime;

        DEBUG_PRINT("pp_data->arg_waittime: %ld\n",pp_data->arg_waittime);
        DEBUG_PRINT("timetosend_ps: %ld\n",timetosend_ps);
        ret = nfp_sal_packet_ingress(pp_data->nfp,
                                     pp_data->arg_nbi,
                                     pp_data->arg_port,
                                     pktdata,
                                     ph.caplen,
                                     timetosend_ps);
        if (ret < 0) {
            fprintf(stderr, "nfp_sal_packet_ingress() failed: %s\n",
                                                    strerror(errno));
            exit(1);
        }
        DEBUG_PRINT("----\n");
        first = 0;
        n++;
    }

    pcap_close(pcap);

    return n;
}

int main(int argc, char **argv)
{
    struct pcap_play_data pp_data;
    int n = 0;
    int i;

    parse_options(&pp_data, argc, argv);

    pp_data.nfp = nfp_device_open(pp_data.arg_device);

    if (!pp_data.nfp) {
        fprintf(stderr, "failed to open nfp device: %s\n", strerror(errno));
        exit(1);
    }

    for (i = 0; i < pp_data.arg_loops; i++) {
        n += play_pcap(&pp_data);
    }

    nfp_device_close(pp_data.nfp);

    if (pp_data.arg_userate == 1) {
        printf("sent %d packets. Line rate: %dG, Interframe gap: %db, Minimum packet size: %db\n",
                n, pp_data.arg_linerate, pp_data.arg_interpacketgap,
                pp_data.arg_minpkt);
    } else {
        printf("sent %d packets\n", n);
    }

    return 0;
}
