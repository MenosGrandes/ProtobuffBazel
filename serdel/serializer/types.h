#include <cstdint>
#include <vector>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <array>
#include <optional>
#include <type_traits>
#pragma once

using SerializeBuffer = std::vector<uint8_t>;


template <typename T, size_t N = 0, typename Enable = void>
struct ConstexprSize {
  static constexpr size_t value = sizeof(T) + N * sizeof(T);
};

// fundamental types and enums: ignore N
template <typename T>
struct ConstexprSize<T, 0, typename std::enable_if<std::is_fundamental<T>::value || std::is_enum<T>::value>::type> {
  static constexpr size_t value = sizeof(T);
};

// specialization for std::array: multiply element size by count
template <typename T, size_t N>
struct ConstexprSize<std::array<T, N>, 0> {
  static constexpr size_t value = N * ConstexprSize<T>::value;
};

// specialization for std::vector: add optional element count
template <typename T, size_t N>
struct ConstexprSize<std::vector<T>, N> {
  static constexpr size_t value = sizeof(std::vector<T>) + N * ConstexprSize<T>::value;
};

// specialization for std::string: add optional char count
template <size_t N>
struct ConstexprSize<std::string, N> {
  static constexpr size_t value = sizeof(std::string) + N * sizeof(char);
};

// Partial specialization for std::map (no default N here)
template <typename K, typename V, size_t N>
struct ConstexprSize<std::map<K,V>, N> {
    static constexpr size_t value = sizeof(std::map<K,V>) + N * (ConstexprSize<K>::value + ConstexprSize<V>::value);
};

// Partial specialization for std::unordered_map (no default N here)
template <typename K, typename V, typename Hash, typename Eq, typename Alloc, size_t N>
struct ConstexprSize<std::unordered_map<K,V,Hash,Eq,Alloc>, N> {
    static constexpr size_t value = sizeof(std::unordered_map<K,V,Hash,Eq,Alloc>) + N * (ConstexprSize<K>::value + ConstexprSize<V>::value);
};

// specialization for std::optional
template <typename T, size_t N>
struct ConstexprSize<std::optional<T>, N> {
  static constexpr size_t value = sizeof(std::optional<T>) + (N > 0 ? ConstexprSize<T, N>::value : 0);
};
template <typename T, size_t N = 0>
struct TypeCount {
    using type = T;
    static constexpr size_t count = N;
};

// ---------------------------
// Sum of types
// ---------------------------

template <typename... Ts>
struct ConstexprSizeSum;

template <>
struct ConstexprSizeSum<> {
    static constexpr size_t value = 0;
};

template <typename TC, typename... Rest>
struct ConstexprSizeSum<TC, Rest...> {
    static constexpr size_t value = ConstexprSize<typename TC::type, TC::count>::value + ConstexprSizeSum<Rest...>::value;
};

template <typename... Ts>
constexpr size_t ConstexprSizeSum_v = ConstexprSizeSum<Ts...>::value;