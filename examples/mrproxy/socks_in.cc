//
// Created by li on 2022/4/23.
//

#include "socks_in.h"

#include "netdb.h"
#include "point.h"
#include "tohka/ioloop.h"
SocksIn::SocksIn(const json& j) {
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
void SocksIn::on_connection(const TcpEventPrt_t& conn) {
  if (conn->Connected()) {
    auto name = conn->GetName();
    in_conn_map_.emplace(name, conn);
    out_conn_map_.emplace(name, nullptr);

    conn->SetContext(kClientAuth);
  } else {
    log_info("%s close", conn->GetName().c_str());
    auto it = out_conn_map_.find(conn->GetName());
    auto it1 = in_conn_map_.find(conn->GetName());
    assert(it != out_conn_map_.end());
    assert(it1 != in_conn_map_.end());

    if (it != out_conn_map_.end()) {
      if (it->second) {
        it->second->DisConnected();
      }
      log_info("erase out conn name=%s ref=%d", it->first.c_str(),
               it->second.use_count());
      out_conn_map_.erase(it);
    }
    if (it1 != in_conn_map_.end()) {
      log_info("erase in conn name=%s ref=%d", it1->first.c_str(),
               it1->second.use_count());
      in_conn_map_.erase(it1);
    }
  }
}
void SocksIn::on_recv(const TcpEventPrt_t& conn, IoBuf* buf) {
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
          if (!host) {
            log_warn("parse dns error!");
            conn->ShutDown();
            return;
          }
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
    // ctx->addr = NetAddress("8.8.8.8", 443);
    // ctx->addr = NetAddress("127.0.0.1", 6666);
    // ctx->addr = NetAddress("220.181.38.149", 80);
    // 根据配置创建不同的对象
    NetAddress addr{ip, port};
    string id = conn->GetName();
    auto out_handler = OutCreate(id, addr, this);
    assert(out_conn_map_.find(id) != out_conn_map_.end());
    assert(out_conn_map_[id] == nullptr);
    out_conn_map_[id] = out_handler;

    out_handler->StartClient();

    conn->SetContext(kTransfer);
  } else if (state_ == kTransfer) {
    assert(out_conn_map_.find(conn->GetName()) != out_conn_map_.end());

    auto& out_handler = out_conn_map_[conn->GetName()];
    assert(out_handler != nullptr);
    // TODO 连接是否成功？
    out_handler->Process();
  }
}
void SocksIn::StartServer() {
  server_->Run();
  log_info("listen at 7777");
  IoLoop::GetLoop()->RunForever();
}
// 脱离对象存在的方法，但是可以使用本类对象的数据
void SocksIn::Process(string id) {
  // 表示out结点已经收到了数据，但是处理过程要交给本类处理
  // 获取in/out结点的conn
  // get in conn 会hit assert？
  // TODO why?
  if (in_conn_map_.find(id) == in_conn_map_.end()) {
    log_fatal("no in conn~");
    return;
  }
  // get out conn
  auto in_conn = in_conn_map_[id];
  auto out_conn = out_conn_map_[id]->GetConn();
  assert(out_conn);
  in_conn->Send(out_conn->GetInputBuf());
}
