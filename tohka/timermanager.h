//
// Created by li on 2022/2/20.
//

#ifndef TOHKA_TOHKA_TIMERMANAGER_H
#define TOHKA_TOHKA_TIMERMANAGER_H
#include <memory.h>

#include <map>
#include <vector>

#include "timepoint.h"
#include "timer.h"
#include "tohka.h"

namespace tohka {
class TimerManager {
 public:
  static constexpr int64_t kDefaultTimeOutMs = 10000;

  void AddTimer(TimePoint when, TimerPrt_t tc);
  ExpiredTimers GetExpiredTimers();
  int64_t GetNextExpiredDuration();

 private:
  // for sort
  using TimerMap = std::multimap<TimePoint, TimerPrt_t>;
  TimerMap timer_map_;
};
}  // namespace tohka
#endif  // TOHKA_TOHKA_TIMERMANAGER_H
