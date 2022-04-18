//
// Created by li on 2022/2/20.
//

#include "ioevent.h"

#include "ioloop.h"
#include "tohka/tohka.h"
#include "util/log.h"

using namespace tohka;

IoEvent::IoEvent(IoLoop* loop, int fd)
    : loop_(loop), fd_(fd), events_(0), revents_(0), tied_(false), index_(-1) {}

void IoEvent::ExecuteEvent() {
  std::shared_ptr<void> guard;
  // 避免每次都去lock
  if (tied_) {
    guard = tie_obj_.lock();
    if (guard) {
      SafeExecuteEvent();
    }
  } else {
    SafeExecuteEvent();
  }
}

void IoEvent::Register() { loop_->GetWatcherRawPoint()->RegisterEvent(this); }
void IoEvent::UnRegister() {
  loop_->GetWatcherRawPoint()->UnRegisterEvent(this);
}
void IoEvent::Tie(const std::shared_ptr<void>& tie) {
  tie_obj_ = tie;
  tied_ = true;
}
void IoEvent::SafeExecuteEvent() {
  log_trace("fd = %d IoEvent::ExecuteEvent() events=0x%x revents=0x%x", fd_,
            events_, revents_);
  if ((events_ & EV_READ) && (revents_ & EV_READ)) {
    if (read_callback_) {
      read_callback_();
    }
  }
  if ((events_ & EV_WRITE) && (revents_ & EV_WRITE)) {
    if (write_callback_) {
      write_callback_();
    }
  }
}
