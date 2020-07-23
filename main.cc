#include <functional>
#include <chrono>
#include <thread>

#include <stdio.h>

#include "l1.h"
#include "l1receiverworker.h"

void receiver_callback(L1Request req) {
    printf("Received request %s.\n", req.to_string().c_str());
}

int main() {
    std::function<void(L1Request)> cb = receiver_callback;
    L1ReceiverWorker receiver{"0.0.0.0", 3001, cb};

    receiver.start();

    std::this_thread::sleep_for(std::chrono::milliseconds(100000));

    receiver.stop();

    fprintf(stderr, "end of main.\n");

    return 0;
}
