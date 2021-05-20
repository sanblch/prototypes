set(CONAN "conan" CACHE STRING "Conan executable")
set(CONAN_PROFILE "default" CACHE STRING "Conan profile")

include(buildtype)
include(conan)

conan_cmake_run(
  PROFILE ${CONAN_PROFILE}
  SETTINGS build_type=${CMAKE_BUILD_TYPE}
  REQUIRES
    boost/1.72.0
    prometheus-cpp/0.12.1
    spdlog/1.8.5
  OPTIONS
    prometheus-cpp:with_pull=False
    prometheus-cpp:with_push=False
    prometheus-cpp:with_compression=False
  GENERATORS cmake_find_package_multi
  CONAN_COMMAND ${CONAN})

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_define_targets()
