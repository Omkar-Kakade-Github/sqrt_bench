# Benchmarking and Validation

## Configure

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

If Catch2 or Google Benchmark are not installed locally, CMake fetches them with
`FetchContent` during configuration.

## Validate

```sh
ctest --test-dir build --output-on-failure
./build/sqrt_bench_report
```

Validation compares every method against `std::sqrt` for `float` and `double`.
The dataset includes tiny positive values, values near one, fractional values,
large finite values, powers of two, and deterministic logarithmic-distribution
random samples.

## Benchmark

```sh
./build/sqrt_benchmarks
```

Benchmarks use a deterministic fixed dataset per type and report throughput for
each method. `std_sqrt` is the baseline for performance comparisons.
