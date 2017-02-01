#!/bin/bash
NETRONOME_DIR=${NETRONOME_DIR:-/opt/netronome}
NETRONOME_BIN=${NETRONOME_BIN:-${NETRONOME_DIR}/bin}

[ $# -lt 1 -o "$1" = "-h" ] && echo usage $0 GRO_CTX >&2 && exit 1

BLKS=`$NETRONOME_BIN/nfp-rtsym _gro_global_config | awk '{print $2}'`
BLKS=$(printf "%d" $BLKS)
CPB=`$NETRONOME_BIN/nfp-rtsym _gro_global_config | awk '{print $3}'`
CPB=$(printf "%d" $CPB)

echo GRO has $BLKS blocks and $CPB contexts per block

B=$(($1 / $CPB))
C=$(($1 % $CPB))

echo Context $1 is GRO context $B.$C

CBASE=$(($1 * 16))

EHI=$($NETRONOME_BIN/nfp-rtsym -l 4 _gro_cli_cntrs:$(($CBASE + 4)) | awk '{print $2}')
ELO=$($NETRONOME_BIN/nfp-rtsym -l 4 _gro_cli_cntrs:$(($CBASE + 0)) | awk '{print $2}')
ENQ=$(($EHI << 32 | $ELO))


RHI=$($NETRONOME_BIN/nfp-rtsym -l 4 _gro_cli_cntrs:$(($CBASE + 12)) | awk '{print $2}')
RLO=$($NETRONOME_BIN/nfp-rtsym -l 4 _gro_cli_cntrs:$(($CBASE + 8)) | awk '{print $2}')
REL=$(($RHI << 32 | $RLO))

echo MEs have enqueued $ENQ packets and released $REL packets on context $1

echo Bitmap for context $1:
$NETRONOME_BIN/nfp-rtsym _gro_bm_${B}_${C}

echo Queue for context $1:
$NETRONOME_BIN/nfp-rtsym _gro_q_${B}_${C}
