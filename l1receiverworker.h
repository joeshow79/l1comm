#pragma once

#include <string>
#include <functional>
#include <atomic>

#include <uv.h>

#include "l1.h"

class L1ReceiverWorker{
public:
    L1ReceiverWorker(const std::string& host, int port, std::function<void(L1Request)> callback);

    L1ReceiverWorker(const L1ReceiverWorker& other) = delete;

    void operator=(const L1ReceiverWorker& rhs) = delete;

    ~L1ReceiverWorker() = default;

    bool start();

    bool stop();

    bool stopped() {
        return _stop.load();
    }

    std::string host() {return _host;}

    int port() {return _port;}

    std::function<void(L1Request)>& callback() {return _cb;}

private:
    std::string _host;
    int _port;
    std::function<void(L1Request)> _cb;
    std::atomic<bool> _stop{false};

    uv_thread_t _thread;
}; 
