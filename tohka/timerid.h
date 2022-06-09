//
// Created by li on 2022/4/4.
//

#ifndef TOHKA_TOHKA_TIMERID_H
#define TOHKA_TOHKA_TIMERID_H

#include "tohka.h"

namespace tohka {

class TimerId {
 public:
  TimerId() : timer_(), sequence_(0) {}

  TimerId(std::weak_ptr<Timer> timer, int64_t seq)
      : timer_(std::move(timer)), sequence_(seq) {}

  friend class TimerManager;

  int64_t GetId() const { return sequence_; }
  std::weak_ptr<Timer> GetTimer() { return timer_.lock(); }

 private:
  std::weak_ptr<Timer> timer_;
  int64_t sequence_;
};
}  // namespace tohka
#endif  // TOHKA_TOHKA_TIMERID_H
