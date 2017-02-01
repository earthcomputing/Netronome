#
# Copyright (C) 2014 Netronome Systems, Inc.  All rights reserved.
#
# Generic Makefile that can be in a directory that has the same directory
# name as a single .uc file to be assembled, and a .nffw generated to load
# into the NFP-6xxx SDK Simulator
SHELL = /bin/sh

# Override NFP_TOOLCHAIN_DIR with an envornment variable if the
# NFP SDK Toolchain is in a different location other than
# in ../../../..
# e.g. export NFP_TOOLCHAIN_DIR=/opt/netronome/nfp-sdk-5.0.0
#      or export NFPSDKDIR=/opt/netronome/nfp-sdk-5.0.0
INSTALL_ROOT=../../../..
ifndef NFPSDKDIR
  ifndef NFP_TOOLCHAIN_DIR
    NFP_TOOLCHAIN_DIR=$(INSTALL_ROOT)
  endif
else
    NFP_TOOLCHAIN_DIR=$(NFPSDKDIR)
endif

TOP ?= $(shell pwd)
# Get the .uc filename based on the directory name
PROJECT=$(lastword $(subst /, ,$(TOP)))
SRC=$(PROJECT).uc
LIST=$(PROJECT).list
NFFW=$(PROJECT).nffw
NFAS_BIN=$(NFP_TOOLCHAIN_DIR)/bin/nfas
NFLD_BIN=$(NFP_TOOLCHAIN_DIR)/bin/nfld
INC=$(NFP_TOOLCHAIN_DIR)/components/standardlibrary/microcode/src/
DEFS=-DNFP_LIB_ANY_NFAS_VERSION
LDFLAGS=-chip nfp-6xxx-b0 -g

# Define the ME list assignment
LD_LIST_ASSIGNMENT=-u mei0.me0 -l $(LIST) -mip -rtsyms

# 
$(NFFW): $(SRC)
	$(NFAS_BIN) $(DEFS) $(LDFLAGS) -I $(INC) $<
	$(NFLD_BIN) $(LDFLAGS) $(LD_LIST_ASSIGNMENT) -elf $(NFFW)

clean:
	rm -f *.nffw *.list *.mif *.mift *.uci *.ucp *~

