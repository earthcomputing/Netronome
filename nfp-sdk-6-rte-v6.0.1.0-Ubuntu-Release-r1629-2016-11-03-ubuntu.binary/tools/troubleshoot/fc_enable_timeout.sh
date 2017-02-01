#!/bin/bash
#
# Copyright (C) 2007-2016 Netronome Systems, Inc.  All rights reserved.
#
# File:        fc_enable_timeout.sh
# Description: Enables flow cache timeout (timeout is enabled by default).
#              A flow cache timeout per flow cache on EMU.
#

NETRONOME_BIN=${NETRONOME_BIN:-${NETRONOME_DIR}/bin}

for fc_timeout in $(${NETRONOME_BIN}/nfp-rtsym --list 2>&1 | grep fc_timeout_settings | awk '{print $1}' 2>/dev/null) ; do
    echo "Enable flow cache timeout " $fc_timeout
    ${NETRONOME_BIN}/nfp-rtsym $fc_timeout 0x00
    ret=$((ret + $?))
done