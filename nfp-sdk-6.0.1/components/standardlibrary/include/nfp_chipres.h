/*
 * Copyright (C) 2014-2015 Netronome Systems, Inc.  All rights reserved.
 *
 * File:        nfp_chipres.h
 */

#ifndef __NFP_CHIPRES_H__
#define __NFP_CHIPRES_H__

#if defined(__NFP_LANG_MICROC)
    #ifndef NFP_LIB_ANY_NFCC_VERSION
        #if ((__NFP_LANG_MICROC < NFP_SW_VERSION(5, 0, 0, 0)) ||   \
            (__NFP_LANG_MICROC > NFP_SW_VERSION(6, 255, 255, 255)))
            #error "This standard library is not supported for the version of the SDK currently in use."
        #endif
    #endif

    #define _NFP_CHIPRES_ASM(x)  __asm { x }
    #define _NFP_CHIPRES_HAS_ISLAND _nfp_has_island
#elif defined(__NFP_LANG_ASM)
    #ifndef NFP_LIB_ANY_NFAS_VERSION
        #if ((__NFP_LANG_ASM < NFP_SW_VERSION(5, 0, 0, 0)) ||   \
            (__NFP_LANG_ASM > NFP_SW_VERSION(6, 255, 255, 255)))
            #error "This standard library is not supported for the version of the SDK currently in use."
        #endif
    #endif

    #define _NFP_CHIPRES_ASM(x) x
    #define _NFP_CHIPRES_HAS_ISLAND __NFP_HAS_ISLAND
#else
    #error "Unknown language"
#endif

/** @file nfp_chipres.h NFP generic resources
 * @addtogroup nfp_chipres NFP generic resources
 * @{
 * @name NFP generic resources
 * @{
 */

#if defined(__NFP_IS_3200)

    /** @name Memory Unit Resources
     *  @{
     *
     * The following resource pools are declared (if the relevant island is
     * available for the target chip):
     * * dram_queues global 1024
     *
     * There are also predefined macros to aid allocation of these
     * resources. Note that using these predefined macros to allocate
     * from the resource pools will produce symbols that cannot be
     * checked with is_rt_const() due to the way the preprocessor
     * works.
     *
     */
    _NFP_CHIPRES_ASM(.declare_resource dram_queues global 1024)

    /** Allocate a queue array index number from dram_queues.
     * @param _name_    Name for the allocated resource symbol
     * @param _scope_   Scope with which to allocate the resource symbol
     */
    #define DRAM_QUEUE_ALLOC(_name_, _scope_) \
        _NFP_CHIPRES_ASM(.alloc_resource _name_ dram_queues _scope_ 1)

    /** @}
     */

#elif defined(__NFP_IS_6000)

    /** @name External Memory Unit Resources
     *  @{
     *
     * The following resource pools are declared (if the relevant island is
     * available for the target chip):
     * * emem0_queues global 1024
     * * emem1_queues global 1024
     * * emem2_queues global 1024
     *
     * There are also predefined macros to aid allocation of these
     * resources. Note that using these predefined macros to allocate
     * from the resource pools will produce symbols that cannot be
     * checked with is_rt_const() due to the way the preprocessor
     * works.
     *
     */

    /** Allocate a queue array index number from an EMEM queue pool.
     * @param _name_    Name for the allocated resource symbol
     * @param _scope_   Scope with which to allocate the resource symbol
     * @param _resourcepool_ Resource pool from which to allocate.
     */
    #define EMEM_QUEUE_ALLOC(_name_, _scope_, _resourcepool_) \
        _NFP_CHIPRES_ASM(.alloc_resource _name_ _resourcepool_ _scope_ 1)

    #if _NFP_CHIPRES_HAS_ISLAND("emem0")
        _NFP_CHIPRES_ASM(.declare_resource emem0_queues global 1024)

        /** Allocate a queue array index number from emem0_queues.
         * @param _name_    Name for the allocated resource symbol
         * @param _scope_   Scope with which to allocate the resource symbol
         */
        #define EMEM0_QUEUE_ALLOC(_name_, _scope_) \
            EMEM_QUEUE_ALLOC(_name_, _scope_, emem0_queues)
    #endif

    #if _NFP_CHIPRES_HAS_ISLAND("emem1")
        _NFP_CHIPRES_ASM(.declare_resource emem1_queues global 1024)

        /** Allocate a queue array index number from emem1_queues.
         * @param _name_    Name for the allocated resource symbol
         * @param _scope_   Scope with which to allocate the resource symbol
         */
        #define EMEM1_QUEUE_ALLOC(_name_, _scope_) \
            EMEM_QUEUE_ALLOC(_name_, _scope_, emem1_queues)
    #endif

    #if _NFP_CHIPRES_HAS_ISLAND("emem2")
        _NFP_CHIPRES_ASM(.declare_resource emem2_queues global 1024)

        /** Allocate a queue array index number from emem2_queues.
         * @param _name_    Name for the allocated resource symbol
         * @param _scope_   Scope with which to allocate the resource symbol
         */
        #define EMEM2_QUEUE_ALLOC(_name_, _scope_) \
            EMEM_QUEUE_ALLOC(_name_, _scope_, emem2_queues)
    #endif

    /** @}
     */

#else
    #error "Unknown chip target"
#endif

/** @name CLS Resources
 *  @{
 * The following resource pools are declared:
 * * cls_rings island 16
 * * cls_apsignals island (NFP-32xx: 8, other: 16)
 * * cls_apfilters island 16
 *
 * There are also predefined macros to aid allocation of these
 * resources. Note that using these predefined macros to allocate
 * from the resource pools will produce symbols that cannot be
 * checked with is_rt_const() due to the way the preprocessor
 * works.
 */
_NFP_CHIPRES_ASM(.declare_resource cls_rings island 16)

/** Allocate a ring number from cls_rings.
 * @param _name_    Name for the allocated resource symbol
 * @param _scope_   Scope with which to allocate the resource symbol
 */
#define CLS_RING_ALLOC(_name_, _scope_) \
    _NFP_CHIPRES_ASM(.alloc_resource _name_ cls_rings _scope_ 1)


#if defined(__NFP_IS_3200)
    _NFP_CHIPRES_ASM(.declare_resource cls_apsignals island 8)
#else
    _NFP_CHIPRES_ASM(.declare_resource cls_apsignals island 16)
#endif

/** Allocate an autopush signal number from cls_apsignals.
 * @param _name_    Name for the allocated resource symbol
 * @param _scope_   Scope with which to allocate the resource symbol
 */
#define CLS_APSIGNAL_ALLOC(_name_, _scope_) \
    _NFP_CHIPRES_ASM(.alloc_resource _name_ cls_apsignals _scope_ 1)

_NFP_CHIPRES_ASM(.declare_resource cls_apfilters island 16)

/** Allocate an autopush filter status monitor number from cls_apfilters.
 * @param _name_    Name for the allocated resource symbol
 * @param _scope_   Scope with which to allocate the resource symbol
 */
#define CLS_APFILTER_ALLOC(_name_, _scope_) \
    _NFP_CHIPRES_ASM(.alloc_resource _name_ cls_apfilters _scope_ 1)

/** Allocate an autopush signal filter status monitor number
 * from cls_apsignals and cls_apfilters at the same offset.
 * @param _name_sig_    Name for the allocated signal number
 * @param _name_filter_    Name for the allocated filter number
 * @param _scope_   Scope with which to allocate the resource symbol
 */
#if defined(__NFP_LANG_MICROC)
/*    #define CLS_APSIGFILTER_ALLOC(_name_sig_, _name_filter_, _scope_) \
        _NFP_CHIPRES_ASM(.alloc_resource _name_sig_ cls_apsignals _scope_ 1) \
        _NFP_CHIPRES_ASM(.alloc_resource _name_filter_ cls_apfilters+_name_sig_ _scope_ 1)
*/
    #define CLS_APSIGFILTER_ALLOC(_name_sig_, _name_filter_, _scope_) \
        _NFP_CHIPRES_ASM(Not yet supported)

#elif defined(__NFP_LANG_ASM)
    #macro CLS_APSIGFILTER_ALLOC(_name_sig_, _name_filter_, _scope_)
        .alloc_resource _name_sig_ cls_apsignals _scope_ 1
        .alloc_resource _name_filter_ cls_apfilters+_name_sig_ _scope_ 1
    #endm
#endif

/** @}
 */

/** @}
 * @}
 */

#endif /* __NFP_CHIPRES_H__ */
