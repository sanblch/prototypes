#pragma once

#include <spdlog/spdlog.h>
#include <wasmedge.h>

#include "wasmedge/memory.h"
#include "wasmedge/reg.h"

using I32 = int32_t;
using I64 = int64_t;

class HostApi {
  Allocator allocator;

public:
  void reg(WasmEdge_ImportObjectContext *ImpObj){
      // clang-format off
    REGISTER_HOST_API_FUNC(HostApi, ext_trie_blake2_256_ordered_root_version_1,
                           ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_offchain_index_set_version_1, ImpObj)

    REGISTER_HOST_API_FUNC(HostApi, ext_logging_log_version_1, ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_crypto_ed25519_generate_version_1, ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_crypto_ed25519_verify_version_1, ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_crypto_finish_batch_verify_version_1,
                           ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_crypto_secp256k1_ecdsa_recover_version_1,
                           ImpObj)
    REGISTER_HOST_API_FUNC(
        HostApi, ext_crypto_secp256k1_ecdsa_recover_compressed_version_1, ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_crypto_sr25519_generate_version_1, ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_crypto_sr25519_public_keys_version_1,
                           ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_crypto_sr25519_sign_version_1, ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_crypto_sr25519_verify_version_2, ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_crypto_start_batch_verify_version_1,
                           ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_hashing_blake2_128_version_1, ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_hashing_blake2_256_version_1, ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_hashing_keccak_256_version_1, ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_hashing_twox_128_version_1, ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_hashing_twox_64_version_1, ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_allocator_free_version_1, ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_allocator_malloc_version_1, ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_misc_print_hex_version_1, ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_misc_print_utf8_version_1, ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_misc_runtime_version_version_1, ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_offchain_is_validator_version_1, ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_offchain_local_storage_clear_version_1,
                           ImpObj)
    REGISTER_HOST_API_FUNC(
        HostApi, ext_offchain_local_storage_compare_and_set_version_1, ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_offchain_local_storage_get_version_1,
                           ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_offchain_local_storage_set_version_1,
                           ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_offchain_network_state_version_1, ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_offchain_random_seed_version_1, ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_offchain_submit_transaction_version_1,
                           ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_offchain_timestamp_version_1, ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_storage_append_version_1, ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_storage_changes_root_version_1, ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_storage_clear_version_1, ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_storage_clear_prefix_version_2, ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_storage_commit_transaction_version_1,
                           ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_storage_exists_version_1, ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_storage_get_version_1, ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_storage_next_key_version_1, ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_storage_read_version_1, ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_storage_rollback_transaction_version_1,
                           ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_storage_root_version_1, ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_storage_set_version_1, ImpObj)
    REGISTER_HOST_API_FUNC(HostApi, ext_storage_start_transaction_version_1,
                         ImpObj)
      // clang-format on
  }

  I32 ext_trie_blake2_256_ordered_root_version_1(I64) {
    spdlog::info(__PRETTY_FUNCTION__);
    return 0;
  }
  void ext_offchain_index_set_version_1(I64, I64) {
    spdlog::info(__PRETTY_FUNCTION__);
  }
  void ext_logging_log_version_1(I32, I64, I64) {
    spdlog::info(__PRETTY_FUNCTION__);
  }
  I32 ext_crypto_ed25519_generate_version_1(I32, I64) {
    spdlog::info(__PRETTY_FUNCTION__);
    return 0;
  }
  I32 ext_crypto_ed25519_verify_version_1(I32, I64, I32) {
    spdlog::info(__PRETTY_FUNCTION__);
    return 0;
  }
  I32 ext_crypto_finish_batch_verify_version_1() {
    spdlog::info(__PRETTY_FUNCTION__);
    return 0;
  }
  I64 ext_crypto_secp256k1_ecdsa_recover_version_1(I32, I32) {
    spdlog::info(__PRETTY_FUNCTION__);
    return 0;
  }
  I64 ext_crypto_secp256k1_ecdsa_recover_compressed_version_1(I32, I32) {
    spdlog::info(__PRETTY_FUNCTION__);
    return 0;
  }
  I32 ext_crypto_sr25519_generate_version_1(I32, I64) {
    spdlog::info(__PRETTY_FUNCTION__);
    return 0;
  }
  I64 ext_crypto_sr25519_public_keys_version_1(I32) {
    spdlog::info(__PRETTY_FUNCTION__);
    return 0;
  }
  I64 ext_crypto_sr25519_sign_version_1(I32, I32, I64) {
    spdlog::info(__PRETTY_FUNCTION__);
    return 0;
  }
  I32 ext_crypto_sr25519_verify_version_2(I32, I64, I32) {
    spdlog::info(__PRETTY_FUNCTION__);
    return 0;
  }
  void ext_crypto_start_batch_verify_version_1() {
    spdlog::info(__PRETTY_FUNCTION__);
  }
  I32 ext_hashing_blake2_128_version_1(I64) {
    spdlog::info(__PRETTY_FUNCTION__);
    return 0;
  }
  I32 ext_hashing_blake2_256_version_1(I64) {
    spdlog::info(__PRETTY_FUNCTION__);
    return 0;
  }
  I32 ext_hashing_keccak_256_version_1(I64) {
    spdlog::info(__PRETTY_FUNCTION__);
    return 0;
  }
  I32 ext_hashing_twox_128_version_1(I64) {
    spdlog::info(__PRETTY_FUNCTION__);
    return 0;
  }
  I32 ext_hashing_twox_64_version_1(I64) {
    spdlog::info(__PRETTY_FUNCTION__);
    return 0;
  }
  void ext_allocator_free_version_1(I32) { spdlog::info(__PRETTY_FUNCTION__); }
  I32 ext_allocator_malloc_version_1(I32 size) {
    spdlog::info(__PRETTY_FUNCTION__);
    return allocator.malloc_v1(size);
  }
  void ext_misc_print_hex_version_1(I64) { spdlog::info(__PRETTY_FUNCTION__); }
  void ext_misc_print_utf8_version_1(I64) { spdlog::info(__PRETTY_FUNCTION__); }
  I64 ext_misc_runtime_version_version_1(I64) {
    spdlog::info(__PRETTY_FUNCTION__);
    return 0;
  }
  I32 ext_offchain_is_validator_version_1() {
    spdlog::info(__PRETTY_FUNCTION__);
    return 0;
  }
  void ext_offchain_local_storage_clear_version_1(I32, I64) {
    spdlog::info(__PRETTY_FUNCTION__);
  }
  I32 ext_offchain_local_storage_compare_and_set_version_1(I32, I64, I64, I64) {
    spdlog::info(__PRETTY_FUNCTION__);
    return 0;
  }
  I64 ext_offchain_local_storage_get_version_1(I32, I64) {
    spdlog::info(__PRETTY_FUNCTION__);
    return 0;
  }
  void ext_offchain_local_storage_set_version_1(I32, I64, I64) {
    spdlog::info(__PRETTY_FUNCTION__);
  }
  I64 ext_offchain_network_state_version_1() {
    spdlog::info(__PRETTY_FUNCTION__);
    return 0;
  }
  I32 ext_offchain_random_seed_version_1() {
    spdlog::info(__PRETTY_FUNCTION__);
    return 0;
  }
  I64 ext_offchain_submit_transaction_version_1(I64) {
    spdlog::info(__PRETTY_FUNCTION__);
    return 0;
  }
  I64 ext_offchain_timestamp_version_1() {
    spdlog::info(__PRETTY_FUNCTION__);
    return 0;
  }
  void ext_storage_append_version_1(I64, I64) {
    spdlog::info(__PRETTY_FUNCTION__);
  }
  I64 ext_storage_changes_root_version_1(I64) {
    spdlog::info(__PRETTY_FUNCTION__);
    return 0;
  }
  void ext_storage_clear_version_1(I64) { spdlog::info(__PRETTY_FUNCTION__); }
  I64 ext_storage_clear_prefix_version_2(I64, I64) {
    spdlog::info(__PRETTY_FUNCTION__);
    return 0;
  }
  void ext_storage_commit_transaction_version_1() {
    spdlog::info(__PRETTY_FUNCTION__);
  }
  I32 ext_storage_exists_version_1(I64) {
    spdlog::info(__PRETTY_FUNCTION__);
    return 0;
  }
  I64 ext_storage_get_version_1(I64) {
    spdlog::info(__PRETTY_FUNCTION__);
    return 0;
  }
  I64 ext_storage_next_key_version_1(I64) {
    spdlog::info(__PRETTY_FUNCTION__);
    return 0;
  }
  I64 ext_storage_read_version_1(I64, I64, I32) {
    spdlog::info(__PRETTY_FUNCTION__);
    return 0;
  }
  void ext_storage_rollback_transaction_version_1() {
    spdlog::info(__PRETTY_FUNCTION__);
  }
  I64 ext_storage_root_version_1() {
    spdlog::info(__PRETTY_FUNCTION__);
    return 0;
  }
  void ext_storage_set_version_1(I64, I64) {
    spdlog::info(__PRETTY_FUNCTION__);
  }
  void ext_storage_start_transaction_version_1() {
    spdlog::info(__PRETTY_FUNCTION__);
  }
};
