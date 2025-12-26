#include <gtest/gtest.h>
#include "cord/inc/cord.h"
#include <string>
#include <vector>
TEST(CordTest, FlatCord)
{
    std::vector<std::string> bucket{"some", "looooooooooooooong", "231", "string", "bleble", "aaaaaaaaaaaaaaaaaaaaaaaaaa"};
    std::string expected;

    using AllocSlice = ArenaAllocator<flatcord::Slice>;
    using AllocChar = ArenaAllocator<char>;

    ArenaState arena;
    AllocSlice alloc(&arena);
    flatcord::Cord<AllocSlice> cord(alloc);
    for (const auto &s : bucket)
    {
        cord.append(s);
        expected += s;
    }
    ArenaState arena2;
    AllocChar alloc2(&arena2);
    MemoryBuffer<ArenaAllocator<char>> buf(alloc2);

    cord.write_to(buf);

    std::string output{buf.data()};

    EXPECT_STREQ(expected.c_str(), output.c_str());
}
/*
TEST(CordTest, CordTest)
{
    ArenaState arena;
    Cord cord(arena);
    std::vector<std::string> bucket{"some", "looooooooooooooong", "231", "string", "bleble", "aaaaaaaaaaaaaaaaaaaaaaaaaa"};
    std::string expected;
    for (const auto &s : bucket)
    {
        cord.append(s);
        expected+=s;
    }

    MemoryBuffer buf;
    cord.write_to(buf);
    std::string output{buf.data()};

    EXPECT_STREQ(expected.c_str(), output.c_str());

}*/