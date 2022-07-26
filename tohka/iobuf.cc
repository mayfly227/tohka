//
// Created by li on 2022/3/1.
//

#include "iobuf.h"

#include "util/log.h"
using namespace tohka;

IoBuf::IoBuf(size_t len)
    : data_(len), read_index_(kPrependSize), write_index_(kPrependSize) {}

void IoBuf::Append(const char* data, size_t len) {
  // get writeable space
  EnsureWritableBytes(len);
  std::copy(data, data + len, Begin() + write_index_);
  assert(GetWriteableSize() >= len);
  write_index_ += len;
}
void IoBuf::Append(const void* data, size_t len) {
  Append((const char*)data, len);
}
void IoBuf::EnsureWritableBytes(size_t len) {
  if (GetWriteableSize() < len) {
    // re malloc size
    MakeSpace(len);
  }
}
void IoBuf::MakeSpace(size_t len) {
  if (GetWriteableSize() + read_index_ < len + kPrependSize) {
    data_.resize(len + write_index_);
  } else {
    assert(kPrependSize < read_index_);
    // last readable size
    size_t readable = GetReadableSize();
    std::copy(Begin() + read_index_, Begin() + write_index_,
              Begin() + kPrependSize);
    read_index_ = kPrependSize;
    write_index_ = read_index_ + readable;
    // now readable size
    assert(readable == GetReadableSize());
  }
}
std::string IoBuf::ReceiveAllAsString() {
  size_t readable = GetReadableSize();

  // c++ 17
  std::string result{Peek(), 0, readable};

  // refresh
  Refresh();
  return result;
}
void IoBuf::Retrieve(size_t len) {
  assert(len <= GetReadableSize());
  if (len < GetReadableSize()) {
    read_index_ += len;
  } else {
    Refresh();
  }
}
void IoBuf::Refresh() {
  read_index_ = kPrependSize;
  write_index_ = kPrependSize;
}

size_t IoBuf::Read(char* buffer, size_t in) {
  size_t readable = GetReadableSize();
  size_t read = in >= readable ? readable : in;
  std::copy(Peek(), Peek() + read, buffer);
  Retrieve(read);
  return read;
}
size_t IoBuf::Read(void* buffer, size_t in) {
  char* data = static_cast<char*>(buffer);
  size_t readable = GetReadableSize();
  size_t read = in >= readable ? readable : in;
  std::copy(Peek(), Peek() + read, data);
  Retrieve(read);
  return read;
}
const char* IoBuf::FindCRLF() {
  // FIXME: replace with memmem()?
  const char* crlf = std::search(Peek(), BeginWrite(), kCRLF, kCRLF + 2);
  return crlf == BeginWrite() ? nullptr : crlf;
}
void IoBuf::Swap(IoBuf& other) {
  data_.swap(other.data_);
  std::swap(read_index_, other.read_index_);
  std::swap(write_index_, other.write_index_);
}
