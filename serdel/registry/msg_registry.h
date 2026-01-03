#include "serdel/msg/base/msg_types.h"
#include "serdel/registry/msg_serde_entry.h"
#include <unordered_map>
#pragma once

class MsgRegistry {
public:
  static MsgRegistry &instance() {
    static MsgRegistry r;
    return r;
  }

  void register_type(MsgType t, MsgSerdeEntry e) { table[t] = e; }

  const MsgSerdeEntry &get(MsgType t) const {
    auto it = table.find(t);
    if (it == table.end())
      throw std::runtime_error("Unregistered MsgType");
    return it->second;
  }

private:
  std::unordered_map<MsgType, MsgSerdeEntry> table;
};
