cmake_minimum_required(VERSION 3.11.0 FATAL_ERROR)
project("aswpp" LANGUAGES CXX VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)

set(AngelScript_VERSION "2.37.0" CACHE STRING "AngelScript version to use" )

add_subdirectory(third_party)
add_subdirectory(src)
