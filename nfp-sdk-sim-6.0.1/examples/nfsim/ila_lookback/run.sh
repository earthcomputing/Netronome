#!/bin/bash

#
# Copyright (C) 2014 Netronome Systems, Inc.  All rights reserved.
#

#
# Simple Interlaken look-aside example
# Single SIM loopback test does the following:
# * sets up MAC with macinit tool
# * sets up CSRs for ILA ACCEL operation
# * sets up BARs to access ILA CLS
# * write test values to CLS
# * start the ilkn loopback example
# * issue frontdoor CPP transaction with nfp-cpp tool to access remote memory
#   - a CPP transaction will traverse from the ILA island through the
#   - ilkn loopback app, back to the ILA island's CLS then back through the
#   - ilkn loopback and then finally out the ILA island again
#   - test values written to CLS will be reported back

if [ ! $SIMROOT ] ; then
    SIMROOT=../../../
fi
ETCDIR=$SIMROOT/etc
PATH=$SIMROOT/bin:$PATH

set -x
# Configure the MAC for ethernet on port 0 and ILKN on port 1 (LA must use port 1)
nfp-macinit -m0 -0$ETCDIR/macinit/1x100GE.json -1$ETCDIR/macinit/8xIL.json
# Tell the MAC to forward ILKN traffic from port 1 to the ILA block
nfp-reg xpb:Nbi0IsldXpbMap.NbiTopXpbMap.MacGlbAdrMap.MacCsr.MacMuxCtrl.LASelect=1

# Turn on ACCEL mode in the ILA block
nfp-reg xpb:Ila0IsldXpbMap.IlaXpbGlbAdrMap.IlaCsr.IlaCtrlCfg.IlaTxIdleCnt=255
nfp-reg xpb:Ila0IsldXpbMap.IlaXpbGlbAdrMap.IlaCsr.IlaCtrlCfg.IlaMode=1

# Set up BAR0 for outgoing xfers
nfp-reg ila:i48.IlaBarCfg.CppToIlaBar0.Address=0x0 #ila local cls
nfp-reg ila:i48.IlaBarCfg.CppToIlaBar0.IlaChSel=0x0

# Set up BAR0 for incoming xfers to point to CLS
nfp-reg ila:i48.IlaBarCfg.IlaToCppBar0.TargetID=15 #target cls
nfp-reg ila:i48.IlaBarCfg.IlaToCppBar0.Token=0
nfp-reg ila:i48.IlaBarCfg.IlaToCppBar0.LengthSelect=0

# write the test values
nfp-mem i48.cls:0 0xdead 0xbeef

# start the ilkn loopback example
$SIMROOT/examples/nfsim/ilkn_loopback > /tmp/ilkn_loopback.log & loopback_pid=$!

nfp-cpp -l8 -f 48@ila:0:0:0

kill $loopback_pid
