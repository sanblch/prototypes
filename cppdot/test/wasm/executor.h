#ifndef WASM_EXECUTOR_HPP
#define WASM_EXECUTOR_HPP

#include "wasm.h"

class Executor {
  std::unique_ptr<wasm::ModuleInstance> inst_;

public:
  Executor(wasm::Module &module, RuntimeExternalInterface *rei) {
    inst_ = std::make_unique<wasm::ModuleInstance>(module, rei);
  }

  template <typename R, typename... Args>
  outcome::result<R> execute(std::string_view name, Args &&... args) {
    uint32_t ptr = 0u;
    uint32_t len = 0u;

    if constexpr (sizeof...(args) > 0) {
      OUTCOME_TRY(buffer, scale::encode(std::forward<Args>(args)...));
      len = buffer.size();
      ptr = memory->allocate(len);
      memory->storeBuffer(ptr, common::Buffer(std::move(buffer)));
    }

    inst_->callExport(
        name, wasm::LiteralList{wasm::Literal(ptr), wasm::Literal(len)});
  }
};

#endif
