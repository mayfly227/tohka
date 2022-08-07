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
      out_conn_map_.erase(it);
    }
    if (it1 != in_conn_map_.end()) {
      in_conn_map_.erase(it1);
    }
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

        // 根据配置创建不同的对象
        NetAddress addr{ip, port};
        string id = conn->GetName();
        auto out_handler = OutCreate(id, conn, addr, this);
        assert(out_conn_map_.find(id) != out_conn_map_.end());
        assert(out_conn_map_[id] == nullptr);
        out_conn_map_[id] = out_handler;
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
    assert(out_conn_map_.find(conn->GetName()) != out_conn_map_.end());
    auto& out_handler = out_conn_map_[conn->GetName()];
    out_handler->Process();
  }
  // 连接目标地址
}
void RunIn::StartServer() {
  server_->Run();
  IoLoop::GetLoop()->RunForever();
}
// 脱离对象存在的方法，但是可以使用本类对象的数据
void RunIn::Process(string id) {
  // 表示out结点已经收到了数据，但是处理过程要交给本类处理
  // 获取in/out结点的conn
  // get in conn
  // TODO why?
  if (in_conn_map_.find(id) == in_conn_map_.end()) {
    // log_fatal("no in conn~");
    return;
  }
  assert(in_conn_map_.find(id) != in_conn_map_.end());
  // get out conn
  auto in_conn = in_conn_map_[id];
  // BUG？ 这个时候还没有创建out handler? 必定被创建了
  auto out_conn = out_conn_map_[id]->GetConn();

  in_conn->Send(out_conn->GetInputBuf());
}
