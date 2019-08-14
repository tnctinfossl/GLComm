#pragma once
#include "types.hpp"
namespace GLProtcol::Command {

enum class CommandID : uint8_t {
    Stop,
    Move,
};

//抽象クラス
class Base : public GLProtcol::IOStream {
public:
    const CommandID id;
    Base(CommandID _id) : id(_id) {}
};

class Stop : public Base {
public:
    Stop() : Base(CommandID::Stop) {}
};

class Move : public Base {
public:
    
};





}  // namespace GLProtcol::Command