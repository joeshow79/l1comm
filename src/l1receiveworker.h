#pragma once

#include <string>
#include <functional>
#include <atomic>

#include <uv.h>

#include "l1.h"

class L1ReceiveWorker{
public:
    L1ReceiveWorker(const std::string& host, int port, std::function<void(void)> callback);

    L1ReceiveWorker(const L1ReceiveWorker& other) = delete;

    void operator=(const L1ReceiveWorker& rhs) = delete;

    ~L1ReceiveWorker() = default;

    bool start();

    bool stop();

    bool stopped() {
        return _stop.load();
    }

    std::string host() {return _host;}

    int port() {return _port;}

    std::function<void(void)>& callback() {return _cb;}

private:
    std::string _host;
    int _port;
    std::function<void(void)> _cb;
    std::atomic<bool> _stop{false};

    uv_thread_t _thread;
}; 
