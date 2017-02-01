/**
 * Copyright (C) 2014 Netronome Systems, Inc.  All rights reserved.
 *
 * Support functions for interactive client
 */
#ifndef INTRACTV_CLIENT_H
#define INTRACTV_CLIENT_H

/* Simulator event handler */
void client_event_handler(struct nfp_device *dev,
                          uint32_t ev_type,
                          void *ev_data,
                          uint32_t ev_data_len);

extern struct command_data client_cmds[];

#endif
