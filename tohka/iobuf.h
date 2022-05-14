//
// Created by li on 2022/3/1.
//

#ifndef TOHKA_TOHKA_IOBUF_H
#define TOHKA_TOHKA_IOBUF_H

// prependable = readIndex
// readable = writeIndex - readIndex
// writable = size() - writeIndex
#include "platform.h"
namespace tohka {
class IoBuf {
 public:
  static constexpr size_t kPreparedSize = 4096;
  static constexpr size_t kPrependSize = 16;

  explicit IoBuf(size_t len = kPreparedSize + kPrependSize);
  // append data to buffer
  void Append(const char* data, size_t len);
  void Append(const void* data, size_t len);

  std::string ReceiveAllAsString();

  // Get the first pointer of readable data
  const char* Peek() { return Begin() + read_index_; }

  char* Begin() { return data_.data(); };
  void Retrieve(size_t len);
  void Refresh();

  size_t Read(char* buffer,size_t in);

  size_t GetReadableSize() const { return write_index_ - read_index_; }
  size_t GetWriteableSize() { return data_.size() - write_index_; }
  size_t GetReadIndex() const { return read_index_; }
  size_t GetWriteIndex() const { return write_index_; }

  void SetWriteIndex(size_t index) { write_index_ = index; }
  void EnsureWritableBytes(size_t len);
  void MakeSpace(size_t len);

  size_t GetBufferSize() { return data_.size(); };

 private:
  std::vector<char> data_;
  size_t read_index_;
  size_t write_index_;
};
}  // namespace tohka
#endif  // TOHKA_TOHKA_IOBUF_H
