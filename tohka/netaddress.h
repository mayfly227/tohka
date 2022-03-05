//
// Created by li on 2022/2/27.
//

#ifndef TOHKA_TOHKA_NETADDRESS_H
#define TOHKA_TOHKA_NETADDRESS_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <any>
#include <string>
namespace tohka {
class NetAddress {
 public:
  NetAddress() = default;
  NetAddress(const std::string& ip, uint16_t port);
  NetAddress(std::string ip, int16_t port, bool ipv6);

  std::string GetIp();
  uint16_t GetPort();
  std::string GetIpAndPort();

  std::any GetAddress();
  uint32_t GetSize();

 private:
  bool ipv6_ = false;
  union {
    struct sockaddr_in in4_;
    struct sockaddr_in6 in6_;
  };
};
}  // namespace tohka
#endif  // TOHKA_TOHKA_NETADDRESS_H
