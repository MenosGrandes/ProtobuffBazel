#include <benchmark/benchmark.h>
#include "cord/inc/cord.h"
#include <iostream>
#include <string>


static constexpr auto MIN_RANGE{1};
static constexpr auto MAX_RANGE{2};
static constexpr auto count_per_bucket = 100; // adjust as needed


struct StringConcatFixture : public benchmark::Fixture
{
    static std::vector<std::vector<std::string>> data;

    void SetUp(const ::benchmark::State &state) override {}
    void TearDown(const ::benchmark::State &) override {}
};

// static member definition
std::vector<std::vector<std::string>> StringConcatFixture::data;

// prepare input once before any benchmark runs
struct PrepareDataOnce
{
    PrepareDataOnce()
    {
        size_t max_len = 120; // maximum string length

        auto &data = StringConcatFixture::data;
        if (!data.empty())
            return;

        data.resize(max_len + 1);

        std::mt19937 rng(123);
        std::uniform_int_distribution<size_t> len_dist(1, max_len); // variable length
        std::uniform_int_distribution<size_t> char_dist(0, 61);
        static constexpr char alphabet[] =
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

        for (size_t bucket = 0; bucket <= max_len; ++bucket)
        {
            auto &vec = data[bucket];
            vec.reserve(count_per_bucket);
            for (size_t i = 0; i < count_per_bucket; ++i)
            {
                size_t len = len_dist(rng); // random length
                std::string s(len, ' ');
                for (size_t j = 0; j < len; ++j)
                {
                    s[j] = alphabet[char_dist(rng)];
                }
                vec.push_back(std::move(s));
            }
        }
    }
};
static PrepareDataOnce prepare_data_once;
BENCHMARK_DEFINE_F(StringConcatFixture, BM_StdStringConcat)(benchmark::State &state)
{
    const auto &bucket = data[state.range(0)];

    for (auto _ : state)
    {
        std::string result;
        for (const auto &s : bucket)
            result += s;

        benchmark::DoNotOptimize(result);
    }

    state.SetBytesProcessed(static_cast<int64_t>(
        state.iterations() * bucket.size() * state.range(0)));
}
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
    }

    state.SetBytesProcessed(static_cast<int64_t>(
        state.iterations() * bucket.size() * state.range(0)));
}

BENCHMARK_REGISTER_F(StringConcatFixture, BM_StdStringConcatReserve)
    ->DenseRange(1, 128)
    ->Unit(benchmark::kNanosecond);

BENCHMARK_REGISTER_F(StringConcatFixture, BM_StdStringConcat)
    ->DenseRange(MIN_RANGE, MAX_RANGE)
    ->Unit(benchmark::kNanosecond);

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

BENCHMARK_DEFINE_F(StringConcatFixture, BM_FlatCordConcat)(benchmark::State &state)
{
    const auto &bucket = data[state.range(0)];

    for (auto _ : state)
    {
        ArenaState arena;
        flatcord::Cord cord(arena);
        for (const auto &s : bucket)
        {
            cord.append(s);
        }

        MemoryBuffer buf;
        cord.write_to(buf);

        benchmark::DoNotOptimize(buf.data());
        benchmark::DoNotOptimize(buf.size());
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


    