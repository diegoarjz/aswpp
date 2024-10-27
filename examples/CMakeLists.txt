add_executable(simple_example simple_example.cpp)
target_link_libraries(simple_example PRIVATE aswpp)
target_include_directories(simple_example PRIVATE ${CMAKE_SOURCE_DIR}/include)
