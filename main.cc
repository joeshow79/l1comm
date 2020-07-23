#include <functional>
#include <chrono>
#include <thread>

#include <stdio.h>

#include "l1receiverworker.h"

void receiver_callback() {
    printf("Receiver callback called.\n");
}

int main() {
    std::function<void(void)> cb = receiver_callback;
    L1ReceiverWorker receiver{"0.0.0.0", 1723, cb};

    receiver.start();

    std::this_thread::sleep_for(std::chrono::milliseconds(8000));

    receiver.stop();

    fprintf(stderr, "end of main.\n");

    return 0;
}
