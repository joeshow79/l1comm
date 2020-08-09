.PHONY: all

UV_LIB=$.lib/libuv.a
CFLAGS=-g -Wall -I./include -I./src -std=c++14 -O0
LIBS=

uname_S=$(shell uname -s)

ifeq (Linux, $(uname_S))
LIBS=-lrt -ldl -lm -pthread
endif

all: demo sender receiver

demo: sample/main.cc src/l1_receive_worker.cc src/l1_send_worker.cc
	g++ $(CFLAGS) -o $@ $^ $(UV_LIB) $(LIBS)
	# valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all

sender: sample/main.cc src/l1_receive_worker.cc src/l1_send_worker.cc
	g++ $(CFLAGS) -o $@ $^ $(UV_LIB) $(LIBS) -DENABLE_SENDER

receiver: sample/main.cc src/l1_receive_worker.cc src/l1_send_worker.cc
	g++ $(CFLAGS) -o $@ $^ $(UV_LIB) $(LIBS) -DENABLE_RECEIVER

simple_main: sample/simple_main.cc
	g++ $(CFLAGS) -o $@ $^ $(UV_LIB) $(LIBS)

