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
 * @file          app_master.c
 * @brief         Dummy code for ME serving as the NFD app master
 */

#include <assert.h>
#include <nfp_chipres.h>
#include <nfp.h>

#include <nfp/me.h>
#include <nfp/mem_bulk.h>

#include <nfp6000/nfp_me.h>

#ifndef PIF_GLOBAL_NFD_DISABLED
#include <vnic/shared/nfd_cfg.h>
#endif /* PIF_GLOBAL_NFD_DISABLED */

#include <system_init_c.h>

#ifndef PIF_GLOBAL_NFD_DISABLED
/* XXX make signals visible for all islands
 * Actually using the signals is WIP */
__visible SIGNAL nfd_cfg_sig_app_master0;
__visible SIGNAL nfd_cfg_sig_app_master1;
__visible SIGNAL nfd_cfg_sig_app_master2;
__visible SIGNAL nfd_cfg_sig_app_master3;

struct nfd_cfg_msg cfg_msg;

__xread unsigned int cfg_bar_data[6];
NFD_CFG_BASE_DECLARE(PCIE_ISL);
NFD_FLR_DECLARE;
#endif /* PIF_GLOBAL_NFD_DISABLED */

int
main(void)
{
    if (ctx() == 0) {
#ifndef PIF_GLOBAL_NFD_DISABLED
        nfd_cfg_init_cfg_msg(&nfd_cfg_sig_app_master0, &cfg_msg);
#endif /* PIF_GLOBAL_NFD_DISABLED */
        system_init_done();
    }

    for (;;) {
        if (ctx() == 0) {
#ifndef PIF_GLOBAL_NFD_DISABLED
            if (!cfg_msg.msg_valid) {
                nfd_cfg_master_chk_cfg_msg(&cfg_msg, &nfd_cfg_sig_app_master0,
                                           PCIE_ISL);

                if (cfg_msg.msg_valid) {
                    mem_read64(cfg_bar_data,
                               NFD_CFG_BAR_ISL(PCIE_ISL, cfg_msg.vnic),
                               sizeof cfg_bar_data);
                }
            } else {

                __xwrite unsigned int link_state;

                __implicit_read(cfg_bar_data, 6);

                local_csr_write(local_csr_mailbox_0, cfg_msg.vnic);
                local_csr_write(local_csr_mailbox_1, cfg_bar_data[0]);

                /* Set link state */
                if (!cfg_msg.error && (cfg_bar_data[NFP_NET_CFG_CTRL] &
                                       NFP_NET_CFG_CTRL_ENABLE)) {
                    link_state = NFP_NET_CFG_STS_LINK;
                } else {
                    link_state = 0;
                }
                mem_write32(&link_state,
                            (NFD_CFG_BAR_ISL(PCIE_ISL, cfg_msg.vnic) +
                             NFP_NET_CFG_STS),
                            sizeof link_state);

                /* Complete the message */
                cfg_msg.msg_valid = 0;
                nfd_cfg_app_complete_cfg_msg(PCIE_ISL, &cfg_msg,
                                             NFD_CFG_BASE_LINK(PCIE_ISL),
                                             &nfd_cfg_sig_app_master0);
            }
#endif /* PIF_GLOBAL_NFD_DISABLED */
            ctx_swap();
        } else
            ctx_swap();
    }
}
