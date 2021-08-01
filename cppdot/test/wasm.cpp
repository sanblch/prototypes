#include <memory>

#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include <wasm-binary.h>

#include "chainspec.hpp"
#include "wasm/wasm.h"

TEST(Wasm, Simple) {
  trie::Trie trie;
  chainspec::loadFrom("localchain-dev.json", trie);
  auto rei = std::make_unique<RuntimeExternalInterface>();
  auto module = std::make_unique<wasm::Module>();
  {
    wasm::WasmBinaryBuilder parser(
        *module,
        reinterpret_cast<std::vector<char> const &>( // NOLINT
            trie.get(kagome::common::Buffer().put(":code"))->asVector()));

    try {
      parser.read();
    }
    catch (wasm::ParseException &e) {
      std::ostringstream msg;
      e.dump(msg);
      spdlog::error(msg.str());
    }

  }
  module->memory.initial = 1024;
  auto inst = std::make_unique<wasm::ModuleInstance>(*module, rei.get());
  auto heap_size = inst->getExport("__heap_base");
  rei->setHeapSize(heap_size[0].geti32());
  // auto res =
  //     inst->callExport("Core_version",
  //                      wasm::LiteralList{wasm::Literal(0u), wasm::Literal(0u)});
  // std::cout << res[0].geti64() << std::endl;
  auto res = inst->callExport("BabeApi_configuration", wasm::LiteralList{wasm::Literal(0u), wasm::Literal(0u)});
}

int main(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
