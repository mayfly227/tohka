//
// Created by li on 2022/2/27.
//

#include "acceptor.h"

#include "ioloop.h"
#include "util/log.h"

using namespace tohka;
Acceptor::Acceptor(IoLoop* loop, NetAddress bind_address)
    : loop_(loop),
      socket_(Socket::CreateNonBlockFd(bind_address.GetFamily(), SOCK_STREAM,
                                       IPPROTO_TCP)),
      event_(loop_, socket_.GetFd()) {
// HINT: idle only support on unix
#if defined(OS_UNIX)
  idle_fd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
#endif
  // ipv4 or ipv6
  socket_.SetReusePort(true);
  socket_.BindAddress(bind_address);

  // set read callback
  event_.SetReadCallback([this] { OnAccept(); });
};

void Acceptor::OnAccept() {
  NetAddress peer_address{};
  // TODO ipv6 test?
  int conn_fd = socket_.Accept(&peer_address);
  if (conn_fd > 0 && conn_fd <= kMaxConn) {
    if (on_accept_) {
      on_accept_(conn_fd, peer_address);
    } else {
      log_warn("no OnAccept callback!");
#if defined(OS_UNIX)
      ::close(conn_fd);
#endif
    }
  } else {
    log_error("accept error!");
#if defined(OS_UNIX)
    if (errno == EMFILE) {
      log_warn("use idle fd...");
      ::close(idle_fd_);
      idle_fd_ = ::accept(socket_.GetFd(), nullptr, nullptr);
      ::close(idle_fd_);
      idle_fd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
    }
#endif
  }
}
Acceptor::~Acceptor() {
  event_.DisableAll();
  event_.UnRegister();
#if defined(OS_UNIX)
  ::close(idle_fd_);
#endif
}
void Acceptor::Listen() {
  socket_.Listen();
  event_.EnableReading();
}
