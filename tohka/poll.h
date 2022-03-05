//
// Created by li on 2022/2/24.
//

#ifndef TOHKA_TOHKA_POLL_H
#define TOHKA_TOHKA_POLL_H
#include <vector>

#include "iowatcher.h"
#include "noncopyable.h"
#include "platform.h"

#ifdef OS_UNIX
#include "sys/poll.h"
#endif

namespace tohka {
class Poll : public IoWatcher {
 public:
  Poll();
  TimePoint PollEvents(int timeout, EventList* event_list) override;
  void RegisterEvent(IoEvent* io_event) override;
  void UnRegisterEvent(IoEvent* io_event) override;

 private:
  static constexpr int kInitialSize = 64;
  using Pfds = std::vector<struct pollfd>;
  Pfds pfds_;
};
}  // namespace tohka
#endif  // TOHKA_TOHKA_POLL_H
