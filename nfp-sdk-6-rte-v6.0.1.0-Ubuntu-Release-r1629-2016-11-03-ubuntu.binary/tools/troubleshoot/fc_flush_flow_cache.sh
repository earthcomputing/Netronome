#!/bin/bash
#
# Copyright (C) 2007-2016 Netronome Systems, Inc.  All rights reserved.
#
# File:        fc_flush _flow_cache.sh
# Description: Flush all entries from the flow cache table.
#

NETRONOME_BIN=${NETRONOME_BIN:-${NETRONOME_DIR}/bin}

for fc_timeout in $(${NETRONOME_BIN}/nfp-rtsym --list 2>&1 | grep fc_timeout_settings | awk '{print $1}' 2>/dev/null) ; do
    echo "Flush flow cache " $fc_timeout
    ${NETRONOME_BIN}/nfp-rtsym $fc_timeout 0xff
    ret=$((ret + $?))
done