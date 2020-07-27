.PHONY: all

UV_LIB=$.lib/libuv.a
CFLAGS=-g -Wall -I./include -I./src -std=c++14
LIBS=

uname_S=$(shell uname -s)

ifeq (Linux, $(uname_S))
LIBS=-lrt -ldl -lm -pthread
endif

all: demo

simple_main: sample/simple_main.cc
	g++ $(CFLAGS) -o $@ $^ $(UV_LIB) $(LIBS)

demo: sample/main.cc src/l1_receive_worker.cc
	g++ $(CFLAGS) -o $@ $^ $(UV_LIB) $(LIBS)
