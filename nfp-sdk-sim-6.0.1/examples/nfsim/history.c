/**
 * Copyright (C) 2013 Netronome Systems, Inc.  All rights reserved.
 *
 * Example using the NFP SDK Simulator Thread History API
 *
 */

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>

#include <nfp-common/nfp_resid.h>
#include <nfp-common/ns_log.h>
#include <nfp.h>
#include <nfp_cpp.h>
#include <nfp_sal.h>

/*
 *
 */

void usage(void)
{
    printf("usage: --enable window            enable history collection\n"
           "                                  window is the number of cycles\n"
           "                                  history to keep\n"
           "       --disable                  disable history collection\n"
           "       --addisl islno             add all an island's state to\n"
           "                                  history collection\n"
           "       --addme meid (iX.meY)      add all an ME's state to\n"
           "                                  history collection\n"
           "       --addxpb address [mask]    add a range of XPB CSRS to\n"
           "                                  history collection\n"
           "       --mecsr meid csr [ctx]     print history for a ME csr\n"
           "       --mereg meid reg           print history for a ME reg\n"
           "       --cls islno offset length  print CLS history\n"
           "       --ctm islno offset length  print CTM history\n"
           "       --emem islno offset length print EMEM history\n"
           "       --lmem meid offset length  print LMEM history\n"
           "       --cpp id address           print history for a CPP CSR\n"
           "       --xpb address              print history for an XPB CSR\n");
}

int main(int argc, char **argv)
{
    struct nfp_device *nfp;
    int enable = 0, disable = 0, addisl = 0, addme = 0;
    int mecsr = -1, mereg = -1, mectx = -1;
    int emem = -1, cls = -1, ctm = -1, meid = -1, lmem = -1;
    uint64_t  clsoff = 0, clslen = 0, ctmoff = 0, ctmlen = 0;
    uint64_t cppid = 0, cppaddr = 0, addxpb = 0, xpboffset = 0, xpbmask = -1;
    int window = 0;

    NS_LOG_SET_LEVEL(NS_LOG_LVL_INFO);

    if (argc <= 1) {
        usage();
        return 0;
    }

    if (argc == 2) {
        if (!strcmp(argv[1], "--disable")) {
            disable = 1;
        } else {
            usage();
            return 0;
        }
    }
    if (argc == 3) {
        if (!strcmp(argv[1], "--enable")) {
            enable = 1;
            window = strtol(argv[2], NULL, 0);
        } else if (!strcmp(argv[1], "--addisl")) {
            addisl = strtol(argv[2], NULL, 0);
        } else if (!strcmp(argv[1], "--addme")) {
            if ((addme = nfp6000_idstr2meid(argv[2], NULL)) == -1)
               addme = strtol(argv[2], NULL, 0);
            printf("====%x\n", addme);
        } else if (!strcmp(argv[1], "--addxpb")) {
            addxpb = strtoul(argv[2], NULL, 0);
            xpbmask = 0xffffffffffffffffULL;
        } else if (!strcmp(argv[1], "--xpb")) {
            xpboffset = strtol(argv[2], NULL, 0);
        } else {
            usage();
            return 0;
        }
    }
    if (argc == 4) {
        if (!strcmp(argv[1], "--mecsr")) {
            mecsr = strtol(argv[3], NULL, 0);
            if ((meid = nfp6000_idstr2meid(argv[2], NULL)) == -1)
                meid = strtol(argv[2], NULL, 0);
            printf("====%x\n", meid);
        } else if (!strcmp(argv[1], "--mereg")) {
            mereg = strtol(argv[3], NULL, 0);
            if ((meid = nfp6000_idstr2meid(argv[2], NULL)) == -1)
                meid = strtol(argv[2], NULL, 0);
            printf("====%x\n", meid);
        } else if (!strcmp(argv[1], "--cpp")) {
            cppid = strtol(argv[2], NULL, 0);
            cppaddr = strtol(argv[3], NULL, 0);
        } else if (!strcmp(argv[1], "--addxpb")) {
            addxpb = strtoul(argv[2], NULL, 0);
            xpbmask = strtoul(argv[3], NULL, 0);
        } else {
            usage();
            return 0;
        }
    }

    if (argc == 5) {
        if (!strcmp(argv[1], "--mecsr")) {
            mecsr = strtol(argv[3], NULL, 0);
            if ((meid = nfp6000_idstr2meid(argv[2], NULL)) == -1)
                meid = strtol(argv[2], NULL, 0);
            printf("====%x\n", meid);
            mectx = strtol(argv[4], NULL, 0);
        } else if (!strcmp(argv[1], "--cls")) {
            cls = strtol(argv[2], NULL, 0);
            clsoff = strtol(argv[3], NULL, 0);
            clslen = strtol(argv[4], NULL, 0);
        } else if (!strcmp(argv[1], "--ctm")) {
            ctm = strtol(argv[2], NULL, 0);
            ctmoff = strtol(argv[3], NULL, 0);
            ctmlen = strtol(argv[4], NULL, 0);
        } else if (!strcmp(argv[1], "--emem")) {
            emem = strtol(argv[2], NULL, 0);
            clsoff = strtol(argv[3], NULL, 0);
            clslen = strtol(argv[4], NULL, 0);
        } else if (!strcmp(argv[1], "--lmem")) {
            lmem = 1;
            clsoff = strtol(argv[3], NULL, 0);
            clslen = strtol(argv[4], NULL, 0);
            if ((meid = nfp6000_idstr2meid(argv[2], NULL)) == -1)
                meid = strtol(argv[2], NULL, 0);
            printf("====%x\n", meid);
        } else {
            usage();
            return 0;
        }
    }

    nfp = nfp_device_open(0);
    if (!nfp) {
        fprintf(stderr, "error: nfp_device_open failed\n");
        return -1;
    }

    if (enable)
        nfp_sal_thistory_enable(nfp, window);
    if (disable)
        nfp_sal_thistory_disable(nfp);
    if (addisl > 0)
        nfp_sal_thistory_add_isl(nfp, addisl);
    if (addme > 0)
        nfp_sal_thistory_add_me(nfp, addme);
    if (addxpb > 0)
        nfp_sal_thistory_add_xpb(nfp, addxpb, xpbmask);

    if (cppid > 0) {
        uint64_t buf[1024*2];
        uint32_t count;
        int i;
        int ret;
        /* get all history */
        ret = nfp_sal_thistory_get_cpp(nfp, 0, 0,
                                         buf, 1024*2*sizeof(uint64_t),
                                         &count, cppid, cppaddr);
        if (ret < 0) {
            printf("failed to get cpp history: %s\n", strerror(errno));
            count = 0;
        }
        for (i = 0; i < count; i++)
            printf("cycle %8ld: 0x%08"PRIx64"\n", buf[i*2], buf[i*2+1]);
    }

    if (xpboffset > 0) {
        uint64_t buf[1024*2];
        uint32_t count;
        int i;
        int ret;
        /* get all history */
        ret = nfp_sal_thistory_get_xpb(nfp, 0, 0,
                                         buf, 1024*2*sizeof(uint64_t),
                                         &count, xpboffset);
        if (ret < 0) {
            printf("failed to get xpb history: %s\n", strerror(errno));
            count = 0;
        }
        for (i = 0; i < count; i++)
            printf("cycle %8ld: 0x%08"PRIx64"\n", buf[i*2], buf[i*2+1]);
    }

    if (lmem > 0) {
        uint64_t buf[1024*3];
        uint32_t count;
        int i;
        int ret;
        /* get all history */
        ret = nfp_sal_thistory_get_lmem(nfp, 0, 0,
                                        buf, 1024*3*sizeof(uint64_t),
                                        &count,
                                        meid, clsoff, clslen);
        if (ret < 0) {
            printf("failed to get lmem history: %s\n", strerror(errno));
            count = 0;
        }
        for (i = 0; i < count; i++)
            printf("cycle %8ld: 0x%08"PRIx64" = 0x%016"PRIx64"\n",
                            buf[i*3], buf[i*3+2], buf[i*3+1]);
    }

    if (emem > 0) {
        uint64_t buf[1024*3];
        uint32_t count;
        int i;
        int ret;
        /* get all history */
        ret = nfp_sal_thistory_get_emem(nfp, 0, 0,
                                        buf, 1024*3*sizeof(uint64_t),
                                        &count,
                                        emem, clsoff, clslen);
        if (ret < 0) {
            printf("failed to get emem history: %s\n", strerror(errno));
            count = 0;
        }
        for (i = 0; i < count; i++)
            printf("cycle %8ld: 0x%08"PRIx64" = 0x%016"PRIx64"\n",
                            buf[i*3], buf[i*3+2], buf[i*3+1]);
    }

    if (cls > 0) {
        uint64_t buf[1024*3];
        uint32_t count;
        int i;
        int ret;
        /* get all history */
        ret = nfp_sal_thistory_get_cls(nfp, 0, 0,
                                        buf, 1024*3*sizeof(uint64_t),
                                        &count,
                                        cls, clsoff, clslen);
        if (ret < 0) {
            printf("failed to get cls history: %s\n", strerror(errno));
            count = 0;
        }
        for (i = 0; i < count; i++)
            printf("cycle %8ld: 0x%08"PRIx64" = 0x%016"PRIx64"\n",
                            buf[i*3], buf[i*3+2], buf[i*3+1]);
    }

    if (ctm > 0) {
        uint64_t buf[1024*3];
        uint32_t count;
        int i;
        int ret;
        /* get all history */
        ret = nfp_sal_thistory_get_ctm(nfp, 0, 0,
                                        buf, 1024*3*sizeof(uint64_t),
                                        &count,
                                        ctm, ctmoff, ctmlen);
        if (ret < 0) {
            printf("failed to get ctm history: %s\n", strerror(errno));
            count = 0;
        }
        for (i = 0; i < count; i++)
            printf("cycle %8ld: 0x%08"PRIx64" = 0x%016"PRIx64"\n",
                            buf[i*3], buf[i*3+2], buf[i*3+1]);
    }

    if (mecsr >= 0) {
        uint64_t buf[1024*2];
        uint32_t count;
        int i;
        int ret;
        /* get all history */
        ret = nfp_sal_thistory_get_mecsr(nfp, 0, 0,
                                         buf, 1024*2*sizeof(uint64_t),
                                         &count, meid, mectx, mecsr);
        if (ret < 0) {
            printf("failed to get mecsr history: %s\n", strerror(errno));
            count = 0;
        }
        for (i = 0; i < count; i++)
            printf("cycle %8ld: 0x%08"PRIx64"\n", buf[i*2], buf[i*2+1]);
    }
    if (mereg >= 0) {
        uint64_t buf[1024*2];
        uint32_t count;
        int i;
        int ret;
        /* get all history */
        ret = nfp_sal_thistory_get_me_register(nfp, 0, 0,
                                               buf, 1024*2*sizeof(uint64_t),
                                               &count, meid, mereg);
        if (ret < 0) {
            printf("failed to get mereg history: %s\n", strerror(errno));
            count = 0;
        }
        for (i = 0; i < count; i++)
            printf("cycle %8ld: 0x%08"PRIx64"\n", buf[i*2], buf[i*2+1]);
    }
    nfp_device_close(nfp);

    return 0;
}
