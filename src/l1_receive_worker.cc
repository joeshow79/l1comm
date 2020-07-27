#include <chrono>
#include <thread>

#include <netinet/in.h>

#include "l1_receive_worker.h"

uv_loop_t* loop{nullptr};
std::function<void(void)>* cb{nullptr};

static void alloc_buffer(uv_handle_t *handle, size_t size, uv_buf_t *buf)
{
     buf->base = (char*)malloc(size);
     buf->len = size;
}

void on_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
     if (nread == -1) {
      /* if (uv_last_error(loop).code != UV_EOF) { */
      /* } */

      uv_close((uv_handle_t *)stream, NULL);
     }

     // validate
     if (nread != sizeof(L1Request)) {
        fprintf(stderr, "Wrong packet received with length %ld.\n", nread);
     }

#if 0  
     uv_write_t *req = (uv_write_t *)malloc(sizeof(uv_write_t));

     int r = uv_write(req, stream, buf, 1, NULL);   // echo

     if (r) {
      /* error */
     }

#endif 

     L1Request l1_req = *(L1Request*)(buf->base);

     //l1_req.set_request_type(L1Request::RequestType::HEARTBEAT_AND_SIGNAL).
       //set_signal_type(L1Request::SignalType::INPLACE);

     if (nullptr != cb &&
        (l1_req.GetRequestType() ==
         L1Request::RequestType::HEARTBEAT_AND_SIGNAL) 
        && (l1_req.GetSignalType() == L1Request::SignalType::INPLACE)) {
        (*cb)();
     }

     free(buf->base);
}

void on_connection(uv_stream_t *server, int status) {
    if (status < 0) {
        fprintf(stderr, "New connection error %s\n", uv_strerror(status));
        // error!
        return;
    }

    uv_tcp_t *client = (uv_tcp_t*) malloc(sizeof(uv_tcp_t));
    uv_tcp_init(loop, client);
    if (uv_accept(server, (uv_stream_t*) client) == 0) {
        uv_read_start((uv_stream_t*) client, alloc_buffer, on_read);
    }
    else {
        uv_close((uv_handle_t*) client, NULL);
    }
}

static void thread_task(void* arg) {
    fprintf(stderr, "%s\n", __FUNCTION__);

    L1ReceiveWorker* pthis = (L1ReceiveWorker*)arg;

    loop = uv_default_loop();
    uv_tcp_t server;
    uv_tcp_init(loop, &server);

    struct sockaddr_in addr;

    int r = uv_ip4_addr(pthis->Host().c_str(), pthis->Port(), &addr);

    r = uv_tcp_bind(&server, (const struct sockaddr*)&addr, 0);

    r = uv_listen((uv_stream_t*)&server, 4/*BACKLOG*/, on_connection);

    if (r) {
        fprintf(stderr, "Listen error.\n");
    }

#if 0
    uv_run(loop, UV_RUN_DEFAULT);
#else
    cb = &(pthis->Callback());

    while (false == pthis->Stopped()) {
        int ret = uv_run(loop, UV_RUN_NOWAIT);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        //fprintf(stderr, ".");

        if (0 == ret) {
            break;
        }
    }
#endif

//    if (false == pthis->stopped()) {
 //   } 
 
    fprintf(stderr, "End of L1ReceiveWorker loop.\n");
}

L1ReceiveWorker::L1ReceiveWorker(const std::string& host, int port, std::function<void(void)> callback):
    host_(host),
    port_(port),
    cb_(callback) {
}

bool L1ReceiveWorker::Start() {
    fprintf(stderr, "%s\n", __FUNCTION__);

    stop_ = false;

    uv_thread_create(&thread_, thread_task, this);

    return true;
}

bool L1ReceiveWorker::Stop() {
    fprintf(stderr, "%s\n", __FUNCTION__);

    stop_ = true;

    if (nullptr != loop) {
        uv_loop_close(loop);
    }

    uv_thread_join(&thread_);

    return true;
}
