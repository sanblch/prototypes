set(CONAN "conan" CACHE STRING "Conan executable")
set(CONAN_PROFILE "default" CACHE STRING "Conan profile")

include(buildtype)
include(conan)

conan_cmake_run(
  PROFILE ${CONAN_PROFILE}
  SETTINGS build_type=${CMAKE_BUILD_TYPE}
  REQUIRES
    boost/1.76.0
    gtest/1.11.0
    ms-gsl/3.1.0
    rocksdb/6.20.3
    spdlog/1.8.5
    WasmEdge/0.8.1@sanblch/stable
    xxhash/0.8.0
    zstd/1.5.0
  OPTIONS
    rocksdb:with_snappy=True
  GENERATORS cmake_find_package_multi
  CONAN_COMMAND ${CONAN})

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_define_targets()
