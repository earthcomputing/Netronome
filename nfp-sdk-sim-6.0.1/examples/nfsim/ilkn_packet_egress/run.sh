#!/bin/bash
#
# Copyright (C) 2014 Netronome Systems, Inc.  All rights reserved.
#
# Load a pcap file into Simulator interlaken ingress and
# record the output a pcap output file
#

if [ ! $SIMROOT ] ; then
    SIMROOT=../../../
fi

EXDIR=$SIMROOT/examples/nfsim
INPCAP=$EXDIR/pcaps/ilkn_64.pcap
OUTPCAP=./ilkn_64_out

$EXDIR/ilkn_pcap_play -n0 -p0 -c0 $INPCAP

#run pcap record in background, save the Process ID
$EXDIR/ilkn_pcap_record -n0 -p0 $OUTPCAP & recpid=$!

#step 3000 simulator cycles, wait for packets to arrive
$EXDIR/step 3000

kill $recpid
