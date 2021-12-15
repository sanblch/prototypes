#include "primitives/version.hpp"
#include "scale/scale.hpp"
#include "wasmedge/host_api.h"
#include "wasmedge/utils.h"

int main(int Argc, const char *Argv[]) {
  HostApi host_api;
  /* Create the configure context and add the WASI support. */
  /* This step is not necessary unless you need WASI support. */
  WasmEdge_ConfigureContext *ConfCxt = WasmEdge_ConfigureCreate();
  WasmEdge_ConfigureAddHostRegistration(ConfCxt,
                                        WasmEdge_HostRegistration_Wasi);
  /* The configure and store context to the VM creation can be NULL. */
  WasmEdge_VMContext *VMCxt = WasmEdge_VMCreate(ConfCxt, NULL);

  WasmEdge_String ExportName = WasmEdge_StringCreateByCString("env");
  WasmEdge_ImportObjectContext *ImpObj =
      WasmEdge_ImportObjectCreate(ExportName, &host_api);
  host_api.reg(ImpObj);

  Memory memory(ImpObj);

  WasmEdge_VMRegisterModuleFromImport(VMCxt, ImpObj);

  auto LoaderCxt = WasmEdge_LoaderCreate(nullptr);
  WasmEdge_ASTModuleContext * ASTCxt = nullptr;
  auto Res = WasmEdge_LoaderParseFromFile(LoaderCxt, &ASTCxt, Argv[1]);

  /* The parameters and returns arrays. */
  WasmEdge_Value Params[2] = {WasmEdge_ValueGenI32(0), WasmEdge_ValueGenI32(0)};
  WasmEdge_Value Returns[1];
  /* Function name. */
  WasmEdge_String FuncName = WasmEdge_StringCreateByCString("BabeApi_configuration");
  /* Run the WASM function from file. */
  Res = WasmEdge_VMRunWasmFromASTModule(VMCxt, ASTCxt, FuncName,
                                                   Params, 2, Returns, 1);

  if (WasmEdge_ResultOK(Res)) {
    auto res = WasmEdge_ValueGetI64(Returns[0]);
    auto [ptr, size] = splitU64(res);
    spdlog::info("Get result: {} {}", ptr, size);
    auto buf = memory.load(ptr, size);
    auto version =
        kagome::scale::decode<kagome::primitives::Version>(std::move(buf));
    if (version.has_value())
      spdlog::info("{} {} {}", version.value().spec_name,
                   version.value().impl_name, version.value().spec_version);
  }
  else {
    printf("Error message: %s\n", WasmEdge_ResultGetMessage(Res));
  }

  /* Resources deallocations. */
  WasmEdge_VMDelete(VMCxt);
  WasmEdge_ConfigureDelete(ConfCxt);
  WasmEdge_StringDelete(FuncName);
  return 0;
}
