//
// Created by li on 2022/2/23.
//

#include "timer.h"

using namespace tohka;

std::atomic<int64_t> Timer::auto_increment_id_ = 1;
void Timer::Restart(TimePoint now) {
  if (repeat_) {
    expired_time_ = now + interval_;
  }
}
