#include <string.h>
#include <iostream>
#include <string>
#include "types.hpp"
int main(int argc, char** argv) {
    using namespace std;
    using namespace GLProtcol;



    
    Target t(RobotID (Team::Blue, 128));
    ostringstream oss;
    t.write(oss);

    string buf;
    copy(oss.str().begin(), oss.str().begin() + oss.str().size(), back_inserter(buf));

    for (auto it = buf.begin(); it != buf.begin() + buf.size(); it++) {
        cout << (int)*it << " ";
    }
    cout << endl;

    istringstream iss(buf, std::istringstream::binary);
    auto l = Target::read(iss).robot.value();
    //RobotID::read(iss);
    cout << (int)l.team << "," <<(int) l.number << endl;
}
