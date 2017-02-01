/*
 * Copyright (C) 2014,  Netronome Systems, Inc.  All rights reserved.
 *
 * @file          _nfp_sal_apiex.h
 *
 */
#ifndef __NFP_SAL_APIEX_H__
#define __NFP_SAL_APIEX_H__

/* Import/Export symbols for MSC and set visibility for GCC */
#if defined(NO_NFP_SAL_API_EXPORT)
    #define NFP_SAL_API
#elif defined(_MSC_VER)
    #ifdef NFP_SAL_API_EXPORT
        #define NFP_SAL_API __declspec(dllexport)
    #else
        #define NFP_SAL_API __declspec(dllimport)
    #endif
#else
    #if __GNUC__ >= 4
        #define NFP_SAL_API __attribute__ ((visibility ("default")))
    #else
        #define NFP_SAL_API
    #endif
#endif

#endif /* __NFP_SAL_APIEX_H__ */
