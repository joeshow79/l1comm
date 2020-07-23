.PHONY: all

UV_LIB=../lib/libuv.a
CFLAGS=-g -Wall -I../include -I.. -std=c++14
LIBS=

uname_S=$(shell uname -s)

ifeq (Linux, $(uname_S))
LIBS=-lrt -ldl -lm -pthread
endif

all: test_l1

test_l1: test_l1.cc
	g++ $(CFLAGS) -o $@ $^ $(UV_LIB) $(LIBS)
	./$@
	rm -rf ./$@
