#include "serde.hpp"

#include <gtest/gtest.h>

#include <cmath>

struct Example {
  int a;
  double b;
  bool operator==(const Example& other) const {
    return a == other.a && (std::abs(b - other.b) < 0.001);
  }
};

template <typename Stream>
Stream& operator&(Stream& s, Example& example) {
  s & example.a;
  s & example.b;
  return s;
}

TEST(sstream, input_output) {
  Example example { 15, 0.02 };
  std::ostringstream oss;
  oss & example;
  Example example2;
  std::istringstream iss(oss.str());
  iss & example2;
  ASSERT_EQ(example, example2);
}
