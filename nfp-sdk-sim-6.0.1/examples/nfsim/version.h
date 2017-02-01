/*
 * Copyright (C) 2010, Netronome Systems, Inc.  All rights reserved.
 *
 */

#ifndef VERSION_H
#define VERSION_H

#include <stdlib.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef PACKAGE_EXTRAVERSION
#define PACKAGE_EXTRAVERSION ""
#endif

/* Common 'usage' strings
 * Comment starts on column 31
 * Use spaces, not tabs.
 */

#define NFP_HELP \
    "  -n NUM, --nfp=NUM           select NFP device\n"

#ifdef CONFIG_REMOTE_NFP
#define REMOTE_HOST_HELP \
    "  -r HOST[:PORT], --remote-host=HOST[:PORT]\n" \
    "                              select NFP device on remote host\n"
#else
#define REMOTE_HOST_HELP ""
#endif

#define HELP_HELP \
    "  -h, -?, --help              show this help message and exit\n"

/* Helpers for the common version information */
#define VERSION_HELP \
	"  -V, --version               show revision of this utility\n"

#define VERSION_OPTION \
	{ .name = "version", .has_arg = 0, .flag = NULL, .val = 'V' }

#define VERSION_FLAG "V"

#define VERSION_PRINT(program) do {\
		printf("%s (%s) %s%s\n", \
		       program, PACKAGE_NAME, PACKAGE_VERSION,\
		       		PACKAGE_EXTRAVERSION); } while (0)

#define VERSION_CASE(program) \
	case 'V': VERSION_PRINT(program); exit(EXIT_SUCCESS); break;

#endif /* VERSION_H */
