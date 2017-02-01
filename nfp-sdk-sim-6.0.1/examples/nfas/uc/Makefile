#
# Copyright (C) 2014 Netronome Systems, Inc.  All rights reserved.
#
# Generic Makefile to recurse into sub-directories to run make
SHELL=/bin/bash

all:
	@for a in $$(ls); do \
		if [ -d $$a ]; then \
			echo "processing folder $$a"; \
			$(MAKE) -s -C $$a; \
		fi; \
	done;
	@echo "Done!"

clean:
	@for a in $$(ls); do \
		if [ -d $$a ]; then \
			echo "processing folder $$a"; \
			$(MAKE) -s -C $$a clean; \
		fi; \
	done;
	@echo "Done!"
