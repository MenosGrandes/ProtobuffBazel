#include <cstdint>
#include <vector>
#include "serdel/msg/base/base_msg.h"
#include "serdel/registry/msg_serde_entry.h"
#include "serdel/registry/msg_registry.h"
#include "serdel/msg/base/msg_types.h"
#include "serdel/serializer/binary_serde.h"

#pragma once

inline void serialize_msg(
    std::vector<uint8_t>& out,
    const std::unique_ptr<MsgBase>& msg
) {
    MsgType t = msg->getType();
    BinarySerde<MsgType>::write(out, t);
    const auto& e = MsgRegistry::instance().get(t);
    #ifdef USE_RESERVE
    out.reserve(e.approximateSize);
    #endif
    e.write(out, *msg);
}

inline std::unique_ptr<MsgBase> deserialize_msg(
    const std::vector<uint8_t>& in,
    size_t& offset
) {
    MsgType t = BinarySerde<MsgType>::read(in, offset);
    const auto& e = MsgRegistry::instance().get(t);

    return e.read(in, offset);
}