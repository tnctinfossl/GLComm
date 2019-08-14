#pragma once
#include <stdint.h>

#include <iostream>
#include <memory>
#include <sstream>
#include <vector>
#include "types.hpp"
namespace GLProtcol {


class Robot : public IOStream {
public:
    const uint32_t id;        //ロボットのID
    const float x, y, theta;  //ロボットの絶対位置、角度
    const float accuracy;     //精度
public:
    Robot(int _id, float _x, float _y, float _theta, float _accuracy)
        : id(_id), x(_x), y(_y), theta(_theta), accuracy(_accuracy) {}
    Robot(const Robot&) = default;
    virtual ~Robot() = default;
    virtual void write(std::ostringstream& oss) {
        write_raw(oss, id);
        write_raw(oss, x);
        write_raw(oss, y);
        write_raw(oss, theta);
        write_raw(oss, accuracy);
    }
};

class Packet : public IOStream {
    // Flags
 Team team;
 Direction direction;
    //送信先のロボットについて
 Robot myPosition;
    //周囲のロボットについて
 std::vector<Robot> ownPositions;
};

/*
GoかRustを使えば楽できた気がする
コマンドのパターンマッチがC++では糞すぎる。

書き方の思想は関数型ライク(immutable)

*/

}  // namespace GLProtcol