//
// Created by li on 2022/4/18.
//

#include "socketutil.h"

#include "platform.h"
#include "util/log.h"

using namespace tohka;

void SockUtil::SetNonBlockAndCloseOnExec_(int fd) {
  // non-block
#ifdef OS_UNIX
  int flags = ::fcntl(fd, F_GETFL, 0);
  flags |= O_NONBLOCK;
  int ret = ::fcntl(fd, F_SETFL, flags);
  if (ret < 0) {
    log_error("SetNonBlockAndCloseOnExec [non-block] error!");
  }

  // Close_-on-exec
  flags = ::fcntl(fd, F_GETFD, 0);
  flags |= FD_CLOEXEC;
  ret = ::fcntl(fd, F_SETFD, flags);
  if (ret < 0) {
    log_error("SetNonBlockAndCloseOnExec [Close_-on-exec] error!");
  }

#elif OS_WIN
  // TODO win32
#endif
}

void SockUtil::BindAddress_(int fd, const struct sockaddr* addr, size_t len) {
  int ret = ::bind(fd, addr, len);
  if (ret < 0) {
    log_error("bind error! errno=%d errstr = %s", errno, strerror(errno));
  }
}

void SockUtil::Listen_(int fd, int backlog) {
  int ret = ::listen(fd, backlog);
  if (ret < 0) {
    log_error("listen error! errno=%d errstr = %s", errno, strerror(errno));
  }
}

int SockUtil::Accept_(int fd, struct sockaddr_in6* addr) {
  socklen_t sock_len = sizeof(*addr);

  int conn_fd = ::accept(fd, (sockaddr*)addr, &sock_len);
  SetNonBlockAndCloseOnExec_(conn_fd);
  if (conn_fd < 0) {
    log_error("[Accept]->accept error! errno=%d errstr = %s", errno,
              strerror(errno));
  }
  return conn_fd;
}

int SockUtil::Connect_(int fd, const struct sockaddr* addr,
                       socklen_t sock_len) {
  return ::connect(fd, addr, sock_len);
}

ssize_t SockUtil::Read_(int fd, void* buffer, size_t len) {
  return ::read(fd, buffer, len);
}
ssize_t SockUtil::Write_(int fd, void* buffer, size_t len) {
  return ::write(fd, buffer, len);
}

int SockUtil::CreateNonBlockFd_(int domain, int type, int protocol) {
  int sock = ::socket(domain, type, protocol);
  if (sock < 0) {
    log_error("create socket error! errno=%d errstr = %s", errno,
              strerror(errno));
  } else {
    SetNonBlockAndCloseOnExec_(sock);
  }
  return sock;
}

int SockUtil::GetSocketError_(int fd) {
  int opt_val;
  auto opt_len = static_cast<socklen_t>(sizeof opt_val);

  if (::getsockopt(fd, SOL_SOCKET, SO_ERROR, &opt_val, &opt_len) < 0) {
    return errno;
  } else {
    return opt_val;
  }
}
void SockUtil::ShutDownWrite_(int fd) {
  if (::shutdown(fd, SHUT_WR) < 0) {
    log_error("[ShutDownWrite]->shutdownWrite fd=%d", fd);
  }
}

int SockUtil::GetPeerName_(int fd, struct sockaddr* addr, socklen_t sock_len) {
  socklen_t len = sock_len;
  if (::getpeername(fd, addr, &len) < 0) {
    log_error("[ GetPeerName] error fd = %d errmsg = %s", fd, strerror(errno));
    return -1;
  } else {
    return 0;
  }
}

void SockUtil::Close_(int fd) { ::close(fd); }
#ifdef OS_UNIX
ssize_t SockUtil::ReadV_(int fd, struct iovec* vec, int vec_cnt) {
  return ::readv(fd, vec, vec_cnt);
}
#endif
