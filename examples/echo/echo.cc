#include "tohka/ioloop.h"
#include "tohka/netaddress.h"
#include "tohka/tcpserver.h"
#include "tohka/util/log.h"

using namespace tohka;

void OnConnection(const TcpEventPrt_t& conn) {
  if (conn->Connected()) {
    log_info("new connection from: %s", conn->GetPeerIpAndPort().c_str());
  } else {
    log_info("connection Close: %s", conn->GetPeerIpAndPort().c_str());
  }
}

void OnMessage(const TcpEventPrt_t& conn, IoBuf* buf) { conn->Send(buf); }

int main() {
  IoLoop* loop = IoLoop::GetLoop();
  NetAddress address(6666);
  log_info("server listen on:%s", address.GetIpAndPort().c_str());
  TcpServer server(loop,address);
  server.SetOnConnection(OnConnection);
  server.SetOnMessage(OnMessage);
  server.Run();
  loop->RunForever();
  return 0;
}