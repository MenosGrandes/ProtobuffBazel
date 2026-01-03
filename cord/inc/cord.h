#include <cassert>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <new>
#include <random>
#include <string>
#include <string_view>

#pragma once
/*
template <typename CharT = char>
class Cord
{
    struct Node
    {
        std::basic_string_view<CharT> view;
        Node *next;
    };

    using Alloc = ArenaAllocator<Node>;
    using Traits = std::allocator_traits<Alloc>;

    Alloc alloc_;
    Node *head_ = nullptr;
    Node *tail_ = nullptr;

public:
    explicit Cord(ArenaState &arena)
        : alloc_(arena) {}

    void append(std::basic_string_view<CharT> sv)
    {
        Node *n = Traits::allocate(alloc_, 1);
        Traits::construct(alloc_, n, Node{sv, nullptr});

        if (!head_)
        {
            head_ = tail_ = n;
        }
        else
        {
            tail_->next = n;
            tail_ = n;
        }
    }

    template <typename Buffer>
    void write_to(Buffer &out) const
    {
        size_t total_size = 0;
        for (Node *n = head_; n; n = n->next)
        {
            total_size += n->view.size();
        }

        out.reserve(total_size);

        for (Node *n = head_; n; n = n->next)
        {
            out.write(n->view.data(), n->view.size());
        }
    }
};
*/
namespace flatcord {
struct Slice {
  const char *data;
  size_t size;
};

template <typename SliceAlloc = std::allocator<Slice>> class Cord {
  using Traits = std::allocator_traits<SliceAlloc>;

public:
  explicit Cord(const SliceAlloc &alloc = SliceAlloc())
      : alloc_(alloc), slices_(nullptr), sliceCount(0), capacity_(0),
        total_bytes_(0) {}

  void append(const char *data, size_t size) {
    ensure_capacity(sliceCount + 1);
    Traits::construct(alloc_, &slices_[sliceCount], Slice{data, size});
    sliceCount++;
    total_bytes_ += size;
  }

  void append(std::string_view sv) { append(sv.data(), sv.size()); }

  size_t size_bytes() const { return total_bytes_; }

  template <typename Buffer> void write_to(Buffer &out) const {
    out.reserve(total_bytes_);
    for (size_t i = 0; i < sliceCount; i++) {
      out.write(slices_[i].data, slices_[i].size);
    }
  }

private:
  void ensure_capacity(size_t needed) {
    if (needed <= capacity_)
      return;

    size_t new_cap = capacity_ ? capacity_ * 2 : 8;
    while (new_cap < needed)
      new_cap *= 2;

    Slice *new_slices = Traits::allocate(alloc_, new_cap);
    for (size_t i = 0; i < sliceCount; i++) {
      Traits::construct(alloc_, &new_slices[i], slices_[i]);
    }
    for (size_t i = 0; i < sliceCount; i++) {
      Traits::destroy(alloc_, &slices_[i]);
    }
    if (slices_) {
      Traits::deallocate(alloc_, slices_, capacity_);
    }

    slices_ = new_slices;
    capacity_ = new_cap;
  }

  SliceAlloc alloc_;
  Slice *slices_{nullptr};
  std::size_t sliceCount;
  std::size_t capacity_;
  std::size_t total_bytes_;
};
} // namespace flatcord

namespace flatcordsso {
struct Slice {
  const char *data;
  size_t size;
};

template <typename SliceAlloc = std::allocator<Slice>> class Cord {
  static constexpr size_t SSO_CAP{8};

  using Traits = std::allocator_traits<SliceAlloc>;

public:
  explicit Cord(const SliceAlloc &alloc = SliceAlloc()) noexcept
      : alloc(alloc) {
    slices = sso;
  }

  ~Cord() {
    if (slices != sso) {
      Traits::deallocate(alloc, slices, capacity);
    }
  }

  void append(const char *data, size_t size) noexcept {
    if (sliceCount == capacity) {
      grow();
    }

    slices[sliceCount++] = {data, size};
    total_bytes += size;
  }

  void append(std::string_view sv) noexcept { append(sv.data(), sv.size()); }

  size_t size_bytes() const noexcept { return total_bytes; }

  template <typename Buffer> void write_to(Buffer &out) const {
    out.reserve(total_bytes);

    for (size_t i = 0; i < sliceCount; ++i) {
      out.write(slices[i].data, slices[i].size);
    }
  }

private:
  void grow() {
    size_t new_cap = capacity * 2;
    Slice *new_slices = Traits::allocate(alloc, new_cap);

    std::memcpy(new_slices, slices, sliceCount * sizeof(Slice));

    if (slices != sso) {
      Traits::deallocate(alloc, slices, capacity);
    }

    slices = new_slices;
    capacity = new_cap;
  }

  SliceAlloc alloc;

  Slice *slices;
  size_t sliceCount{0};
  size_t capacity{SSO_CAP};
  size_t total_bytes{0};

  Slice sso[SSO_CAP];
};
} // namespace flatcordsso
