#include <benchmark/benchmark.h>
#include "cord/inc/cord.h"
#include <iostream>
#include <string>

static constexpr auto MIN_RANGE{10};
static constexpr auto MAX_RANGE{50};
static constexpr auto count_per_bucket{100};  // adjust as needed
static constexpr auto MAX_STRING_LENGTH{220}; // adjust as needed

struct StringConcatFixture : public benchmark::Fixture
{
    static std::vector<std::vector<std::string>> data;
};

inline size_t FixedLength(size_t bucket, std::mt19937 &)
{
    return bucket;
}

inline size_t RandomLength(size_t bucket, std::mt19937 &rng)
{
    std::uniform_int_distribution<size_t> dist(1, bucket);
    return dist(rng);
}

template <typename LengthFn>
std::vector<std::vector<std::string>>
MakeBenchmarkData(size_t max_len,
                  size_t count_per_bucket,
                  LengthFn length_fn)
{
    std::vector<std::vector<std::string>> data;
    data.resize(max_len + 1);

    std::mt19937 rng(123); // deterministic seed
    std::uniform_int_distribution<size_t> char_dist(0, 61);

    static constexpr char alphabet[] =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789";

    for (size_t bucket = 1; bucket <= max_len; ++bucket)
    {
        auto &vec = data[bucket];
        vec.reserve(count_per_bucket);

        for (size_t i = 0; i < count_per_bucket; ++i)
        {
            const size_t len = length_fn(bucket, rng);

            std::string s;
            s.resize(len);

            for (size_t j = 0; j < len; ++j)
                s[j] = alphabet[char_dist(rng)];

            vec.push_back(std::move(s));
        }
    }

    return data;
}
std::vector<std::vector<std::string>> StringConcatFixture::data =
    MakeBenchmarkData(MAX_STRING_LENGTH, count_per_bucket, RandomLength);
/*
BENCHMARK_DEFINE_F(StringConcatFixture, BM_StdStringConcat)(benchmark::State &state)
{
    const auto &bucket = data[state.range(0)];

    for (auto _ : state)
    {
        std::string result;
        for (const auto &s : bucket)
            result += s;

        benchmark::DoNotOptimize(result);
        benchmark::ClobberMemory();
    }

    state.SetBytesProcessed(static_cast<int64_t>(
        state.iterations() * bucket.size() * state.range(0)));
}
BENCHMARK_REGISTER_F(StringConcatFixture, BM_StdStringConcat)
    ->DenseRange(MIN_RANGE, MAX_RANGE)
    ->Unit(benchmark::kNanosecond);
*/
BENCHMARK_DEFINE_F(StringConcatFixture, BM_StdStringConcatReserve)(benchmark::State &state)
{
    const auto &bucket = data[state.range(0)];
    size_t total_len = 0;
    for (const auto &s : bucket)
        total_len += s.size();

    for (auto _ : state)
    {
        std::string result;
        result.reserve(total_len); // reserve total capacity once

        for (const auto &s : bucket)
            result += s;

        benchmark::DoNotOptimize(result);
        benchmark::ClobberMemory();
    }

    state.SetBytesProcessed(static_cast<int64_t>(
        state.iterations() * bucket.size() * state.range(0)));
}

BENCHMARK_REGISTER_F(StringConcatFixture, BM_StdStringConcatReserve)
    ->DenseRange(MIN_RANGE, MAX_RANGE)
    ->Unit(benchmark::kNanosecond);

/*
BENCHMARK_DEFINE_F(StringConcatFixture, BM_CordConcat)(benchmark::State &state)
{
    const auto &bucket = data[state.range(0)];

    for (auto _ : state)
    {
        ArenaState arena;       // recreated each iteration
        Cord<char> cord(arena); // fresh cord

        for (const auto &s : bucket)
        {
            cord.append(s);
        }

        MemoryBuffer buf;
        cord.write_to(buf);

        benchmark::DoNotOptimize(buf.data());
        benchmark::DoNotOptimize(buf.size());
        benchmark::ClobberMemory();
    }

    std::size_t total_bytes = 0;
    for (const auto &s : bucket)
        total_bytes += s.size();

    state.SetBytesProcessed(static_cast<int64_t>(
        state.iterations() * total_bytes));
}

BENCHMARK_REGISTER_F(StringConcatFixture, BM_CordConcat)
    ->DenseRange(MIN_RANGE, MAX_RANGE)
    ->Unit(benchmark::kNanosecond);
*/
BENCHMARK_DEFINE_F(StringConcatFixture, BM_FlatCordConcat)(benchmark::State &state)
{
    const auto &bucket = data[state.range(0)];

    for (auto _ : state)
    {
        using AllocSlice = ArenaAllocator<flatcord::Slice>;
        using AllocChar = ArenaAllocator<char>;

        ArenaState arena;
        AllocSlice alloc(&arena);
        flatcord::Cord<AllocSlice> cord(alloc);
        for (const auto &s : bucket)
        {
            cord.append(s);
        }
        AllocChar alloc2(&arena);
        MemoryBuffer<ArenaAllocator<char>> buf(alloc2);

        cord.write_to(buf);

        benchmark::DoNotOptimize(buf.data());
        benchmark::DoNotOptimize(buf.size());
        benchmark::ClobberMemory();
    }

    std::size_t total_bytes = 0;
    for (const auto &s : bucket)
        total_bytes += s.size();

    state.SetBytesProcessed(static_cast<int64_t>(
        state.iterations() * total_bytes));
}

BENCHMARK_REGISTER_F(StringConcatFixture, BM_FlatCordConcat)
    ->DenseRange(MIN_RANGE, MAX_RANGE)
    ->Unit(benchmark::kNanosecond);
