//
// Created by li on 2022/4/23.
//

#ifndef TOHKA_EXAMPLES_MRPROXY_CONTEXT_H
#define TOHKA_EXAMPLES_MRPROXY_CONTEXT_H
#include "map"
#include "tohka/iobuf.h"
#include "tohka/tcpevent.h"
using namespace std;
using namespace tohka;

class Point;
class InHandler;
class OutHandler;
struct Context;

using ContextPtr_t = std::shared_ptr<Context>;

class InHandler {
 public:
  InHandler() = default;
  virtual ~InHandler() = default;
  virtual void StartServer() = 0;
  virtual void Process(string id) = 0;
};

class OutHandler {
 public:
  OutHandler() = default;
  virtual ~OutHandler() = default;
  virtual void StartClient() = 0;
  virtual void Process() = 0;
  virtual void DisConnected() = 0;
  virtual TcpEventPrt_t GetConn() = 0;
};

using OutHandlerPrt_t = std::shared_ptr<OutHandler>;
using InHandlerPrt_t = std::shared_ptr<InHandler>;

struct Context {
  TcpEventPrt_t in;
  TcpEventPrt_t out;
  InHandler* in_handler;
  OutHandlerPrt_t out_handler;
  NetAddress addr;
};

#endif  // TOHKA_EXAMPLES_MRPROXY_CONTEXT_H
