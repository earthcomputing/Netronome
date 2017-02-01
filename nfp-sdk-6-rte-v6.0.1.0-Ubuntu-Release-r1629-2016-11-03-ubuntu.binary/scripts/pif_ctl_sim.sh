#!/bin/bash
# Copyright (C) 2015-2016 Netronome Systems, Inc.  All rights reserved.

set -e

# ENV VARS
#
# NBI_MAC8_JSON - MAC config for NBI 0
# NBI_MAC9_JSON - MAC config for NBI 1
# NBIDMACFGBIN  - path of nbi dma config binary
# NBITMCFGBIN   - path of nbi tm config binary

on_err () {
	echo "Error on line $1: err($2)"
	exit 1
}

trap 'on_err $LINENO $?' ERR

firmwarefile=
maccfgfile=
islandsinuse=

NETRODIR=${NETRODIR:-/opt/netronome}


load() {
    echo "Resetting Simulator"
    $NETRODIR/examples/nfsim/load_me -b

    if [ -n "$islandsinuse" ] ; then
        echo "Enabling island" $islandsinuse
        $NETRODIR/examples/nfsim/load_me -F
        $NETRODIR/examples/nfsim/load_me -e $islandsinuse
        $NETRODIR/examples/nfsim/load_me -s 30
    fi

    echo "Loading firmware"
    $NETRODIR/bin/nfp-nffw $firmwarefile load
    echo maccfg: $maccfgfile
    if [ -f "$maccfgfile" ] ; then
        >&2 echo "Doing mac init 0: " $maccfgfile
        $NETRODIR/bin/nfp-macinit -m 0 -0$maccfgfile
        if [ $? -ne 0 ]; then
            exit 1
        fi
    fi
    if [ -f "$NBI_MAC8_JSON" ] ; then
        >&2 echo "Doing mac init 0: " $NBI_MAC8_JSON
        $NETRODIR/bin/nfp-macinit -m 0 -0$NBI_MAC8_JSON
        if [ $? -ne 0 ]; then
            exit 1
        fi
    fi
    if [ -f "$NBI_MAC9_JSON" ] ; then
        >&2 echo "Doing mac init 0: " $NBI_MAC9_JSON
        $NETRODIR/bin/nfp-macinit -m 1 -0$NBI_MAC9_JSON
        if [ $? -ne 0 ]; then
            exit 1
        fi
    fi

    MELIST=`$NETRODIR/bin/nfp-rtsym -L |grep _parrep_0 | sed -e 's/\._parrep_0.*//'`
    echo "Detected PIF worker MES:"
    echo "$MELIST"

    for me in $MELIST; do
        #$NETRODIR/bin/nfp-reg mecsr:$me.Mailbox0=1
        $NETRODIR/bin/nfp-reg mecsr:i32.me0.Mailbox0=1
    done

    if [ -z "$NBIDMACFGBIN" ] ; then
      $NETRODIR/examples/nfsim/nbidma_setup
    fi
    if [ -z "$NBITMCFGBIN" ] ; then
      $NETRODIR/examples/nfsim/nbitm_setup
    fi

    # XXX: this is a hack to get this script to work
    #      only me island 0 will get used
    if [ -f "$NBIDMACFGBIN" ] ; then
        echo "Configuring NBI DMA 0"
        $NBIDMACFGBIN -n0 -I 32 -m 25
    fi
    if [ -f "$NBITMCFGBIN" ] ; then
        echo "Configuring NBI TM 0"
        $NBITMCFGBIN -n0
    fi
}

unload() {
    echo "Unloading"
    # nothing really to do here for sim
}

usage() {
    echo $"Usage:"
    echo -e "\t $0 {load|unload} <OPERATION ARGUMENTS> "
    echo -e "\t OPERATION ARGUMENTS:"
    echo -e "\t \t load [-f|--firmwarefile <path_to_fwfile/fwfile>]"
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
        -m|--maccfg)
        maccfgfile="$2"
        shift # past argument
        ;;
        -i|--islands)
        islandsinuse="$2"
        shift # past argument
        ;;
        *)
                # unknown option
        ;;
    esac
    shift # past argument or value
done

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


