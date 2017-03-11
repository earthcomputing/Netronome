/**
 * Copyright (C) 2013 Netronome Systems, Inc.  All rights reserved.
 *
 * Example IPC client which uses the NFHAL/NFSAL APIs
 *
 * @file          load_me.c
 * @brief         Example code loader from .list file or UC opcode HEX file
 *                with various options to interact with a SIM from the Command Line
 *
 */

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>

#include <pthread.h>

#include <sys/stat.h>
#include <sys/types.h>
//#include <sys/ioctl.h>

#include <nfp-common/nfp_platform.h>
#include <nfp-common/nfp_resid.h>
#include <nfp-common/ns_log.h>
#include <nfp-common/nfp_platform.h>

#include <nfp.h>
#include <nfp_cpp.h>
#include <nfp_hwinfo.h>
#include <nfp_sal.h>
#include <sim_events.h>

#include "read_mecsr.h"
#include "simevent_names.h"

/***** Defines *****/

#define DEFAULT_UC_FILE         "/tmp/uc"
#define ME_MEM_SIZE             1024
#define UC_HEX_OPCODE_LENGTH    14
#define DEFAULT_MEID            0x204       /* mei0.me0, i32.me0 */

#define MAX_LINE_LENGTH         128
#define MAX_STR_LENGTH          128
#define MAX_ISLANDS             64

#define TIMEOUT_INTERVAL_MS     100         /* Timeout interval in ms */
#define TIMEOUT_STEP_MS         1000        /* Timeout for an a single step */
#define DEFAULT_TIMEOUT         30          /* Default timeout in seconds for command/callback event response */
#define CALC_CB_TIMEOUT(x)      ((x) * 1000 / TIMEOUT_INTERVAL_MS)
#define CB_TIMEOUT_TO_SEC(x)    ((x) * TIMEOUT_INTERVAL_MS / 1000)

struct globalargs_data {
    int verbose;                /* -v verbose */
    const char *meid_str;       /* -m <MEID name> */
    const char *in_fname;       /* -i input file (.list or UC opcodes */
    int nfp_debug_level;        /* -d <DEBUG LEVEL> */
    int dev_no;                 /* -n <NFP Device number> */
    int no_load;                /* Don't load ME code */
    int sim_reset;              /* -r reset SIM before load */
    int sim_bsp_reset;          /* -b do a bsp reset SIM before load */
    int sim_run;                /* -R run SIM */
    int sim_stop;               /* -S stop SIM */
    int sim_shutdown;           /* -k shutdown SIM */
    int sim_read_cycle_count;   /* -c read SIM cycle count */
    int sim_set_ctx_cc;         /* -g Set CTX Enables and CC */
    int sim_clr_ctx;            /* -G Clear CTX Enables */
    int sim_query;              /* -q Query SIM Status */
    const char *sim_en_clks;    /* -e <list of island clocks> */
    const char *sim_dis_clks;   /* -f <list of island clocks> */
    int sim_en_all_clks;        /* -E Enable all island clocks */
    int sim_dis_all_clks;       /* -F Disable all island clocks */
    int sim_step;               /* -s <step number> */
    int sim_cb_timeout;         /* -t <timeout> */
};

/***** Globals *****/

int g_event_type;
char g_event_occured;
struct nfp_device *g_nfp;
pthread_mutex_t g_evtmtx = PTHREAD_MUTEX_INITIALIZER;

struct globalargs_data globalargs;
static const char *optstring = "rbvh?m:i:d:HRSkcgn:Gqe:f:EFs:t:";


/* Display program usage, and exit.  */
void usage(char *argv[])
{
    fprintf(stderr,
            "Usage: %s [options]\n"
            "\nWhere options are:\n"
            "  -n <dev no>       Device number (default:0)\n"
            "  -i <filename>     ME .list file or UC HEX OPCODE FILE\n"
            "  -m <ME name>      ME Name (default:mei0.me0)\n"
            "  -r                Reset SIM\n"
            "  -b                BSP Reset SIM\n"
            "  -R                RUN SIM\n"
            "  -s <steps>        Step SIM, <steps> times\n"
            "  -S                STOP SIM\n"
            "  -k                Shutdown SIM\n"
            "  -c                Read SIM cycle count\n"
            "  -g                Set ME CTX Enables and CC\n"
            "  -G                Clear ME CTX Enables\n"
            "  -F                Disable all island clocks\n"
            "  -f <list>         Disable a list of island clocks e.g. 4,9,28-29\n"
            "  -E                Enable all island clocks\n"
            "  -e <list>         Enable a list of island clocks e.g. 4,9,32-35\n"
            "  -d <debug level>  Debug Level\n"
            "  -q                Query SIM Status (Islands/Clocks)\n"
            "  -t <seconds>      SIM connection timeout (default:30 seconds)\n"
            "  -v                Verbose output\n"
            "  -h, -?            Help\n"
            "\n"
            "example usage:\n"
            "  load_me -i file1.list -m mei0.me0 -r -v        Reset SIM and load list file\n"
            "                                                 into mei0.me0, Verbose output\n"
            "  load_me -i file2.list -m mei0.me1              Load list file into mei0.me1\n"
            "  load_me -i file1.list -m mei0.me0 -r -g -R -v  Reset SIM and load list file\n"
            "                                                 into mei0.me0, set Contexts and\n"
            "                                                 CC and RUN SIM, verbose output\n",
            argv[0]);
    exit(EXIT_FAILURE);
}

/**
 * Check if string is digit
 *
 * @param p     Pointer to string (asciiz)
 *
 * @return 1 on success, or 0 on failure
 */
int isstrdigit(char *p) {
    char *ptr = p;
    char c;
    int retval = 1;

    do {
        c = *ptr++;
        if (c == '\0') break;
        if (isdigit(c)) {
        } else {
            retval = 0;
        }
    } while ((c != '\0') && (retval == 1));

    return retval;
}

/**
 * Check if string is isxdigit (0-9, A-F, a-f)
 *
 * @param p     Pointer to string (asciiz)
 *
 * @return number of xdigits on success, or 0 on failure
 */
int isstrxdigit(char *p) {
    char *ptr = p;
    char c;
    int retval = 0;

    do {
        c = *ptr++;
        if (c == '\0') break;
        if (isxdigit(c)) {
            retval++;
        } else {
            retval = 0;
        }
    } while ((c != '\0') && (retval != 0));

    return retval;
}

/**
 * Convert UC .list or UC ME HEX OPCODE file to a buffer to load into ME USTORE
 *
 * @param filename      Filename
 * @param wmem          UC Memory Buffer
 *
 * @return words to write to ME UC on sucess, else -1 on error
 */
int create_uc_buffer(char *filename, uint64_t *wmem)
{
    FILE *file;
    char line[MAX_LINE_LENGTH];
    char str[MAX_STR_LENGTH];
    char str1[MAX_STR_LENGTH];
    char str2[MAX_STR_LENGTH];
    uint64_t word;
    int index = 0;
    int retval = 0;

    file = fopen(filename, "r");
    if (file) {
        /* Check we have opened the file */
        while (fgets(line, 128, file) != NULL) {
            str[0]  = 0;
            str1[0] = 0;
            str2[0] = 0;
            sscanf(line, "%s %s %s", str, str1, str2);

            if ((isstrdigit(str+1) == 1) && (strstr(str2, "common_code") != NULL)) {
                word = strtol((char *)str1, NULL, 16);
                wmem[index++] = word;
            } else {
                if ((isstrxdigit(str) == UC_HEX_OPCODE_LENGTH) &&
                    (str1[0] == 0) &&
                    (str2[0] == 0)) {
                    word = strtol((char *)str, NULL, 16);
                    wmem[index++] = word;
                }
            }
        }
        retval = index;
        fclose(file);
    } else {
        fprintf(stderr, "Unable to open file %s (%s)\n",
                filename, strerror(errno));
        retval = -1;
    }

    return retval;
}

/**
 * Lookup Sim event type name
 *
 * @param  ev_type          ev_type enum number
 *
 * @return pointer to ev_type name, or NULL if name not found
 */
char *lookup_ev_type(uint32_t ev_type)
{
    int i;
    char *retval = NULL;
    const char *ev_type_name = NULL;

    i = 0;
    while (1) {
        ev_type_name = all_simevent_names[i].name;
        if (!ev_type_name)
            break;

        if (ev_type == all_simevent_names[i].type) {
            retval = (char *)ev_type_name;
            break;
        }
        i++;
    }

    return retval;
}

/* Callback function for simulator events */
void ievent_handler(struct nfp_device *dev,
                    uint32_t ev_type,
                    void *ev_data,
                    uint32_t ev_data_len)
{
    char *ev_type_name = NULL;

    ev_type_name = lookup_ev_type(ev_type);

    if (globalargs.verbose) {
        if (ev_type_name != NULL) {
            printf("got event[%d:%s] @%p [%u bytes]\n",
                    ev_type, ev_type_name, ev_data, ev_data_len);
        } else {
            printf("got event[%d] @%p [%u bytes]\n",
                   ev_type, ev_data, ev_data_len);
        }
    }

    /* Check event type we are waiting for and set a flag */
    pthread_mutex_lock(&g_evtmtx);
    if (ev_type == g_event_type) {
        g_event_occured = 1;
    }
    pthread_mutex_unlock(&g_evtmtx);
}

/**
 * Prepare to wait for a particular type of sim event
 * @param ev_type       Sim event type
 */

void prime_for_sim_event(int ev_type)
{
    /* Set the variables that the callback uses */
    pthread_mutex_lock(&g_evtmtx);
    g_event_occured = 0;
    g_event_type = ev_type;
    pthread_mutex_unlock(&g_evtmtx);
}

/**
 * Wait for a sim event to occur within a period of time
 *
 * @param timeout       timeout (in multiple of 100ms)
 *
 * @return number of 100ms counts on success, or -1 on failure
 */
int wait_for_sim_event(int timeout)
{
    int retval;
    int tc = timeout;

    /* Set the variables that the callback uses */
    do {
        /* Delay */
        nfp_sleep_ms(TIMEOUT_INTERVAL_MS);
    } while ((tc--) && (g_event_occured == 0));

    if (g_event_occured == 1) {
        retval = timeout - tc;
    } else {
        retval = -1;
    }

    return retval;
}

/**
 * Simulator Reset
 *
 * @param nfp           NFP device
 * @param timeout       timeout (in multiple of 100ms)
 *
 * @return number of 100ms counts on success, or -1 on failure
 */
int sim_reset(struct nfp_device *nfp, int timeout, int level)
{
    /* Issue a non-blocking RESET */
    prime_for_sim_event(SIMEVENT_RUN_RESETDONE);
    if (nfp_sal_reset(nfp, level, 0) < 0)
        return -1;
    return wait_for_sim_event(timeout);
}

/**
 * Simulator Stop
 *
 * @param nfp           NFP device
 * @param timeout       timeout (in multiple of 100ms)
 *
 * @return number of 100ms counts on success, or -1 on failure
 */
int sim_stop(struct nfp_device *nfp, int timeout)
{
    /* Issue a non-blocking SIM STOP */
    prime_for_sim_event(SIMEVENT_RUN_STOP);
    if (nfp_sal_stop(nfp, 0) < 0)
        return -1;
    return wait_for_sim_event(timeout);
}

/**
 * Simulator Run
 *
 * @param nfp           NFP device
 * @param timeout       timeout (in multiple of 100ms)
 *
 * @return number of 100ms counts on success, or -1 on failure
 */
int sim_run(struct nfp_device *nfp, int timeout)
{
    /* Issue a Run SIM */
    /* Check if SIM is running */
    int retval = nfp_sal_is_running(nfp);
    if (retval < 0) {
        fprintf(stderr, "Error checking is SIM is running\n");
        exit(EXIT_FAILURE);
    }
    if (retval == 1) {
        printf("sim_run() SIM is running\n");
    } else if (retval == 0) {
        printf("sim_run() SIM is not running\n");
        prime_for_sim_event(SIMEVENT_RUN_START);
        if (nfp_sal_run(nfp) < 0)
             return -1;
        return wait_for_sim_event(timeout);   
    }
    return 0 ;

}

/**
 * Simulator Step
 *
 * @param nfp           NFP device
 * @param steps         Number of steps
 * @param timeout       timeout (in multiple of 100ms)
 *
 * @return number of 100ms counts on success, or -1 on failure
 */
int sim_steps(struct nfp_device *nfp, int steps, int timeout)
{
    /* Issue a non-blocking Step SIM */
    prime_for_sim_event(SIMEVENT_RUN_STOP);
    if (nfp_sal_clock_step(nfp, steps, 0) < 0)
        return -1;
    return wait_for_sim_event(timeout);
}

/**
 * Simulator Shutdown
 *
 * @param nfp           NFP device
 *
 * @return 0 on success, or -1 on failure
 */
int sim_shutdown(struct nfp_device *nfp)
{
    /* Issue a Shutdown SIM */
    return nfp_sal_shutdown(nfp);
}

/**
 * Simulator Read ME cycle count
 *
 * @param nfp           NFP device
 *
 * @return the current ME cycle
 */
uint64_t sim_cycle_count(struct nfp_device *nfp)
{
    return nfp_sal_cycle_count(nfp);
}


/**
 * Simulator Set ME Contexts and CC
 *
 * @param nfp           NFP device
 * @param meid          MEID
 *
 * @return 0 on success, -1 on failure
 */
int sim_set_ctx_cc(struct nfp_device *nfp, int meid)
{
    /* Enable all Contexts */
    if (nfp_mecsr_write(nfp, meid, -1, NFP_ME_CtxEnables, 0x0000ff00) < 0)
        return -1;
    /* Enable CC */
    if (nfp_mecsr_write(nfp, meid, -1, NFP_ME_CondCodeEn, (1 << 13)) < 0)
        return -1;

    return 0;
}

/**
 * Simulator Clear ME Contexts
 *
 * @param nfp        NFP device
 * @param meid       ME ID
 *
 * @return 0 on success, -1 on failure
 */
int sim_clr_ctx(struct nfp_device *nfp, int meid)
{
    uint32_t value;

    /* Read the CTX Enables */
    if (nfp_mecsr_read(nfp, meid, -1, NFP_ME_CtxEnables, &value) < 0)
        return -1;
    /* Clear the CTX Enables */
    value = value & 0xffff00ff;
    if (nfp_mecsr_write(nfp, meid, -1, NFP_ME_CtxEnables, value) < 0)
        return -1;

    return 0;
}

/**
 * Display Simulator Info
 *
 */
void sim_info()
{
    const char *sim_version;
    uint32_t target_period, actual_period;
    int retval, i, c, nl;

    sim_version = nfp_hwinfo_lookup(g_nfp, "sim.version");
    if (!sim_version)
        printf("Simulator version unknown\n");
    else
        printf("Simulator version: %s\n", sim_version);

    /* Check if SIM is running */
    retval = nfp_sal_is_running(g_nfp);
    if (retval < 0) {
        fprintf(stderr, "Error checking is SIM is running\n");
        exit(EXIT_FAILURE);
    }
    if (retval == 1) {
        printf("SIM is running\n");
    } else if (retval == 0) {
        printf("SIM is not running\n");
    }
    /* List Island Clocks enabled */
    printf("Island Clocks\n");
    c = 0;
    nl = 0;
    for (i = 0; i < MAX_ISLANDS; i++) {
        retval = nfp_sal_island_clock_is_enabled(g_nfp, i);
        if (retval < 0)
            continue; /* Error: No clock on island */
        nl = 1;
        printf("%si%d : %s",
               i > 9 ? "" : " ", i, retval == 1 ? "on     " : "off    ");
        if (c % 4 == 3) {
            nl = 0;
            printf("\n");
        }
        c++;
    }
    if (nl) /* print a new line if we haven't yet */
        printf("\n");
    printf("%d Island Clocks\n", c);

    {
        int ret ;
        int menum = NFP6000_MEID(32, 0);
        uint32_t val;

        /* List Clock Frequencies */
        int clockids[7] = {NFP_SAL_CLOCK_TYPE_TCLK,
                           NFP_SAL_CLOCK_TYPE_PCLK,
                           NFP_SAL_CLOCK_TYPE_ILA0CLK,
                           NFP_SAL_CLOCK_TYPE_ILA1CLK,
                           NFP_SAL_CLOCK_TYPE_D0CLK,
                           NFP_SAL_CLOCK_TYPE_D1CLK,
                           NFP_SAL_CLOCK_TYPE_D2CLK};
        const char *clockstrs[7] = {"TCLK:   ", "PCLK:   ",
                                    "ILA0CLK:", "ILA1CLK:",
                                    "D0CLK:  ", "D1CLK:  ", "D2CLK:  "};
        printf("Clock Speeds\n");
        for (i = 0; i < sizeof(clockids)/sizeof(clockids[0]); i++) {
            retval = nfp_sal_clock_info(g_nfp, clockids[i],
                                        &target_period, &actual_period);
            printf("%s\t", clockstrs[i]);
            if (retval < 0)
                printf("Error\n");
            else
                printf("fTarget: %.0f\t\tfActual: %.0f\n",
                       1000000.0 / target_period, 1000000.0 / actual_period);
        }

#define NFP_ME_MAILBOX_0                                     0x00000170

        ret = nfp_mecsr_read(g_nfp, menum, -1, NFP_ME_MAILBOX_0 , &val);
        if (ret)
            printf("error reading Mailbox%d: %s\n", 0, strerror(errno));
        else
            printf("Mailbox%d = %x\n", 0, val);
    }
}

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

/**
 * Enable/Disable Simulator island clocks
 *
 * @return total number of island clocks, or -1 on failure
 */
int sim_island_clocks()
{
    int en_clks[MAX_ISLANDS];
    int dis_clks[MAX_ISLANDS];
    int i;
    int num_clocks = 0;
    int retval = 0;

    /* Initialise the island clocks */
    for (i = 0; i < MAX_ISLANDS; i++) {
        if (globalargs.sim_en_all_clks == 1) {
            en_clks[i] = 1;
        } else {
            en_clks[i] = 0;
        }
        if (globalargs.sim_dis_all_clks == 1) {
            dis_clks[i] = 1;
        } else {
            dis_clks[i] = 0;
        }
    }

    /* Get the list of clocks to enable */
    if ((globalargs.sim_en_clks != NULL) && (globalargs.sim_en_all_clks == 0)) {
        retval = process_num_str((char *) globalargs.sim_en_clks, (int *)&en_clks, MAX_ISLANDS);
    }
    if (retval < 0)
        return retval;
    /* Get the list of clocks to disable */
    if ((globalargs.sim_dis_clks != NULL)  && (globalargs.sim_dis_all_clks == 0)) {
        retval = process_num_str((char *) globalargs.sim_dis_clks, (int *)&dis_clks, MAX_ISLANDS);
    }
    if (retval < 0)
        return retval;

    /* Let's see what to do with the island clocks */
    for (i = 0; i < MAX_ISLANDS; i++) {
        retval = nfp_sal_island_clock_is_enabled(g_nfp, i);
        if (retval < 0) {
            /* Island clock does not exist */
            continue;
        }
        num_clocks++;

        /* Island clock is enabled */
        if (retval == 1) {
            /* Only disable the clock if it was enabled */
            if ((dis_clks[i] == 1) && (en_clks[i] == 0)) {
                if (globalargs.verbose) {
                    printf("Calling nfp_sal_island_clock_disable on %d\n", i);
                }

                retval = nfp_sal_island_clock_disable(g_nfp, i);

                if (globalargs.verbose) {
                    printf("Called nfp_sal_island_clock_disable on %d and got %d\n", i, retval);
                }

            }
        }
        /* Island clock is disabled */
        if (retval == 0) {
            if (en_clks[i] == 1) {
                if (globalargs.verbose) {
                    printf("Calling nfp_sal_island_clock_enable on %d\n", i);
                }
                retval = nfp_sal_island_clock_enable(g_nfp, i);
                if (globalargs.verbose) {
                    printf("Called nfp_sal_island_clock_enable on %d and got %d\n", i, retval);
                }
            }
        }
    }

    return num_clocks;
}

int main(int argc, char *argv[] )
{
    uint64_t uc_mem[ME_MEM_SIZE * 8 * 8];
    char meid_str[NFP_MEID_STR_SZ];
    char filename[MAX_STR_LENGTH];
    const char *meid_str_ptr;
    int opt = 0;
    int meid;
    int retval;
    int me_words;
    int me_offset = 0;
    int n;

    /* Initialise globalargs to defaults */
    globalargs.meid_str = NULL;
    globalargs.in_fname = NULL;
    globalargs.verbose = 0;
    globalargs.nfp_debug_level = NS_LOG_LVL_INFO;
    globalargs.no_load = 0;
    globalargs.sim_reset = 0;
    globalargs.sim_bsp_reset = 0;
    globalargs.sim_run = 0;
    globalargs.sim_stop = 0;
    globalargs.sim_shutdown = 0;
    globalargs.sim_read_cycle_count = 0;
    globalargs.sim_set_ctx_cc = 0;
    globalargs.dev_no = 0;
    globalargs.sim_query = 0;
    globalargs.sim_en_all_clks = 0;
    globalargs.sim_dis_all_clks = 0;
    globalargs.sim_en_clks = NULL;
    globalargs.sim_dis_clks = NULL;
    globalargs.sim_step = -1;
    globalargs.sim_cb_timeout = CALC_CB_TIMEOUT(DEFAULT_TIMEOUT);

    /* Initialise variables */
    g_event_type = -1;
    me_words = 0;

    /* Process the arguments with getopt(), then populate globalargs */
    n = 0;
    while (1) {
        opt = getopt(argc, argv, optstring);
        if (opt == -1)
            break;
        n++;

        switch( opt ) {
            case 'g':
                globalargs.sim_set_ctx_cc = 1;
                break;
            case 'G':
                globalargs.sim_clr_ctx = 1;
                break;
            case 'r':
                globalargs.sim_reset = 1;
                break;
            case 'b':
                globalargs.sim_bsp_reset = 1;
                break;
            case 'S':
                globalargs.sim_stop = 1;
                break;
            case 's':
                globalargs.sim_step = atoi(optarg);
                break;
            case 'R':
                globalargs.sim_run = 1;
                break;
            case 'k':
                globalargs.sim_shutdown = 1;
                break;
            case 'c':
                globalargs.sim_read_cycle_count = 1;
                break;
            case 'd':
                globalargs.nfp_debug_level = atoi(optarg);
                break;
            case 'm':
                globalargs.meid_str = optarg;
                break;
            case 'i':
                globalargs.in_fname = optarg;
                break;
            case 'v':
                globalargs.verbose++;
                break;
            case 'n':
                globalargs.dev_no = atoi(optarg);
                break;
            case 'q':
                globalargs.sim_query = 1;
                break;
            case 'h':
            case '?':
                usage(argv);
                break;
            case 'e':
                globalargs.sim_en_clks = optarg;
                break;
            case 'f':
                globalargs.sim_dis_clks = optarg;
                break;
            case 'E':
                globalargs.sim_en_all_clks = 1;
                break;
            case 'F':
                globalargs.sim_dis_all_clks = 1;
                break;
            case 't':
                globalargs.sim_cb_timeout = CALC_CB_TIMEOUT(atoi(optarg));
            default:
                break;
        }
    }
    /* if no arguments were provided print usage and exit */
    if (n == 0)
        usage(argv);

    /* Work out the MEID to use */
    if (globalargs.meid_str == NULL) {
        meid = DEFAULT_MEID;
    } else {
        retval = nfp6000_idstr2meid(globalargs.meid_str, NULL);
        if (retval < 0) {
            fprintf(stderr, "Invalid ME ID: %s\n", globalargs.meid_str);
            return -1;
        } else {
            meid = retval;
        }
    }

    /* Get the alt name for the MEID */
    meid_str_ptr = nfp6000_meid2altstr(meid_str, meid);
    if (meid_str_ptr == NULL) {
        sprintf(meid_str, "%04x", meid);
    }

    if (globalargs.in_fname != NULL) {
        /* Get filename for loading into ME */
        if (globalargs.in_fname == NULL) {
            strcpy(filename, (char *) DEFAULT_UC_FILE);
        } else {
            strcpy(filename, (char *) globalargs.in_fname);
        }

        if (globalargs.verbose) {
            printf("Creating ME UC Buffer from '%s'\n", filename);
        }
        /* Create the ME USTORE buffer from the filename */
        me_words = create_uc_buffer(filename, uc_mem);
        if (me_words == -1) {
            fprintf(stderr, "error: Unable to load file %s\n", filename);
            exit(EXIT_FAILURE);
        }

        if (me_words == 0) {
            if (globalargs.verbose) {
                printf("Created %d words (64 bit) for ME\n", me_words);
            } else {
                printf("Nothing to write to ME. Run with -v option\n");
            }
            exit(EXIT_FAILURE);
        }
    }

    /* Set NS log level */
    NS_LOG_SET_LEVEL(globalargs.nfp_debug_level);

    /* Open NFP Device */
    if (globalargs.verbose) {
        printf("opening nfp_device (%d)\n", globalargs.dev_no);
    }
    g_nfp = nfp_device_open(globalargs.dev_no);
    if (!g_nfp) {
        fprintf(stderr, "error: nfp_device_open (%d) failed\n", globalargs.dev_no);
        exit(EXIT_FAILURE);
    }

    /* Setup the event handler */
    if (globalargs.verbose) {
        printf("Setting up SIM Event callback (Timeout:%d seconds)\n", CB_TIMEOUT_TO_SEC(globalargs.sim_cb_timeout));
    }
    nfp_sal_subs_simevent_all(g_nfp, &ievent_handler);

    /* Check if we have a buffer to load into the ME */
    if (me_words > 0) {

        if (globalargs.verbose) {
            printf("Created %d words (64 bit) for ME\n", me_words);
        }
        if (globalargs.sim_bsp_reset == 1) {
            /* Reset the SIM if option set before loading ME code */
            if (globalargs.verbose) {
               printf("Resetting SIM before load\n");
            }
            retval = sim_reset(g_nfp, globalargs.sim_cb_timeout, NFP_SAL_RESET_LEVEL_BSP);
            if (retval < 0) {
                fprintf(stderr,"Unable to reset SIM...Try again!\n");
                exit(EXIT_FAILURE);
            }
            printf("SIM Reset Done (%d x 100ms)\n", retval);
        }

        if (globalargs.sim_reset == 1) {
            /* Reset the SIM if option set before loading ME code */
            if (globalargs.verbose) {
               printf("Resetting SIM before load\n");
            }
            retval = sim_reset(g_nfp, globalargs.sim_cb_timeout, NFP_SAL_RESET_LEVEL_CHIP);
            if (retval < 0) {
                fprintf(stderr,"Unable to reset SIM...Try again!\n");
                exit(EXIT_FAILURE);
            }
            printf("SIM Reset Done (%d x 100ms)\n", retval);
        }
        if (globalargs.verbose) {
            printf("Doing nfp_ustore_write\n");
        }
        retval = nfp_ustore_write(g_nfp, meid, uc_mem, ME_MEM_SIZE * 8 * 8, me_offset);
        if (retval < 0) {
            fprintf(stderr, "error:nfp_ustore_write() failed : %s\n", strerror(errno));
        }
        if (globalargs.verbose) {
            printf("%d (0x%08x) bytes written to ME (%s:0x%x)\n", retval, retval, meid_str, meid);
        }

        /* Handle things AFTER we have successfully loaded the ME code */
        /* i.e. Set CSRs, Run, Step some cycles */
        /* ... */

    } else {
        /* Reset the SIM if option set before loading ME code */
        if (globalargs.sim_reset == 1) {
            if (globalargs.verbose) {
                printf("Resetting SIM before\n");
            }
            retval = sim_reset(g_nfp, globalargs.sim_cb_timeout, NFP_SAL_RESET_LEVEL_CHIP);
            if (retval < 0) {
                fprintf(stderr,"Unable to reset SIM...Timeout\n");
                exit(EXIT_FAILURE);
            }
            printf("SIM Reset Done (%d seconds)\n", CB_TIMEOUT_TO_SEC(retval));
        }
        if (globalargs.sim_bsp_reset == 1) {
            /* Reset the SIM if option set before loading ME code */
            if (globalargs.verbose) {
               printf("Resetting SIM before load\n");
            }
            retval = sim_reset(g_nfp, globalargs.sim_cb_timeout, NFP_SAL_RESET_LEVEL_BSP);
            if (retval < 0) {
                fprintf(stderr,"Unable to reset SIM...Try again!\n");
                exit(EXIT_FAILURE);
            }
            printf("SIM Reset Done (%d x 100ms)\n", retval);
        }
    }

    /* Check and see if we need to set the ME Contexts and CC */
    if (globalargs.sim_clr_ctx == 1) {
        retval = sim_clr_ctx(g_nfp, meid);
        if (retval < 0) {
            fprintf(stderr,"Unable to clear ME CTX Enables\n");
            exit(EXIT_FAILURE);
        }
        printf("Cleared CTX Enables for ME (%s:0x%x)\n", meid_str, meid);
    }

    /* Check and see if we need to set the ME Contexts and CC */
    if (globalargs.sim_set_ctx_cc == 1) {
        retval = sim_set_ctx_cc(g_nfp, meid);
        if (retval < 0) {
            fprintf(stderr,"Unable to set ME CTX & CC\n");
            exit(EXIT_FAILURE);
        }
        printf("Enabled CTX and CC for ME (%s:0x%x)\n", meid_str, meid);
    }

    /* Check what clocks to Enable */
    if ((globalargs.sim_en_clks != NULL) ||
        (globalargs.sim_dis_clks != NULL) ||
        (globalargs.sim_en_all_clks == 1) ||
        (globalargs.sim_dis_all_clks == 1)) {
            retval = sim_island_clocks();
            if (retval < 0) {
                fprintf(stderr, "Error with list of island clocks\n");
                exit(EXIT_FAILURE);
            } else {
                printf("%d island clocks\n", retval);
            }
    }

    /* Check and see if we need run RUN the SIM */
    if (globalargs.sim_run == 1) {
        retval = sim_run(g_nfp, globalargs.sim_cb_timeout);
        if (retval < 0) {
            fprintf(stderr,"Unable to RUN SIM...Maybe already running or Timeout\n");
            exit(EXIT_FAILURE);
        }
        printf("SIM RUN Done (%d x 100ms)\n", retval);
    }

    /* Check and see if we need to STOP the SIM */
    if (globalargs.sim_stop == 1) {
        retval = sim_stop(g_nfp, globalargs.sim_cb_timeout);
        if (retval < 0) {
            fprintf(stderr,"Unable to STOP SIM...Maybe already Stopped or Timeout\n");
            exit(EXIT_FAILURE);
        }
        printf("SIM STOP Done (%d x 100ms)\n", retval);
    }

    /* Check and see if we need STEP the SIM */
    if (globalargs.sim_step != -1) {
        retval = sim_steps(g_nfp,
                           globalargs.sim_step,
                           globalargs.sim_step * TIMEOUT_STEP_MS / TIMEOUT_INTERVAL_MS);
        if (retval < 0) {
           fprintf(stderr,"Unable to STEP SIM...Maybe already Running or Timeout\n");
           exit(EXIT_FAILURE);
        }
    }

    /* Check and see if we need to Shutdown the SIM */
    if (globalargs.sim_shutdown == 1) {
        retval = sim_shutdown(g_nfp);
        if (retval < 0) {
            fprintf(stderr,"Unable to Shutdown SIM\n");
            exit(EXIT_FAILURE);
        }
        printf("SIM Shutdown Done\n");
    }

    /* Query SIM status */
    if (globalargs.sim_query == 1) {
        sim_info();
    }

    /* Read SIM cycle count */
    if (globalargs.sim_read_cycle_count == 1) {
        printf("%ld\n", sim_cycle_count(g_nfp));
    }

    /* Close NFP handle */
    if (globalargs.verbose) {
        printf("closing nfp_device (%d)\n", globalargs.dev_no);
    }
    nfp_device_close(g_nfp);

    return EXIT_SUCCESS;
}
