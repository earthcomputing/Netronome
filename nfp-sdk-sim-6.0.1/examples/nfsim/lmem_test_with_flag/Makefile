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
PROJECT=lmem_test
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
# $(eval $(call micro_c.compile,entl_state_machine_obj,entl_state_machine.c,.))
include $(scripts_dir)/Makefile.templates

#
# Application definition starts here
#
#signal_test_obj.list: signal_test.c
$(eval $(call micro_c.compile_with_rtl,lmem_test_obj,lmem_test.c,.))
$(eval $(call micro_c.add_src_lib,lmem_test_obj,me,$(base_dir)/microc/lib/nfp/_c))
$(eval $(call fw.add_obj,lmem_test,lmem_test_obj,i32.me0 ))
$(eval $(call fw.link_with_rtsyms,lmem_test))

include $(scripts_dir)/Makefile.debug

#########################################################################
#
#  Host Test Code Build
#
#########################################################################

SHELL=/bin/sh

LIB_INC_PATH=../../../lib

CC=gcc
CPP=g++

HOST_EXEC_C = 	entl_test_run.c
HOST_SUB_C = entl_state_machine.c

HOST_EXEC = $(addprefix ,$(HOST_EXEC_C:.c=))
HOST_INC = -I../../../include -I../../include -I.
HOST_FLAGS = -Wall -L${LIB_INC_PATH} -g -Wl,-rpath,'$$ORIGIN/../../../lib' -DPACKAGE_NAME="\"nfp-sim-tools\"" -DPACKAGE_VERSION="\"beta\""
HOST_LIBS = -lpcap -lnfp_hwdbg -lnfp -lrt -lnfp_common -ljansson -lz

# Compile Switch for Host code
HOST_FLAGS+= -DNETRONOME_HOST 

lmem_test_run: lmem_test_run.c 
	$(CC) $(HOST_FLAGS) $(HOST_INC) -pthread -o $@ $^ $(HOST_LIBS) 

%.cpp.o: %.cpp
	$(CPP) $(HOST_FLAGS) $(HOST_INC) -c -o $@ $<

%.c.o: %.c
	$(CC) $(HOST_FLAGS) $(HOST_INC) -c -o $@ $<

%: %.c $(NFPLIB_DIR) $(NFSALLIB_DIR) $(NFPCPPLIB_DIR)
	$(CC) $(EX_FLAGS) $(EX_INC) -pthread -o $@ $< $(EX_LIBS)

clean:
	find . -name "*.c.o" -print -delete
	find . -name "*.c~" -print -delete
	find . -name "*.h~" -print -delete
	rm -f Makefile~ core
	rm -f $(HOST_EXEC)




