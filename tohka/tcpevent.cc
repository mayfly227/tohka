//
// Created by li on 2022/3/1.
//

#include "tcpevent.h"

#include "tohka/iobuf.h"
using namespace tohka;

void tohka::DefaultOnConnection(const TcpEventPrt_t& conn) {
}

void tohka::DefaultOnMessage(const TcpEventPrt_t& conn, IoBuf* buf) {
  buf->ReceiveAllAsString();
}

TcpEvent::TcpEvent(IoLoop* loop, std::string name, int fd, NetAddress& peer)
    : loop_(loop),
      event_(std::make_unique<IoEvent>(loop_, fd)),
      socket_(std::make_unique<Socket>(fd)),
      peer_(peer),
      name_(std::move(name)),
      state_(kConnecting),
      high_water_mark_(64 * 1024 * 1024) {
  log_info("tcp event create fd=%d addr=%p name = %s", fd, this, name_.c_str());
  socket_->SetKeepAlive(true);
  socket_->SetTcpNoDelay(true);

  event_->SetReadCallback([this] { HandleRead(); });
  event_->SetWriteCallback([this] { HandleWrite(); });
}

void TcpEvent::HandleRead() {
  // TODO debug only(set read ext_buf size=1)
  ssize_t n;
#if defined(OS_UNIX)
  char ext_buf[65535];
  struct iovec vec[2];
  const size_t writeable_size = in_buf_.GetWriteableSize();
  vec[0].iov_base = in_buf_.Begin() + in_buf_.GetWriteIndex();
  vec[0].iov_len = writeable_size;
  vec[1].iov_base = ext_buf;
  vec[1].iov_len = sizeof(ext_buf);
  const int vec_number = (writeable_size < sizeof(ext_buf)) ? 2 : 1;
  n = socket_->ReadV(vec, vec_number);  // read from fd
  // 也就是说还没有占满预分配的vector
  if (n > 0) {
    if (n <= writeable_size) {
      in_buf_.SetWriteIndex(in_buf_.GetWriteIndex() + n);
    } else {
      in_buf_.SetWriteIndex(in_buf_.GetBufferSize());
      in_buf_.Append(ext_buf, n - (long)writeable_size);
    }
  }

#elif defined(OS_UNIX_TEST)
  char ext_buf[65535];
  n = socket_->Read(ext_buf, 65535);  // read from fd
  if (n > 0) {
    in_buf_.Append(ext_buf, n);
  }
#endif
  // check
  if (n > 0) {
    // call msg callback
    on_message_(shared_from_this(), &in_buf_);
  } else if (n == 0) {
    DoClose();
  } else {
    log_error("[TcpEvent::HandleRead]-> read < 0 errno=%d errmsg = %s", errno,
              strerror(errno));
    DoError();
  }
}
void TcpEvent::HandleWrite() {
  if (event_->IsWriting()) {
    ssize_t n = socket_->Write(out_buf_.Peek(), out_buf_.GetReadableSize());
    if (n >= 0) {
      out_buf_.Retrieve(n);
      // Once the data is written, Close_ the write event immediately to avoid
      // busy loop
      if (out_buf_.GetReadableSize() == 0) {
        StopWriting();
        if (on_write_done_) {
          on_write_done_(shared_from_this());
        }
        // Ensure that the data in the buffer has been sent out.
        // Then shutdown the connection
        if (state_ == kDisconnecting) {
          TryEagerShutDown();
        }
      }
    }
  } else {
    log_error("TcpEvent::HandleWrite error");
  }
}
void TcpEvent::DoClose() {
  assert(state_ == kConnected || state_ == kDisconnecting);
  SetState(kDisconnected);
  StopAllEvent();
  // call user callback

  on_connection_(shared_from_this());
  // TODO 这里调用了conn->ConnectDestroyed()用户的连接,目的是为了清除tcp连接
  on_close_(shared_from_this());
}
void TcpEvent::DoError() {
  // TODO 增加更多的测试条件
  DoClose();
}
TcpEvent::~TcpEvent() {
  log_info("tcp event delete fd=%d addr=%p name = %s", this->socket_->GetFd(),
           this, name_.c_str());
  assert(state_ == kDisconnected);
}

void TcpEvent::ConnectEstablished() {
  assert(state_ == kConnecting);
  SetState(kConnected);

  // 这里需要ioevent把tcpevent绑住，
  // 因为前者的执行event的时候可能后者已经被析构了
  event_->Tie(shared_from_this());
  StartReading();

  // on connection open(accepted callback)
  if (on_connection_) {
    on_connection_(shared_from_this());
  }
}
void TcpEvent::ConnectDestroyed() {
  // HINT tcpclient
  assert(state_ == kDisconnected);
  if (state_ == kConnected) {
    SetState(kDisconnected);
    event_->DisableAllEvent();
    on_connection_(shared_from_this());
  }
  assert(state_ == kDisconnected);
  // remove event from event map and  remove fd from pfds
  event_->UnRegister();
}
void TcpEvent::Send(std::string_view msg) { Send(msg.data(), msg.size()); }
void TcpEvent::Send(const void* data_dummy, size_t len) {
  char* data = (char*)data_dummy;
  // for force close
  if (state_ == kDisconnecting) {
    log_warn("Disconnecting, give up writing");
    return;
  }
  if (state_ == kDisconnected) {
    log_warn("Disconnected, give up writing");
    return;
  }

  size_t remaining = len;
  ssize_t n = 0;
  // If there is still data in the output buffer at this time,
  // it should not be sent directly, but the data is added to the buffer.
  if (!event_->IsWriting() && out_buf_.GetReadableSize() == 0) {
    // FIXME test
    n = socket_->Write(data, len);
    if (n >= 0) {
      // may be not write done
      remaining -= n;
      if (remaining == 0) {
        if (on_write_done_) {
          on_write_done_(shared_from_this());
        }
      }

    } else {
      n = 0;
      // Resource temporarily unavailable(EAGAIN)
      if (errno != EWOULDBLOCK) {
        log_error("TcpEvent::Send errno != EWOULDBLOCK");
      }
    }
  }

  // Put the unsent data into the output buffer and pay attention to the write
  // event
  assert(remaining <= len);
  if (remaining > 0) {
    // Judging whether the current cache data has exceeded the high watermark
    size_t exist = out_buf_.GetReadableSize();
    // FIXME why
    if (exist + remaining >= high_water_mark_) {
      // if(exist + remaining >= high_water_mark_ && exist < high_water_mark_){
      if (on_high_water_mark_) {
        on_high_water_mark_(shared_from_this());
      }
    }
    // append remaining data to buffer
    out_buf_.Append(data + n, remaining);
    StartWriting();
  }
}
void TcpEvent::Send(IoBuf* buffer) {
  Send(buffer->Peek(), buffer->GetReadableSize());
  buffer->Refresh();
}
void TcpEvent::Send(IoBuf& buffer) {
  Send(buffer.Peek(), buffer.GetReadableSize());
  buffer.Refresh();
}
void TcpEvent::ShutDown() {
  if (state_ == kConnected) {
    SetState(kDisconnecting);
    TryEagerShutDown();
  }
}
void TcpEvent::ForceClose() {
  if (state_ == kConnected || state_ == kDisconnecting) {
    SetState(kDisconnecting);
    // as if we received 0 byte in handleRead();
    DoClose();
  }
}
void TcpEvent::TryEagerShutDown() {
  // we are not writing
  // 保证没有发送完毕的数据能够发送出去
  if (!event_->IsWriting()) {
    socket_->ShutDownWrite();
  }
}

void TcpEvent::SetTcpNoDelay() { socket_->SetTcpNoDelay(true); }
