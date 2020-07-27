#include <chrono>
#include <thread>

#include <netinet/in.h>

#include "l1_send_worker.h"

static void write_cb(uv_write_t* req, int status) {
  fprintf(stderr, "%s\n", __FUNCTION__);

  free(req);
}

static void on_connect(uv_connect_t *server, int status) {
    fprintf(stderr, "%s\n", __FUNCTION__);

    if (status < 0) {
        fprintf(stderr, "New connection error %s\n", uv_strerror(status));
        // error!
        return;
    }

    uv_write_t* req;
    uv_buf_t buf;
    int i, r;

    buf = uv_buf_init((char*)"abcd", 4);

    for (i = 0; i < 2; i++) {
      fprintf(stderr, "write to peer.\n");

      req = (uv_write_t*) malloc(sizeof *req);

      r = uv_write(req, (uv_stream_t*)server->data, &buf, 1, write_cb);
    }
}

static void on_close(uv_handle_t* handle) {
  fprintf(stderr, "%s\n", __FUNCTION__);
  // TODO:
}

void L1SendWorker::thread_task(void* arg) {
    fprintf(stderr, "L1SendWorker::%s\n", __FUNCTION__);

    L1SendWorker* pthis = (L1SendWorker*)arg;

    uv_tcp_t* socket = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
    uv_tcp_init(L1SendWorker::GetLoop(), socket);

    uv_connect_t* connect = (uv_connect_t*)malloc(sizeof(uv_connect_t));

    connect->data = socket;

    struct sockaddr_in dest;
    uv_ip4_addr(pthis->Host().c_str(), pthis->Port(), &dest);

    int r = uv_tcp_connect(connect, socket, (const struct sockaddr*)&dest, on_connect);

    while (false == pthis->Stopped()) {
      int ret = uv_run(L1SendWorker::GetLoop(), UV_RUN_NOWAIT);
      fprintf(stderr, "<");

      //if (0 == ret) {
        //break;
       //}

      // TODO: fetch data and send to peer
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    uv_close((uv_handle_t*)socket, on_close);

    fprintf(stderr, "End of L1SendWorker loop.\n");
}

L1SendWorker::L1SendWorker(const std::string& host, int port):
    host_(host),
    port_(port) {
}

bool L1SendWorker::Start() {
    fprintf(stderr, "L1SendWorker::%s\n", __FUNCTION__);

    stop_ = false;

    uv_thread_create(&thread_, thread_task, this);

    return true;
}

bool L1SendWorker::Stop() {
    fprintf(stderr, "L1SendWorker::%s\n", __FUNCTION__);

    stop_ = true;

    if (nullptr != L1SendWorker::GetLoop()) {
        uv_loop_close(L1SendWorker::GetLoop());
    }

    uv_thread_join(&thread_);

    return true;
}
