//
// Created by li on 2022/3/1.
//

#ifndef TOHKA_TOHKA_TCPEVENT_H
#define TOHKA_TOHKA_TCPEVENT_H

#include "iobuf.h"
#include "ioevent.h"
#include "netaddress.h"
#include "socket.h"
#include "tohka.h"
#include "util/log.h"

namespace tohka {

class IoWatcher;

// one tcp connection
class TcpEvent : noncopyable, public std::enable_shared_from_this<TcpEvent> {
 public:
  TcpEvent(IoWatcher* io_watcher, std::string name, int fd, NetAddress& peer);
  ~TcpEvent();

  void StartReading() { event_->EnableReading(); }
  void StartWriting() { event_->EnableWriting(); }
  void StopReading() { event_->DisableReading(); }
  void StopWriting() { event_->DisableWriting(); }
  void StopAll() { event_->DisableAll(); }

  void SetOnConnection(const OnConnectionCallback& on_connection) {
    on_connection_ = on_connection;
  };
  void SetOnOnMessage(const OnMessageCallback& on_message) {
    on_message_ = on_message;
  };
  void SetOnWriteDone(const OnWriteDoneCallback& on_write_done) {
    on_write_done_ = on_write_done;
  };

  void Send(std::string_view msg);
  void Send(const char* data, size_t len);
  void Send(IoBuf* buffer);

  void ShutDown();
  void ForceClose();

  void SetContext(const std::any& context) { context_ = context; }
  std::any& GetContext() { return context_; }
  bool Connected() { return state_ == kConnected; }
  std::string GetPeerIp() { return peer_.GetIp(); };
  uint16_t GetPeerPort() { return peer_.GetPort(); };
  std::string GetPeerIpAndPort() { return peer_.GetIpAndPort(); };

  std::string GetName() const { return name_; };

  int GetFd() { return socket_->GetFd(); };

  IoBuf* GetInputBuf() { return &in_buf_; };
  IoBuf* GetOutputBuf() { return &out_buf_; };

  /// Internal use only.
  void SetOnClose(const OnCloseCallback& on_close) { on_close_ = on_close; }
  // Be called when this connection establishing(call on accept)
  void ConnectEstablished();
  // Be called when this connection destroying (call on Close)
  void ConnectDestroyed();

 private:
  void HandleRead();
  void HandleWrite();
  void HandleClose();
  void HandleError();

  void TryEagerShutDown();
  enum STATE { kConnecting, kConnected, kDisconnecting, kDisconnected };
  IoWatcher* io_watcher_;
  std::unique_ptr<IoEvent> event_;
  std::unique_ptr<Socket> socket_;
  NetAddress peer_;
  std::string name_;
  STATE state_;
  IoBuf in_buf_;
  IoBuf out_buf_;
  std::any context_;
  void SetState(STATE state) { state_ = state; }
  OnMessageCallback on_message_;
  OnConnectionCallback on_connection_;
  OnCloseCallback on_close_;
  OnWriteDoneCallback on_write_done_;
};
}  // namespace tohka

#endif  // TOHKA_TOHKA_TCPEVENT_H
