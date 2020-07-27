#pragma once

#include <bitset>
#include <string>

typedef unsigned char       WORD;   // Actually it's not WORD type(4 bytes)
typedef unsigned short      DWORD;  // Keep same name as user document.

#pragma pack(push, 1)

// struct size: 8 bytes
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

// struct size: 32 bytes
struct L1Response {
  DWORD dbw0{0}; // heartbeat seq num: 0~32767
  DWORD dbw2{0}; // bit 0: 1 (signal with heartbeat)
                 //       0 (heartbeat)
                 // bit 1: 1 (Recognition done)
                 //       0 (Not defined)
                 // bit 2: 1 (Steel number recognition done)
                 //       0 (Steel number recognition error)
                 // bit 3: 1 (Steel number found)
                 //       0 (Steel number not found)
                 // bit 4: 1 (Edge crack found)
                 //       0 (Edge Crack not found)
                 // bit 5: 1 (Is tower-shape)
                 //       0 (Is not tower-shape)
  DWORD dbw4{0}; // Lenght of steel number
  WORD dbw6[20]; // Steel number, length 20
  WORD dbw26{0}; // bit 0: 1 (表示钢卷传动侧拍钢卷号的相机无法连上，故障)
                 //       0 (表示该位置相机正常)
                 // bit 1: 1 (表示钢卷操作室侧拍钢卷号的相机无法连上，故障)
                 //       0 (表示该位置相机正常)
  WORD dbw27{0}; // bit 0: 1 (表示钢卷传动侧，左上角拍裂边的相机无法连上，故障)
                 //       0 (表示该位置相机正常)
                 // bit 1: 1 (表示钢卷传动侧，左下角拍裂边的相机无法连上，故障)
                 //       0 (表示该位置相机正常)
                 // bit 2: 1 (表示钢卷传动侧，右下角拍裂边的相机无法连上，故障)
                 //       0 (表示该位置相机正常)
                 // bit 3: 1 (表示钢卷传动侧，右上角拍裂边的相机无法连上，故障)
                 //       0 (表示该位置相机正常)
                 // bit 4: 1 (表示钢卷操作侧，左上角拍裂边的相机无法连上)
                 //       0 (表示该位置相机正常)
                 // bit 5: 1 (钢卷操作侧，左下角拍裂边的相机无法连上表示，故障)
                 //       0 (表示该位置相机正常)
                 // bit 6: 1 (表示钢卷操作侧，右下角拍裂边的相机无法连上，故障)
                 //       0 (表示该位置相机正常)
                 // bit 7: 1 (表示钢卷操作侧，右上角拍裂边的相机无法连上，故障)
                 //       0 (表示该位置相机正常)
  WORD dbw28{0}; // bit 0: 1 (表示拍钢卷塔形的相机无法连上，故障)
                 //       0 (表示该位置相机正常)
  DWORD dbw29{0}; // Reserved
  WORD dbw31{0}; // Reserved
};

#pragma pack(pop)
