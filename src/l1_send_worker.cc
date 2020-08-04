#include <chrono>
#include <thread>

#include <netinet/in.h>

#include "l1_send_worker.h"
#include "l1_num_generator.h"

static void write_cb(uv_write_t* req, int status) {
  fprintf(stderr, "%s\n", __FUNCTION__);

  free(req);
}

static void on_close(uv_handle_t* handle) {
  fprintf(stderr, "%s\n", __FUNCTION__);
  // TODO:
  free(handle);
}

static void on_connect(uv_connect_t *server, int status) {
    fprintf(stderr, "%s\n", __FUNCTION__);

    if (status < 0) {
        fprintf(stderr, "New connection error %s\n", uv_strerror(status));
        // error!
        uv_close((uv_handle_t*)server, on_close);
        return;
    }
}

static void send_heartbeat_cb(uv_write_t* req, int status) {
  free((L1Response*)req->data);
  free(req);
}

static void on_timer(uv_timer_t* handle) {
    // Send heartbeat
    uv_tcp_t* socket = (uv_tcp_t*)(handle->data);

    L1Response* payload = (L1Response*)malloc(sizeof(L1Response));
    unsigned short num = NumGenerator<unsigned short>::GetNum();
    payload->dbw0 = num;

    fprintf(stderr, "Send HB: %u\n", num);

    uv_write_t* req;
    uv_buf_t buf;

    buf = uv_buf_init((char*)payload, sizeof(L1Response));

    fprintf(stderr, "write to peer.\n");

    req = (uv_write_t*) malloc(sizeof *req);
    req->data = payload;

    uv_write(req, (uv_stream_t*)socket, &buf, 1, send_heartbeat_cb);
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

    uv_tcp_connect(connect, socket, (const struct sockaddr*)&dest, on_connect);

    uv_timer_t* timer = (uv_timer_t*)malloc(sizeof(uv_timer_t));
    uv_timer_init(L1SendWorker::GetLoop(), timer);
    timer->data = socket;
    uv_timer_start(timer, on_timer, 5000, 1);

    while (false == pthis->Stopped()) {
      int ret = uv_run(L1SendWorker::GetLoop(), UV_RUN_NOWAIT);
      // fprintf(stderr, "error code: %d. %s, %s\n", ret, uv_err_name(ret), uv_strerror(ret));

      //if (0 != ret) {
        //break;
       //}

      // TODO: fetch data and send to peer
      
#if 0  // Test data
    uv_write_t* req;
    uv_buf_t buf;
    int i;

    buf = uv_buf_init((char*)"abcd\n", 5);

    for (i = 0; i < 2; i++) {
      fprintf(stderr, "write to peer.\n");

      req = (uv_write_t*) malloc(sizeof *req);

      uv_write(req, (uv_stream_t*)socket, &buf, 1, write_cb);
    }
#else
    //TODO: Real data
#endif

      std::this_thread::sleep_for(std::chrono::milliseconds(100));

      //fprintf(stderr, "<");
    }

    uv_close((uv_handle_t*)connect, on_close);
    uv_close((uv_handle_t*)socket, on_close);

    uv_timer_stop(timer);
    free(timer);

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
