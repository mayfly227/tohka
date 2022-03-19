//
// Created by li on 2022/3/5.
//

#include "connector.h"

#include <memory>

#include "util/log.h"
using namespace tohka;

Connector::Connector(IoWatcher* io_watcher, NetAddress& peer)
    : io_watcher_(io_watcher),
      peer_(peer),
      connect_(false),
      state_(kDisconnected) {}
Connector::~Connector() {
  log_trace("Connector::~Connector");
  assert(!event_);
}

void Connector::OnConnect() {
  // call user callback
  log_trace("Connector::OnConnect");
  if (state_ == kConnecting) {
    // 这时连接已经成功，要把poll中的event去掉
    RemoveAndResetEvent();
    // TODO 处理错误
    int err = sock_->GetSocketError();
    //     handle error
    if (err) {
      log_trace("Connector::OnConnect err");
      // 一定后时间重连
      Retry();
    } else {
      if (connect_) {
        on_connect_(sock_->GetFd());
      } else {
        sock_->Close();
      }
    }
    SetState(kConnected);

  } else {
    assert(state_ == kDisconnected);
  }
}
void Connector::Connect() {
  sock_ = std::make_unique<Socket>(Socket::CreateNonBlockFd());

  int ret = sock_->Connect(peer_);
  int saved_errno = (ret == 0) ? 0 : errno;
  log_trace("errno = %d", errno);
  switch (saved_errno) {
    case 0:
    case EINPROGRESS:
    case EINTR:
    case EISCONN:
      Connecting();
      break;

    case EAGAIN:
    case EADDRINUSE:
    case EADDRNOTAVAIL:
    case ECONNREFUSED:
    case ENETUNREACH:
      Retry();
      break;

    case EACCES:
    case EPERM:
    case EAFNOSUPPORT:
    case EALREADY:
    case EBADF:
    case EFAULT:
    case ENOTSOCK:
      log_error("Connector::Connect error errno=%d", saved_errno);
      sock_->Close();
      break;

    default:
      log_error("Connector::Connect Unexpected error");
      sock_->Close();
      // connectErrorCallback_();
      break;
  }
}
void Connector::Start() {
  connect_ = true;
  assert(state_ == kDisconnected);
  Connect();
}
void Connector::Connecting() {
  SetState(kConnecting);
  // 设置写事件
  event_ = std::make_unique<IoEvent>(io_watcher_, sock_->GetFd());
  //  event_.reset(new IoEvent(io_watcher_,sock_.GetFd()));
  event_->SetWriteCallback([this] { OnConnect(); });
  event_->SetErrorCallback([this] { OnError(); });

  // 监听写事件，如果socket可写，那么就会触发OnConnect回调
  event_->EnableWriting();
}
void Connector::Retry() {
  //  event_
  log_trace("Connector::Retry");
  // 关闭当前的socket
  sock_->Close();
  SetState(kDisconnected);
  // 一定时间后重新尝试连接
  //  Start();
}
void Connector::OnError() {
  log_error("Connector::OnError state=%d", state_);
  if (state_ == kConnecting) {
    log_trace("Connector::OnError retry");
    RemoveAndResetEvent();
    //    int sockfd = removeAndResetChannel();
    //    int err = sockets::getSocketError(sockfd);
    //    LOG_TRACE << "SO_ERROR = " << err << " " << strerror_tl(err);
    //    retry(sockfd);
  }
}
void Connector::Stop() {
  connect_ = false;
  if (state_ == kConnecting) {
    SetState(kDisconnected);
    RemoveAndResetEvent();
    //    int sockfd = removeAndResetChannel();
    //    retry(sockfd);
  }
}
void Connector::RemoveAndResetEvent() {
  event_->DisableAll();
  event_->UnRegister();

  ResetEvent();
}
void Connector::ResetEvent() { event_.reset(); }
