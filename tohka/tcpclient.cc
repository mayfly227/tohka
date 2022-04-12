//
// Created by li on 2022/3/8.
//

#include "tcpclient.h"

#include "iobuf.h"
#include "tcpevent.h"
#include "util/log.h"

using namespace tohka;

TcpClient::TcpClient(IoLoop* loop, const NetAddress& peer, std::string name)
    : loop_(loop),
      connector_(std::make_shared<Connector>(loop_, peer)),
      retry_(false),
      connect_(true),
      on_connection_(DefaultOnConnection),
      on_message_(DefaultOnMessage),
      name_(std::move(name)) {
  // socket writeable
  connector_->SetOnConnect(
      std::bind(&TcpClient::OnConnect, this, std::placeholders::_1));
}
TcpClient::~TcpClient() {
  log_info("[TcpClient::~TcpClient]");
  TcpEventPrt_t conn;
  bool unique;
  unique = (connection_.use_count() == 1);
  conn = connection_;
  if (conn) {
    //
    conn->ConnectDestroyed();
    if (unique) {
      conn->ForceClose();
    }
  } else {
    connector_->Stop();
  }
}
void TcpClient::Connect() {
  log_info("[TcpClient::connect]->try to Connect to %s",
           connector_->GetPeerAddress().GetIpAndPort().c_str());
  connect_ = true;
  connector_->Start();
}
void TcpClient::Disconnect() {
  connect_ = false;
  // 连接端还未正式连接成功
  if (connection_) {
    connection_->ShutDown();
  }
}
void TcpClient::Stop() {
  connect_ = false;
  connector_->Stop();
}
void TcpClient::OnConnect(int sock_fd) {
  auto name = connector_->GetPeerAddress().GetIpAndPort();
  log_info("[TcpClient::onConnect]->Connected to %s fd = %d", name.c_str(),
           sock_fd);
  NetAddress peer_address = connector_->GetPeerAddress();
  auto new_conn =
      std::make_shared<TcpEvent>(loop_, name, sock_fd, peer_address);

  new_conn->SetOnConnection(on_connection_);
  new_conn->SetOnOnMessage(on_message_);
  new_conn->SetOnWriteDone(on_write_done_);
  // handle close
  new_conn->SetOnClose(
      std::bind(&TcpClient::RemoveConnection, this, std::placeholders::_1));

  // 把新连接托管给tcp client管理
  connection_ = new_conn;
  new_conn->ConnectEstablished();
}

void TcpClient::RemoveConnection(const TcpEventPrt_t& conn) {
  log_info("[TcpClient::removeConnection]->remove connection from %s",
           connector_->GetPeerAddress().GetIpAndPort().c_str());
  assert(connection_ == conn);
  connection_.reset();
  // remove from pollfd
  conn->ConnectDestroyed();
  if (retry_ && connect_) {
    log_info("[TcpClient::removeConnection]->try to reconnecting to %s",
             connector_->GetPeerAddress().GetIpAndPort().c_str());
    connector_->Restart();
  }
}
