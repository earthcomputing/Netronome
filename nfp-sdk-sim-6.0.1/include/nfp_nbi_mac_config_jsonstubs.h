/**
 * Copyright (C) 2013-2014 Netronome Systems, Inc.  All rights reserved.
 *
 * nfp6000 MAC initialization
 *
 * This file was automatically generated by jansson_stubs_gen.py.
 * Do not edit by hand.
 */
#ifndef __NFP_NBI_MAC_CONFIG_JSONSTUBS__
#define __NFP_NBI_MAC_CONFIG_JSONSTUBS__

#include <jansson.h>

#define NFP_NBI_MAC_CONFIG_MAX_KEY 29

int nfp_nbi_mac_chan_config_jinit(json_t *jobj, struct nfp_nbi_mac_chan_config *config, int core);
int nfp_nbi_mac_enet_config_jinit(json_t *jobj, struct nfp_nbi_mac_enet_config *config);
int nfp_nbi_mac_ilk_tx_config_jinit(json_t *jobj, struct nfp_nbi_mac_ilk_tx_config *config);
int nfp_nbi_mac_ilk_rx_config_jinit(json_t *jobj, struct nfp_nbi_mac_ilk_rx_config *config);
int nfp_nbi_mac_ilk_config_jinit(json_t *jobj, struct nfp_nbi_mac_ilk_config *config);
int nfp_nbi_mac_serdes_config_jinit(json_t *jobj, struct nfp_nbi_mac_serdes_config *config);

#endif /* __NFP_NBI_MAC_CONFIG_JSONSTUBS__ */
