//
// Created by li on 2022/2/23.
//

#ifndef TOHKA_TOHKA_TIMER_H
#define TOHKA_TOHKA_TIMER_H

#include "noncopyable.h"
#include "timepoint.h"
#include "tohka.h"
#include "util/log.h"
namespace tohka {
class Timer : noncopyable {
 public:
  Timer() = delete;
  ~Timer() { log_debug("delete timer id=%ld", timer_id_); }
  Timer(TimePoint expired_time, TimerCallback timer_callback, int32_t interval)
      : expired_time_(expired_time),
        timer_callback_(std::move(timer_callback)),
        interval_(interval),
        repeat_(interval > 0),
        timer_id_(auto_increment_id_.fetch_add(1)) {
    log_debug("create timer id=%ld", timer_id_);
  };
  void run() { timer_callback_(); }

  TimePoint GetExpiredTime() const { return expired_time_; }
  int64_t GetTimerId() const { return timer_id_; }
  bool IsRepeat() const { return repeat_; }

  void Restart(TimePoint now);

 private:
  TimePoint expired_time_;
  TimerCallback timer_callback_;
  int32_t interval_;
  bool repeat_;
  int64_t timer_id_;
  static std::atomic<int64_t> auto_increment_id_;
};
}  // namespace tohka
#endif  // TOHKA_TOHKA_TIMER_H
