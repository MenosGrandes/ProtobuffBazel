#include "serdel/msg/base/base_msg.h"
#include "msg_types.h"
#include "serdel/serializer/binary_serde.h"
#include <cstdint>
#include <ostream>
#include <tuple>
#include <vector>

void MsgBase::toString(std::ostream &os) const { os << type; }
