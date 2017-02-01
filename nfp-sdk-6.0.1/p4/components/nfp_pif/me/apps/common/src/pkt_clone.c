/* Copyright (C) 2016,  Netronome Systems, Inc.  All rights reserved. */

#include <nfp.h>
#include <nfp_chipres.h>
#include <nfp/me.h>
#include <nfp/mem_bulk.h>
#include <nfp/cls.h>

#include <nfp6000/nfp_me.h>

#include <std/reg_utils.h>

#ifndef PIF_GLOBAL_NFD_DISABLED
#include <vnic/pci_out.h>
#include <vnic/utils/ordering.h>
#endif /* PIF_GLOBAL_NFD_DISABLED */

#include <blm.h>

#include <pif_pkt.h>
#include <pif_common.h>
#include <pif_counters.h>

#include "config.h"
#include "pkt_clone.h"
#include "mcast.h"

#define CTM_ALLOC_ERR 0xffffffff

/***************************************************************************
 * Packet clone                                                            *
 ***************************************************************************/

__import __shared __cls struct ctm_pkt_credits ctm_credits;  // configured in pkt_ops.c

#define PIF_8_BIT_XFR_LW     8
#define PIF_8_BIT_XFR_BYTES  ((PIF_8_BIT_XFR_LW) * 4)

#define PIF_32_BIT_XFR_LW     16
#define PIF_32_BIT_XFR_BYTES  ((PIF_32_BIT_XFR_LW) * 4)

/*
 * Copy source CTM packet in 8 X 32-bit chunks to destination CTM packet.
 * Copy the length of the CTM packet minus the PKT_NBI_OFFSET and PKT_MAC_PREPEND_BYTES.
 */
static void copy_ctm_packet(uint32_t source_ctm_pkt_num, uint32_t dest_ctm_pkt_num, uint32_t ctm_src_off, uint32_t ctm_src_byte_len)
{
    __mem __addr40 uint32_t     *source_ctm_ptr;
    __mem __addr40 uint32_t     *dest_ctm_ptr;
    __gpr uint32_t              chunk;
    __gpr uint32_t              i;
    SIGNAL                      sig;

    /* Copy 32-bit words whole words for bulk of packet */
    {
        __xread uint32_t        rd_buffer[PIF_32_BIT_XFR_LW];
        __xwrite uint32_t       wr_buffer[PIF_32_BIT_XFR_LW];
        __gpr uint32_t          to_copy_words = (ctm_src_byte_len/4);

        chunk = PIF_32_BIT_XFR_LW;
        while (to_copy_words)
        {
            if (to_copy_words < PIF_32_BIT_XFR_LW)
                chunk = to_copy_words;

            source_ctm_ptr = pkt_ctm_ptr40(__ISLAND, source_ctm_pkt_num, ctm_src_off);
            dest_ctm_ptr = pkt_ctm_ptr40(__ISLAND, dest_ctm_pkt_num, ctm_src_off);

            __mem_read32(rd_buffer, source_ctm_ptr, chunk << 2, PIF_32_BIT_XFR_BYTES, ctx_swap, &sig);
            reg_cp(wr_buffer, rd_buffer, sizeof(rd_buffer));
            __mem_write32(wr_buffer, dest_ctm_ptr, chunk << 2, PIF_32_BIT_XFR_BYTES, ctx_swap, &sig);

            to_copy_words -= chunk;
            ctm_src_off += (chunk << 2);
        }
    }

    /* Copy 8-bits for remainder of packet */
    {
        __xread uint32_t        rd_buffer[PIF_8_BIT_XFR_LW];
        __xwrite uint32_t       wr_buffer[PIF_8_BIT_XFR_LW];
        __gpr uint32_t          to_copy_bytes = (ctm_src_byte_len - ((ctm_src_byte_len/4) * 4));

        chunk = PIF_8_BIT_XFR_BYTES;

        while (to_copy_bytes)
        {
            if (to_copy_bytes < PIF_8_BIT_XFR_BYTES)
                chunk = to_copy_bytes;

            source_ctm_ptr = pkt_ctm_ptr40(__ISLAND, source_ctm_pkt_num, ctm_src_off);
            dest_ctm_ptr = pkt_ctm_ptr40(__ISLAND, dest_ctm_pkt_num, ctm_src_off);

            __mem_read8(rd_buffer, source_ctm_ptr, chunk, PIF_8_BIT_XFR_BYTES, ctx_swap, &sig);
            reg_cp(wr_buffer, rd_buffer, sizeof(rd_buffer));
            __mem_write8(wr_buffer, dest_ctm_ptr, chunk, PIF_8_BIT_XFR_BYTES, ctx_swap, &sig);

            to_copy_bytes -= chunk;
            ctm_src_off += chunk;
        }
    }

    return;
}


/*
 * Copy MU buffer packet in 64-bit chunks for byte_length.
 */
static void copy_mu_buffer(__mem __addr40 uint32_t *source_mu_ptr, __mem __addr40 uint32_t *dest_mu_ptr, uint32_t byte_length)
{
    __mem __addr40 uint32_t     *source_ptr;
    __mem __addr40 uint32_t     *dest_ptr;
    SIGNAL                      sig;
    __gpr uint32_t              chunk;
    __gpr uint32_t              offset = 0;

    /* Copy 32-bit words whole words for bulk of packet */
    {
        __xread uint32_t        rd_buffer[PIF_32_BIT_XFR_LW];
        __xwrite uint32_t       wr_buffer[PIF_32_BIT_XFR_LW];
        __gpr uint32_t          to_copy_words = (byte_length/4);

        chunk = PIF_32_BIT_XFR_LW;
        while (to_copy_words)
        {
            if (to_copy_words < PIF_32_BIT_XFR_LW)
                chunk = to_copy_words;

            source_ptr = (__mem __addr40 uint32_t *)((uint64_t)source_mu_ptr + offset);
            dest_ptr = (__mem __addr40 uint32_t *)((uint64_t)dest_mu_ptr + offset);

            __mem_read32(rd_buffer, source_ptr, chunk << 2, PIF_32_BIT_XFR_BYTES, ctx_swap, &sig);
            reg_cp(wr_buffer, rd_buffer, sizeof(rd_buffer));
            __mem_write32(wr_buffer, dest_ptr, chunk << 2, PIF_32_BIT_XFR_BYTES, ctx_swap, &sig);

            to_copy_words -= chunk;
            offset += (chunk << 2);
        }
    }

    /* Copy 8-bits for remainder of packet */
    {
        __xread uint32_t        rd_buffer[PIF_8_BIT_XFR_LW];
        __xwrite uint32_t       wr_buffer[PIF_8_BIT_XFR_LW];
        __gpr uint32_t          to_copy_bytes = (byte_length - ((byte_length/4) * 4));

        chunk = PIF_8_BIT_XFR_BYTES;
        while (to_copy_bytes)
        {
            if (to_copy_bytes < PIF_8_BIT_XFR_BYTES)
                chunk = to_copy_bytes;

            source_ptr = (__mem __addr40 uint32_t *)((uint64_t)source_mu_ptr + offset);
            dest_ptr = (__mem __addr40 uint32_t *)((uint64_t)dest_mu_ptr + offset);

            __mem_read8(rd_buffer, source_ptr, chunk, PIF_8_BIT_XFR_BYTES, ctx_swap, &sig);
            reg_cp(wr_buffer, rd_buffer, sizeof(rd_buffer));
            __mem_write8(wr_buffer, dest_ptr, chunk, PIF_8_BIT_XFR_BYTES, ctx_swap, &sig);

            to_copy_bytes -= chunk;
            offset += chunk;
        }
    }

    return;
}




struct pkt_clone_destination pkt_clone(uint32_t source_ctm_pkt_num, enum PKT_CTM_SIZE ctm_size, __mem __addr40 uint32_t *source_mu_ptr,
                                            uint32_t blq, uint32_t pkt_off, uint32_t pkt_len, uint32_t packet_split)
{

    struct pkt_clone_destination destination;

    /*
    * Poll for a CTM packet until one is returned.
    * Set alloc_internal to acquire ctm packet and buffer credits, before allocating CTM buffer.
    */

    destination.ctm_pkt_num = pkt_ctm_alloc(&ctm_credits, __ISLAND, ctm_size, 1, 0);

    while (destination.ctm_pkt_num == CTM_ALLOC_ERR)
    {
        sleep(BACKOFF_SLEEP);
        PIF_COUNT(ERROR_WAIT_CTM_PKT_ALLOC);
        destination.ctm_pkt_num = pkt_ctm_alloc(&ctm_credits, __ISLAND, ctm_size, 1, 0);
    }

    /*
     * Poll for MU buffer until one is returned.
     */
    {
        __xread blm_buf_handle_t    buf;

        while (blm_buf_alloc(&buf, blq) != 0)
        {
            sleep(BACKOFF_SLEEP);
            PIF_COUNT(ERROR_WAIT_BLM_BUF_ALLOC);
        }

        destination.mu_ptr = (uint64_t) blm_buf_handle2ptr(buf);
    }

    /*
     * Depending on packet length, copy only CTM portion or copy full CTM buffer and portion residing in MU buffer
     */
    {
        __gpr uint32_t  ctm_buffer_size = (256 << ctm_size);        // maximum CTM size

        if (!packet_split)
        {
            /*
             * CTM buffer size is bigger than packet size, only copy the CTM buffer
             * and no need to copy MU buffer as there is nothing there.
             */
            copy_ctm_packet(source_ctm_pkt_num, destination.ctm_pkt_num, pkt_off, pkt_len);

            return destination;
        }
        else
        {
             __gpr uint32_t  mu_len = 0;

            /*
             * Packet length is bigger than CTM buffer size, copy full CTM buffer size - pkt_off
             * and then copy rest of packet into MU buffer. Start of data in MU buffer is size of CTM buffer.
             */
            mu_len = pkt_len - (ctm_buffer_size - pkt_off);

            copy_mu_buffer(
                (__addr40 __mem uint32_t *)((uint64_t)source_mu_ptr + ctm_buffer_size),
                (__addr40 __mem uint32_t *)((uint64_t)destination.mu_ptr + ctm_buffer_size),
                mu_len
            );
            copy_ctm_packet(source_ctm_pkt_num, destination.ctm_pkt_num, pkt_off, ctm_buffer_size - pkt_off);

            return destination;
        }
    }

    return destination;
}



/***************************************************************************
 * Packet info state                                                       *
 ***************************************************************************/

extern __forceinline void save_pkt_info(__cls void *cls_ptr)
{
    __xwrite struct pif_pkt_info        wr_pkt_info;

    reg_cp(&wr_pkt_info, (void *)&pif_pkt_info_global, sizeof(pif_pkt_info_global));
    cls_write((__xwrite void *)&wr_pkt_info, (__cls void *)cls_ptr, sizeof(wr_pkt_info));
    return;
}

extern __forceinline void retrieve_pkt_info(__cls void *cls_ptr)
{
    __xread struct pif_pkt_info         rd_pkt_info;

    cls_read((__xread void *)&rd_pkt_info, (__cls void *)cls_ptr, sizeof(rd_pkt_info));
    reg_cp((void *)&pif_pkt_info_global, &rd_pkt_info, sizeof(pif_pkt_info_global));
    return;
}


/***************************************************************************
 * Parsed header                                                           *
 ***************************************************************************/

extern __forceinline void save_parrep(__lmem uint32_t *parrep, __cls uint32_t *cls_ptr, uint32_t word_length)
{
    __xwrite uint32_t   wr_buffer[4];
    __gpr uint32_t     i;

    /*
     * Copy in chunks of 4 32-bit words
     */
    for (i = 0; i < (word_length >> 2); i++)
    {
        reg_cp(wr_buffer,(void *)parrep, sizeof(wr_buffer));
        cls_write((__xwrite void *)wr_buffer, (__cls void *)cls_ptr, sizeof(wr_buffer));
        cls_ptr += 4;
        parrep += 4;
    }

    /*
     * Copy one 32-bit word at a time for the rest of the word_length
     */
    for (i = 0; i < (word_length - ((word_length >> 2) << 2)); i++)
    {
        reg_cp(wr_buffer,(void *)parrep, 1 << 2);
        cls_write((__xwrite void *)wr_buffer, (__cls void *)cls_ptr, 1 << 2);
        cls_ptr++;
        parrep++;
    }

    return;
}

extern __forceinline void retrieve_parrep(__lmem uint32_t *parrep, __cls uint32_t *cls_ptr, uint32_t word_length)
{
    __xread uint32_t   rd_buffer[4];
    __gpr uint32_t     i;

    /*
     * Copy in chunks of 4 32-bit words
     */
    for (i = 0; i < (word_length >> 2); i++)
    {
        cls_read((__xread void *)rd_buffer, (__cls void *)cls_ptr, sizeof(rd_buffer));
        reg_cp((void *)parrep, rd_buffer, sizeof(rd_buffer));
        cls_ptr += 4;
        parrep += 4;
    }

    /*
     * Copy one 32-bit word at a time for the rest of the word_length
     */
    for (i = 0; i < (word_length - ((word_length >> 2) << 2)); i++)
    {
        cls_read((__xread void *)rd_buffer, (__cls void *)cls_ptr, 1 << 2);
        reg_cp((void *)parrep, rd_buffer, 1 << 2);
        cls_ptr++;
        parrep++;
    }

    return;
}



