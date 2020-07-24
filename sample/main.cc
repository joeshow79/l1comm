#include <functional>
#include <chrono>
#include <thread>

#include <stdio.h>

#include "l1.h"
#include "l1receiveworker.h"

void receiver_callback(void) {
    printf("Received request.\n");
}

int main() {
    std::function<void(void)> cb = receiver_callback;
    L1ReceiveWorker receiver{"0.0.0.0", 3001, cb};

    receiver.start();

    std::this_thread::sleep_for(std::chrono::milliseconds(100000));

    receiver.stop();

    fprintf(stderr, "end of main.\n");

    return 0;
}
