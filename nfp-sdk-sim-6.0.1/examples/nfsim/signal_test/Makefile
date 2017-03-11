#
# Copyright (C) 2014 Netronome Systems, Inc.  All rights reserved.
#
# A simple egress packet API test:
# Load ucode which receives packets and reflects the count in a mailbox
# Packets are passed directly out to NBI0 port0 and dumped by the sim
# client application
#

# folders
INSTALL_ROOT ?= ../../..
ifdef NFPSDKDIR
NFP_TOOLCHAIN_DIR = $(NFPSDKDIR)
endif
NFP_TOOLCHAIN_DIR ?= /opt/netronome
EXDIR = $(INSTALL_ROOT)/examples/nfsim
BINDIR = $(INSTALL_ROOT)/bin
ETCDIR = $(INSTALL_ROOT)/etc

# ucode defines
PROJECT=signal_test
#UC_SRC=$(PROJECT).uc
C_SRC=$(PROJECT).c
LIST=$(PROJECT).list
ELF=$(PROJECT).elf
UC_INC=$(NFP_TOOLCHAIN_DIR)/components/standardlibrary/microcode/src/
UC_DEFS=-DNFP_LIB_ANY_NFAS_VERSION
UC_FLAGS=-chip nfp-6xxx
LDFLAGS=-chip nfp-6xxx

# sim client application
SIMAPP=run
SIMAPPSRC=run.c
C_INC=-I$(INSTALL_ROOT)/include
C_LIB=-L$(INSTALL_ROOT)/lib -lnfp -lpthread -lnfp_common

ENV=LD_LIBRARY_PATH=$(INSTALL_ROOT)/lib


# C Make part
app_src_dir     := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
NETRO_INSTALL_ROOT ?= ../../../..

# Define src_dir FIRST - it is the directory that this makefile resides in
# MUST OCCUR BEFORE ANY include's (which will change MAKEFILE_LIST)
ROOT_SRC_DIR  ?= $(NETRO_INSTALL_ROOT)/c_packetprocessing-master
base_dir        := $(ROOT_SRC_DIR)

#NETRONOME_DIR  ?= $(realpath $(app_src_dir)/../../..)
#top_dir        := $(base_dir)/..

microc_blocks_dir   := $(base_dir)/microc/blocks
microc_libs_dir     := $(base_dir)/microc/lib
microc_inc_dir      := $(base_dir)/microc/include
scripts_dir         := .

include $(scripts_dir)/Makefile.nfp.config
-include Makefile.nfp.config

#overwrite from Makefile.nfp.config
NFCC_BASE_INC := -I. -I$(base_dir)/microc/include -I$(base_dir)/microc/lib -I$(base_dir)/microc/lib/nfp  


FW_BUILD     := $(app_src_dir)
FW_FW        := $(app_src_dir)
include $(scripts_dir)/Makefile.templates

#
# Application definition starts here
#
#signal_test_obj.list: signal_test.c
    $(eval $(call micro_c.compile_with_rtl,signal_test_obj,signal_test.c,.))
    $(eval $(call micro_c.add_src_lib,signal_test_obj,me,$(base_dir)/microc/lib/nfp/_c))
    $(eval $(call fw.add_obj,signal_test,signal_test_obj,i32.me0 i32.me1))
    $(eval $(call fw.link_with_rtsyms,signal_test))

include $(scripts_dir)/Makefile.debug

pdefault: $(SIMAPP) chip_setup
	$(ENV) ./$(SIMAPP)

$(SIMAPP): $(SIMAPPSRC)
	gcc $(C_INC) -o $@ $< $(C_LIB)

$(ELF): $(LIST)
	$(NFP_TOOLCHAIN_DIR)/bin/nfld -elf $@ -u mei0.me0 $<

#$(LIST): $(UC_SRC)
#	$(NFP_TOOLCHAIN_DIR)/bin/nfas $(UC_DEFS) $(UC_FLAGS) -I$(UC_INC) $<

chip_setup: $(ELF)
	@echo "Running chip setup, this may take some time..."
	@echo "Enabling island 8,32 clocks only"
	#$(EXDIR)/load_me -b
	#$(EXDIR)/load_me -b -F -e 8,32 
	$(EXDIR)/nbidma_setup
	$(EXDIR)/nbitm_setup
	$(BINDIR)/nfp-macinit -m0 -0$(ETCDIR)/macinit/1x100GE.json -1$(ETCDIR)/macinit/1x100GE.json
	@echo "loading firmware"
	$(BINDIR)/nfp-nffw $(ELF) load

clean:
	rm -f *.elf *.list *.uci *.ucp $(SIMAPP)

