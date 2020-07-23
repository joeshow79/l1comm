#include <string>
#include <functional>
#include <atomic>

#include <uv.h>

class L1ReceiverWorker{
public:
    L1ReceiverWorker(const std::string& host, int port, std::function<void(void)> callback);

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

    std::function<void(void)> callback() {return _cb;}

private:
/*
    static void* s_run(void *args);

    void run();*/

    /*void alloc_buffer(uv_handle_t *handle, size_t size, uv_buf_t *buf);
    void on_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);
    void on_connection(uv_stream_t *server, int status);*/

private:
    std::string _host;
    int _port;
    std::function<void(void)> _cb;
    std::atomic<bool> _stop{false};

    uv_thread_t _thread;
}; 
