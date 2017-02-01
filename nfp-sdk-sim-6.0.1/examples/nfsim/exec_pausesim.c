/**
 * Copyright (C) 2014 Netronome Systems, Inc.  All rights reserved.
 *
 * A tool that runs a given command with the SDK simulator
 * paused while the command runs
 *
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <nfp.h>
#include <nfp_sal.h>

#include <sys/types.h>

#ifndef _WIN32
#include <unistd.h>
#include <sys/wait.h>
#endif

int add_pause(struct nfp_device *nfp)
{
    int wt;
    int ret;
    wt = NFP_SAL_WATCH_WTYPE(NFP_SAL_WATCH_ACTION_PAUSE,
                             NFP_SAL_WATCH_TYPE_VALUE);

    ret = nfp_sal_watch_add_cycle(nfp, wt, 0, NFP_SAL_WATCH_CYCLE_RELATIVE, 1);
    if (ret < 0) {
        fprintf(stderr, "failed to add watch (%s)\n", strerror(errno));
        return -1;
    }

    return 0;
}

void exit_usage(const char *program, int error)
{
    fprintf(error ? stderr : stdout,
            "Usage: %s [options] PROGRAM [ARGUMENTS]\n"
            "Where options are:\n"
            "       -n <device>    -- NFP device\n"
            "       -h             -- print help\n"
            "Execute a given PROGRAM with optional ARGUMENTS with the\n"
            "simulator paused. This tool is useful for reading or writing\n"
            "state on a running simulator, which would otherwise be slow.\n"
            "Note that no clock cycles will pass while PROGRAM is running.\n"
            "It is not recommended that PROGRAM affect simulator run\n"
            "controls, as this may lead to PROGRAM hanging.\n",
            program);
    exit(error ? EXIT_FAILURE : EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
    struct nfp_device *nfp;
    int opt, opt_nfp = 0;
#ifndef _WIN32
    int pid, status;
#endif

    while (1) {
        opt = getopt(argc, argv, "+hn:");
        if (opt == -1)
            break;

        switch (opt) {
        case 'h':
            exit_usage(argv[0], 0);
            break;
        case 'n':
            opt_nfp = atoi(optarg);
            break;
        default:
            exit_usage(argv[0], 1);
        }
    }

    if (argc - optind < 1)
        exit_usage(argv[0], 1);

    nfp = nfp_device_open(opt_nfp);
    if (!nfp) {
        fprintf(stderr, "error: nfp_device_open failed (%s)\n",
                strerror(errno));
        return EXIT_FAILURE;
    }
    if (add_pause(nfp) < 0)
        return EXIT_FAILURE;

#ifdef _WIN32
    /* On windows just exec the tool and let the OS clean up */

    /* TODO: implement something equivalent to fork waitpid on windows */
    if (execv(argv[optind], argv + optind) < 0) {
        fprintf(stderr, "failed to exec (%s)\n", strerror(errno));
        return EXIT_FAILURE;
    }
    /* not reached */
    return EXIT_SUCCESS;
#else
    /* On linux do things properly and fork a process for the tool,
     * wait for the tool to exit, close the nfp handle and then
     * report the tool exit code
     */
    pid = fork();
    if (pid < 0) {
        fprintf(stderr, "failed to fork (%s)\n", strerror(errno));
        return EXIT_FAILURE;
    }

    if (pid == 0) { /* child */
        if (execv(argv[optind], argv + optind) < 0) {
            fprintf(stderr, "failed to exec (%s)\n", strerror(errno));
            return EXIT_FAILURE;
        }
        /* not reached */
        return EXIT_SUCCESS;
    }

    if (pid > 0) { /* parent */
        /* wait for child to close */
        if (waitpid(pid, &status, 0) < 0) {
            fprintf(stderr, "failed to wait for child process (%s)",
                    strerror(errno));
            return EXIT_FAILURE;
        }
    }

    /* close the nfp handle, this will free the watch automatically */
    nfp_device_close(nfp);

    /* report child return code */
    if (WIFEXITED(status)) /* child exited normally */
        return WEXITSTATUS(status);

    return EXIT_FAILURE;
#endif
}

