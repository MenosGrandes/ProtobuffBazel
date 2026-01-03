#include "serdel/msg/base/msg_types.h"
#include "serdel/registry/msg_registry.h"
#include "serdel/registry/msg_serde_entry.h"
#pragma once

struct MsgRegistrar {
  MsgRegistrar(MsgType t, MsgSerdeEntry entry) {
    MsgRegistry::instance().register_type(t, entry);
  }
};