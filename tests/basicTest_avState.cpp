
#include "../include/flightControl/AvState.h"
#include "../include/flightControl/AvData.h"
#include <cassert>


// Add more test cases as needed
int main(int argc, char** argv) {
    AvState avState;
    AvData avData;

   

    // print current state
    std::cout << "Current state: " << avState.stateToString(avState.getCurrentState()) << std::endl;

    avState.update(avData);
    
}

