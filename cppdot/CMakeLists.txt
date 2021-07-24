cmake_minimum_required(VERSION 3.18)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

project(cppdot)

include(packages)

find_package(Boost CONFIG REQUIRED)
find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(RocksDB CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(zstd CONFIG REQUIRED)

add_executable(cppdot
  chainspec.cpp
  chainspec.hpp
  common/blob.cpp
  common/blob.hpp
  common/buffer.cpp
  common/buffer.hpp
  common/hexutil.cpp
  common/hexutil.hpp
  common/outcome.hpp
  common/outcome-register.hpp
  common/outcome_throw.hpp
  common/unused.hpp
  macro/unreachable.hpp
  main.cpp
  primitives/block_header.hpp
  primitives/common.hpp
  primitives/compact_integer.hpp
  primitives/digest.hpp
  scale/compact_len_utils.hpp
  scale/detail/fixed_width_integer.hpp
  scale/scale.hpp
  scale/scale_decoder_stream.cpp
  scale/scale_decoder_stream.hpp
  scale/scale_encoder_stream.cpp
  scale/scale_encoder_stream.hpp
  scale/scale_error.cpp
  scale/scale_error.hpp
  scale/types.hpp
  trie/trie.hpp
  )

target_include_directories(cppdot PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(cppdot
  Boost::Boost
  Microsoft.GSL::Microsoft.GSL
  RocksDB::RocksDB
  spdlog::spdlog
  zstd::zstd)
