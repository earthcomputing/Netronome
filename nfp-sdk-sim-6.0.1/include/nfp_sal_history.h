/*
 * Copyright (C) 2012-2013,  Netronome Systems, Inc.  All rights reserved.
 */

#ifndef __NFP_SAL_HISTORY_H__
#define __NFP_SAL_HISTORY_H__

#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/cdefs.h>

#include <nfp.h>

#include "_nfp_sal_apiex.h"


__BEGIN_DECLS;

/** @file nfp_sal_history.h
 *  @addtogroup nfsal NFSAL
 *  @{
 */

/** @name Functions for accessing history collection within the NFP simulator
 *  @{
 */

/**
 * Enable thread history collection
 *
 * @param dev           NFP device
 * @param cycle_window  The window size to keep history
 *
 * @returns 0 on success, or -1 on failure (sets errno accordingly)
 */
NFP_SAL_API
int nfp_sal_thistory_enable(struct nfp_device *dev, int cycle_window);

/**
 * Disable thread history collection
 *
 * @param dev           NFP device
 *
 * @returns 0 on success, or -1 on failure (sets errno accordingly)
 */
NFP_SAL_API
int nfp_sal_thistory_disable(struct nfp_device *dev);

/**
 * Add an island to the thread history collection list
 *
 * @param dev           NFP device
 * @param islno         Island to add
 *
 * @returns Number of entries added to the collection list on success,
 *          or -1 on failure
 */
NFP_SAL_API
int nfp_sal_thistory_add_isl(struct nfp_device *dev, int islno);

/**
 * Add an ME to the thread history collection list
 *
 * @param dev           NFP device
 * @param menum         ME number
 *
 * @returns Number of entries added to the collection list on success,
 *          or -1 on failure
 */
NFP_SAL_API
int nfp_sal_thistory_add_me(struct nfp_device *dev, int menum);

/**
 * Add XPB CSRs to history collection
 *
 * @param dev           NFP device
 * @param xpb_tgt       XPB target
 * @param mask          Mask to match
 *
 * @returns Number of entries added to the collection list on success,
 *          or -1 on failure
 */
NFP_SAL_API
int nfp_sal_thistory_add_xpb(struct nfp_device *dev, uint64_t xpb_tgt, uint64_t mask);

/**
 * Retrieve history for an XPB CSR
 *
 * @param dev           NFP device
 * @param cycle_start   Start cycle
 * @param cycle_end     End cycle or 0 for all cycles
 * @param buffer        Buffer for return data
 * @param length        Length of buffer
 * @param count         Number of history values returned in buffer
 * @param xpb_tgt       XPB target
 *
 * Retrieve register history data; return format is pairs of
 * uint64ts, starting with the cycle followed by the value
 *
 * @returns 0 on success, or -1 on failure (sets errno accordingly)
 *          and, if there was not enough space in @a buffer for all the
 *          available history, the number of words that fit the window.
 *          Note that a @a length of 0 will always return the number of
 *          words within the window
 */
NFP_SAL_API
int nfp_sal_thistory_get_xpb(struct nfp_device *dev, uint64_t cycle_start, uint64_t cycle_end, void *buffer, unsigned long long length, uint32_t *count, uint32_t xpb_tgt);

/**
 * Retrieve history for an ME CSR
 *
 * @param dev           NFP device
 * @param cycle_start   Start cycle
 * @param cycle_end     End cycle or 0 for all cycles
 * @param buffer        Buffer for return data
 * @param length        Length of buffer
 * @param count         Number of history values returned in buffer
 * @param menum         ME number
 * @param ctx           ME context number
 * @param csr           ME CSR address
 *
 * Retrieve register history data for an mecsr; return format is pairs of
 * uint64ts, starting with the cycle followed by the value
 *
 * @returns 0 on success, or -1 on failure (sets errno accordingly)
 *          and, if there was not enough space in @a buffer for all the
 *          available history, the number of words that fit the window.
 *          Note that a @a length of 0 will always return the number of
 *          words within the window
 */
NFP_SAL_API
int nfp_sal_thistory_get_mecsr(struct nfp_device *dev, uint64_t cycle_start, uint64_t cycle_end, void *buffer, unsigned long long length, uint32_t *count, unsigned int menum, int ctx, unsigned int csr);

/**
 * Retrieve history for an ME register
 *
 * @param dev           NFP device
 * @param cycle_start   Start cycle
 * @param cycle_end     End cycle or 0 for all cycles
 * @param buffer        Buffer for return data
 * @param length        Length of buffer
 * @param count         Number of history values returned in buffer
 * @param menum         ME number
 * @param reg           ME register
 *
 * Retrieve register history data for an me register; return format is pairs of
 * uint64ts, starting with the cycle followed by the value;
 * refer to nfp.h for macros to build up the ME register argument
 *
 * @returns 0 on success, or -1 on failure (sets errno accordingly)
 *          and, if there was not enough space in @a buffer for all the
 *          available history, the number of words that fit the window.
 *          Note that a @a length of 0 will always return the number of
 *          words within the window
 */
NFP_SAL_API
int nfp_sal_thistory_get_me_register(struct nfp_device *dev, uint64_t cycle_start, uint64_t cycle_end, void *buffer, unsigned long long length, uint32_t *count, unsigned int menum, unsigned int reg);

/**
 * Retrieve history for a CPP CSR
 *
 * @param dev           NFP device
 * @param cycle_start   Start cycle
 * @param cycle_end     End cycle or 0 for all cycles
 * @param buffer        Buffer for return data
 * @param length        Length of buffer
 * @param count         Number of history values returned in buffer
 * @param id            CPP ID
 * @param address       CPP address
 *
 * Retrieve register history data for an me register; return format is pairs of
 * uint64ts, starting with the cycle followed by the value
 *
 * @returns 0 on success, or -1 on failure (sets errno accordingly)
 *          and, if there was not enough space in @a buffer for all the
 *          available history, the number of words that fit the window.
 *          Note that a @a length of 0 will always return the number of
 *          words within the window
 */
NFP_SAL_API
int nfp_sal_thistory_get_cpp(struct nfp_device *dev, uint64_t cycle_start, uint64_t cycle_end, void *buffer, unsigned long long length, uint32_t *count, uint32_t id, uint64_t address);

/**
 * Retrieve history for an area of ME Local Memory
 *
 * @param dev           NFP device
 * @param cycle_start   Start cycle
 * @param cycle_end     End cycle or 0 for all cycles
 * @param buffer        Buffer for return data
 * @param length        Length of buffer
 * @param count         Number of history values returned in buffer
 * @param menum         ME number
 * @param areaoffset    Byte offset
 * @param arealength    Byte length
 *
 * Retrieve memory history data; return format is groups of three
 * uint64ts, starting with the cycle followed by the data word and
 * the byte offset; data word is 4B
 *
 * @returns 0 on success, or -1 on failure (sets errno accordingly)
 *          and, if there was not enough space in @a buffer for all the
 *          available history, the number of words that fit the window.
 *          Note that a @a length of 0 will always return the number of
 *          words within the window
 */
NFP_SAL_API
int nfp_sal_thistory_get_lmem(struct nfp_device *dev, uint64_t cycle_start, uint64_t cycle_end, void *buffer, unsigned long long length, uint32_t *count, int menum, uint64_t areaoffset, uint64_t arealength);

/**
 * Retrieve history for an area of CLS memory
 *
 * @param dev           NFP device
 * @param cycle_start   Start cycle
 * @param cycle_end     End cycle or 0 for all cycles
 * @param buffer        Buffer for return data
 * @param length        Length of buffer
 * @param count         Number of history values returned in buffer
 * @param islnum        Island number
 * @param areaoffset    Byte offset
 * @param arealength    Byte length
 *
 * Retrieve memory history data; return format is groups of three
 * uint64ts, starting with the cycle followed by the data word and
 * the byte offset; data word is 8B
 *
 * @returns 0 on success, or -1 on failure (sets errno accordingly)
 *          and, if there was not enough space in @a buffer for all the
 *          available history, the number of words that fit the window.
 *          Note that a @a length of 0 will always return the number of
 *          words within the window
 */
NFP_SAL_API
int nfp_sal_thistory_get_cls(struct nfp_device *dev, uint64_t cycle_start, uint64_t cycle_end, void *buffer, unsigned long long length, uint32_t *count, int islnum, uint64_t areaoffset, uint64_t arealength);

/**
 * Retrieve history for an area of CTM memory
 *
 * @param dev           NFP device
 * @param cycle_start   Start cycle
 * @param cycle_end     End cycle or 0 for all cycles
 * @param buffer        Buffer for return data
 * @param length        Length of buffer
 * @param count         Number of history values returned in buffer
 * @param islnum        Island number
 * @param areaoffset    Byte offset
 * @param arealength    Byte length
 *
 * Retrieve memory history data; return format is groups of three
 * uint64ts, starting with the cycle followed by the data word and
 * the byte offset; data word is 8B
 *
 * @returns 0 on success, or -1 on failure (sets errno accordingly)
 *          and, if there was not enough space in @a buffer for all the
 *          available history, the number of words that fit the window.
 *          Note that a @a length of 0 will always return the number of
 *          words within the window
 */
NFP_SAL_API
int nfp_sal_thistory_get_ctm(struct nfp_device *dev, uint64_t cycle_start, uint64_t cycle_end, void *buffer, unsigned long long length, uint32_t *count, int islnum, uint64_t areaoffset, uint64_t arealength);

/**
 * Retrieve history for an area of Internal Memory
 *
 * @param dev           NFP device
 * @param cycle_start   Start cycle
 * @param cycle_end     End cycle or 0 for all cycles
 * @param buffer        Buffer for return data
 * @param length        Length of buffer
 * @param count         Number of history values returned in buffer
 * @param islnum        Island number
 * @param areaoffset    Byte offset
 * @param arealength    Byte length
 *
 * Retrieve memory history data; return format is groups of three
 * uint64ts, starting with the cycle followed by the data word and
 * the byte offset; data word is 8B
 *
 * @returns 0 on success, or -1 on failure (sets errno accordingly)
 *          and, if there was not enough space in @a buffer for all the
 *          available history, the number of words that fit the window.
 *          Note that a @a length of 0 will always return the number of
 *          words within the window
 */
NFP_SAL_API
int nfp_sal_thistory_get_imem(struct nfp_device *dev, uint64_t cycle_start, uint64_t cycle_end, void *buffer, unsigned long long length, uint32_t *count, int islnum, uint64_t areaoffset, uint64_t arealength);

/**
 * Retrieve history for an area of External Memory
 *
 * @param dev           NFP device
 * @param cycle_start   Start cycle
 * @param cycle_end     End cycle or 0 for all cycles
 * @param buffer        Buffer for return data
 * @param length        Length of buffer
 * @param count         Number of history values returned in buffer
 * @param islnum        Island number
 * @param areaoffset    Byte offset
 * @param arealength    Byte length
 *
 * Retrieve memory history data; return format is groups of three
 * uint64ts, starting with the cycle followed by the data word and
 * the byte offset; data word is 8B
 *
 * @returns 0 on success, or -1 on failure (sets errno accordingly)
 *          and, if there was not enough space in @a buffer for all the
 *          available history, the number of words that fit the window.
 *          Note that a @a length of 0 will always return the number of
 *          words within the window
 */
NFP_SAL_API
int nfp_sal_thistory_get_emem(struct nfp_device *dev, uint64_t cycle_start, uint64_t cycle_end, void *buffer, unsigned long long length, uint32_t *count, int islnum, uint64_t areaoffset, uint64_t arealength);

/**
 * Retrieve history for an area of Crypto memory
 *
 * @param dev           NFP device
 * @param cycle_start   Start cycle
 * @param cycle_end     End cycle or 0 for all cycles
 * @param buffer        Buffer for return data
 * @param length        Length of buffer
 * @param count         Number of history values returned in buffer
 * @param islnum        Island number
 * @param memtype       Crypto memory type
 * @param areaoffset    Byte offset
 * @param arealength    Byte length
 *
 * Retrieve memory history data; return format is groups of three
 * uint64ts, starting with the cycle followed by the data word and
 * the byte offset; data word is 8B
 *
 * @returns 0 on success, or -1 on failure (sets errno accordingly)
 *          and, if there was not enough space in @a buffer for all the
 *          available history, the number of words that fit the window.
 *          Note that a @a length of 0 will always return the number of
 *          words within the window
 */
NFP_SAL_API
int nfp_sal_thistory_get_crypto(struct nfp_device *dev, uint64_t cycle_start, uint64_t cycle_end, void *buffer, unsigned long long length, uint32_t *count, int islnum, int memtype, uint64_t areaoffset, uint64_t arealength);

/**
 * Retrieve history for an area of ILA shared memory
 *
 * @param dev           NFP device
 * @param cycle_start   Start cycle
 * @param cycle_end     End cycle or 0 for all cycles
 * @param buffer        Buffer for return data
 * @param length        length of buffer
 * @param count         Number of history values returned in buffer
 * @param islnum        Island number
 * @param areaoffset    Byte offset
 * @param arealength    Byte length
 *
 * Retrieve memory history data; return format is groups of three
 * uint64ts, starting with the cycle followed by the data word and
 * the byte offset; data word is 8B
 *
 * @returns 0 on success, or -1 on failure (sets errno accordingly)
 *          and, if there was not enough space in @a buffer for all the
 *          available history, the number of words that fit the window.
 *          Note that a @a length of 0 will always return the number of
 *          words within the window
 */
NFP_SAL_API
int nfp_sal_thistory_get_ila_shmem(struct nfp_device *dev, uint64_t cycle_start, uint64_t cycle_end, void *buffer, unsigned long long length, uint32_t *count, int islnum, uint64_t areaoffset, uint64_t arealength);

/**
 * Retrieve history for an area of External Memory data cache
 *
 * @param dev           NFP device
 * @param cycle_start   Start cycle
 * @param cycle_end     End cycle or 0 for all cycles
 * @param buffer        Buffer for return data
 * @param length        length of buffer
 * @param count         Number of history values returned in buffer
 * @param islnum        Island number
 * @param areaoffset    Byte offset
 * @param arealength    Byte length
 *
 * Retrieve memory history data; return format is groups of three
 * uint64ts, starting with the cycle followed by the data word and
 * the byte offset; data word is 8B
 *
 * @returns 0 on success, or -1 on failure (sets errno accordingly)
 *          and, if there was not enough space in @a buffer for all the
 *          available history, the number of words that fit the window.
 *          Note that a @a length of 0 will always return the number of
 *          words within the window
 */
NFP_SAL_API
int nfp_sal_thistory_get_emem_dc(struct nfp_device *dev, uint64_t cycle_start, uint64_t cycle_end, void *buffer, unsigned long long length, uint32_t *count, int islnum, uint64_t areaoffset, uint64_t arealength);



/** @}
 * @}
 */

__END_DECLS;

#endif /* !__NFP_SAL_HISTORY_H__ */
