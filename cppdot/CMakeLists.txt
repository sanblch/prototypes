cmake_minimum_required(VERSION 3.18)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

project(cppdot)

include(packages)

find_package(RocksDB CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_executable(cppdot
  main.cpp
  )

target_link_libraries(cppdot
  RocksDB::RocksDB
  spdlog::spdlog)
