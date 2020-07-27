#!/usr/bin/env bash

loop=0

while true
do
  echo "iteration: " $loop

  echo "abcd\n" | telnet localhost 3001

  sleep 1

  loop=`expr $loop + 1`
done
