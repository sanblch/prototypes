cmake_minimum_required(VERSION 3.18)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

project(prometheus)

include(packages)

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
  prometheus-cpp::prometheus-cpp
  spdlog::spdlog)
