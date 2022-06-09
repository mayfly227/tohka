//
// Created by li on 2022/4/23.
//

#ifndef TOHKA_EXAMPLES_MRPROXY_SOCKS_IN_H
#define TOHKA_EXAMPLES_MRPROXY_SOCKS_IN_H
#include <map>

#include "context.h"
#include "freedom.h"
#include "tohka/tcpclient.h"
#include "tohka/tcpserver.h"
using namespace tohka;

// class SocksFactory : public InConnFactory {
//  public:
//   InHandler* Create(Point* point) override;
//   static void Init();
// };

class socks_in : public InHandler {
 public:
  socks_in();
  void StartServer() override;
  void Process(const ContextPtr_t& ctx) override;

 private:
  void on_connection(const TcpEventPrt_t& conn);
  void on_recv(const TcpEventPrt_t& conn, IoBuf* buf);
  using TcpServerPrt_t = std::unique_ptr<TcpServer>;
  TcpServerPrt_t server_;
  std::map<string, ContextPtr_t> ctx_map_;
  enum State { kClientAuth, KClientConnected, kTransfer };
};

#endif  // TOHKA_EXAMPLES_MRPROXY_SOCKS_IN_H
