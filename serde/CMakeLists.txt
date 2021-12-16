cmake_minimum_required(VERSION 3.15.0)

set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

project(serde)

find_package(GTest REQUIRED)

add_executable(serde-test
  serde-test.cpp)

target_link_libraries(serde-test
  GTest::gtest
  GTest::gtest_main)


