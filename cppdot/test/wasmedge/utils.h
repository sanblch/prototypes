#pragma once

std::pair<uint32_t, uint32_t> splitU64(uint64_t val) {
  return {val & 0xFFFFFFFFlu, (val >> 32u) & 0xFFFFFFFFlu};
}
