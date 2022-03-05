//
// Created by li on 2022/2/23.
//

#ifndef TOHKA_TOHKA_TIMER_H
#define TOHKA_TOHKA_TIMER_H


#include "timepoint.h"
#include "tohka.h"
namespace tohka {
class Timer {
 public:
  Timer() = delete;
  Timer(TimePoint expired_time, TimerCallback timer_callback)
      : expired_time_(expired_time),
        timer_callback_(std::move(timer_callback)){};
  Timer(TimePoint expired_time, TimerCallback timer_callback, int64_t delay,
        bool repeat = false)
      : expired_time_(expired_time),
        timer_callback_(std::move(timer_callback)),
        delay_{delay},
        repeat_{repeat} {};
  void run() { timer_callback_(); }

  TimePoint GetExpiredTime() const { return expired_time_; }
  void SetExpiredTime(const TimePoint ExpiredTime) {
    expired_time_ = ExpiredTime;
  }
  bool IsRepeat() const { return repeat_; }
  void SetRepeat(bool Repeat) { repeat_ = Repeat; }
  int64_t GetDelay() const { return delay_; }
  void SetDelay(int64_t Delay) { delay_ = Delay; }

 private:
  TimePoint expired_time_;
  TimerCallback timer_callback_;
  bool repeat_{false};
  int64_t delay_{-1};  // ms
};
}  // namespace tohka
#endif  // TOHKA_TOHKA_TIMER_H
