#include <boost/algorithm/hex.hpp>
#include <cstdlib>
#include <iomanip>
#include <rocksdb/db.h>
#include <spdlog/spdlog.h>

#include "chainspec.hpp"
#include "common/buffer.hpp"
#include "primitives/block_header.hpp"
#include "scale/scale.hpp"

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

std::string unhex(const std::string_view &strv) {
    std::string res;
    res.reserve(strv.size() / 2);
    boost::algorithm::unhex(strv.begin(), strv.end(), std::back_inserter(res));
    return res;
}

class Database {
    friend class PolkadotDatabase;

    rocksdb::DB *db_;
    std::vector<rocksdb::ColumnFamilyHandle *> column_handles_;
public:
    Database(const std::string &name) {
        rocksdb::Options options;
        options.max_open_files = 100;
        std::vector<std::string> column_names;
        rocksdb::Status status =
                rocksdb::DB::ListColumnFamilies(options, name, &column_names);
        if (!status.ok()) {
            spdlog::error("DB error: {}", status.ToString());
        }
        std::vector<rocksdb::ColumnFamilyDescriptor> column_families;
        for (const auto &col : column_names) {
            column_families.push_back(
                    rocksdb::ColumnFamilyDescriptor(col, rocksdb::ColumnFamilyOptions()));
        }
        status =
                rocksdb::DB::Open(options, name, column_families, &column_handles_, &db_);
        if (!status.ok()) {
            spdlog::error("DB error: {}", status.ToString());
        }
    }

    ~Database() {
        for (auto c : column_handles_)
            delete c;
        delete db_;
    }

    uint64_t getInt(uint32_t column, const std::string &key) {
        uint64_t res;
        db_->GetIntProperty(column_handles_[column], rocksdb::Slice(key), &res);
        spdlog::info("{}[{}] = {}", column_handles_[column]->GetName(), key, res);
        return res;
    }

    void list(uint32_t column, uint32_t count) {
        rocksdb::ReadOptions roptions;
        roptions.verify_checksums = false;
        auto it = db_->NewIterator(
                roptions, column_handles_[column]);
        unsigned i = 0;
        for (it->SeekToFirst(); i < count; it->Next(), ++i) {
            if (it->Valid()) {
                spdlog::info("{}", hex(std::string(it->key().data(), it->key().size())));
                spdlog::info("{}",
                             hex(std::string(it->value().data(), it->value().size())));
                spdlog::info("{}", it->value().size());
            }
        }
        delete it;
    }
};

class PolkadotDatabase {
    Database db_;
public:
    PolkadotDatabase(const std::string &path) : db_(path) {}

    Database *db() {
        return &db_;
    }

    outcome::result<kagome::primitives::BlockHeader> getHeader(uint32_t num) {
        kagome::common::Buffer bnum;
        bnum.putUint32(num);
        rocksdb::Slice key = bnum.asString();
        rocksdb::ReadOptions roptions;
        roptions.verify_checksums = false;
        rocksdb::ReadOptions roptions2;
        auto it2 = db_.db_->NewIterator(
                roptions2, db_.column_handles_[static_cast<uint32_t>(Columns::HEADER)]);
        it2->Seek(key);
        std::string value;
        db_.db_->Get(roptions, db_.column_handles_[static_cast<uint32_t>(Columns::BODY)],
                     it2->value(), &value);
        kagome::common::Buffer buf;
        buf.put(value);
        spdlog::info("{}", hex(value));
        spdlog::info("{}",
                     hex(std::string(it2->key().data(), it2->key().size())));
        spdlog::info(
                "{}", hex(std::string(it2->value().data(), it2->value().size())));
        auto res = kagome::scale::decode<kagome::primitives::BlockHeader>(
                gsl::make_span(buf.data(), buf.size()));

        // rocksdb::Slice key2 = res.value().extrinsics_root.toString();

        std::string value2;
        db_.db_->Get(roptions, db_.column_handles_[static_cast<uint32_t>(Columns::KEY_LOOKUP)],
                     it2->value(), &value2);

        kagome::common::Buffer buf2;
        buf2.put(value2);
        spdlog::info("extrinsics{}", hex(value2));
        delete it2;
        return res;
    }

    void getTransactions(uint32_t num) {
        kagome::common::Buffer bnum;
        bnum.putUint32(num);
        rocksdb::Slice key = bnum.asString();
        rocksdb::ReadOptions roptions;
        roptions.verify_checksums = false;
        rocksdb::ReadOptions roptions2;
        auto it2 = db_.db_->NewIterator(
                roptions2, db_.column_handles_[static_cast<uint32_t>(Columns::HEADER)]);
        it2->Seek(key);
        std::string value;
        db_.db_->Get(roptions, db_.column_handles_[static_cast<uint32_t>(Columns::TRANSACTIONS)], it2->value(), &value);
        spdlog::info("{}", hex(value));
        delete it2;
    }
};

int main() {
    PolkadotDatabase db("db");
    db.db()->getInt(static_cast<uint32_t>(Columns::KEY_LOOKUP), "rocksdb.estimate-num-keys");
//    db.db()->list(static_cast<uint32_t>(Columns::BODY), 5);
    auto header = db.getHeader(5639739).value();
    spdlog::info("{}", header.number);
    // db.getTransactions(5639739);
    // std::string* value = nullptr;
    // status = db->Get(rocksdb::ReadOptions(),
    //         column_handles[static_cast<uint32_t>(Columns::META)],
    //         rocksdb::Slice(meta_keys::GENESIS_HASH), value);
    // if (!status.ok()) {
    //   spdlog::error("DB error: {}", status.ToString());
    // }
//    spdlog::info("{}", hex(std::string(it2->key().data(), it2->key().size())));
//    spdlog::info("{}",
//                 hex(std::string(it2->value().data(), it2->value().size())));
    // spdlog::info("Key: {}, size: {}", value, value.size());
    // trie::Trie trie;
    // chainspec::loadFrom("localchain-dev.json", trie);
    db.db()->list(static_cast<uint32_t>(Columns::KEY_LOOKUP), 5);
    return EXIT_SUCCESS;
}
