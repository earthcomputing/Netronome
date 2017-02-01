/**
 * Copyright (C) 2013 Netronome Systems, Inc.  All rights reserved.
 *
 * A shell based client tool to interface and interact to a simulator
 * or hardware debug server
 */

#include <errno.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <nfp-common/nfp_resid.h>
#include <nfp-common/ns_log.h>
#include <nfp.h>
#include <nfp_cpp.h>
#include <nfp_hwinfo.h>
#include <nfp_hwdbg.h>
#include <nfp_sal.h>

#ifdef USE_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

#include "interactive.h"
#include "intractv_client.h"
#include "intractv_hwdbg.h"

/*************************************************
 * Defines                                       *
 *************************************************/

#define MODE_HWDBG_STR "HWDBG"
#define MODE_SIM_STR   "SIM"

#define ARG_DEFAULT_NFP 0
#define DEFAULT_MEID_STR "i32.me0"
#define READLINE_HISTORY_FILE ".interactive_history"

#define PROMPTSTR "\n>> "
#define HWDBGPROMPTSTR "\n[HWDBG]>> "

/* define this macro to enable external link time commands */
#ifdef EXTERNALCMDS
extern struct command_data external_cmds[];
#endif

/*************************************************
 * Globals                                       *
 *************************************************/

struct interactive_data *interactive_glob = NULL;

/*************************************************
 * Global commands                               *
 *************************************************/

static int global_cmd_help_all(struct interactive_data *d, const char *argstr)
{
    int i;
    printf("\nHelp\n");
    for (i = 0; i < d->cmds_cnt; i++) {
        printf("\t%s\t%s\n", d->cmds[i]->cmdstr, d->cmds[i]->help);
    }
    return 0;
}

static int global_cmd_help(struct interactive_data *d, const char *argstr)
{
    int i;
    printf("\nHelp\n");
    for (i = 0; i < d->cmds_cnt; i++) {
        if (d->cmds[i]->mode != MODE_ANY && d->cmds[i]->mode != d->mode)
            continue;
        printf("\t%s\t%s\n", d->cmds[i]->cmdstr, d->cmds[i]->help);
    }
    return 0;
}

static int global_cmd_mode(struct interactive_data *d, const char *argstr)
{
    int mode;

    if (strlen(argstr) == 0) {
        mode = d->mode == MODE_SIM ? MODE_HWDBG : MODE_SIM;
    } else {
        if (!strcmp(argstr, MODE_HWDBG_STR)) {
            mode = MODE_HWDBG;
        } else if (!strcmp(argstr, MODE_SIM_STR)) {
            mode = MODE_SIM;
        } else {
            fprintf(stderr, "unrecognized mode %s\n", argstr);
            return -1;
        }
    }
    if (mode == MODE_SIM && !d->issim) {
        fprintf(stderr, "cannot use %s mode on hardware\n", MODE_SIM_STR);
        return -1;
    }
    d->mode = mode;
    printf("Mode now %s\n",
           d->mode == MODE_SIM ? MODE_SIM_STR : MODE_HWDBG_STR);
    return 0;
}

static int global_cmd_comment(struct interactive_data *d, const char *argstr)
{
    return 0;
}

static int global_cmd_print(struct interactive_data *d, const char *argstr)
{
    printf("%% %s\n", argstr);
    return 0;
}

static int global_cmd_sleep(struct interactive_data *d, const char *argstr)
{
    int ms;
    if (strlen(argstr) == 0)
        ms = 1000; /* default 1s */
    else
        ms = atoi(argstr);
    printf("sleeping %d milliseconds\n", ms);
    usleep(1000 * ms);
    return 0;
}

static int global_cmd_quit(struct interactive_data *d, const char *argstr)
{
    d->shutdown = 1;
    return 0;
}

static int global_cmd_script(struct interactive_data *d, const char *argstr)
{
    FILE *file;

    if (!argstr) {
        fprintf(stderr, "no scriptfile provided\n");
        return -1;
    }

    file = fopen(argstr, "r+");
    if (!file) {
        fprintf(stderr, "failed to open script file (%s)\n", strerror(errno));
        return -1;
    }

    if (d->scriptfile) {
        fclose(d->scriptfile);
        fprintf(stderr, "replacing running scriptfile with %s\n", argstr);
    } else {
        fprintf(stderr, "running scriptfile %s\n", argstr);
    }

    d->scriptfile = file;

    return 0;
}

struct command_data global_cmds[] = {
    { "h",     global_cmd_help_all, MODE_ANY,  "Help for all modes"},
    { "?",     global_cmd_help,     MODE_ANY,  "Help for current mode only"},
    { "q",     global_cmd_quit,     MODE_ANY,  "Quit"},
    { "D",     global_cmd_mode,     MODE_ANY,  "Toggle/set mode <arg mode>"},
    { "%",     global_cmd_comment,  MODE_ANY,  "Comment - no action"},
    { ".",     global_cmd_script,   MODE_ANY,  "run a script file [scriptfile]"},
    { "sleep", global_cmd_sleep,    MODE_ANY,  "Sleep in milliseconds <arg duration>"},
    { "print", global_cmd_print,    MODE_ANY,  "Print a string <arg string>"},
    { NULL } /* end of list */
    };

/*************************************************
 * Signal handlers                               *
 *************************************************/

void interactive_shutdown(void)
{
    interactive_glob->shutdown++;
#ifdef USE_READLINE
    abort();
#else
    if (interactive_glob->shutdown > 5) {
        fprintf(stderr,
                "%d outstanding shutdown requests, forcing exit",
                interactive_glob->shutdown);
        abort();
    }
#endif
}

static void shutdown_handler(int signum)
{
    interactive_shutdown();
}

/*************************************************
 * Init/Free                                     *
 *************************************************/

static void exit_usage(const char *program, int error)
{
    fprintf(error ? stderr : stdout ,
            "usage: %s [options]\n"
            "\nWhere options are:\n"
            "        -n <device> -- select NFP device, default %d\n"
            "        -H <device> -- select HWDBG device, default is NFP device\n"
            "        -s <script> -- run a script file then exit\n"
            "        -S          -- force no simulator support\n"
            "        -h          -- print help\n",
            program,
            ARG_DEFAULT_NFP);
    exit(error ? EXIT_FAILURE : EXIT_SUCCESS);
}

static void handle_args(struct interactive_data *d,
                        int argc, char **argv)
{
    int o;

    d->opt_forcenosim = 0;
    d->opt_nfp_dev = ARG_DEFAULT_NFP;
    d->opt_nfp_hwdbg_dev = -1; /* filled in at the end */
    d->opt_scriptfile = NULL;

    while (1) {
        o = getopt(argc, argv, "n:H:s:hS");
        if (o == -1)
            break;

        switch (o) {
        case 'n':
            d->opt_nfp_dev = atoi(optarg);
            break;
        case 'H':
            d->opt_nfp_hwdbg_dev = atoi(optarg);
            break;
        case 's':
            d->opt_scriptfile = optarg;
            break;
        case 'S':
            d->opt_forcenosim = 1;
            break;
        case 'h':
            exit_usage(argv[0], 0);
            break;
        default:
            exit_usage(argv[0], 1);
        }
    }

    if (d->opt_nfp_hwdbg_dev < 0)
        d->opt_nfp_hwdbg_dev = d->opt_nfp_dev;
}

static int do_init(struct interactive_data *d)
{
    /* defaults */
    d->verbose = 0;

    /* TODO: set as command line argument */
    NS_LOG_SET_LEVEL(NS_LOG_LVL_INFO);

    d->bp_inst_step_loaded = 0;
    d->bp_inst_step_busy = 0;

    d->evt_display = 1;
    d->evt_old_clk_space = -1;
    d->evt_new_clk_space = -1;

    d->readline_historyfile = READLINE_HISTORY_FILE;
#ifdef USE_READLINE
    read_history(d->readline_historyfile);
#endif

    d->hwdbg_breakid = 0;
    d->hwdbg_autoclear = 0;
    d->latency_usec = 1000000; /* default to 1 Second */

    d->h_nfp = nfp_device_open(d->opt_nfp_dev);
    if (!d->h_nfp) {
        fprintf(stderr, "nfp_device_open failed (%s)\n", strerror(errno));
        return -1;
    }

    d->h_nfp_cpp = nfp_cpp_from_device_id(d->opt_nfp_dev);
    if (!d->h_nfp_cpp) {
        fprintf(stderr, "error: nfp_cpp_from_device_id failed (%s)\n",
                strerror(errno));
        return -1;
    }

    d->h_hwdbg = nfp_hwdbg_open(d->opt_nfp_dev);
    if (!d->h_hwdbg) {
        fprintf(stderr, "nfp_hwdbg_open failed (%s)\n", strerror(errno));
        return -1;
    }

    nfp_hwdbg_subs_events_all(d->h_hwdbg, hwdbg_event_handler);

    /* determine mode */
    if (!nfp_hwinfo_lookup(d->h_nfp, "sim.version") || d->opt_forcenosim) {
        d->mode = MODE_HWDBG;
        d->issim = 0;
    } else {
        d->mode = MODE_SIM;
        d->issim = 1;
    }

    /* FIXME : use DEFAULT_MEID_STR */
    d->cmd_meid = 0x204; /* default meid */
    d->cmd_watchid = 0;
    d->cmd_stepctx = -1;

    d->cmds = NULL;
    d->cmds_cnt = 0;

    d->shutdown = 0;

    if (d->opt_scriptfile) {
        d->scriptfile = fopen(d->opt_scriptfile, "r");
        if (!d->scriptfile) {
            fprintf(stderr, "failed to open script file (%s)", strerror(errno));
            return -1;
        }
    } else
        d->scriptfile = NULL;

    if (d->issim)
        nfp_sal_subs_simevent_all(d->h_nfp, &client_event_handler);

    return 0;
};

static void do_free(struct interactive_data *d)
{
    if (d->h_nfp)
        nfp_device_close(d->h_nfp);

    if (d->h_nfp_cpp)
        nfp_cpp_free(d->h_nfp_cpp);

    if (d->h_hwdbg)
        nfp_hwdbg_close(d->h_hwdbg);

    if (d->cmds)
        free(d->cmds);

    if (d->scriptfile)
        fclose(d->scriptfile);

#ifdef USE_READLINE
    write_history(d->readline_historyfile);
#endif
}

/*************************************************
 * Command handling                              *
 *************************************************/

int handle_cmd(struct interactive_data *d, const char *cmdstr)
{
    char *cmd, *s;
    int found;
    int i;

    if (!cmdstr) {
        fprintf(stderr, "invalid cmd string\n");
        return -1;
    }

    /* skip over white space at beginning */
    s = (char *)cmdstr;
    while (1) {
        if (*s == '\0')
            return 0; /* nothing but white space in the string */
        if (!isspace(*s))
            break;
        s++;
    }

    cmd = strdup(s);
    if (!cmd) {
        fprintf(stderr, "failed to allocate cmd string\n");
        return -1;
    }

    /* make sure there is no white space at the end of the line */
    found = 0;
    for (i = strlen(cmd) - 1; i >= 0; i--) {
        if (isspace(cmd[i]))
            continue;
        cmd[i + 1] = '\0';
        found = 1;
        break;
    }
    if (!found) /* whole line was white space */
        cmd[0] = '\0';

    /* remove comments */
    s = strchr(cmd, '%');
    if (s)
        *s = '\0';

    /* whole line was comment so we ignore */
    if (strlen(cmd) == 0)
        return 0;

    s = cmd;
    /* find the end of the cmd str */
    while (1) {
        if (*s == '\0')
            break;
        if (isspace(*s)) {
            /* end the string there */
            *s = '\0';
            s++;
            /* now skip over the white space */
            while (1) {
                if (*s == '\0')
                    break;
                if (!isspace(*s))
                    break;
                s++;
            }
            break;
        }
        s++;
    }
    /* cmd is now a null termed string and s points to the args */
    for (i = 0; i < d->cmds_cnt; i++) {
        if (strcmp(d->cmds[i]->cmdstr, cmd))
            continue;

        /* check if the mode is compatible */
        if (d->cmds[i]->mode != MODE_ANY && d->cmds[i]->mode != d->mode)
            continue;

        if (d->cmds[i]->cmdfunc(d, s) < 0) {
            fprintf(stderr, "command failed\n");
            free(cmd);
            return -1;
        }

        free(cmd);
        return 0;
    }

    fprintf(stderr, "command not found\n");
    free(cmd);
    return -1;
}

int add_cmd(struct interactive_data *d, struct command_data *cmd)
{
    struct command_data **cmds;

    cmds = realloc(d->cmds, sizeof(*cmds) * (d->cmds_cnt + 1));
    if (!cmds) {
        fprintf(stderr, "failed to allocate memory for command\n");
        return -1;
    }

    d->cmds = cmds;
    d->cmds[d->cmds_cnt] = cmd;
    d->cmds_cnt++;

    return 0;
}

/*************************************************
 * Main loop                                     *
 *************************************************/

static void print_info(struct interactive_data *d)
{
    uint32_t ap = 0, tp = 0;

    printf("Remote server: %s\n", d->issim ? "simulator" : "hardware");

    if (d->issim) {
        nfp_sal_clock_info(d->h_nfp, NFP_SAL_CLOCK_TYPE_TCLK, &ap, &tp);
        printf("T_CLK is running at %d (nominal %d)\n",
                1000000 / ap, 1000000 / tp);
        nfp_sal_clock_info(d->h_nfp, NFP_SAL_CLOCK_TYPE_PCLK, &ap, &tp);
        printf("P_CLK is running at %d (nominal %d)\n",
                1000000 / ap, 1000000 / tp);
        nfp_sal_clock_info(d->h_nfp, NFP_SAL_CLOCK_TYPE_ILA0CLK, &ap, &tp);
        printf("ILA0_CLK is running at %d (nominal %d)\n",
               1000000 / ap, 1000000 / tp);
        printf("Simulator version: %s\n",
               nfp_hwinfo_lookup(d->h_nfp, "sim.version"));
    }
}

static const char * get_prompt(struct interactive_data *d)
{
    return d->mode == MODE_SIM ? PROMPTSTR : HWDBGPROMPTSTR;
}

static char *grab_stdin(struct interactive_data *d)
{
    char *buf;
#ifdef USE_READLINE
    buf = readline(get_prompt(d));
    if (buf && *buf)
        add_history(buf);
#else
    buf = fgets(d->readline_buffer, sizeof(d->readline_buffer), stdin);
    if (!buf)
        fprintf(stderr, "failed to read stdin string\n");
    fflush(stdout);
#endif
    return buf;
}

static char *grab_scriptfile(struct interactive_data *d)
{
    char *buf;

    buf = fgets(d->readline_buffer, sizeof(d->readline_buffer), d->scriptfile);
    if (!buf) /* EOF or read error */
        return NULL;

    return buf;
}

#ifndef USE_READLINE
/* wait for activity on a fd */
int select_fd(int fd, int timeoutms)
{
    fd_set rfds;
    struct timeval tv;
    int max_fd;
    int ret;

    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);
    max_fd = fd;

    tv.tv_sec = timeoutms / 1000;
    tv.tv_usec = (timeoutms % 1000) * 1000;

    ret = select(max_fd + 1, &rfds, NULL, NULL, &tv);

    if (ret < 0) {
        if (errno != EINTR) {
            fprintf(stderr, "select failed (%s)\n", strerror(errno));
            return ret;
        }
        return 0;
    }

    if (!ret)
        return 0;

    return 1;
}
#endif

/* periodic house keeping */
static void house_keeping(struct interactive_data *d)
{
    if (d->issim) {
        if (d->bp_inst_step_loaded && !d->bp_inst_step_busy) {
            nfp_sal_watch_del(d->h_nfp, INST_STEP_BREAKID);
            d->bp_inst_step_loaded = 0;
        }
    }
}

int main(int argc, char **argv)
{
    struct interactive_data data;
    char *cmd;
#ifndef USE_READLINE
    int ret;
    int prompt = 1;
#endif
    int exitcode = EXIT_SUCCESS;
    int i;

    interactive_glob = &data;

    /* register signals for clean shutdown */
    signal(SIGINT, shutdown_handler);
    signal(SIGHUP, shutdown_handler);
    signal(SIGTERM, shutdown_handler);

    memset(&data, '\0', sizeof(data));

    handle_args(&data, argc, argv);

    if (do_init(&data) < 0)
        exit(EXIT_FAILURE);

    /* register global commands */
    for (i = 0; global_cmds[i].cmdstr; i++) {
        if (add_cmd(&data, &global_cmds[i]) < 0)
            exit(EXIT_FAILURE);
    }
    /* register client commands */
    for (i = 0; client_cmds[i].cmdstr; i++) {
        if (add_cmd(&data, &client_cmds[i]) < 0)
            exit(EXIT_FAILURE);
    }
    /* register hwdbg commands */
    for (i = 0; hwdbg_cmds[i].cmdstr; i++) {
        if (add_cmd(&data, &hwdbg_cmds[i]) < 0)
            exit(EXIT_FAILURE);
    }
#ifdef EXTERNALCMDS
    /* register optional link time external commands */
    for (i = 0; external_cmds[i].cmdstr; i++) {
        if (add_cmd(&data, &external_cmds[i]) < 0)
            exit(EXIT_FAILURE);
    }
#endif

    print_info(&data);

    while (!data.shutdown) {
        house_keeping(&data);

        /* TODO - use select on stdin or scriptfile */
        if (data.scriptfile) {
            cmd = grab_scriptfile(&data);
            if (!cmd) {
                fclose(data.scriptfile);
                data.scriptfile = NULL;
            }
        } else {
#ifndef USE_READLINE
            if (prompt){
                printf("%s", get_prompt(&data));
                fflush(stdout);
            }
            prompt = 0;
            ret = select_fd(STDIN_FILENO, 100);
            if (ret < 0) {
                fprintf(stderr, "select failed (%s)", strerror(errno));
                exitcode = EXIT_FAILURE;
                break;
            }
            if (ret == 0) /* no activity */
                continue;
            cmd = grab_stdin(&data);
            prompt = 1;
#else
            cmd = grab_stdin(&data);
#endif
        }

        if (!cmd && data.opt_scriptfile) /* file done */
            break;

        if (!cmd || *cmd == '\0') /* no string to handle */
            continue;

        if (handle_cmd(&data, cmd) < 0) {
            if (data.opt_scriptfile) {
                exitcode = EXIT_FAILURE;
                break;
            }
        }
    }

    do_free(&data);

    return exitcode;
}
