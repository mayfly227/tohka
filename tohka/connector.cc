//
// Created by li on 2022/3/5.
//

#include "connector.h"

#include "ioloop.h"
#include "util/log.h"
using namespace tohka;

Connector::Connector(IoLoop* loop, const NetAddress& peer)
    : loop_(loop),
      retry_delay_ms_(kInitDelayMs),
      peer_(peer),
      state_(kDisconnected),
      connect_(false),
      enable_connect_timeout_(false),
      connect_timeout_ms_(kDefaultTimeoutMs) {}
Connector::~Connector() {
  log_trace("Connector::~Connector");
  assert(!event_);
}

void Connector::OnConnect() {
  // call user callback
  log_trace("Connector::OnConnect");
  if (state_ == kConnecting) {
    // HINT 这时候连接不一定成功，但是我们任然要把poll中的event去掉
    RemoveAndResetEvent();
    // HINT 判断是否是真正连接成功
    int err = sock_->GetPeerName(peer_);
    if (err) {
      log_trace("Connector::OnConnect err");
      // 如果没有真正连接成功，那么一定后时间重连
      Retry();
    } else {
      SetState(kConnected);
      if (connect_) {
        on_connect_(sock_->GetFd());
      } else {
        sock_->Close();
      }
    }
  } else {
    assert(state_ == kDisconnected);
  }
}
void Connector::Connect() {
  sock_ = std::make_unique<Socket>(
      Socket::CreateNonBlockFd(peer_.GetFamily(), SOCK_STREAM, IPPROTO_TCP));

  int ret = sock_->Connect(peer_);
  int saved_errno = (ret == 0) ? 0 : errno;

  // set connect timeout
  if (enable_connect_timeout_) {
    IoLoop::GetLoop()->CallLater(connect_timeout_ms_,
                                 [this] { OnConnectTimeout(); });
  }
  log_trace("errno = %d errmsg = %s", errno, strerror(errno));
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
      log_error("Connector::Connect Unexpected error errno=%d errmsg = %s",
                errno, strerror(errno));
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
  event_ = std::make_unique<IoEvent>(loop_, sock_->GetFd());
  event_->SetWriteCallback([this] { OnConnect(); });
  // 设置写事件，当连接的socket失败或者成功时会调用OnConnect回调
  event_->EnableWriting();
}
void Connector::Retry() {
  //  event_
  log_trace("Connector::Retry");
  // 关闭当前的socket
  sock_->Close();
  SetState(kDisconnected);
  // 一定时间后重新尝试连接
  if (connect_) {
    IoLoop::GetLoop()->CallLater(retry_delay_ms_, [this] { Start(); });
    retry_delay_ms_ = std::min(retry_delay_ms_ * 2, kMaxDelayMs);
  } else {
    log_debug("[Connector::Retry]->do not reconnect");
  }
}

void Connector::Stop() {
  connect_ = false;
  if (state_ == kConnecting) {
    SetState(kDisconnected);
    RemoveAndResetEvent();
    Retry();
  }
}
void Connector::RemoveAndResetEvent() {
  event_->DisableAll();
  event_->UnRegister();
  ResetEvent();
}
void Connector::ResetEvent() { event_.reset(); }

void Connector::OnClose() {
  log_trace("Connector::OnClose call stop");
  Stop();
}
void Connector::Restart() {
  SetState(kDisconnected);
  connect_ = true;
  retry_delay_ms_ = kInitDelayMs;
  Start();
}

void Connector::OnConnectTimeout() {
  log_warn("[ConnectTimeout]->connect");
  if (state_ == kConnected) {
    log_debug("[ConnectTimeout]->connect %s ok! status=kConnected",
              peer_.GetIpAndPort().c_str());
  }
  if (state_ == kConnecting || state_ == kDisconnected) {
    log_debug(
        "[Connector::OnConnectTimeout]-> connect %s timeout now status= "
        "kConnecting and try to reconnected!",
        peer_.GetIpAndPort().c_str());
    SetState(kDisconnected);
    RemoveAndResetEvent();
    Retry();
  }
}