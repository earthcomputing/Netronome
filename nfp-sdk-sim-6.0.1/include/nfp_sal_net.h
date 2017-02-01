/*
 * Copyright (C) 2012-2013,  Netronome Systems, Inc.  All rights reserved.
 */

#ifndef __NFP_SAL_NET_H__
#define __NFP_SAL_NET_H__

#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/cdefs.h>

#include <nfp.h>

#include "_nfp_sal_apiex.h"


__BEGIN_DECLS;

/** @file nfp_sal_net.h
 *  @addtogroup nfsal NFSAL
 *  @{
 */

/** Types of stats for @a nfp_sal_packet_get_stat */
enum NFP_SAL_PACKET_STAT_TYPES {
    NFP_SAL_PACKET_STAT_PORT_RATE = 0, /**< Port rate in bps */
    NFP_SAL_PACKET_STAT_INGRESS_RATE,  /**< Current ingress rate in bps */
    NFP_SAL_PACKET_STAT_EGRESS_RATE,   /**< Current egress rate in bps */
    NFP_SAL_PACKET_STAT_INGRESS_PKTS,  /**< Number of packets injected */
    NFP_SAL_PACKET_STAT_INGRESS_BYTES, /**< Number of bytes injected */
    NFP_SAL_PACKET_STAT_EGRESS_PKTS,   /**< Number of packets retrieved */
    NFP_SAL_PACKET_STAT_EGRESS_BYTES,  /**< Number of bytes retrieved */
    /** Number of pause frame packets injected */
    NFP_SAL_PACKET_STAT_PAUSE_INGRESS_PKTS,
    /** Number of pause frame bytes injected */
    NFP_SAL_PACKET_STAT_PAUSE_INGRESS_BYTES,
    /** Number of pause frame packets retrieved */
    NFP_SAL_PACKET_STAT_PAUSE_EGRESS_PKTS,
    /** Number of pause frame bytes retrieved */
    NFP_SAL_PACKET_STAT_PAUSE_EGRESS_BYTES,
    NFP_SAL_PACKET_STAT_LAST,          /**< End of type indicator */
};

/** Types of packet events to enable and disable with
 *  @a nfp_sal_packet_event_enable/ @a nfp_sal_packet_event_disable
 */
enum PACKET_EVENT_TYPES {
    PACKET_EVENT_INGRESS_INJECTED = 0, /**< Ingress packet has been injected */
    PACKET_EVENT_EGRESS_READY,         /**< Egress packet has been retrieved */
    PACKET_EVENT_LAST,                 /**< End of type indicator */
};

/** Types of stats for @a nfp_sal_ilkn_get_stat */
enum NFP_SAL_ILKN_STAT_TYPES {
    NFP_SAL_ILKN_STAT_PORT_RATE = 0,  /**< Port rate in bps */
    NFP_SAL_ILKN_STAT_INGRESS_RATE,   /**< Current ingress rate in bps */
    NFP_SAL_ILKN_STAT_EGRESS_RATE,    /**< Current egress rate in bps */
    NFP_SAL_ILKN_STAT_INGRESS_PKTS,   /**< Number of packets injected */
    NFP_SAL_ILKN_STAT_INGRESS_BURSTS, /**< Number of bursts injected */
    NFP_SAL_ILKN_STAT_INGRESS_BYTES,  /**< Number of bytes injected */
    NFP_SAL_ILKN_STAT_EGRESS_PKTS,    /**< Number of packets retrieved */
    NFP_SAL_ILKN_STAT_EGRESS_BURSTS,  /**< Number of bursts retrieved */
    NFP_SAL_ILKN_STAT_EGRESS_BYTES,   /**< Number of bytes retrieved */
    NFP_SAL_ILKN_STAT_LAST,           /**< End of type indicator */
};

/** Types of ilkn packet events to enable and disable with
 *  nfp_sal_ilkn @a packet_event_enable/@a nfp_sal_ilkn packet_event_disable
 */
enum ILKN_EVENT_TYPES {
    ILKN_EVENT_INGRESS_INJECTED = 0, /**< Ingress ilkn packet has been injected */
    ILKN_EVENT_EGRESS_READY,         /**< Egress ilkn packet has been retrieved */
    ILKN_EVENT_LAST,                 /**< End of type indicator */
};

/** @name Functions for interacting with simulated NFP networking interfaces, both Ethernet and Interlaken
 *  @{
 */

/**
 * Retrieve an Ethernet packet statistic
 *
 * @param dev           NFP device
 * @param intf          The NFP interface to use
 * @param port          The interface port to use
 * @param type          The type of statistic
 * @param val           The retrieved value
 *
 * @returns 0 on success, or -1 on failure (sets errno accordingly)
 */
NFP_SAL_API
int nfp_sal_packet_get_stat(struct nfp_device *dev, int intf, int port, int type, uint64_t *val);

/**
 * Inject a packet into a simulated Ethernet port
 *
 * @param dev           NFP device
 * @param buffer        The packet data buffer to use
 * @param length        The length of the buffer
 * @param intf          The NFP interface to use
 * @param port          The interface port to use
 * @param time          The time to to start inputting the packet (ps)
 *
 * @returns 0 on success, or -1 on failure (sets errno accordingly)
 */
NFP_SAL_API
int nfp_sal_packet_ingress(struct nfp_device *dev, int intf, int port, const void *buffer, unsigned long long length, uint64_t time);

/**
 * Get the status of the ingress queue for a given Ethernet port
 *
 * @param dev           NFP device
 * @param intf          The NFP interface to use
 * @param port          The interface port to use
 *
 * @returns -1 on failure or the number of queued packets on success
 */
NFP_SAL_API
int nfp_sal_packet_ingress_status(struct nfp_device *dev, int intf, int port);

/**
 * Retrieve an egress packet from an Ethernet network port
 *
 * @param dev           NFP device
 * @param buffer        The packet data buffer to use
 * @param length        The length of the buffer
 * @param intf          The NFP interface to use
 * @param port          The interface port to use
 * @param time          The time the packet was received (ps)
 *
 * @returns The size of the packet in bytes, 0 if no packet retrieved
 *          or -1 on failure (sets errno accordingly)
 */
NFP_SAL_API
int nfp_sal_packet_egress(struct nfp_device *dev, int intf, int port, void *buffer, unsigned long long length, uint64_t *time);

/**
 * Get the status of the output queue for a given Ethernet port
 *
 * @param dev           NFP device
 * @param intf          The NFP interface to use
 * @param port          The interface port to use
 *
 * @returns -1 on failure or the number of queued packets on success
 */
NFP_SAL_API
int nfp_sal_packet_egress_status(struct nfp_device *dev, int intf, int port);

/**
 * Get the current MAC time in picoseconds
 *
 * @param dev           NFP device
 *
 * @returns the number of MAC clock tick since reset
 */
NFP_SAL_API
uint64_t nfp_sal_packet_get_time(struct nfp_device *dev);

/**
 * Enable Ethernet packet events of a certain type
 *
 * @param dev           NFP device
 * @param intf          The NFP interface to use
 * @param port          The interface port to use
 * @param type          The type of event
 *
 * @returns -1 on error, otherwise 0
 */
NFP_SAL_API
int nfp_sal_packet_event_enable(struct nfp_device *dev, int intf, int port, unsigned int type);

/**
 * Disable Ethernet packet events of a certain type
 *
 * @param dev          NFP device
 * @param intf         The NFP interface to use
 * @param port         The interface port to use
 * @param type         The type of event
 *
 * @returns -1 on error, otherwise 0
 */
NFP_SAL_API
int nfp_sal_packet_event_disable(struct nfp_device *dev, int intf, int port, unsigned int type);

/**
 * Enable breaking when the Ethernet ingress queue for a port reaches a level
 *
 * @param dev          NFP device
 * @param intf         The NFP interface to use
 * @param port         The interface port to use
 * @param level        The ingress queue level at and below which the simulator
 *                     will be stopped
 *
 * @returns -1 on error, otherwise 0
 */
NFP_SAL_API
int nfp_sal_packet_break_enable(struct nfp_device *dev, int intf, int port, unsigned int level);

/**
 * Disable breaking based on the Ethernet ingress queue level
 *
 * @param dev           NFP device
 * @param intf          The NFP interface to use
 * @param port          The interface port to use
 *
 * @returns -1 on error, otherwise 0
 */
NFP_SAL_API
int nfp_sal_packet_break_disable(struct nfp_device *dev, int intf, int port);

/**
 * Retrieve an interlaken manager statistic
 *
 * @param dev           NFP device
 * @param intf          The NFP interface to use
 * @param port          The interface port to use
 * @param type          The type of statistic
 * @param val           The retrieved value
 *
 * @returns 0 on success, or -1 on failure (sets errno accordingly)
 */
NFP_SAL_API
int nfp_sal_ilkn_get_stat(struct nfp_device *dev, int intf, int port, int type, uint64_t *val);

/**
 * Inject interlaken data into a simulated network port
 *
 * @param dev           NFP device
 * @param buffer        The data buffer to use
 * @param length        The length of the buffer
 * @param intf          The NFP interface to use
 * @param port          The interface port to use
 * @param time          The time to to start inputting the data (ps)
 *
 * @returns 0 on success, or -1 on failure (sets errno accordingly)
 */
NFP_SAL_API
int nfp_sal_ilkn_ingress(struct nfp_device *dev, int intf, int port, const void *buffer, unsigned long long length, uint64_t time);

/**
 * Retrieve egress interlaken data from a simulated ilkn port
 *
 * @param dev           NFP device
 * @param intf          The NFP interface to use
 * @param port          The interface port to use
 * @param data          The data buffer to use
 * @param length        The length of the buffer
 * @param time          The time the data was received (ps)
 *
 * @returns The size of the packet in bytes, 0 if no packet retrieved
 *          or -1 on failure (sets errno accordingly)
 */
NFP_SAL_API
int nfp_sal_ilkn_egress(struct nfp_device *dev, int intf, int port, void *data, unsigned long long length, uint64_t *time);

/**
 * Get the status of the output queue for a given ilkn port
 *
 * @param dev           NFP device
 * @param intf          The NFP interface to use
 * @param port          The interface port to use
 *
 * @returns -1 on failure or the number of queued entries on success
 */
NFP_SAL_API
int nfp_sal_ilkn_egress_status(struct nfp_device *dev, int intf, int port);

/**
 * Enable Interlaken events of a certain type
 *
 * @param dev           NFP device
 * @param intf          The NFP interface to use
 * @param port          The interface port to use
 * @param type          The type of event
 *
 * @returns -1 on error, otherwise 0
 */
NFP_SAL_API
int nfp_sal_ilkn_event_enable(struct nfp_device *dev, int intf, int port, unsigned int type);

/** @}
 * @}
 */

__END_DECLS;

#endif /* !__NFP_SAL_NET_H__ */
