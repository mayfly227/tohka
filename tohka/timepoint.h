//
// Created by li on 2022/2/20.
//

#ifndef TOHKA_TOHKA_TIMEPOINT_H
#define TOHKA_TOHKA_TIMEPOINT_H

#include "platform.h"

namespace tohka {
class TimePoint {
 public:
  // init an invalid time
  TimePoint() : microseconds_(-1) {}
  explicit TimePoint(int64_t microseconds) : microseconds_(microseconds) {}

  int64_t GetMicroSeconds() const { return microseconds_; }
  int64_t GetMilliSeconds() const {
    return microseconds_ / kMilliSecondsPerSecond;
  }
  std::string ToString() const;
  std::string ToFormatString(bool show_microseconds = true) const;
  static TimePoint now();
  bool operator<(const TimePoint& other) const {
    return this->microseconds_ < other.microseconds_;
  }
  bool operator==(const TimePoint& other) const {
    return this->microseconds_ == other.microseconds_;
  }
  TimePoint operator+(int32_t delay_ms) const {
    return TimePoint{microseconds_ + delay_ms * kMilliSecondsPerSecond};
  }

  static constexpr int kMicroSecondPerSecond = 1000 * 1000;
  static constexpr int kMilliSecondsPerSecond = 1000;

 private:
  // microseconds since epoch
  int64_t microseconds_;
};

}  // namespace tohka
#endif  // TOHKA_TOHKA_TIMEPOINT_H
