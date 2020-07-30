#pragma once

#include <limits>

template<class T>
class NumGenerator {
public:
  static T GetNum() {
    static T num_{0};
    T ret = num_;
    num_ = (++num_) % std::numeric_limits<T>::max();
    return ret;
  }
};
