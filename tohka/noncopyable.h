//
// Created by li on 2022/2/17.
//

#ifndef TOHKA_TOHKA_NONCOPYABLE_H
#define TOHKA_TOHKA_NONCOPYABLE_H

namespace tohka {

class noncopyable {
 public:
  noncopyable(const noncopyable&) = delete;
  noncopyable& operator=(const noncopyable&) = delete;

 protected:
  noncopyable() = default;
  ~noncopyable() = default;
};

}  // namespace tohka

#endif  // TOHKA_TOHKA_NONCOPYABLE_H
