/**
 * Copyright (C) 2013 Netronome Systems, Inc.  All rights reserved.
 *
 * Example code usage to set and get simulation server log level
 *
 */

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <nfp-common/ns_log.h>
#include <nfp.h>
#include <nfp_cpp.h>
#include <nfp_sal.h>

#include "nfsim_logmodule_names.h"

const char * ns_log_lvl_descrptn[] = {
    "none",
    "fatal",
    "error",
    "warn",
    "info",
    "debug",
    "extra",
    "heavy",
    "verbose"
};

void usage(char *argv[])
{
    int i;
    const char *name;
    int num;

    fprintf(stdout, "Usage: %s --get [LOGMODULE] \n", argv[0]);
    fprintf(stdout, "       %s --set [LOGMODULE] LOGLEVEL\n", argv[0]);
    fprintf(stdout, "Valid levels are:\n");
    for(i=0; i<NS_LOG_LVL_COUNT; i++) {
        fprintf(stdout, "\t%d : %s\n", i, ns_log_lvl_descrptn[i]);
    }
    /* Display LOG Modules */
    fprintf(stdout, "\nValid log modules are:\n");
    i = 0;
    do {
        name = all_nfsim_logmodule_names[i].name;
        num = all_nfsim_logmodule_names[i].num;
        if (name != NULL) {
            fprintf(stdout,"\t%d : %s\n", num, name);
        }
        i++;
    } while (name != NULL);
    exit(1);
}

int main(int argc, char **argv)
{
    struct nfp_device *nfp;
    char *lvlstr;
    int lvl = NS_LOG_LVL_INFO;
    int logmodule = 0;
    int set = -1;
    int retval;

    NS_LOG_SET_LEVEL(lvl);

    if (argc == 1) {
        usage(argv);
    }

    if (argc >= 2) {
        if (!strcmp(argv[1], "--get"))
            set = 0;
        if (!strcmp(argv[1], "--set"))
            set = 1;
        if (set < 0)
            usage(argv);
        if (!set) {
            if (argc > 3)
                usage(argv);
            if (argc == 3)
                logmodule = atoi(argv[2]);
        } else {
            if (argc > 4 || argc == 2)
                usage(argv);
            if (argc == 4) {
                logmodule = atoi(argv[2]);
                lvlstr = argv[3];
            } else
                lvlstr = argv[2];

            /* support numbers or strings */
            if (isdigit(lvlstr[0])) {
                lvl = atoi(lvlstr);
            } else {
                int i;
                lvl = NS_LOG_LVL_COUNT;
                for (i = 0; i < sizeof(ns_log_lvl_descrptn) / sizeof(ns_log_lvl_descrptn[0]); i++) {
                    if (!strcmp(lvlstr, ns_log_lvl_descrptn[i]))
                        lvl = i;
                }
            }

            if (lvl >= NS_LOG_LVL_COUNT) {
                fprintf(stderr, "error: invalid log level (must be less than %d)\n", NS_LOG_LVL_COUNT);
                usage(argv);
            }
        }
    }

    nfp = nfp_device_open(0);
    if (!nfp) {
        fprintf(stderr, "error: nfp_device_open failed\n");
        return -1;
    }

    if (set) {
        nfp_sal_set_log_level(nfp, lvl, logmodule);
        if (logmodule)
            printf("Log level for module %d set to %d (%s)\n",
                   logmodule, lvl, ns_log_lvl_descrptn[lvl]);
        else
            printf("Log level set to %d (%s)\n", lvl, ns_log_lvl_descrptn[lvl]);
    } else {
        retval = nfp_sal_get_log_level(nfp, logmodule);
        printf("Log level is %d (%s)\n", retval, ns_log_lvl_descrptn[retval]);
    }

    nfp_device_close(nfp);

    return 0;
}
