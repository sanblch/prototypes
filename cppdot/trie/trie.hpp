#ifndef TRIE_HPP
#define TRIE_HPP

#include <map>

#include <spdlog/spdlog.h>

#include "common/buffer.hpp"

namespace trie {

class Trie {
  std::map<kagome::common::Buffer, kagome::common::Buffer> data_;

public:
  void put(const kagome::common::Buffer &key,
           const kagome::common::Buffer &value) {
    data_[key] = value;
    // spdlog::info("{}", key.toHex());
  }

  kagome::common::Buffer* get(const kagome::common::Buffer& key) {
    if(auto it = data_.find(key); it != data_.end()) {
      return &it->second;
    }
    spdlog::info("Not found");
    return nullptr;
  }
};

} // namespace trie

#endif // TRIE_HPP
