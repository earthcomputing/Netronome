/**
 * Copyright (C) 2014 Netronome Systems, Inc.  All rights reserved.
 *
 * Support functions for interactive hwdbg client
 */

#ifndef _INTRACTV_HWDBG_H
#define _INTRACTV_HWDBG_H

#include <sys/stat.h>
#include <sys/types.h>

#include <nfp_hwdbg.h>

extern struct command_data hwdbg_cmds[];

void hwdbg_event_handler(struct nfp_hwdbg *nfp_hwdbg,
                         uint32_t ev_type,
                         void *ev_data,
                         uint32_t ev_data_len);

#endif /* _INTRACTV_HWDBG_H */
