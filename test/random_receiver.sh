#!/usr/bin/env bash

loop=0

BIN_PATH=receiver_random

while true
do
  echo $0 " iteration: " $loop

  #expr $BIN_PATH
  ./receiver_random

  sleep $((1 + RANDOM % 10))

  loop=`expr $loop + 1`
done
