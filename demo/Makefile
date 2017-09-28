# Makefile for Earth Computing test harnesses
#
# Author : Mike Mian

# enable bash extensions for build shell
SHELL :=/bin/bash

# include headers for driver api
INCLUDE = .

CJSON = ../cJSON/
CJSON_SRC = ../cJSON/cJSON.c

SRC= tx_test_main.c 

OBJS = $(SRC: .c=.o)

TARGETS = tx_test

all: ${TARGETS}

#entl_signal_test: entl_signal_test_main.c
#	cc -I ${INCLUDE} -o $@ $?

#entl_test: entl_test_main.c
#	cc -I ${INCLUDE} -o $@ $?

#demo_window: demo_window.c
#	cc -pthread -lpthread -I ${INCLUDE} -o $@ $?

#demo_web: demo_window.c
#	cc -pthread -lpthread -I ${INCLUDE} -o $@ $?

#mm: mm.c
#	cc -pthread -lpthread -I ${INCLUDE} -o $@ $?

#demo_server_test: demo_server_test.c
#	cc -pthread -lpthread -I ${INCLUDE} -I ${CJSON} -o $@ $? ${CJSON_SRC}  -lm 

#demo_client: demo_client.c
#	cc -pthread -lpthread -I ${INCLUDE} -I ${CJSON} -o $@ $? ${CJSON_SRC}  -lm

tx_test: tx_test_main.c
	cc -pthread -lpthread -I ${INCLUDE} -o $@ $?

clean:
	rm ${TARGETS}
