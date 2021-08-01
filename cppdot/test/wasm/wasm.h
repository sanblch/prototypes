#ifndef WASM_WASM_HPP
#define WASM_WASM_HPP

#include <shell-interface.h>
#include <spdlog/spdlog.h>
#include <xxhash.h>

#include <iostream>

#include "common/blob.hpp"
#include "common/literals.hpp"

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

class RuntimeExternalInterface : public wasm::ShellExternalInterface {
  uint32_t heap_size_, offset_, size_;

  kagome::common::Buffer load(uint64_t span) {
    auto [ptr, size] = splitU64(span);
    kagome::common::Buffer res;
    res.reserve(size);
    for (auto i = ptr; i < ptr + size; i++) {
      res.putUint8(load8u(i));
    }
    return res;
  }

  void store(uint32_t ptr, gsl::span<const uint8_t> buf) {
    for (size_t i = 0; i < buf.size(); ++i) {
      memory.set(ptr + i, buf[i]);
    }
  }

public:
  RuntimeExternalInterface() : heap_size_(1_MB), offset_(1_MB), size_(1_MB) {
    memory.resize(size_);
  }

  void setHeapSize(uint32_t size) {
    heap_size_ = size;
    // if (heap_size_ < size && size_ < size) {
    //   heap_size_ = size;
    //   size_ = size;
    //   offset_ = size;
    //   memory.resize(size);
    //   spdlog::info("set heapsize: {}", size);
    // }
  }

  wasm::Literals callImport(wasm::Function *import,
                            wasm::LiteralList &arguments) override {
    std::cout << import->module << std::endl;
    if (import->module == "env") {
      std::cout << import->base << std::endl;
      if (import->base == "ext_logging_max_level_version_1") {
        auto res =
            wasm::Literal(int32_t(runtime::WasmLogLevel::WasmLL_Info));
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
        std::cout << 5_MB << " " << arguments.at(0).geti32() << " "
                  << roundUp(arguments.at(0).geti32()) << std::endl;
        memory.resize(roundUp(5_MB) + roundUp(arguments.at(0).geti32()));
        return {wasm::Literal(roundUp(5_MB))};
      }
      else if (import->base == "ext_allocator_free_version_1") {
        std::cout << arguments.at(0).geti32() << std::endl;
      }
      else if (import->base == "ext_storage_get_version_1") {
      }
    }
    return wasm::Literals();
  }
};

#endif // WASM_WASM_HPP
