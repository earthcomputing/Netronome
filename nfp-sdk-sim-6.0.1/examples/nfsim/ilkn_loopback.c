/**
 * Copyright (C) 2013 Netronome Systems, Inc.  All rights reserved.
 *
 * Example with loops back an Interlaken port
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

#include <nfp.h>
#include <nfp_cpp.h>
#include <nfp_sal.h>

#include "ilkn_bus.h"
#include "sim_events.h"

/***** Defines *****/

#define ARG_DEFAULT_DEVICE   0
#define ARG_DEFAULT_NBI      0
#define ARG_DEFAULT_ILPORT   1  // this is the IL-LA port
#define ARG_DEFAULT_ALLPORTS 0

struct ilkn_lpbk_cfg {
    int arg_device; /* which nfp_device to use */
    int arg_nbi;
    int arg_port;
    int arg_allports;
    struct nfp_device *nfp;
    char *ilkn_filename;
    FILE *ilkn_fh;
    /* main thread wake up vars */
    pthread_mutex_t wake_mtx;
    pthread_cond_t wake_cond;
    int wake_activity;
};

/***** Globals *****/

int shutdown = 0;

static struct ilkn_lpbk_cfg *ilkn_lpbk_glob;

/***** *****/

static void shutdown_handler(int signum)
{
    shutdown = 1;
}

/***** Arg handling *****/

static void usage(char *progname)
{
    fprintf(stderr,
            "usage: %s [options] <ilfile>\n"
            "\nWhere options are:\n"
            "        -d <device> -- select NFP device (0) \n"
            "        -n <nbi>   -- select NBI (0)\n"
            "        -p <port>  -- select NBI port (1) \n"
            "and arguments are:\n"
            "        <ilfile>  -- the file to write interlaken data to (/tmp/ilkn_loopback.log)\n",
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

static void parse_options(struct ilkn_lpbk_cfg* ilkn_lpbk,
                          int argc,
                          char *argv[])
{
    int o;
    uint64_t val;

    ilkn_lpbk->arg_device = ARG_DEFAULT_DEVICE;
    ilkn_lpbk->arg_nbi = ARG_DEFAULT_NBI;
    ilkn_lpbk->arg_port = ARG_DEFAULT_ILPORT;
    ilkn_lpbk->arg_allports = ARG_DEFAULT_ALLPORTS;

    while ((o = getopt(argc, argv, "d:n:p:h")) != -1) {
        switch(o) {
        case 'h':
            usage(argv[0]);
            exit(0);
            break;
        case 'd':
            if (uintarg(optarg, &val)) {
                usage(argv[0]);
                exit(1);
            }
            ilkn_lpbk->arg_device = val;
            break;
        case 'n':
            if (uintarg(optarg, &val)) {
                usage(argv[0]);
                exit(1);
            }
            ilkn_lpbk->arg_nbi = val;
            break;
        case 'p':
            if (uintarg(optarg, &val)) {
                usage(argv[0]);
                exit(1);
            }
            ilkn_lpbk->arg_port = val;
            break;
        default:
            usage(argv[0]);
            exit(1);
            break;
        }
    }

    if (argc  - optind != 1) {
        ilkn_lpbk->ilkn_filename = "/tmp/ilkn_loopback.log";
    } else {
        ilkn_lpbk->ilkn_filename = argv[optind];
    }
}

/* ilkn data receive callback; runs in seperate thread, wakes main thread */
static void ilkn_out_cb(struct nfp_device *dev,
                        struct simevent_ilkn_outrecv *ev)
{
    /* il data arrived - wake up the main thread */
    pthread_mutex_lock(&ilkn_lpbk_glob->wake_mtx);
    ilkn_lpbk_glob->wake_activity = 1;
    pthread_cond_signal(&ilkn_lpbk_glob->wake_cond);
    pthread_mutex_unlock(&ilkn_lpbk_glob->wake_mtx);
}

/* wait for the cond var to fire */
static int ilkn_wait(struct ilkn_lpbk_cfg *ilkn_lpbk)
{
    struct timespec ts;
    int ret;

    pthread_mutex_lock(&ilkn_lpbk->wake_mtx);
    // avoid cond var race condition
    if (ilkn_lpbk->wake_activity) {
        ilkn_lpbk->wake_activity = 0;
        pthread_mutex_unlock(&ilkn_lpbk->wake_mtx);
        return 0;
    }
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 1;
    ret = pthread_cond_timedwait(&ilkn_lpbk->wake_cond,
                                 &ilkn_lpbk->wake_mtx,
                                 &ts);
    if (ret) {
        pthread_mutex_unlock(&ilkn_lpbk->wake_mtx);
        if (ret == ETIMEDOUT || ret == EAGAIN)
            return 1;
        sleep(1);
        fprintf(stderr, "error during pthread_cond_timedwait\n");
        return -1;
    }
    ilkn_lpbk->wake_activity = 0;
    pthread_mutex_unlock(&ilkn_lpbk->wake_mtx);
    return 0;
}

static int process_data(struct ilkn_lpbk_cfg *ilkn_lpbk, int nbi, int port)
{
    uint64_t rx_time_ps;
    int rxcnt, len;
    struct ilkn_buffer data;
    int cnt = 0;
    int ret;

    /* get the number of egress packets queued */
    while (1) {
        /* see if there are packets available */
        rxcnt = nfp_sal_ilkn_egress_status(ilkn_lpbk->nfp, nbi, port);

        if (rxcnt <= 0)
            break;

        /* grab the data */
        len = nfp_sal_ilkn_egress(ilkn_lpbk->nfp, nbi, port,
                                  (void *) &data, sizeof(data),
                                  &rx_time_ps);

        /* loop it back - send immediately */
        ret = nfp_sal_ilkn_ingress(ilkn_lpbk->nfp, nbi, port,
                                   (const void *) &data, sizeof(data),
                                   0);

        if (ret < 0) {
            fprintf(stderr,
                    "nfp_sal_packet_ingress() failed: %s\n",
                    strerror(errno));
            exit(1);
        }

        if (len <= 0)
            break;

        printf("ILKN data received\n");

        /* write the data to the file */
        fprintf(ilkn_lpbk->ilkn_fh, "sop=%d, eop=%d, sob=%d, eob=%d, "
            "0x%"PRIx64"%"PRIx64"%"PRIx64"%"PRIx64"\n",
            data.sop, data.eop, data.sob, data.eob,
            data.data_0, data.data_1, data.data_2, data.data_3);

        cnt++;
    }
    return cnt;
}

int main(int argc, char **argv)
{
    struct ilkn_lpbk_cfg ilkn_lpbk;
    uint64_t port_rate = 0;
    int n = 0;
    int ret;

    /* parse command line options */
    parse_options(&ilkn_lpbk, argc, argv);

    /* register signals for clean shutdown */
    signal(SIGINT, shutdown_handler);
    signal(SIGHUP, shutdown_handler);
    signal(SIGTERM, shutdown_handler);

    /* set global handle for the event callback */
    ilkn_lpbk_glob = &ilkn_lpbk;

    /* initialize ilkn data */
    pthread_mutex_init(&ilkn_lpbk.wake_mtx, NULL);
    pthread_cond_init(&ilkn_lpbk.wake_cond, NULL);
    ilkn_lpbk.wake_activity = 0;

    /* connect to sim server */
    ilkn_lpbk.nfp = nfp_device_open(ilkn_lpbk.arg_device);
    if (!ilkn_lpbk.nfp) {
        fprintf(stderr, "failed to open nfp device: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    nfp_sal_ilkn_get_stat(ilkn_lpbk.nfp, ilkn_lpbk.arg_nbi, ilkn_lpbk.arg_port,
                          NFP_SAL_ILKN_STAT_PORT_RATE, &port_rate);
    printf("ilkn port rate = %lu mbps\nCtrl-C to exit\n",
           (uint64_t) (port_rate/1e6));

    ilkn_lpbk.ilkn_fh = fopen(ilkn_lpbk.ilkn_filename, "w");

    if (!ilkn_lpbk.ilkn_fh) {
        fprintf(stderr, "ILKN record fopen(%s) failed\n",
                ilkn_lpbk.ilkn_filename);
        exit(EXIT_FAILURE);
    }

    /* enable packet egress events -
     * we will now get ilkn-descriptor received event on this nbi/port
     */
    nfp_sal_ilkn_event_enable(ilkn_lpbk.nfp, ilkn_lpbk.arg_nbi,
        ilkn_lpbk.arg_port, ILKN_EVENT_EGRESS_READY);


    /* enable the event callback */
    nfp_sal_subs_simevent_ilkn_outrecv(ilkn_lpbk.nfp, ilkn_out_cb);

    /* main loop */
    while (!shutdown) {
        /* wait for interlaken data to arrive */
        ret = ilkn_wait(&ilkn_lpbk);
        if (ret == 0) {
            n += process_data(&ilkn_lpbk,
                              ilkn_lpbk.arg_nbi,
                              ilkn_lpbk.arg_port);
        } else if (ret < 0) {
            fprintf(stderr, "error while waiting for interlaken data\n");
            exit(EXIT_FAILURE);
        }
    }

    nfp_device_close(ilkn_lpbk.nfp);
    fclose(ilkn_lpbk.ilkn_fh);

    printf("captured %d interlaken transfers\n", n);

    return EXIT_SUCCESS;
}
