//
// Created by li on 2022/2/27.
//

#include "acceptor.h"
using namespace tohka;

Acceptor::Acceptor(IoWatcher* io_watcher, NetAddress bind_address)
    : socket_(Socket::CreateNonBlockFd(AF_INET, SOCK_STREAM, IPPROTO_TCP)),
      event_(io_watcher, socket_.GetFd()),
      io_watcher_(io_watcher) {
  // ipv4 or ipv6
  socket_.SetReusePort(true);
  socket_.BindAddress(bind_address);

  // set read callback
  event_.SetReadCallback([this] { OnAccept(); });
  event_.EnableReading();
};

void Acceptor::OnAccept() {
  NetAddress peer_address;
  // TODO support ipv6?
  int conn_fd = socket_.Accept(&peer_address);
  if (conn_fd > 0) {
    if (on_accept_) {
      on_accept_(conn_fd, peer_address);
    } else {
      log_warn("no OnAccept callback!");
    }
  } else {
    // TODO handle error(idle id)
    log_error("accept error!");
  }
}
Acceptor::~Acceptor() {
  event_.DisableAll();
  event_.UnRegister();
}
void Acceptor::Listen() { socket_.Listen(); }
