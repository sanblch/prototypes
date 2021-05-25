cmake_minimum_required(VERSION 3.18)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

include(HunterGate)
HunterGate(
  URL  "https://github.com/soramitsu/soramitsu-hunter/archive/tags/v0.23.257-soramitsu7.tar.gz"
  SHA1 "24f3cae2b6b3212b87daf643cdaad0f9afc1b54b"
  LOCAL
  )

project(use_soralog)

hunter_add_package(soralog)

find_package(soralog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

message(STATUS "Found soralog: ${soralog_FOUND} ${soralog_LIBRARIES}")

add_executable(use_soralog
  main.cpp)
target_link_libraries(use_soralog
  soralog::soralog)
