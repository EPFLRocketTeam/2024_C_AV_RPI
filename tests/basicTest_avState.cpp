
#include "../include/flightControl/AvState.h"

#include <cassert>
#include <data/fakeData.h>


// Add more test cases as needed
int main(int argc, char** argv) {
    // create a new AvState object
    Thresholds thresholds;
    AvState avState(thresholds);

    std::string test = "1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16";

    FakeData data;

    data.update(test);
    DataDump dump = data.dump();

    std::cout << "Altitude: " << dump.sensors_data.altitude << std::endl;



}

