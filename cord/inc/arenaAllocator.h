#include "arenaState.h"
#pragma once

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