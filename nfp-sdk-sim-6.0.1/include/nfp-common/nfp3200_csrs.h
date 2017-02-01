/*
 * Copyright (C) 2011-2014,  Netronome Systems, Inc.  All rights reserved. 
 *                
 * @file          nfp3200_csrs.h
 * @brief         API for lookin. up NFP CSRs based on register names
 *                
 */
#ifndef __NFP_CSRS_H__
#define __NFP_CSRS_H__

#include <nfp-common/nfp_platform.h>

#include "_nfp_common_apiex.h"

/**
 * Skipflags used for nfp_csr_matches() function.
 */
enum nfp_csr_skipflags {
    NFP_CSR_SKIP_NONE = 0x00,
    NFP_CSR_SKIP_FULLNAME = 0x01,
    NFP_CSR_SKIP_ALTNAME = 0x02,
    NFP_CSR_SKIP_ALLNAMES = 0x03,
    NFP_CSR_SKIP_BITFIELDS = 0x04,
    NFP_CSR_SKIP_READONLY = 0x0100,
    NFP_CSR_SKIP_WRITEONLY = 0x0200,
    NFP_CSR_SKIP_RESERVED = 0x0400,
};

/**
 * Register types for CSRs.
 */
enum nfp_csr_type {
    NFP_CSR_TYPE_NONE = 0,
    NFP_CSR_TYPE_CPP,
    NFP_CSR_TYPE_MECSR,
};

/**
 * Access modes for CSR bitfields.
 */
enum nfp_csr_bf_mode {
    NFP_CSR_BF_NONE = 0,
    NFP_CSR_BF_RO = 1,
    NFP_CSR_BF_WO = 2,
    NFP_CSR_BF_RW = 3,
    NFP_CSR_BF_RESERVED,
};

/**
 * Specification of a single bitfield within a CSR.
 */
struct nfp_csr_bitfield {
    const unsigned char msb, lsb;
    const enum nfp_csr_bf_mode mode;
    const char *name;
};

/**
 * Specification of access properties for a CSR or CSR bitfield.  The
 * structure is filled out by passing it into the nfp_csr_lookup()
 * function.
 */
struct nfp_csr {
    /** Type of CSR. */
    enum nfp_csr_type type;

    /** Bitfield for CSR, or list of bitfields if complete CSR match.
     *
     * Bitfield or bitfield list is read-only.  The bitfield
     * structure(s) should under no circumstance be modified.  For
     * lists, the list is terminated when bf->mods == 0.
     */
    const struct nfp_csr_bitfield *bf;

    /** Offset of CSR within target. */
    unsigned long offset;

    /** True if matching complete CSR, false otherwise. */
    int complete_csr:1;

    union {
        /** Access spec for CPP target CSRs. */
        struct {
            /** CPP read action to access CSR. */
            unsigned char read_action;
            /** CPP write acction to access CSR. */
            unsigned char write_action;
            /** CPP target for CSR. */
            unsigned char target;
        } cpp;

        /** Access spec for microengine CSRs. */
        struct {
            /** ME number of CSR. */
            char menum;
            /** Context number of CSR (if applicable). */
            char ctx;
            /** Just align the members of the union to make
             * the initialiser clearer. */
            char dummy;
        } mecsr;
    } tgt;
};

__BEGIN_DECLS

/**
 * Find matching CSRs and bitfields
 * 
 * @param name          partial or full CSR name
 * @param matches       output buffer of strings
 * @param skipflags     skipflags
 *
 * Take @name parameter and find all matching CSRs and/or bitfields.
 * @name can either be a fully qualified CSR name (or bitfield within
 * a CSR), or it can be a prefix identifying a component.  Prefix
 * matching only matches the complete name up to a dot.  For example,
 * the name 'pcie' only matches the 'pcie' component.  It does not
 * match 'pcie_csr' or 'pcie_im'.
 *
 * By default all full CSR names, alternate short CSR names, and all
 * bitfields within CSRs are listed.  This list can be filtered using
 * @skipflags.  For example, setting %NFP_CSR_SKIP_ALTNAME will filter
 * out alternate names from the output, and %NFP_CSR_SKIP_BITFIELDS
 * will not give a list of each bitfield in the output (only the name
 * of the CSR itself).
 *
 * The matching names are returned in @matches which is a buffer
 * containing back-to-back strings.  The caller should perform free()
 * on the buffer when done.  Note that the @matches output paramters
 * is only valid if the return value indicates that any matches were
 * found.
 *
 * @return number of matches found or -1 on error (and set errno
 * accordingly).
 */
NFP_COMMON_API
int nfp_csr_matches(const char *name, char **matches, unsigned skipflags);

/**
 * Lookup CSR or bitfield and find its access properties
 *
 * @param name          full CSR or bitfield name
 * @param csr           CSR access properties
 *
 * Take @name parameter and fine the matching CSR or bitfield.  If
 * lookup is succcessful then fill in @csr with the CSR access
 * properties (e.g., CPP target, CSR offset, etc).  Note that the @csr
 * output parameter is only valid if the lookup matches exactly one
 * CSR or CSR bitfield.
 *
 * @return number of matches found or -1 on error (and set errno
 * accordingly).
 */
NFP_COMMON_API
int nfp_csr_lookup(const char *name, struct nfp_csr *csr);

__END_DECLS

#endif /* !__NFP_CSRS_H__ */
