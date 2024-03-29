/* Copyright (C) 2015,  Netronome Systems, Inc.  All rights reserved. */

PIF_COUNTER(RX)
PIF_COUNTER(RX_NFD)
PIF_COUNTER(TX)
PIF_COUNTER(TX_NFD)
PIF_COUNTER(DROP)
PIF_COUNTER(DROP_META)
PIF_COUNTER(DROP_NFD_NO_CREDITS)
PIF_COUNTER(DROP_MCAST_PORT_OUT_BOUNDS)
PIF_COUNTER(DROP_PORT_PAUSED)
PIF_COUNTER(FLOWCACHE_HIT)
PIF_COUNTER(FLOWCACHE_MISS)
PIF_COUNTER(FLOWCACHE_BYPASS)
PIF_COUNTER(MULTICAST)
PIF_COUNTER(RESUBMIT)
PIF_COUNTER(RECIRCULATE)
PIF_COUNTER(CLONE_I2I)
PIF_COUNTER(CLONE_I2E)
PIF_COUNTER(CLONE_E2I)
PIF_COUNTER(CLONE_E2E)
PIF_COUNTER(ERROR_PKTMETA)
PIF_COUNTER_ARRAY(ERROR_PARREP, PIF_PARSE_ERROR_LAST)
PIF_COUNTER(ERROR_PARREP_CUSTOM)
PIF_COUNTER(ERROR_FLOWCACHE)
PIF_COUNTER(ERROR_INGRESS)
PIF_COUNTER(ERROR_EGRESS)
PIF_COUNTER(ERROR_DEPARSE)
PIF_COUNTER(ERROR_WAIT_CTM_PKT_ALLOC)
PIF_COUNTER(ERROR_WAIT_BLM_BUF_ALLOC)
PIF_COUNTER(ERROR_NFDMETA)
