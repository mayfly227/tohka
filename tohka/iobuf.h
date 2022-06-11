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
  static constexpr char kCRLF[] = "\r\n";

  explicit IoBuf(size_t len = kPreparedSize + kPrependSize);
  // append data to buffer
  void Append(const char* data, size_t len);
  void Append(const void* data, size_t len);

  std::string ReceiveAllAsString();

  // Get the first pointer of readable data
  const char* Peek() { return Begin() + read_index_; }
  const char* BeginWrite() {return Begin()+write_index_;}
  char* Begin() { return data_.data(); };
  void Retrieve(size_t len);
  void Refresh();

  size_t Read(char* buffer, size_t in);
  size_t Read(void* buffer, size_t in);

  const char* FindCRLF();
  void RetrieveUntil(const char* end)
  {
    assert(Peek() <= end);
    assert(end <= BeginWrite());
    Retrieve(end - Peek());
  }
  void Prepend(const void* data, size_t len)
  {
    assert(len <= read_index_);
    read_index_ -= len;
    const char* d = static_cast<const char*>(data);
    std::copy(d, d+len, Begin()+read_index_);
  }
  size_t ReadUntil(const char* end,void *dst,size_t len)
  {
    assert(Peek() <= end);
    assert(end <= BeginWrite());
    auto can_read_len = end - Peek();
    assert(len > can_read_len);
    return Read(dst,can_read_len);
  }
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
