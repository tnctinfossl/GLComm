#pragma once
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <sstream>
#include <vector>
/* 作成者　terurin
** メモ　関数型ライクに書くことで、副作用を減らしプロパティの数を削減している。
** 構造はreadmeを参考にすること
*/

namespace GLProtcol {

struct IOStream {
    virtual void write(std::ostringstream&) const {};
};

//ヘルパー関数
template <class T>
void write_raw(std::ostringstream& oss, const T& data) {
    oss.write((const char*)&data, sizeof(T));
}

template <class T>
void write_raw(std::ostringstream& oss, const std::vector<T>& vec) {
    write_raw(oss, vec.size());
    for (const auto& it : vec) {
        write_raw(oss, it);
    }
}

template <class T>
T read_as(std::istringstream& iss) {
    T data;
    iss.read((char*)&data, sizeof(data));
    return data;
}

template <class T>
std::vector<T> read_list(std::istringstream& iss) {
    std::vector<T> vec;
    const size_t size = read_as<uint8_t>(iss);
    vec.clear();
    for (int i = 0; i < size; i++) {
        vec.push_back(read_as<T>(iss));
    }
    return vec;
}

struct Position : public IOStream {
public:
    float x, y, theta;
    Position() = default;
    Position(float _x, float _y, float _theta) : x(_x), y(_y), theta(_theta) {}
    virtual void write(std::ostringstream& oss) const;
    static Position read(std::istringstream& iss) {
        return Position{read_as<float>(iss), read_as<float>(iss),
                        read_as<float>(iss)};  //{}呼び出しじゃないと正しく評価されない。
    }
};

enum class Team : uint8_t { Blue = 0x0, Yellow = 0x1 };
struct RobotID {
    Team team;
    uint8_t number;
    RobotID() = default;
    RobotID(Team _team, uint8_t _number) : team(_team), number(_number) {}
    virtual void write(std::ostringstream& oss) const {
        write_raw(oss, team);
        write_raw(oss, number);
    }
    static RobotID read(std::istringstream& iss) {
        return RobotID{read_as<Team>(iss), read_as<uint8_t>(iss)};
    }

    bool operator==(const RobotID& r) const {
        return team == r.team && number == r.number;
    }

    bool operator!=(const RobotID& r) const {
        return team != r.team || number != r.number;
    }
    // map実装用
    bool operator<(const RobotID& r) const {
        return team != r.team ? (uint8_t)team < (uint8_t)team : number < r.number;
    }
};

struct Ball : public IOStream {
    const float x, y;
    Ball(float _x, float _y) : x(_x), y(_y) {}
    virtual void write(std::ostringstream& oss) const {
        write_raw(oss, x);
        write_raw(oss, y);
    }
    static Ball read(std::istringstream& iss) {
        return Ball{read_as<float>(iss), read_as<float>(iss)};
    }
};

enum class Direction : uint8_t { RightToLeft = 0x0, LeftToright = 0x1 };
enum class TargetMode : uint8_t { Position, RobotID };
//目的語
struct Target : public IOStream {
    const TargetMode mode{TargetMode::Position};
    const std::optional<Position> position{std::nullopt};
    const std::optional<RobotID> robot{std::nullopt};

    Target(const Position& p) : mode(TargetMode::Position), position(p) {}
    Target(const RobotID& r) : mode(TargetMode::RobotID), robot(r) {}

    virtual void write(std::ostringstream& oss) const;
    static Target read(std::istringstream& iss);
};

//操作
//抽象クラス
enum class CommandID : uint8_t { Stop, Move, Send, Receive, Block };

struct CommandBase : public IOStream {
    const CommandID commandID;

protected:
    CommandBase(const CommandID _id) : commandID(_id) {}
};

struct Stop : public CommandBase {
public:
    Stop() : CommandBase(CommandID::Stop) {}
    virtual void write(std::ostringstream& oss) const {}
    static Stop read(std::istringstream& iss) {
        return Stop();
    }
};

struct Move : public CommandBase {
public:
    const Target target;
    Move(const Target& _target) : CommandBase(CommandID::Move), target(_target) {}
    virtual void write(std::ostringstream& oss) const {
        target.write(oss);
    }
    static Move read(std::istringstream& iss) {
        return Move(Target::read(iss));
    }
};

struct Send : public CommandBase {
    const Target target;
    Send(const Target& _target) : CommandBase(CommandID::Send), target(_target) {}
    virtual void write(std::ostringstream& oss) const {
        target.write(oss);
    }
    static Send read(std::istringstream& iss) {
        return Send{Target::read(iss)};
    }
};

struct Receive : public CommandBase {
    const Target target;
    Receive(const Target& _target) : CommandBase(CommandID::Receive), target(_target) {}
    virtual void write(std::ostringstream& oss) const {
        target.write(oss);
    }
    static Receive read(std::istringstream& iss) {
        return Receive(Target::read(iss));
    }
};

struct Block : public CommandBase {
    const Target target;
    Block(const Target& _target) : CommandBase(CommandID::Block), target(_target) {}
    virtual void write(std::ostringstream& oss) const {
        target.write(oss);
    }
    static Block read(std::istringstream& iss) {
        return Block(Target::read(iss));
    }
};

struct Command : public IOStream {
    const std::shared_ptr<CommandBase> command;
    Command(std::shared_ptr<CommandBase> _command) : command(_command) {}
    virtual void write(std::ostringstream& oss) const {
        write_raw<uint8_t>(oss, (uint8_t)command->commandID);
        command->write(oss);
    }
    static Command read(std::istringstream& iss);
};

struct Frame : public IOStream {
    const Direction direction;
    const Ball ball;
    const RobotID myRobotID;
    const Position myPosition;
    const std::map<RobotID, Position> robots;
    const Command command;
    Frame(Direction _direction,
          const Ball& _ball,
          const RobotID& _myRobotID,
          const Position& _myPosition,
          const std::map<RobotID, Position>& _robots,
          const Command& _command)
        : direction(_direction),
          ball(_ball),
          myRobotID(_myRobotID),
          myPosition(_myPosition),
          robots(_robots),
          command(_command) {}
    virtual void write(std::ostringstream&) const;
    static Frame read(std::istringstream&);
};

};  // namespace GLProtcol
