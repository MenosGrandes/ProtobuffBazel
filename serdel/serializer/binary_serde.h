#include "serdel/serializer/types.h"
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>
#pragma once

template <typename> inline constexpr bool dependent_false_v = false;

template <typename T, typename = void> struct BinarySerde {
  static_assert(dependent_false_v<T>, "BinarySerde<T> is not registered");
};

/* ============================================================
   memcpy-based trivial serde (N × uint8_t, no reinterpret_cast)
   ============================================================ */
template <typename T> struct BinarySerdeMemcpy {
  static_assert(std::is_trivially_copyable_v<T>,
                "BinarySerdeMemcpy requires trivially copyable type");

  static void write(SerializeBuffer &out, const T &v) {
    std::uint8_t bytes[sizeof(T)];
    std::memcpy(bytes, &v, sizeof(T));
    out.insert(out.end(), bytes, bytes + sizeof(T));
  }

  static T read(const SerializeBuffer &in, size_t &offset) {
    T v;
    std::memcpy(&v, in.data() + offset, sizeof(T));
    offset += sizeof(T);
    return v;
  }
};

/* ============================================================
   Integer specializations
   ============================================================ */
#define X(T)                                                                   \
  template <> struct BinarySerde<T> : BinarySerdeMemcpy<T> {};
X(std::int8_t)
X(std::uint8_t)
X(std::int16_t)
X(std::uint16_t)
X(std::int32_t)
X(std::uint32_t)
X(std::int64_t)
X(std::uint64_t)
#undef X

/* ============================================================
   Floating point
   ============================================================ */
template <> struct BinarySerde<float> : BinarySerdeMemcpy<float> {};

template <> struct BinarySerde<double> : BinarySerdeMemcpy<double> {};

/* ============================================================
   bool (explicit representation, not memcpy)
   ============================================================ */
template <> struct BinarySerde<bool> {
  static void write(SerializeBuffer &out, bool v) {
    std::uint8_t b = v ? 1 : 0;
    out.push_back(b);
  }

  static bool read(const SerializeBuffer &in, size_t &offset) {
    return in[offset++] != 0;
  }
};

/* ============================================================
   std::string
   ============================================================ */
template <> struct BinarySerde<std::string> {
  static void write(SerializeBuffer &out, const std::string &s) {
    std::uint64_t size = static_cast<std::uint64_t>(s.size());
    BinarySerde<std::uint64_t>::write(out, size);

    if (size != 0) {
      size_t old_size = out.size();
      out.resize(old_size + size);
      std::memcpy(out.data() + old_size, s.data(), size);
    }
  }

  static std::string read(const SerializeBuffer &in, size_t &offset) {
    std::uint64_t size = BinarySerde<std::uint64_t>::read(in, offset);
    std::string s;

    if (size != 0) {
      s.resize(size);
      std::memcpy(s.data(), in.data() + offset, size);
      offset += size;
    }

    return s;
  }
};
template <typename T>
struct BinarySerde<
    std::vector<T>,
    std::enable_if_t<std::is_trivially_copyable_v<T>>> {

  static void write(SerializeBuffer& out, const std::vector<T>& v) {
    std::uint64_t size = static_cast<std::uint64_t>(v.size());
    BinarySerde<std::uint64_t>::write(out, size);

    if (size != 0) {
      size_t bytes = size * sizeof(T);
      size_t old_size = out.size();
      out.resize(old_size + bytes);
      std::memcpy(out.data() + old_size, v.data(), bytes);
    }
  }

  static std::vector<T> read(const SerializeBuffer& in, size_t& offset) {
    std::uint64_t size = BinarySerde<std::uint64_t>::read(in, offset);
    std::vector<T> v(size);

    //if (size != 0) { //vector cannot be serialized with 0 elements, its pointless.
      size_t bytes = size * sizeof(T);
      std::memcpy(v.data(), in.data() + offset, bytes);
      offset += bytes;
    //}

    return v;
  }
};

// GENERIC PATH — non-trivial
template <typename T>
struct BinarySerde<
    std::vector<T>,
    std::enable_if_t<!std::is_trivially_copyable_v<T>>> {

  static void write(SerializeBuffer& out, const std::vector<T>& v) {
    std::uint64_t size = static_cast<std::uint64_t>(v.size());
    BinarySerde<std::uint64_t>::write(out, size);

    for (const T& e : v)
      BinarySerde<T>::write(out, e);
  }

  static std::vector<T> read(const SerializeBuffer& in, size_t& offset) {
    std::uint64_t size = BinarySerde<std::uint64_t>::read(in, offset);
    std::vector<T> v;
    v.reserve(size);

    for (std::uint64_t i = 0; i < size; ++i)
      v.push_back(BinarySerde<T>::read(in, offset));

    return v;
  }
};
/* ============================================================
   std::map
   ============================================================ */
template <typename K, typename V> struct BinarySerde<std::map<K, V>> {
  static void write(SerializeBuffer &out, const std::map<K, V> &m) {
    std::uint64_t size = static_cast<std::uint64_t>(m.size());
    BinarySerde<std::uint64_t>::write(out, size);

    for (const auto &[k, v] : m) {
      BinarySerde<K>::write(out, k);
      BinarySerde<V>::write(out, v);
    }
  }

  static std::map<K, V> read(const SerializeBuffer &in, size_t &offset) {
    std::uint64_t size = BinarySerde<std::uint64_t>::read(in, offset);
    std::map<K, V> m;

    for (std::uint64_t i = 0; i < size; ++i) {
      K k = BinarySerde<K>::read(in, offset);
      V v = BinarySerde<V>::read(in, offset);
      m.emplace(std::move(k), std::move(v));
    }

    return m;
  }
};
/* ============================================================
   std::unordered_map
   ============================================================ */
template <typename K, typename V, typename Hash, typename Eq, typename Alloc>
struct BinarySerde<std::unordered_map<K, V, Hash, Eq, Alloc>> {

  static void write(SerializeBuffer &out,
                    const std::unordered_map<K, V, Hash, Eq, Alloc> &m) {
    std::uint64_t size = static_cast<std::uint64_t>(m.size());
    BinarySerde<std::uint64_t>::write(out, size);

    for (const auto &kv : m) {
      BinarySerde<K>::write(out, kv.first);
      BinarySerde<V>::write(out, kv.second);
    }
  }

  static std::unordered_map<K, V, Hash, Eq, Alloc>
  read(const SerializeBuffer &in, size_t &offset) {

    std::uint64_t size = BinarySerde<std::uint64_t>::read(in, offset);
    std::unordered_map<K, V, Hash, Eq, Alloc> m;
    m.reserve(static_cast<size_t>(size));

    for (std::uint64_t i = 0; i < size; ++i) {
      K k = BinarySerde<K>::read(in, offset);
      V v = BinarySerde<V>::read(in, offset);
      m.emplace(std::move(k), std::move(v));
    }

    return m;
  }
};
/* ============================================================
   std::optional
   ============================================================ */
template <typename T> struct BinarySerde<std::optional<T>> {
  static void write(SerializeBuffer &out, const std::optional<T> &opt) {
    BinarySerde<bool>::write(out, opt.has_value());
    if (opt.has_value()) {
      BinarySerde<T>::write(out, *opt);
    }
  }

  static std::optional<T> read(const SerializeBuffer &in, size_t &offset) {
    bool has_value = BinarySerde<bool>::read(in, offset);
    if (!has_value)
      return std::nullopt;

    return BinarySerde<T>::read(in, offset);
  }
};

/* ============================================================
   Variadic serializer / deserializer
   ============================================================ */
template <typename... Ts> struct Deserializer {
  static std::tuple<Ts...> read(const SerializeBuffer &in, size_t &offset) {
    return std::make_tuple(BinarySerde<Ts>::read(in, offset)...);
  }
};

template <typename... Ts> struct Serializer {
  static void write(SerializeBuffer &out, const std::tuple<Ts...> &t) {
    std::apply(
        [&](auto &&...args) {
          (BinarySerde<std::decay_t<decltype(args)>>::write(out, args), ...);
        },
        t);
  }
};

/* ============================================================
   Enums (serialize underlying type)
   ============================================================ */
template <typename E>
struct BinarySerde<E, std::enable_if_t<std::is_enum_v<E>>> {
  using U = std::underlying_type_t<E>;

  static void write(SerializeBuffer &out, E value) {
    BinarySerde<U>::write(out, static_cast<U>(value));
  }

  static E read(const SerializeBuffer &in, size_t &offset) {
    U v = BinarySerde<U>::read(in, offset);
    return static_cast<E>(v);
  }
};