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

  void Connect();
  void Disconnect();
  void Stop();

  void EnableConnectTimeout(bool on) { connector_->EnableConnectTimeout(on); };

  void SetConnectTimeout(int connect_timeout_ms) {
    connector_->SetConnectTimeout(connect_timeout_ms);
  }

  void SetOnConnection(OnConnectionCallback cb) {
    on_connection_ = std::move(cb);
  }
  void SetOnMessage(OnMessageCallback cb) { on_message_ = std::move(cb); }
  void SetOnWriteDone(OnWriteDoneCallback cb) {
    on_write_done_ = std::move(cb);
  }
  void SetRetry(bool status) { retry_ = status; }

  bool GetRetry() const { return retry_; }

 private:
  void OnConnect(int sock_fd);
  void RemoveConnection(const TcpEventPrt_t& conn);
  IoLoop* loop_;
  // 持有连接器的共享指针
  using ConnectorPrt_t = std::shared_ptr<Connector>;
  ConnectorPrt_t connector_;
  TcpEventPrt_t connection_;
  bool retry_;
  bool connect_;
  OnConnectionCallback on_connection_;
  OnMessageCallback on_message_;
  OnWriteDoneCallback on_write_done_;
  std::string name_;
};
}  // namespace tohka
#endif  // TOHKA_TOHKA_TCPCLIENT_H
