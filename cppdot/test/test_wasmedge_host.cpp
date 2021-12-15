#include <stdio.h>
#include <string>
#include <tuple>
#include <utility>
#include <wasmedge.h>

class Addition {
public:
  int32_t add(int32_t a, int32_t b) { return a + b; }
};

template <typename T> WasmEdge_Value (*fromType())(const T) { return nullptr; }
template <> WasmEdge_Value (*fromType<int32_t>())(const int32_t) {
  return WasmEdge_ValueGenI32;
}

template <typename T> T (*toType())(const WasmEdge_Value) { return nullptr; }
template <> int32_t (*toType<int32_t>())(const WasmEdge_Value) {
  return WasmEdge_ValueGetI32;
}

template <typename T> WasmEdge_ValType type() { return WasmEdge_ValType_V128; }
template <> WasmEdge_ValType type<int32_t>() { return WasmEdge_ValType_I32; }

template <typename T, auto f, typename TP, size_t... I>
auto internal_fun(T *fun, const WasmEdge_Value *In, const TP &tup,
                  std::index_sequence<I...>) {
  return (fun->*f)(std::get<I>(tup)(In[I])...);
}

/* Host function body definition. */
template <typename T, auto f, typename... Args>
WasmEdge_Result fun(void *Data, WasmEdge_MemoryInstanceContext *MemCxt,
                    const WasmEdge_Value *In, WasmEdge_Value *Out) {
  auto indices = std::index_sequence_for<Args...>{};
  auto tup = std::make_tuple(toType<Args>()...);
  auto res = internal_fun<T, f>(static_cast<T *>(Data), In, tup, indices);
  Out[0] = fromType<decltype(res)>()(res);
  return WasmEdge_Result_Success;
}

template <typename T, T> struct HostApiFunc;
template <typename T, typename R, typename... Args, R (T::*mf)(Args...)>
struct HostApiFunc<R (T::*)(Args...), mf> {
  static WasmEdge_Result (*fun())(void *Data,
                                  WasmEdge_MemoryInstanceContext *MemCxt,
                                  const WasmEdge_Value *In,
                                  WasmEdge_Value *Out) {
    return ::fun<T, mf, Args...>;
  }

  static WasmEdge_FunctionTypeContext *type() {
    enum WasmEdge_ValType ParamList[sizeof...(Args)] = {::type<Args>()...};
    enum WasmEdge_ValType ReturnList[1] = {::type<R>()};
    return WasmEdge_FunctionTypeCreate(ParamList, sizeof...(Args), ReturnList,
                                       1);
  }
};

template <auto mf>
void registerHostApiFunc(const std::string &name,
                         WasmEdge_ImportObjectContext *ImpObj) {
  WasmEdge_FunctionTypeContext *HostFType =
      HostApiFunc<decltype(mf), mf>::type();
  WasmEdge_HostFunctionContext *HostFunc = WasmEdge_HostFunctionCreate(
      HostFType, HostApiFunc<decltype(mf), mf>::fun(), 0);
  WasmEdge_FunctionTypeDelete(HostFType);
  WasmEdge_String HostFuncName = WasmEdge_StringCreateByCString(name.c_str());
  WasmEdge_ImportObjectAddHostFunction(ImpObj, HostFuncName, HostFunc);
  WasmEdge_StringDelete(HostFuncName);
}

int main() {
  /* Create the VM context. */
  WasmEdge_VMContext *VMCxt = WasmEdge_VMCreate(NULL, NULL);

  /* The WASM module buffer. */
  uint8_t WASM[] = {/* WASM header */
                    0x00, 0x61, 0x73, 0x6D, 0x01, 0x00, 0x00, 0x00,
                    /* Type section */
                    0x01, 0x07, 0x01,
                    /* function type {i32, i32} -> {i32} */
                    0x60, 0x02, 0x7F, 0x7F, 0x01, 0x7F,
                    /* Import section */
                    0x02, 0x13, 0x01,
                    /* module name: "extern" */
                    0x06, 0x65, 0x78, 0x74, 0x65, 0x72, 0x6E,
                    /* extern name: "func-add" */
                    0x08, 0x66, 0x75, 0x6E, 0x63, 0x2D, 0x61, 0x64, 0x64,
                    /* import desc: func 0 */
                    0x00, 0x00,
                    /* Function section */
                    0x03, 0x02, 0x01, 0x00,
                    /* Export section */
                    0x07, 0x0A, 0x01,
                    /* export name: "addTwo" */
                    0x06, 0x61, 0x64, 0x64, 0x54, 0x77, 0x6F,
                    /* export desc: func 0 */
                    0x00, 0x01,
                    /* Code section */
                    0x0A, 0x0A, 0x01,
                    /* code body */
                    0x08, 0x00, 0x20, 0x00, 0x20, 0x01, 0x10, 0x00, 0x0B};

  /* Create the import object. */
  Addition o;
  WasmEdge_String ExportName = WasmEdge_StringCreateByCString("extern");
  WasmEdge_ImportObjectContext *ImpObj =
      WasmEdge_ImportObjectCreate(ExportName, &o);
  registerHostApiFunc<&Addition::add>("func-add", ImpObj);

  WasmEdge_VMRegisterModuleFromImport(VMCxt, ImpObj);

  /* The parameters and returns arrays. */
  WasmEdge_Value Params[2] = {WasmEdge_ValueGenI32(1234),
                              WasmEdge_ValueGenI32(5678)};
  WasmEdge_Value Returns[1];
  /* Function name. */
  WasmEdge_String FuncName = WasmEdge_StringCreateByCString("addTwo");
  /* Run the WASM function from file. */
  WasmEdge_Result Res = WasmEdge_VMRunWasmFromBuffer(
      VMCxt, WASM, sizeof(WASM), FuncName, Params, 2, Returns, 1);

  if (WasmEdge_ResultOK(Res)) {
    printf("Get the result: %d\n", WasmEdge_ValueGetI32(Returns[0]));
  }
  else {
    printf("Error message: %s\n", WasmEdge_ResultGetMessage(Res));
  }

  /* Resources deallocations. */
  WasmEdge_VMDelete(VMCxt);
  WasmEdge_StringDelete(FuncName);
  WasmEdge_ImportObjectDelete(ImpObj);
  return 0;
}
