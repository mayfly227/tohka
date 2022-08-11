//
// Created by li on 2022/4/22.
//

#include "freedom.h"

#include <utility>

#include "tohka/ioloop.h"

void FreeDom::on_connection(const TcpEventPrt_t& conn) {
  if (conn->Connected()) {
    log_warn("FreeDom connected");
    conn->SetTcpNoDelay();
    self_ = conn;

    if (in_->GetInConn(id_).has_value()) {
      auto other = in_->GetInConn(id_).value();
      // in结点开启读事件
      other->StartReading();
      if (other->GetInputBuf()->GetReadableSize() > 0) {
        Process();
      }
    }

  } else {
    log_warn("FreeDom disconnected");
    client_->SetOnConnection(DefaultOnConnection);
    client_->SetOnMessage(DefaultOnMessage);
    if (in_->GetInConn(id_).has_value()) {
      auto other = in_->GetInConn(id_).value();
      // 关闭in结点
      other->ShutDown();
    }
    self_.reset();
  }
}
void FreeDom::on_recv(const TcpEventPrt_t& conn, IoBuf* buf) {
  // HINT 调用in结点来处理数据
  // assert(other_);
  assert(self_);
  if (in_->GetInConn(id_).has_value()) {
    in_->Process(id_);
  } else {
    log_fatal("exit -1 freedom");
    self_->ForceClose();
    //    exit(-1);
  }
}
void FreeDom::StartClient() {
  auto self(shared_from_this());
  // 如果连不上就让client一直连，直到in结点主动关闭
  client_->SetOnConnection(
      [this, self](const TcpEventPrt_t& conn) { on_connection(conn); });
  client_->SetOnMessage([this, self](const TcpEventPrt_t& conn, IoBuf* buf) {
    on_recv(conn, buf);
  });
  client_->Connect();
}

void FreeDom::Process() {
  // 表示in结点已经收到了数据
  // 数据交给本类处理
  if (!self_) {
    return;
  }
  if (in_->GetInConn(id_).has_value()) {
    auto other = in_->GetInConn(id_).value();
    IoBuf* buf_in = other->GetInputBuf();
    assert(self_);
    self_->Send(buf_in);
  }
}

void FreeDom::DisConnected() {
  if (self_) {
    client_->Disconnect();
  } else {
    client_->SetOnConnection(DefaultOnConnection);
    client_->SetOnMessage(DefaultOnMessage);
    client_->Stop();
  }
}
TcpEventPrt_t FreeDom::GetConn() { return self_; }

FreeDom::~FreeDom() { log_warn("~FreeDom"); }
FreeDom::FreeDom(string id, NetAddress dest, InHandler* in) {
  id_ = std::move(id);
  dest_ = dest;
  in_ = in;
  client_ = std::make_unique<TcpClient>(IoLoop::GetLoop(), dest_, "freedom");
}
