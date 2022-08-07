//
// Created by li on 2022/5/30.
//

#ifndef TOHKA_EXAMPLES_MRPROXY_RUN_OUT_H
#define TOHKA_EXAMPLES_MRPROXY_RUN_OUT_H

#include "context.h"
#include "json.hpp"
#include "tohka/tcpclient.h"
#include "tohka/tcpserver.h"

using namespace tohka;
using namespace nlohmann;

class RunOut : public OutHandler, public enable_shared_from_this<RunOut> {
 public:
  // 在初始化的时候就应该提供足够量的信息
  RunOut(string id, TcpEventPrt_t other, NetAddress dest, InHandler* in,
         const json& j);
  ~RunOut() override;

  void StartClient() override;
  void Process() override;
  void DisConnected() override;
  TcpEventPrt_t GetConn() override { return self_; };

 private:
  void on_connection(const TcpEventPrt_t& conn);
  void on_recv(const TcpEventPrt_t& conn, IoBuf* buf);

  using ClientPrt_t = std::unique_ptr<TcpClient>;
  ClientPrt_t client_;
  TcpEventPrt_t self_;
  TcpEventPrt_t other_;

  NetAddress dest_;
  string id_;
  InHandler* in_;

  enum State { kPrePareHeader, kTransfer };
  State state_ = kPrePareHeader;
};

#endif  // TOHKA_EXAMPLES_MRPROXY_RUN_OUT_H
