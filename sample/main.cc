#include <chrono>
#include <functional>
#include <thread>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "l1.h"
#include "l1_receive_worker.h"
#include "l1_send_worker.h"

void receiver_callback(void) { printf("Received request.\n"); }

int main() {
  signal(SIGPIPE, SIG_IGN);

#if ENABLE_RECEIVER
  std::function<void(void)> cb = receiver_callback;
  L1ReceiveWorker receiver{"0.0.0.0", 3002, cb};
#endif

#ifdef ENABLE_SENDER
  // In order server is running
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  L1SendWorker sender{"0.0.0.0", 3011};
#endif

#ifdef ENABLE_RECEIVER
  receiver.Start();
#endif

#ifdef ENABLE_SENDER
  sender.Start();
#endif

#ifdef RUN_RANDOM_TIME
  srand(time(NULL));
  int run_time = rand() % 13 + 1;  // 1 ~ 13
  std::this_thread::sleep_for(std::chrono::seconds(run_time));
#else
  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
#endif

#ifdef ENABLE_RECEIVER
  receiver.Stop();
#endif

#ifdef ENABLE_SENDER
  sender.Stop();
#endif

  fprintf(stderr, "end of main.\n");

  return 0;
}
