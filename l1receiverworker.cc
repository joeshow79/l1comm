#include <netinet/in.h>

#include "l1receiverworker.h"

uv_loop_t* loop{nullptr};

void alloc_buffer(uv_handle_t *handle, size_t size, uv_buf_t *buf)
{
     buf->base = (char*)malloc(size);
     buf->len = size;
}

void on_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
     uv_write_t *req = (uv_write_t *)malloc(sizeof(uv_write_t));

     if (nread == -1) {
      /* if (uv_last_error(loop).code != UV_EOF) { */
      /* } */

      uv_close((uv_handle_t *)stream, NULL);
     }

     int r = uv_write(req, stream, buf, 1, NULL);

     if (r) {
      /* error */
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

void thread_task(void* arg) {
    fprintf(stderr, "%s\n", __FUNCTION__);

    L1ReceiverWorker* pthis = (L1ReceiverWorker*)arg;

    loop = uv_default_loop();
    uv_tcp_t server;
    uv_tcp_init(loop, &server);

    struct sockaddr_in addr;

    int r = uv_ip4_addr(pthis->host().c_str(), pthis->port(), &addr);

    r = uv_tcp_bind(&server, (const struct sockaddr*)&addr, 0);

    r = uv_listen((uv_stream_t*)&server, 4/*BACKLOG*/, on_connection);

    if (r) {
        fprintf(stderr, "Listen error.\n");
    }

#if 0
    uv_run(loop, UV_RUN_DEFAULT);
#else
    while (false == pthis->stopped()) {
        int ret = uv_run(loop, UV_RUN_NOWAIT);
        fprintf(stderr, ".");
        if (0 == ret) {
            break;
        }
    }
#endif

//    if (false == pthis->stopped()) {
 //   } 
}

L1ReceiverWorker::L1ReceiverWorker(const std::string& host, int port, std::function<void(void)> callback):
    _host(host),
    _port(port),
    _cb(callback) {
}

bool L1ReceiverWorker::start() {
    fprintf(stderr, "%s\n", __FUNCTION__);

    _stop = false;

    uv_thread_create(&_thread, thread_task, this);

    return true;
}

bool L1ReceiverWorker::stop() {
    fprintf(stderr, "%s\n", __FUNCTION__);

    _stop = true;

    if (nullptr != loop) {
        uv_loop_close(loop);
    }

    uv_thread_join(&_thread);

    return true;
}
