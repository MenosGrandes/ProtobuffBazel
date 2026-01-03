#include "serdel/msg/base/base_msg.h"
#include <vector>
#include <memory>
#pragma once

struct MsgSerdeEntry {
    void (*write)(std::vector<uint8_t>&, const MsgBase&);
    std::unique_ptr<MsgBase> (*read)(const std::vector<uint8_t>&, size_t&);
    std::size_t approximateSize{0};
};
