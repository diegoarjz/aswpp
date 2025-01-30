cmake_minimum_required(VERSION 3.11.0 FATAL_ERROR)
project("aswpp" LANGUAGES CXX VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

option("WITH_COVERAGE" "Build with coverage information" OFF)
set(Angelscript_PATH
  "${CMAKE_SOURCE_DIR}/build/AngelScript/"
  CACHE STRING "Path to Angelscript source"
)
set(Angelscript_VERSION
  "2.37.0"
  CACHE STRING "Angelscript version to use")

add_subdirectory(third_party)

if (WITH_COVERAGE)
  SET(CMAKE_CXX_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage --coverage")
  SET(CMAKE_C_FLAGS "-g -O0 -fprofile-arcs -ftest-coverage --coverage")
endif()

set(Angelscript_DIR
  "${Angelscript_PATH}/build/install/lib/cmake/Angelscript")


find_package(magic_enum REQUIRED)
find_package(Threads REQUIRED)
find_package(Angelscript REQUIRED)

add_subdirectory(src)

enable_testing()
add_subdirectory(unit_tests)

add_subdirectory(examples)
