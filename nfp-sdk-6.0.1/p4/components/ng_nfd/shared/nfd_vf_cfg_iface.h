/*
 * Copyright (C) 2016,  Netronome Systems, Inc.  All rights reserved.
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
 * @file          blocks/vnic/shared/nfd_vf_cfg_iface.h
 * @brief         Interface to per VF configuration for *ndo_set/get functions
 */
#ifndef _BLOCKS__SHARED_NFD_VF_CFG_IFACE_H_
#define _BLOCKS__SHARED_NFD_VF_CFG_IFACE_H_

#if defined(__NFP_LANG_MICROC) || defined (__NFP_LANG_ASM)
#include <nfd_user_cfg.h>
#endif

#define NFD_VF_CFG_ABI_VER      1

/* Field defines, supporting MAC and VLAN values in this ABI version */
#define NFD_VF_CFG_MAC_HI_fld   0, 31, 0
#define NFD_VF_CFG_MAC_HI_msk   0xffffffff
#define NFD_VF_CFG_MAC_HI_wrd   0
#define NFD_VF_CFG_MAC_LO_fld   1, 31, 16
#define NFD_VF_CFG_MAC_LO_msk   0xffff
#define NFD_VF_CFG_MAC_LO_sft   16
#define NFD_VF_CFG_MAC_LO_wrd   1
#define NFD_VF_CFG_MAC_SZ       6
#define NFD_VF_CFG_VLAN_fld     1, 15, 0
#define NFD_VF_CFG_VLAN_msk     0xffff
#define NFD_VF_CFG_VLAN_shf     0
#define NFD_VF_CFG_VLAN_wrd     1

#define NFD_VF_CFG_SZ           8
#define NFD_VF_CFG_OFF(_vf)     (NFD_VF_CFG_SZ * _vf)

#define NFD_VF_CFG_MAX_VFS_SZ           4


#if defined(__NFP_LANG_MICROC)
/* Expose NFD_MAX_VFS to the host */
#define NFD_VF_CFG_MAX_VFS                                    \
    __asm { .alloc_mem nfd_vf_cfg_max_vfs emem global 8 8 }   \
    __asm { .init nfd_vf_cfg_max_vfs NFD_MAX_VFS }

#define NFD_VF_CFG_BASE_IND2(_x, _abi) _pf##_x##_net_vf_cfg##_abi
#define NFD_VF_CFG_BASE_IND1(_x, _abi) NFD_VF_CFG_BASE_IND2(_x, _abi)
#define NFD_VF_CFG_BASE(_x) NFD_VF_CFG_BASE_IND1(_x, NFD_VF_CFG_ABI_VER)

/* MicroC declaration of per island symbol */
#define NFD_VF_CFG_DECLARE(_isl)                                    \
    __asm{ .alloc_mem NFD_VF_CFG_BASE(_isl) emem global             \
            (NFD_MAX_VFS * NFD_VF_CFG_SZ)                           \
            NFD_VF_CFG_SZ }

/* MicroC access to per island symbol */
#define NFD_VF_CFG_BASE_LINK_IND1(_sym)         \
    ((__emem char *) _link_sym(_sym))
#define NFD_VF_CFG_BASE_LINK_IND0(_isl)                 \
    NFD_VF_CFG_BASE_LINK_IND1(NFD_VF_CFG_BASE(_isl))
#define NFD_VF_CFG_BASE_LINK(_isl) NFD_VF_CFG_BASE_LINK_IND0(_isl)

/* MicroC access to per VF data address */
#define NFD_VF_CFG_ADDR(_base, _vf)           \
    ((_base) + NFD_VF_CFG_OFF(_vf))
#endif


#endif /* !_BLOCKS__SHARED_NFD_VF_CFG_IFACE_H_ */
