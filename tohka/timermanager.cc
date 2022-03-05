//
// Created by li on 2022/2/20.
//

#include "timermanager.h"

using namespace tohka;

void TimerManager::AddTimer(TimePoint when, TimerPrt_t tc) {
  tc->SetExpiredTime(when);
  // TODO set delay?
  timer_map_.emplace(when, std::move(tc));
}
ExpiredTimers TimerManager::GetExpiredTimers() {
  TimePoint now{TimePoint::now()};
  ExpiredTimers expired_times;
  for (auto it = timer_map_.begin(); it != timer_map_.end();) {
    if (it->first < now) {
      // push to expired_times vector
      expired_times.emplace_back(std::move(it->second));
      it = timer_map_.erase(it);
    } else {
      break;
    }
  }
  return expired_times;
}
int64_t TimerManager::GetNextExpiredDuration() {
  if (timer_map_.empty()) {
    return kDefaultTimeOutMs;
  }
  TimePoint now{TimePoint::now()};
  auto it = timer_map_.begin();
  int64_t left_time = it->first.GetMilliSeconds() - now.GetMilliSeconds();
  // It means that the execution time of the timer exceeds the expected
  // execution time of the next timer
  //  We will wake up loop
  if (left_time < 0) {
    return 0;
  }
  return left_time;
}
