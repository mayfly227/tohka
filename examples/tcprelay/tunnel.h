//
// Created by li on 2022/3/21.
//
#ifndef TOHKA_EXAMPLES_TCPRELAY_TUNNEL_H
#define TOHKA_EXAMPLES_TCPRELAY_TUNNEL_H
#include <any>
#include <cassert>
#include <iostream>
#include <memory>

#include "tohka/ioloop.h"
#include "tohka/netaddress.h"
#include "tohka/noncopyable.h"
#include "tohka/tcpclient.h"
#include "tohka/tcpevent.h"
#include "tohka/util/log.h"
using namespace tohka;
using namespace std;
using namespace std::placeholders;

class Tunnel : public std::enable_shared_from_this<Tunnel> {
 public:
  Tunnel(IoLoop* loop, const NetAddress& serverAddr,
         const TcpEventPrt_t& serverConn)
      : client_(loop, serverAddr, serverConn->GetName()),
        serverConn_(serverConn) {
    log_info("Tunnel %s <-> %s client fd = %d", serverConn->GetPeerIpAndPort().c_str(),
             serverAddr.GetIpAndPort().c_str(),serverConn->GetFd());
  }

  ~Tunnel() { log_info("~Tunnel"); }

  void setup() {
    client_.SetOnConnection(
        std::bind(&Tunnel::onClientConnection, shared_from_this(), _1));
    client_.SetOnMessage(
        std::bind(&Tunnel::onClientMessage, shared_from_this(), _1, _2));
  }

  void connect() { client_.Connect(); }

  void disconnect() {
    client_.Disconnect();
    // serverConn_.reset();
  }

 private:
  void teardown() {
    client_.SetOnConnection(DefaultOnConnection);
    client_.SetOnMessage(DefaultOnMessage);
    if (serverConn_) {
      serverConn_->SetContext(any());
      serverConn_->ShutDown();
    }
    clientConn_.reset();
  }

  void onClientConnection(const TcpEventPrt_t& conn) {
    log_debug(conn->Connected() ? "server UP" : "server DOWN");
    if (conn->Connected()) {
      serverConn_->SetContext(conn);
      serverConn_->StartReading();
      clientConn_ = conn;
      if (serverConn_->GetInputBuf()->GetReadableSize() > 0) {
        conn->Send(serverConn_->GetInputBuf());
      }
    } else {
      teardown();
    }
  }

  void onClientMessage(const TcpEventPrt_t& conn, IoBuf* buf) {
    log_debug("name: %s readable= %d", conn->GetName().c_str(),
              buf->GetReadableSize());
    if (serverConn_) {
      serverConn_->Send(buf);
    } else {
      buf->Refresh();
      abort();
    }
  }

 private:
  TcpClient client_;
  // 代表做为server的那个连接，也就是与客户端的连接
  TcpEventPrt_t serverConn_;
  // 代表做为client的那个连接，也就是与服户端的连接
  TcpEventPrt_t clientConn_;
};
using TunnelPtr = std::shared_ptr<Tunnel>;

#endif  // TOHKA_EXAMPLES_TCPRELAY_TUNNEL_H
