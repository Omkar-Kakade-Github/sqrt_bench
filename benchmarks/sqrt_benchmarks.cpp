#include <sqrt_bench/sqrt_bench.hpp>

#include <benchmark/benchmark.h>

#include <cstddef>

namespace {

template <typename T>
void run_method_benchmark(benchmark::State &state, sqrt_bench::Method method) {
  const auto inputs = sqrt_bench::benchmark_inputs<T>();
  std::size_t index = 0;

  for (auto _ : state) {
    const T value = inputs[index++ % inputs.size()];
    benchmark::DoNotOptimize(sqrt_bench::sqrt_method(method, value));
  }

  state.SetItemsProcessed(state.iterations());
}

template <typename T>
void register_type_benchmarks(const char *type_name) {
  for (const auto method : sqrt_bench::kAllMethods) {
    const auto name = std::string{"sqrt/"} + type_name + "/" +
                      std::string{sqrt_bench::method_id(method)};
    benchmark::RegisterBenchmark(
        name.c_str(), [method](benchmark::State &state) {
          run_method_benchmark<T>(state, method);
        });
  }
}

} // namespace

int main(int argc, char **argv) {
  register_type_benchmarks<float>("float");
  register_type_benchmarks<double>("double");
  benchmark::Initialize(&argc, argv);
  if (benchmark::ReportUnrecognizedArguments(argc, argv)) {
    return 1;
  }
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();
  return 0;
}
