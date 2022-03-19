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
  Connector(IoWatcher* io_watcher, NetAddress& peer);
  ~Connector();

  void SetOnConnect(OnConnectCallback on_connect) {
    on_connect_ = std::move(on_connect);
  }

  void Start();
  void Stop();

  NetAddress& GetPeerAddress() { return peer_; }

 private:
  enum State { kDisconnected, kConnecting, kConnected };
  void OnConnect();
  void OnError();
  void Connect();
  void Connecting();
  void Retry();
  void RemoveAndResetEvent();
  void ResetEvent();

  void SetState(State s) { state_ = s; };
  IoWatcher* io_watcher_;
  std::unique_ptr<Socket> sock_;
  std::unique_ptr<IoEvent> event_;
  NetAddress peer_;
  State state_;
  bool connect_;
  OnConnectCallback on_connect_;
};
}  // namespace tohka

#endif  // TOHKA_TOHKA_CONNECTOR_H
