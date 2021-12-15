#pragma once

#include "common/buffer.hpp"

#include <wasmedge.h>

#include <map>

class Allocator {
  std::map<uint32_t, uint32_t> allocated_;
  std::map<uint32_t, uint32_t> deallocatedPxS_;
  std::map<uint32_t, uint32_t> deallocatedSxP_;
  uint32_t init_{4096}, size_{1000000};

public:
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
};

class Memory {
  WasmEdge_MemoryInstanceContext *memory_;

public:
  Memory(WasmEdge_ImportObjectContext *ImpObj) {
    WasmEdge_Limit MemLimit = {false, 40, 40};
    /* Create the memory instance with limit. The memory page size is 64KiB. */
    memory_ = WasmEdge_MemoryInstanceCreate(MemLimit);
    WasmEdge_String MemoryName = WasmEdge_StringCreateByCString("memory");
    WasmEdge_ImportObjectAddMemory(ImpObj, MemoryName, memory_);
  }

  kagome::common::Buffer load(uint32_t ptr, uint32_t size) {
    kagome::common::Buffer res(size, 0);
    WasmEdge_Result Res =
        WasmEdge_MemoryInstanceGetData(memory_, res.data(), ptr, size);
    return res;
  }
};
