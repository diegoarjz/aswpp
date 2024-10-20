find_package(GTest REQUIRED)

add_executable(aswpp_unit_tests
  script_engine.test.cpp
  script_module.test.cpp
)

target_link_libraries(
  aswpp_unit_tests
  PRIVATE
    GTest::gtest
    GTest::gtest_main
    aswpp
)

target_include_directories(
  aswpp_unit_tests
  PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

add_test(
  NAME
    aswpp_unit_tests
  COMMAND
    aswpp_unit_tests
)
