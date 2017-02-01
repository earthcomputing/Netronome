/**
 * Copyright (C) 2013 Netronome Systems, Inc.  All rights reserved.
 *
 * Example IPC client which uses the NFHAL/NFSAL APIs
 *
 * @file          read_mecsr.c
 * @brief         Command Line Tool to read ME CSRs
 *
 */

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include <nfp-common/ns_log.h>
#include <nfp-common/nfp_resid.h>

#include <nfp.h>
#include <nfp_sal.h>

#include "read_mecsr.h"

/***** Defines *****/

#define MAX_VALUES              200
#define DEFAULT_WATCH_DELAY     3
#define DEFAULT_MEID            NFP6000_MEID(32, 0)

#define CLS()       do { printf("\033[2J"); } while(0)
#define POS(_x, _y) do { printf("\033[%d;%dH", (_y), (_x)); } while(0)
#define GREEN()     do { printf("\033[0;32m"); } while(0)
#define RED()       do { printf("\033[0;31m"); } while(0)
#define NORMAL()    do { printf("\033[0;0m"); } while(0)

struct globalargs_data {
    int verbose;                /* -v verbose */
    const char *meid_str;       /* -m <MEID name> */
    int nfp_debug_level;        /* -d <DEBUG LEVEL> */
    int devnum;                 /* -n <NFP Device number> */
    int watch_delay;            /* -w <delay in seconds> */
};

/***** Globals *****/

int terminate = 0;

/* Define command line arguments */
static const char *optstring = "n:m:d:w:vh?";

struct globalargs_data globalargs;

/* Display program usage, and exit. */
void usage(const char *program, int err)
{
    fprintf(stderr,
            "Usage: %s [options]    Read ME CSRs\n"
            "\nWhere options are:\n"
            "  -n <dev no>          Device number (default: 0)\n"
            "  -m <ME name/number>  ME Name/number (default: mei0.me0)\n"
            "  -d <debug level>     Debug Level\n"
            "  -w <delay>           Watch delay (seconds)\n"
            "  -v                   Verbose output\n"
            "  -h, -?               Help\n"
            "\n",
            program);

    exit(err ? EXIT_FAILURE : EXIT_SUCCESS);
}

static void sigint_handler(int signum)
{
    terminate++;
}

/* non-blocking stdin get key */
int getkey()
{
    int character;
    struct termios orig_term_attr;
    struct termios new_term_attr;

    /* set the terminal to raw mode */
    tcgetattr(fileno(stdin), &orig_term_attr);
    memcpy(&new_term_attr, &orig_term_attr, sizeof(struct termios));
    new_term_attr.c_lflag &= ~(ECHO|ICANON);
    new_term_attr.c_cc[VTIME] = 0;
    new_term_attr.c_cc[VMIN] = 0;
    tcsetattr(fileno(stdin), TCSANOW, &new_term_attr);

    /* read a character from the stdin stream without blocking */
    /* returns EOF (-1) if no character is available */
    character = fgetc(stdin);

    /* restore the original terminal attributes */
    tcsetattr(fileno(stdin), TCSANOW, &orig_term_attr);

    return character;
}

int main(int argc, char **argv)
{
    struct nfp_device *nfp;
    struct winsize ws;
    uint64_t new_values[MAX_VALUES];
    uint64_t old_values[MAX_VALUES] = { 0 };
    char meid_str[NFP_MEID_STR_SZ];
    const char *meid_str_ptr;
    const char *program = argv[0];
    const char *name;
    uint64_t me_cycle_count;
    uint32_t value;
    char key_pressed = 0;
    char valid;
    int offset;
    int ctx;
    int meid;
    int retval;
    int watch_delay;
    int watch_delay1;
    int watch;
    int max_str_len = 0;
    int pos_x = 0, pos_y = 0;
    int watch_count;
    int key = -1;
    int opt = 0;
    int devnum;
    int i;

    signal(SIGINT, sigint_handler);
    signal(SIGTERM, sigint_handler);

    /* Initialise globalargs to defaults */
    globalargs.meid_str = NULL;
    globalargs.verbose = 0;
    globalargs.watch_delay = 0;
    globalargs.nfp_debug_level = NS_LOG_LVL_INFO;
    globalargs.devnum = 0;

    /* Initialise variables */
    meid = DEFAULT_MEID;
    watch_delay = DEFAULT_WATCH_DELAY;
    watch = 0;

    /* Process the arguments with getopt(), then populate globalargs */
    while (1) {
        opt = getopt(argc, argv, optstring);
        if (opt == -1)
            break;

        switch( opt ) {
            case 'h':
            case '?':
                usage(program, 0);
                break;
            case 'm':
                globalargs.meid_str = optarg;
                break;
            case 'v':
                globalargs.verbose++;
                break;
            case 'd':
                globalargs.nfp_debug_level = atoi(optarg);
                break;
            case 'n':
                globalargs.devnum = atoi(optarg);
                break;
            case 'w':
                globalargs.watch_delay = atoi(optarg);
            default:
                break;
        }
    }

    devnum = globalargs.devnum;
    if (globalargs.watch_delay != 0) {
        watch_delay = globalargs.watch_delay;
        watch = 1;
    }

    /* Work out the MEID to use */
    if (globalargs.meid_str != NULL) {
        if ((globalargs.meid_str[0] == '0') && (globalargs.meid_str[1] == 'x')) {
            meid = strtoull(globalargs.meid_str, NULL, 0);
        } else {
            retval = nfp6000_idstr2meid(globalargs.meid_str, NULL);
            if (retval < 0) {
                meid = -1;
            } else {
                meid = retval;
            }
        }
    }

    /* Validate the meid */
    if (!NFP6000_MEID_IS_VALID(meid)) {
        fprintf(stderr, "%s: Invalid MEID: %s\n", program, globalargs.meid_str);
        exit(EXIT_FAILURE);
    }

    /* Get the alt name for the MEID */
    meid_str_ptr = nfp6000_meid2altstr(meid_str, meid);
    if (meid_str_ptr == NULL)
        sprintf(meid_str, "%04x", meid);

    /* Set NS log level */
    NS_LOG_SET_LEVEL(globalargs.nfp_debug_level);

    /* Open NFP Device */
    if (globalargs.verbose)
        printf("opening nfp_device (%d)\n", devnum);

    nfp = nfp_device_open(devnum);
    if (!nfp) {
        fprintf(stderr,
                "%s: ERROR: nfp_device_open (%d) failed\n", program, devnum);
        exit(EXIT_FAILURE);
    }

    if (watch) {
        CLS();
        watch_count = 0;
        /* Find the maximum length string in our all_mestate_types[] */
        /* Used for formatting the screen when in watch mode */
        max_str_len = 0;
        i = 0;
        do {
            name = (char *) all_mecsr_info[i].name;
            if (name != NULL) {
                if (strlen(name) > max_str_len) max_str_len = strlen(name);
                i++;
            }
        } while (name != NULL);
    }
    max_str_len += 9;

    while (!terminate) {
        if (watch == 0)
            terminate = 1;

        if (watch) {
            /* Display a header on the screen when in watch mode */
            CLS();
            pos_x = 1;
            pos_y = 1;
            POS(pos_x, pos_y);
            me_cycle_count = nfp_sal_cycle_count(nfp);
            printf("ME:%s (%04x)  ME CSRs run_count:%d  "
                   "SIM cycle:%ld watch delay:%d secs",
                   meid_str, meid, watch_count++, me_cycle_count, watch_delay);
            ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
            pos_y = 2;

            /* Handle the keypress if we've had one */
            if (key_pressed != 0) {
                watch_delay = key_pressed - 0x30;
                if ((watch_delay < 0) || (watch_delay > 9)) {
                    watch_delay = DEFAULT_WATCH_DELAY;
                }
                if (key_pressed == 'q')
                    terminate = 1;
                key_pressed = 0;
            }
        }

        /* Display all ME CSRs */
        for (i = 0; all_mecsr_info[i].name; i++) {
            name = all_mecsr_info[i].name;
            offset = all_mecsr_info[i].offset;
            ctx = all_mecsr_info[i].ctx;

            /* Handle the cursor position if in watch mode */
            if (watch) {
                POS(pos_x, pos_y);
                pos_y += 1;
                if (pos_y > (ws.ws_row-1)) {
                    pos_x += (max_str_len + 8 + 2 + 2 - 5);
                    pos_y = 2;
                }
                if ((pos_x > (ws.ws_col - 8 - 2 - 2 - max_str_len - 5 ))) {
                    break;
                }
            }

            /* Print the ME CSR name */
            printf("%20s ", name);
            if (ctx == -1)
                RED();
            printf("(%04x) ", offset);
            if (ctx == -1)
                NORMAL();

            /* Read the ME CSR */
            valid = 1;
            if (ctx == -1)
                retval = nfp_mecsr_read(nfp, meid, 0, offset, &value);
            else
                retval = nfp_mecsr_read(nfp, meid, ctx, offset, &value);

            if (retval < 0) {
                value = -1;
                /* Indicate we have had a failure reading the ME CSR */
                valid = 0;
            }

            /* Let's be fancy! if there is a change in state from previous
             * state, indicate this in RED
             */
            new_values[i] = value;
            if (old_values[i] != new_values[i]) {
                if (watch)
                    RED();
                old_values[i] = new_values[i];
            } else {
                if (watch)
                    NORMAL();
            }

            if (valid)
                printf("%08x\n", value);
            else
                printf("READFAIL\n");

            if (watch)
                NORMAL();
        }

        if (!watch)
            continue;

        /* Delay if we are in watch mode */
        watch_delay1 = watch_delay * 2;
        while (watch_delay1--) {
            usleep(500000); /* 500ms delay */
            key = getkey();
            if (key != -1) {
                if (key_pressed == 0) {
                    key_pressed = (char) key;
                }
            }
        }
    }

    /* Close/Free NFP handles */
    nfp_device_close(nfp);

    printf("done\n");
    return 0;
}
