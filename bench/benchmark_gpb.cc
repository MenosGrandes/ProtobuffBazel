#include <google/protobuf/arena.h>
#include "proto/my_proto.pb.h"
#include <benchmark/benchmark.h>
#include <iostream>
#include <string>

using demo::BigObject;
using demo::BigObjectArray;
using google::protobuf::Arena;

static constexpr auto MIN_RANGE{1};
static constexpr auto MAX_RANGE{2};
struct GPBFixture : public benchmark::Fixture
{
    void SetUp(const ::benchmark::State &state) override {}
    void TearDown(const ::benchmark::State &) override {}
};

BENCHMARK_DEFINE_F(GPBFixture, BM_NoArena)(benchmark::State &state)
{
    const int object_count = state.range(0);

    for (auto _ : state)
    {
        BigObjectArray container;

        for (int i = 0; i < object_count; ++i)
        {
            BigObject *obj = container.add_objects();

            obj->set_id(i);
            obj->set_name("Object_" + std::to_string(i));
            obj->set_description("No arena benchmark");

            obj->add_values(1.0);
            obj->add_tags("tag");
        }

        benchmark::DoNotOptimize(container);
    }

    state.SetItemsProcessed(state.iterations() * object_count);
}

BENCHMARK_DEFINE_F(GPBFixture, BM_Arena)(benchmark::State &state)
{
    const int object_count = state.range(0);

    for (auto _ : state)
    {
        google::protobuf::Arena arena;
        auto *container =
            google::protobuf::Arena::Create<BigObjectArray>(&arena);

        for (int i = 0; i < object_count; ++i)
        {
            BigObject *obj = container->add_objects();

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
BENCHMARK_REGISTER_F(GPBFixture, BM_Arena)
    ->RangeMultiplier(2)
    ->Range(2, 1 << MAX_RANGE)
    ->Unit(benchmark::kNanosecond);
BENCHMARK_REGISTER_F(GPBFixture, BM_NoArena)
    ->RangeMultiplier(2)
    ->Range(2, 1 << MAX_RANGE)
    ->Unit(benchmark::kNanosecond);
*/
BENCHMARK_DEFINE_F(GPBFixture, BM_ParseFromString_NoArena)(benchmark::State &state)
{
    const int object_count = state.range(0);

    // 1️⃣ Prepare serialized message
    BigObjectArray container;
    for (int i = 0; i < object_count; ++i)
    {
        BigObject *obj = container.add_objects();
        obj->set_id(i);
        obj->set_name("Object_" + std::to_string(i));
        obj->set_description("No Arena parse benchmark");
        obj->add_values(1.0);
        obj->add_tags("tag");
    }

    std::string serialized;
    container.SerializeToString(&serialized);

    for (auto _ : state)
    {
        BigObjectArray parsed;
        parsed.ParseFromString(serialized);
        benchmark::DoNotOptimize(parsed);
    }

    state.SetItemsProcessed(state.iterations() * object_count);
}
BENCHMARK_DEFINE_F(GPBFixture, BM_ParseFromString_Arena)(benchmark::State &state)
{
    const int object_count = state.range(0);

    Arena arena;
    auto *container = Arena::Create<BigObjectArray>(&arena);
    for (int i = 0; i < object_count; ++i)
    {
        BigObject *obj = container->add_objects();
        obj->set_id(i);
        obj->set_name("Object_" + std::to_string(i));
        obj->set_description("Arena parse benchmark");
        obj->add_values(1.0);
        obj->add_tags("tag");
    }

    std::string serialized;
    container->SerializeToString(&serialized);

    for (auto _ : state)
    {
        Arena parse_arena;
        auto *parsed = Arena::Create<BigObjectArray>(&parse_arena);
        parsed->ParseFromString(serialized);
        benchmark::DoNotOptimize(parsed);
    }

    state.SetItemsProcessed(state.iterations() * object_count);
}
BENCHMARK_REGISTER_F(GPBFixture, BM_ParseFromString_Arena)
    ->RangeMultiplier(2)
    ->Range(2, 1 << MAX_RANGE)
    ->Unit(benchmark::kNanosecond);

BENCHMARK_REGISTER_F(GPBFixture, BM_ParseFromString_NoArena)
    ->RangeMultiplier(2)
    ->Range(2, 1 << MAX_RANGE)
    ->Unit(benchmark::kNanosecond);