#pragma once

#include <bitset>
#include <string>

typedef unsigned char       WORD;   // Actually it's not WORD type(4 bytes)
typedef unsigned short       DWORD; // Keep same name as user document.

#pragma pack(push, 1)

// struct size: 8
struct L1Request {
  DWORD dbw0{0}; // heartbeat seq num: 0~32767

  DWORD dbw2{0}; // bit 0: 1 (signal with heartbeat)
                 //       0 (heartbeat)
                 // bit 1: 1 (in-place)
                 //       0 (not ready)

  DWORD dbw4{0}; // reserved

  DWORD dbw7{0}; // reserved

  enum class RequestType:unsigned char { HEARTBEAT, HEARTBEAT_AND_SIGNAL};
  enum class SignalType:unsigned char { NOTREADY, INPLACE};

  void set_heartbeat_seq_num(DWORD num) {
    dbw0 = num;
  }

  DWORD GetHeartbeatSeqNum() {
    return dbw0;
  }

  L1Request& SetRequestType(RequestType t) {
    std::bitset<16> v(dbw2);

    if (RequestType::HEARTBEAT == t) {
      v.reset(0);
    } else {
      v.set(0);
    }

    dbw2 = (DWORD)v.to_ulong();

    return (*this);
  }

  RequestType GetRequestType() {
    std::bitset<16> v(dbw2);

    if (v.test(0)) {
      return RequestType::HEARTBEAT_AND_SIGNAL;
    }
    return RequestType::HEARTBEAT;
  }

  L1Request& SetSignalType(SignalType t) {
    std::bitset<16> v(dbw2);

    if (SignalType::NOTREADY == t) {
      v.reset(1);
    } else {
      v.set(1);
    }

    dbw2 = (DWORD)v.to_ulong();

    return (*this);
  }

  SignalType GetSignalType() {
    std::bitset<16> v(dbw2);

    if (v.test(1)) {
      return SignalType::INPLACE;
    }
    return SignalType::NOTREADY;
  }

  std::string ToString() {
    std::string ret;

    std::bitset<16> v(dbw2);

    ret = "HB: " + std::to_string(dbw0);

    if (v.test(0)) {
      ret += " TYPE: HEARTBEAT_AND_SIGNAL";
    } else {
      ret += " TYPE: HEARTBEAT";
    }

    if (v.test(1)) {
      ret += " SIGNAL: INPLACE";
    } else {
      ret += " SIGNAL: NOTREADY";
    }

    return ret;
  }
};

#pragma pack(pop)