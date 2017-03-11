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
NFP_TOOLCHAIN_DIR ?= $(INSTALL_ROOT)
EXDIR = $(INSTALL_ROOT)/examples/nfsim
BINDIR = $(INSTALL_ROOT)/bin
ETCDIR = $(INSTALL_ROOT)/etc

# ucode defines
PROJECT=ctmpacketegress
UC_SRC=$(PROJECT).uc
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

default: $(SIMAPP) chip_setup
	$(ENV) ./$(SIMAPP)

$(SIMAPP): $(SIMAPPSRC)
	gcc $(C_INC) -o $@ $< $(C_LIB)

$(ELF): $(LIST)
	$(NFP_TOOLCHAIN_DIR)/bin/nfld -elf $@ -u mei0.me0 $<

$(LIST): $(UC_SRC)
	$(NFP_TOOLCHAIN_DIR)/bin/nfas $(UC_DEFS) $(UC_FLAGS) -I$(UC_INC) $<

chip_setup: $(ELF)
	@echo "Running chip setup, this may take some time..."
	@echo "Enabling island 8,32 clocks only"
	$(EXDIR)/load_me -b
	$(EXDIR)/load_me -F -e 8,32
	$(EXDIR)/nbidma_setup
	$(EXDIR)/nbitm_setup
	$(BINDIR)/nfp-macinit -m0 -0$(ETCDIR)/macinit/1x100GE.json -1$(ETCDIR)/macinit/1x100GE.json
	@echo "loading firmware"
	$(BINDIR)/nfp-nffw $(ELF) load

clean:
	rm -f *.elf *.list *.uci *.ucp $(SIMAPP)

