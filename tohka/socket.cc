//
// Created by li on 2022/2/27.
//

#include "socket.h"

#include "socketutil.h"
#include "util/log.h"
using namespace tohka;

Socket::Socket() : fd_(-1) {}
Socket::Socket(int domain, int type, int protocol) {
  int sock = ::socket(domain, type, protocol);
  if (sock < 0) {
    log_error("create socket error! errno=%d errstr = %s", errno,
              strerror(errno));
  }
  fd_ = sock;
}

Socket::Socket(int fd) : fd_(fd) {}

Socket::~Socket() {
  // Close_ socket
  assert(fd_ != -1);
  Close();
  log_debug("Socket::~Socket close socket fd = %d", fd_);
}
void Socket::BindAddress(NetAddress& address) const {
  SockUtil::BindAddress_(fd_, address.GetAddress(), address.GetSize());
}

void Socket::Listen(int backlog) const { SockUtil::Listen_(fd_, backlog); }

int Socket::Accept(NetAddress* peer_address) const {
  struct sockaddr_in6 socket_address6 {};
  ::memset(&socket_address6, 0, sizeof(socket_address6));
  int conn_fd = SockUtil::Accept_(fd_, &socket_address6);
  //     TODO For debug only
  //      int opt = 3;
  //      if (::setsockopt(conn_fd, SOL_SOCKET, SO_SNDBUF, &opt,
  //                       (socklen_t)(sizeof(opt))) < 0) {
  //        log_error("SocketFd SetSO_SNDBUF error");
  //      }

  if (conn_fd < 0) {
    log_error("[Socket::Accept]->accept error! errno=%d errstr = %s", errno,
              strerror(errno));
  }
  peer_address->SetSockAddrInet6(socket_address6);
  return conn_fd;
}

int Socket::Connect(NetAddress& peer_address) const {
  socklen_t sock_len = peer_address.GetSize();
  return SockUtil::Connect_(fd_, peer_address.GetAddress(), sock_len);
}

void Socket::SetTcpNoDelay(bool on) const {
  int opt = on ? 1 : 0;
  if (::setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &opt,
                   (socklen_t)(sizeof(opt))) < 0) {
    log_error("SocketFd SetReusePort error");
  }
}

void Socket::SetReuseAddress(bool on) const {
  int opt = on ? 1 : 0;
  if (::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt,
                   (socklen_t)(sizeof(opt))) < 0) {
    log_error("Socket::SetReuseAddress SetReuseAddress error");
  }
}

void Socket::SetReusePort(bool on) const {
  int opt = on ? 1 : 0;
  if (::setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &opt,
                   (socklen_t)(sizeof(opt))) < 0) {
    log_error("Socket::SetReuseAddress SetReusePort error");
  }
}

void Socket::SetKeepAlive(bool on) const {
  int opt = on ? 1 : 0;
  if (::setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &opt,
                   (socklen_t)(sizeof(opt))) < 0) {
    log_error("Socket::SetKeepAlive SetKeepAlive error");
  }
}

ssize_t Socket::Read(void* buffer, size_t len) const {
  return ::read(fd_, buffer, len);
}
ssize_t Socket::Write(void* buffer, size_t len) const {
  return ::write(fd_, buffer, len);
}
void Socket::Close() const { SockUtil::Close_(fd_); }
int Socket::GetSocketError() const {
  int opt_val;
  auto opt_len = static_cast<socklen_t>(sizeof opt_val);

  if (::getsockopt(fd_, SOL_SOCKET, SO_ERROR, &opt_val, &opt_len) < 0) {
    return errno;
  } else {
    return opt_val;
  }
}
void Socket::ShutDownWrite() const { SockUtil::ShutDownWrite_(fd_); }

int Socket::GetPeerName(NetAddress& peer) const {
  socklen_t sock_len = peer.GetSize();
  return SockUtil::GetPeerName_(fd_, peer.GetAddress(), sock_len);
}

#ifdef OS_UNIX
ssize_t Socket::ReadV(const struct iovec* vec, int vec_cnt) const {
  return ::readv(fd_, vec, vec_cnt);
}
#endif
