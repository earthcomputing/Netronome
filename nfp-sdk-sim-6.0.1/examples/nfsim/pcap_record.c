/**
 * Copyright (C) 2013 Netronome Systems, Inc.  All rights reserved.
 *
 * Example code to record packets on egress and save to a PCAP file
 *
 */

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <pthread.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <pcap/pcap.h>
#include <arpa/inet.h>

#include <nfp.h>
#include <nfp_sal.h>

#include <sim_events.h>

#define ARG_DEFAULT_DEVICE       0
#define ARG_DEFAULT_NBI          0
#define ARG_DEFAULT_PORT         0
#define ARG_DEFAULT_USASNS       0
#define ARG_DEFAULT_ALLPORTS     0
#define ARG_DEFAULT_SHOWRATE     0
#define ARG_DEFAULT_FILTERPAUSES 0

#define NBICNT      (2)
#define PORTCNT     (24)
#define IFG         (24)
#define MIN_PTK_LEN (64)

#define MEGA (1000 * 1000)
#define GIGA (MEGA * 1000)
#define BITS (8)
#define PS_IN_S (1000000000000lu)

#define STRMAX 1024

#define ETYPE_PAUSE_LINK 0x8808

struct performance_data {
    int total_cnt;
    int start_packet_count;
    int measured_bytes;
    uint64_t prev_pkt_time_ps;
    double avg_rate;
    double start_packet_time;
    double avg_packets_per_sec;
    int start_packet;
};

struct pcap_record_data {
    int arg_device; /* which nfp_device to use */
    int arg_nbi;
    int arg_port;
    int arg_count;
    int arg_usasns;
    int arg_allports;
    int arg_showrate;
    int arg_filter_pauses;
    struct nfp_device *nfp;
    char *pcap_filename;

    /* pcap handles */
#define SNAPLEN 65535
    pcap_t *pcap_h[NBICNT * PORTCNT];
    pcap_dumper_t *pcap_dumper[NBICNT * PORTCNT];

    /* main thread wake up vars */
    pthread_mutex_t wake_mtx;
    pthread_cond_t wake_cond;
    int wake_activity;

    /* performance counters */
    struct performance_data *perf_data[NBICNT * PORTCNT];
};

/* global handle for callbacks */
static struct pcap_record_data *pr_data_glob;

int do_shutdown = 0;

static void shutdown_handler(int signum)
{
    do_shutdown = 1;
}

static void usage(char *progname)
{
    fprintf(stderr,
            "usage: %s [options] <pcapfile>\n"
            "\nWhere options are:\n"
            "        -d <device> -- select NFP device\n"
            "        -n <nbi>    -- select NBI\n"
            "        -p <port>   -- select NBI port\n"
            "        -c <count>  -- stop recording after <count> packets\n"
            "        -u          -- interpret microsecond as nanoseconds\n"
            "                       for the pcap file\n"
            "        -r          -- display the egress rate\n"
            "        -a          -- record on all possible ports\n"
            "                       will create %d pcap files\n"
            "        -f          -- filter pause frames\n"
            "and arguments are:\n"
            "        <pcapfile>  -- the pcap to write to\n",
            progname, NBICNT * PORTCNT);
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

static void parse_options(struct pcap_record_data* pr_data,
                          int argc,
                          char *argv[])
{
    int o,i;
    uint64_t val;

    pr_data->arg_device = ARG_DEFAULT_DEVICE;
    pr_data->arg_nbi = ARG_DEFAULT_NBI;
    pr_data->arg_port = ARG_DEFAULT_PORT;
    pr_data->arg_usasns = ARG_DEFAULT_USASNS;
    pr_data->arg_allports = ARG_DEFAULT_ALLPORTS;
    pr_data->arg_showrate = ARG_DEFAULT_SHOWRATE;
    pr_data->arg_filter_pauses = ARG_DEFAULT_FILTERPAUSES;
    pr_data->arg_count = 0;

    while ((o = getopt(argc, argv, "d:n:p:s:c:rhuaf")) != -1) {
        switch(o) {
        case 'h':
            usage(argv[0]);
            exit(0);
            break;
        case 'f':
            pr_data->arg_filter_pauses = 1;
            break;
        case 'u':
            pr_data->arg_usasns = 1;
            break;
        case 'a':
            pr_data->arg_allports = 1;
            break;
        case 'r':
            pr_data->arg_showrate = 1;
            break;
        case 'd':
            if (uintarg(optarg, &val)) {
                usage(argv[0]);
                exit(1);
            }
            pr_data->arg_device = val;
            break;
        case 'n':
            if (uintarg(optarg, &val)) {
                usage(argv[0]);
                exit(1);
            }
            pr_data->arg_nbi = val;
            break;
        case 'p':
            if (uintarg(optarg, &val)) {
                usage(argv[0]);
                exit(1);
            }
            pr_data->arg_port = val;
            break;
        case 'c':
            if (uintarg(optarg, &val)) {
                usage(argv[0]);
                exit(1);
            }
            pr_data->arg_count = val;
            break;
        case 's':
            if (uintarg(optarg, &val)) {
                usage(argv[0]);
                exit(1);
            }
            for (i = 0; i < NBICNT * PORTCNT; i++) {
                /* start performance measure after this packet */
                pr_data->perf_data[i]->start_packet = val - 1;
            }
            break;
        default:
            usage(argv[0]);
            exit(1);
            break;
        }
    }

    if (argc  - optind != 1) {
        fprintf(stderr, "must specify a pcap file\n");
        usage(argv[0]);
        exit(1);
    }

    pr_data->pcap_filename = argv[optind];
}

/* packet receive callback; runs in seperate thread, use to wake main thread */
void pkt_out_cb(struct nfp_device *dev, struct simevent_pkt_outrecv *ev)
{
    /* a packet arrived - wake up the main thread */
    pthread_mutex_lock(&pr_data_glob->wake_mtx);
    pr_data_glob->wake_activity = 1;
    pthread_cond_signal(&pr_data_glob->wake_cond);
    pthread_mutex_unlock(&pr_data_glob->wake_mtx);
}

/* wait for the cond var to fire */
int packet_wait(struct pcap_record_data *pr_data)
{
    struct timespec ts;
    int ret;

    pthread_mutex_lock(&pr_data->wake_mtx);
    /* avoid cond var race condition */
    if (pr_data->wake_activity) {
        pr_data->wake_activity = 0;
        pthread_mutex_unlock(&pr_data->wake_mtx);
        return 0;
    }
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 1;
    ret = pthread_cond_timedwait(&pr_data->wake_cond, &pr_data->wake_mtx, &ts);
    if (ret) {
        pthread_mutex_unlock(&pr_data->wake_mtx);
        if (ret == ETIMEDOUT || ret == EAGAIN)
            return 1;
        sleep(1);
        fprintf(stderr, "error during pthread_cond_timedwait\n");
        return -1;
    }
    pr_data->wake_activity = 0;
    pthread_mutex_unlock(&pr_data->wake_mtx);
    return 0;
}

int process_packet(struct pcap_record_data *pr_data, int nbi, int port)
{
    struct pcap_pkthdr pkthdr;
    struct performance_data *perf;
    unsigned char pktdata[SNAPLEN];
    uint64_t pkt_time_ps, pkt_time;
    int pcnt, len, padded_len;
    int ind = nbi * PORTCNT + port;
    int cnt = 0;

    /* get the number of egress packets queued */
    while (1) {
        /* see if there are packets available */
        pcnt = nfp_sal_packet_egress_status(pr_data->nfp,
                                            nbi,
                                            port);
        if (pcnt < 0) {
            fprintf(stderr,
                    "nfp_sal_packet_egress_status() failed (%s)",
                    strerror(errno));
            exit(1);
        }

        if (pcnt == 0) /* no packets */
            break;

        /* grab a packet */
        len = nfp_sal_packet_egress(pr_data->nfp,
                                    nbi,
                                    port,
                                    (void *) pktdata,
                                    SNAPLEN,
                                    &pkt_time_ps);
        if (len < 0) {
            fprintf(stderr,
                    "nfp_sal_packet_egress() failed (%s)",
                    strerror(errno));
            exit(1);
        }

        if (len == 0) {
            fprintf(stderr, "egress packet disappeared");
            exit(1);
        }

        /* we optionally filter pauses */
        if (pr_data->arg_filter_pauses) {
            /* get the ethertype */
            uint16_t etype = ntohs(*((uint16_t *)(((uint8_t *)pktdata) + 12)));
            if (etype == ETYPE_PAUSE_LINK)
                continue; /* ignore the pause */
        }

        /* okay, we have a packet... */

        /* fill in the pcap header info */
        memset(&pkthdr, '\0', sizeof(pkthdr));
        /* convert time from ps to tv */
        if (pr_data->arg_usasns) { /* use ns */
            pkt_time = pkt_time_ps / (1000);
            pkthdr.ts.tv_sec = pkt_time / (1000 * 1000 * 1000);
            pkthdr.ts.tv_usec = pkt_time;
        } else { /* use us */
            pkt_time = pkt_time_ps / (1000 * 1000);
            pkthdr.ts.tv_sec = pkt_time / (1000 * 1000);
            pkthdr.ts.tv_usec = pkt_time;
        }
        pkthdr.caplen = len;
        pkthdr.len = len;
        padded_len = len;
        if(padded_len < MIN_PTK_LEN)
            padded_len = MIN_PTK_LEN;

        /* calculate the average egress rates */
        perf = pr_data->perf_data[ind];

        if (perf->total_cnt == perf->start_packet) {
            perf->start_packet_time = pkt_time_ps;
            perf->start_packet_count = perf->total_cnt;
        }

        if (perf->total_cnt > perf->start_packet) {
            perf->measured_bytes += (padded_len + IFG);
        }

        if (perf->total_cnt > perf->start_packet) {
            perf->avg_rate = perf->measured_bytes * BITS; /* bits */
            perf->avg_rate /= (pkt_time_ps - perf->start_packet_time);
            perf->avg_rate *= PS_IN_S; /* bits per second */
            perf->avg_rate /= GIGA; /* Gbps */

            perf->avg_packets_per_sec = perf->total_cnt - perf->start_packet_count;
            perf->avg_packets_per_sec /= (pkt_time_ps - perf->start_packet_time);
            perf->avg_packets_per_sec *= PS_IN_S; /* pkt/s */
            perf->avg_packets_per_sec /= MEGA; /* Mpkt/s */
        }

        pr_data->perf_data[ind]->total_cnt++;
        printf("Pkt rcv'd nbi:%1d port:%1d: ", nbi, port);
        if (len == padded_len)
            printf("len = %5d", len);
        else
            printf("len = %5d (padded)", padded_len);

        if (pr_data->arg_showrate) {
            printf(", pkt time = %9lups, delta time = %7lups, pkt # = %4d, "
                   "avg rate = %4.1fMpps / %4.1fG\n",
                   pkt_time_ps,
                   pkt_time_ps - perf->prev_pkt_time_ps, perf->total_cnt,
                   perf->avg_packets_per_sec, perf->avg_rate);
        } else
            printf("\n");

        pr_data->perf_data[ind]->prev_pkt_time_ps = pkt_time_ps;
        /* write the packet to the pcap */
        pcap_dump((u_char *)pr_data->pcap_dumper[ind], &pkthdr, pktdata);
        pcap_dump_flush(pr_data->pcap_dumper[ind]);
        cnt++;
    }
    return cnt;
}

/* free pcap record data */
void pcap_record_free(struct pcap_record_data *pr_data)
{
    int i;

    nfp_device_close(pr_data->nfp);

    pthread_cond_destroy(&pr_data->wake_cond);
    pthread_mutex_destroy(&pr_data->wake_mtx);

    for (i = 0; i < NBICNT * PORTCNT; i++) {
        free(pr_data->perf_data[i]);

        if (pr_data->pcap_dumper[i])
            pcap_dump_close(pr_data->pcap_dumper[i]);

        if (pr_data->pcap_h[i])
            pcap_close(pr_data->pcap_h[i]);
    }
}

/* initialize pcap record data */
void pcap_record_init(struct pcap_record_data *pr_data)
{
    struct performance_data *perf_data;
    char strbuf[STRMAX];
    int i;

    /* init performance data */
    for (i = 0; i < NBICNT * PORTCNT; i++) {
        perf_data = malloc(sizeof(*perf_data));
        if (perf_data != NULL) {
            pr_data->perf_data[i] = perf_data;
            /* start performance measure after this packet */
            pr_data->perf_data[i]->start_packet = 16;
        } else {
            printf("malloc failed\n");
            exit(1);
        }

        pr_data->pcap_dumper[i] = NULL;
        pr_data->pcap_h[i] = NULL;
    }

    /* initialize packet record data */
    pthread_mutex_init(&pr_data->wake_mtx, NULL);
    pthread_cond_init(&pr_data->wake_cond, NULL);
    pr_data->wake_activity = 0;

    /* connect to sim server */
    pr_data->nfp = nfp_device_open(pr_data->arg_device);
    if (!pr_data->nfp) {
        fprintf(stderr, "failed to open nfp device: %s\n", strerror(errno));
        exit(1);
    }

    /* open pcap(s) for writing */
    if (pr_data->arg_allports == 0) {
        int ind = pr_data->arg_nbi * PORTCNT + pr_data->arg_port;

        pr_data->pcap_h[ind] = pcap_open_dead(DLT_EN10MB, SNAPLEN);
        if (!pr_data->pcap_h[ind]) {
            fprintf(stderr, "pcap_open_dead() failed");
            exit(1);
        }
        pr_data->pcap_dumper[ind] = pcap_dump_open(pr_data->pcap_h[ind],
                                                pr_data->pcap_filename);
        if (!pr_data->pcap_dumper[ind]) {
            fprintf(stderr, "pcap_dump_open(%s) failed", pr_data->pcap_filename);
            exit(1);
        }
        /* ensure we flush the pcap header out */
        pcap_dump_flush(pr_data->pcap_dumper[ind]);
        /* enable packet egress events -
         * we will now get packet received event on this nbi/port */
        nfp_sal_packet_event_enable(pr_data->nfp,
                pr_data->arg_nbi,
                pr_data->arg_port,
                PACKET_EVENT_EGRESS_READY);
    } else { /* open all ports */
        char *ext;
        char *file;

        /* put a NULL term where the ".pcap" is */
        file = strrchr(pr_data->pcap_filename, '/');
        if (!file)
            file = pr_data->pcap_filename;
        ext = strstr(file, ".pcap");
        if (ext)
            ext[0] = '\0';

        for (i = 0; i < NBICNT * PORTCNT; i++) {
            snprintf(strbuf, STRMAX, "%s-nbi%d-port%02d.pcap",
                                     pr_data->pcap_filename,
                                     i / PORTCNT, i % PORTCNT);
            pr_data->pcap_h[i] = pcap_open_dead(DLT_EN10MB, SNAPLEN);
            if (!pr_data->pcap_h[i]) {
                fprintf(stderr, "pcap_open_dead() failed");
                exit(1);
            }
            pr_data->pcap_dumper[i] = pcap_dump_open(pr_data->pcap_h[i],
                                                    strbuf);
            if (!pr_data->pcap_dumper[i]) {
                fprintf(stderr, "pcap_dump_open(%s) failed", strbuf);
                exit(1);
            }
            /* enable packet egress events on all ports */
            nfp_sal_packet_event_enable(pr_data->nfp,
                    i / PORTCNT, /* nbi */
                    i % PORTCNT, /* port */
                    PACKET_EVENT_EGRESS_READY);
        }
    }

}

int main(int argc, char **argv)
{
    struct pcap_record_data pr_data;
    int n = 0;
    int ret;
    int i;

    /* register signals for clean shutdown */
    signal(SIGINT, shutdown_handler);
    signal(SIGHUP, shutdown_handler);
    signal(SIGTERM, shutdown_handler);

    /* parse command line options */
    parse_options(&pr_data, argc, argv);

    if (pr_data.arg_showrate) {
        if (pr_data.arg_usasns) {
            printf("packet time recorded seconds.nano-seconds\n");
        } else {
            printf("packet time recorded seconds.micro-seconds\n");
        }
    }

    /* set global for the event callback */
    pcap_record_init(&pr_data);
    pr_data_glob = &pr_data;
    /* enable the event callback */
    nfp_sal_subs_simevent_pkt_outrecv(pr_data.nfp, pkt_out_cb);

    while (!do_shutdown) {
        /* wait for a packet to arrive */
        ret = packet_wait(&pr_data);
        if (ret >= 0) {
            /* process the packet */
            if (pr_data.arg_allports == 0) {
                n += process_packet(&pr_data,
                                    pr_data.arg_nbi,
                                    pr_data.arg_port);
            } else {
                /* check all ports for packets */
                for (i = 0; i < NBICNT * PORTCNT; i++) {
                    n += process_packet(&pr_data,
                                        i / PORTCNT,
                                        i % PORTCNT);
                }
            }
            if (pr_data.arg_count && n >= pr_data.arg_count)
                break;
        } else {
            fprintf(stderr, "error while waiting for packet\n");
            break;
        }
    }

    /* one last check for packets */
    if (pr_data.arg_allports == 0) {
        n += process_packet(&pr_data,
                pr_data.arg_nbi,
                pr_data.arg_port);
    } else {
        /* check all ports for packets */
        for (i = 0; i < NBICNT * PORTCNT; i++) {
            n += process_packet(&pr_data,
                    i / PORTCNT,
                    i % PORTCNT);
        }
    }

    pcap_record_free(&pr_data);

    printf("captured %d packets\n", n);

    return 0;
}
