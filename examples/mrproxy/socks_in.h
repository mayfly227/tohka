//
// Created by li on 2022/4/23.
//

#ifndef TOHKA_EXAMPLES_MRPROXY_MANAGER_H
#define TOHKA_EXAMPLES_MRPROXY_MANAGER_H
#include "tohka/tcpserver.h"
#include "tohka/tcpclient.h"
#include "socks5.h"
#include "freedom.h"
#include "context.h"
#include <map>
using namespace tohka;


class Manager {
 public:
  explicit Manager(NetAddress addr);
 private:
  void on_connection(const TcpEventPrt_t& conn);
  void on_recv(const TcpEventPrt_t& conn,IoBuf* buf);
  TcpServer server_;
  int64_t id_;
  std::map<int64_t,context *> ctx_map_;
};

#endif  // TOHKA_EXAMPLES_MRPROXY_MANAGER_H
