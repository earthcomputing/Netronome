/*
 * Copyright 2015 Netronome, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file          apps/nic/config.h
 * @brief         Infrastructure configuration for the NIC application.
 */

#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

#include <pif_design.h>

/*
 * PIF app configuration.
 *
 * Define PIF_APP_CONFIG as one of the following to configure the main pif app.
 * - PIF_APP_DEFAULT
 *    Use 4 contexts each for NBI and PCIe/NFD packet ingress.
 *    Use GRO for packet egress.
 * - PIF_APP_NBI_INGRESS_ONLY
 *    Use all contexts for NBI packet ingress.
 *    Do not use GRO for egress; all packets unsequenced.
 * - PIF_APP_PCIE_INGRESS_ONLY
 *    Use all contexts for PCIe/NFD packet ingress.
 *    Do not use GRO for egress; all packets unsequenced.
 * - PIF_APP_CUSTOM
 *    Do not override pre-defined configuration constants.
 * - PIF_APP_NBI_ONLY
 *    Use all contexts for NBI packets.
 *    Do not use NFD
 *
 * The following configuration constants can be set directly after defining
 * PIF_APP_CONFIG as PIF_APP_CUSTOM.
 *  - PIF_APP_NUM_NBI_CTX
 *      Number of threads to use for NBI packet ingress.
 *  - PIF_APP_NUM_PCIE_CTX
 *      Number of threads to use for PCIe/NFD packet ingress.
 *  - PIF_USE_GRO
 *      Toggles whether or not GRO used. If undefined NBI and PCIe/NFD packets
 *      are set as unsequenced and use sequencer 0.
 */

#define PIF_APP_DEFAULT             0
#define PIF_APP_NBI_INGRESS_ONLY    1
#define PIF_APP_PCIE_INGRESS_ONLY   2
#define PIF_APP_CUSTOM              3
#define PIF_APP_NBI_ONLY            4

#ifndef PIF_APP_CONFIG
    #ifdef PIF_GLOBAL_NFD_DISABLED
        #define PIF_APP_CONFIG PIF_APP_NBI_ONLY
    #else
        #define PIF_APP_CONFIG PIF_APP_DEFAULT
    #endif
#else
    #ifdef PIF_GLOBAL_NFD_DISABLED
        #undef PIF_APP_CONFIG
        #define PIF_APP_CONFIG PIF_APP_NBI_ONLY
    #endif
#endif

#if (PIF_APP_CONFIG == PIF_APP_NBI_INGRESS_ONLY)
    #define PIF_APP_NUM_NBI_CTX     8
    #define PIF_APP_NUM_PCIE_CTX    (8 - PIF_APP_NUM_NBI_CTX)
    #ifdef PIF_USE_GRO
        #undef PIF_USE_GRO
    #endif
#elif (PIF_APP_CONFIG == PIF_APP_PCIE_INGRESS_ONLY)
    #define PIF_APP_NUM_NBI_CTX     0
    #define PIF_APP_NUM_PCIE_CTX    (8 - PIF_APP_NUM_NBI_CTX)
    #ifdef PIF_USE_GRO
        #undef PIF_USE_GRO
    #endif
#elif (PIF_APP_CONFIG == PIF_APP_CUSTOM)
    #ifndef PIF_APP_NUM_NBI_CTX
        #define PIF_APP_NUM_NBI_CTX     4
    #endif
    #ifndef PIF_APP_NUM_PCIE_CTX
        #define PIF_APP_NUM_PCIE_CTX    (8 - PIF_APP_NUM_NBI_CTX)
    #endif
#elif (PIF_APP_CONFIG == PIF_APP_NBI_ONLY)
    #define PIF_APP_NUM_NBI_CTX     8
    #define PIF_APP_NUM_PCIE_CTX    (8 - PIF_APP_NUM_NBI_CTX)
    #define PIF_NO_NFD  1
#else /* (PIF_APP_CONFIG == PIF_APP_DEFAULT) */
    #define PIF_USE_GRO             1
    #define PIF_APP_NUM_NBI_CTX     4
    #define PIF_APP_NUM_PCIE_CTX    (8 - PIF_APP_NUM_NBI_CTX)
#endif

#ifdef PIF_GLOBAL_GRO_DISABLED
    #ifdef PIF_USE_GRO
        #undef PIF_USE_GRO
    #endif
#endif


/*
 * RX/TX configuration
 * - Set pkt offset the NBI uses
 * - Set the number of bytes the MAC prepends data into
 * - Configure RX checksum offload so the wire can validate checksums
 */
#define PKT_NBI_OFFSET           128
#define EN_MAC_PREPEND           1
#define PKT_MAC_PREPEND_BYTES    8
#define MAC_PREPEND_BYTES        8
#define HOST_PREPEND_BYTES       0
#define CFG_RX_CSUM_PREPEND

/*
 * General
 */

#define BACKOFF_SLEEP           256

/*
 * DCFL
 */

#define DCFL_SUPPORT_MATCH_FILTERING
#define DCFL_CUSTOM_TABLE_LW 4


/*
 * NBI configuration
 */
#define NBI_PKT_PREPEND_BYTES    MAC_PREPEND_BYTES
#define NBI_TM_NUM_SEQUENCERS    4
#define NBI_TM_ENABLE_SEQUENCER0 1
#define NBI_DMA_BP0_BLQ_TARGET   0,0
#define NBI_DMA_BP1_BLQ_TARGET   0,0
#define NBI_DMA_BP2_BLQ_TARGET   0,0
#define NBI_DMA_BP3_BLQ_TARGET   0,0
#define NBI_DMA_BP4_BLQ_TARGET   0,0
#define NBI_DMA_BP5_BLQ_TARGET   0,0
#define NBI_DMA_BP6_BLQ_TARGET   0,0
#define NBI_DMA_BP7_BLQ_TARGET   0,0
#define NBI0_DMA_BPE_CONFIG_ME_ISLAND0   1,256,127
#define NBI0_DMA_BPE_CONFIG_ME_ISLAND1   1,256,127
#define NBI0_DMA_BPE_CONFIG_ME_ISLAND2   1,256,127
#define NBI0_DMA_BPE_CONFIG_ME_ISLAND3   1,256,127
#define NBI0_DMA_BPE_CONFIG_ME_ISLAND4   1,256,127
#define NBI0_DMA_BPE_CONFIG_ME_ISLAND5   1,256,127
#define NBI0_DMA_BPE_CONFIG_ME_ISLAND6   1,256,127

#define NBI1_DMA_BPE_CONFIG_ME_ISLAND0   0,0,0
#define NBI1_DMA_BPE_CONFIG_ME_ISLAND1   0,0,0
#define NBI1_DMA_BPE_CONFIG_ME_ISLAND2   0,0,0
#define NBI1_DMA_BPE_CONFIG_ME_ISLAND3   0,0,0
#define NBI1_DMA_BPE_CONFIG_ME_ISLAND4   0,0,0
#define NBI1_DMA_BPE_CONFIG_ME_ISLAND5   0,0,0
#define NBI1_DMA_BPE_CONFIG_ME_ISLAND6   0,0,0

/*
 * NFD configuration - calculate the MAX NFD credit cache size per ME
 */
#define _ISL_RX(x,y,z)          x
#define ISL_RX(x)               _ISL_RX(x)

#define NFD_QUEUE_CREDITS       256
#define ME_PER_ISL              10
#define NUM_RX_ISL              (ISL_RX(NBI0_DMA_BPE_CONFIG_ME_ISLAND0) + \
                                 ISL_RX(NBI0_DMA_BPE_CONFIG_ME_ISLAND1) + \
                                 ISL_RX(NBI0_DMA_BPE_CONFIG_ME_ISLAND2) + \
                                 ISL_RX(NBI0_DMA_BPE_CONFIG_ME_ISLAND3) + \
                                 ISL_RX(NBI0_DMA_BPE_CONFIG_ME_ISLAND4) + \
                                 ISL_RX(NBI0_DMA_BPE_CONFIG_ME_ISLAND5) + \
                                 ISL_RX(NBI0_DMA_BPE_CONFIG_ME_ISLAND6))

#define CREDIT_BATCH            (NFD_QUEUE_CREDITS / (NUM_RX_ISL * ME_PER_ISL))

#define NFD_VNIC_VF


/*
 * MAC configuration
 */
#define MAC0_PORTS               0
#define MAC0_CHANNELS            3
#define MAC0_PORTS_LIST          MAC0_PORTS
#define MAC0_CHANNELS_LIST       MAC0_CHANNELS

/*
 * Flowcache config
 */
#ifndef NUM_IMU
#define FC_LB_IMU 1
#else
#define FC_LB_IMU NUM_IMU
#endif

#ifndef NUM_EMU
#define FC_LB_EMU 2
#else
#define FC_LB_EMU NUM_EMU
#endif

#define FC_ENABLE_OVERFLOW

/*
 * BLM configuration
 */
#define BLM_CUSTOM_CONFIG
#define BLM_BLQ_EMEM_TYPE emem

/*
 * GRO configuration
 */
#define GRO_NUM_BLOCKS           1
#define GRO_CTX_PER_BLOCK        8
#define GRO_REDUCED              0
#define GRO_ISL                  48
/* Configure which Traffic Manager sequencer GRO uses for NBI egress. */
#define GRO0_NBI_SEQR            1
#define GRO_NBI0                 0
#define GRO_CTX_NFD_BASE         5

/*
 * NFD configuration
 */
#define PCIE_ISL                0
#define PCI                     PCIE_ISL
#define NIC_PCI                 PCIE_ISL

/*
 * App master configuration
 */
#define APP_MASTER              ENABLE_CHANNEL_STATS

/* Enable debug counters */
#define CFG_NIC_LIB_DBG_CNTRS

/* For the VF specialisation */
#define NIC_INTF                0


#if defined(__NFP_LANG_MICROC)
struct port_accumulate_octets {
    unsigned long long rx_discards;
    unsigned long long rx_errors;
    unsigned long long rx_uc_octets;
    unsigned long long rx_mc_octets;
    unsigned long long rx_bc_octets;
    unsigned long long tx_discards;
    unsigned long long tx_errors;
    unsigned long long tx_uc_octets;
    unsigned long long tx_mc_octets;
    unsigned long long tx_bc_octets;
};
#endif

/* The missing stats in the MAC stats - must be accumulated by the app */
//__import __shared __emem struct port_accumulate_octets port_stats_extra[2][24] = {0};

#define PORT_STATS_INCR_IND(_pci, _port, _cntr) \
        mem_incr64(&port_stats_extra[_pci][_port].##_cntr)
#define PORT_STATS_INCR(_port, _cntr) \
    PORT_STATS_INCR_IND(NIC_PCI, _port, _cntr)

#define PORT_STATS_ADD_IND(_pci, _port, _cntr, _value) \
        mem_add64_imm(_value, &port_stats_extra[_pci][_port].##_cntr)
#define PORT_STATS_ADD(_port, _cntr, _value) \
    PORT_STATS_ADD_IND(NIC_PCI, _port, _cntr, _value)

#endif /* __APP_CONFIG_H__ */
