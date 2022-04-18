//
// Created by li on 2022/4/18.
//

#ifndef TOHKA_TOHKA_SOCKETUTIL_H
#define TOHKA_TOHKA_SOCKETUTIL_H
#include "platform.h"

namespace tohka {

class SockUtil {
 public:
  static int CreateNonBlockFd_(int domain, int type, int protocol);
  static void BindAddress_(int fd, const struct sockaddr* addr, size_t len);
  static void Listen_(int fd, int backlog);
  static int Accept_(int fd, struct sockaddr_in6* addr);
  static int Connect_(int fd, const struct sockaddr* addr, socklen_t sock_len);
  static int GetPeerName_(int fd, struct sockaddr* addr, socklen_t sock_len);
  static void ShutDownWrite_(int fd);
  static int GetSocketError_(int fd);
  static void SetNonBlockAndCloseOnExec_(int fd);
  static void Close_(int fd);
  static ssize_t Read_(int fd, void* buffer, size_t len);
  static ssize_t Write_(int fd, void* buffer, size_t len);
#ifdef OS_UNIX
  static ssize_t ReadV_(int fd, struct iovec* vec, int vec_cnt);
#endif
};

}  // namespace tohka

#endif  // TOHKA_TOHKA_SOCKETUTIL_H
