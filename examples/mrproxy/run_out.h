//
// Created by li on 2022/5/30.
//

#ifndef TOHKA_EXAMPLES_MRPROXY_RUN_OUT_H
#define TOHKA_EXAMPLES_MRPROXY_RUN_OUT_H

#include "context.h"
#include "tohka/tcpclient.h"
#include "tohka/tcpserver.h"
#include "json.hpp"

using namespace tohka;
using namespace nlohmann;

class RunOut : public OutHandler, public enable_shared_from_this<RunOut> {
 public:
  // 在初始化的时候就应该提供足够量的信息
  RunOut(const ContextPtr_t& ctx, const json& j);
  ~RunOut() override;

  void StartClient() override;
  void Process(const ContextPtr_t& ctx) override;
  void DisConnected() override;

 private:
  void on_connection(const TcpEventPrt_t& conn);
  void on_recv(const TcpEventPrt_t& conn, IoBuf* buf);

  using ClientPrt_t = std::unique_ptr<TcpClient>;
  ClientPrt_t client_;
  ContextPtr_t ctx_;

  enum State { kPrePareHeader, kTransfer };
  State state_ = kPrePareHeader;
};


#endif  // TOHKA_EXAMPLES_MRPROXY_RUN_OUT_H
