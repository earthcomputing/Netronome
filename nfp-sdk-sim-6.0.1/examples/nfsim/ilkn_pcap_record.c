/**
 * Copyright (C) 2013 Netronome Systems, Inc.  All rights reserved.
 *
 * Example code to record Interlaken packets on Egress and save to a PCAP file
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
#include <inttypes.h>

#include <pthread.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <pcap/pcap.h>

#include <nfp.h>
#include <nfp_sal.h>

#include <sim_events.h>
#include <ilkn_bus.h>

#define ARG_DEFAULT_DEVICE   0
#define ARG_DEFAULT_NBI      0
#define ARG_DEFAULT_PORT     0
#define ARG_DEFAULT_USASNS   0

#define NBICNT 2
#define PORTCNT 2
#define CHNLCNT 64      // can be 128 if port 0 - for now, will limit to 64

#define ETH_IFG 12
#define ETH_OH 8

#define PKTMAX 16000
#define BURSTMAX 512

/* global for shutdown */
int shutdown = 0;

/* global pcap record data */
static struct pcap_record_data *pr_data_glob;

/* signal handler for clean shutdown */
static void shutdown_handler(int signum)
{
    shutdown = 1;
}

struct scratch_data {
    int active;
    int complete;
    int position;
    unsigned char buffer[PKTMAX];
    // pcap handles
    pcap_t *pcap_h;
    pcap_dumper_t *pcap_dumper;
};

struct pcap_record_data {
    int arg_device; // which nfp_device to use
    int arg_nbi;
    int arg_port;
    int arg_usasns;
    struct nfp_device *nfp;
    char *pcap_filename;

    // scratch buffer
    struct scratch_data scratch[CHNLCNT];

    uint64_t rxtime_ps;
    uint64_t total_bytes;
    uint64_t total_pkts;
    uint64_t first_time; //time of first data received

    // main thread wake up vars
    pthread_mutex_t wake_mtx;
    pthread_cond_t wake_cond;
    int wake_activity;
};

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

static void usage(char *progname)
{
    fprintf(stderr,
            "usage: %s [options] <pcapfile>\n"
            "\nWhere options are:\n"
            "        -d <device> -- select NFP device\n"
            "        -n <nbi>    -- select NBI\n"
            "        -p <port>   -- select NBI port\n"
            "        -u          -- interpret microsecond as nanoseconds\n"
            "                       for the pcap file\n"
            "and arguments are:\n"
            "        <pcapfile>  -- the pcap to write to, note there will be 1 \
                                    per active chnl, i.e. <pcapfile>1.pcap, etc\n",
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

static void parse_options(struct pcap_record_data* pr_data,
                          int argc,
                          char *argv[])
{
    int o;
    uint64_t val;

    pr_data->arg_device = ARG_DEFAULT_DEVICE;
    pr_data->arg_nbi = ARG_DEFAULT_NBI;
    pr_data->arg_port = ARG_DEFAULT_PORT;
    pr_data->arg_usasns = ARG_DEFAULT_USASNS;

    while ((o = getopt(argc, argv, "d:n:p:hu")) != -1) {
        switch(o) {
        case 'h':
            usage(argv[0]);
            exit(0);
            break;
        case 'u':
            pr_data->arg_usasns = 1;
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
            if (val < 0 || val > 1) {
                fprintf(stderr, "port must be 0 or 1\n");
                exit(1);
            }
            pr_data->arg_port = val;
            break;
        default:
            usage(argv[0]);
            exit(1);
            break;
        }
    }

    if (argc - optind != 1) {
        fprintf(stderr, "must specify a pcap file\n");
        usage(argv[0]);
        exit(1);
    }

    pr_data->pcap_filename = argv[optind];
}

/* packet receive callback; runs in seperate thread, use to wake main thread */
void brst_out_cb(struct nfp_device *dev, struct simevent_ilkn_outrecv *ev)
{
    /* a burst has arrived - wake up the main thread */
    pthread_mutex_lock(&pr_data_glob->wake_mtx);
    pr_data_glob->wake_activity = 1;
    pthread_cond_signal(&pr_data_glob->wake_cond);
    pthread_mutex_unlock(&pr_data_glob->wake_mtx);
}

/* wait for the event cond var to fire */
int burst_wait(struct pcap_record_data *pr_data)
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


void open_pcap(struct pcap_record_data *pr_data,
               struct scratch_data * scratch,
               int chnl)
{
    char filename[512];

    /* open pcap for writing */
    scratch->pcap_h = pcap_open_dead(DLT_EN10MB, PKTMAX);
    if (!scratch->pcap_h) {
        fprintf(stderr, "pcap_open_dead() failed");
        exit(1);
    }

    sprintf(filename, "%s_%03d.pcap", pr_data->pcap_filename, chnl);

    scratch->pcap_dumper = pcap_dump_open(scratch->pcap_h, filename);
    if (!scratch->pcap_dumper) {
        fprintf(stderr, "pcap_dump_open(%s) failed", filename);
        exit(1);
    }

    /* Ensure we flush the pcap header out */
    pcap_dump_flush(scratch->pcap_dumper);
    scratch->active = 1;
}


void calc_rates(struct pcap_record_data * pr_data, int freq)
{
    // calculate pkt & bit rate
    double prate, brate, erate;

    /* no packets, nothing to report */
    if (!pr_data->total_pkts)
        return;

    /* if this is the first time called, just save the start time */
    if (pr_data->first_time == 0) {
        pr_data->first_time = pr_data->rxtime_ps;
        return;
    }

    prate = (pr_data->total_pkts * 1.0e12) /
            (pr_data->rxtime_ps - pr_data->first_time);
    brate = (pr_data->total_bytes * 8 * 1.0e12) /
            (pr_data->rxtime_ps - pr_data->first_time);

    // scale bit rate up by ethernet overhead to get equivalent ethernet rate
    erate = (pr_data->total_bytes + (ETH_IFG + ETH_OH) * pr_data->total_pkts);
    erate *= 8 * 1.0e12;
    erate /= pr_data->rxtime_ps - pr_data->first_time;

    /* only print every 'freq' packets */
    if (!(pr_data->total_pkts % freq))
        printf("rx_time %lu, stime %lu, pkt %lu, "
               "prate = %0.1f mpps, brate = %0.3f gbit/s "
               "(eqv. eth. rate = %0.3f gbit/s)\n",
                pr_data->rxtime_ps, pr_data->first_time,
                pr_data->total_pkts, prate/1e6, brate/1e9, erate/1e9);
}

int data_available(struct pcap_record_data *pr_data)
{
    return nfp_sal_ilkn_egress_status(pr_data->nfp,
                                      pr_data->arg_nbi,
                                      pr_data->arg_port);
}

int process_burst(struct pcap_record_data *pr_data)
{
    int nbi = pr_data->arg_nbi, port = pr_data->arg_port;
    struct ilkn_buffer brstdata;
    union ilkn_buffer_u brstdata_u;
    struct pcap_pkthdr pkthdr;
    uint64_t pkt_time_ps, pkt_time;
    int bcnt, len, sz;
    struct scratch_data * scratch;

    // get the number of egress bursts queued
    while (1) {
        int bytes_remaining, bytes;
        int i, j;

        // see if there are bursts available
        bcnt = data_available(pr_data);

        if (bcnt < 0) {
            exit(1);
            printf("error getting queue status (%s)\n", strerror(errno));
        }

        if (bcnt <= 0)
            break;

        // grab a burst word
        len = nfp_sal_ilkn_egress(pr_data->nfp, nbi, port, (void *) &brstdata,
                                  sizeof(brstdata), &pkt_time_ps);

        if (len < 0) {
            printf("nfp_sal_ilkn_egress() failed (%s)\n", strerror(errno));
            exit(1);
        }

        if (len == 0) { //The packet disappeared? shouldn't happen
            fprintf(stderr,
                    "error: nfp_sal_ilkn_egress returned zero-sized packet\n");
            exit(1);
        }

        if (brstdata.chan >= CHNLCNT) {
            fprintf(stderr,
                    "channel number > %d not supported", brstdata.chan - 1);
            break;
        }

        // copying pointer now as l8r will make adding multiple scratches (per
        // channel) easier
        scratch = &(pr_data->scratch[brstdata.chan]);

        // reset scratch variables for new packet
        if (brstdata.sop == 1) {
            scratch->complete = 0;
            scratch->position = 0;
        }

        // if first time getting data for the channel, open an output pcap file
        if (!scratch->active)
            open_pcap(pr_data, scratch, brstdata.chan);

        // add the ilkn word to the local scratch buffer
        // converting to union to allow byte access to 8B data words
        brstdata_u.pstruct = brstdata;
        sz = (brstdata.sz > 0) ? brstdata.sz : ILKN_WORD_SZ;

        // printf("rx'd ilkn burst: len %d, sz %d, pos %d : ", len, sz, scratch->position); ilkn_buff_print(&brstdata);

        bytes_remaining = sz;
        bytes = 0;
        j = 0;

        // when transferring to scratch need to reorder bytes, from LWBE to
        // network order
        while (bytes_remaining > 0) {
            for (i = 7; i >= 0 && bytes_remaining > 0; i--) {
                scratch->buffer[scratch->position + bytes] = brstdata_u.parray[j + i];
                bytes_remaining--;
                bytes++;
            }
            j += 8;
        }

        scratch->position += sz;
        if (brstdata.eop == 1)
            scratch->complete = 1;

        if (scratch->complete == 0)
            break;

        // complete packet received,

        // printf("Packet received on N%d P%d C%d: len=%d\n", nbi, port,
        //         brstdata.chan, scratch->position);

        // fill in the pcap header info
        memset(&pkthdr, '\0', sizeof(pkthdr));
        // convert time from ps to tv
        if (pr_data->arg_usasns) { // use ns
            pkt_time = pkt_time_ps / (1000);
            pkthdr.ts.tv_sec = pkt_time / (1000 * 1000 * 1000);
            pkthdr.ts.tv_usec = pkt_time;
        } else { // use us
            pkt_time = pkt_time_ps / (1000 * 1000);
            pkthdr.ts.tv_sec = pkt_time / (1000 * 1000);
            pkthdr.ts.tv_usec = pkt_time;
        }
        pkthdr.caplen = scratch->position;
        pkthdr.len = scratch->position;

        // write the packet to the pcap
        pcap_dump((u_char *)scratch->pcap_dumper, &pkthdr, (u_char *)&scratch->buffer);
        pcap_dump_flush(scratch->pcap_dumper);

        // copy the clock cycle count time for rate calcs
        pr_data->rxtime_ps = pkt_time_ps;

        // inc totals for rates calc
        pr_data->total_bytes += scratch->position;
        pr_data->total_pkts++;

        calc_rates(pr_data, 10);
    }
    return pr_data->total_pkts;
}


void reset(struct pcap_record_data * pr_data)
{
    int i;

    pr_data->total_bytes = 0;
    pr_data->total_pkts = 0;
    pr_data->first_time = 0;

    for(i = 0; i < CHNLCNT; i++)
        pr_data->scratch[i].active = 0;
}


int main(int argc, char **argv)
{
    uint64_t port_rate, ig_rate, eg_rate;
    struct pcap_record_data pr_data;
    int ret;
    int i;

    /* register signals for clean shutdown */
    signal(SIGINT, shutdown_handler);
    signal(SIGHUP, shutdown_handler);
    signal(SIGTERM, shutdown_handler);

    /* initialize packet record data */
    pthread_mutex_init(&pr_data.wake_mtx, NULL);
    pthread_cond_init(&pr_data.wake_cond, NULL);
    pr_data.wake_activity = 0;

    /* parse command line options */
    parse_options(&pr_data, argc, argv);

    /* connect to sim server */
    pr_data.nfp = nfp_device_open(pr_data.arg_device);
    if (!pr_data.nfp) {
        fprintf(stderr, "failed to open nfp device: %s\n", strerror(errno));
        exit(1);
    }

    // enable packet egress events -
    // we will now get packet received event on this nbi/port
    nfp_sal_ilkn_event_enable(pr_data.nfp,
                              pr_data.arg_nbi,
                              pr_data.arg_port,
                              ILKN_EVENT_EGRESS_READY);

    // set global for the event callback
    pr_data_glob = &pr_data;

    // enable the event callback
    nfp_sal_subs_simevent_ilkn_outrecv(pr_data.nfp, brst_out_cb);

    // reset any variables as needed
    reset(&pr_data);

    while (!shutdown) {
        // wait for a ilkn burst word to arrive
        ret = burst_wait(&pr_data);
        if (ret < 0) {
            fprintf(stderr, "error while waiting for packet\n");
            break;
        }
        /* process data while we have it */
        while (data_available(&pr_data) > 0) {
            // process the burst word
            process_burst(&pr_data);
        }
    }

    /* process data while we have it */
    while (data_available(&pr_data) > 0) {
        // process the burst word
        process_burst(&pr_data);
    }

    nfp_sal_ilkn_get_stat(pr_data.nfp,
                          pr_data.arg_nbi, pr_data.arg_port,
                          NFP_SAL_ILKN_STAT_PORT_RATE,
                          &port_rate);
    nfp_sal_ilkn_get_stat(pr_data.nfp,
                          pr_data.arg_nbi, pr_data.arg_port,
                          NFP_SAL_ILKN_STAT_INGRESS_RATE,
                          &ig_rate);
    nfp_sal_ilkn_get_stat(pr_data.nfp,
                          pr_data.arg_nbi, pr_data.arg_port,
                          NFP_SAL_ILKN_STAT_EGRESS_RATE,
                          &eg_rate);
    printf("port rate = %lu mbps, ig rate = %lu mbps, eg rate = %lu mbps\n",
           (uint64_t) (port_rate/1e6), (uint64_t) (ig_rate/1e6),
           (uint64_t) (eg_rate/1e6));

    /* clean up a bit */
    for (i = 0; i < CHNLCNT; i++) {
        if (!pr_data.scratch[i].active)
            continue;
        pcap_dump_close(pr_data.scratch[i].pcap_dumper);
        pcap_close(pr_data.scratch[i].pcap_h);
    }

    nfp_device_close(pr_data.nfp);

    printf("captured %lu packets\n", pr_data.total_pkts);

    return 0;
}
