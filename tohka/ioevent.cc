//
// Created by li on 2022/2/20.
//

#include "ioevent.h"

#include "tohka/tohka.h"
#include "util/log.h"

using namespace tohka;

IoEvent::IoEvent(IoWatcher* io_watcher, int fd)
    : fd_(fd), events_(0), revents_(0), index_(-1), io_watcher_(io_watcher) {}

void IoEvent::ExecuteEvent() {
  log_trace("fd = %d IoEvent::ExecuteEvent() revents=0x%x", fd_, revents_);
  // FIXME TEST
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

void IoEvent::Register() { io_watcher_->RegisterEvent(this); }
void IoEvent::UnRegister() { io_watcher_->UnRegisterEvent(this); }
