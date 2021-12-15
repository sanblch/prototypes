#pragma once

#include <wasmedge.h>

#include <string>
#include <tuple>
#include <utility>

template <typename T> WasmEdge_Value (*fromType())(const T) { return nullptr; }
template <> WasmEdge_Value (*fromType<int32_t>())(const int32_t) {
  return WasmEdge_ValueGenI32;
}
template <> WasmEdge_Value (*fromType<int64_t>())(const int64_t) {
  return WasmEdge_ValueGenI64;
}
template <> WasmEdge_Value (*fromType<float>())(const float) {
  return WasmEdge_ValueGenF32;
}
template <> WasmEdge_Value (*fromType<double>())(const double) {
  return WasmEdge_ValueGenF64;
}
template <> WasmEdge_Value (*fromType<__int128>())(const __int128) {
  return WasmEdge_ValueGenV128;
}

template <typename T> T (*toType())(const WasmEdge_Value) { return nullptr; }
template <> int32_t (*toType<int32_t>())(const WasmEdge_Value) {
  return WasmEdge_ValueGetI32;
}
template <> int64_t (*toType<int64_t>())(const WasmEdge_Value) {
  return WasmEdge_ValueGetI64;
}
template <> float (*toType<float>())(const WasmEdge_Value) {
  return WasmEdge_ValueGetF32;
}
template <> double (*toType<double>())(const WasmEdge_Value) {
  return WasmEdge_ValueGetF64;
}
template <> __int128 (*toType<__int128>())(const WasmEdge_Value) {
  return WasmEdge_ValueGetV128;
}

template <typename T> WasmEdge_ValType type() { return WasmEdge_ValType_V128; }
template <> WasmEdge_ValType type<int32_t>() { return WasmEdge_ValType_I32; }
template <> WasmEdge_ValType type<int64_t>() { return WasmEdge_ValType_I64; }
template <> WasmEdge_ValType type<float>() { return WasmEdge_ValType_F32; }
template <> WasmEdge_ValType type<double>() { return WasmEdge_ValType_F64; }

template <typename T, auto f, typename TP, size_t... I>
auto internal_fun(T *fun, const WasmEdge_Value *In, const TP &tup,
                  std::index_sequence<I...>) {
  return (fun->*f)(std::get<I>(tup)(In[I])...);
}

template <typename T, T> struct HostApiFuncRet;
template <typename T, typename R, typename... Args, R (T::*mf)(Args...)>
struct HostApiFuncRet<R (T::*)(Args...), mf> {
  using Ret = R;
};

/* Host function body definition. */
template <typename T, auto f, typename... Args>
WasmEdge_Result fun(void *Data, WasmEdge_MemoryInstanceContext *MemCxt,
                    const WasmEdge_Value *In, WasmEdge_Value *Out) {
  auto indices = std::index_sequence_for<Args...>{};
  auto tup = std::make_tuple(toType<Args>()...);
  if constexpr (not std::is_same_v<
                    void, typename HostApiFuncRet<decltype(f), f>::Ret>) {
    auto res = internal_fun<T, f>(static_cast<T *>(Data), In, tup, indices);
    Out[0] = fromType<decltype(res)>()(res);
  }
  else {
    internal_fun<T, f>(static_cast<T *>(Data), In, tup, indices);
  }
  return WasmEdge_Result_Success;
}

template <typename T, T> struct HostApiFunc;
template <typename T, typename R, typename... Args, R (T::*mf)(Args...)>
struct HostApiFunc<R (T::*)(Args...), mf> {
  using Ret = R;
  static WasmEdge_Result (*fun())(void *Data,
                                  WasmEdge_MemoryInstanceContext *MemCxt,
                                  const WasmEdge_Value *In,
                                  WasmEdge_Value *Out) {
    return ::fun<T, mf, Args...>;
  }

  static WasmEdge_FunctionTypeContext *type() {
    enum WasmEdge_ValType ParamList[sizeof...(Args)] = {::type<Args>()...};

    size_t r = 1;
    enum WasmEdge_ValType ReturnListNonVoid[1] = {::type<R>()};
    enum WasmEdge_ValType *ReturnList = ReturnListNonVoid;

    if constexpr (std::is_same_v<R, void>) {
      r = 0;
      ReturnList = nullptr;
    }

    return WasmEdge_FunctionTypeCreate(ParamList, sizeof...(Args), ReturnList,
                                       r);
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

#define REGISTER_HOST_API_FUNC(class, name, obj)                               \
  registerHostApiFunc<&class ::name>(#name, obj);
