//
// Created by li on 2022/5/30.
//

#include "run_out.h"

#include <utility>

#include "tohka/ioloop.h"

void RunOut::on_connection(const TcpEventPrt_t& conn) {
  if (conn->Connected()) {
    log_warn("run_out connected");
    conn->SetTcpNoDelay();
    self_ = conn;
    // in结点开启读事件
    if (in_->GetInConn(id_).has_value()) {
      auto other = in_->GetInConn(id_).value();
      other->StartReading();
      if (other->GetInputBuf()->GetReadableSize() > 0) {
        Process();
      }
    }
  } else {
    log_warn("run_out disconnected");
    client_->SetOnConnection(DefaultOnConnection);
    client_->SetOnMessage(DefaultOnMessage);
    // 关闭in结点
    if (in_->GetInConn(id_).has_value()) {
      auto other = in_->GetInConn(id_).value();
      other->ShutDown();
    }
    self_.reset();
  }
}
void RunOut::on_recv(const TcpEventPrt_t& conn, IoBuf* buf) {
  // HINT 调用in结点来处理数据
  // TODO 这里应该对数据进行解密
  // assert(other_);
  assert(self_);
  if (in_->GetInConn(id_).has_value()) {
    in_->Process(id_);
  } else {
    log_fatal("exit -1 run out");
    self_->ForceClose();
    //    exit(-1);
  }
}
void RunOut::StartClient() {
  auto self(shared_from_this());

  // 如果连不上就让client一直连，直到in结点主动关闭
  client_->SetOnConnection(
      [this, self](const TcpEventPrt_t& conn) { on_connection(conn); });
  client_->SetOnMessage([this, self](const TcpEventPrt_t& conn, IoBuf* buf) {
    on_recv(conn, buf);
  });
  client_->Connect();
}

void RunOut::Process() {
  // 表示in结点已经收到了数据
  // 数据交给本类处理
  if (!self_) {
    return;
  }
  if (in_->GetInConn(id_).has_value()) {
    auto other = in_->GetInConn(id_).value();
    IoBuf* buf_in = other->GetInputBuf();

    if (state_ == kPrePareHeader) {
      // 构造请求头
      unsigned char header[10];
      header[0] = 0x01;
      header[1] = 0x01;

      log_info("run out get ip %s", dest_.GetIp().c_str());
      inet_pton(AF_INET, dest_.GetIp().c_str(), header + 2);

      uint16_t port = dest_.GetPort();
      header[6] = port >> 8;
      header[7] = port & 0xff;
      header[8] = '\r';
      header[9] = '\n';
      buf_in->Prepend(header, 10);
      state_ = kTransfer;
    }
    assert(state_ == kTransfer);
    self_->Send(buf_in);
  }
}

RunOut::RunOut(string id, NetAddress dest, InHandler* in, const json& j)
    : dest_(dest), id_(std::move(id)) {
  // 这个addr应该是确定的，连接下一个节点
  static std::string address = j["address"];
  static unsigned short port = j["port"];
  in_ = in;
  NetAddress addr{address, port};
  client_ = std::make_unique<TcpClient>(IoLoop::GetLoop(), addr, "run_out");
}
void RunOut::DisConnected() {
  log_info("call disconnected");
  if (self_) {
    //    client_->Disconnect();
    self_->ForceClose();
  } else {
    client_->SetOnConnection(DefaultOnConnection);
    client_->SetOnMessage(DefaultOnMessage);
    client_->Stop();
  }
}
RunOut::~RunOut() { log_info("~run_out"); }
