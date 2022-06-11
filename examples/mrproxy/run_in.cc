//
// Created by li on 2022/5/30.
//

#include "run_in.h"

#include "netdb.h"
#include "tohka/ioloop.h"
RunIn::RunIn(const json& j) {
  std::string listen_addr = j["listen"];
  int port = j["port"];
  server_ =
      make_unique<TcpServer>(IoLoop::GetLoop(), NetAddress(listen_addr, port));
  log_info("listen on %s:%d", listen_addr.c_str(), port);

  server_->SetOnConnection(
      [this](const TcpEventPrt_t& conn) { on_connection(conn); });
  server_->SetOnMessage(
      [this](const TcpEventPrt_t& conn, IoBuf* buf) { on_recv(conn, buf); });
}
void RunIn::on_connection(const TcpEventPrt_t& conn) {
  if (conn->Connected()) {
    // 创建上下文
    auto ctx = std::make_shared<Context>();
    ctx->in = conn;
    ctx->in_handler = this;

    auto name = conn->GetName();
    ctx_map_.emplace(name, ctx);

    conn->SetContext(kClientAuth);
    log_info("ctx_map_ size = %d", ctx_map_.size());
  } else {
    // TODO 这里也需要关闭out结点的conn
    log_info("%s close", conn->GetName().c_str());
    assert(ctx_map_.find(conn->GetName()) != ctx_map_.end());
    auto ctx = ctx_map_[conn->GetName()];
    if (ctx->out_handler) {
      log_info("ctx->out DisConnected");
      // 释放out_handler的内存
      ctx_map_[conn->GetName()]->out_handler->DisConnected();
    }
    //  out_handler的生命周期和context一样长
    // BUG 还有其它对象持有context对象(也就是FreeDom)
    ctx_map_.erase(conn->GetName());
  }
}
void RunIn::on_recv(const TcpEventPrt_t& conn, IoBuf* buf) {
  State state_ = std::any_cast<State>(conn->GetContext());
  // 解析协议
  if (state_ == kClientAuth) {
    // 寻找crlf
    if (auto crlf = buf->FindCRLF()) {
      unsigned char buffer[128];
      size_t len = buf->ReadUntil(crlf, buffer, 128);
      // TODO 自动去除crlf
      buf->Retrieve(2);

      sockaddr_in client{};
      memset(&client, 0, sizeof(client));
      // 解析cmd
      if (buffer[0] == 0x01) {
        switch (buffer[1]) {
          case 0x01:
            log_info("ipv4");
            client.sin_family = AF_INET;
            memcpy(&client.sin_addr.s_addr, buffer + 2, 4);
            break;
          case 0x03: {
            log_info("domain");
            int domain_size = (int)buffer[4];
            char domain[64] = {0};
            memcpy(domain, buffer + 5, domain_size);
            // TODO async dns
            auto host = gethostbyname(domain);
            client.sin_family = AF_INET;
            assert(host->h_addrtype == AF_INET);
            memcpy(&client.sin_addr.s_addr, host->h_addr_list[0],
                   host->h_length);
          } break;
          case 0x04:
            log_warn("not support ipv6");
            conn->ShutDown();
            return;
            break;
        }

        char ip[16];
        inet_ntop(AF_INET, &client.sin_addr, ip, 16);
        // parse port
        uint16_t port = ((uint16_t)buffer[len - 2]) << 8 | buffer[len - 1];
        assert(port > 0);
        log_info("runin get ip = %s port = %d", ip, port);
        auto ctx = ctx_map_[conn->GetName()];
        ctx->addr = NetAddress(ip, port);

        // 根据配置创建不同的对象
        auto out_handler = OutCreate(ctx);
        ctx->out_handler = out_handler;

        out_handler->StartClient();

        conn->SetContext(kTransfer);
      } else if (buffer[0] == 0x03) {
        // udp
        log_warn("no support udp now");
        conn->ShutDown();
      } else {
        conn->ShutDown();
      }
    } else {
      // 头部没有接受完成
      return;
    }

  } else if (state_ == kTransfer) {
    assert(ctx_map_.find(conn->GetName()) != ctx_map_.end());
    auto& ctx = ctx_map_[conn->GetName()];
    assert(ctx->out_handler);
    if (ctx->out) {
      ctx->out_handler->Process(ctx);
    } else {
      log_warn("run out no out save data to buffer fd = %d", conn->GetFd());
    }
  }
  // 连接目标地址
}
void RunIn::StartServer() {
  server_->Run();
  IoLoop::GetLoop()->RunForever();
}
// 脱离对象存在的方法，但是可以使用本类对象的数据
void RunIn::Process(const ContextPtr_t& ctx) {
  // 表示out结点已经收到了数据，但是处理过程要交给本类处理
  // TODO 数据dump到一个地方
  // 获取out结点的数据
  assert(ctx->out);
  if (ctx->out) {
    IoBuf* buf_in = ctx->out->GetInputBuf();
    assert(ctx->in);
    ctx->in->Send(buf_in);
  } else {
    log_warn("out is release dont not send");
    exit(-2);
  }
}

