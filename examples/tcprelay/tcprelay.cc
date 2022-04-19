//
// Created by li on 2022/3/21.
//
#include <unistd.h>

#include <iostream>
#include <memory>

#include "tohka/ioloop.h"
#include "tohka/netaddress.h"
#include "tohka/tcpclient.h"
#include "tohka/tcpserver.h"
#include "tohka/util/log.h"
#include "tunnel.h"
using namespace tohka;
using namespace std;
using namespace std::placeholders;

NetAddress* g_server_addr;
std::map<string, TunnelPtr> g_tunnels;

void onServerConnection(const TcpEventPrt_t& conn) {
  log_debug(conn->Connected() ? "client UP" : "client DOWN");
  if (conn->Connected()) {
    conn->StopReading();
    TunnelPtr tunnel(new Tunnel(IoLoop::GetLoop(), *g_server_addr, conn));
    tunnel->setup();
    tunnel->connect();
    g_tunnels[conn->GetName()] = tunnel;
  } else {
    log_info("conn name = %s fd = %d",conn->GetName().c_str(),conn->GetFd());
    assert(g_tunnels.find(conn->GetName()) != g_tunnels.end());
    g_tunnels[conn->GetName()]->disconnect();
    g_tunnels.erase(conn->GetName());
  }
}

void onServerMessage(const TcpEventPrt_t& conn, IoBuf* buf) {
  log_debug("onServerMessage %d", buf->GetReadableSize());
  if (conn->GetContext().has_value()) {
    const auto& clientConn =
        std::any_cast<const TcpEventPrt_t&>(conn->GetContext());
    clientConn->Send(buf);
  }
}

int main() {
  IoLoop* loop = IoLoop::GetLoop();
  NetAddress serverAddr("127.0.0.1", 8080);
  g_server_addr = &serverAddr;

  NetAddress listen_addr(2000);
  log_set_level(LOG_INFO);
  log_info("start at %s", listen_addr.GetIpAndPort().c_str());
  TcpServer server(loop, listen_addr);

  server.SetOnConnection(onServerConnection);
  server.SetOnMessage(onServerMessage);

  server.Run();
  loop->RunForever();
}
