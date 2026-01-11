#include "camel/camel.h"
#include <benchmark/benchmark.h>
#include <iostream>
#include <string>

/*
Pytyhon script to generate tests.
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
static constexpr auto MAX_RANGE{20};
static constexpr auto STEP{1};

////////

struct SnakeFixture : public benchmark::Fixture
{
};
class irange
{
    int _current, _end, _step;

public:
    irange(int start, int end, int step = 1)
        : _current(start), _end(end), _step(step) {}

    class iterator
    {
        int value, step;

    public:
        iterator(int v, int s) : value(v), step(s) {}
        int operator*() const { return value; }
        iterator &operator++()
        {
            value += step;
            return *this;
        }
        bool operator!=(const iterator &other) const
        {
            return step > 0 ? value < other.value : value > other.value;
        }
    };

    iterator begin() const { return iterator(_current, _step); }
    iterator end() const { return iterator(_end, _step); }
};

BENCHMARK_DEFINE_F(SnakeFixture, BM_RuntimeSnake_99)(benchmark::State &state)
{
    const auto object_count = state.range(0);
    [[maybe_unused]] std::array<int, 10> abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstu{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    for (auto _ : state)
    {
        for (int i : irange(0, object_count))
        {
            benchmark::DoNotOptimize(OBJ_TO_STR_RUNTIME(abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstu));
        }
    }
    benchmark::DoNotOptimize(abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstu);

    state.SetItemsProcessed(state.iterations() * object_count);
}

BENCHMARK_REGISTER_F(SnakeFixture, BM_RuntimeSnake_99)
    ->DenseRange(MIN_RANGE, MAX_RANGE, STEP)
    ->Unit(benchmark::kNanosecond);

    
BENCHMARK_DEFINE_F(SnakeFixture, BM_ConstexprSnake_99)(benchmark::State &state)
{
    const auto object_count = state.range(0);
    [[maybe_unused]] std::array<int, 10> abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstu{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    for (auto _ : state)
    {
        for (int i : irange(0, object_count))
        {
            benchmark::DoNotOptimize(OBJ_TO_STR_CONSTEXPR(abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstu));
        }
        benchmark::DoNotOptimize(abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstu);
    }
    state.SetItemsProcessed(state.iterations() * object_count);
}
BENCHMARK_REGISTER_F(SnakeFixture, BM_ConstexprSnake_99)
    ->DenseRange(MIN_RANGE, MAX_RANGE, STEP)
    ->Unit(benchmark::kNanosecond);
