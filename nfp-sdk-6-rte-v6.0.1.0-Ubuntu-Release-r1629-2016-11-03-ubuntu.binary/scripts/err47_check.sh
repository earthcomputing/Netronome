#!/bin/bash
#
# Copyright (C) 2007-2016 Netronome Systems, Inc.  All rights reserved.
#
# File:         err47_check.sh
# Description:  Determines whether the ERR47 workaround is installed on the system.
#               This script returns 0 if the test passes, 1 if the test fails and 2 if
#               the result is unknown.
#

INTERACTIVE_OPTION=${1:-"auto"}

LSPCI=$(which lspci 2>/dev/null)
SETPCI=$(which setpci 2>/dev/null)
LOGGER=$(which logger 2>/dev/null)

nfp_series=$(${LSPCI} -d 19ee: 2>/dev/null | grep -oE 'Device [0-9]+' | sed 's/.*\([0-9]\)000/\1/g' | head -1)
result=$(${SETPCI} -d 19ee:${nfp_series}000 0xFFC.L 2>/dev/null)

rc=2
for line in ${result}; do
  if [ "${line}" != "ffffffff" ]; then
    rc=1
  elif [ "${rc}" == "2" -a "${line}" == "ffffffff" ]; then
    rc=0
  fi
done

if [ "${INTERACTIVE_OPTION}" != "auto" -a "${rc}" != "0" ]; then
  if [ "${rc}" == "1" ]; then
    echo -e "\n *****\nThe 'ERR47' workaround was not detected on this system."
  else
    echo -e "\n *****\nUnable to determine whether the 'ERR47' workaround is installed on this system."
  fi
  echo -e "The ERR47 kernel patch is available from the Netronome Support site."
  echo -e "Please refer to the 'ERR47 Kernel Patch' section in the README file supplied with the NFP SDK6 RTE installation.\n"

  if [ -n "$BYPASS_ERR47_INTERACTIVE_CHECK" ]; then
    echo -e "Bypassing the interactive check for ERR47. System stability cannot be guaranteed."
    confirm="bypassed"
  else
    echo -e "Without the 'ERR47' workaround installed, the system may become unstable when reading the PCIe configuration space."
    echo -e "Are you sure you want to continue? Type 'yes' followed by the [ENTER] key to continue."
    read confirm
  fi

  if [ "${confirm}" == "yes" -o "${confirm}" == "Yes" -o "${confirm}" == "YES" -o "${confirm}" == "bypassed" ]; then
    rc=0
    if [ -n "${LOGGER}" ]; then
      ${LOGGER} -t "SDN 2.2" -p local0.warning "Confirmation to continue without ERR47 workaround in place [${confirm}]"
    fi
  fi
fi

exit ${rc}
