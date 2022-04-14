//
// Created by li on 2022/2/20.
//

#ifndef TOHKA_TOHKA_IOEVENT_H
#define TOHKA_TOHKA_IOEVENT_H

#include "iowatcher.h"
#include "noncopyable.h"
#include "tohka.h"
#include "util/log.h"
namespace tohka {
class IoEvent : noncopyable {
 public:
  IoEvent(IoLoop* loop, int fd);
  ~IoEvent() { log_debug("~IoEvent at %p", this); }

  // Register to the monitor event of Poll
  void Register();

  // Unregister to the monitor event of Poll
  void UnRegister();

  void ExecuteEvent();

  void EnableReading() {
    events_ |= EV_READ;
    Register();
  };
  void DisableReading() {
    events_ &= ~EV_READ;
    Register();
  };
  void EnableWriting() {
    events_ |= EV_WRITE;
    Register();
  };
  void DisableWriting() {
    events_ &= ~EV_WRITE;
    Register();
  };
  void DisableAll() {
    events_ = EV_NONE;
    Register();
  };
  void SetReadCallback(EventCallback read_callback) {
    read_callback_ = std::move(read_callback);
  }
  void SetWriteCallback(EventCallback write_callback) {
    write_callback_ = std::move(write_callback);
  }

  // HINT: 延长ioevent的生命周期
  void Tie(const std::shared_ptr<void>& tie);
  short GetEvents() const { return events_; }
  void SetEvents(short events) { events_ = events; }
  short GetRevents() const { return revents_; }
  void SetRevents(short revents) { revents_ = revents; }
  int GetFd() const { return fd_; }
  int GetIndex() const { return index_; }
  void SetIndex(int index) { index_ = index; }
  bool IsWriting() const { return events_ & EV_WRITE; }
  bool IsReading() const { return events_ & EV_READ; }

  IoLoop* loop_;

 private:
  int fd_;
  short events_;
  short revents_;
  void SafeExecuteEvent();
  std::weak_ptr<void> tie_obj_;
  bool tied_;
  int index_;  // for poller
  EventCallback read_callback_;
  EventCallback write_callback_;
};
}  // namespace tohka
#endif  // TOHKA_TOHKA_IOEVENT_H
