//
// Created by li on 2022/2/17.
//

#include "ioloop.h"

#include "tohka/iowatcher.h"
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
thread_local IoLoop* current_loop_thread = nullptr;

class SignalHandler {
 public:
  SignalHandler() {
    // ::signal(SIGPIPE, sig_int);
    ::signal(SIGPIPE, SIG_IGN);
  }
};
SignalHandler SH;
}  // namespace
#endif
IoLoop::IoLoop()
    : io_watcher_(IoWatcher::ChooseIoWatcher()),
      timer_manager_(std::make_unique<TimerManager>()),
      running_(false) {
  // init log level
  log_set_level(LOG_INFO);
  if (current_loop_thread) {
    log_fatal("Another EventLoop exists in this thread! At:%p",
              current_loop_thread);
  } else {
    current_loop_thread = this;
  }
}

void IoLoop::RunForever() {
  if (current_loop_thread != this) {
    log_fatal("this thread has thread at %p", current_loop_thread);
    return;
  }
  running_ = true;
  std::vector<IoEvent*> activate_event_list;
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
    timer_manager_->DoExpiredTimers();
  }
}
TimerId IoLoop::CallAt(TimePoint when, TimerTask callback) {
  return timer_manager_->AddTimer(when, std::move(callback), 0);
}
TimerId IoLoop::CallLater(int delay, TimerTask callback) {
  auto expired = TimePoint::now() + delay;
  return CallAt(expired, std::move(callback));
}
TimerId IoLoop::CallEvery(int interval, TimerTask callback) {
  auto expired = TimePoint::now() + interval;
  return timer_manager_->AddTimer(expired, std::move(callback), interval);
}
IoWatcher* IoLoop::GetWatcherRawPoint() { return io_watcher_.get(); }
IoLoop* IoLoop::GetLoop() {
  if (!current_loop_thread) {
    static IoLoop loop;
    log_info("using static loop at %p", &loop);
    current_loop_thread = &loop;
  }
  return current_loop_thread;
}
void IoLoop::DeleteTimer(const TimerId& timer_id) {
  timer_manager_->DeleteTimer(timer_id);
}
