#include <bitset>
#include <cassert>

#include <gtest/gtest.h>

#include "common/buffer.hpp"

using nibble = std::bitset<4>;
using radix16key = std::vector<nibble>;

kagome::common::Buffer nibblesToBuf(const std::vector<nibble> &nibbles) {
  assert(nibbles.size() % 2 == 0);
  auto res = kagome::common::Buffer(nibbles.size() / 2, 0);
  for (size_t i = 0; i < nibbles.size(); i += 2) {
    uint8_t c{0x00};
    res[i / 2] = (c | nibbles[i].to_ulong()) << 4u | nibbles[i + 1].to_ulong();
  }
  return res;
}

std::vector<nibble> bufToNibbles(const kagome::common::Buffer &buf) {
  auto res = std::vector<nibble>(buf.size() * 2, 0x00);
  for (size_t i = 0; i < buf.size(); ++i) {
    res[2 * i] = buf[i] >> 4u;
    res[2 * i + 1] = buf[i] & 0xFu;
  }
  return res;
}

struct NibblesToBufPar : public ::testing::TestWithParam<
                             std::pair<radix16key, kagome::common::Buffer>> {};

struct BufToNibblesPar : public ::testing::TestWithParam<
                             std::pair<radix16key, kagome::common::Buffer>> {};

TEST_P(NibblesToBufPar, nibblesToBuf) {
  auto [nibbles, buf] = GetParam();
  auto actualBuf = nibblesToBuf(nibbles);
  ASSERT_EQ(buf, actualBuf);
}

TEST_P(BufToNibblesPar, bufToNibbles) {
  auto [nibbles, buf] = GetParam();
  auto actualNibbles = bufToNibbles(buf);
  ASSERT_EQ(nibbles, actualNibbles);
}

const std::vector<std::pair<radix16key, kagome::common::Buffer>>
    NIBBLES_TO_BUF = {
        {{0x0, 0x0}, {0x00}},
        {{0xF, 0xF}, {0xFF}},
        {{0x3, 0xA, 0x0, 0x5}, {0x3A, 0x05}},
        {{0xA, 0xA, 0xF, 0xF, 0x0, 0x1}, {0xAA, 0xFF, 0x01}},
        {{0xA, 0xA, 0xF, 0xF, 0x0, 0x1, 0xC, 0x2}, {0xAA, 0xFF, 0x01, 0xC2}},
        {{0xA, 0xA, 0xF, 0xF, 0x0, 0x1, 0xC, 0x0}, {0xAA, 0xFF, 0x01, 0xC0}},
};

INSTANTIATE_TEST_CASE_P(BufToNibblesPar, BufToNibblesPar,
                        ::testing::ValuesIn(NIBBLES_TO_BUF));
INSTANTIATE_TEST_CASE_P(NibblesToBufPar, NibblesToBufPar,
                        ::testing::ValuesIn(NIBBLES_TO_BUF));

struct Node {
  std::optional<kagome::common::Buffer> value;
  std::optional<std::map<radix16key, Node>> children;
};

class Trie {
  void put(const kagome::common::Buffer& key, const kagome::common::Buffer& value) {
    auto nkey = bufToNibbles(key);

  }

private:
  Node root_node_;
};

TEST(trie, simple) { ; }

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
