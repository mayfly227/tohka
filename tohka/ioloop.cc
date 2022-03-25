//
// Created by li on 2022/2/17.
//

#include "ioloop.h"

#include "util/log.h"

using namespace tohka;

#ifdef OS_UNIX
namespace {
//信号处理函数
// void sig_handler(int signo);

// typedef void Sigfunc(int);

// Sigfunc* signal(int signo, Sigfunc* func);
// Sigfunc* signal(int signo, Sigfunc* func) {
//   struct sigaction act {
//   }, oact{};
//   act.sa_handler = func;
//   act.sa_flags = 0;
//   sigemptyset(&act.sa_mask);
//   if (signo == SIGALRM) {
// #ifdef SA_INTERRUPT
//     act.sa_flags |= SA_INTERRUPT;
// #endif
//   } else {
// #ifdef SA_RESTART
//     act.sa_flags |= SA_RESTART;
// #endif
//   }
//   if (sigaction(signo, &act, &oact) < 0) {
//     return SIG_ERR;
//   }
//   return oact.sa_handler;
// }
// void sig_int(int signo) {
//   printf("signo=%d\n", signo);
//   exit(0);
// }

class IgnoreSigPipe {
 public:
  IgnoreSigPipe() {
    // ::signal(SIGPIPE, sig_int);
    ::signal(SIGPIPE, SIG_IGN);
    // LOG_TRACE << "Ignore SIGPIPE";
  }
};
IgnoreSigPipe isp;
}  // namespace
#endif

IoLoop::IoLoop()
    : io_watcher_(std::make_unique<Poll>()),
      timer_manager_(
          std::make_unique<TimerManager>()),  // TODO choose poller by os
      running_(false) {
  // init log level
  log_set_level(LOG_INFO);
}

void IoLoop::RunForever() {
  running_ = true;
  std::vector<IoEvent*> activate_event_list;
  int i = 100;
  while (running_) {
    activate_event_list.clear();
    int64_t next_expired_duration = timer_manager_->GetNextExpiredDuration();

    // get activate event and fill those to activate_event_list
    io_watcher_->PollEvents((int)next_expired_duration, &activate_event_list);

    // do io event
    for (auto event : activate_event_list) {
      event->ExecuteEvent();
    }
    // do timer
    for (auto& timer : timer_manager_->GetExpiredTimers()) {
      timer->run();
      if (timer->IsRepeat()) {
        const auto when = timer->GetExpiredTime() + (int)timer->GetDelay();
        timer_manager_->AddTimer(when, std::move(timer));
      }
      // timer delete there
    }
  }
}
void IoLoop::CallAt(TimePoint when, IoLoop::TimerTask callback) {
  auto timer = std::make_unique<Timer>(when, std::move(callback));
  timer_manager_->AddTimer(when, move(timer));
}
void IoLoop::CallLater(int delay, IoLoop::TimerTask callback) {
  auto now = TimePoint::now();
  CallAt(now + delay, std::move(callback));
}
void IoLoop::CallEvery(int interval, IoLoop::TimerTask callback) {
  auto when = TimePoint::now() + interval;
  auto timer =
      std::make_unique<Timer>(when, std::move(callback), interval, true);
  timer_manager_->AddTimer(when, std::move(timer));
}
IoWatcher* IoLoop::GetWatcherRawPoint() { return io_watcher_.get(); }
IoLoop* IoLoop::GetLoop() {
  static IoLoop loop;
  return &loop;
}
