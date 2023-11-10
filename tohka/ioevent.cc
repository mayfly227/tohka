//
// Created by li on 2022/2/20.
//

#include "ioevent.h"

#include "ioloop.h"
#include "tohka/tohka.h"
#include "util/log.h"

using namespace tohka;

IoEvent::IoEvent(IoLoop* loop, int fd)
    : loop_(loop),
      fd_(fd),
      events_(0),
      revents_(0),
      event_handling_(false),
      tied_(false),
      index_(-1) {}

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
  event_handling_ = true;
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
  event_handling_ = false;
}
IoEvent::~IoEvent() {
  { log_debug("~IoEvent at %p fd = %d", this, fd_); }
  assert(!event_handling_);
}
