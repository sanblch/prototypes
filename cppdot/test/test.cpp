#include <iomanip>
#include <iostream>
#include <type_traits>

template <typename T, auto N = sizeof(T)>
auto toArray(T t) {
  std::array<uint8_t, N> res;
  for(unsigned i = 0; i < N; ++i) {
    res[i] = static_cast<unsigned char&&>((t >> (N - i - 1) * 8) & 0xFF);
  }
  return res;
}

int main() {
  uint64_t n = 103583948502702485;
  std::cout << std::hex << n << std::endl;
  for(auto i : toArray(n))
    std::cout << std::hex << static_cast<uint64_t>(i) << std::endl;
  std::cout << sizeof(std::array<uint8_t, 16>) << std::endl;
}
