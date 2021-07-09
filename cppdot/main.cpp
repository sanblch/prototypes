#include <cstdlib>
#include <iomanip>
#include <rocksdb/db.h>
#include <spdlog/spdlog.h>

namespace meta_keys {
const char *TYPE = "type";
const char *BEST_BLOCK = "best";
const char *FINALIZED_BLOCK = "Final";
const char *FINALIZED_STATE = "fstate";
const char *CACHE_META_PREFIX = "cache";
const char *CHANGES_TRIES_META = "ctrie";
const char *GENESIS_HASH = "gen";
const char *LEAF_PREFIX = "leaf";
const char *CHILDREN_PREFIX = "children";
} // namespace meta_keys

enum class Columns : uint32_t {
  META,
  STATE,
  STATE_META,
  KEY_LOOKUP,
  HEADER,
  BODY,
  JUSTIFICATIONS,
  CHANGES_TRIE,
  AUX,
  OFFCHAIN,
  CACHE,
  TRANSACTIONS
};

std::string hex(const std::string &str) {
  std::stringstream res;
  res << std::hex << std::setfill('0');
  for (std::string::size_type i = 0; i < str.length(); ++i) {
    res << std::setw(2) << static_cast<unsigned>(static_cast<uint8_t>(str[i]));
  }
  return res.str();
}

int main() {
  rocksdb::DB *db;
  rocksdb::Options options;
  options.max_open_files = 100;
  std::vector<std::string> column_names;
  rocksdb::Status status =
      rocksdb::DB::ListColumnFamilies(options, "db", &column_names);
  if (!status.ok()) {
    spdlog::error("DB error: {}", status.ToString());
  }
  std::vector<rocksdb::ColumnFamilyDescriptor> column_families;
  for (const auto &col : column_names) {
    column_families.push_back(
        rocksdb::ColumnFamilyDescriptor(col, rocksdb::ColumnFamilyOptions()));
  }
  std::vector<rocksdb::ColumnFamilyHandle *> column_handles;
  status =
      rocksdb::DB::Open(options, "db", column_families, &column_handles, &db);
  if (!status.ok()) {
    spdlog::error("DB error: {}", status.ToString());
  }
  uint64_t val;
  spdlog::info("{}",
               db->GetIntProperty(
                   column_handles[static_cast<uint32_t>(Columns::KEY_LOOKUP)],
                   rocksdb::Slice("rocksdb.estimate-num-keys"), &val));
  spdlog::info("{}", val);
  rocksdb::ReadOptions roptions;
  roptions.verify_checksums = false;
  auto it = db->NewIterator(
      roptions, column_handles[static_cast<uint32_t>(Columns::HEADER)]);
  unsigned count = 0;
  for (it->SeekToFirst(); it->Valid(), count < 10; it->Next(), count++) {
    spdlog::info("{}",
                 hex(std::string(it->key().data(), it->key().size())));
    spdlog::info("{}", hex(std::string(it->value().data(), it->value().size())));
    spdlog::info("{}", it->value().size());
  }
  std::string value;
  spdlog::info("{}", db->KeyMayExist(
                         roptions,
                         column_handles[static_cast<uint32_t>(Columns::HEADER)],
                         rocksdb::Slice("0000000000000000"), &value));
  // std::string* value = nullptr;
  // status = db->Get(rocksdb::ReadOptions(),
  //         column_handles[static_cast<uint32_t>(Columns::META)],
  //         rocksdb::Slice(meta_keys::GENESIS_HASH), value);
  // if (!status.ok()) {
  //   spdlog::error("DB error: {}", status.ToString());
  // }
  spdlog::info("Key: {}, size: {}", value, value.size());
  delete it;
  for (auto c : column_handles)
    delete c;
  delete db;
  return EXIT_SUCCESS;
}
