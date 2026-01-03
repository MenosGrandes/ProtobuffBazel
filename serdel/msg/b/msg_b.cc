#include "serdel/msg/b/msg_b.h"
#include "serdel/serializer/binary_serde.h"

#include "serdel/registry/msg_registrar.h"

#include <cstdint>
#include <memory>
#include <vector>

void B::toString(std::ostream &os) const {
  os << "A { v: " << v << ", map_uint_str: {";
  for (const auto &[k, val] : map_uint_str)
    os << k << ":" << val << ",";
  os << "}, vec_int64: [";
  for (auto n : vec_int64)
    os << n << ",";
  os << "], umap_somename_str: {";
  for (const auto &[k, val] : umap_somename_str)
    os << "{f:" << k.f << ",d:" << k.d << ",b:" << k.b << ",arr:[" << k.arr[0]
       << "," << k.arr[1] << "," << k.arr[2] << "]}" << ":" << val << ",";
  os << "} }";
}

void write_MsgB(SerializeBuffer &out, const MsgBase &base) {
  const auto &m = static_cast<const B &>(base);

  BinarySerde<float>::write(out, m.v);
  BinarySerde<std::map<uint32_t, std::string>>::write(out, m.map_uint_str);
  BinarySerde<std::vector<std::int64_t>>::write(out, m.vec_int64);
  BinarySerde<std::unordered_map<SomeName, std::string, SomeNameHash>>::write(
      out, m.umap_somename_str);
}

std::unique_ptr<MsgBase> read_MsgB(const SerializeBuffer &in, size_t &offset) {
  auto m = std::make_unique<B>();
  m->v = BinarySerde<float>::read(in, offset);
  m->map_uint_str =
      BinarySerde<std::map<uint32_t, std::string>>::read(in, offset);
  m->vec_int64 = BinarySerde<std::vector<std::int64_t>>::read(in, offset);
  m->umap_somename_str = BinarySerde<
      std::unordered_map<SomeName, std::string, SomeNameHash>>::read(in,
                                                                     offset);

  return m;
}
namespace {
constexpr auto APPROX_SIZE {
  ConstexprSizeSum_v<
      TypeCount<float>, TypeCount<std::map<uint32_t, std::string>, 100>,
      TypeCount<std::vector<std::int64_t>, 100>,
      TypeCount<std::unordered_map<SomeName, std::string, SomeNameHash>, 100>>
};
} // namespace
const MsgRegistrar reg_MsgA{
    MsgType::B, MsgSerdeEntry{&write_MsgB, &read_MsgB, APPROX_SIZE}};