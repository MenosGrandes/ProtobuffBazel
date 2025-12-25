#include <cassert>
#include <charconv>
#include <cstdint>
#include <cstring>
#include <string>
#include <iostream>
#include <string_view>
#include <string>
#include <random>

#include <cstddef>
#include <cstdlib>
#include <memory>
#include <new>
#include <string_view>
#include <cassert>
struct ArenaState {
    struct Block {
        Block* next;
        size_t used;
        alignas(std::max_align_t) char data[4096];
    };

    Block* head = nullptr;

    ~ArenaState() {
        while (head) {
            Block* next = head->next;
            std::free(head);
            head = next;
        }
    }

    void* allocate(size_t size, size_t align) {
        if (!head || !has_space(head, size, align))
            add_block();

        char* p = align_ptr(head->data + head->used, align);
        head->used = (p + size) - head->data;
        return p;
    }

private:
    static bool has_space(Block* b, size_t size, size_t align) {
        char* p = align_ptr(b->data + b->used, align);
        return p + size <= b->data + sizeof(b->data);
    }

    static char* align_ptr(char* p, size_t a) {
        uintptr_t v = reinterpret_cast<uintptr_t>(p);
        return reinterpret_cast<char*>((v + a - 1) & ~(a - 1));
    }

    void add_block() {
        Block* b = static_cast<Block*>(std::malloc(sizeof(Block)));
        b->used = 0;
        b->next = head;
        head = b;
    }
};
template<typename T>
class ArenaAllocator {
    ArenaState* state_;

public:
    using value_type = T;

    explicit ArenaAllocator(ArenaState& s) noexcept
        : state_(&s) {}

    template<typename U>
    ArenaAllocator(const ArenaAllocator<U>& other) noexcept
        : state_(other.state_) {}

    T* allocate(std::size_t n) {
        return static_cast<T*>(
            state_->allocate(sizeof(T) * n, alignof(T))
        );
    }

    void deallocate(T*, std::size_t) noexcept {}

    template<typename U>
    friend class ArenaAllocator;
};


struct Node {
    std::string_view view;
    Node* next;
};
template<typename CharT = char>
class Cord {
    struct Node {
        std::basic_string_view<CharT> view;
        Node* next;
    };

    using Alloc = ArenaAllocator<Node>;
    using Traits = std::allocator_traits<Alloc>;

    Alloc alloc_;
    Node* head_ = nullptr;
    Node* tail_ = nullptr;

public:
    explicit Cord(ArenaState& arena)
        : alloc_(arena) {}

    void append(std::basic_string_view<CharT> sv) {
        Node* n = Traits::allocate(alloc_, 1);
        Traits::construct(alloc_, n, Node{sv, nullptr});

        if (!head_) {
            head_ = tail_ = n;
        } else {
            tail_->next = n;
            tail_ = n;
        }
    }

template<typename Buffer>
void write_to(Buffer& out) const {
    // 1️⃣ Precompute total size
    size_t total_size = 0;
    for (Node* n = head_; n; n = n->next) {
        total_size += n->view.size();
    }

    out.reserve(total_size);

    for (Node* n = head_; n; n = n->next) {
        out.write(n->view.data(), n->view.size());
    }
}
};


#include <cstdlib>
#include <cstring>

class MemoryBuffer {
    char*  data_     = nullptr;
    size_t size_     = 0;
    size_t capacity_ = 0;

public:
    ~MemoryBuffer() {
        std::free(data_);
    }

    const char* data() const { return data_; }
    size_t size() const { return size_; }

    void clear() { size_ = 0; }

    void write(const char* src, size_t len) {
        if (size_ + len > capacity_)
            grow(size_ + len);

        std::memcpy(data_ + size_, src, len);
        size_ += len;
    }
void reserve(size_t new_cap) {
    if (capacity_ < new_cap) {
        data_ = static_cast<char*>(std::realloc(data_, new_cap));
        capacity_ = new_cap;
    }
}
private:
    void grow(size_t min_capacity) {
        size_t new_cap = capacity_ ? capacity_ : 64;
        while (new_cap < min_capacity)
            new_cap *= 2;

        data_ = static_cast<char*>(std::realloc(data_, new_cap));
        capacity_ = new_cap;
    }
};