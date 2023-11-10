//
// Created by li on 2022/3/5.
//

#ifndef TOHKA_TOHKA_CONNECTOR_H
#define TOHKA_TOHKA_CONNECTOR_H

#include "ioevent.h"
#include "socket.h"
#include "timerid.h"
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

  NetAddress& GetPeerAddress() { return peer_; }

 private:
  enum State { kDisconnected, kConnecting, kConnected };
  void OnConnect();
  void OnConnectError();
  void Connect();
  void Connecting(int sock_fd);
  void Retry(int sock_fd);
  int RemoveAndResetEvent();
  void ResetEvent();

  void SetState(State s) { state_ = s; };
  static constexpr int kInitDelayMs = 500;
  static constexpr int kMaxDelayMs = 10 * 1000;
  static constexpr int kDefaultTimeoutMs = 8000;
  IoLoop* loop_;
  TimerId timer_id_;
  int retry_delay_ms_;
  std::unique_ptr<IoEvent> event_;
  NetAddress peer_;
  State state_;
  bool connect_;
  OnConnectCallback on_connect_;
};
}  // namespace tohka

#endif  // TOHKA_TOHKA_CONNECTOR_H
