//
// Created by li on 2022/2/27.
//

#ifndef TOHKA_TOHKA_SOCKET_H
#define TOHKA_TOHKA_SOCKET_H

#include "netaddress.h"
#include "noncopyable.h"
#include "util/log.h"

// socket and it's ops
namespace tohka {
class Socket : noncopyable {
 public:
  Socket();
  Socket(int domain, int type, int protocol);
  explicit Socket(int fd);
  ~Socket();

  int GetFd() const { return fd_; }
  void BindAddress(NetAddress& address) const;
  void Listen(int backlog) const;
  int Accept(NetAddress* peer_address) const;
  int Connect(NetAddress& peer_address) const;
  void ShutDownWrite() const;
  void Close() const;
  ssize_t Read(void* buffer, size_t len) const;
  ssize_t Write(void* buffer, size_t len) const;
#ifdef OS_UNIX
  ssize_t ReadV(const struct iovec* vec, int vec_cnt) const;
#endif
  void SetTcpNoDelay(bool on) const;

  void SetReuseAddress(bool on) const;

  void SetReusePort(bool on) const;
  void SetKeepAlive(bool on) const;

  int GetSocketError() const;
  int GetPeerName(NetAddress& peer) const;

 private:
  int fd_;
};
}  // namespace tohka
#endif  // TOHKA_TOHKA_SOCKET_H
