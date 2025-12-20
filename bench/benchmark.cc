#include <google/protobuf/arena.h>
#include "proto/my_proto.pb.h"
#include <benchmark/benchmark.h>

#include <iostream>
#include <string>

using google::protobuf::Arena;
using demo::BigObject;
using demo::BigObjectArray;


static constexpr int OBJECT_COUNT = 1000;
static constexpr int VALUES_PER_OBJECT = 2000;
static constexpr int TAGS_PER_OBJECT = 50;

/*
 * Benchmark WITHOUT Arena (default heap allocation)
 */
static void BM_NoArena(benchmark::State& state) {
    const int object_count = state.range(0);

    for (auto _ : state) {
        BigObjectArray container;

        for (int i = 0; i < object_count; ++i) {
            BigObject* obj = container.add_objects();

            obj->set_id(i);
            obj->set_name("Object_" + std::to_string(i));
            obj->set_description("No arena benchmark");

            obj->add_values(1.0);
            obj->add_tags("tag");
        }

        benchmark::DoNotOptimize(container);
    }

    // Report processed elements
    state.SetItemsProcessed(state.iterations() * object_count);
}

/*
 * Benchmark WITH Arena
 */
static void BM_Arena(benchmark::State& state) {
    const int object_count = state.range(0);

    for (auto _ : state) {
        google::protobuf::Arena arena;
        auto* container =
            google::protobuf::Arena::Create<BigObjectArray>(&arena);

        for (int i = 0; i < object_count; ++i) {
            BigObject* obj = container->add_objects();

            obj->set_id(i);
            obj->set_name("Object_" + std::to_string(i));
            obj->set_description("Arena benchmark");

            obj->add_values(1.0);
            obj->add_tags("tag");
        }

        benchmark::DoNotOptimize(container);
    }

    state.SetItemsProcessed(state.iterations() * object_count);
}
/*
BENCHMARK(BM_NoArena)
    ->RangeMultiplier(2)
    ->Range(2, 1 << 10);

BENCHMARK(BM_Arena)
    ->RangeMultiplier(2)
    ->Range(2, 1 << 10);
*/
static void BM_ParseFromString_NoArena(benchmark::State& state) {
    const int object_count = state.range(0);

    // 1️⃣ Prepare serialized message
    BigObjectArray container;
    for (int i = 0; i < object_count; ++i) {
        BigObject* obj = container.add_objects();
        obj->set_id(i);
        obj->set_name("Object_" + std::to_string(i));
        obj->set_description("No Arena parse benchmark");
        obj->add_values(1.0);
        obj->add_tags("tag");
    }

    std::string serialized;
    container.SerializeToString(&serialized);

    // 2️⃣ Benchmark parsing
    for (auto _ : state) {
        BigObjectArray parsed;
        parsed.ParseFromString(serialized);
        benchmark::DoNotOptimize(parsed);
    }

    state.SetItemsProcessed(state.iterations() * object_count);
}

static void BM_ParseFromString_Arena(benchmark::State& state) {
    const int object_count = state.range(0);

    // 1️⃣ Prepare serialized message
    Arena arena;
    auto* container = Arena::Create<BigObjectArray>(&arena);
    for (int i = 0; i < object_count; ++i) {
        BigObject* obj = container->add_objects();
        obj->set_id(i);
        obj->set_name("Object_" + std::to_string(i));
        obj->set_description("Arena parse benchmark");
        obj->add_values(1.0);
        obj->add_tags("tag");
    }

    std::string serialized;
    container->SerializeToString(&serialized);

    // 2️⃣ Benchmark parsing
    for (auto _ : state) {
        Arena parse_arena;
        auto* parsed = Arena::Create<BigObjectArray>(&parse_arena);
        parsed->ParseFromString(serialized);
        benchmark::DoNotOptimize(parsed);
    }

    state.SetItemsProcessed(state.iterations() * object_count);
}

// Register benchmarks with size range
BENCHMARK(BM_ParseFromString_NoArena)
    ->RangeMultiplier(2)
    ->Range(2, 1 << 10);

BENCHMARK(BM_ParseFromString_Arena)
    ->RangeMultiplier(2)
    ->Range(2, 1 << 10);