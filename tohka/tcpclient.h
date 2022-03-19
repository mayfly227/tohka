//
// Created by li on 2022/3/8.
//

#ifndef TOHKA_TOHKA_TCPCLIENT_H
#define TOHKA_TOHKA_TCPCLIENT_H
#include "connector.h"
#include "netaddress.h"
#include "tohka.h"
namespace tohka {
class IoWatcher;
class TcpClient : noncopyable {
 public:
  using ConnectorPrt_t = std::shared_ptr<Connector>;
  TcpClient(IoWatcher* io_watcher, NetAddress& peer);
  ~TcpClient();

  void Connect();
  void Disconnect();
  void Stop();

  void SetOnConnection(OnConnectionCallback cb) {
    on_connection_ = std::move(cb);
  }
  void SetOnMessage(OnMessageCallback cb) { on_message_ = std::move(cb); }
  void SetOnWriteDone(OnWriteDoneCallback cb) {
    on_write_done_ = std::move(cb);
  }

 private:
  void onConnect(int sock_fd);
  void removeConnection(const TcpEventPrt_t& conn);
  IoWatcher* io_watcher_;
  // 持有连接器有tcp的共享指针
  ConnectorPrt_t connector_;
  TcpEventPrt_t connection_;

  bool connect_;
  OnConnectionCallback on_connection_;
  OnMessageCallback on_message_;
  OnWriteDoneCallback on_write_done_;

  //  void DefaultOnConnection(const TcpEventPrt_t& conn);
  //  void DefaultOnMessage(const TcpEventPrt_t& conn, IoBuf* buf);
};
}  // namespace tohka
#endif  // TOHKA_TOHKA_TCPCLIENT_H
