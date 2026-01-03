
#include <benchmark/benchmark.h>

#include "serdel/msg/a/msg_a.h"
#include "serdel/msg/b/msg_b.h"
#include "serdel/msg/base/base_msg.h"

#include "serdel/registry/types.h"
#include "serdel/serializer/types.h"

#include <memory>
#include <random>
#include <string>
#include <vector>

static constexpr auto MIN_RANGE{10};
static constexpr auto MAX_RANGE{100};
static constexpr auto STEP{10};
using TestDataType = std::vector<std::vector<std::unique_ptr<MsgBase>>>;
struct SerdelFixture : public benchmark::Fixture {
  static TestDataType data;
};

TestDataType MakeBenchmarkData(size_t max_len) {
  TestDataType data;
  data.resize(max_len + 1);
  const auto make_b = []() {
    B b;

    // scalar
    b.v = 3.1415926f;

    // map<uint32_t, string> with 100+ elements
    for (uint32_t i = 1; i <= 150; ++i) {
      b.map_uint_str.emplace(i, "value_" + std::to_string(i));
    }

    // vector<int64_t> with 150 elements
    for (int64_t i = -75; i <= 74; ++i) {
      b.vec_int64.push_back(i * 123456789LL); // scaled to be non-trivial
    }

    // unordered_map<SomeName, string> with 100+ elements
    for (int i = 0; i < 120; ++i) {
      SomeName k;
      k.f = static_cast<float>(i) * 1.1f;
      k.d = static_cast<double>(i) * 2.2;
      k.b = (i % 2 == 0);
      k.arr[0] = i;
      k.arr[1] = i + 1;
      k.arr[2] = i + 2;

      b.umap_somename_str.emplace(k, "entry_" + std::to_string(i));
    }

    return std::make_unique<B>(b);
  };
  for (std::size_t i{0}; i < max_len + 1; i++) {
    for (std::size_t j{0}; j < i; j++) {
      data[i].emplace_back(make_b());
    }
  }

  return data;
}
TestDataType SerdelFixture::data = MakeBenchmarkData(MAX_RANGE);

BENCHMARK_DEFINE_F(SerdelFixture, BM_MsgBBenchmarkClean)
(benchmark::State &state) {
  const auto &bucket = data[state.range(0)];

  for (auto _ : state) {

    for (const auto &s : bucket) {
      SerializeBuffer buf;
      serialize_msg(buf, s);
      size_t off = 0;
      auto msg = deserialize_msg(buf, off);
      benchmark::DoNotOptimize(msg);
      benchmark::DoNotOptimize(buf);
    }
  }
}
BENCHMARK_REGISTER_F(SerdelFixture, BM_MsgBBenchmarkClean)
    ->DenseRange(MIN_RANGE, MAX_RANGE, STEP)
    ->Unit(benchmark::kNanosecond);

#define USE_RESERVE
BENCHMARK_DEFINE_F(SerdelFixture, BM_MsgBBenchmarkReserve)
(benchmark::State &state) {
  const auto &bucket = data[state.range(0)];

  for (auto _ : state) {

    for (const auto &s : bucket) {
      SerializeBuffer buf;
      serialize_msg(buf, s);
      size_t off = 0;
      auto msg = deserialize_msg(buf, off);
      benchmark::DoNotOptimize(msg);
      benchmark::DoNotOptimize(buf);
    }
  }
}
BENCHMARK_REGISTER_F(SerdelFixture, BM_MsgBBenchmarkReserve)
    ->DenseRange(MIN_RANGE, MAX_RANGE, STEP)
    ->Unit(benchmark::kNanosecond);
