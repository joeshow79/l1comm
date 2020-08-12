#include <atomic>
#include <chrono>
#include <thread>

#include <netinet/in.h>
#include <signal.h>
#include <string.h>

#include "l1_num_generator.h"
#include "l1_send_worker.h"

static struct ConnStruct {
  uv_tcp_t* socket{nullptr};

  uv_connect_t* connect{nullptr};

  struct sockaddr_in dest;

  uv_timer_t* timer{nullptr};

  // -1: disconnect; 0: connecting; 1: connected;
  std::atomic<int> status{-1};
} conn_struct;

static void write_cb(uv_write_t* req, int status) {
  fprintf(stderr, "%s\n", __FUNCTION__);

  free(req);
}

static void on_close(uv_handle_t* handle) {
  fprintf(stderr, "%s\n", __FUNCTION__);
  // TODO:
  free(handle);
}

static void send_heartbeat_cb(uv_write_t* req, int status) {
  fprintf(stderr, "%s\n", __FUNCTION__);

  if (status < 0) {
    fprintf(stderr, "----------------%s error\n", __FUNCTION__);

    conn_struct.status.store(-1);
    uv_timer_stop(conn_struct.timer);
  } else { 
    fprintf(stderr, "----------------%s ok\n", __FUNCTION__);
  }

  free((L1Response*)req->data);
  free(req);
}

static void on_timer(uv_timer_t* handle) {
  fprintf(stderr, "%s\n", __FUNCTION__);

  // Send heartbeat
  uv_tcp_t* socket = conn_struct.socket;

  L1Response* payload = (L1Response*)malloc(sizeof(L1Response));
  unsigned short num = NumGenerator<unsigned short>::GetNum();
  payload->dbw0 = num;

  fprintf(stderr, "Send HB: %u\n", num);

  uv_write_t* req;
  uv_buf_t buf;

  buf = uv_buf_init((char*)payload, sizeof(L1Response));

  fprintf(stderr, "write to peer.\n");

  req = (uv_write_t*)malloc(sizeof *req);
  req->data = payload;

  uv_write(req, (uv_stream_t*)socket, &buf, 1, send_heartbeat_cb);
}

static void on_connect(uv_connect_t* server, int status) {
  fprintf(stderr, "%s\n", __FUNCTION__);

  if (status < 0) {
    fprintf(stderr, "New connection error %s\n", uv_strerror(status));
    // error!
    // uv_close((uv_handle_t*)server, on_close);
    conn_struct.status.store(-1);

    return;
  }

  conn_struct.status.store(1);

  if (conn_struct.timer != nullptr) {
    uv_timer_stop(conn_struct.timer);

    free(conn_struct.timer);
  }

  conn_struct.timer = (uv_timer_t*)malloc(sizeof(uv_timer_t));
  uv_timer_init(L1SendWorker::GetLoop(), conn_struct.timer);
  conn_struct.timer->data = &conn_struct;
  uv_timer_start(conn_struct.timer, on_timer, 5000, 1);
}

void L1SendWorker::thread_task(void* arg) {
  fprintf(stderr, "L1SendWorker::%s\n", __FUNCTION__);

  L1SendWorker* pthis = (L1SendWorker*)arg;

  uv_connect_t* connect = (uv_connect_t*)malloc(sizeof(uv_connect_t));

  uv_tcp_t* socket{nullptr};

  struct sockaddr_in dest;
  uv_ip4_addr(pthis->Host().c_str(), pthis->Port(), &dest);

  conn_struct.dest = dest;

  while (false == pthis->Stopped()) {
    if (conn_struct.status.load() == -1) {
      fprintf(stderr, "reconnect\n");

      conn_struct.status.store(0);

      if (socket != nullptr) {
        uv_close((uv_handle_t*)socket, on_close);
      }

      socket = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
      uv_tcp_init(L1SendWorker::GetLoop(), socket);

      conn_struct.socket = socket;
      conn_struct.connect = connect;
      connect->data = &conn_struct;

      uv_tcp_connect(connect, socket, (const struct sockaddr*)&dest,
                     on_connect);
    }

    if (conn_struct.status.load() == 1) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    } else {
      fprintf(stderr, "sleep and wait to connect.\n");
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    uv_run(L1SendWorker::GetLoop(), UV_RUN_NOWAIT);

    // TODO: fetch data and send to peer
  }

  uv_close((uv_handle_t*)connect, on_close);
  uv_close((uv_handle_t*)socket, on_close);

  if (nullptr != conn_struct.timer) {
    uv_timer_stop(conn_struct.timer);
    free(conn_struct.timer);
  }

  fprintf(stderr, "End of L1SendWorker loop.\n");
}

L1SendWorker::L1SendWorker(const std::string& host, int port)
    : host_(host), port_(port) {}

bool L1SendWorker::Start() {
  fprintf(stderr, "L1SendWorker::%s\n", __FUNCTION__);

  //signal(SIGPIPE, SIG_IGN);
  //struct sigaction act;
  //memset(&act, 0, sizeof(act));
  //act.sa_handler = SIG_IGN;
  //act.sa_flags = SA_RESTART;
  //int r = sigaction(SIGPIPE, &act, NULL);
  //if (r) {
    //fprintf(stderr, "error at setup signal handler\n");
  //}

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
