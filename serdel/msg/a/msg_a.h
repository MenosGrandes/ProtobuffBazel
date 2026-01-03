#include "serdel/msg/base/base_msg.h"
#include <ostream>

#pragma once

class A : public MsgBase {
public:
  A(float v) : MsgBase(MsgType::A), v{v} {};
  A() :MsgBase(MsgType::A),v{0.0f}{};
  void toString(std::ostream &os) const override;

  float v{};
};

inline std::ostream &operator<<(std::ostream &os, const A &msg) {
  msg.toString(os); // dispatches to derived class
  return os;
}