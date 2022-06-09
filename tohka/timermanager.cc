//
// Created by li on 2022/2/20.
//

#include "timermanager.h"

#include "timer.h"
#include "util/log.h"
using namespace tohka;

TimerId TimerManager::AddTimer(TimePoint when, TimerCallback cb,
                               int32_t interval) {
  auto timer = std::make_shared<Timer>(when, std::move(cb), interval);
  assert(timer_map_.size() == activate_timers_.size());

  timer_map_.emplace(when, timer);
  int64_t timer_id = timer->GetTimerId();
  activate_timers_.emplace(timer_id, timer);

  return {timer, timer_id};
}
ExpiredTimers TimerManager::GetExpiredTimers() {
  TimePoint now{TimePoint::now()};
  ExpiredTimers expired_times;
  for (auto it = timer_map_.begin(); it != timer_map_.end();) {
    if (it->first < now) {
      // push to expired_times vector
      expired_times.emplace_back(it->second);
      it = timer_map_.erase(it);
    } else {
      break;
    }
  }
  for (const auto& item : expired_times) {
    int64_t activate_timer_id = item->GetTimerId();
    activate_timers_.erase(activate_timer_id);
  }
  assert(timer_map_.size() == activate_timers_.size());
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
void TimerManager::DeleteTimer(const TimerId& timer_id) {
  TimerPrt_t timer = timer_id.timer_.lock();
  if (!timer) {
    log_fatal("not valid timer id=%d", timer_id.sequence_);
    return;
  }
  auto it = activate_timers_.find(timer_id.sequence_);
  if (it != activate_timers_.end()) {
    // 因为有可能有多个时间相同的timer，这里要找出确定的那个timer_id
    auto range = timer_map_.equal_range(it->second->GetExpiredTime());
    for (auto i = range.first; i != range.second; ++i) {
      if (i->second == it->second) {
        activate_timers_.erase(it);
        timer_map_.erase(i);
        break;
      }
    }
  } else if (calling_expired_timers_) {
    cancel_timers_.emplace(timer->GetTimerId(), timer);
  }
  assert(timer_map_.size() == activate_timers_.size());
}
TimerManager::TimerManager()
    : timer_map_(), activate_timers_(), calling_expired_timers_(false) {}

void TimerManager::DoExpiredTimers() {
  auto expired_timers = GetExpiredTimers();

  calling_expired_timers_ = true;
  cancel_timers_.clear();
  for (const auto& expired_timer : expired_timers) {
    expired_timer->run();
  }
  calling_expired_timers_ = false;
  Reset(expired_timers);
}
void TimerManager::Reset(ExpiredTimers& expired_timers) {
  auto now = TimePoint::now();
  assert(timer_map_.size() == activate_timers_.size());
  for (const auto& expired_timer : expired_timers) {
    if (expired_timer->IsRepeat() &&
        cancel_timers_.find(expired_timer->GetTimerId()) ==
            cancel_timers_.end()) {
      expired_timer->Restart(now);

      timer_map_.emplace(expired_timer->GetExpiredTime(), expired_timer);
      activate_timers_.emplace(expired_timer->GetTimerId(), expired_timer);
    }
  }
}
