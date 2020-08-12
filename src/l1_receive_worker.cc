#include <chrono>
#include <thread>

#include <netinet/in.h>

#include "l1_receive_worker.h"

static void alloc_buffer(uv_handle_t *handle, size_t size, uv_buf_t *buf) {
  buf->base = (char *)malloc(size);
  buf->len = size;
}

static void on_close(uv_handle_t *handle) {
  fprintf(stderr, "%s\n", __FUNCTION__);
  // TODO:
  free(handle);
}

static void on_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
  if (nread < 0) {
    if (nread != UV_EOF) {
      fprintf(stderr, "Read error %s\n", uv_err_name(nread));
    }
    fprintf(stderr, "going to close the socket.\n");

    uv_close((uv_handle_t *)stream, on_close);
    return;
  }

  // validate
  if (nread != sizeof(L1Request)) {
    fprintf(stderr, "Wrong packet received with length %d.\n", (int)nread);
  }

  L1Request l1_req = *(L1Request *)(buf->base);

  // l1_req.set_request_type(L1Request::RequestType::HEARTBEAT_AND_SIGNAL).
  // set_signal_type(L1Request::SignalType::INPLACE);

  std::function<void(void)> *cb = (std::function<void()> *)stream->data;

  if (nullptr != cb &&
      (l1_req.GetRequestType() ==
       L1Request::RequestType::HEARTBEAT_AND_SIGNAL) &&
      (l1_req.GetSignalType() == L1Request::SignalType::INPLACE)) {
    fprintf(stderr, "Receive HB: %u with signal.\n",
            l1_req.GetHeartbeatSeqNum());

    (*cb)();
  } else {
    if (l1_req.GetRequestType() == L1Request::RequestType::HEARTBEAT) {
      fprintf(stderr, "Receive HB: %u\n", l1_req.GetHeartbeatSeqNum());
    }
  }

  free(buf->base);
}

static void on_connection(uv_stream_t *server, int status) {
  fprintf(stderr, "New connection ... \n");

  if (status != 0) {
    fprintf(stderr, "New connection error %s\n", uv_strerror(status));
    // error!
    uv_close((uv_handle_t *)server, on_close);
    return;
  }

  uv_tcp_t *client = (uv_tcp_t *)malloc(sizeof(uv_tcp_t));
  uv_tcp_init(L1ReceiveWorker::GetLoop(), client);
  if (uv_accept(server, (uv_stream_t *)client) == 0) {
    client->data = server->data;
    uv_read_start((uv_stream_t *)client, alloc_buffer, on_read);
  } else {
    uv_close((uv_handle_t *)client, on_close);
  }
}

void L1ReceiveWorker::thread_task(void *arg) {
  fprintf(stderr, "%s\n", __FUNCTION__);

  L1ReceiveWorker *pthis = (L1ReceiveWorker *)arg;

  uv_tcp_t *server = (uv_tcp_t *)malloc(sizeof(uv_tcp_t));

  uv_tcp_init(L1ReceiveWorker::GetLoop(), server);

  struct sockaddr_in addr;

  int r = uv_ip4_addr(pthis->Host().c_str(), pthis->Port(), &addr);

  r = uv_tcp_bind(server, (const struct sockaddr *)&addr, 0);

  r = uv_listen((uv_stream_t *)server, 4 /*BACKLOG*/, on_connection);

  if (r) {
    fprintf(stderr, "Listen error.\n");
  }

#if 0
    uv_run(loop, UV_RUN_DEFAULT);
#else
  server->data = (void *)(&pthis->Callback());

  while (false == pthis->Stopped()) {
    uv_run(L1ReceiveWorker::GetLoop(), UV_RUN_NOWAIT);
    // fprintf(stderr, "error code: %d. %s, %s\n", ret, uv_err_name(ret),
    // uv_strerror(ret));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // fprintf(stderr, ">");

    // if (0 == ret) {
    // break;
    //}
  }
#endif

  uv_close((uv_handle_t *)server, on_close);

  fprintf(stderr, "End of L1ReceiveWorker loop.\n");
}

L1ReceiveWorker::L1ReceiveWorker(const std::string &host, int port,
                                 std::function<void(void)> callback)
    : host_(host), port_(port), cb_(callback) {}

bool L1ReceiveWorker::Start() {
  fprintf(stderr, "%s\n", __FUNCTION__);

  stop_ = false;

  uv_thread_create(&thread_, thread_task, this);

  return true;
}

bool L1ReceiveWorker::Stop() {
  fprintf(stderr, "%s\n", __FUNCTION__);

  stop_ = true;

  if (nullptr != L1ReceiveWorker::GetLoop()) {
    uv_loop_close(L1ReceiveWorker::GetLoop());
  }

  uv_thread_join(&thread_);

  return true;
}
