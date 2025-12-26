#include <cassert>
#include <charconv>
#include <cstdint>
#include <cstring>
#include <string>
#include <iostream>
#include <string_view>
#include <string>
#include <random>
#include <cstdlib>
#include <cstring>
#include <cstddef>
#include <cstdlib>
#include <memory>
#include <new>
#include <string_view>
#include <cassert>
static constexpr auto PAGE_DEFAULT_SIZE{4096};
// Basic arena state
struct ArenaState
{
    struct Block
    {
        Block *next;
        size_t used;
        alignas(std::max_align_t) char data[4096];
    };

    Block *head = nullptr;

    ~ArenaState()
    {
        while (head)
        {
            Block *next = head->next;
            std::free(head);
            head = next;
        }
    }

    void *allocate(size_t size, size_t align)
    {
        if (!head || !has_space(head, size, align))
        {
            add_block();
        }
        char *p = align_ptr(head->data + head->used, align);
        head->used = (p + size) - head->data;
        return p;
    }

private:
    static bool has_space(Block *b, size_t size, size_t align)
    {
        char *p = align_ptr(b->data + b->used, align);
        return p + size <= b->data + sizeof(b->data);
    }

    static char *align_ptr(char *p, size_t a)
    {
        uintptr_t v = reinterpret_cast<uintptr_t>(p);
        return reinterpret_cast<char *>((v + a - 1) & ~(a - 1));
    }

    void add_block()
    {
        Block *b = static_cast<Block *>(std::malloc(sizeof(Block)));
        b->used = 0;
        b->next = head;
        head = b;
    }
};

template <typename T>
class ArenaAllocator
{
public:
    using value_type = T;

    explicit ArenaAllocator(ArenaState *arena = nullptr) noexcept
        : arena_(arena) {}

    template <typename U>
    ArenaAllocator(const ArenaAllocator<U> &other) noexcept
        : arena_(other.arena_) {}

    T *allocate(std::size_t n)
    {
        void *ptr = arena_->allocate(n * sizeof(T), alignof(T));
        return static_cast<T *>(ptr);
    }

    void deallocate(T *, std::size_t) noexcept {}

    template <typename U>
    struct rebind
    {
        using other = ArenaAllocator<U>;
    };

    ArenaState *arena_;
};

// Comparison operators (required by standard allocator interface)
template <typename T, typename U>
inline bool operator==(const ArenaAllocator<T> &a, const ArenaAllocator<U> &b) noexcept
{
    return a.arena_ == b.arena_;
}

template <typename T, typename U>
inline bool operator!=(const ArenaAllocator<T> &a, const ArenaAllocator<U> &b) noexcept
{
    return !(a == b);
}
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
namespace flatcord
{
    struct Slice
    {
        const char *data;
        size_t size;
    };

    template <typename SliceAlloc = std::allocator<Slice>>
    class Cord
    {
        using Traits = std::allocator_traits<SliceAlloc>;

    public:
        explicit Cord(const SliceAlloc &alloc = SliceAlloc())
            : alloc_(alloc), slices_(nullptr), sliceCount(0), capacity_(0), total_bytes_(0) {}

        void append(const char *data, size_t size)
        {
            ensure_capacity(sliceCount + 1);
            Traits::construct(alloc_, &slices_[sliceCount], Slice{data, size});
            sliceCount++;
            total_bytes_ += size;
        }

        void append(std::string_view sv) { append(sv.data(), sv.size()); }

        size_t size_bytes() const { return total_bytes_; }

        template <typename Buffer>
        void write_to(Buffer &out) const
        {
            out.reserve(total_bytes_);
            for (size_t i = 0; i < sliceCount; i++)
            {
                out.write(slices_[i].data, slices_[i].size);
            }
        }

    private:
        void ensure_capacity(size_t needed)
        {
            if (needed <= capacity_)
                return;

            size_t new_cap = capacity_ ? capacity_ * 2 : 8;
            while (new_cap < needed)
                new_cap *= 2;

            Slice *new_slices = Traits::allocate(alloc_, new_cap);
            for (size_t i = 0; i < sliceCount; i++)
            {
                Traits::construct(alloc_, &new_slices[i], slices_[i]);
            }
            for (size_t i = 0; i < sliceCount; i++)
            {
                Traits::destroy(alloc_, &slices_[i]);
            }
            if (slices_)
            {
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

template <typename Alloc = std::allocator<char>>
class MemoryBuffer
{
    using Traits = std::allocator_traits<Alloc>;
    Alloc alloc_;
    char *data_ = nullptr;
    size_t size_ = 0;
    size_t capacity_ = 0;

public:
    explicit MemoryBuffer(const Alloc &alloc = Alloc()) : alloc_(alloc) {}
    ~MemoryBuffer()
    {
        if (data_)
        {
            Traits::deallocate(alloc_, data_, capacity_);
        }
    }

    const char *data() const { return data_; }
    size_t size() const { return size_; }
    void clear() { size_ = 0; }

    void write(const char *src, size_t len)
    {
        if (size_ + len > capacity_)
        {
            grow(size_ + len);
        }
        std::memcpy(data_ + size_, src, len);
        size_ += len;
    }

    void reserve(size_t new_cap)
    {
        if (capacity_ < new_cap)
        {
            grow(new_cap);
        }
    }

private:
    void grow(size_t min_capacity)
    {
        size_t new_cap = capacity_ ? capacity_ : 64;
        while (new_cap < min_capacity)
        {
            new_cap *= 2;
        }

        char *new_data = Traits::allocate(alloc_, new_cap);
        if (data_)
        {
            std::memcpy(new_data, data_, size_);
            Traits::deallocate(alloc_, data_, capacity_);
        }

        data_ = new_data;
        capacity_ = new_cap;
    }
};