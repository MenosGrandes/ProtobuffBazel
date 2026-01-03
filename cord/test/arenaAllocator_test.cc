#include <gtest/gtest.h>
#include <random>
#include <string>
#include <vector>
#include <cstdint>

#include "cord/inc/cord.h"
#include "cord/inc/arenaAllocator.h"
#include "cord/inc/arenaState.h"
#include "cord/inc/memoryBuffer.h"

template<typename T>
using ArenaVec = std::vector<T, ArenaAllocator<T>>;

using ArenaString =
    std::basic_string<char, std::char_traits<char>, ArenaAllocator<char>>;

struct TestObject {
    int id;
    double value;
    ArenaString text;

    bool operator==(const TestObject& o) const {
        return id == o.id && value == o.value && text == o.text;
    }
};

static ArenaString make_random_string(
    ArenaState& arena,
    std::mt19937& rng,
    std::size_t len)
{
    static constexpr char alphabet[] =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789";

    ArenaAllocator<char> alloc(&arena);
    ArenaString s(alloc);
    s.reserve(len);

    std::uniform_int_distribution<size_t> d(0, sizeof(alphabet) - 2);
    for (size_t i = 0; i < len; ++i)
        s.push_back(alphabet[d(rng)]);

    return s;
}

// ------------------------------------------------------------
// Basic allocation
// ------------------------------------------------------------
TEST(ArenaAllocatorTest, AllocatePrimitiveTypes)
{
    ArenaState arena(4096);
    ArenaAllocator<int> alloc(&arena);

    int* a = alloc.allocate(1);
    int* b = alloc.allocate(1);

    *a = 42;
    *b = 1337;

    EXPECT_EQ(*a, 42);
    EXPECT_EQ(*b, 1337);
    EXPECT_NE(a, b);
}

// ------------------------------------------------------------
// Alignment correctness
// ------------------------------------------------------------
TEST(ArenaAllocatorTest, AlignmentIsCorrect)
{
    ArenaState arena(4096);
    ArenaAllocator<std::max_align_t> alloc(&arena);

    for (int i = 0; i < 100; ++i) {
        void* p = alloc.allocate(1);
        EXPECT_EQ(reinterpret_cast<std::uintptr_t>(p) %
                  alignof(std::max_align_t), 0u);
    }
}

// ------------------------------------------------------------
// Large allocation crossing blocks
// ------------------------------------------------------------
TEST(ArenaAllocatorTest, LargeAllocationsCreateNewBlocks)
{
    ArenaState arena(1024);
    ArenaAllocator<char> alloc(&arena);

    char* p1 = alloc.allocate(800);
    char* p2 = alloc.allocate(800);
    char* p3 = alloc.allocate(800);

    EXPECT_NE(p1, p2);
    EXPECT_NE(p2, p3);

    p1[0] = 'a';
    p2[0] = 'b';
    p3[0] = 'c';

    EXPECT_EQ(p1[0], 'a');
    EXPECT_EQ(p2[0], 'b');
    EXPECT_EQ(p3[0], 'c');
}

