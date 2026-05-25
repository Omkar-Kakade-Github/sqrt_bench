# sqrt_bench

`sqrt_bench` implements, validates, and benchmarks representative algorithms for
computing the non-negative square root of positive real scalar values.

The first version targets C++20, `float`, and `double`.

## Build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

The default build includes:

- `sqrt_bench`: library target.
- `sqrt_bench_report`: validation summary CLI.
- `sqrt_bench_tests`: Catch2 validation tests.
- `sqrt_benchmarks`: Google Benchmark benchmark binary.

Disable optional targets when dependencies are unavailable:

```sh
cmake -S . -B build -DSQRT_BENCH_BUILD_TESTS=OFF -DSQRT_BENCH_BUILD_BENCHMARKS=OFF
```

## Usage

```cpp
#include <sqrt_bench/sqrt_bench.hpp>

double y = sqrt_bench::sqrt_method(sqrt_bench::Method::NewtonRaphson, 2.0);
```

See `docs/algorithms.md` and `docs/benchmarking.md` for method notes and the
validation methodology.
