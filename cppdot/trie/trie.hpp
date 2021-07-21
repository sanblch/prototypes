#ifndef TRIE_HPP
#define TRIE_HPP

#include <map>

#include "common/buffer.hpp"

namespace trie {

class Trie {
  std::map<kagome::common::Buffer, kagome::common::Buffer> data_;

public:
  void put(const kagome::common::Buffer &key,
           const kagome::common::Buffer &value) {
    data_[key] = value;
  }
};

} // namespace trie

#endif // TRIE_HPP
