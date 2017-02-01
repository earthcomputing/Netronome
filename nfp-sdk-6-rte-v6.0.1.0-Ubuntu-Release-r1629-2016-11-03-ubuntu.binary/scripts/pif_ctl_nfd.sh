#!/bin/bash
# Copyright (C) 2015-2016 Netronome Systems, Inc.  All rights reserved.

set -e

# ENV VARS
#
# LOAD_NETDEV - load the netdev driver
# NUM_VFS - number of VFs to create
# BASE_MAC - base MAC address for netdevs, #VF masked into lower word
# NBI_MAC8_JSON - MAC config for NBI 0
# NBI_MAC9_JSON - MAC config for NBI 1
# NBI_DMA8_JSON - DMA config for NBI 0
# NBI_DMA9_JSON - DMA config for NBI 1
# NBI_TM_JSON - TM config for NBI 0 and 1
# NFPSHUTILS - Location of nfp-shutils
# DISABLE_NFD - Set to "yes" to disable
# DETECT_MAC - Set to "yes" to detect the NFP MAC config and set links to platform_*.json files for NBI init

on_err () {
	echo "Error on line $1: err($2)"
	exit 1
}

trap 'on_err $LINENO $?' ERR

#
# Variables
#

# Loaded from command line
firmwarefile=

# Loaded from enrivonment
LOAD_NETDEV=${LOAD_NETDEV:-1}
NUM_VFS=${NUM_VFS:-4}
#BASE_MAC=${BASE_MAC:-"00:15:4d:00:00:"}
NETRODIR=${NETRODIR:-/opt/netronome}
NFPPIFDIR=${NFPPIFDIR:-/opt/nfp_pif}
NFPSHUTILS=${NFPSHUTILS:-$NFPPIFDIR/scripts/shared/nfp-shutils}
DISABLE_NFD=${DISABLE_NFD:-no}
DETECT_MAC=${DETECT_MAC:-yes}
#
# Functions
#

set_common_vars() {
  # Set BASE_MAC based on NFP_NUM
  BASE_MAC=${BASE_MAC:-"00:15:4d:0$NFP_NUM:00:"}

  PCI_IDS=`find /sys/bus/pci/drivers/nfp/0000* | cut -d':' -f2`
  #echo "PCI IDs found:$PCI_IDS"

  PCI_ID=-1
  for id in $PCI_IDS; do
    _NFP_NUM=`ls /sys/bus/pci/drivers/nfp/0000:$id:00.0/cpp/nfp-dev-cpp | cut -d'-' -f3`
    _MODEL=`$NETRODIR/bin/nfp-hwinfo -n$_NFP_NUM | grep 'assembly.model' | cut -d'=' -f2`
    _PARTNO=`$NETRODIR/bin/nfp-hwinfo -n$_NFP_NUM | grep 'partno' | cut -d'=' -f2`
    #echo "NFP_NUM:$_NFP_NUM PCI_ID:$id    $_PARTNO $_MODEL"
    if [ $NFP_NUM == $_NFP_NUM ]; then
      PCI_ID=$id
      #break
    fi
  done
  if [ $PCI_ID == -1 ]; then
    echo "No NFP card detected"
    exit 1
  fi

  PF_SYS="/sys/bus/pci/devices/0000:${PCI_ID}:00.0"

# Check for ARI support
  ARI_SUPPORT=$($NETRODIR/bin/nfp-support | grep ARI -m1)

  if [[ "$ARI_SUPPORT" == *"PASS"* ]]; then
    ARI=1
    echo "ARI Support detected"
  else
    ARI=0
    echo "No ARI Support found. VFs not supported."
  fi

  NBI0_PORT_MASK=0
  NBI1_PORT_MASK=0

  for PORTSPEC in `$NETRODIR/bin/nfp-phymod -n $NFP_NUM -E |grep "^eth" | sed -e 's/.*NBI//g' -e 's/(.*//'`; do
      PORT=`echo $PORTSPEC | sed -e 's/^.*\.//'`
      NBI=`echo $PORTSPEC | sed -e 's/\..*//'`
      if [ "$NBI" = "0" ]; then
          NBI0_PORT_MASK=$[NBI0_PORT_MASK | (1 << PORT)]
      else
          NBI1_PORT_MASK=$[NBI1_PORT_MASK | (1 << PORT)]
      fi
  done

  PLATFORM=`$NETRODIR/bin/nfp-hwinfo -n $NFP_NUM | grep assembly.model | sed 's/.*=//g'`
  VARIANT=`$NETRODIR/bin/nfp-mactool -n $NFP_NUM -p 0x1  2>&1 >/dev/null | sed -e 's/^.* \([^ ]*GE\).*/\1/'`
}

set_platform_vars() {

  PHY0_ETH_COUNT=`$NETRODIR/bin/nfp-phymod -n $NFP_NUM -i 0 2>&1 | grep eth | wc -l`
  PHY1_ETH_COUNT=`$NETRODIR/bin/nfp-phymod -n $NFP_NUM -i 1 2>&1 | grep eth | wc -l`

  PHY0_ETH_10G=`$NETRODIR/bin/nfp-phymod -n $NFP_NUM -i 0 2>&1 | grep eth | grep 10G | wc -l`
  PHY1_ETH_10G=`$NETRODIR/bin/nfp-phymod -n $NFP_NUM -i 1 2>&1 | grep eth | grep 10G | wc -l`

  NBI0_PORT_MASK=0
  NBI1_PORT_MASK=0

  for PORTSPEC in `$NETRODIR/bin/nfp-phymod -n $NFP_NUM -E |grep "^eth" | sed -e 's/.*NBI//g' -e 's/(.*//'`; do
      PORT=`echo $PORTSPEC | sed -e 's/^.*\.//'`
      NBI=`echo $PORTSPEC | sed -e 's/\..*//'`
      if [ "$NBI" = "0" ]; then
          NBI0_PORT_MASK=$[NBI0_PORT_MASK | (1 << PORT)]
      else
          NBI1_PORT_MASK=$[NBI1_PORT_MASK | (1 << PORT)]
      fi
  done
  
  NBI_TM_JSON=${NBI_TM_JSON:-nfp_nbi_tm_12x10GE.json}

  # default configs
  MAC8_CFG=hy-1x40GE-prepend.json
  DMA8_CFG=nfp_nbi8_dma_hy.json

  if [[ "$PLATFORM" == *"starfighter1"* ]] ; then
    DMA8_CFG=nfp_nbi8_dma_sf.json
    if [ "$PHY0_ETH_COUNT" -eq "1" -a "$PHY1_ETH_COUNT" -eq "0" ]; then
        MAC8_CFG=sf1-1x100GE-prepend.json
    elif [ "$PHY0_ETH_COUNT" -eq "10" -a "$PHY1_ETH_COUNT" -eq "0" ]; then
        MAC8_CFG=sf1-10x10GE-prepend.json
    elif [ "$PHY0_ETH_COUNT" -eq "2" -a "$PHY1_ETH_COUNT" -eq "0" ]; then
        MAC8_CFG=sf1-2x40GE-prepend.json
    elif [ "$PHY0_ETH_COUNT" -eq "1" -a "$PHY1_ETH_COUNT" -eq "1" ]; then
        MAC8_CFG=sf1-2x40GE-prepend.json
    elif [ "$PHY0_ETH_COUNT" -eq "4" -a "$PHY1_ETH_COUNT" -eq "4" ]; then
        MAC8_CFG=sf1-8x10GE-prepend.json
    elif [ "$PHY0_ETH_COUNT" -eq "4" -a "$PHY1_ETH_COUNT" -eq "1" ]; then
        MAC8_CFG=sf1-4x10GE-1x40GE-prepend.json
    else
        echo "Unrecognised starfighter variant $VARIANT"
        exit 1
    fi
  elif [ "$PLATFORM" = "hydrogen" ] ; then
    DMA8_CFG=nfp_nbi8_dma_hy.json
    if [ "$PHY0_ETH_COUNT" -eq "1" -a "$PHY1_ETH_COUNT" -eq "0" ]; then
        MAC8_CFG=hy-1x40GE-prepend.json
    elif [ "$PHY0_ETH_COUNT" -eq "4" -a "$PHY1_ETH_COUNT" -eq "0" ]; then
        MAC8_CFG=hy-4x10GE-prepend.json
    else
        echo "Unrecognised hydrogen variant $VARIANT"
        exit 1
    fi
  elif [ "$PLATFORM" = "lithium" ] ; then
    #DMA8_CFG=nfp_nbi8_dma_li.json
    if [ "$PHY0_ETH_10G" -eq "1" -a "$PHY1_ETH_10G" -eq "1" ]; then
        MAC8_CFG=li-2x10GE-prepend.json
    elif [ "$PHY0_ETH_10G" -eq "0" -a "$PHY1_ETH_10G" -eq "0" ]; then
        MAC8_CFG=li-2x1GE-prepend.json
    else
        echo "Unrecognised lithium variant $VARIANT"
        exit 1
    fi
  elif [ "$PLATFORM" = "beryllium" ] ; then
    #DMA8_CFG=nfp_nbi8_dma_be.json
    if [ "$PHY0_ETH_COUNT" -eq "1" -a "$PHY1_ETH_COUNT" -eq "0" ]; then
        MAC8_CFG=be-1x40GE-prepend.json
    elif [ "$PHY0_ETH_COUNT" -eq "4" -a "$PHY1_ETH_COUNT" -eq "0" ]; then
        MAC8_CFG=be-4x10GE-prepend.json
    elif [ "$PHY0_ETH_COUNT" -eq "1" -a "$PHY1_ETH_COUNT" -eq "1" ]; then
        MAC8_CFG=be-2x40GE-prepend.json
    elif [ "$PHY0_ETH_COUNT" -eq "4" -a "$PHY1_ETH_COUNT" -eq "1" ]; then
        MAC8_CFG=be-4x10GE-1x40GE-prepend.json
    elif [ "$PHY0_ETH_COUNT" -eq "4" -a "$PHY1_ETH_COUNT" -eq "4" ]; then
        MAC8_CFG=be-8x10GE-prepend.json
    else
        echo "Unrecognised beryllium variant $VARIANT"
        exit 1
    fi
  elif [ "$PLATFORM" = "bataan" ] ; then
    DMA8_CFG=nfp_nbi8_dma_cdp.json
    MAC8_CFG=cdp-1x100GE-2x10GE-3x40GE-prepend.json
  elif [ "$PLATFORM" = "carbon" ] ; then
    if [ "$PHY0_ETH_10G" -eq "1" -a "$PHY1_ETH_10G" -eq "1" ]; then
        MAC8_CFG=c-2x10GE-prepend.json
    elif [ "$PHY0_ETH_10G" -eq "0" -a "$PHY1_ETH_10G" -eq "0" ]; then
        MAC8_CFG=c-2x25GE-prepend.json
    else
        echo "Unrecognised carbon variant $VARIANT"
        exit 1
    fi
  else
    echo "Unrecognised platform $PLATFORM"
    exit 1
  fi

  # Setup links
  ##ln -sf "$NFPPIFDIR/etc/configs/$MAC8_CFG" "$NFPPIFDIR/etc/configs/platform_mac8_config.json"
  ##ln -sf "$NFPPIFDIR/etc/configs/$DMA8_CFG" "$NFPPIFDIR/etc/configs/platform_dma8_config.json"
  NBI_MAC8_JSON="$NFPPIFDIR/etc/configs/$MAC8_CFG"
  
  if [ $DEBUG_MODE -eq 1 ] ; then
    NBI_DMA8_JSON="$NFPPIFDIR/etc/configs/nfp_nbi8_dma_debug.json"
  else
    NBI_DMA8_JSON="$NFPPIFDIR/etc/configs/$DMA8_CFG"
  fi

  echo "Detected $PLATFORM platform, using $MAC8_CFG for MAC init"
  echo " and $DMA8_CFG for DMA init (when not in debug mode)"
}

nfd_post_load() {
    echo "Performing NFD post load operations:"
    if [ "$LOAD_NETDEV" == "1" ];then
        DRIVER="nfp_netvf"
        UNUSED_DRIVER="igb_uio"
    else
        DRIVER="igb_uio"
        UNUSED_DRIVER="nfp_netvf"
    fi

    # Load firmware
    if [ "$LOAD_NETDEV" != "1" ];then
        echo -n " - Create hugeTLB FS if needed..."
        if [ `cat /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages` -eq 0 ];
        then
            echo 0 > /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages
            (umount /mnt/hugetlbfs || true) 2>/dev/null
            mkdir -p /mnt/hugetlbfs
            mount -thugetlbfs none /mnt/hugetlbfs
            echo 1024 > /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages
        fi
        echo "done"
    fi

    # Load firmware
    echo -n " - Loading UIO and ${DRIVER} modules..."
    modprobe uio
    modprobe ${DRIVER}
    echo "done"

    echo -n " - Enumerating $NUM_VFS VFs..."
    echo $NUM_VFS > ${PF_SYS}/sriov_numvfs
    sleep 0.5
    echo "done"

    echo -n " - Make sure VFs bound to ${DRIVER}..."
    for i in $(seq 0 $NUM_VFS)
    do
        (echo -n "0000:${PCI_ID}:08.${i}" > \
            /sys/bus/pci/drivers/${UNUSED_DRIVER}/unbind || true) 2>/dev/null
        (echo -n "0000:${PCI_ID}:08.${i}" > \
            /sys/bus/pci/drivers/${DRIVER}/bind || true) 2>/dev/null
    done
    sleep 0.5
    if [ "$LOAD_NETDEV" == "0" ];then
       (echo -n 19ee 6003 > /sys/bus/pci/drivers/igb_uio/new_id || true) 2>/dev/null
    fi
    echo "done"

    # Set MAC and netdev name
    if [ "$LOAD_NETDEV" == "1" ];then
    echo -n " - Configure MAC addr and name..."
    sleep 2
    VF=0
    eths=`find /sys/bus/pci/devices/0000\:${PCI_ID}*/net -maxdepth 1 -mindepth 1 | sed -e 's#.*/##'`
    for eth in $eths; do
        MAC=`printf "%s%02x" $BASE_MAC $VF`
        ip l s dev $eth down
        ip l s dev $eth address $MAC
        ip l s dev $eth name vf${NFP_NUM}_${VF}
        ip l s dev vf${NFP_NUM}_${VF} up
        sysctl -w net.ipv6.conf.vf${NFP_NUM}_${VF}.disable_ipv6=1
        VF=$[VF + 1]
        sleep 0.5
    done
    echo "done"
    fi

    echo ""
}

nfd_pre_unload() {
    echo "Preparing for NFD unload:"

    echo -n " - Removing VFs..."
    echo 0 > ${PF_SYS}/sriov_numvfs
    sleep 0.5
    echo "done"

    echo -n " - Remove net_dev"
    #XXX Don't remove the modules
    #(rmmod nfp_netvf || true) 2>/dev/null
    #(rmmod igb_uio || true) 2>/dev/null
    #(rmmod nfp_net || true) 2>/dev/null
    sleep 1
    echo "done"
}

#
# Interface functions
#

load() {
    if [ "$DETECT_MAC" = "yes" ]; then
        set_platform_vars
    fi
    
    (. $NFPSHUTILS; appctl -n $NFP_NUM start $firmwarefile  )

    if [ "$DISABLE_NFD" = "no" ] && [ $ARI -eq 1 ]; then
        nfd_post_load
    fi

    # enable all pif application MEs
    # TODO: this may change

	if [ $DEBUG_MODE -eq 1 ]; then
        $NETRODIR/bin/nfp-reg -n $NFP_NUM mecsr:i32.me0.Mailbox0=2
    else
        MELIST=`$NETRODIR/bin/nfp-rtsym -n $NFP_NUM -L |grep _parrep_0 | sed -e 's/\._parrep_0.*//'`
        echo "Detected PIF worker MES:"
        echo "$MELIST"

        for me in $MELIST; do
            $NETRODIR/bin/nfp-reg -n $NFP_NUM mecsr:$me.Mailbox0=1
        done
    fi

    if [ "$PLATFORM" = "carbon" ] ; then
        # Start the Carbon flow control mechanism
        $NETRODIR/bin/nfp-reg -n $NFP_NUM mecsr:i1.me3.Mailbox1.Mailbox=0x40
        $NETRODIR/bin/nfp-reg -n $NFP_NUM mecsr:i1.me3.Mailbox0.Mailbox=0x25
    fi

    # enable mac interfaces
    if [ -n "$NBI_MAC8_JSON" ]; then
        $NETRODIR/bin/nfp-mactool -n $NFP_NUM -m0 -u -p $NBI0_PORT_MASK
        # enable mac poll/flush
        if $NETRODIR/bin/nfp-rtsym -n $NFP_NUM -L | grep -q 'pif_cfg_active_ports_nbi0'; then
            NBIO_ACTV_CFG=`$NETRODIR/bin/nfp-rtsym -n $NFP_NUM -L | grep -E 'pif_cfg_active_ports_nbi0' | sed -e 's/\s\+.\+//'`
            $NETRODIR/bin/nfp-rtsym -n $NFP_NUM $NBIO_ACTV_CFG $NBI0_PORT_MASK
        fi
    fi
    if [ -n "$NBI_MAC9_JSON" ]; then
        $NETRODIR/bin/nfp-mactool -n $NFP_NUM -m1 -u -p $NBI1_PORT_MASK
        # enable mac poll/flush
        if $NETRODIR/bin/nfp-rtsym -n $NFP_NUM -L | grep -q 'pif_cfg_active_ports_nbi1'; then
            NBI1_ACTV_CFG=`$NETRODIR/bin/nfp-rtsym -n $NFP_NUM -L | grep -E 'pif_cfg_active_ports_nbi1' | sed -e 's/\s\+.\+//'`
            $NETRODIR/bin/nfp-rtsym -n $NFP_NUM $NBI1_ACTV_CFG $NBI1_PORT_MASK
        fi
    fi
}

unload() {
    if [ "$DETECT_MAC" = "yes" ]; then
        set_platform_vars
    fi

    # Bring down mac interfaces
    if [ -n "$NBI_MAC8_JSON" ]; then
        # disable mac poll/flush
        if $NETRODIR/bin/nfp-rtsym -n $NFP_NUM -L | grep -q 'pif_cfg_active_ports_nbi0' 2> /dev/null ; then
            NBI0_ACTV_CFG=`$NETRODIR/bin/nfp-rtsym -n $NFP_NUM -L | grep -E 'pif_cfg_active_ports_nbi0' | sed -e 's/\s\+.\+//'`
            $NETRODIR/bin/nfp-rtsym -n $NFP_NUM $NBI0_ACTV_CFG 0x0
        fi
        # disable ports
        $NETRODIR/bin/nfp-mactool -n $NFP_NUM -m0 -d -p $NBI0_PORT_MASK || true
    fi
    if [ -n "$NBI_MAC9_JSON" ]; then
        # disable mac poll/flush
        if $NETRODIR/bin/nfp-rtsym -n $NFP_NUM -L | grep -q 'pif_cfg_active_ports_nbi0' 2> /dev/null ; then
            NBI1_ACTV_CFG=`$NETRODIR/bin/nfp-rtsym -n $NFP_NUM -L | grep -E 'pif_cfg_active_ports_nbi1' | sed -e 's/\s\+.\+//'`
            $NETRODIR/bin/nfp-rtsym -n $NFP_NUM $NBI1_ACTV_CFG 0x0
        fi
        # disable ports
        $NETRODIR/bin/nfp-mactool -n $NFP_NUM -m1 -d -p $NBI1_PORT_MASK || true
    fi

    if [ "$DISABLE_NFD" = "no" ] && [ $ARI -eq 1 ]; then
        nfd_pre_unload
    fi

    echo -n "Firmware unload, NFP Reset..."
    RET=0
    if ! (bash -c ". $NFPSHUTILS; appctl -n $NFP_NUM stop") ; then
        >&2 echo "Failed to unload NFP"
        RET=1
    fi
    exit $RET
}

usage() {
    echo $"Usage:"
    echo -e "\t $0 {load|unload} <OPERATION ARGUMENTS> "
    echo -e "\t OPERATION ARGUMENTS:"
    echo -e "\t \t load [-f|--firmwarefile <fwfile>]"
    echo -e "\t \t unload"
    echo $"When envoking load option a firmwarefile file needs to be specified"
}

while [ -n "$1" ]; do
    key="$1"
    
    case $key in
        load)
        OPERATION="load"
        ;;
        unload)
        OPERATION="unload"
        ;;
        -f|--firmwarefile)
        firmwarefile="$2"
        shift # past argument
        ;;
        -n|--nfp)
        NFP_NUM=$2
        shift # past argument
        ;;
        -d|--debug)
        DEBUG_MODE=1
        ;;
        *)
        # unknown option
        ;;
    esac
    shift # past argument or value
done

NFP_NUM=${NFP_NUM:=0}
DEBUG_MODE=${DEBUG_MODE:=0}

set_common_vars

case $OPERATION in
  load)
      if !( [ -f "$firmwarefile" ] ); then
          echo "Firmware file not specified"
          usage
          exit 2
      fi
      load
      ;;
  unload)
      unload
      ;;
   *)
      usage
      exit 2
esac


