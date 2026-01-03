#include "serdel/msg/a/msg_a.h"
#include "serdel/serializer/binary_serde.h"

#include "serdel/registry/msg_registrar.h"

#include <memory>

void A::toString(std::ostream &os) const {
  MsgBase::toString(os);
  os << " " << this->v << "\n";
}

void write_MsgA(std::vector<uint8_t> &out, const MsgBase &base) {
  const auto &m = static_cast<const A &>(base);
  BinarySerde<float>::write(out, m.v);
}

std::unique_ptr<MsgBase> read_MsgA(const std::vector<uint8_t> &in,
                                   size_t &offset) {
  auto m = std::make_unique<A>();
  m->v = BinarySerde<float>::read(in, offset);
  return m;
}
namespace {
constexpr auto APPROX_SIZE{ConstexprSizeSum_v<TypeCount<float>>};
} // namespace
const MsgRegistrar reg_MsgA{
    MsgType::A, MsgSerdeEntry{&write_MsgA, &read_MsgA, APPROX_SIZE}};