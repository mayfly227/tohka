//
// Created by li on 2022/3/5.
//

#include "connector.h"

#include "ioloop.h"
#include "socketutil.h"
#include "util/log.h"
using namespace tohka;

Connector::Connector(IoLoop* loop, const NetAddress& peer)
    : loop_(loop),
      timer_id_(),
      retry_delay_ms_(kInitDelayMs),
      peer_(peer),
      state_(kDisconnected),
      connect_(false) {}
Connector::~Connector() {
  // 关闭定时器
  if (timer_id_.GetId() != 0) {
    IoLoop::GetLoop()->DeleteTimer(timer_id_);
  }
  log_info("connector delete");
  assert(!event_);
}

void Connector::OnConnect() {
  // call user callback
  if (state_ == kConnecting) {
    // HINT 这时候连接不一定成功(可能发生错误),但是我们任然要把poll中的event去掉
    int fd = RemoveAndResetEvent();
    // HINT 判断是否是真正连接成功
    int err = SockUtil::GetPeerName_(fd, peer_.GetAddress(), peer_.GetSize());
    if (err == -1) {
      log_error("Connector::OnConnect err");
      // 如果没有真正连接成功，那么一定后时间重连
      Retry(fd);
    } else {
      SetState(kConnected);
      if (connect_) {
        on_connect_(fd);
      } else {
        SockUtil::Close_(fd);
      }
    }
  } else {
    assert(state_ == kDisconnected);
  }
}
void Connector::OnConnectError() {}
void Connector::Connect() {
  int sockfd =
      SockUtil::CreateNonBlockFd_(peer_.GetFamily(), SOCK_STREAM, IPPROTO_TCP);
  int ret = SockUtil::Connect_(sockfd, peer_.GetAddress(), peer_.GetSize());
  int saved_errno = (ret == 0) ? 0 : errno;

  log_info("errno = %d errmsg = %s", errno, strerror(errno));
  switch (saved_errno) {
    case 0:
    case EINPROGRESS:
    case EINTR:
    case EISCONN:
      Connecting(sockfd);
      break;

    case EAGAIN:
    case EADDRINUSE:
    case EADDRNOTAVAIL:
    case ECONNREFUSED:
    case ENETUNREACH:
      Retry(sockfd);
      log_info("Connector::Connect() retry");
      break;

    case EACCES:
    case EPERM:
    case EAFNOSUPPORT:
    case EALREADY:
    case EBADF:
    case EFAULT:
    case ENOTSOCK:
      log_error("Connector::Connect error errno=%d", saved_errno);
      SockUtil::Close_(sockfd);
      break;

    default:
      log_error("Connector::Connect Unexpected error errno=%d errmsg = %s",
                errno, strerror(errno));
      SockUtil::Close_(sockfd);
      break;
  }
}
void Connector::Start() {
  connect_ = true;
  assert(state_ == kDisconnected);
  Connect();
}
void Connector::Connecting(int sock_fd) {
  SetState(kConnecting);
  // event必需为空
  assert(!event_);
  event_ = std::make_unique<IoEvent>(loop_, sock_fd);
  event_->SetWriteCallback([this] { OnConnect(); });
  // 设置写事件，当连接的socket失败或者成功时会调用OnConnect回调
  event_->SetReadCallback([this] { OnConnect(); });
  event_->EnableWriting();
  event_->EnableReading();
}
void Connector::Retry(int sock_fd) {
  //  event_
  // 关闭当前的socket
  SockUtil::Close_(sock_fd);
  SetState(kDisconnected);
  // 一定时间后重新尝试连接
  if (connect_) {
    timer_id_ =
        IoLoop::GetLoop()->CallLater(retry_delay_ms_, [this] { Start(); });
    retry_delay_ms_ = std::min(retry_delay_ms_ * 2, kMaxDelayMs);
  } else {
    log_warn("[Connector::Retry]->do not reconnect");
  }
}

void Connector::Stop() {
  connect_ = false;
  if (state_ == kConnecting) {
    SetState(kDisconnected);
    int fd = RemoveAndResetEvent();
    SockUtil::Close_(fd);
  }
  assert(event_ == nullptr);
  // 关闭定时器
  if (timer_id_.GetId() != 0) {
    IoLoop::GetLoop()->DeleteTimer(timer_id_);
  }
}
int Connector::RemoveAndResetEvent() {
  event_->DisableAllEvent();
  event_->UnRegister();
  int fd = event_->GetFd();
  // FIXME  Can't reset channel_ here, because we are inside
  // Ioevent::SafeExecuteEvent
  IoLoop::GetLoop()->CallLater(0, [this] { ResetEvent(); });

  return fd;
}
void Connector::ResetEvent() { event_.reset(); }

void Connector::Restart() {
  SetState(kDisconnected);
  connect_ = true;
  retry_delay_ms_ = kInitDelayMs;
  Start();
}
