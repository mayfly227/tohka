//
// Created by li on 2022/2/27.
//

#ifndef TOHKA_TOHKA_ACCEPTOR_H
#define TOHKA_TOHKA_ACCEPTOR_H

#include <sys/socket.h>

#include <functional>

#include "ioevent.h"
#include "log.h"
#include "netaddress.h"
#include "noncopyable.h"
#include "socket.h"
#include "tohka.h"

namespace tohka {
class IoWatcher;

class Acceptor : noncopyable {
 public:
  Acceptor(IoWatcher* io_watcher, NetAddress bind_address);
  ~Acceptor();

  void SetOnAccept(const OnAcceptCallback& on_accept) {
    on_accept_ = on_accept;
  }

  void Listen();

 private:
  void OnAccept();

  Socket socket_;
  IoEvent event_;
  IoWatcher* io_watcher_;

  OnAcceptCallback on_accept_;
};
}  // namespace tohka
#endif  // TOHKA_TOHKA_ACCEPTOR_H
