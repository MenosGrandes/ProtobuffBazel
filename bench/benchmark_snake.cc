#include "camel/camel.h"
#include <benchmark/benchmark.h>
#include <google/protobuf/arena.h>
#include <iostream>
#include <string>


/*

template = """BENCHMARK_DEFINE_F(SnakeFixture, BM_RuntimeSnake_{length})(benchmark::State &state) {{
    const auto object_count = state.range(0);
    for (auto _ : state) {{
        [[maybe_unused]] std::array<int, 10> {var_name}{{1,2,3,4,5,6,7,8,9,10}};
        std::string parsed{{OBJ_TO_STR_RUNTIME({var_name})}};
        benchmark::DoNotOptimize(parsed);
        benchmark::DoNotOptimize({var_name});
    }}
    state.SetItemsProcessed(state.iterations() * object_count);
}}
BENCHMARK_REGISTER_F(SnakeFixture, BM_RuntimeSnake_{length})
    ->DenseRange(MIN_RANGE, MAX_RANGE, STEP)
    ->Unit(benchmark::kNanosecond);
BENCHMARK_DEFINE_F(SnakeFixture, BM_ConstexprSnake_{length})(benchmark::State &state) {{
    const auto object_count = state.range(0);
    for (auto _ : state) {{
        [[maybe_unused]] std::array<int, 10> {var_name}{{1,2,3,4,5,6,7,8,9,10}};
        std::string parsed{{OBJ_TO_STR_CONSTEXPR({var_name})}};
        benchmark::DoNotOptimize(parsed);
        benchmark::DoNotOptimize({var_name});
    }}
    state.SetItemsProcessed(state.iterations() * object_count);
}}
BENCHMARK_REGISTER_F(SnakeFixture, BM_ConstexprSnake_{length})
    ->DenseRange(MIN_RANGE, MAX_RANGE, STEP)
    ->Unit(benchmark::kNanosecond);
"""

# Function to generate a variable name of a given length
def make_var_name(length):
    name = ""
    for i in range(length):
        name += chr(ord('a') + (i % 26))  # cycle a-z
    return name

# Generate benchmarks for lengths 3 -> 103
for length in range(3, 20):
    var_name = make_var_name(length)
    print(template.format(length=length, var_name=var_name))


*/
static constexpr auto MIN_RANGE{1};
static constexpr auto MAX_RANGE{2};
static constexpr auto STEP{1};

////////

struct SnakeFixture : public benchmark::Fixture {};
BENCHMARK_DEFINE_F(SnakeFixture, BM_RuntimeSnake_3)(benchmark::State &state) {
    const auto object_count = state.range(0);
    for (auto _ : state) {
        [[maybe_unused]] std::array<int, 10> abc{1,2,3,4,5,6,7,8,9,10};
        std::string parsed{OBJ_TO_STR_RUNTIME(abc)};
        benchmark::DoNotOptimize(parsed);
        benchmark::DoNotOptimize(abc);
    }
    state.SetItemsProcessed(state.iterations() * object_count);
}
BENCHMARK_REGISTER_F(SnakeFixture, BM_RuntimeSnake_3)
    ->DenseRange(MIN_RANGE, MAX_RANGE, STEP)
    ->Unit(benchmark::kNanosecond);
BENCHMARK_DEFINE_F(SnakeFixture, BM_ConstexprSnake_3)(benchmark::State &state) {
    const auto object_count = state.range(0);
    for (auto _ : state) {
        [[maybe_unused]] std::array<int, 10> abc{1,2,3,4,5,6,7,8,9,10};
        std::string parsed{OBJ_TO_STR_CONSTEXPR(abc)};
        benchmark::DoNotOptimize(parsed);
        benchmark::DoNotOptimize(abc);
    }
    state.SetItemsProcessed(state.iterations() * object_count);
}
BENCHMARK_REGISTER_F(SnakeFixture, BM_ConstexprSnake_3)
    ->DenseRange(MIN_RANGE, MAX_RANGE, STEP)
    ->Unit(benchmark::kNanosecond);

BENCHMARK_DEFINE_F(SnakeFixture, BM_RuntimeSnake_4)(benchmark::State &state) {
    const auto object_count = state.range(0);
    for (auto _ : state) {
        [[maybe_unused]] std::array<int, 10> abcd{1,2,3,4,5,6,7,8,9,10};
        std::string parsed{OBJ_TO_STR_RUNTIME(abcd)};
        benchmark::DoNotOptimize(parsed);
        benchmark::DoNotOptimize(abcd);
    }
    state.SetItemsProcessed(state.iterations() * object_count);
}
BENCHMARK_REGISTER_F(SnakeFixture, BM_RuntimeSnake_4)
    ->DenseRange(MIN_RANGE, MAX_RANGE, STEP)
    ->Unit(benchmark::kNanosecond);
BENCHMARK_DEFINE_F(SnakeFixture, BM_ConstexprSnake_4)(benchmark::State &state) {
    const auto object_count = state.range(0);
    for (auto _ : state) {
        [[maybe_unused]] std::array<int, 10> abcd{1,2,3,4,5,6,7,8,9,10};
        std::string parsed{OBJ_TO_STR_CONSTEXPR(abcd)};
        benchmark::DoNotOptimize(parsed);
        benchmark::DoNotOptimize(abcd);
    }
    state.SetItemsProcessed(state.iterations() * object_count);
}
BENCHMARK_REGISTER_F(SnakeFixture, BM_ConstexprSnake_4)
    ->DenseRange(MIN_RANGE, MAX_RANGE, STEP)
    ->Unit(benchmark::kNanosecond);

BENCHMARK_DEFINE_F(SnakeFixture, BM_RuntimeSnake_5)(benchmark::State &state) {
    const auto object_count = state.range(0);
    for (auto _ : state) {
        [[maybe_unused]] std::array<int, 10> abcde{1,2,3,4,5,6,7,8,9,10};
        std::string parsed{OBJ_TO_STR_RUNTIME(abcde)};
        benchmark::DoNotOptimize(parsed);
        benchmark::DoNotOptimize(abcde);
    }
    state.SetItemsProcessed(state.iterations() * object_count);
}
BENCHMARK_REGISTER_F(SnakeFixture, BM_RuntimeSnake_5)
    ->DenseRange(MIN_RANGE, MAX_RANGE, STEP)
    ->Unit(benchmark::kNanosecond);
BENCHMARK_DEFINE_F(SnakeFixture, BM_ConstexprSnake_5)(benchmark::State &state) {
    const auto object_count = state.range(0);
    for (auto _ : state) {
        [[maybe_unused]] std::array<int, 10> abcde{1,2,3,4,5,6,7,8,9,10};
        std::string parsed{OBJ_TO_STR_CONSTEXPR(abcde)};
        benchmark::DoNotOptimize(parsed);
        benchmark::DoNotOptimize(abcde);
    }
    state.SetItemsProcessed(state.iterations() * object_count);
}
BENCHMARK_REGISTER_F(SnakeFixture, BM_ConstexprSnake_5)
    ->DenseRange(MIN_RANGE, MAX_RANGE, STEP)
    ->Unit(benchmark::kNanosecond);

BENCHMARK_DEFINE_F(SnakeFixture, BM_RuntimeSnake_6)(benchmark::State &state) {
    const auto object_count = state.range(0);
    for (auto _ : state) {
        [[maybe_unused]] std::array<int, 10> abcdef{1,2,3,4,5,6,7,8,9,10};
        std::string parsed{OBJ_TO_STR_RUNTIME(abcdef)};
        benchmark::DoNotOptimize(parsed);
        benchmark::DoNotOptimize(abcdef);
    }
    state.SetItemsProcessed(state.iterations() * object_count);
}
BENCHMARK_REGISTER_F(SnakeFixture, BM_RuntimeSnake_6)
    ->DenseRange(MIN_RANGE, MAX_RANGE, STEP)
    ->Unit(benchmark::kNanosecond);
BENCHMARK_DEFINE_F(SnakeFixture, BM_ConstexprSnake_6)(benchmark::State &state) {
    const auto object_count = state.range(0);
    for (auto _ : state) {
        [[maybe_unused]] std::array<int, 10> abcdef{1,2,3,4,5,6,7,8,9,10};
        std::string parsed{OBJ_TO_STR_CONSTEXPR(abcdef)};
        benchmark::DoNotOptimize(parsed);
        benchmark::DoNotOptimize(abcdef);
    }
    state.SetItemsProcessed(state.iterations() * object_count);
}
BENCHMARK_REGISTER_F(SnakeFixture, BM_ConstexprSnake_6)
    ->DenseRange(MIN_RANGE, MAX_RANGE, STEP)
    ->Unit(benchmark::kNanosecond);

BENCHMARK_DEFINE_F(SnakeFixture, BM_RuntimeSnake_7)(benchmark::State &state) {
    const auto object_count = state.range(0);
    for (auto _ : state) {
        [[maybe_unused]] std::array<int, 10> abcdefg{1,2,3,4,5,6,7,8,9,10};
        std::string parsed{OBJ_TO_STR_RUNTIME(abcdefg)};
        benchmark::DoNotOptimize(parsed);
        benchmark::DoNotOptimize(abcdefg);
    }
    state.SetItemsProcessed(state.iterations() * object_count);
}
BENCHMARK_REGISTER_F(SnakeFixture, BM_RuntimeSnake_7)
    ->DenseRange(MIN_RANGE, MAX_RANGE, STEP)
    ->Unit(benchmark::kNanosecond);
BENCHMARK_DEFINE_F(SnakeFixture, BM_ConstexprSnake_7)(benchmark::State &state) {
    const auto object_count = state.range(0);
    for (auto _ : state) {
        [[maybe_unused]] std::array<int, 10> abcdefg{1,2,3,4,5,6,7,8,9,10};
        std::string parsed{OBJ_TO_STR_CONSTEXPR(abcdefg)};
        benchmark::DoNotOptimize(parsed);
        benchmark::DoNotOptimize(abcdefg);
    }
    state.SetItemsProcessed(state.iterations() * object_count);
}
BENCHMARK_REGISTER_F(SnakeFixture, BM_ConstexprSnake_7)
    ->DenseRange(MIN_RANGE, MAX_RANGE, STEP)
    ->Unit(benchmark::kNanosecond);
