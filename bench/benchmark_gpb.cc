#include "proto/my_proto.pb.h"
#include <benchmark/benchmark.h>
#include <google/protobuf/arena.h>
#include <iostream>
#include <string>

using demo::BigObject;
using demo::BigObjectArray;
using google::protobuf::Arena;
using google::protobuf::ArenaOptions;

static constexpr auto MIN_RANGE{40};
static constexpr auto MAX_RANGE{80};
static constexpr auto STEP{4};

////////

struct GPBFixture : public benchmark::Fixture {
  static std::vector<std::string> data;
};

std::vector<std::string> BuildSerializedBigObjectArray() {
  std::vector<std::string> serializedV;
  static constexpr auto SIZE{MIN_RANGE + MAX_RANGE};
  serializedV.reserve(SIZE);
  // 1 string = 1 BigObjectArray

  for (int k = 0; k < SIZE; ++k) {
    Arena arena;
    auto *container = Arena::Create<BigObjectArray>(&arena);

    for (int i = 0; i < SIZE; ++i) {

      auto *obj = container->add_objects();
      obj->set_id(i);
      obj->set_name("Object_" + std::to_string(i));
      obj->set_description("Arena parse benchmark");

      for (int j = 0; j < SIZE; ++j) {
        obj->add_values(j);
        obj->add_tags("tag" + std::to_string(j));
      }
    }
    std::string serialized;
    container->SerializeToString(&serialized);

    serializedV.emplace_back(serialized);
  }

  return serializedV;
}
std::vector<std::string> GPBFixture::data = BuildSerializedBigObjectArray();

///////
BENCHMARK_DEFINE_F(GPBFixture, BM_ParseFromString_NoArena)
(benchmark::State &state) {
  const auto object_count = state.range(0);
  const std::string &serialized = data[object_count];

  for (auto _ : state) {
    BigObjectArray parsed;
    parsed.ParseFromString(serialized);
    benchmark::DoNotOptimize(parsed);
  }

  state.SetItemsProcessed(state.iterations() * object_count);
}

BENCHMARK_REGISTER_F(GPBFixture, BM_ParseFromString_NoArena)
    ->DenseRange(MIN_RANGE, MAX_RANGE, STEP)
    ->Unit(benchmark::kNanosecond);
////////////
BENCHMARK_DEFINE_F(GPBFixture, BM_ParseFromString_Arena)
(benchmark::State &state) {
  const auto object_count = state.range(0);
  const std::string &serialized = data[object_count];

  for (auto _ : state) {
    Arena parse_arena;
    auto *parsed = Arena::Create<BigObjectArray>(&parse_arena);
    parsed->ParseFromString(serialized);
    benchmark::DoNotOptimize(parsed);
  }

  state.SetItemsProcessed(state.iterations() * object_count);
}
BENCHMARK_REGISTER_F(GPBFixture, BM_ParseFromString_Arena)
    ->DenseRange(MIN_RANGE, MAX_RANGE, STEP)
    ->Unit(benchmark::kNanosecond);

BENCHMARK_DEFINE_F(GPBFixture, BM_ParseFromString_Arena_ReservedSizeOfArena)
(benchmark::State &state) {
  const int object_count = state.range(0);
  const std::string &serialized = data[object_count];

  for (auto _ : state) {
    ArenaOptions options;
    options.initial_block_size = serialized.size() * 3;
    Arena parse_arena(options);
    auto *parsed = Arena::Create<BigObjectArray>(&parse_arena);
    parsed->ParseFromString(serialized);
    benchmark::DoNotOptimize(parsed);
  }

  state.SetItemsProcessed(state.iterations() * object_count);
}

BENCHMARK_REGISTER_F(GPBFixture, BM_ParseFromString_Arena_ReservedSizeOfArena)
    ->DenseRange(MIN_RANGE, MAX_RANGE, STEP)
    ->Unit(benchmark::kNanosecond);

BENCHMARK_DEFINE_F(GPBFixture, BM_ParseFromString_ArenaOptions)
(benchmark::State &state) {
  const int object_count = state.range(0);
  const std::string &serialized = data[object_count];

  for (auto _ : state) {
    ArenaOptions options;
    auto estimated{serialized.size() * 3};
    options.initial_block_size = estimated;
    options.start_block_size = estimated;
    options.max_block_size = estimated;
    Arena parse_arena(options);
    auto *parsed = Arena::Create<BigObjectArray>(&parse_arena);
    parsed->ParseFromString(serialized);
    benchmark::DoNotOptimize(parsed);
  }

  state.SetItemsProcessed(state.iterations() * object_count);
}

BENCHMARK_REGISTER_F(GPBFixture, BM_ParseFromString_ArenaOptions)
    ->DenseRange(MIN_RANGE, MAX_RANGE, STEP)
    ->Unit(benchmark::kNanosecond);

BENCHMARK_DEFINE_F(GPBFixture, BM_ParseFromString_ArenaParseFromArray)
(benchmark::State &state) {
  const int object_count = state.range(0);
  const std::string &serialized = data[object_count];

  const void *data = serialized.data();
  const size_t size = serialized.size();
  for (auto _ : state) {
    ArenaOptions options;
    auto estimated{serialized.size() * 3};
    options.initial_block_size = estimated;
    options.start_block_size = estimated;
    options.max_block_size = estimated;
    Arena parse_arena(options);
    auto *parsed = Arena::Create<BigObjectArray>(&parse_arena);
    parsed->ParseFromArray(data, size);

    benchmark::DoNotOptimize(parsed);
  }

  state.SetItemsProcessed(state.iterations() * object_count);
}

BENCHMARK_REGISTER_F(GPBFixture, BM_ParseFromString_ArenaParseFromArray)
    ->DenseRange(MIN_RANGE, MAX_RANGE, STEP)
    ->Unit(benchmark::kNanosecond);

