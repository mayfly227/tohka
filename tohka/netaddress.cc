//
// Created by li on 2022/2/27.
//

#include "netaddress.h"
using namespace tohka;
NetAddress::NetAddress(uint16_t port, bool ipv6) {
  if (ipv6) {
    ipv6_ = true;
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

std::string NetAddress::GetIp() {
  char ip[64]{0};
  if (ipv6_) {
    ::inet_ntop(AF_INET6, &in6_.sin6_addr, ip, sizeof(ip));
  } else {
    ::inet_ntop(AF_INET, &in4_.sin_addr, ip, sizeof(ip));
  }
  return {ip};
}
uint16_t NetAddress::GetPort() {
  if (ipv6_) {
    return ntohs(in6_.sin6_port);
  } else {
    return ntohs(in4_.sin_port);
  }
}
std::string NetAddress::GetIpAndPort() {
  return GetIp() + ":" + std::to_string(GetPort());
}
std::any NetAddress::GetAddress() {
  if (ipv6_) {
    return (sockaddr*)&in6_;
  }
  return (sockaddr*)&in4_;
}
uint32_t NetAddress::GetSize() {
  if (ipv6_) {
    return sizeof(in6_);
  }
  return sizeof(in4_);
}
