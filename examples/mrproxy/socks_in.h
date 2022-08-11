//
// Created by li on 2022/4/23.
//

#ifndef TOHKA_EXAMPLES_MRPROXY_SOCKS_IN_H
#define TOHKA_EXAMPLES_MRPROXY_SOCKS_IN_H
#include <map>
#include <optional>

#include "context.h"
#include "point.h"
#include "tohka/tcpclient.h"
#include "tohka/tcpserver.h"

using namespace tohka;

class SocksIn : public InHandler {
 public:
  explicit SocksIn(const json& j);
  void StartServer() override;
  void Process(string id) override;

  std::optional<TcpEventPrt_t> GetInConn(const std::string& id) override {
    if (in_conn_map_.count(id) == 1) {
      return {in_conn_map_[id]};
    }
    return {};
  }

 private:
  void on_connection(const TcpEventPrt_t& conn);
  void on_recv(const TcpEventPrt_t& conn, IoBuf* buf);
  using TcpServerPrt_t = std::unique_ptr<TcpServer>;
  TcpServerPrt_t server_;
  enum State { kClientAuth, KClientConnected, kTransfer };
  std::map<string, TcpEventPrt_t> in_conn_map_;
  std::map<string, OutHandlerPrt_t> out_conn_map_;
};

#endif  // TOHKA_EXAMPLES_MRPROXY_SOCKS_IN_H
