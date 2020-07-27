#include <stdio.h>

#include <assert.h>

#include "l1.h"

int main() {
  L1Request req;

  fprintf(stderr, "Expected size is 8, actual size of L1Request: %ld\n",
      sizeof(req));

  assert(8==sizeof(req));

  L1Request::RequestType t{L1Request::RequestType::HEARTBEAT_AND_SIGNAL};

  req.SetRequestType(t);

  assert(t == req.GetRequestType());

  L1Request::SignalType s{L1Request::SignalType::INPLACE};

  req.SetSignalType(s);

  assert(s == req.GetSignalType());

  fprintf(stderr, "Pass test.\n");

  return 0;
}
