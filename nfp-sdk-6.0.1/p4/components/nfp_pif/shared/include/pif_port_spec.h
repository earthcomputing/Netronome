/*
 * Definition for interpreting ingress/egress port values for ME and host
 *
 * Copyright (C) 2016,  Netronome Systems, Inc.  All rights reserved.
 */

#ifndef __PIF_PORT_SPEC_H__
#define __PIF_PORT_SPEC_H__

/*
 * Type of port for egress spec or egress port
 */
enum pif_port_spec_type {
    /* Physical port */
    PIF_PORT_SPEC_TYPE_PHYSICAL = 0,
    /* Logical port, actual mapping determined by app */
    PIF_PORT_SPEC_TYPE_LOGICAL,
    /* Multicast, a mapping to multiple physical or host ports */
    PIF_PORT_SPEC_TYPE_MULTICAST,
    /* A host/PCIE port */
    PIF_PORT_SPEC_TYPE_HOST
};

/* for now PCIE instance is not supported */
#define PIF_PORT_SPEC_HOST_PORT(_vf, _pcie) (_vf)

#define PIF_PORT_SPEC_CHAN_of(_pspec) (((_pspec) >> 10) & 0x3f)
#define PIF_PORT_SPEC_TYPE_of(_pspec) (((_pspec) >> 8) & 0x3)
#define PIF_PORT_SPEC_PORT_of(_pspec) (((_pspec) >> 0) & 0xff)

#define PIF_PORT_SPEC(_port, _type, _chan) \
                ((_port & 0xff) | \
                ((_type & 0x3) << 8) | \
                ((_chan & 0x3f) << 10))

#define PIF_PORT_SPEC_DROP ((-1) & 0xffff)

#endif /* __PIF_PORT_SPEC_H__ */
