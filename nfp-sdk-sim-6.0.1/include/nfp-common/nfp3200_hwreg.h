/*
 * Copyright (C) 2010,  Netronome Systems, Inc.  All rights reserved.
 *
 * Disclaimer: this file is provided without any warranty
 * as part of an early-access program, and the content is
 * bound to change before the final release.
 */

#ifndef __NFP3200_HWREG_H__
#define __NFP3200_HWREG_H__

#include <nfp-common/nfp3200_csrs.h>

#include "_nfp_common_apiex.h"

#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable:4245)
#pragma warning (disable:4200)
#endif

#define NFP_CSR_NULLFIELD { 0, 0, 0, NULL }

struct nfp_csr_regtype {
    const char *name;
    struct nfp_csr_bitfield bf[];
};

enum nfp_csr_maptype {
    NFP_CSRMAP_NONE = 0,
    NFP_CSRMAP_REG,
    NFP_CSRMAP_MAP,
};

struct nfp_csr_mapentry {
    const enum nfp_csr_maptype type;
    const unsigned long offset;
    const char *name;
    const char *altname;
    union {
        struct nfp_csr_regtype *reg;
        struct nfp_csr_mapentry *map;
    } ptr;
};

#define NFP_CSR_NULLMAP { 0, 0, NULL }

#ifndef __maybe_unused
    #if defined(_MSC_VER)
        #define __maybe_unused
    #else
        #define __maybe_unused  __attribute__((unused))
    #endif
#endif

#ifdef _MSC_VER
#pragma warning (pop)
#endif

#endif /* __NFP_HWREG_H__ */

