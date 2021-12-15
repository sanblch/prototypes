enable_testing()
include(GoogleTest)

add_executable(trie-test trie.cpp)
target_link_libraries(trie-test cppdot GTest::gtest)
gtest_discover_tests(trie-test)

add_executable(wasm-test
  wasm.cpp
  wasm/wasm.h
  )
target_include_directories(wasm-test PUBLIC
  ${CMAKE_SOURCE_DIR}/binaryen/src)
target_link_libraries(wasm-test
  binaryen
  cppdot
  GTest::gtest
  xxHash::xxHash)
gtest_discover_tests(wasm-test)

add_executable(wasmedge-test
  wasmedge.cpp
  )
target_link_libraries(wasmedge-test
  cppdot
  GTest::gtest
  WasmEdge::WasmEdge
  xxHash::xxHash)
gtest_discover_tests(wasmedge-test)

add_executable(wasmedge-exe
  test_wasmedge.cpp
  )

target_include_directories(wasmedge-exe PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(wasmedge-exe
  cppdot
  spdlog::spdlog
  WasmEdge::WasmEdge)

add_executable(wasmedge-comp-test
  test_wasmedge_comp.cpp
  )

target_link_libraries(wasmedge-comp-test
  WasmEdge::WasmEdge)


add_executable(wasmedge-host-test
  test_wasmedge_host.cpp
  )

target_link_libraries(wasmedge-host-test
  WasmEdge::WasmEdge)

add_executable(fast_test test.cpp)
