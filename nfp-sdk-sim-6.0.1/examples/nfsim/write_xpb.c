/**
 * Copyright (C) 2013 Netronome Systems, Inc.  All rights reserved.
 *
 * Example IPC client which uses the NFHAL/NFSAL APIs
 *
 * @file          write_xpb.c
 * @brief         Example code to write to XPB registers
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

#include "xpbcsr_regdesc_addr_map.h"

/***** Defines *****/

#define MAX_STR_LENGTH          128

#define ISL_ARM 1
#define ISL_MEI0 32
#define CPP_TGT_NBI 1
#define CPP_TGT_MEM 7
#define CPP_TGT_CT  14
#define CPP_TGT_CLS 15

#define CPP_XPBM_ID(rnw) \
        NFP_CPP_ISLAND_ID(CPP_TGT_CT, (rnw) ? 0 : 1, 0, (ISL_ARM))
#define CPP_XPBM_ADDR(x) ((x) | (1 << 30))

#define CPP_XPB_ID(isl, rnw) \
        NFP_CPP_ISLAND_ID(CPP_TGT_CT, (rnw) ? 0 : 1, 0, isl)
#define CPP_XPB_ADDR(x) ((x) | (0 << 30))

struct globalargs_data {
    int verbose;                /* -v verbose */
    int nfp_debug_level;        /* -d <DEBUG LEVEL> */
    int dev_no;                 /* -n <NFP Device number> */
    int xpb_list;               /* -l list all XPB registers and display address */
    int frontdoor;              /* -f frontdoor cpp access mode */
    int backfirst;              /* -b try backdoor cpp first, then try cpp frontdoor */
    int backonly;               /* -c backdoor cpp access mode */
    char **input;
    int numinput;
};

/***** Globals *****/

static const char *optstring = "h?vd:fbcn:l";

struct globalargs_data globalargs;

/* */

/**
 * Check if string is isxdigit (0-9, A-F, a-f) inc x/X
 *
 * @param p     Pointer to string (asciiz)
 *
 * @return number of xdigits on success, or -1 on failure
 */
int isstrxdigit(char *p) {
    char *ptr = p;
    char c;

    int retval = -1;
    do {
        c = *ptr++;
        if (c == '\0') break;
        if (isxdigit(c) || (c == 'x') || (c == 'X')) {
            retval++;
        } else {
            retval = -1;
        }
    } while ((c != '\0') && (retval != -1));

    return retval;
}


/**
 * Lookup XPB Address from XPP register name
 *
 *
 * @param p     Pointer to address name (asciiz)
 *
 * @return address on success, or -1 on failure
 */
int lookupxpbaddr(char *p)
{
    int i;

    for (i = 0; i < xpb_csr_addr_map_count; i++) {
        if (strstr(p, xpb_csr_addr_maps[i].name))
            return xpb_csr_addr_maps[i].addr;
    }

    return -1;
}

/**
 * Dump XPB Register names and Addresses
 *
 */
void dump_xpb_registers()
{
    int i;
    for (i = 0; (i < xpb_csr_addr_map_count); i++) {
        fprintf(stdout,
                "0x%08x %s\n",
                xpb_csr_addr_maps[i].addr, xpb_csr_addr_maps[i].name);
    }
}

/**
 * Lookup XPB name from address
 *
 * @param  addr          XPB Register address
 *
 * @return pointer to XPB name, or NULL if name not found
 */
char *lookupxpbname(uint32_t addr)
{
    int i;
    char *retval = NULL;

    for (i = 0; (i < xpb_csr_addr_map_count) && (retval == NULL); i++) {
        if (xpb_csr_addr_maps[i].addr == addr) {
            retval = xpb_csr_addr_maps[i].name;
            break;
        }
    }

    return retval;
}

int cpp_xpb_write(struct nfp_cpp *nfp_cpp, uint32_t tgt, uint32_t val)
{
    int retval;
    retval = nfp_cpp_write(nfp_cpp,
                           CPP_XPBM_ID(0),
                           CPP_XPBM_ADDR(tgt),
                           (void *)(&val), 4);
    return retval;
}


/**
 * Display program usage, and exit.
 *
 * @param argv      main argv pointer
 */
void usage(char *argv[])
{
    fprintf(stderr,
            "Usage: %s [options] <regname>|<regaddr> <data>\n"
            "\nWhere options are:\n"
            "  -n <dev no>       Device number (default:0)\n"
            "  -d <debug level>  Debug Level\n"
            "  -l                List XPB Registers and Address Offsets\n"
            "  -c                Backdoor CPP access mode\n"
            "  -f                Frontdoor CPP access mode\n"
            "  -b                Try Backdoor CPP first, then try Frontdoor CPP access\n"
            "  -v                Verbose output\n"
            "  -h, -?            Help\n"
            "\n"
            "  <regname>         name of XPB register)\n"
            "  <regaddr>         address of XPB register\n"
            "  <data>            32-bit data value to write to register\n"
            "\n"
            , argv[0]);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    struct nfp_device *nfp;
    struct nfp_cpp *nfp_cpp;
    char regname_str[MAX_STR_LENGTH];
    char *regname;
    uint32_t data;
    uint32_t addr;
    int opt = 0;
    int retval;
    int access_mode = -1;
    int tmp;

    /* Initialise globalargs to defaults */
    globalargs.verbose = 0;
    globalargs.nfp_debug_level = NS_LOG_LVL_INFO;
    globalargs.dev_no = 0;
    globalargs.xpb_list = 0;
    globalargs.backonly = 0;
    globalargs.frontdoor = 0;
    globalargs.backfirst = 0;
    globalargs.input = NULL;
    globalargs.numinput = 0;

    /* Process the arguments with getopt(), then populate globalargs */
    while (1) {
        opt = getopt(argc, argv, optstring);
        if (opt == -1)
            break;

        switch (opt) {
            case 'd':
                globalargs.nfp_debug_level = atoi(optarg);
                break;
            case 'l':
                globalargs.xpb_list = 1;
                break;
            case 'f':
                globalargs.frontdoor = 1;
                break;
            case 'c':
                globalargs.backonly = 1;
                break;
            case 'b':
                globalargs.backfirst = 1;
                break;
            case 'v':
                globalargs.verbose++;
                break;
            case 'n':
                globalargs.dev_no = atoi(optarg);
                break;
            case 'h':
            case '?':
                usage(argv);
                break;
        }
    }

    globalargs.input = argv + optind;
    globalargs.numinput = argc - optind;

    if ((globalargs.numinput == 0) && (globalargs.xpb_list == 0)) {
        usage(argv);
    }

    /* Check to see if we need to display the list of XPB registers */
    if (globalargs.xpb_list) {
        dump_xpb_registers();
        if (globalargs.numinput == 0) {
            exit(EXIT_SUCCESS);
        }
    }

    /* Set NS log level */
    NS_LOG_SET_LEVEL(globalargs.nfp_debug_level);

    /* Open NFP Device */
    if (globalargs.verbose)
        printf("opening nfp_device (%d)\n", globalargs.dev_no);

    nfp = nfp_device_open(globalargs.dev_no);
    if (!nfp) {
        fprintf(stderr, "error: nfp_device_open (%d) failed\n", globalargs.dev_no);
        exit(EXIT_FAILURE);
    }

    /* Open NFP CPP */
    if (globalargs.verbose) {
        printf("opening nfp_cpp (%d)\n", globalargs.dev_no);
    }
    nfp_cpp = nfp_cpp_from_device_id(globalargs.dev_no);
    if (!nfp_cpp) {
        fprintf(stderr, "error: nfp_cpp_from_device_id (%d) failed\n", globalargs.dev_no);
        exit(EXIT_FAILURE);
    }

    /* Get the xpb register name */
    regname = globalargs.input[0];

    /* Let's see if the 2nd argument is present */
    /* XXX No error checking done here! */
    if (globalargs.input[1] != NULL) {
        data = strtoull(globalargs.input[1], NULL, 0);
    } else {
        fprintf(stderr, "error: No value specified to write\n");
        exit(EXIT_FAILURE);
    }

    /* Check the regname is all alphanum characters; i.e. a xpb register name */
    /* XXX Just handle this correctly */
    if (isstrxdigit(regname) < 0) {
        addr = lookupxpbaddr(regname);
        if (addr == -1) {
            fprintf(stderr,"error: Unknown XPB name: %s\n", regname);
            exit(EXIT_FAILURE);
        }
    } else {
        addr = strtoull(regname, NULL, 0);
    }

    /* Check for CPP access mode */
    tmp = 0;
    if (globalargs.frontdoor) {
        access_mode = NFP_SAL_CPP_MODE_FRONTONLY;
        tmp += 1;
    }

    if (globalargs.backfirst) {
        access_mode = NFP_SAL_CPP_MODE_BACKFIRST;
        tmp += 1;
    }

    if (globalargs.backonly) {
        access_mode = NFP_SAL_CPP_MODE_BACKONLY;
        tmp += 1;
    }

    if (tmp > 1) {
        fprintf(stdout, "Conflicting CPP access mode options");
        usage(argv);
    } else if (tmp == 1) {
        retval = nfp_sal_cpp_mode(nfp_cpp, access_mode);
        if (retval < 0) {
            fprintf(stdout, "Unable to set acess mode:%d\n", access_mode);
            usage(argv);
        }
    }

    switch (access_mode) {
        case NFP_SAL_CPP_MODE_FRONTONLY:
            if (globalargs.verbose) {
                printf("Frontdoor CPP Access Mode\n");
            }
            break;
        case NFP_SAL_CPP_MODE_BACKONLY:
            if (globalargs.verbose) {
                printf("Backdoor CPP Access Mode\n");
            }
            break;
        case NFP_SAL_CPP_MODE_BACKFIRST:
            if (globalargs.verbose) {
                printf("Try Backdoor CPP Access Mode, then Try Frontdoor\n");
            }
            break;
    }

    switch (access_mode) {
        case NFP_SAL_CPP_MODE_FRONTONLY:
        case NFP_SAL_CPP_MODE_BACKONLY:
        case NFP_SAL_CPP_MODE_BACKFIRST:
            retval = cpp_xpb_write(nfp_cpp, addr, data);
            break;
        default:
            retval = nfp_xpb_writel(nfp_cpp, addr, data);
    }

    if (retval < 0) {
        fprintf(stderr, "xpb write to 0x%08x=0x%08x failed, %s\n",
                addr, data, strerror(errno));
        exit(EXIT_FAILURE);
    }
    regname = lookupxpbname(addr);
    if (regname == NULL) {
        strcpy(regname_str, "Register name unknown.");
        regname = regname_str;
    }

    if (globalargs.verbose) {
        fprintf(stdout, "0x%08x = 0x%08x %s\n", addr, data, regname);
    }

    /* Close/Free NFP handles */
    if (globalargs.verbose) {
        printf("closing nfp handles\n");
    }
    nfp_device_close(nfp);
    nfp_cpp_free(nfp_cpp);

    return 0;
}
