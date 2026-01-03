#include "serdel/msg/base/base_msg.h"
#include "serdel/serializer/binary_serde.h"

#include <map>
#include <ostream>
#include <unordered_map>

#pragma once

struct SomeName {
  float f{};
  double d{};
  bool b{};
  int arr[3]{};

  // Comparison operators
  bool operator==(const SomeName &other) const {
    if (f != other.f || d != other.d || b != other.b)
      return false;
    for (int i = 0; i < 3; ++i) {
      if (arr[i] != other.arr[i])
        return false;
    }
    return true;
  }

  bool operator!=(const SomeName &other) const { return !(*this == other); }

  bool operator<(const SomeName &other) const {
    if (f != other.f)
      return f < other.f;
    if (d != other.d)
      return d < other.d;
    if (b != other.b)
      return b < other.b;
    for (int i = 0; i < 3; ++i)
      if (arr[i] != other.arr[i])
        return arr[i] < other.arr[i];
    return false;
  }
};

// Hash for unordered_map
struct SomeNameHash {
  std::size_t operator()(const SomeName &sn) const {
    std::size_t h = std::hash<float>{}(sn.f);
    h ^= std::hash<double>{}(sn.d) + 0x9e3779b9 + (h << 6) + (h >> 2);
    h ^= std::hash<bool>{}(sn.b) + 0x9e3779b9 + (h << 6) + (h >> 2);
    for (int i = 0; i < 3; ++i) {
      h ^= std::hash<int>{}(sn.arr[i]) + 0x9e3779b9 + (h << 6) + (h >> 2);
    }
    return h;
  }
};

template <>
struct BinarySerde<SomeName> {
  static void write(SerializeBuffer& out, const SomeName& v) {
    BinarySerde<float>::write(out, v.f);
    BinarySerde<double>::write(out, v.d);
    BinarySerde<bool>::write(out, v.b);

    for (int i = 0; i < 3; ++i) {
      BinarySerde<int>::write(out, v.arr[i]);
    }
  }

  static SomeName read(const SerializeBuffer& in, size_t& offset) {
    SomeName v;
    v.f = BinarySerde<float>::read(in, offset);
    v.d = BinarySerde<double>::read(in, offset);
    v.b = BinarySerde<bool>::read(in, offset);

    for (int i = 0; i < 3; ++i) {
      v.arr[i] = BinarySerde<int>::read(in, offset);
    }

    return v;
  }
};

class B : public MsgBase {
public:
  B() : MsgBase(MsgType::B){};
  void toString(std::ostream &os) const override;

  float v{};
  std::map<uint32_t, std::string> map_uint_str;
  std::vector<std::int64_t> vec_int64;
  std::unordered_map<SomeName, std::string, SomeNameHash> umap_somename_str;
};

inline std::ostream &operator<<(std::ostream &os, const B &msg) {
  msg.toString(os);
  return os;
}