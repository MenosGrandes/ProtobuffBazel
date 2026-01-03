#include <cstring>
#include <memory>
#pragma once

template <typename Alloc = std::allocator<char>> class MemoryBuffer {
  using Traits = std::allocator_traits<Alloc>;
  Alloc alloc_;
  char *data_ = nullptr;
  size_t size_ = 0;
  size_t capacity_ = 0;

public:
  explicit MemoryBuffer(const Alloc &alloc = Alloc()) : alloc_(alloc) {}
  ~MemoryBuffer() {
    if (data_) {
      Traits::deallocate(alloc_, data_, capacity_);
    }
  }

  const char *data() const { return data_; }
  size_t size() const { return size_; }
  void clear() { size_ = 0; }

  void write(const char *src, size_t len) {
    if (size_ + len > capacity_) {
      grow(size_ + len);
    }
    std::memcpy(data_ + size_, src, len);
    size_ += len;
  }

  void reserve(size_t new_cap) {
    if (capacity_ < new_cap) {
      grow(new_cap);
    }
  }

private:
  void grow(size_t min_capacity) {
    size_t new_cap = capacity_ ? capacity_ : 64;
    while (new_cap < min_capacity) {
      new_cap *= 2;
    }

    char *new_data = Traits::allocate(alloc_, new_cap);
    if (data_) {
      std::memcpy(new_data, data_, size_);
      Traits::deallocate(alloc_, data_, capacity_);
    }

    data_ = new_data;
    capacity_ = new_cap;
  }
};