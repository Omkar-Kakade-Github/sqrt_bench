#pragma once

#include <sqrt_bench/algorithms.hpp>

#include <cmath>
#include <limits>
#include <random>
#include <vector>

namespace sqrt_bench {

template <SupportedFloat T>
std::vector<T> validation_inputs() {
  std::vector<T> values{
      std::numeric_limits<T>::denorm_min(),
      std::numeric_limits<T>::min(),
      std::numeric_limits<T>::epsilon(),
      T{0.000001},
      T{0.01},
      T{0.25},
      T{0.5},
      T{0.999999},
      T{1},
      T{1.000001},
      T{2},
      T{10},
      T{12345.6789},
      T{1000000},
      std::numeric_limits<T>::max() / T{4},
  };

  for (int exponent = -12; exponent <= 12; ++exponent) {
    values.push_back(std::ldexp(T{1}, exponent));
  }

  std::mt19937_64 rng{0x51C0FFEEULL};
  const T min_exp = std::is_same_v<T, float> ? T{-30} : T{-300};
  const T max_exp = std::is_same_v<T, float> ? T{30} : T{300};
  std::uniform_real_distribution<T> distribution{min_exp, max_exp};
  for (int i = 0; i < 128; ++i) {
    values.push_back(std::exp2(distribution(rng)));
  }

  return values;
}

template <SupportedFloat T>
std::vector<T> benchmark_inputs() {
  std::vector<T> values;
  values.reserve(4096);

  std::mt19937_64 rng{0xBEEFCACEULL};
  std::uniform_real_distribution<T> mantissa{T{0.5}, T{2}};
  std::uniform_int_distribution<int> exponent{
      std::is_same_v<T, float> ? -60 : -600,
      std::is_same_v<T, float> ? 60 : 600,
  };

  for (int i = 0; i < 4096; ++i) {
    values.push_back(std::ldexp(mantissa(rng), exponent(rng)));
  }

  return values;
}

} // namespace sqrt_bench
