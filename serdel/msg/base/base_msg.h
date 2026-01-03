#include "serdel/msg/base/msg_types.h"
#include <cstdint>
#include <ostream>
#include <vector>
#include "serdel/serializer/types.h"

#pragma once

class MsgBase {
public:
  explicit MsgBase(MsgType t) : type{t} {};
  virtual void toString(std::ostream &os) const;
  virtual ~MsgBase() = default; 
  MsgBase() = default;
  MsgType getType() const { return type;}
protected:
  MsgType type{MsgType::Base};
};

inline std::ostream &operator<<(std::ostream &os, const MsgBase &msg) {
  msg.toString(os); // dispatches to derived class
  return os;
}