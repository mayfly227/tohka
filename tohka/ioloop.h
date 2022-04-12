//
// Created by li on 2022/2/17.
//

#ifndef TOHKA_TOHKA_IOLOOP_H
#define TOHKA_TOHKA_IOLOOP_H

#include "ioevent.h"
#include "iowatcher.h"
#include "platform.h"
#include "poll.h"
#include "timepoint.h"
#include "timermanager.h"

namespace tohka {
class IoLoop : public noncopyable {
 public:
  using TimerTask = std::function<void()>;
  IoLoop();
  ~IoLoop() = default;
  void RunForever();
  void Quit() { running_ = false; };

  //  void CallSoon();
  TimerId CallAt(TimePoint when, TimerTask callback);
  TimerId CallLater(int delay, TimerTask callback);
  TimerId CallEvery(int interval, TimerTask callback);
  void DeleteTimer(const TimerId& timer_id);

  IoWatcher* GetWatcherRawPoint();
  static IoLoop* GetLoop();

 private:
  using IoWatcherPtr = std::unique_ptr<IoWatcher>;
  using TimerManagerPtr = std::unique_ptr<TimerManager>;
  IoWatcherPtr io_watcher_;
  TimerManagerPtr timer_manager_;

  bool running_;
};
}  // namespace tohka
#endif  // TOHKA_TOHKA_IOLOOP_H
