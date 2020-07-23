.PHONY: all

UV_LIB=$.lib/libuv.a
CFLAGS=-g -Wall -I./include -std=c++14
LIBS=

uname_S=$(shell uname -s)

ifeq (Linux, $(uname_S))
LIBS=-lrt -ldl -lm -pthread
endif

all: sample

simple_main: simple_main.cc
	g++ $(CFLAGS) -o $@ $^ $(UV_LIB) $(LIBS)

sample: main.cc l1receiverworker.cc
	g++ $(CFLAGS) -o $@ $^ $(UV_LIB) $(LIBS)
