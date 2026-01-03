#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <memory>
#include <new>
#pragma once

class ArenaState {
  struct Block {
    Block *next;
    char *begin;
    char *current;
    char *end;
  };

  Block *head_ = nullptr;
  std::size_t block_size_;

public:
  explicit ArenaState(std::size_t block_size = 4096)
      : block_size_(block_size) {}

  ~ArenaState() {
    while (head_) {
      Block *next = head_->next;
      ::operator delete(head_->begin);
      delete head_;
      head_ = next;
    }
  }

  void *allocate(std::size_t size, std::size_t alignment) {
    if (!head_ || !try_allocate(head_, size, alignment)) {
      add_block(std::max(block_size_, size + alignment));
      if (!try_allocate(head_, size, alignment))
        throw std::bad_alloc();
    }
    return last_allocation_;
  }

  void reset() {
    while (head_->next) {
      Block *next = head_->next;
      ::operator delete(head_->begin);
      delete head_;
      head_ = next;
    }
    head_->current = head_->begin;
  }

private:
  void *last_allocation_ = nullptr;

  bool try_allocate(Block *b, std::size_t size, std::size_t alignment) {
    char *ptr = b->current;
    std::size_t space = b->end - b->current;

    void *aligned = ptr;
    if (!std::align(alignment, size, aligned, space))
      return false;

    b->current = static_cast<char *>(aligned) + size;
    last_allocation_ = aligned;
    return true;
  }

  void add_block(std::size_t size) {
    char *mem = static_cast<char *>(::operator new(size));
    head_ = new Block{head_, mem, mem, mem + size};
  }
};