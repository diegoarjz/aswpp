cmake_minimum_required(VERSION 3.11.0 FATAL_ERROR)
project("aswpp" LANGUAGES CXX VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

option("WITH_COVERAGE" "Build with coverage information" OFF)

if (WITH_COVERAGE)
  SET(CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage --coverage")
  SET(CMAKE_C_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage --coverage")
endif()

find_package(magic_enum REQUIRED)

add_subdirectory(third_party)
add_subdirectory(src)

enable_testing()
add_subdirectory(unit_tests)

add_subdirectory(examples)
