/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_PRIMITIVES_SCHEDULED_CHANGE
#define KAGOME_CORE_PRIMITIVES_SCHEDULED_CHANGE

#include "primitives/authority.hpp"
#include "primitives/common.hpp"

namespace kagome::primitives {
struct DelayInChain {
  uint32_t subchain_length = 0;

  DelayInChain() = default;
  explicit DelayInChain(uint32_t delay) : subchain_length(delay) {}
  virtual ~DelayInChain() = default;
};

struct AuthorityListChange {
  AuthorityList authorities{};
  uint32_t subchain_length = 0;

  AuthorityListChange() = default;
  AuthorityListChange(AuthorityList authorities, uint32_t delay)
      : authorities(std::move(authorities)), subchain_length(delay) {}
  virtual ~AuthorityListChange() = default;
};

using Randomness = common::Blob<32>;

struct NextEpochData {
  /// The authorities actual for corresponding epoch
  AuthorityList authorities;

  /// The value of randomness to use for the slot-assignment.
  Randomness randomness;

  bool operator==(const NextEpochData &rhs) const {
    return authorities == rhs.authorities and randomness == rhs.randomness;
  }
  bool operator!=(const NextEpochData &rhs) const {
    return not operator==(rhs);
  }
};

struct NextConfigData final {
  std::tuple<uint64_t, uint64_t> ratio;
  uint8_t second_slot;
};

struct ScheduledChange final : public AuthorityListChange {
  using AuthorityListChange::AuthorityListChange;
};
struct ForcedChange final : public AuthorityListChange {
  using AuthorityListChange::AuthorityListChange;
};
struct OnDisabled {
  uint32_t authority_index = 0;
};
struct Pause final : public DelayInChain {
  using DelayInChain::DelayInChain;
};
struct Resume final : public DelayInChain {
  using DelayInChain::DelayInChain;
};

template <class Stream, typename = std::enable_if_t<Stream::is_encoder_stream>>
Stream &operator<<(Stream &s, const NextEpochData &digest) {
  return s << digest.authorities << digest.randomness;
}

template <class Stream, typename = std::enable_if_t<Stream::is_decoder_stream>>
Stream &operator>>(Stream &s, NextEpochData &digest) {
  return s >> digest.authorities >> digest.randomness;
}

template <class Stream>
Stream &operator<<(Stream &s, const NextConfigData &config) {
  return s << config.ratio << config.second_slot;
}

template <class Stream> Stream &operator>>(Stream &s, NextConfigData &config) {
  return s >> config.ratio >> config.second_slot;
}

template <class Stream>
Stream &operator<<(Stream &s, const DelayInChain &delay) {
  return s << delay.subchain_length;
}

template <class Stream> Stream &operator>>(Stream &s, DelayInChain &delay) {
  return s >> delay.subchain_length;
}

template <class Stream>
Stream &operator<<(Stream &s, const OnDisabled &target) {
  return s << target.authority_index;
}

template <class Stream> Stream &operator>>(Stream &s, OnDisabled &target) {
  return s >> target.authority_index;
}

template <class Stream>
Stream &operator<<(Stream &s, const AuthorityListChange &alc) {
  return s << alc.authorities << alc.subchain_length;
}

template <class Stream>
Stream &operator>>(Stream &s, AuthorityListChange &alc) {
  return s >> alc.authorities >> alc.subchain_length;
}
} // namespace kagome::primitives

#endif // KAGOME_CORE_PRIMITIVES_SCHEDULED_CHANGE
