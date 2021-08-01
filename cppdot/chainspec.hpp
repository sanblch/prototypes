#ifndef CHAIN_SPEC_HPP
#define CHAIN_SPEC_HPP

#include <boost/property_tree/json_parser.hpp>
#include <gsl/span_ext>
#include <spdlog/spdlog.h>
#include <zstd.h>

#include "common/hexutil.hpp"
#include "common/outcome.hpp"
#include "trie/trie.hpp"

namespace chainspec {
enum class ChainSpecError;
}

OUTCOME_HPP_DECLARE_ERROR(chainspec, ChainSpecError)

namespace chainspec {

namespace pt = boost::property_tree;

enum class ChainSpecError {
  MISSING_ENTRY = 1,
  MISSING_PEER_ID,
  PARSER_ERROR,
  NOT_IMPLEMENTED
};

// constexpr uint8_t kZstdPrefix[8] = {82, 188, 83, 118, 70, 219, 142, 5};
constexpr uint8_t kZstdPrefix[8] = {0x52, 0xBC, 0x53, 0x76, 0x46, 0xDB, 0x8E, 0x05};
constexpr size_t kCodeBlobBombLimit = 50 * 1024 * 1024;

namespace {

template <typename T>
outcome::result<std::decay_t<T>> ensure(std::string_view entry_name,
                                        boost::optional<T> opt_entry) {
  if (not opt_entry) {
    spdlog::error("Required '{}' entry not found in the chain spec",
                  entry_name);
    return ChainSpecError::MISSING_ENTRY;
  }
  return opt_entry.value();
}

} // namespace

inline outcome::result<void> loadFrom(const std::string &path,
                                      trie::Trie &trie) {
  pt::ptree tree;
  try {
    pt::read_json(path, tree);
  }
  catch (pt::json_parser_error &e) {
    spdlog::error("Parser error: {}, line {}: {}", e.filename(), e.line(),
                  e.message());
    return ChainSpecError::PARSER_ERROR;
  }

  OUTCOME_TRY(genesis_tree,
              ensure("genesis", tree.get_child_optional("genesis")));
  OUTCOME_TRY(genesis_raw_tree,
              ensure("genesis/raw", genesis_tree.get_child_optional("raw")));
  boost::property_tree::ptree top_tree;
  // v0.7+ format
  if (auto top_tree_opt = genesis_raw_tree.get_child_optional("top");
      top_tree_opt.has_value()) {
    top_tree = top_tree_opt.value();
  }
  else {
    // Try to fall back to v0.6
    top_tree = genesis_raw_tree.begin()->second;
  }

  for (const auto &[key, value] : top_tree) {
    OUTCOME_TRY(key_processed, kagome::common::unhexWith0x(key));
    OUTCOME_TRY(value_processed, kagome::common::unhexWith0x(value.data()));
    if (kagome::common::Buffer(key_processed) ==
        kagome::common::Buffer().put(":code")) {
      spdlog::info(
          kagome::common::hex_lower(gsl::make_span(value_processed.data(), 8)));
      if (std::equal(value_processed.begin(), value_processed.begin() + 8,
                     std::begin(kZstdPrefix))) {
        std::vector<uint8_t> rBuff;
        rBuff.resize(kCodeBlobBombLimit);
        auto dSize = ZSTD_decompress(rBuff.data(), kCodeBlobBombLimit,
                                     value_processed.data() + 8,
                                     value_processed.size() - 8);
        rBuff.resize(dSize);
        trie.put(kagome::common::Buffer(key_processed), kagome::common::Buffer(rBuff));
        spdlog::info("Genesis decompressed data placed to trie.");
        return outcome::success();
      }
    }
    trie.put(kagome::common::Buffer(key_processed),
             kagome::common::Buffer(value_processed));
  }
  spdlog::info("Genesis data placed to trie.");

  kagome::common::Buffer babe({'b', 'a', 'b', 'e'});
  kagome::common::Buffer res(ZSTD_compressBound(babe.size()), 0);
  auto cSize = ZSTD_compress(res.data(), res.size(), babe.data(), babe.size(), 1);
  res.resize(cSize);
  spdlog::info(res.toHex());

  return outcome::success();
}

} // namespace chainspec

#endif // CHAIN_SPEC_HPP
