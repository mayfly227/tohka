//
// Created by li on 2022/3/1.
//

#ifndef TOHKA_TOHKA_TOHKA_H
#define TOHKA_TOHKA_TOHKA_H
// typedef and callbacks

#include "platform.h"

namespace tohka {
class IoLoop;
class IoEvent;
class IoBuf;
class TcpEvent;
class TimerManager;
class TimePoint;
class Socket;
class NetAddress;
class Timer;

// typedef
using TimerPrt_t = std::unique_ptr<Timer>;
using TcpEventPrt_t = std::shared_ptr<TcpEvent>;

using EventList = std::vector<IoEvent*>;
using ExpiredTimers = std::vector<TimerPrt_t>;
// callback
using EventCallback = std::function<void()>;
using TimerCallback = std::function<void()>;

// for acceptor
using OnAcceptCallback =
    std::function<void(int conn_fd, NetAddress& peer_address)>;
// for connector
using OnConnectCallback = std::function<void(int sock_fd)>;

// for tcpserver
using OnConnectionCallback = std::function<void(const TcpEventPrt_t& conn)>;
using OnMessageCallback =
    std::function<void(const TcpEventPrt_t& conn, IoBuf* buf)>;
using OnWriteDoneCallback = std::function<void(const TcpEventPrt_t& conn)>;

using OnCloseCallback = std::function<void(const TcpEventPrt_t& conn)>;
using OnHighWaterMark = std::function<void(const TcpEventPrt_t& conn)>;

// for tcp event
void DefaultOnConnection(const TcpEventPrt_t& conn);
void DefaultOnMessage(const TcpEventPrt_t& conn, IoBuf* buf);

enum {
  EV_NONE = 0x0000,
  EV_READ = 0x0001,
  EV_WRITE = 0x0004,
};

}  // namespace tohka
#endif  // TOHKA_TOHKA_TOHKA_H
