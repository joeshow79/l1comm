#include <stdio.h>

#include <assert.h>

#include "l1.h"

int main() {
  L1Request req;

  fprintf(stderr, "Expected size is 8, actual size of L1Request: %ld\n",
      sizeof(req));

  assert(8==sizeof(req));

  L1Request::RequestType t{L1Request::RequestType::HEARTBEAT_AND_SIGNAL};

  req.set_request_type(t);

  assert(t == req.get_request_type());

  L1Request::SignalType s{L1Request::SignalType::INPLACE};

  req.set_signal_type(s);

  assert(s == req.get_signal_type());

  fprintf(stderr, "Pass test.\n");

  return 0;
}
