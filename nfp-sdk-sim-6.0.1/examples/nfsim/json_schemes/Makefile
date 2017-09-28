#
# Copyright (C) 2014 Netronome Systems, Inc.  All rights reserved.
#
SHELL=/bin/sh

#LIB_INC_PATH=../../lib
LIB_INC_PATH=/mnt/hgfs/atsushi/earthcomputing/Netronome/nfp-sdk-sim-6.0.1/lib

CC=gcc
CPP=g++

# simple source examples - handled generically
EX_FILES = nbitm_setup.c 

# multisource source examples - specifically handled
EX_APPS = interactive


EX_PROGS = $(addprefix ,$(EX_FILES:.c=))
EX_INC = -I../../../include -I../include
EX_FLAGS = -Wall -Werror -L${LIB_INC_PATH} -g -Wl,-rpath,'$$ORIGIN/../../lib' -DPACKAGE_NAME="\"nfp-sim-tools\"" -DPACKAGE_VERSION="\"beta\""
EX_LIBS = -L.. -lpcap -lnfp_hwdbg -lnfp -lrt -lnfp_common -ljansson -lz -lwjelement -lwjreader

#IA_FLAGS = -DUSE_READLINE
IA_LIBS = -lreadline

all: $(EX_APPS) $(EX_PROGS)

interactive: interactive.c intractv_client.c intractv_hwdbg.c
	$(CC) $(EX_FLAGS) $(IA_FLAGS) $(EX_INC) -pthread -o $@ $^ $(EX_LIBS) $(IA_LIBS)

%.cpp.o: %.cpp
	$(CPP) $(EX_FLAGS) $(EX_INC) -c -o $@ $<

%.c.o: %.c
	$(CC) $(EX_FLAGS) $(EX_INC) -c -o $@ $<

%: %.c $(NFPLIB_DIR) $(NFSALLIB_DIR) $(NFPCPPLIB_DIR)
	$(CC) $(EX_FLAGS) $(EX_INC) -pthread -o $@ $< $(EX_LIBS)

clean:
	find . -name "*.c.o" -print -delete
	find . -name "*.c~" -print -delete
	find . -name "*.h~" -print -delete
	rm -f Makefile~ core
	rm -f $(EX_PROGS) $(EX_APPS)

