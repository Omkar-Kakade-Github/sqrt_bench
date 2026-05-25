cmake_minimum_required(VERSION 3.20)

project(
  sqrt_bench
  VERSION 0.1.0
  DESCRIPTION "Square-root algorithm validation and benchmarks"
  LANGUAGES CXX
)

include(FetchContent)
include(CTest)

option(SQRT_BENCH_BUILD_APPS "Build sqrt_bench command-line apps" ON)
option(SQRT_BENCH_BUILD_TESTS "Build sqrt_bench tests" ON)
option(SQRT_BENCH_BUILD_BENCHMARKS "Build sqrt_bench benchmarks" ON)

add_library(sqrt_bench src/sqrt_bench.cpp)
add_library(sqrt_bench::sqrt_bench ALIAS sqrt_bench)

target_compile_features(sqrt_bench PUBLIC cxx_std_20)
target_include_directories(
  sqrt_bench
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(MSVC)
  target_compile_options(sqrt_bench PRIVATE /W4 /permissive-)
else()
  target_compile_options(sqrt_bench PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(SQRT_BENCH_BUILD_APPS)
  add_executable(sqrt_bench_report apps/sqrt_bench_report.cpp)
  target_link_libraries(sqrt_bench_report PRIVATE sqrt_bench::sqrt_bench)
endif()

if(BUILD_TESTING AND SQRT_BENCH_BUILD_TESTS)
  find_package(Catch2 3 QUIET)
  if(NOT Catch2_FOUND)
    FetchContent_Declare(
      Catch2
      GIT_REPOSITORY https://github.com/catchorg/Catch2.git
      GIT_TAG v3.6.0
    )
    FetchContent_MakeAvailable(Catch2)
  endif()

  add_executable(sqrt_bench_tests tests/sqrt_validation_tests.cpp)
  target_link_libraries(
    sqrt_bench_tests
    PRIVATE
      sqrt_bench::sqrt_bench
      Catch2::Catch2WithMain
  )

  include(Catch)
  catch_discover_tests(sqrt_bench_tests)
endif()

if(SQRT_BENCH_BUILD_BENCHMARKS)
  find_package(benchmark QUIET)
  if(NOT benchmark_FOUND)
    FetchContent_Declare(
      benchmark
      GIT_REPOSITORY https://github.com/google/benchmark.git
      GIT_TAG v1.8.5
    )
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(benchmark)
  endif()

  add_executable(sqrt_benchmarks benchmarks/sqrt_benchmarks.cpp)
  target_link_libraries(
    sqrt_benchmarks
    PRIVATE
      sqrt_bench::sqrt_bench
      benchmark::benchmark_main
  )
endif()
