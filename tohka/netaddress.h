//
// Created by li on 2022/2/27.
//

#ifndef TOHKA_TOHKA_NETADDRESS_H
#define TOHKA_TOHKA_NETADDRESS_H

#include "platform.h"

namespace tohka {
class NetAddress {
 public:
  NetAddress() = default;
  explicit NetAddress(uint16_t port, bool ipv6 = false);
  NetAddress(const std::string& ip, uint16_t port, bool ipv6 = false);

  sa_family_t GetFamily() const;
  std::string GetIp() const;
  uint16_t GetPort() const;
  std::string GetIpAndPort() const;

  sockaddr* GetAddress();
  uint32_t GetSize() const;

  void SetSockAddrInet6(sockaddr_in6& in6);

 private:
  union {
    struct sockaddr_in in4_;
    struct sockaddr_in6 in6_;
  };
};
}  // namespace tohka
#endif  // TOHKA_TOHKA_NETADDRESS_H
