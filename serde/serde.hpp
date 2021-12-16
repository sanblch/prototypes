#pragma once

#include <istream>
#include <ostream>

template <typename T>
concept std_deserializable = requires(std::istream& is, T& a) {
  is >> a;
};

template <typename T>
concept std_serializable = requires(std::ostream& os, T a) {
  os << a;
};

template <std_deserializable T>
std::istream& operator&(std::istream& is, T& t) {
  is >> t;
  return is;
}

template <std_serializable T>
std::ostream& operator&(std::ostream& os, T t) {
  os << t << '\t';
  return os;
}
