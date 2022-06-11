//
// Created by li on 2022/4/22.
//

#ifndef TOHKA_EXAMPLES_MRPROXY_FREEDOM_H
#define TOHKA_EXAMPLES_MRPROXY_FREEDOM_H
#include "context.h"
#include "tohka/tcpclient.h"
#include "tohka/tcpserver.h"
using namespace tohka;

class FreeDom : public OutHandler, public enable_shared_from_this<FreeDom> {
 public:
  // 在初始化的时候就应该提供足够量的信息
  explicit FreeDom(const ContextPtr_t& ctx);
  ~FreeDom() override;
  void StartClient() override;
  void Process(const ContextPtr_t& ctx) override;
  void DisConnected() override;

 private:
  void on_connection(const TcpEventPrt_t& conn);
  void on_recv(const TcpEventPrt_t& conn, IoBuf* buf);
  using ClientPrt_t = std::unique_ptr<TcpClient>;
  ClientPrt_t client_;
  ContextPtr_t ctx_;
};

#endif  // TOHKA_EXAMPLES_MRPROXY_FREEDOM_H
