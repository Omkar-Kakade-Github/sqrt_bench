#include <sqrt_bench/sqrt_bench.hpp>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <limits>

namespace {

template <typename T>
void print_report_for_type(std::string_view type_name) {
  const auto inputs = sqrt_bench::validation_inputs<T>();

  std::cout << "\n" << type_name << "\n";
  std::cout << std::left << std::setw(24) << "method" << std::right
            << std::setw(16) << "max abs" << std::setw(16) << "max rel"
            << std::setw(12) << "max ulp" << std::setw(8) << "ok" << "\n";

  for (const auto method : sqrt_bench::kAllMethods) {
    T max_abs = T{0};
    T max_rel = T{0};
    std::uint64_t max_ulp = 0;
    bool all_ok = true;

    for (const T input : inputs) {
      const auto metrics = sqrt_bench::evaluate(method, input);
      max_abs = std::max(max_abs, metrics.absolute);
      max_rel = std::max(max_rel, metrics.relative);
      max_ulp = std::max(max_ulp, metrics.ulp);
      all_ok = all_ok && metrics.ok;
    }

    std::cout << std::left << std::setw(24) << sqrt_bench::method_id(method)
              << std::right << std::setw(16) << max_abs << std::setw(16)
              << max_rel << std::setw(12) << max_ulp << std::setw(8)
              << (all_ok ? "yes" : "no") << "\n";
  }
}

} // namespace

int main() {
  std::cout << "sqrt_bench " << sqrt_bench::version() << "\n";
  std::cout << "Methods:\n";
  for (const auto &info : sqrt_bench::kMethodInfos) {
    std::cout << "  " << std::setw(20) << std::left << info.id << " "
              << info.name << "\n";
  }

  std::cout << std::scientific << std::setprecision(6);
  print_report_for_type<float>("float validation summary");
  print_report_for_type<double>("double validation summary");

  std::cout << "\nBenchmark command:\n";
  std::cout << "  cmake --build build --target sqrt_benchmarks\n";
  std::cout << "  ./build/sqrt_benchmarks\n";
  return 0;
}
