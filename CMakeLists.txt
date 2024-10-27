cmake_minimum_required(VERSION 3.11.0 FATAL_ERROR)
project("aswpp" LANGUAGES CXX VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(third_party)
add_subdirectory(src)

enable_testing()
add_subdirectory(unit_tests)

add_subdirectory(examples)
