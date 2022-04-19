//
// Created by li on 2022/3/5.
//

#ifndef TOHKA_TOHKA_CONNECTOR_H
#define TOHKA_TOHKA_CONNECTOR_H

#include "ioevent.h"
#include "socket.h"
#include "tohka.h"

namespace tohka {
class Connector {
 public:
  Connector(IoLoop* loop, const NetAddress& peer);
  ~Connector();

  void SetOnConnect(OnConnectCallback on_connect) {
    on_connect_ = std::move(on_connect);
  }

  void Start();
  void Restart();
  void Stop();

  // internal use only
  void EnableConnectTimeout(bool on) { enable_connect_timeout_ = on; };
  // internal use only
  void SetConnectTimeout(int connect_timeout_ms) {
    connect_timeout_ms_ = connect_timeout_ms;
  }
  NetAddress& GetPeerAddress() { return peer_; }

 private:
  enum State { kDisconnected, kConnecting, kConnected };
  void OnConnect();
  void Connect();
  void Connecting(int sock_fd);
  void Retry(int sock_fd);
  int RemoveAndResetEvent();
  void ResetEvent();

  void OnConnectTimeout();
  void SetState(State s) { state_ = s; };

  static constexpr int kInitDelayMs = 500;
  static constexpr int kMaxDelayMs = 30 * 1000;
  static constexpr int kDefaultTimeoutMs = 8000;
  IoLoop* loop_;
  int retry_delay_ms_;
  std::unique_ptr<IoEvent> event_;
  NetAddress peer_;
  State state_;
  bool connect_;
  bool enable_connect_timeout_;
  int connect_timeout_ms_;
  OnConnectCallback on_connect_;
};
}  // namespace tohka

#endif  // TOHKA_TOHKA_CONNECTOR_H
