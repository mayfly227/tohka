//
// Created by li on 2022/3/5.
//

#ifndef TOHKA_TOHKA_CONNECTOR_H
#define TOHKA_TOHKA_CONNECTOR_H

#include "ioevent.h"
#include "socket.h"
#include "tohka.h"

namespace tohka {
class IoWatcher;

class Connector {
 public:
  Connector(IoWatcher* io_watcher, const NetAddress& peer);
  ~Connector();

  void SetOnConnect(OnConnectCallback on_connect) {
    on_connect_ = std::move(on_connect);
  }

  void Start();
  void Restart();
  void Stop();

  // internal use only
  void SetConnectTimeout(int connect_timeout_ms) {
    connect_timeout_ms_ = connect_timeout_ms;
  }
  NetAddress& GetPeerAddress() { return peer_; }

 private:
  enum State { kDisconnected, kConnecting, kConnected };
  void OnConnect();
  void OnClose();
  void OnError();
  void Connect();
  void Connecting();
  void Retry();
  void RemoveAndResetEvent();
  void ResetEvent();

  void OnConnectTimeout();

  static constexpr int kInitDelayMs = 500;
  static constexpr int kMaxDelayMs = 30 * 1000;
  static constexpr int kDefaultTimeoutMs = 8000;
  int retry_delay_ms_;
  void SetState(State s) { state_ = s; };
  IoWatcher* io_watcher_;
  std::unique_ptr<Socket> sock_;
  std::unique_ptr<IoEvent> event_;
  NetAddress peer_;
  State state_;
  bool connect_;
  int connect_timeout_ms_ = kDefaultTimeoutMs;
  OnConnectCallback on_connect_;
};
}  // namespace tohka

#endif  // TOHKA_TOHKA_CONNECTOR_H
