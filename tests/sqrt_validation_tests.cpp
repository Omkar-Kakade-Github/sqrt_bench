#include <sqrt_bench/sqrt_bench.hpp>

#include <catch2/catch_test_macros.hpp>

#include <cmath>
#include <limits>

namespace {

template <typename T>
void validate_all_methods_for_type() {
  const auto values = sqrt_bench::validation_inputs<T>();

  for (const auto method : sqrt_bench::kAllMethods) {
    for (const T value : values) {
      const auto metrics = sqrt_bench::evaluate(method, value);
      INFO("method=" << sqrt_bench::method_id(method));
      INFO("input=" << value);
      INFO("expected=" << metrics.expected);
      INFO("actual=" << metrics.actual);
      INFO("absolute=" << metrics.absolute);
      INFO("relative=" << metrics.relative);
      INFO("ulp=" << metrics.ulp);
      REQUIRE(std::isfinite(metrics.actual));
      REQUIRE(metrics.actual >= T{0});
      REQUIRE(metrics.ok);
    }
  }
}

template <typename T>
void reject_invalid_inputs_for_type() {
  for (const T value :
       {T{0}, T{-1}, std::numeric_limits<T>::infinity(),
        std::numeric_limits<T>::quiet_NaN()}) {
    for (const auto method : sqrt_bench::kAllMethods) {
      INFO("method=" << sqrt_bench::method_id(method));
      REQUIRE_THROWS_AS(sqrt_bench::sqrt_method(method, value),
                        std::domain_error);
    }
  }
}

} // namespace

TEST_CASE("float methods validate against std::sqrt", "[float]") {
  validate_all_methods_for_type<float>();
}

TEST_CASE("double methods validate against std::sqrt", "[double]") {
  validate_all_methods_for_type<double>();
}

TEST_CASE("float methods reject non-positive and non-finite inputs", "[float]") {
  reject_invalid_inputs_for_type<float>();
}

TEST_CASE("double methods reject non-positive and non-finite inputs", "[double]") {
  reject_invalid_inputs_for_type<double>();
}
