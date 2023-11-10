//
// Created by li on 2022/3/2.
//

#ifndef TOHKA_TOHKA_TCPSERVER_H
#define TOHKA_TOHKA_TCPSERVER_H

#include "acceptor.h"
#include "ioloop.h"
#include "iowatcher.h"
#include "noncopyable.h"
#include "tcpevent.h"
#include "tohka.h"
#include "util/log.h"

// manage tcpevent
namespace tohka {
class TcpServer : noncopyable {
 public:
  TcpServer(IoLoop* loop,NetAddress bind_address);
  ~TcpServer();

  void Run();
  void SetOnConnection(const OnConnectionCallback& cb) { on_connection_ = cb; }
  void SetOnMessage(const OnMessageCallback& cb) { on_message_ = cb; }
  void SetOnWriteDone(const OnWriteDoneCallback& cb) { on_write_done_ = cb; }

 private:
  // call OnConnectionCallback
  void OnAccept(int conn_fd, NetAddress& peer_address);

  void OnClose(const TcpEventPrt_t& conn);

  IoLoop* loop_;
  std::unique_ptr<Acceptor> acceptor_;
  std::map<std::string, TcpEventPrt_t> connection_map_;
  OnConnectionCallback on_connection_;
  OnMessageCallback on_message_;
  OnWriteDoneCallback on_write_done_;
  int64_t conn_id_;
};
}  // namespace tohka
#endif  // TOHKA_TOHKA_TCPSERVER_H
