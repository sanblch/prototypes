cmake_minimum_required(VERSION 3.17)

project(prototype-grpc)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
list(APPEND CONAN_PACKAGES
  grpc/1.34.1@gm/thirdparty
  gtest/1.8.1)

enable_testing()
include(GoogleTest)
# add conan packages and apply find_package
include(packages)
include(grpc)

protobuf_generate_grpc_cpp("service/src/main/proto/plotter.proto")

configure_file(server.crt ${CMAKE_BINARY_DIR}/server.crt COPYONLY)
configure_file(server.key ${CMAKE_BINARY_DIR}/server.key COPYONLY)

add_executable(GrpcTestClient
  GrpcTestClient.cxx)
target_link_directories(GrpcTestClient PUBLIC
  ${CONAN_LIB_DIRS_PROTOBUF})
target_link_libraries(GrpcTestClient PUBLIC
  CONAN_PKG::gtest
  plotter_proto_lib)

add_executable(GrpcTestServer
  GrpcTestServer.cxx)
target_link_directories(GrpcTestServer PUBLIC
  ${CONAN_LIB_DIRS_PROTOBUF})
target_link_libraries(GrpcTestServer PUBLIC
  CONAN_PKG::gtest
  plotter_proto_lib)

gtest_discover_tests(GrpcTestClient)
