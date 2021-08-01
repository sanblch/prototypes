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
