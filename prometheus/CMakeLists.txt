cmake_minimum_required(VERSION 3.18)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

set(PACKAGER HUNTER CACHE STRING "Package manager")
set_property(CACHE PACKAGER PROPERTY STRINGS CONAN HUNTER)

if(PACKAGER STREQUAL "HUNTER")
  include(HunterGate)
  HunterGate(
    URL  "https://github.com/soramitsu/soramitsu-hunter/archive/tags/v0.23.257-soramitsu7.tar.gz"
    SHA1 "24f3cae2b6b3212b87daf643cdaad0f9afc1b54b"
    LOCAL
    )
endif()

project(prometheus)

if(PACKAGER STREQUAL "CONAN")
  include(packages)
endif()
if(PACKAGER STREQUAL "HUNTER")
  hunter_add_package(Boost)
  hunter_add_package(prometheus-cpp)
  hunter_add_package(spdlog)
endif()

find_package(Boost CONFIG REQUIRED)
find_package(prometheus-cpp CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_executable(prometheus
  httplistener.cpp
  httpsession.cpp
  metricshandler.cpp
  prometheus.cpp
  )

target_link_libraries(prometheus
  Boost::boost
  prometheus-cpp::core
  spdlog::spdlog)
