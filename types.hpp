#pragma once
#include <iostream>
#include <optional>
#include <sstream>
#include <variant>
#include <vector>
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

struct Position :public IOStream{
public:
    float x, y, theta;
    Position() = default;
    Position(float _x, float _y, float _theta) : x(_x), y(_y), theta(_theta) {}
    virtual void write(std::ostringstream& oss) const {
        write_raw(oss, x);
        write_raw(oss, y);
        write_raw(oss, theta);
    }

    static Position read(std::istringstream& iss) {
        //{}呼び出しじゃないと正しく評価されない。
        return Position{read_as<float>(iss), read_as<float>(iss), read_as<float>(iss)};
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

enum class Direction : uint8_t { RightToLeft = 0x0, LeftToright = 0x1 };
enum class NamedPosition : uint8_t { None, Center, GoalRight, GoalLeft };
//目的語
struct Target : public IOStream {
    std::variant<NamedPosition, Position, RobotID> select;

    Target() : select(NamedPosition::None) {}
    Target(const Position& p) : select(p) {}
    Target(const RobotID& r) : select(r) {}
    Target(const NamedPosition& p) : select(p) {}

    virtual void write(std::ostringstream& oss) const {
        const uint8_t index = select.index();
        write_raw(oss, index);
        switch (index) {
            case 0:
                write_raw(oss, std::get<NamedPosition>(select));
                break;
            case 1:
                std::get<1>(select).write(oss);
                break;
            case 2:
                std::get<2>(select).write(oss);
                break;
        }
    }

    static Target read(std::istringstream& iss) {
        switch (read_as<uint8_t>(iss)) {
            case 0:
                return Target(read_as<NamedPosition>(iss));
            case 1:
                return Target(Position::read(iss));
            case 2:
                return Target(RobotID::read(iss));
            default:
                return Target();
        }
    }
};

};  // namespace GLProtcol
