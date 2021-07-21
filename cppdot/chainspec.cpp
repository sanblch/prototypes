#include "chainspec.hpp"

OUTCOME_CPP_DEFINE_CATEGORY(chainspec, ChainSpecError, e) {
  using E = chainspec::ChainSpecError;
  switch (e) {
  case E::MISSING_ENTRY:
    return "A required entry is missing in the config file";
  case E::MISSING_PEER_ID:
    return "Peer id is missing in a multiaddress provided in the config file";
  case E::PARSER_ERROR:
    return "Internal parser error";
  case E::NOT_IMPLEMENTED:
    return "Known entry name, but parsing not implemented";
  }
  return "Unknown error in ChainSpec";
}
