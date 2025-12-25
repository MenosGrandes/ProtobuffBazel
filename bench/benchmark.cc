#include <google/protobuf/arena.h>
#include "proto/my_proto.pb.h"
#include <benchmark/benchmark.h>
#include "cord/cord.h"
#include <iostream>
#include <string>

using google::protobuf::Arena;
using demo::BigObject;
using demo::BigObjectArray;


static constexpr int OBJECT_COUNT = 1000;
static constexpr int VALUES_PER_OBJECT = 2000;
static constexpr int TAGS_PER_OBJECT = 50;
static constexpr auto MIN_RANGE{20};
static constexpr auto MAX_RANGE{50};
static constexpr auto count_per_len = 1000; // adjust as needed

//  * Benchmark WITHOUT Arena (default heap allocation)
//  */
// static void BM_NoArena(benchmark::State& state) {
//     const int object_count = state.range(0);

//     for (auto _ : state) {
//         BigObjectArray container;

//         for (int i = 0; i < object_count; ++i) {
//             BigObject* obj = container.add_objects();

//             obj->set_id(i);
//             obj->set_name("Object_" + std::to_string(i));
//             obj->set_description("No arena benchmark");

//             obj->add_values(1.0);
//             obj->add_tags("tag");
//         }

//         benchmark::DoNotOptimize(container);
//     }

//     // Report processed elements
//     state.SetItemsProcessed(state.iterations() * object_count);
// }

// /*
//  * Benchmark WITH Arena
//  */
// static void BM_Arena(benchmark::State& state) {
//     const int object_count = state.range(0);

//     for (auto _ : state) {
//         google::protobuf::Arena arena;
//         auto* container =
//             google::protobuf::Arena::Create<BigObjectArray>(&arena);

//         for (int i = 0; i < object_count; ++i) {
//             BigObject* obj = container->add_objects();

//             obj->set_id(i);
//             obj->set_name("Object_" + std::to_string(i));
//             obj->set_description("Arena benchmark");

//             obj->add_values(1.0);
//             obj->add_tags("tag");
//         }

//         benchmark::DoNotOptimize(container);
//     }

//     state.SetItemsProcessed(state.iterations() * object_count);
// }
// /*
// BENCHMARK(BM_NoArena)
//     ->RangeMultiplier(2)
//     ->Range(2, 1 << 10);

// BENCHMARK(BM_Arena)
//     ->RangeMultiplier(2)
//     ->Range(2, 1 << 10);
// */
// static void BM_ParseFromString_NoArena(benchmark::State& state) {
//     const int object_count = state.range(0);

//     // 1️⃣ Prepare serialized message
//     BigObjectArray container;
//     for (int i = 0; i < object_count; ++i) {
//         BigObject* obj = container.add_objects();
//         obj->set_id(i);
//         obj->set_name("Object_" + std::to_string(i));
//         obj->set_description("No Arena parse benchmark");
//         obj->add_values(1.0);
//         obj->add_tags("tag");
//     }

//     std::string serialized;
//     container.SerializeToString(&serialized);

//     // 2️⃣ Benchmark parsing
//     for (auto _ : state) {
//         BigObjectArray parsed;
//         parsed.ParseFromString(serialized);
//         benchmark::DoNotOptimize(parsed);
//     }

//     state.SetItemsProcessed(state.iterations() * object_count);
// }

// static void BM_ParseFromString_Arena(benchmark::State& state) {
//     const int object_count = state.range(0);

//     // 1️⃣ Prepare serialized message
//     Arena arena;
//     auto* container = Arena::Create<BigObjectArray>(&arena);
//     for (int i = 0; i < object_count; ++i) {
//         BigObject* obj = container->add_objects();
//         obj->set_id(i);
//         obj->set_name("Object_" + std::to_string(i));
//         obj->set_description("Arena parse benchmark");
//         obj->add_values(1.0);
//         obj->add_tags("tag");
//     }

//     std::string serialized;
//     container->SerializeToString(&serialized);

//     // 2️⃣ Benchmark parsing
//     for (auto _ : state) {
//         Arena parse_arena;
//         auto* parsed = Arena::Create<BigObjectArray>(&parse_arena);
//         parsed->ParseFromString(serialized);
//         benchmark::DoNotOptimize(parsed);
//     }

//     state.SetItemsProcessed(state.iterations() * object_count);
// }

// // Register benchmarks with size range
// BENCHMARK(BM_ParseFromString_NoArena)
//     ->RangeMultiplier(2)
//     ->Range(2, 1 << 10);

// BENCHMARK(BM_ParseFromString_Arena)
//     ->RangeMultiplier(2)
//     ->Range(2, 1 << 10);

static std::vector<std::vector<std::string>> g_data;

std::string random_string(std::size_t len) {
    static thread_local std::mt19937 rng(123);
    static constexpr char chars[] =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    std::uniform_int_distribution<std::size_t> dist(0, sizeof(chars) - 2);

    std::string s;
    s.resize(len);
    for (std::size_t i = 0; i < len; ++i)
        s[i] = chars[dist(rng)];
    return s;
}

void generate_data(std::size_t max_len) {
    g_data.resize(max_len + 1);
    for (std::size_t len = 0; len <= max_len; ++len) {
        auto& bucket = g_data[len];
        bucket.reserve(count_per_len);
        for (std::size_t i = 0; i < count_per_len; ++i)
            bucket.push_back(random_string(len));
    }
}

struct StringConcatFixture : public benchmark::Fixture {
    std::vector<std::vector<std::string>> data;

    void SetUp(const ::benchmark::State& state) override {
        size_t max_len = state.range(0);

        data.resize(max_len + 1);

        std::mt19937 rng(123);
        std::uniform_int_distribution<size_t> dist(0, 61);
        static constexpr char alphabet[] =
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

        for (size_t len = 0; len <= max_len; ++len) {
            auto& bucket = data[len];
            bucket.reserve(count_per_len);
            for (size_t i = 0; i < count_per_len; ++i) {
                std::string s(len, ' ');
                for (size_t j = 0; j < len; ++j)
                    s[j] = alphabet[dist(rng)];
                bucket.push_back(std::move(s));
            }
        }
    }

    void TearDown(const ::benchmark::State&) override {
        data.clear();
    }
};
BENCHMARK_DEFINE_F(StringConcatFixture, BM_StdStringConcat)(benchmark::State& state) {
    const auto& bucket = data[state.range(0)];

    for (auto _ : state) {
        std::string result;
        for (const auto& s : bucket)
            result += s;

        benchmark::DoNotOptimize(result);
    }

    state.SetBytesProcessed(static_cast<int64_t>(
        state.iterations() * bucket.size() * state.range(0)
    ));
}
BENCHMARK_DEFINE_F(StringConcatFixture, BM_StdStringConcatReserve)(benchmark::State& state) {
    const auto& bucket = data[state.range(0)];
    size_t total_len = 0;
    for (const auto& s : bucket)
        total_len += s.size();

    for (auto _ : state) {
        std::string result;
        result.reserve(total_len); // reserve total capacity once

        for (const auto& s : bucket)
            result += s;

        benchmark::DoNotOptimize(result);
    }

    state.SetBytesProcessed(static_cast<int64_t>(
        state.iterations() * bucket.size() * state.range(0)
    ));
}
/*
BENCHMARK_REGISTER_F(StringConcatFixture, BM_StdStringConcatReserve)
    ->DenseRange(1, 128)
    ->Unit(benchmark::kNanosecond);
*/
BENCHMARK_REGISTER_F(StringConcatFixture, BM_StdStringConcat)
    ->DenseRange(MIN_RANGE, MAX_RANGE)
    ->Unit(benchmark::kNanosecond);

BENCHMARK_DEFINE_F(StringConcatFixture, BM_CordConcat)(benchmark::State& state) {
    const auto& bucket = data[state.range(0)];

    for (auto _ : state) {
        ArenaState arena;        // recreated each iteration
        Cord<char> cord(arena);  // fresh cord

        for (const auto& s : bucket){
            cord.append(s);}

        MemoryBuffer buf;
        cord.write_to(buf);

        benchmark::DoNotOptimize(buf.data());
        benchmark::DoNotOptimize(buf.size());
    }

    std::size_t total_bytes = 0;
    for (const auto& s : bucket)
        total_bytes += s.size();

    state.SetBytesProcessed(static_cast<int64_t>(
        state.iterations() * total_bytes
    ));
}

BENCHMARK_REGISTER_F(StringConcatFixture, BM_CordConcat)
    ->DenseRange(MIN_RANGE, MAX_RANGE)
    ->Unit(benchmark::kNanosecond);