//
// Created by li on 2022/4/23.
//

#include "socks_in.h"

#include "netdb.h"
#include "tohka/ioloop.h"
socks_in::socks_in() {
  server_ = make_unique<TcpServer>(IoLoop::GetLoop(), NetAddress(7777));
  server_->SetOnConnection(
      [this](const TcpEventPrt_t& conn) { on_connection(conn); });
  server_->SetOnMessage(
      [this](const TcpEventPrt_t& conn, IoBuf* buf) { on_recv(conn, buf); });
}
void socks_in::on_connection(const TcpEventPrt_t& conn) {
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
void socks_in::on_recv(const TcpEventPrt_t& conn, IoBuf* buf) {
  State state_ = std::any_cast<State>(conn->GetContext());
  if (state_ == kClientAuth) {
    char buffer[16];
    assert(buf->GetReadableSize() == 3);
    buf->Read(buffer, 16);
    if (buffer[0] == 0x05 && buffer[1] == 0x01 && buffer[2] == 0x00) {
      char resp[2] = {0x05, 0x00};
      conn->Send(resp, 2);
    } else {
      conn->ShutDown();
      return;
    }
    conn->SetContext(KClientConnected);
  } else if (state_ == KClientConnected) {
    sockaddr_in client{};
    memset(&client, 0, sizeof(client));
    unsigned char buffer[64] = {0};
    size_t len = buf->Read(buffer, 64);
    if (buffer[0] == 0x05) {
      // parse cmd
      switch (buffer[1]) {
        case 0x01:
          log_info("connected");
          break;
        case 0x02:
          log_info("Bind");
          break;
        case 0x03:
          log_info("UDP");
          break;
      }
      // parse atyp
      switch (buffer[3]) {
        case 0x01:
          log_info("ipv4");
          client.sin_family = AF_INET;
          memcpy(&client.sin_addr.s_addr, buffer + 4, 4);
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
          memcpy(&client.sin_addr.s_addr, host->h_addr_list[0], host->h_length);
        } break;
        case 0x04:
          log_warn("not support ipv6");
          conn->ShutDown();
          return;
          break;
      }
    }
    char ip[16];
    inet_ntop(AF_INET, &client.sin_addr, ip, 16);
    // parse port
    uint16_t port = ((uint16_t)buffer[len - 2]) << 8 | buffer[len - 1];
    assert(port > 0);
    log_info("socks5 get ip = %s port = %d", ip, port);
    //    addr = {ip, port};
    char resp[] = {0x05, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    conn->Send(resp, 10);

    // conn->StopReading();
    auto ctx = ctx_map_[conn->GetName()];
    // 注入地址
    // TODO only debug
    ctx->addr = NetAddress(ip, port);
    // ctx->addr = NetAddress("8.8.8.8", 443);
    // ctx->addr = NetAddress("127.0.0.1", 6666);
    //     ctx->addr = NetAddress("127.0.0.1", 6667);
    //     ctx->addr = NetAddress("220.181.38.149", 80);
    // TODO 根据配置创建不同的对象
    auto out_handler = make_shared<FreeDom>(ctx);
    ctx->out_handler = out_handler;

    out_handler->StartClient();

    conn->SetContext(kTransfer);
  } else if (state_ == kTransfer) {
    // TODO 加密的数据 主动消费数据
    // TODO 调用out结点来处理数据(out连接必需创建成功)
    // TODO 这个时候应该注入一些相关的上下文信息（）

    assert(ctx_map_.find(conn->GetName()) != ctx_map_.end());
    auto& ctx = ctx_map_[conn->GetName()];
    assert(ctx->out_handler);
    if (ctx->out) {
      ctx->out_handler->Process(ctx);
    } else {
      log_warn("no out save data to buffer fd = %d", conn->GetFd());
    }
  }
}
void socks_in::StartServer() {
  server_->Run();
  IoLoop::GetLoop()->RunForever();
}
// 脱离对象存在的方法，但是可以使用本类对象的数据
void socks_in::Process(const ContextPtr_t& ctx) {
  // 表示out结点已经收到了数据，但是处理过程要交给本类处理
  // TODO 数据dump到一个地方
  // 获取out结点的数据
  assert(ctx->out);
  if (ctx->out) {
    IoBuf* buf_in = ctx->out->GetInputBuf();
    assert(ctx->in);
    ctx->in->Send(buf_in);
  } else {
    exit(-2);
    log_warn("out is release dont not send");
  }
}

// InHandler* SocksFactory::Create(Point* point) { return new socks_in(); }
// void SocksFactory::Init() { RegIn("socks", new SocksFactory()); }
