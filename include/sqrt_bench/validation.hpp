#pragma once

#include <sqrt_bench/algorithms.hpp>

#include <bit>
#include <cmath>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace sqrt_bench {

template <SupportedFloat T>
struct Tolerance {
  T absolute;
  T relative;
  std::uint64_t ulp;
};

template <SupportedFloat T>
struct ErrorMetrics {
  T input{};
  T expected{};
  T actual{};
  T absolute{};
  T relative{};
  std::uint64_t ulp{};
  bool ok{};
};

namespace detail {

template <SupportedFloat T>
std::uint64_t positive_float_bits(T value) {
  if constexpr (std::is_same_v<T, float>) {
    return std::bit_cast<std::uint32_t>(value);
  } else {
    return std::bit_cast<std::uint64_t>(value);
  }
}

} // namespace detail

template <SupportedFloat T>
std::uint64_t ulp_distance(T lhs, T rhs) {
  const auto left = detail::positive_float_bits(lhs);
  const auto right = detail::positive_float_bits(rhs);
  return left > right ? left - right : right - left;
}

template <SupportedFloat T>
Tolerance<T> tolerance_for(Method method) {
  if constexpr (std::is_same_v<T, float>) {
    switch (method) {
    case Method::FastInverseSqrt:
      return {T{1e-5F}, T{1e-5F}, 128};
    case Method::ExpLog:
    case Method::PowHalf:
      return {T{2e-6F}, T{2e-6F}, 64};
    default:
      return {T{1e-6F}, T{1e-6F}, 32};
    }
  } else {
    switch (method) {
    case Method::FastInverseSqrt:
      return {T{1e-11}, T{1e-11}, 65536};
    case Method::ExpLog:
    case Method::PowHalf:
      return {T{1e-12}, T{1e-12}, 4096};
    default:
      return {T{1e-13}, T{1e-13}, 1024};
    }
  }
}

template <SupportedFloat T>
bool within_tolerance(const ErrorMetrics<T> &metrics,
                      const Tolerance<T> &tolerance) {
  return metrics.absolute <= tolerance.absolute ||
         metrics.relative <= tolerance.relative || metrics.ulp <= tolerance.ulp;
}

template <SupportedFloat T>
ErrorMetrics<T> evaluate(Method method, T input) {
  const T expected = std::sqrt(input);
  const T actual = sqrt_method(method, input);
  const T absolute = std::abs(actual - expected);
  const T denominator = expected == T{0} ? T{1} : expected;
  const T relative = absolute / denominator;
  ErrorMetrics<T> metrics{
      input,
      expected,
      actual,
      absolute,
      relative,
      std::isfinite(actual) ? ulp_distance(actual, expected)
                            : std::numeric_limits<std::uint64_t>::max(),
      false,
  };
  metrics.ok = within_tolerance(metrics, tolerance_for<T>(method));
  return metrics;
}

} // namespace sqrt_bench
