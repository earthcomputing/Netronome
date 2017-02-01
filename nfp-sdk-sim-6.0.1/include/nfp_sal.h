/*
 * Copyright (C) 2012-2013,  Netronome Systems, Inc.  All rights reserved.
 */

#ifndef __NFP_SAL_H__
#define __NFP_SAL_H__

#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/cdefs.h>

#include <nfp.h>
#include <nfp_cpp.h>
#include <nfp_sal_net.h>
#include <nfp_sal_watch.h>
#include <nfp_sal_history.h>
#include <sim_events.h>

#include "_nfp_sal_apiex.h"


__BEGIN_DECLS;

/** @file nfp_sal.h
 *  @addtogroup nfsal NFSAL
 *  @{
 */

/** @name Functions for interacting with Simulated NFP devices.
 *  @{
 *
 *   Function include the following categories:
 *   - Run control
 *   - Clock control and status
 *   - Simulation only state access
 *   - Performance statistics and execution coverage
 *   - Simulator event subscription
 */

/**
 * Step the simulated NFP by a single ME clock cycle
 *
 * @param dev           NFP device
 * @param num_cycles    Number of cycles to step; -1 for run indefinitely
 * @param block         If non-zero the call will block until the step command
 *                      is completed, otherwise the call will return immediately
 *
 * @return 0 on success, or -1 on failure (sets errno accordingly)
 */
NFP_SAL_API
int nfp_sal_clock_step(struct nfp_device *dev, int num_cycles, int block);

/**
 * Instruct the simulator to run freely
 *
 * @param dev           NFP device
 *
 * @return 0 on success, or -1 on failure (sets errno accordingly)
 */
NFP_SAL_API
int nfp_sal_run(struct nfp_device *dev);

/**
 * Instruct the simulator to stop
 *
 * @param dev           NFP device
 * @param block         If non-zero the call will block until the stop command
 *                      is completed, otherwise the call will return immediately
 *
 * @return 0 on success, or -1 on failure (sets errno accordingly)
 */
NFP_SAL_API
int nfp_sal_stop(struct nfp_device *dev, int block);

/** Types of reset applicable to @a nfp_sal_reset */
enum NFP_SAL_RESET_LEVEL_TYPES {
    /** Reset to booted chip state */
    NFP_SAL_RESET_LEVEL_CHIP = 0,
    /** Reset to BSP bootloader state */
    NFP_SAL_RESET_LEVEL_BSP,
};

/**
 * Reset the simulator
 *
 * @param dev           NFP device
 * @param level         Reset level, either chip or bsp
 * @param block         If non-zero the call will block until the reset command
 *                      is completed, otherwise the call will return immediately
 *
 * @return 0 on success, or -1 on failure (sets errno accordingly)
 */
NFP_SAL_API
int nfp_sal_reset(struct nfp_device *dev, int level, int block);

/**
 * Instruct the simulator to shut down
 *
 * @param dev           NFP device
 *
 * @return 0 on success, or -1 on failure (sets errno accordingly)
 */
NFP_SAL_API
int nfp_sal_shutdown(struct nfp_device *dev);

/**
 * Returns '1' if the simulator is running or resetting
 *
 * @param dev           NFP device
 *
 * @returns 1 if running, 0 if not, or -1 on failure (sets errno accordingly)
 */
NFP_SAL_API
int nfp_sal_is_running(struct nfp_device *dev);

/**
 * Returns '1' if the simulator is in a paused state
 *
 * @param dev           NFP device
 *
 * The simulator may enter a paused state in which no cycles will be issued
 * until a user clears the pause. Use the watch APIs to control entering and
 * exiting the pause state.
 *
 * @returns 1 if paused, 0 if not, or -1 on failure (sets errno accordingly)
 */
NFP_SAL_API
int nfp_sal_is_paused(struct nfp_device *dev);

/**
 * Returns the current ME cycle count
 *
 * @param dev           NFP device
 *
 * @returns the current ME cycle
 */
NFP_SAL_API
uint64_t nfp_sal_cycle_count(struct nfp_device *dev);

/** The supported user run types for @a nfp_sal_user_run_update */
enum NFP_SAL_USER_RUN_TYPES {
    /** update based on cycles */
    NFP_SAL_USER_RUN_UPDATE_CYCLES = 0,
    /** update based on time */
    NFP_SAL_USER_RUN_UPDATE_TIME,
};

/**
 * Enable user run status updates
 *
 * @param dev           NFP device
 * @param type          Update type, time or cycles
 * @param count         Cycle count if type is cycles, otherwise time
 *                      in milliseconds
 *
 * @return 0 on success, or -1 on failure (sets errno accordingly)
 */
NFP_SAL_API
int nfp_sal_user_run_update(struct nfp_device *dev, int type, uint64_t count);

/** The supported clock types for @a nfp_sal_clock_info */
enum NFP_SAL_CLOCK_TYPES {
    /** MicroEngine Clock */
    NFP_SAL_CLOCK_TYPE_TCLK = 0,
    /** System Clock */
    NFP_SAL_CLOCK_TYPE_PCLK,
    /** MAC 0 Clock */
    NFP_SAL_CLOCK_TYPE_ILA0CLK,
    /** MAC 1 Clock */
    NFP_SAL_CLOCK_TYPE_ILA1CLK,
    /** EMEM 0 Clock */
    NFP_SAL_CLOCK_TYPE_D2CLK,
    /** EMEM 1 Clock */
    NFP_SAL_CLOCK_TYPE_D1CLK,
    /** EMEM 2 Clock */
    NFP_SAL_CLOCK_TYPE_D0CLK,
    /** End indicator */
    NFP_SAL_CLOCK_TYPE_LAST,
};

/**
 * Retrieve information about a clock
 *
 * @param dev             NFP device
 * @param clock_type      Clock type; an entry in NFP_SAL_CLOCK_TYPES
 * @param target_period   Target period in picoseconds
 * @param actual_period   Actual period in picoseconds
 *
 * @returns 0 on success, or -1 on failure (sets errno accordingly)
 */
NFP_SAL_API
int nfp_sal_clock_info(struct nfp_device *dev, int clock_type, uint32_t *target_period, uint32_t *actual_period);

/**
 * Enable an island's clock
 *
 * @param dev             NFP device
 * @param isl_num         Island number
 *
 * Enable an island's clock; note that a number of clock cycles will need
 * to pass before the island's logic will be enabled
 *
 * @returns 0 on success, or -1 on failure (sets errno accordingly)
 */
NFP_SAL_API
int nfp_sal_island_clock_enable(struct nfp_device *dev, int isl_num);

/**
 * Disable an island's clock - note that this will require a number of
 * simulation cycles to take effect
 *
 * @param dev             NFP device
 * @param isl_num         Island number
 *
 * Disable an island's clock; note that a number of clock cycles will need
 * to pass before the island's logic will be disabled
 *
 * @returns 0 on success, or -1 on failure (sets errno accordingly)
 */
NFP_SAL_API
int nfp_sal_island_clock_disable(struct nfp_device *dev, int isl_num);

/**
 * Check if an island's clock is enabled or not
 *
 * @param dev             NFP device
 * @param isl_num         Island number
 *
 * @returns 0 is disabled, 1 if enabled and -1 on error
 */
NFP_SAL_API
int nfp_sal_island_clock_is_enabled(struct nfp_device *dev, int isl_num);

/**
 * Read External Memory data cache
 * @ingroup expextmemfunc
 *
 * @param dev           NFP device
 * @param islnum        Island number
 * @param buffer        Output buffer
 * @param length        Number of bytes to read
 * @param offset        Offset into External Memory
 *
 * Read contents of External Memory data cache into @a buffer.  @a length
 * and @a offset must both be 8-byte aligned.
 *
 * @return Number of bytes actually read, or -1 on error (and set errno accordingly)
 */
NFP_SAL_API
ssize_t nfp_sal_extmem_dc_read(struct nfp_device *dev, int islnum, void *buffer, unsigned long long length, unsigned long long offset);

/**
 * Write to External Memory data cache
 * @ingroup expextmemfunc
 *
 * @param dev           NFP device
 * @param islnum        Island number
 * @param buffer        Input buffer
 * @param length        Number of bytes to write
 * @param offset        Offset into External Memory
 *
 * Write contents of @a buffer into External Memory data cache.  @a length
 * and @a offset must both be 8-byte aligned.
 *
 * @return Number of bytes actually written, or -1 on error (and set errno accordingly)
 */
NFP_SAL_API
ssize_t nfp_sal_extmem_dc_write(struct nfp_device *dev, int islnum, const void *buffer, unsigned long long length, unsigned long long offset);

/**
 * Fill External Memory data cache with a given pattern
 * @ingroup expextmemfunc
 *
 * @param dev           NFP device
 * @param islnum        Island number
 * @param buffer        Input buffer
 * @param buf_length    Number of bytes in input buffer
 * @param offset        Offset into External Memory
 * @param write_length  Number of bytes to write
 *
 * Write contents of @a buffer into internal memory.  @a length
 * and @a offset must both be 8-byte aligned.
 *
 * @return Number of bytes actually written, or -1 on error (and set errno accordingly)
 */
NFP_SAL_API
ssize_t nfp_sal_extmem_dc_fill(struct nfp_device *dev, int islnum, const void *buffer, unsigned long long buf_length, unsigned long long offset, unsigned long long write_length);

/**
 * Read External Memory tag cache
 * @ingroup expextmemfunc
 *
 * @param dev           NFP device
 * @param islnum        Island number
 * @param tcnum         Tag cache memory index
 * @param buffer        Output buffer
 * @param length        Number of bytes to read
 * @param offset        Offset into External Memory
 *
 * Read contents of External Memory tag cache into @a buffer.  @a length
 * and @a offset must both be 8-byte aligned.
 *
 * @return Number of bytes actually read, or -1 on error (and set errno accordingly)
 */
NFP_SAL_API
ssize_t nfp_sal_extmem_tc_read(struct nfp_device *dev, int islnum, int tcnum, void *buffer, unsigned long long length, unsigned long long offset);

/**
 * Write to External Memory tag cache
 * @ingroup expextmemfunc
 * @param dev           NFP device
 * @param islnum        Island number
 * @param tcnum         Tag cache memory index
 * @param buffer        Input buffer
 * @param length        Number of bytes to write
 * @param offset        Offset into External Memory
 *
 * Write contents of @a buffer into External Memory tag cache.  @a length
 * and @a offset must both be 8-byte aligned.
 *
 * @return Number of bytes actually written, or -1 on error (and set errno accordingly)
 */
NFP_SAL_API
ssize_t nfp_sal_extmem_tc_write(struct nfp_device *dev, int islnum, int tcnum, const void *buffer, unsigned long long length, unsigned long long offset);

/**
 * Read memory from Crypto SRAM
 * @ingroup expcryptosramfunc
 *
 * @param dev           NFP device
 * @param islnum        Island number
 * @param memnum        Crypto memory type
 * @param buffer        Output buffer
 * @param length        Number of bytes to read
 * @param offset        Offset into NFP Crypto SRAM
 *
 * Read contents of NFP Crypto SRAM into @a buffer.  @a length and @a offset must
 * both be 4-byte aligned.
 *
 * @return Number of bytes actually read, or -1 on error (and set errno accordingly)
 */
NFP_SAL_API
ssize_t nfp_sal_crypto_sram_read(struct nfp_device *dev, int islnum, int memnum, void *buffer, unsigned long long length, unsigned long long offset);

/**
 * Write to Crypto SRAM
 * @ingroup expcryptosramfunc
 *
 * @param dev           NFP device
 * @param islnum        Island number
 * @param memnum        Crypto memory type
 * @param buffer        Input buffer
 * @param length        Number of bytes to write
 * @param offset        Offset into NFP Crypto SRAM
 *
 * Write contents of @a buffer into NFP Crypto SRAM.  @a length and @a offset must
 * both be 4-byte aligned.
 *
 * @return Number of bytes actually written, or -1 on error (and set errno accordingly)
 */
NFP_SAL_API
ssize_t nfp_sal_crypto_sram_write(struct nfp_device *dev, int islnum, int memnum, const void *buffer, unsigned long long length, unsigned long long offset);

/**
 * Fill Crypto SRAM with given pattern.
 * @ingroup expcryptosramfunc
 *
 * @param dev           NFP device
 * @param islnum        Island number
 * @param memtype       Crypto memory type
 * @param buffer        Input buffer to fill with
 * @param buf_length    Number of bytes in input buffer
 * @param offset        Offset into NFP SRAM
 * @param write_length  number of bytes to write
 *
 * Fill indicated NFP Crypto SRAM area with contents of @a buffer.  If
 * @a write_length is larger than @a buf_length the contents of @a buffer
 * will be repeated until the whole area is filled.  @a buf_length,
 * @a offset, and @a write_length must all be 4-byte aligned.
 *
 * @return Number of bytes actually written, or -1 on error (and set errno accordingly)
 */
NFP_SAL_API
ssize_t nfp_sal_crypto_sram_fill(struct nfp_device *dev, int islnum, int memtype, const void *buffer, unsigned long long buf_length, unsigned long long offset, unsigned long long write_length);

/**
 * Read memory from ILA Shared SRAM
 * @ingroup expilasramfunc
 *
 * @param dev           NFP device
 * @param islnum        Island number
 * @param buffer        Output buffer
 * @param length        Number of bytes to read
 * @param offset        Offset into NFP ILA Shared SRAM
 *
 * Read contents of NFP ILA Shared SRAM into @a buffer.  @a length and @a offset must
 * both be 4-byte aligned.
 *
 * @return Number of bytes actually read, or -1 on error (and set errno accordingly)
 */
NFP_SAL_API
ssize_t nfp_sal_ila_shmem_read(struct nfp_device *dev, int islnum, void *buffer, unsigned long long length, unsigned long long offset);

/**
 * Write to ILA Shared SRAM
 * @ingroup expilasramfunc
 *
 * @param dev           NFP device
 * @param islnum        Island number
 * @param buffer        Input buffer
 * @param length        Number of bytes to write
 * @param offset        Offset into NFP ILA Shared SRAM
 *
 * Write contents of @a buffer into NFP ILA Shared SRAM.  @a length and @a offset must
 * both be 4-byte aligned.
 *
 * @return Number of bytes actually written, or -1 on error (and set errno accordingly)
 */
NFP_SAL_API
ssize_t nfp_sal_ila_shmem_write(struct nfp_device *dev, int islnum, const void *buffer, unsigned long long length, unsigned long long offset);

/**
 * Fill ILA Shared SRAM with given pattern.
 * @ingroup expilasramfunc
 *
 * @param dev           NFP device
 * @param islnum        Island number
 * @param buffer        Input buffer to fill with
 * @param buf_length    Number of bytes in input buffer
 * @param offset        Offset into NFP ILA Shared SRAM
 * @param write_length  Number of bytes to write
 *
 * Fill indicated NFP ILA Shared SRAM area with contents of @a buffer.  If
 * @a write_length is larger than @a buf_length the contents of @a buffer
 * will be repeated until the whole area is filled.  @a buf_length,
 * @a offset, and @a write_length must all be 4-byte aligned.
 *
 * @return Number of bytes actually written, or -1 on error (and set errno accordingly)
 */
NFP_SAL_API
ssize_t nfp_sal_ila_shmem_fill(struct nfp_device *dev, int islnum, const void *buffer, unsigned long long buf_length, unsigned long long offset, unsigned long long write_length);

/**
 * Enable execution coverage collection for an ME or all MEs
 *
 * @param dev           NFP device
 * @param menum         If -1 all MEs otherwise the MEID
 *
 * This function may be called multiple times to enable multiple MEs
 *
 * @return 0 on success, or -1 on failure (sets errno accordingly)
 */
NFP_SAL_API
int nfp_sal_excover_enable(struct nfp_device *dev, int menum);

/**
 * Disable all execution coverage collection
 *
 * @param dev           NFP device
 *
 * @return 0 on success, or -1 on failure (sets errno accordingly)
 */
NFP_SAL_API
int nfp_sal_excover_disable(struct nfp_device *dev);

/**
 * Check if execution coverage collection is enabled
 *
 * @param dev           NFP device
 * @param menum         ME number
 *
 * @return 1 if enabled, 0 if not
 */
NFP_SAL_API
int nfp_sal_excover_is_enabled(struct nfp_device *dev, int menum);

/**
 * Retrieve execution coverage data for ME context
 *
 * @param dev           NFP device
 * @param menum         ME Number
 * @param ctx           ME context
 * @param buffer        Return buffer (should be 64KB)
 * @param length        Return buffer length
 *
 * @return 0 on success, or -1 on failure (sets errno accordingly)
 */
NFP_SAL_API
int nfp_sal_excover_get(struct nfp_device *dev, int menum, int ctx, void *buffer, unsigned int length);

/**
 * Enable performance statistics collection
 *
 * @param dev           NFP device
 *
 * @return 0 on success, or -1 on failure (sets errno accordingly)
 */
NFP_SAL_API
int nfp_sal_pstats_enable(struct nfp_device *dev);

/**
 * Disable performance statistics collection
 *
 * @param dev           NFP device
 *
 * @return 0 on success, or -1 on failure (sets errno accordingly)
 */
NFP_SAL_API
int nfp_sal_pstats_disable(struct nfp_device *dev);

/**
 * Check if performance statistics collection is enabled
 *
 * @param dev           NFP device
 *
 * @return 1 if enabled, 0 if not
 */
NFP_SAL_API
int nfp_sal_pstats_is_enabled(struct nfp_device *dev);

/**
 * Reset performance statistics
 *
 * @param dev           NFP device
 *
 * @return 0 on success, or -1 on failure (sets errno accordingly)
 */
NFP_SAL_API
int nfp_sal_pstats_reset(struct nfp_device *dev);

/**
 * Retrieve performance statistics for an ME
 *
 * @param dev           NFP device
 * @param menum         ME Number
 * @param ctx           ME context
 * @param cycles        Total collected cycles
 * @param executing     Executing cycles
 * @param aborted       Aborted cycles
 * @param stalled       Stalled cycles
 * @param idle          Idle cycles
 * @param swapped       Swapped-out cycles
 *
 * @return 0 on success, or -1 on failure (sets errno accordingly)
 */
NFP_SAL_API
int nfp_sal_pstats_get_me(struct nfp_device *dev, int menum, int ctx, uint64_t *cycles, uint64_t *executing, uint64_t *aborted, uint64_t *stalled, uint64_t *idle, uint64_t *swapped);

/** FIFO types for @a nfp_sal_pstats_get_fifo */
enum PSTATS_FIFO_TYPES {
    /** MicroEngine command FIFO */
    PSTATS_FIFO_TYPE_MECMD = 0,
    /** Next Neighbour FIFO */
    PSTATS_FIFO_TYPE_MENN,
};

/** Macro for building up the @a fifoid used for @a nfp_sal_pstats_get_fifo */
#define PSTATS_FIFO_ID(isl_or_me, type) \
             ((((isl_or_me) & 0xffff) << 16) | ((type) & 0xffff)) \
/** Macro for getting the FIFO type from a @a fifoid */
#define PSTATS_FIFO_ID_TYPE(id) ((((id)) >> 0) & 0xffff)
/** Macro for getting an Island or MicroEngine number from a @a fifoid */
#define PSTATS_FIFO_ID_ISL_OR_ME(id) ((((id)) >> 16) & 0xffff)

/**
 * Retrieve performance statistics histogram for a FIFO
 *
 * @param dev           NFP device
 * @param fifoid        ID used to identify FIFO
 * @param cycles        Total collected cycles
 * @param buffer        Return buffer for histogram
 * @param length        Size of buffer
 * @param count         Count of histogram entries returned
 *
 * Histogram data is stored in buffer in uint64_t pairs, with the level first
 * followed by the cycle count. count will contain the number of pairs returned.
 *
 * @return 0 on success, or -1 on failure (sets errno accordingly)
 */
NFP_SAL_API
int nfp_sal_pstats_get_fifo(struct nfp_device *dev, uint64_t fifoid, uint64_t *cycles, void *buffer, int length, uint32_t *count);

/**
 * Enable DSF statistics collection
 *
 * @param dev           NFP device
 * @param window        Window size in cycles (maximum 10000), 0 for no window
 *
 * @return 0 on success, or -1 on failure (sets errno accordingly)
 */
NFP_SAL_API
int nfp_sal_dsfstats_enable(struct nfp_device *dev, int window);

/**
 * Disable DSF statistics collection
 *
 * @param dev           NFP device
 *
 * @return 0 on success, or -1 on failure (sets errno accordingly)
 */
NFP_SAL_API
int nfp_sal_dsfstats_disable(struct nfp_device *dev);

/**
 * Check if DSF statistics collection is enabled
 *
 * @param dev           NFP device
 * @param windowsize    Return buffer for window size
 *
 * @return 1 if enabled, 0 if not
 */
NFP_SAL_API
int nfp_sal_dsfstats_is_enabled(struct nfp_device *dev, int *windowsize);

/**
 * Reset current DSF statistics
 *
 * @param dev           NFP device
 *
 * @return 0 on success, or -1 on failure (sets errno accordingly)
 */
NFP_SAL_API
int nfp_sal_dsfstats_reset(struct nfp_device *dev);

/** Types of DSF statistics available */
enum NFP_SAL_DSFSTATS_TYPES {
    /** Statistics per DSF port */
    NFP_SAL_DSFSTATS_TYPE_PORT = 0,
    /** Statistics per DSF island destination */
    NFP_SAL_DSFSTATS_TYPE_DEST,
};

/** Types of DSF payloads */
enum NFP_SAL_DSFSTATS_DATA_TYPES {
    /** DSF Command - statistics reported as command count */
    NFP_SAL_DSFSTATS_TYPE_CMD = 0,
    /** DSF DATA port 0 - statistics reported as 32-bit word count */
    NFP_SAL_DSFSTATS_TYPE_DAT0,
    /** DSF DATA port 1 - statistics reported as 32-bit word count */
    NFP_SAL_DSFSTATS_TYPE_DAT1,
    /** DSF PullID - statistics reported as pullID count */
    NFP_SAL_DSFSTATS_TYPE_PULLID,
};

/** DSF port for type NFP_SAL_DSFSTATS_TYPE_PORT
  * This is 0-7 for egress ports;  8 and 9 for ingress ports
  * 6 and 7
  */
#define NFP_SAL_DSFSTATS_DATA_PORT_of(v) (((v) >> 56) & 0xff)
#define NFP_SAL_DSFSTATS_DATA_PORT(v)    (((v) & 0xff) << 56)
/** Destination island (0-63) for type NFP_SAL_DSFSTATS_TYPE_DEST */
#define NFP_SAL_DSFSTATS_DATA_DEST_of(v) (((v) >> 56) & 0xff)
#define NFP_SAL_DSFSTATS_DATA_DEST(v)    (((v) & 0xff) << 56)
/** DSF payload type: see NFP_SAL_DSFSTATS_DATA_TYPES */
#define NFP_SAL_DSFSTATS_DATA_TYPE_of(v) (((v) >> 52) & 0xf)
#define NFP_SAL_DSFSTATS_DATA_TYPE(v)    (((v) & 0xf) << 52)
/* 52-bits of data */
#define NFP_SAL_DSFSTATS_DATA_STAT_of(v) ((v) & 0xfffffffffffff)
#define NFP_SAL_DSFSTATS_DATA_STAT(v)    ((v) & 0xfffffffffffff)

/**
 * Retrieve DSF statistics
 *
 * @param dev           NFP device
 * @param islnum        Island number
 * @param type          Type of statistics, see NFP_SAL_DSFSTATS_TYPES
 * @param usewindow     Set to '1' to retrieve the window statistics,
 *                      otherwise retrieve all stats since enabled
 * @param buffer        Return buffer
 * @param len           Return buffer length
 * @param cycles        ME cycles over which the statistics apply
 *
 * Retrieve DSF statistics for DSF ports or between a source and
 * destination islands. All statistics for a source island are populated in
 * @a buffer. The return data includes the type and value;
 * use NFP_SAL_DSFSTATS_DATA_* macros to retrieve these fields.
 * Note that @a len should be 64*4 for type DEST and 10*4 for type PORT.
 * If @a len is less than zero just @a cycles will be populated.
 *
 * @returns -1 on failure (sets errno accordingly), otherwise returns
 *  the elements populated in the return array
 */
NFP_SAL_API
int nfp_sal_dsfstats_retrieve(struct nfp_device *dev, int islnum, int type, int usewindow, uint64_t *buffer, int len, uint64_t *cycles);

/**
 * Enable MAC Minipacket statistics collection
 *
 * @param dev           NFP device
 * @param window        Window size in cycles (maximum 10000), 0 for no window
 *
 * @return 0 on success, or -1 on failure (sets errno accordingly)
 */
NFP_SAL_API
int nfp_sal_minipktstats_enable(struct nfp_device *dev, int window);

/**
 * Disable MAC Minipacket statistics collection
 *
 * @param dev           NFP device
 *
 * @return 0 on success, or -1 on failure (sets errno accordingly)
 */
NFP_SAL_API
int nfp_sal_minipktstats_disable(struct nfp_device *dev);

/**
 * Check if MAC Minipacket statistics collection is enabled
 *
 * @param dev           NFP device
 * @param windowsize    Return buffer for window size
 *
 * @return 1 if enabled, 0 if not
 */
NFP_SAL_API
int nfp_sal_minipktstats_is_enabled(struct nfp_device *dev, int *windowsize);

/**
 * Reset current MAC Minipacket statistics
 *
 * @param dev           NFP device
 *
 * @return 0 on success, or -1 on failure (sets errno accordingly)
 */
NFP_SAL_API
int nfp_sal_minipktstats_reset(struct nfp_device *dev);

/**
 * Retrieve MAC Minipacket statistics
 *
 * @param dev           NFP device
 * @param nbi           Which NBI to use
 * @param chan          MAC channel, or -1 to combine all channels
 * @param usewindow     Set to '1' to retrieve the window statistics,
 *                      otherwise retrieve all stats since enabled
 * @param ig_pkts       Ingress packet count
 * @param ig_bytes      Ingress byte count
 * @param ig_errs       Ingress error count
 * @param eg_pkts       Egress packet count
 * @param eg_bytes      Egress byte count
 * @param eg_errs       Egress error count
 * @param cycles        ME cycles over which the statistics apply
 *
 * @return 0 on success, or -1 on failure (sets errno accordingly)
 */
NFP_SAL_API
int nfp_sal_minipktstats_retrieve(struct nfp_device *dev, int nbi, int chan, int usewindow, uint64_t *ig_pkts, uint64_t *ig_bytes, uint64_t *ig_errs, uint64_t *eg_pkts, uint64_t *eg_bytes, uint64_t *eg_errs, uint64_t *cycles);

/** Modes applicable to @a nfp_sal_cpp_mode */
enum NFP_SAL_CPP_MODES {
    /** Backdoor access only - no clocks issued for CPP transactions */
    NFP_SAL_CPP_MODE_BACKONLY = 0,
    /** Frontdoor access only - CPP transactions issued on clocked bus  */
    NFP_SAL_CPP_MODE_FRONTONLY,
    /** Attempt backdoor access, upon failure use frontdoor */
    NFP_SAL_CPP_MODE_BACKFIRST,
};

/**
 * Check if a client is connected to the PCIe socket interface
 *
 * @param dev          NFP device
 * @param pci          Which PCI interface to use
 *
 * @returns 1 if connected, 0 if not, and -1 on error
 */
NFP_SAL_API
int nfp_sal_pcie_isconnected(struct nfp_device *dev, int pci);

/**
 * Configure the CPP API access mode
 *
 * @param dev          NFP device
 * @param mode         CPP access mode
 *
 * @returns 0 is disabled, 1 if enabled and -1 on error
 */
NFP_SAL_API
int nfp_sal_cpp_mode(struct nfp_cpp *dev, int mode);

/** Types of available tweaks for @a nfp_sal_tweak */
enum NFP_SAL_TWEAK_TYPES {
    /** Accelerate the reset for all MAC blocks */
    NFP_SAL_TWEAK_MACRESET = 0,
};

/**
 * Perform an SDK tweak
 *
 * @param dev           NFP device
 * @param tweakid       ID of the tweak to perform
 * @param arg           Generic argument
 *
 * @return 0 on success, or -1 on failure (sets errno accordingly)
 */
NFP_SAL_API
int nfp_sal_tweak(struct nfp_device *dev, int tweakid, uint64_t arg);

/**
 * Get the simulator log level
 *
 * @param dev           NFP device
 * @param component     Simulator component or 0 for default
 * @return Log level
 *
 */
NFP_SAL_API
int nfp_sal_get_log_level(struct nfp_device *dev, int component);

/**
 * Set the simulator log level
 *
 * @param dev           NFP device
 * @param component     Simulator component or 0 for default
 * @param lvl           Log level
 *
 */
NFP_SAL_API
void nfp_sal_set_log_level(struct nfp_device *dev, int lvl, int component);

/**
 * Broadcast a user-generated simulation event
 *
 * @param dev           NFP device
 * @param type          user definer type
 * @param subtype       user definer subtype
 *
 * Broadcase a user-generated simulation event to all listening clients;
 * @a type and @a subtype will be copied in the event structure simevent_user;
 * refer to the simevent_user structure in sim_events.h for for information
 *
 * @returns -1 on error, otherwise value to use to unsubscribe
 */
NFP_SAL_API
int nfp_sal_send_simevent(struct nfp_device *dev, uint64_t type, uint64_t subtype);

/**
 * Subscribe to all events
 *
 * @param dev           NFP device
 * @param callback      The function to call
 *
 * @returns -1 on error, otherwise value to use to unsubscribe
 */
NFP_SAL_API
int nfp_sal_subs_simevent_all(struct nfp_device *dev, void (*callback)(struct nfp_device *dev, uint32_t ev_type, void *ev_data, uint32_t ev_data_len));

/**
 * Unsubscribe to an event
 *
 * @param dev           NFP device
 * @param handle        The handle returned from the subscribe function
 *
 * @returns -1 on error, otherwise 0
 */
NFP_SAL_API
int nfp_sal_unsubs_simevent(struct nfp_device *dev, int handle);

/**
 * Register the event handler for SIM event shutdown
 *
 * @param dev           NFP device
 * @param callback      The event handler which must be called for the event
 *
 * @returns -1 on error, otherwise a handle which must be passed to
 *          nfp_sal_unsubs_simevent() when unsubscribing from the event
 */
NFP_SAL_API
int nfp_sal_subs_simevent_sim_shutdown(struct nfp_device *dev, void (*callback)(struct nfp_device *dev, struct simevent_sim_shutdown *ev));

/**
 * Register the event handler for SIM event restart
 *
 * @param dev           NFP device
 * @param callback      The event handler which must be called for the event
 *
 * @returns -1 on error, otherwise a handle which must be passed to
 *          nfp_sal_unsubs_simevent() when unsubscribing from the event
 */
NFP_SAL_API
int nfp_sal_subs_simevent_sim_restart(struct nfp_device *dev, void (*callback)(struct nfp_device *dev, struct simevent_sim_restart *ev));

/**
 * Register the event handler for SIM event run start
 *
 * @param dev           NFP device
 * @param callback      The event handler which must be called for the event
 *
 * @returns -1 on error, otherwise a handle which must be passed to
 *          nfp_sal_unsubs_simevent() when unsubscribing from the event
 */
NFP_SAL_API
int nfp_sal_subs_simevent_run_start(struct nfp_device *dev, void (*callback)(struct nfp_device *dev, struct simevent_run_start *ev));

/**
 * Register the event handler for SIM event run update
 *
 * @param dev           NFP device
 * @param callback      The event handler which must be called for the event
 *
 * @returns -1 on error, otherwise a handle which must be passed to
 *          nfp_sal_unsubs_simevent() when unsubscribing from the event
 */
NFP_SAL_API
int nfp_sal_subs_simevent_run_update(struct nfp_device *dev, void (*callback)(struct nfp_device *dev, struct simevent_run_update *ev));

/**
 * Register the event handler for SIM event run stop
 *
 * @param dev           NFP device
 * @param callback      The event handler which must be called for the event
 *
 * @returns -1 on error, otherwise a handle which must be passed to
 *          nfp_sal_unsubs_simevent() when unsubscribing from the event
 */
NFP_SAL_API
int nfp_sal_subs_simevent_run_stop(struct nfp_device *dev, void (*callback)(struct nfp_device *dev, struct simevent_run_stop *ev));

/**
 * Register the event handler for SIM event reset
 *
 * @param dev           NFP device
 * @param callback      The event handler which must be called for the event
 *
 * @returns -1 on error, otherwise a handle which must be passed to
 *          nfp_sal_unsubs_simevent() when unsubscribing from the event
 */
NFP_SAL_API
int nfp_sal_subs_simevent_run_reset(struct nfp_device *dev, void (*callback)(struct nfp_device *dev, struct simevent_run_reset *ev));

/**
 * Register the event handler for SIM event reset done
 *
 * @param dev           NFP device
 * @param callback      The event handler which must be called for the event
 *
 * @returns -1 on error, otherwise a handle which must be passed to nfp_sal_unsubs_simevent() when unsubscribing from the event
 */
NFP_SAL_API
int nfp_sal_subs_simevent_run_resetdone(struct nfp_device *dev, void (*callback)(struct nfp_device *dev, struct simevent_run_resetdone *ev));

/**
 * Register the event handler for SIM event watch
 *
 * @param dev           NFP device
 * @param callback      The event handler which must be called for the event
 *
 * @returns -1 on error, otherwise a handle which must be passed to
 *          nfp_sal_unsubs_simevent() when unsubscribing from the event
 */
NFP_SAL_API
int nfp_sal_subs_simevent_watch(struct nfp_device *dev, void (*callback)(struct nfp_device *dev, struct simevent_watch *ev));

/**
 * Register the event handler for SIM event pkt in sent
 *
 * @param dev           NFP device
 * @param callback      The event handler which must be called for the event
 *
 * @returns -1 on error, otherwise a handle which must be passed to
            nfp_sal_unsubs_simevent() when unsubscribing from the event
 */
NFP_SAL_API
int nfp_sal_subs_simevent_pkt_insent(struct nfp_device *dev, void (*callback)(struct nfp_device *dev, struct simevent_pkt_insent *ev));

/**
 * Register the event handler for SIM event pkt out receive
 *
 * @param dev           NFP device
 * @param callback      The event handler which must be called for the event
 *
 * @returns -1 on error, otherwise a handle which must be passed to
 *          nfp_sal_unsubs_simevent() when unsubscribing from the event
 */
NFP_SAL_API
int nfp_sal_subs_simevent_pkt_outrecv(struct nfp_device *dev, void (*callback)(struct nfp_device *dev, struct simevent_pkt_outrecv *ev));

/**
 * Register the event handler for SIM event ilkn out receive
 *
 * @param dev           NFP device
 * @param callback      The event handler which must be called for the event
 *
 * @returns -1 on error, otherwise a handle which must be passed to
 *          nfp_sal_unsubs_simevent() when unsubscribing from the event
 */
NFP_SAL_API
int nfp_sal_subs_simevent_ilkn_outrecv(struct nfp_device *dev, void (*callback)(struct nfp_device *dev, struct simevent_ilkn_outrecv *ev));

/**
 * Report the SIM event queue level
 *
 * @param dev           NFP device
 *
 * @returns -1 on error, otherwise the event queue depth
 */
NFP_SAL_API
int nfp_sal_simevent_queue_level(struct nfp_device *dev);

/**
 * Retrieve the RPC version numbers from the client libraries and remote server
 *
 * @param cln_ver           Client library version
 * @param cln_compat        Client library compatibility version
 * @param srv_ver           RPC server version
 * @param srv_compat        RPC server compatibility version
 *
 * @returns -1 on error and 0 on success, and 1 if not-compatible
 */
NFP_SAL_API
int nfp_sal_rpc_version(int *cln_ver, int *cln_compat, int *srv_ver, int *srv_compat);

/** @}
 * @}
 */

__END_DECLS;

#endif /* !__NFP_SAL_H__ */

