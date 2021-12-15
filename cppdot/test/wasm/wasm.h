#ifndef WASM_WASM_HPP
#define WASM_WASM_HPP

#include <shell-interface.h>
#include <spdlog/spdlog.h>
#include <xxhash.h>

#include <iostream>

#include "common/blob.hpp"
#include "common/literals.hpp"
#include "scale/scale.hpp"
#include "trie/trie.hpp"

using namespace kagome::common::literals;

namespace runtime {

enum WasmLogLevel {
  WasmLL_Error = 0,
  WasmLL_Warn = 1,
  WasmLL_Info = 2,
  WasmLL_Debug = 3,
  WasmLL_Trace = 4,
};

}

constexpr uint32_t roundUp(uint32_t t) { return (t + 7) & ~7; }

std::pair<uint32_t, uint32_t> splitU64(uint64_t val) {
  return {val & 0xFFFFFFFFlu, (val >> 32u) & 0xFFFFFFFFlu};
}

void make_twox128(const uint8_t *in, uint32_t len, uint8_t *out) {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
  auto *ptr = reinterpret_cast<uint64_t *>(out);
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  ptr[0] = XXH64(in, len, 0);
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  ptr[1] = XXH64(in, len, 1);
}

kagome::common::Hash128 make_twox128(const kagome::common::Buffer &buf) {
  kagome::common::Hash128 hash{};
  make_twox128(buf.data(), buf.size(), hash.data());
  return hash;
}

class Allocator {
  std::map<uint32_t, uint32_t> allocated_;
  std::map<uint32_t, uint32_t> deallocatedPxS_;
  std::map<uint32_t, uint32_t> deallocatedSxP_;
  uint32_t init_{0}, size_{0};
  wasm::ShellExternalInterface::Memory *memory_;

  void clear() {
    allocated_.clear();
    deallocatedPxS_.clear();
    deallocatedSxP_.clear();
  }

public:
  Allocator(wasm::ShellExternalInterface::Memory *memory) : memory_(memory) {}

  void setheap(uint32_t init_offset, uint32_t heap_size) {
    clear();
    init_ = init_offset;
    size_ = heap_size;
  }

  uint32_t malloc_v1(uint32_t size) {
    uint32_t ptr{0};
    if (allocated_.empty()) {
      ptr = init_;
    }
    else {
      ptr = allocated_.rbegin()->first + allocated_.rbegin()->second;
      if (ptr + size > init_ + size_) {
        ptr = 0;
        spdlog::warn("Allocator memory finished, optimizations are not "
                     "implemented yet!");
      }
    }
    if (ptr) {
      spdlog::trace("Allocator allocated {} at {}", size, ptr);
      allocated_[ptr] = size;
    }
    return ptr;
  }

  void free_v1(uint32_t ptr) {
    if (auto it = allocated_.find(ptr); it != allocated_.end()) {
      spdlog::trace("Allocator freed {} at {}", it->second, it->first);
      deallocatedPxS_[it->first] = it->second;
      deallocatedSxP_[it->second] = it->first;
      allocated_.erase(it);
    }
    else {
      spdlog::warn("Allocator {} was not allocated!", ptr);
    }
  }
};

class RuntimeExternalInterface : public wasm::ShellExternalInterface {
  friend class Executor;

  uint32_t heap_size_, offset_, size_;
  trie::Trie *trie_;
  Allocator allocator_{&memory};

  kagome::common::Buffer load(uint64_t span) {
    auto [ptr, size] = splitU64(span);
    kagome::common::Buffer res;
    res.reserve(size);
    for (auto i = ptr; i < ptr + size; i++) {
      res.putUint8(load8u(i));
    }
    return res;
  }

  uint64_t store(uint32_t ptr, gsl::span<const uint8_t> buf) {
    for (size_t i = 0; i < buf.size(); ++i) {
      memory.set(ptr + i, buf[i]);
    }
    return static_cast<uint64_t>(ptr) | (buf.size() << 32ull);
  }

public:
  RuntimeExternalInterface(trie::Trie *trie)
      : heap_size_(5_MB), offset_(5_MB), size_(5_MB), trie_(trie) {
    memory.resize(size_);
  }

  void setHeapSize(uint32_t size) {
    heap_size_ = size;
    allocator_.setheap(size, 5_MB - size);
  }

  wasm::Literals callImport(wasm::Function *import,
                            wasm::LiteralList &arguments) override {
    std::cout << import->module << std::endl;
    if (import->module == "env") {
      std::cout << import->base << std::endl;
      if (import->base == "ext_logging_log_version_1") {
        ;
      }
      else if (import->base == "ext_logging_max_level_version_1") {
        auto res = wasm::Literal(int32_t(runtime::WasmLogLevel::WasmLL_Info));
        return {res};
      }
      else if (import->base == "ext_hashing_twox_128_version_1") {
        auto val = load(arguments.at(0).geti64());
        auto hash = make_twox128(val);
        auto [ptr, size] = splitU64(arguments.at(0).geti64());
        std::cout << 5_MB << " " << roundUp(5_MB) << " " << roundUp(hash.size())
                  << std::endl;
        // memory.resize(2_MB);
        store(0, hash);
        return {wasm::Literal(0)};
      }
      else if (import->base == "ext_allocator_malloc_version_1") {
        return {wasm::Literal(allocator_.malloc_v1(arguments.at(0).geti32()))};
      }
      else if (import->base == "ext_allocator_free_version_1") {
        allocator_.free_v1(arguments.at(0).geti32());
      }
      else if (import->base == "ext_storage_get_version_1") {
        auto val = trie_->get(load(arguments.at(0).geti64()));
        auto res = val ? boost::make_optional(*val) : boost::none;
        return {wasm::Literal(store(0, kagome::scale::encode(res).value()))};
      }
    }
    return wasm::Literals();
  }
};

#endif // WASM_WASM_HPP
