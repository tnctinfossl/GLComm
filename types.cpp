#include "types.hpp"
using namespace std;
namespace GLProtcol {
void Position::write(std::ostringstream& oss) const {
    write_raw(oss, x);
    write_raw(oss, y);
    write_raw(oss, theta);
}

void Target::write(std::ostringstream& oss) const {
    write_raw<uint8_t>(oss, (uint8_t)mode);
    switch (mode) {
        case TargetMode::Position:
            position.value().write(oss);
            break;
        case TargetMode::RobotID:
            robot.value().write(oss);
            break;
    }
}

Target Target::read(std::istringstream& iss) {
    switch (read_as<TargetMode>(iss)) {
        case TargetMode::Position:
            return Target(Position::read(iss));
        case TargetMode::RobotID:
            return Target(RobotID::read(iss));
        default:
            return Target(Position(0, 0, 0));  // Error
    }
}

Command Command::read(std::istringstream& iss) {
    switch (read_as<CommandID>(iss)) {
        case CommandID::Stop:
            return Command(make_shared<CommandBase>(Stop::read(iss)));
        case CommandID::Move:
            return Command(make_shared<CommandBase>(Move::read(iss)));
        case CommandID::Send:
            return Command(make_shared<CommandBase>(Send::read(iss)));
        case CommandID::Receive:
            return Command(make_shared<CommandBase>(Receive::read(iss)));
        case CommandID::Block:
            return Command(make_shared<CommandBase>(Block::read(iss)));
        default:
            return Command(make_shared<CommandBase>(Stop::read(iss)));
    }
}

void Frame::write(std::ostringstream& oss) const {
    write_raw(oss, direction);
    ball.write(oss);
    myRobotID.write(oss);
    myPosition.write(oss);
    write_raw(oss, (uint8_t)robots.size());
    for (const auto& [id, pos] : robots) {
        id.write(oss);
        pos.write(oss);
    }
    command.write(oss);
}
Frame Frame::read(std::istringstream& iss) {
    const auto read_map = [](std::istringstream& iss) {
        const size_t size = read_as<uint8_t>(iss);
        std::map<RobotID, Position> result;
        for (size_t i = 0; i < size; i++) {
            auto key = RobotID::read(iss);
            auto value = Position::read(iss);
            result.insert_or_assign(key, value);
        }
        return result;
    };
    return Frame{read_as<Direction>(iss), Ball::read(iss), RobotID::read(iss),
                 Position::read(iss),     read_map(iss),   Command::read(iss)};
}

};  // namespace GLProtcol