//
// Created by li on 2022/2/27.
//

#ifndef TOHKA_TOHKA_SOCKET_H
#define TOHKA_TOHKA_SOCKET_H

#include "netaddress.h"
#include "noncopyable.h"
// socket and it's ops

namespace tohka {
class Socket : noncopyable {
 public:
  Socket() = default;
  Socket(int domain, int type, int protocol);
  explicit Socket(int fd);
  ~Socket();

  int GetFd() const { return fd_; }
  void BindAddress(NetAddress& address) const;
  void Listen() const;
  int Accept(NetAddress* peer_address) const;

  ssize_t Read(void* buffer, size_t len) const;
  ssize_t Write(void* buffer, size_t len) const;
  void SetTcpNoDelay(bool on) const;

  void SetReuseAddress(bool on) const;

  void SetReusePort(bool on) const;
  void SetKeepAlive(bool on) const;

  static void SetNonBlockAndCloseOnExec(int sock_fd);
  static int CreateNonBlockFd(int domain = AF_INET, int type = SOCK_STREAM,
                              int protocol = IPPROTO_TCP);

 private:
  int fd_;
};
}  // namespace tohka
#endif  // TOHKA_TOHKA_SOCKET_H
