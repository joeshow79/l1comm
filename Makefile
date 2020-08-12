.PHONY: all clean

UV_LIB=$.lib/libuv.a
CFLAGS=-g -Wall -I./include -I./src -std=c++14 -O0
LIBS=

uname_S=$(shell uname -s)

ifeq (Linux, $(uname_S))
LIBS=-lrt -ldl -lm -pthread
endif

all: demo sender receiver demo_random sender_random receiver_random

demo: sample/main.cc src/l1_receive_worker.cc src/l1_send_worker.cc
	g++ $(CFLAGS) -o $@ $^ $(UV_LIB) $(LIBS) -DENABLE_SENDER -DENABLE_RECEIVER
	# valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all

sender: sample/main.cc src/l1_receive_worker.cc src/l1_send_worker.cc
	g++ $(CFLAGS) -o $@ $^ $(UV_LIB) $(LIBS) -DENABLE_SENDER

receiver: sample/main.cc src/l1_receive_worker.cc src/l1_send_worker.cc
	g++ $(CFLAGS) -o $@ $^ $(UV_LIB) $(LIBS) -DENABLE_RECEIVER

simple_main: sample/simple_main.cc
	g++ $(CFLAGS) -o $@ $^ $(UV_LIB) $(LIBS)

demo_random: sample/main.cc src/l1_receive_worker.cc src/l1_send_worker.cc
	g++ $(CFLAGS) -o $@ $^ $(UV_LIB) $(LIBS) -DENABLE_SENDER -DENABLE_RECEIVER -DRUN_RANDOM_TIME
	# valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all

sender_random: sample/main.cc src/l1_receive_worker.cc src/l1_send_worker.cc
	g++ $(CFLAGS) -o $@ $^ $(UV_LIB) $(LIBS) -DENABLE_SENDER -DRUN_RANDOM_TIME

receiver_random: sample/main.cc src/l1_receive_worker.cc src/l1_send_worker.cc
	g++ $(CFLAGS) -o $@ $^ $(UV_LIB) $(LIBS) -DENABLE_RECEIVER -DRUN_RANDOM_TIME

clean:
	rm -rf demo sender receiver demo_random sender_random receiver_random
