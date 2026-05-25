#pragma once

#include <array>
#include <bit>
#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string_view>
#include <type_traits>

namespace sqrt_bench {

enum class Method {
  StdSqrt,
  PowHalf,
  ExpLog,
  NewtonRaphson,
  Bisection,
  Halley,
  Goldschmidt,
  FastInverseSqrt,
  BitEstimateNewton,
  DigitByDigit,
};

struct MethodInfo {
  Method method;
  std::string_view id;
  std::string_view name;
};

inline constexpr std::array<MethodInfo, 10> kMethodInfos{{
    {Method::StdSqrt, "std_sqrt", "std::sqrt baseline"},
    {Method::PowHalf, "pow_half", "std::pow(x, 0.5)"},
    {Method::ExpLog, "exp_log", "exp(log(x) / 2)"},
    {Method::NewtonRaphson, "newton_raphson", "Newton-Raphson / Heron"},
    {Method::Bisection, "bisection", "Bisection search"},
    {Method::Halley, "halley", "Halley iteration"},
    {Method::Goldschmidt, "goldschmidt", "Goldschmidt inverse sqrt"},
    {Method::FastInverseSqrt, "fast_inverse_sqrt", "Fast inverse sqrt"},
    {Method::BitEstimateNewton, "bit_estimate_newton", "Bit estimate + Newton"},
    {Method::DigitByDigit, "digit_by_digit", "Binary digit-by-digit"},
}};

inline constexpr std::array<Method, kMethodInfos.size()> kAllMethods{{
    Method::StdSqrt,
    Method::PowHalf,
    Method::ExpLog,
    Method::NewtonRaphson,
    Method::Bisection,
    Method::Halley,
    Method::Goldschmidt,
    Method::FastInverseSqrt,
    Method::BitEstimateNewton,
    Method::DigitByDigit,
}};

constexpr std::string_view method_id(Method method) noexcept {
  for (const auto &info : kMethodInfos) {
    if (info.method == method) {
      return info.id;
    }
  }
  return "unknown";
}

constexpr std::string_view method_name(Method method) noexcept {
  for (const auto &info : kMethodInfos) {
    if (info.method == method) {
      return info.name;
    }
  }
  return "Unknown method";
}

template <typename T>
concept SupportedFloat =
    std::is_same_v<T, float> || std::is_same_v<T, double>;

template <SupportedFloat T>
constexpr bool is_positive_finite(T x) noexcept {
  return x > T{0} && std::isfinite(x);
}

namespace detail {

template <SupportedFloat T>
constexpr int accurate_iterations() noexcept {
  return std::is_same_v<T, float> ? 7 : 10;
}

template <SupportedFloat T, typename Fn>
T scale_to_unit_interval(T x, Fn &&fn) {
  int exponent = 0;
  T mantissa = std::frexp(x, &exponent);
  if ((exponent & 1) != 0) {
    mantissa *= T{2};
    --exponent;
  }
  return std::ldexp(fn(mantissa), exponent / 2);
}

template <SupportedFloat T>
T newton_unit_from(T x, T initial, int iterations) {
  T y = initial;
  for (int i = 0; i < iterations; ++i) {
    const T next = (y + x / y) / T{2};
    if (next == y) {
      break;
    }
    y = next;
  }
  return y;
}

template <SupportedFloat T>
T newton_unit(T x, int iterations = accurate_iterations<T>()) {
  const T initial = x >= T{1} ? x : T{1};
  return newton_unit_from(x, initial, iterations);
}

template <SupportedFloat T>
T halley_unit(T x) {
  T y = x >= T{1} ? x : T{1};
  constexpr int iterations = std::is_same_v<T, float> ? 5 : 7;
  for (int i = 0; i < iterations; ++i) {
    const T y2 = y * y;
    const T denominator = T{3} * y2 + x;
    if (denominator == T{0}) {
      break;
    }
    const T next = y * (y2 + T{3} * x) / denominator;
    if (next == y) {
      break;
    }
    y = next;
  }
  return y;
}

template <SupportedFloat T>
T goldschmidt_unit(T x) {
  T y = T{1};
  constexpr int iterations = std::is_same_v<T, float> ? 6 : 8;
  for (int i = 0; i < iterations; ++i) {
    const T b = x * y * y;
    y *= (T{3} - b) / T{2};
  }
  return x * y;
}

template <SupportedFloat T>
T bisection_unit(T x) {
  T low = T{0};
  T high = x >= T{1} ? x : T{1};
  constexpr int iterations = std::is_same_v<T, float> ? 32 : 64;
  for (int i = 0; i < iterations; ++i) {
    const T mid = (low + high) / T{2};
    if (mid * mid <= x) {
      low = mid;
    } else {
      high = mid;
    }
  }
  return (low + high) / T{2};
}

template <SupportedFloat T>
T fast_inverse_sqrt_unit(T x) {
  if constexpr (std::is_same_v<T, float>) {
    const float xhalf = 0.5F * x;
    std::uint32_t bits = std::bit_cast<std::uint32_t>(x);
    bits = 0x5f3759dfU - (bits >> 1U);
    float y = std::bit_cast<float>(bits);
    for (int i = 0; i < 3; ++i) {
      y *= 1.5F - xhalf * y * y;
    }
    return x * y;
  } else {
    const double xhalf = 0.5 * x;
    std::uint64_t bits = std::bit_cast<std::uint64_t>(x);
    bits = 0x5fe6eb50c7b537a9ULL - (bits >> 1U);
    double y = std::bit_cast<double>(bits);
    for (int i = 0; i < 4; ++i) {
      y *= 1.5 - xhalf * y * y;
    }
    return x * y;
  }
}

template <SupportedFloat T>
T bit_estimate_unit(T x) {
  if constexpr (std::is_same_v<T, float>) {
    std::uint32_t bits = std::bit_cast<std::uint32_t>(x);
    bits = (bits >> 1U) + 0x1fc00000U;
    return std::bit_cast<float>(bits);
  } else {
    std::uint64_t bits = std::bit_cast<std::uint64_t>(x);
    bits = (bits >> 1U) + 0x1ff8000000000000ULL;
    return std::bit_cast<double>(bits);
  }
}

template <SupportedFloat T>
T digit_by_digit_unit(T x) {
  T result = T{0};
  T bit = T{1};
  constexpr int iterations = std::numeric_limits<T>::digits + 2;
  for (int i = 0; i < iterations; ++i) {
    const T candidate = result + bit;
    if (candidate * candidate <= x) {
      result = candidate;
    }
    bit /= T{2};
  }
  return result;
}

} // namespace detail

namespace algorithms {

template <SupportedFloat T>
T std_sqrt(T x) {
  return std::sqrt(x);
}

template <SupportedFloat T>
T pow_half(T x) {
  return std::pow(x, T{0.5});
}

template <SupportedFloat T>
T exp_log(T x) {
  return std::exp(std::log(x) / T{2});
}

template <SupportedFloat T>
T newton_raphson(T x) {
  return detail::scale_to_unit_interval(
      x, [](T y) { return detail::newton_unit(y); });
}

template <SupportedFloat T>
T bisection(T x) {
  return detail::scale_to_unit_interval(
      x, [](T y) { return detail::bisection_unit(y); });
}

template <SupportedFloat T>
T halley(T x) {
  return detail::scale_to_unit_interval(
      x, [](T y) { return detail::halley_unit(y); });
}

template <SupportedFloat T>
T goldschmidt(T x) {
  return detail::scale_to_unit_interval(
      x, [](T y) { return detail::goldschmidt_unit(y); });
}

template <SupportedFloat T>
T fast_inverse_sqrt(T x) {
  return detail::scale_to_unit_interval(
      x, [](T y) { return detail::fast_inverse_sqrt_unit(y); });
}

template <SupportedFloat T>
T bit_estimate_newton(T x) {
  return detail::scale_to_unit_interval(x, [](T y) {
    T estimate = detail::bit_estimate_unit(y);
    return detail::newton_unit_from(y, estimate,
                                    std::is_same_v<T, float> ? 4 : 6);
  });
}

template <SupportedFloat T>
T digit_by_digit(T x) {
  return detail::scale_to_unit_interval(
      x, [](T y) { return detail::digit_by_digit_unit(y); });
}

} // namespace algorithms

template <SupportedFloat T>
T sqrt_method(Method method, T x) {
  if (!is_positive_finite(x)) {
    throw std::domain_error{"sqrt_bench methods require positive finite input"};
  }

  switch (method) {
  case Method::StdSqrt:
    return algorithms::std_sqrt(x);
  case Method::PowHalf:
    return algorithms::pow_half(x);
  case Method::ExpLog:
    return algorithms::exp_log(x);
  case Method::NewtonRaphson:
    return algorithms::newton_raphson(x);
  case Method::Bisection:
    return algorithms::bisection(x);
  case Method::Halley:
    return algorithms::halley(x);
  case Method::Goldschmidt:
    return algorithms::goldschmidt(x);
  case Method::FastInverseSqrt:
    return algorithms::fast_inverse_sqrt(x);
  case Method::BitEstimateNewton:
    return algorithms::bit_estimate_newton(x);
  case Method::DigitByDigit:
    return algorithms::digit_by_digit(x);
  }

  throw std::invalid_argument{"unknown sqrt_bench method"};
}

} // namespace sqrt_bench
