//
// Created by li on 2022/2/27.
//

#ifndef TOHKA_TOHKA_ACCEPTOR_H
#define TOHKA_TOHKA_ACCEPTOR_H

#include "ioevent.h"
#include "netaddress.h"
#include "noncopyable.h"
#include "socket.h"
#include "tohka.h"
#include "util/log.h"

namespace tohka {
class Acceptor : noncopyable {
 public:
  Acceptor(IoLoop* loop, NetAddress bind_address);
  ~Acceptor();

  void SetOnAccept(const OnAcceptCallback& on_accept) {
    on_accept_ = on_accept;
  }

  void Listen();

 private:
  void OnAccept();
  static constexpr int kMaxConn = 200000;
  static constexpr int kBackLog = 512;
  IoLoop* loop_;
  Socket socket_;
  IoEvent event_;
  int idle_fd_;  // For discarding failed connections

  OnAcceptCallback on_accept_;
};
}  // namespace tohka
#endif  // TOHKA_TOHKA_ACCEPTOR_H
