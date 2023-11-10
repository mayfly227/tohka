//
// Created by li on 2022/3/8.
//

#ifndef TOHKA_TOHKA_TCPCLIENT_H
#define TOHKA_TOHKA_TCPCLIENT_H
#include "connector.h"
#include "netaddress.h"
#include "tcpevent.h"
#include "tohka.h"
namespace tohka {

class TcpClient : noncopyable {
 public:
  TcpClient(IoLoop* loop, const NetAddress& peer, std::string name);
  ~TcpClient();

  // 主动连接
  void Connect();
  // 主动断开连接(已经连接好了)
  void Disconnect();
  // 停止连接
  void Stop();

  void SetOnConnection(OnConnectionCallback cb) {
    on_connection_ = std::move(cb);
  }
  void SetOnMessage(OnMessageCallback cb) { on_message_ = std::move(cb); }
  void SetOnWriteDone(OnWriteDoneCallback cb) {
    on_write_done_ = std::move(cb);
  }
  void SetRetry(bool status) { retry_ = status; }

  bool IsRetry() const { return retry_; }

  void RemoveConnection(const TcpEventPrt_t& conn);

 private:
  void OnConnect(int sock_fd);
  IoLoop* loop_;
  // 持有连接器的共享指针
  using ConnectorPrt_t = std::unique_ptr<Connector>;
  ConnectorPrt_t connector_;
  TcpEventPrt_t connection_;
  bool retry_;
  bool connect_;
  int64_t conn_id_;
  NormalCallback normal_callback_;
  OnConnectionCallback on_connection_;
  OnMessageCallback on_message_;
  OnWriteDoneCallback on_write_done_;
  std::string name_;
};
}  // namespace tohka
#endif  // TOHKA_TOHKA_TCPCLIENT_H
