/**
 * Copyright (C) 2013 Netronome Systems, Inc.  All rights reserved.
 *
 * Example code to read XPB registers
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
    int verbose;            /* -v verbose */
    int nfp_debug_level;    /* -d <DEBUG LEVEL> */
    int dev_no;             /* -n <NFP Device number> */
    int count;              /* -C <num> number of registers to read */
    int xpb_list;           /* -l list all XPB registers and display address */
    int frontdoor;          /* -f frontdoor cpp access mode */
    int backfirst;          /* -b try backdoor cpp first, then try cpp frontdoor */
    int backonly;           /* -c backdoor cpp access mode */
    int disp_bin;           /* -B display value in binary */
    char **input;
    int numInput;
};

/***** Globals *****/

static const char *optstring = "h?vd:C:fbcn:Bl";

struct globalargs_data globalargs;

int g_access_mode;

/* */

int cpp_xpb_read(struct nfp_cpp *nfp_cpp, uint32_t tgt, uint32_t *val)
{
    int retval;
    retval = nfp_cpp_read(nfp_cpp,
                          CPP_XPBM_ID(1),
                          CPP_XPBM_ADDR(tgt),
                         (void *)(val), 4);
    return retval;
}

int xpb_read(struct nfp_cpp *nfp_cpp, uint32_t tgt, uint32_t *val, int access_mode)
{
    int retval;
    switch (access_mode) {
        case NFP_SAL_CPP_MODE_FRONTONLY:
        case NFP_SAL_CPP_MODE_BACKONLY:
        case NFP_SAL_CPP_MODE_BACKFIRST:
            retval = cpp_xpb_read(nfp_cpp, tgt, val);
            break;
        default:
            retval = nfp_xpb_readl(nfp_cpp, tgt, val);
    }
    return retval;
}

/**
 * Check if string is alphanumeric
 * including '.' and '_' as valid characters
 *
 * @param p     Pointer to string (asciiz)
 *
 * @return 0 on success, or -1 on failure
 */
int isstralnum(char *p) {
    char *ptr = p;
    char c;

    int retval = 0;
    do {
        c = *ptr++;
        if (c == '\0') break;
        if (isalnum(c) || (c == '.') || (c == '_')) {
        } else {
            retval = -1;
        }
    } while ((c != '\0') && (retval == 0));

    return retval;
}

/**
 * Convert a 32-bit value as sequence of binary digits
 *
 * e.g. 0101000 0111000 1111000 00001111
 *
 * @param value   Value to display
 * @param p       Pointer to buffer to contain binary digits (asciiz)
 *
 * @return 0 on sucess else -1
 */
int hex32_bin(uint32_t value, char *p)
{
    char *ptr = p;
    int i;
    uint32_t mask = 0x8000000;

    if (p == NULL) {
        return -1;
    }

    for (i=0; i<32; i++) {
        if ((value & 1<<(31-i))) {
            *ptr++ = '1';
        } else {
            *ptr++ = '0';
        }
        if ((i != 0) && (((i+1) % 8) == 0)) {
            *ptr++ = ' ';
        }
        mask >>= 1;
    }
    *--ptr = 0;

    return 0;
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
    int retval = -1;
    for (i = 0; (i < xpb_csr_addr_map_count) && (retval == -1); i++) {
        if ((strstr(p, xpb_csr_addr_maps[i].name) != NULL) &&
            (strlen(p) == strlen(xpb_csr_addr_maps[i].name))) {
            retval = xpb_csr_addr_maps[i].addr;
        }
    }

    return retval;
}

/**
 * Dump XPB Register names and Addresses
 *
 */
void dump_xpb_registers()
{
    int i;
    for (i = 0; (i < xpb_csr_addr_map_count); i++) {
        fprintf(stdout, "0x%08x %s\n", xpb_csr_addr_maps[i].addr, xpb_csr_addr_maps[i].name);
    }
}

/**
 * Read wildcarded named XPB registers
 *
 *
 * @param wildstr       Wildcard string of XPB register to read (asciiz)
 *
 * @return 0
 */
int readxpbwildname(char *wildstr, struct nfp_cpp *nfp_cpp)
{
    int i;
    int retval = 0;
    uint32_t addr, data;
    char retvalstr[MAX_STR_LENGTH];
    char bin_str[MAX_STR_LENGTH];

    for (i = 0; ((i < xpb_csr_addr_map_count) && (retval == 0)); i++) {
        if (strstr(xpb_csr_addr_maps[i].name, wildstr) != NULL) {
            addr = xpb_csr_addr_maps[i].addr;
            retval = xpb_read(nfp_cpp, addr, (uint32_t *) &data, g_access_mode);
            if (retval < 0) {
                strcpy(retvalstr, "\e[1;31mXPB READ FAILED\e[0m");
            } else {
                strcpy(retvalstr, "");
            }
            /* Now let's get the register value */
            if (globalargs.disp_bin) {
                retval = hex32_bin(data, bin_str);
                if (retval < 0) *bin_str = 0;
                fprintf(stdout, "0x%08x = 0x%08x %s %s\n", addr, data, bin_str, xpb_csr_addr_maps[i].name);
            } else {
                fprintf(stdout, "0x%08x = 0x%08x %s\n", addr, data, xpb_csr_addr_maps[i].name);
            }

            //fprintf(stdout, "0x%08x = 0x%08x %s %s\n", addr, data, , retvalstr);
            retval = 0;
        }
    }

    return retval;
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

/* Display program usage, and exit. */
void usage(char *argv[])
{
    fprintf(stderr,
            "Usage: %s [options] <regname>|<regaddr>\n"
            "\nWhere options are:\n"
            "  -n <dev no>       Device number (default:0)\n"
            "  -d <debug level>  Debug Level\n"
            "  -l                List XPB Registers and Address Offsets\n"
            "  -C <num>          number of registers to read (default:1)\n"
            "  -c                Backdoor CPP access mode\n"
            "  -f                Frontdoor CPP access mode\n"
            "  -b                Try Backdoor CPP first, then try Frontdoor CPP access\n"
            "  -B                Display value in binary\n"
            "  -v                Verbose output\n"
            "  -h, -?            Help\n"
            "\n"
            "  <regname>         name of XPB register (wildcard with *, ignores <count>)\n"
            "  <regaddr>         address of XPB register\n"
            "\n",
            argv[0]);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    struct nfp_device *nfp;
    struct nfp_cpp *nfp_cpp;
    char regname_str[MAX_STR_LENGTH];
    char bin_str[MAX_STR_LENGTH];
    char *regname;
    uint32_t addr, data;
    int opt;
    int regcount;
    int wildcard = 0;
    int retval;
    int tmp;
    int i, c;

    /* Initialise globalargs to defaults */
    globalargs.verbose = 0;
    globalargs.nfp_debug_level = NS_LOG_LVL_INFO;
    globalargs.dev_no = 0;
    globalargs.count = 1;
    globalargs.backonly = 0;
    globalargs.frontdoor = 0;
    globalargs.backfirst = 0;
    globalargs.xpb_list = 0;
    globalargs.disp_bin = 0;
    globalargs.input = NULL;
    globalargs.numInput = 0;

    /* Initialise variables */
    g_access_mode = -1;

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
            case 'B':
                globalargs.disp_bin = 1;
                break;
            case 'v':
                globalargs.verbose++;
                break;
            case 'n':
                globalargs.dev_no = atoi(optarg);
                break;
            case 'C':
                globalargs.count = atoi(optarg);
                break;
            case 'h':
            case '?':
                usage(argv);
                break;
        }
    }

    globalargs.input = argv + optind;
    globalargs.numInput = argc - optind;

    if ((globalargs.numInput == 0) && (globalargs.xpb_list == 0)) {
        usage(argv);
    }

    /* Check to see if we need to display the list of XPB registers */
    if (globalargs.xpb_list == 1) {
        dump_xpb_registers();
        if (globalargs.numInput == 0) {
            exit(EXIT_SUCCESS);
        }
    }

    /* Set NS log level */
    NS_LOG_SET_LEVEL(globalargs.nfp_debug_level);

    /* Open NFP Device */
    if (globalargs.verbose) {
        printf("opening nfp_device (%d)\n", globalargs.dev_no);
    }
    nfp = nfp_device_open(globalargs.dev_no);
    if (!nfp) {
        fprintf(stderr,
                "error: nfp_device_open (%d) failed\n",
                globalargs.dev_no);
        exit(EXIT_FAILURE);
    }

    /* Open NFP CPP */
    if (globalargs.verbose) {
        printf("opening nfp_cpp (%d)\n", globalargs.dev_no);
    }
    nfp_cpp = nfp_cpp_from_device_id(globalargs.dev_no);
    if (!nfp_cpp) {
        fprintf(stderr,
                "error: nfp_cpp_from_device_id (%d) failed\n",
                globalargs.dev_no);
        exit(EXIT_FAILURE);
    }

    /* Check for CPP access mode */
    tmp = 0;
    if (globalargs.frontdoor) {
        g_access_mode = NFP_SAL_CPP_MODE_FRONTONLY;
        tmp += 1;
    }

    if (globalargs.backfirst) {
        g_access_mode = NFP_SAL_CPP_MODE_BACKFIRST;
        tmp += 1;
    }

    if (globalargs.backonly) {
        g_access_mode = NFP_SAL_CPP_MODE_BACKONLY;
        tmp += 1;
    }

    if (tmp > 1) {
        fprintf(stdout, "Conflicting CPP access mode options");
        usage(argv);
    } else if (tmp == 1) {
        retval = nfp_sal_cpp_mode(nfp_cpp, g_access_mode);
        if (retval < 0) {
            fprintf(stdout, "Unable to set acess mode:%d\n", g_access_mode);
            usage(argv);
        }
    }

    switch (g_access_mode) {
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

    for (c = 0; c < globalargs.numInput; c++) {
        /* Get the xpb register name */
        regname = globalargs.input[c];

        regcount = globalargs.count;

        /* Check for wildcard on regname */
        if (strstr(regname, "*") != NULL) {
            regname[strlen(regname) - 1] = '\0';
            wildcard = 1;
        } else {
            /* Handle HEX address/count with arguments */
            /* Check for 0x prefix in arg  */
            if ((regname[0] == '0') && (regname[1] == 'x')) {
                if (strlen(regname) != 10) {
                    fprintf(stderr,"error: invalid address length (%d). "
                                   "(Format: 0xaabbccdd)\n",
                                   (int) strlen(regname));
                    exit(EXIT_FAILURE);
                } else {
                    i = sscanf(regname + 2, "%x", (uint32_t *) &addr);
                }
            /* Check the regname is all alphanum characters; i.e. a xpb register name */
            } else if (isstralnum(regname) == 0) {
                //printf("looking up register %s\n", regname + 1);
                addr = lookupxpbaddr(regname);
                if (addr == -1) {
                    fprintf(stderr,"error: unknown XPB name: %s\n", regname);
                    exit(EXIT_FAILURE);
                }
            }
        }

        /* Search for all registers with wildcard name and display the address/data  */
        if (wildcard == 1) {
            readxpbwildname(regname, nfp_cpp);
        } else {
            /* Display xpb registers */
            for (i = 0; i < regcount; i++) {
                retval = xpb_read(nfp_cpp, addr, (uint32_t *) &data, g_access_mode);
                if (retval < 0) {
                    fprintf(stdout, "0x%08x = %s\n", addr, strerror(errno));
                }
                regname = lookupxpbname(addr);
                if (regname == NULL) {
                    strcpy(regname_str, "Register name unknown.");
                    regname = regname_str;
                }
                /* Now let's get the register value */
                if (globalargs.disp_bin) {
                    retval = hex32_bin(data, bin_str);
                    if (retval < 0) *bin_str = 0;
                    fprintf(stdout, "0x%08x = 0x%08x %s %s\n",
                            addr, data, bin_str, regname);
                } else {
                    fprintf(stdout, "0x%08x = 0x%08x %s\n",
                            addr, data, regname);
                }
                addr += 4; /* XPBs are always 4B */
            }
        }
    }

    /* Close/Free NFP handles */
    if (globalargs.verbose) {
        printf("closing nfp handles\n");
    }
    nfp_device_close(nfp);
    nfp_cpp_free(nfp_cpp);

    return 0;
}
