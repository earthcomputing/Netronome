/*
 * Copyright (C) 2014-2016,  Netronome Systems, Inc.  All rights reserved.
 *
 * @file          system_init_c.h
 * @brief         Interface for system init functions
 */

#ifndef _SYSTEM_INIT_C_H_
#define _SYSTEM_INIT_C_H_


#define ME_INIT_SIGNAL                  15  /* System init done signal */


void system_init_wait_done(void);

void system_init_done(void);

void system_reorder_contexts(void);

#endif //_SYSTEM_INIT_C_H_