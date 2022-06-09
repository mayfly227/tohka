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
  virtual void StartServer() = 0;
  virtual void Process(const ContextPtr_t& ctx) = 0;
};

class OutHandler {
 public:
  virtual ~OutHandler() = default;
  virtual void StartClient() = 0;
  virtual void Process(const ContextPtr_t& ctx) = 0;
  virtual void DisConnected() = 0;
};

using OutHandlerPrt_t = std::shared_ptr<OutHandler>;
struct Context {
  // public:
  TcpEventPrt_t in;
  TcpEventPrt_t out;
  InHandler* in_handler;
  OutHandlerPrt_t out_handler;
  //  OutHandler *out_handler;
  NetAddress addr;
};

class InConnFactory {
 public:
  // 应该连接已经建立好了
  virtual InHandler* Create(Point* point) = 0;
};

class OutConnFactory {
 public:
  virtual OutHandler* Create(Point* point) = 0;
};

class Point {
 public:
  InConnFactory* in;
  OutConnFactory* out;
};

extern map<string, InConnFactory*> inFactories;
extern map<string, OutConnFactory*> outFactories;

inline void RegIn(const string& name, InConnFactory* fac) {
  inFactories[name] = fac;
}

inline void RegOut(const string& name, OutConnFactory* fac) {
  outFactories[name] = fac;
};

inline Point* newPoint() {
  // 注册各个结点
  auto point = new Point();
  auto in = inFactories["socks"];
  auto out = outFactories["freedom"];
  point->in = in;
  point->out = out;

  return point;
}

#endif  // TOHKA_EXAMPLES_MRPROXY_CONTEXT_H
