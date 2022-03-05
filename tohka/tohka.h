//
// Created by li on 2022/3/1.
//

#ifndef TOHKA_TOHKA_TOHKA_H
#define TOHKA_TOHKA_TOHKA_H
// typedef and callbacks
#include <functional>
#include <memory>

#include "platform.h"

namespace tohka {
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

using OnAcceptCallback =
    std::function<void(int conn_fd, NetAddress& peer_address)>;
using OnConnectionCallback = std::function<void(const TcpEventPrt_t& conn)>;
using OnMessageCallback =
    std::function<void(const TcpEventPrt_t& conn, IoBuf* buf)>;
using OnCloseCallback = std::function<void(const TcpEventPrt_t& conn)>;

#ifdef OS_UNIX
enum {
  TOHKA_NONE = 0x0000,
  TOHKA_READ = 0x0001,
  TOHKA_WRITE = 0x0004,
};

#endif
}  // namespace tohka
#endif  // TOHKA_TOHKA_TOHKA_H
