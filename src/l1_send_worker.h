#pragma once

#include <string>
#include <functional>
#include <atomic>

#include <uv.h>

#include "l1.h"

class L1SendWorker{
public:
    L1SendWorker(const std::string& host, int port);

    L1SendWorker(const L1SendWorker& other) = delete;

    void operator=(const L1SendWorker& rhs) = delete;

    ~L1SendWorker() = default;

    bool Start();

    bool Stop();

    bool Stopped() {
        return stop_.load();
    }

    std::string Host() {return host_;}

    int Port() {return port_;}

    static uv_loop_t* GetLoop() {
      static uv_loop_t* loop{nullptr};

      if (nullptr == loop) {
        loop = new uv_loop_t();
        uv_loop_init(loop);
      }

      return loop;
    }

private:
    static void thread_task(void* arg);

private:
    std::string host_;
    int port_;
    std::atomic<bool> stop_{false};
    uv_thread_t thread_;
}; 
