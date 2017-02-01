/* Copyright (C) 2015,  Netronome Systems, Inc.  All rights reserved. */

#ifndef __PKT_OPS_H__
#define __PKT_OPS_H__

#include <pif_pkt.h>

enum infra_src {
    FROM_WIRE,
    FROM_HOST,
};

enum infra_dst {
    TO_WIRE,
    TO_HOST,
    TO_WIRE_DROP,
    TO_HOST_DROP,
};



/** pkt_ops_rx
 *
 * Receive a packet from either the HOST or WIRE (NBI)
 *
 * @param src            FROM_WIRE or FROM_HOST
 * @return               0 if rx successful otherwise -1
 */
int pkt_ops_rx(enum infra_src src);



/** pkt_ops_tx
 *
 * Transmit a packet to either HOST or WIRE (NBI)
 *
 * @param dst            TO_WIRE or TO_HOST
 * @param pkt_byteoffset Data is starting at this offset.
 * @return               0 if tx successful otherwise -1 if packet was dropped.
 */
int pkt_ops_tx(enum infra_dst dst, int pkt_byteoffset);


/** pkt_ops_tx_drop
 *
 * Drop a packet depending on the ingress port, either HOST or WIRE (NBI)
 * @return               always 0
 */
int pkt_ops_tx_drop(void);


#endif /* __PKT_OPS_H__ */
