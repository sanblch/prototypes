find_program(_PROTOBUF_PROTOC protoc ${CONAN_BIN_DIRS_PROTOBUF} NO_DEFAULT_PATH)

if(WIN32)
  set(ZLIB_ROOT ${CONAN_ZLIB_ROOT})
endif()

# Find gRPC installation
include(${CONAN_GRPC_ROOT}/lib/cmake/grpc/gRPCConfig.cmake)
message(STATUS "Using gRPC ${gRPC_VERSION}")

set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)

function(protobuf_generate_grpc_cpp proto_file)
  # Proto file
  get_filename_component(proto_name ${proto_file} NAME_WLE)
  get_filename_component(proto "${proto_file}" ABSOLUTE)
  get_filename_component(proto_path "${proto}" PATH)

  # Generated sources
  set(proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.pb.cc")
  set(proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.pb.h")
  set(grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.grpc.pb.cc")
  set(grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/${proto_name}.grpc.pb.h")
  add_custom_command(
    OUTPUT "${proto_srcs}" "${proto_hdrs}" "${grpc_srcs}" "${grpc_hdrs}"
    COMMAND ${_PROTOBUF_PROTOC}
    ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
    --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
    -I "${proto_path}"
    --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
    "${proto}"
    DEPENDS "${proto}")

  # Build generated sources as an object library
  add_library(${proto_name}_proto_lib STATIC ${proto_srcs} ${grpc_srcs})

  # Include generated *.pb.h files
  target_include_directories(${proto_name}_proto_lib PUBLIC
    "${CONAN_INCLUDE_DIRS_PROTOBUF}"
    "${CMAKE_CURRENT_BINARY_DIR}")

  if(WIN32)
    target_compile_definitions(${proto_name}_proto_lib PUBLIC
      _WIN32_WINNT=0x600)
  endif()
  set_target_properties(${proto_name}_proto_lib PROPERTIES
    POSITION_INDEPENDENT_CODE ON)
  target_link_libraries(${proto_name}_proto_lib
    gRPC::grpc++
    gRPC::grpc++_reflection)
endfunction()
