//
// Created by li on 2022/4/23.
//

#include "Manager.h"

#include "tohka/ioloop.h"

Manager::Manager(NetAddress addr) : server_(IoLoop::GetLoop(), addr), id_(1) {
  server_.SetOnConnection(
      std::bind(&Manager::on_connection, this, std::placeholders::_1));

  server_.SetOnMessage(std::bind(&Manager::on_recv, this, std::placeholders::_1,
                                 std::placeholders::_2));
}
void Manager::on_connection(const TcpEventPrt_t& conn) {
  if (conn->Connected()) {
    context *ctx = new context{};
    Socks5 *socks = new Socks5();

    ctx->conn_in_ = conn;
    ctx->in = socks;
    conn->SetContext(id_);
    ctx_map_.insert({id_,ctx});
    ++id_;
  } else {
    //    ctx.find()
  }
}
void Manager::on_recv(const TcpEventPrt_t& conn, IoBuf* buf) {
  if (conn->GetContext().has_value()){
    int64_t id = std::any_cast<int64_t>(conn->GetContext());
    auto it = ctx_map_.find(id);
    assert(it!=ctx_map_.end());
    it->second->in->process(it->second);
  }
}
