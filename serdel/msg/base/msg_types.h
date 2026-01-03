#include <cstdint>
#include <ostream>
#pragma once
enum class MsgType : uint8_t{
    Base,A,B,C,D,E
};

inline std::ostream& operator<<(std::ostream& os, MsgType type) {
    switch (type) {
        case MsgType::Base: os << "Base"; break;
        case MsgType::A:    os << "A"; break;
        case MsgType::B:    os << "B"; break;
        case MsgType::C:    os << "C"; break;
        case MsgType::D:    os << "D"; break;
        case MsgType::E:    os << "E"; break;
        default:            os << "Unknown"; break;
    }
    return os;
}