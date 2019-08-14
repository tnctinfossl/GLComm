#include <string.h>
#include <iostream>
#include <string>
#include "packet.hpp"
int main(int argc, char** argv) {
    using namespace std;
    using namespace GLProtcol;



    RobotID p(Team::Blue, 128);
    ostringstream oss;
    p.write(oss);

    string buf;
    copy(oss.str().begin(), oss.str().begin() + oss.str().size(), back_inserter(buf));
/*
    for (auto it = buf.begin(); it != buf.begin() + buf.size(); it++) {
        cout << (int)*it << " ";
    }
    cout << endl;
*/
    istringstream iss(buf, std::istringstream::binary);
    auto q = RobotID::read(iss);
    cout << (int)q.team << "," <<(int) q.number << endl;
}
