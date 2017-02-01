#!/bin/bash

netdev_if=`cat /sys/module/nfp_fallback/control/ctrl_netdev`

echo "ifconfig $netdev_if"
ifconfig $netdev_if
echo
echo "nfp-rtsym nfd_cfg_base0; sleep 1; nfp-rtsym nfd_cfg_base0;"
nfp-rtsym nfd_cfg_base0; sleep 1; nfp-rtsym nfd_cfg_base0;
echo
echo "nfp-reg pcie:i4.PcieInternalTargets.Queue{0..255}; sleep 1; nfp-reg pcie:i4.PcieInternalTargets.Queue{0..255}"
nfp-reg pcie:i4.PcieInternalTargets.Queue{0..255}; sleep 1; nfp-reg pcie:i4.PcieInternalTargets.Queue{0..255}
echo
