/*
 * Copyright (C) 2014-2016,  Netronome Systems, Inc.  All rights reserved.
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
 * @file          blocks/vnic/shared/nfd_cfg_pf_bars.uc
 * @brief         Microcode access to CFG BAR defines
 */

#ifndef _BLOCKS__SHARED_NFD_CFG_PF_BARS_UC_
#define _BLOCKS__SHARED_NFD_CFG_PF_BARS_UC_

#include "nfd_user_cfg.h"
#include <nfp_net_ctrl.h>

#ifndef NFD_MAX_PFS
#error "NFD_MAX_PFS is not defined but is required"
#endif

#define_eval NFD_TOTAL_VNICS (NFD_MAX_VFS + NFD_MAX_PFS)
#define_eval NFD_CFG_BAR_SZ (NFD_TOTAL_VNICS * NFP_NET_CFG_BAR_SZ)
#define_eval NFD_CFG_BAR0_OFF (NFD_MAX_VFS * NFP_NET_CFG_BAR_SZ)


#macro nfd_define_pf_bars(_isl)
.alloc_mem nfd_cfg_base/**/_isl NFD_PCIE/**/_isl/**/_EMEM global \
    NFD_CFG_BAR_SZ 0x200000
#if NFD_MAX_PFS != 0
.declare_resource nfd_cfg_base/**/_isl/**/_res global NFD_CFG_BAR_SZ \
    nfd_cfg_base/**/_isl
.alloc_resource _pf/**/_isl/**/_net_bar0 \
    nfd_cfg_base/**/_isl/**/_res+NFD_CFG_BAR0_OFF global \
    (NFD_MAX_PFS * NFP_NET_CFG_BAR_SZ)
.alloc_mem nfd_cfg_pf/**/_isl/**/_num_ports emem global 8 8
.init nfd_cfg_pf/**/_isl/**/_num_ports NFD_MAX_PFS
#endif
#endm


#ifdef NFD_PCIE0_EMEM
nfd_define_pf_bars(0)
#endif

#ifdef NFD_PCIE1_EMEM
nfd_define_pf_bars(1)
#endif

#ifdef NFD_PCIE2_EMEM
nfd_define_pf_bars(2)
#endif

#ifdef NFD_PCIE3_EMEM
nfd_define_pf_bars(3)
#endif

#undef NFD_TOTAL_VNICS
#undef NFD_CFG_BAR_SZ
#undef NFD_CFG_BAR0_OFF

#endif /* !_BLOCKS__SHARED_NFD_CFG_PF_BARS_UC_ */
