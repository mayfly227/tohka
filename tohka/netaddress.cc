//
// Created by li on 2022/2/27.
//

#include "netaddress.h"
using namespace tohka;
NetAddress::NetAddress(uint16_t port, bool ipv6) {
  if (ipv6) {
    ::memset(&in6_, 0, sizeof(in6_));
    in6_.sin6_family = AF_INET6;
    in6_.sin6_port = htons(port);
    in6_.sin6_addr = in6addr_any;
  } else {
    ::memset(&in4_, 0, sizeof(in4_));
    in4_.sin_family = AF_INET;
    in4_.sin_port = htons(port);
    in4_.sin_addr.s_addr = INADDR_ANY;
  }
};

NetAddress::NetAddress(const std::string& ip, int16_t port, bool ipv6)
    : NetAddress(port, ipv6) {
  if (ipv6) {
    ::inet_pton(AF_INET6, ip.c_str(), &in6_.sin6_addr);
  } else {
    ::inet_pton(AF_INET, ip.c_str(), &in4_.sin_addr);
  }
}

std::string NetAddress::GetIp() const {
  auto* sock = (sockaddr*)&in6_;
  char ip[64]{0};
  if (sock->sa_family == AF_INET6) {
    ::inet_ntop(AF_INET6, &in6_.sin6_addr, ip, sizeof(ip));
  } else {
    ::inet_ntop(AF_INET, &in4_.sin_addr, ip, sizeof(ip));
  }
  return {ip};
}
uint16_t NetAddress::GetPort() const {
  auto* sock = (sockaddr*)&in6_;
  if (sock->sa_family == AF_INET6) {
    return ntohs(in6_.sin6_port);
  } else {
    return ntohs(in4_.sin_port);
  }
}
std::string NetAddress::GetIpAndPort() const {
  return GetIp() + ":" + std::to_string(GetPort());
}
sockaddr* NetAddress::GetAddress() { return (sockaddr*)&in6_; }
uint32_t NetAddress::GetSize() const {
  if (in4_.sin_family == AF_INET) {
    return sizeof(struct sockaddr_in);
  } else if (in4_.sin_family == AF_INET6) {
    return sizeof(struct sockaddr_in6);
  }

  return sizeof(struct sockaddr_in6);
}
sa_family_t NetAddress::GetFamily() const { return in4_.sin_family; }
void NetAddress::SetSockAddrInet6(sockaddr_in6& in6) { in6_ = in6; }
