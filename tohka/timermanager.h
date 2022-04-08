//
// Created by li on 2022/2/20.
//

#ifndef TOHKA_TOHKA_TIMERMANAGER_H
#define TOHKA_TOHKA_TIMERMANAGER_H

#include "noncopyable.h"
#include "timepoint.h"
#include "tohka.h"
#include "timerid.h"

namespace tohka {
class TimerManager : noncopyable {
 public:
  TimerManager();
  static constexpr int64_t kDefaultTimeOutMs = 10000;

  TimerId AddTimer(TimePoint when, TimerCallback cb, int32_t interval);
  void DeleteTimer(const TimerId& timer_id);

  int64_t GetNextExpiredDuration();
  void DoExpiredTimers();

 private:
  ExpiredTimers GetExpiredTimers();
  void Reset(ExpiredTimers& expired);

  // for sort
  using TimerMap = std::multimap<TimePoint, TimerPrt_t>;
  using ActivateTimers = std::multimap<int64_t, TimerPrt_t>;
  TimerMap timer_map_;
  ActivateTimers activate_timers_;
  ActivateTimers cancel_timers_;
  bool calling_expired_timers_;
};
}  // namespace tohka
#endif  // TOHKA_TOHKA_TIMERMANAGER_H
