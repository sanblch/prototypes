#ifndef CHAIN_SPEC_HPP
#define CHAIN_SPEC_HPP

#include <boost/property_tree/json_parser.hpp>
#include <spdlog/spdlog.h>

#include "common/outcome.hpp"

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

inline outcome::result<void> loadFrom(const std::string &path) {
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
    spdlog::info("Key {} : value {}", key, value.data());
  }

  return outcome::success();
}

} // namespace chainspec

#endif // CHAIN_SPEC_HPP
