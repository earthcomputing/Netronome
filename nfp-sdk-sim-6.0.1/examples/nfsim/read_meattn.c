/**
 * Copyright (C) 2013 Netronome Systems, Inc.  All rights reserved.
 *
 * Example that reads ME ATTN using the NFP APIs
 *
 */

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>

#include <nfp-common/nfp_resid.h>
#include <nfp-common/ns_log.h>
#include <nfp.h>
#include <nfp_cpp.h>
#include <nfp_sal.h>

/***** Defines *****/

#define DEFAULT_MEID NFP6000_MEID(32, 0)

/* CPP defines */
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
    const char *meid_str;       /* -m <MEID name> */
    int nfp_debug_level;        /* -d <DEBUG LEVEL> */
    int devnum;                 /* -n <NFP Device number> */
};

/* Define command line arguments */
static const char *optstring = "n:m:d:vh?";
struct globalargs_data globalargs;

/**
 * Display program usage, and exit.
 *
 * @param program       program name
 * @param err           error number (0 = EXIT_SUCCESS, 1 = EXIT_FAILURE)
 */
void usage(const char *program, int err)
{
    fprintf(stderr,
            "Usage: %s [options]    Read ME ATTN\n"
            "\nWhere options are:\n"
            "  -n <dev no>          Device number (default: 0)\n"
            "  -m <ME name/number>  ME Name/number (default: mei0.me0)\n"
            "  -d <debug level>     Debug Level\n"
            "  -v                   Verbose output\n"
            "  -h, -?               Help\n",
            program);

    exit(err ? EXIT_FAILURE : EXIT_SUCCESS);
}


int cpp_xpb_read(struct nfp_cpp *nfp_cpp, uint32_t tgt, uint32_t *val)
{
    int retval;
    retval = nfp_cpp_read(nfp_cpp,
                          CPP_XPBM_ID(1),
                          CPP_XPBM_ADDR(tgt),
                         (void *)(val), 4);
    return retval;
}

int read_meattn(struct nfp_device *nfp, struct nfp_cpp *nfp_cpp, int meid)
{
    uint32_t value;
    uint32_t meid_attn_mask;
    uint64_t cls_im_status_offset;
    int retval;

    /* Calculate mask for ME ATTN signal in the
     * ClusterScratch InterruptManager Status register
     */
    meid_attn_mask = 1 << (NFP6000_MEID_MENUM_of(meid) + 16);
    /* Calculate offset of ClusterScratch InterruptManager Status register */
    cls_im_status_offset = (NFP6000_MEID_ISLAND_of(meid) << 24) | 0x000b0000;

    /* recover the ME ATTN state */
    retval = cpp_xpb_read(nfp_cpp, cls_im_status_offset, &value);
    if (retval < 0)
        return retval;

    if ((value & meid_attn_mask) == meid_attn_mask)
        return 1;

    return 0;
}

int main(int argc, char **argv)
{
    struct nfp_device *nfp;
    struct nfp_cpp *nfp_cpp;
    const char *program = argv[0];
    char meid_str[NFP_MEID_STR_SZ];
    const char *meid_str_ptr;
    int meid;
    int retval;
    int opt = 0;
    int devnum;

    /* Initialise globalargs to defaults */
    globalargs.meid_str = NULL;
    globalargs.verbose = 0;
    globalargs.nfp_debug_level = NS_LOG_LVL_INFO;
    globalargs.devnum = 0;

    /* Initialise variables */
    meid = DEFAULT_MEID;

    /* Process the arguments with getopt(), then populate globalargs */
    opt = getopt(argc, argv, optstring);

    while (opt != -1) {
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
            default:
                break;
        }
        opt = getopt(argc, argv, optstring);
    }

    devnum = globalargs.devnum;

    /* Work out the MEID to use */
    if (globalargs.meid_str != NULL) {
        if ((globalargs.meid_str[0] == '0') &&
                    (globalargs.meid_str[1] == 'x')) {
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
    meid_str_ptr = nfp6000_meid2str(meid_str, meid);
    if (meid_str_ptr == NULL)
        sprintf(meid_str, "%04x", meid);

    /* Set NS log level */
    NS_LOG_SET_LEVEL(globalargs.nfp_debug_level);
    /* Open NFP Device */
    if (globalargs.verbose)
        printf("opening nfp_device (%d)\n", devnum);

    nfp = nfp_device_open(devnum);
    if (!nfp) {
        fprintf(stderr, "%s: ERROR: nfp_device_open (%d) failed\n",
                program, devnum);
        return -1;
    }

    nfp_cpp = nfp_cpp_from_device_id(devnum);
    if (!nfp_cpp) {
        fprintf(stderr, "%s: ERROR: nfp_cpp_from_device_id (%d) failed\n",
                program, devnum);
        return -1;
    }

    /* Set CPP Access Mode */
    nfp_sal_cpp_mode(nfp_cpp, NFP_SAL_CPP_MODE_BACKONLY);

    retval = read_meattn(nfp, nfp_cpp, meid);
    if (retval < 0) {
        fprintf(stderr, "failed to get meattn\n");
        return -1;
    }
    printf("%s:%d\n", meid_str_ptr, retval);

    /* Close/Free NFP handles */
    nfp_cpp_free(nfp_cpp);
    nfp_device_close(nfp);

    return 0;
}
