
#include "../include/flightControl/AvState.h"
#include "../include/flightControl/AvData.h"


// Add more test cases as needed

int main(int argc, char** argv) {
    AvState avState;
    AvData avData;
    avState.update(avData);
    //print in console the current state
    std::cout << "Current state: " << avState.stateToString(avState.getCurrentState()) << std::endl;
    return 0;
    
}
