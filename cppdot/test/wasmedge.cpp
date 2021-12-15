#include <memory>

#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include "chainspec.hpp"

TEST(Wasm, Simple) {
  trie::Trie trie;
  chainspec::loadFrom("localchain-dev.json", trie);
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  spdlog::flush_on(spdlog::level::trace);
  spdlog::set_level(spdlog::level::trace);
  return RUN_ALL_TESTS();
}
