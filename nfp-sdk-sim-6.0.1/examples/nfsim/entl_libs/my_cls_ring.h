/*
 * Copyright (C) 2008-2014 Netronome Systems, Inc.  All rights reserved.
 * Copyright (C) 2017,  Earth Computing Inc.  All rights reserved.
 *
 *
 * @file          cls_ring.h
 * @brief         Copy of nfp_cls_ring.h which does not compile 
 *
 * Author:        Atsushi Kasuya
 *
 */

#ifndef _CLS_RING_H_
#define _CLS_RING_H_

#define NFP_CLS_RINGS_RING_BASE(x)                         (0x10000 + ((x) * 0x8))

/** Lock a Cluster Local Scratch memory ring for reordering
 *
 * @param ring_number       Ring number to read data from (0 to 15)
 * @param sequence_number   Sequence number to reorder on.
 * @param sync              Type of synchronization to use (sig_done or ctx_swap)
 * @param sig_ptr           Signal to raise upon completion
 *
 * Permits a ring to be locked for reordering. Compare sequence number with ring head pointer
 * on a specified ring. If match is found then push signal only, otherwise write the
 * signal information to the ring at offset specified by sequence number.
 *
 */
__intrinsic
void cls_ring_ordered_lock_ptr32(
    unsigned int ring_number,
    unsigned int sequence_number,
    sync_t sync,
    SIGNAL *sig_ptr
);


/** Lock a Cluster Local Scratch memory ring for reordering
 *
 * @param ring_number       Ring number to read data from (0 to 15)
 * @param sequence_number   Sequence number to reorder on.
 * @param island_id         Island id indicating CLS locality. See 6xxx databook for Local Scratch
 *                          recommended addressing mode.
 * @param sync              Type of synchronization to use (sig_done or ctx_swap)
 * @param sig_ptr           Signal to raise upon completion
 *
 * Permits a ring to be locked for reordering. Compare sequence number with ring head pointer
 * on a specified ring. If match is found then push signal only, otherwise write the
 * signal information to the ring at offset specified by sequence number.
 *
 */
__intrinsic
void cls_ring_ordered_lock_ptr40(
    unsigned int ring_number,
    unsigned int sequence_number,
    unsigned int island_id,
    sync_t sync,
    SIGNAL *sig_ptr
);

/** Unlock a Cluster Local Scratch memory ring after reordering
 *
 * @param ring_number       Ring number to read data from (0 to 15)
 * @param sequence_number   Sequence number
 *
 * Unlocks a ring after reordering. Increment the head pointer on the specified ring.
 * Read the ring at the new head pointer and overwrite with zero, if the data was
 * non-zero then push to the signal data that was read.
 *
 * @ind_6000
 */
__intrinsic
void cls_ring_ordered_unlock_ptr32(
    unsigned int ring_number,
    unsigned int sequence_number
);


/** Unlock a Cluster Local Scratch memory ring after reordering
 *
 * @param ring_number       Ring number to read data from (0 to 15)
 * @param sequence_number   Sequence number
 * @param island_id         Island id indicating CLS locality. See 6xxx databook for Local Scratch
 *                           recommended addressing mode.
 *
 * Unlocks a ring after reordering. Increment the head pointer on the specified ring.
 * Read the ring at the new head pointer and overwrite with zero, if the data was
 * non-zero then push to the signal data that was read.
 *
 * @ind_6000
 */
__intrinsic
void cls_ring_ordered_unlock_ptr40(
    unsigned int ring_number,
    unsigned int sequence_number,
    unsigned int island_id
);

/** Add a thread to a Cluster Local Scratch memory ring (queue)
 *
 * @param data          Work received from ring
 * @param ring_number   Ring number to read data from (0 to 15)
 * @param count         Number of 32-bit words to read (1 to 16)
 * @param sync          Type of synchronization to use (sig_done or ctx_swap)
 * @param sig_ptr       Signal to raise upon completion
 *
 * Add a thread to the work queue. Adding a thread to a queue that contains
 * work will get the first work and deliver it to the thread. If there are no
 * work on the queue, the thread is added to the ring.
 *
 */
__intrinsic
void cls_ring_workq_add_thread_ptr32(
    __xread void *data,
    unsigned int ring_number,
    unsigned int count,
    sync_t sync,
    SIGNAL *sig_ptr
);

/** Add a thread to a Cluster Local Scratch memory ring (queue)
 *
 * @param data          Work received from ring
 * @param ring_number   Ring number to read data from (0 to 15)
 * @param count         Number of 32-bit words to read (1 to 16)
 * @param island_id     Island id indicating CLS locality. See 6xxx databook for Local Scratch
 *                       recommended addressing mode.
 * @param sync          Type of synchronization to use (sig_done or ctx_swap)
 * @param sig_ptr       Signal to raise upon completion
 *
 * Add a thread to the work queue. Adding a thread to a queue that contains
 * work will get the first work and deliver it to the thread. If there are no
 * work on the queue, the thread is added to the ring.
 *
 */
__intrinsic
void cls_ring_workq_add_thread_ptr40(
    __xread void *data,
    unsigned int ring_number,
    unsigned int count,
    unsigned int island_id,
    sync_t sync,
    SIGNAL *sig_ptr
);

/** Add work to a Cluster Local Scratch memory ring (queue)
 *
 * @param data          Work written to ring
 * @param ring_number   Ring number to read data from (0 to 15)
 * @param count         Number of 32-bit words to read (1 to 16)
 * @param sync          Type of synchronization to use (sig_done or ctx_swap)
 * @param sig_ptr       Signal to raise upon completion
 *
 * Adding work to a queue that contains threads will get the first thread and
 * deliver the work to it. If there are no threads on the ring, the work is written
 * to the ring.
 *
 */
__intrinsic
void cls_ring_workq_add_work_ptr32(
    __xwrite void *data,
    unsigned int ring_number,
    unsigned int count,
    sync_t sync,
    SIGNAL *sig_ptr
);

/** Add work to a Cluster Local Scratch memory ring (queue)
 *
 * @param data          Work written to ring
 * @param ring_number   Ring number to read data from (0 to 15)
 * @param count         Number of 32-bit words to read (1 to 16)
 * @param island_id     Island id indicating CLS locality. See 6xxx databook for Local Scratch
 *                       recommended addressing mode.
 * @param sync          Type of synchronization to use (sig_done or ctx_swap)
 * @param sig_ptr       Signal to raise upon completion
 *
 * Adding work to a queue that contains threads will get the first thread and
 * deliver the work to it. If there are no threads on the ring, the work is written
 * to the ring.
 *
 */
__intrinsic
void cls_ring_workq_add_work_ptr40(
    __xwrite void *data,
    unsigned int ring_number,
    unsigned int count,
    unsigned int island_id,
    sync_t sync,
    SIGNAL *sig_ptr
);

__intrinsic
void cls_read_ring_ptr( __xread void *data, unsigned int ring_number ) ;

#endif /* _CLS_RING_H_ */

/* -*-  Mode:C; c-basic-offset:4; tab-width:4 -*- */
