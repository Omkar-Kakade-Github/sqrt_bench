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

## Current Results

These results were captured on 2026-05-25 with:

```sh
cmake -S . -B build_full -DCMAKE_BUILD_TYPE=Release
cmake --build build_full
ctest --test-dir build_full --output-on-failure
./build_full/sqrt_bench_report
./build_full/sqrt_benchmarks --benchmark_min_time=0.02s
```

CTest validation passed:

```text
100% tests passed, 0 tests failed out of 4
```

Google Benchmark reported CPU scaling was enabled, so benchmark timings should be
treated as a local sample rather than a reproducible performance claim.

### Validation Summary

All methods passed their configured tolerances against `std::sqrt`.

| type | method | max abs | max rel | max ulp |
| --- | --- | ---: | ---: | ---: |
| float | std_sqrt | 0.000000e+00 | 0.000000e+00 | 0 |
| float | pow_half | 0.000000e+00 | 0.000000e+00 | 0 |
| float | exp_log | 1.484341e+13 | 1.609326e-06 | 26 |
| float | newton_raphson | 5.497558e+11 | 1.164153e-07 | 1 |
| float | bisection | 5.497558e+11 | 1.177747e-07 | 1 |
| float | halley | 1.953125e-03 | 1.973861e-07 | 2 |
| float | goldschmidt | 9.765625e-04 | 1.150141e-07 | 1 |
| float | fast_inverse_sqrt | 1.953125e-03 | 1.839832e-07 | 2 |
| float | bit_estimate_newton | 5.497558e+11 | 1.164153e-07 | 1 |
| float | digit_by_digit | 9.765625e-04 | 1.164153e-07 | 1 |
| double | std_sqrt | 0.000000e+00 | 0.000000e+00 | 0 |
| double | pow_half | 0.000000e+00 | 0.000000e+00 | 0 |
| double | exp_log | 9.154679e+139 | 2.220446e-14 | 123 |
| double | newton_raphson | 7.442829e+137 | 2.220445e-16 | 1 |
| double | bisection | 7.442829e+137 | 2.220445e-16 | 1 |
| double | halley | 1.980704e+28 | 4.017431e-16 | 3 |
| double | goldschmidt | 1.980704e+28 | 2.220445e-16 | 1 |
| double | fast_inverse_sqrt | 1.980704e+28 | 2.775558e-16 | 2 |
| double | bit_estimate_newton | 7.442829e+137 | 2.220445e-16 | 1 |
| double | digit_by_digit | 1.547425e+26 | 2.220445e-16 | 1 |

### Benchmark Summary

Sample benchmark run on a 32-thread system reporting 5460.53 MHz CPUs:

| type | method | CPU time | throughput |
| --- | --- | ---: | ---: |
| float | std_sqrt | 1.55 ns | 646.214M/s |
| float | pow_half | 3.87 ns | 258.221M/s |
| float | exp_log | 4.28 ns | 233.821M/s |
| float | newton_raphson | 12.6 ns | 79.0781M/s |
| float | bisection | 90.5 ns | 11.0457M/s |
| float | halley | 10.3 ns | 97.2757M/s |
| float | goldschmidt | 9.66 ns | 103.494M/s |
| float | fast_inverse_sqrt | 8.90 ns | 112.34M/s |
| float | bit_estimate_newton | 8.80 ns | 113.58M/s |
| float | digit_by_digit | 44.6 ns | 22.4004M/s |
| double | std_sqrt | 1.73 ns | 578.384M/s |
| double | pow_half | 7.56 ns | 132.239M/s |
| double | exp_log | 6.76 ns | 147.931M/s |
| double | newton_raphson | 15.9 ns | 62.7689M/s |
| double | bisection | 186 ns | 5.38841M/s |
| double | halley | 13.2 ns | 75.6075M/s |
| double | goldschmidt | 11.7 ns | 85.6076M/s |
| double | fast_inverse_sqrt | 9.19 ns | 108.77M/s |
| double | bit_estimate_newton | 13.9 ns | 71.8849M/s |
| double | digit_by_digit | 111 ns | 9.00597M/s |

See `docs/algorithms.md` and `docs/benchmarking.md` for method notes and the
validation methodology.
