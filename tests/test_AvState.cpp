
#include "../include/flightControl/AvState.h"
#include "../include/flightControl/AvData.h"
#include <cassert>


// Add more test cases as needed
/*int main(int argc, char** argv) {
    AvState avState;
    AvData avData;

    while (true){
        avState.update(avData);
    }
    
}

avState.update(avData);
    //print in console the current state
    std::cout << "Current state: " << avState.stateToString(avState.getCurrentState()) << std::endl;
    return 0;*/

    
int main() {
    FILE *pipe = popen("python3 server.py", "w");
    if (!pipe) {
        std::cerr << "Error: Couldn't open pipe to Python script." << std::endl;
        return EXIT_FAILURE;
    }
    AvData avData;
    AvState avState;

    while (true) {
        // Request data from Python
        fprintf(pipe, "request\n");
        fflush(pipe);

        // Receive response from Python
        char response[256];
       
        fgets(response, sizeof(response), pipe);
        std::cout << "Received from Python: " << std::endl;
        avData.update(response);

        //print the data in console
        std::cout << "Pressure: " << avData.pressure << std::endl;

        avState.update(avData);
        assert(avState.getCurrentState() == State::IDLE);
        
        //print in console the current state
        std::cout << "Current state: " << avState.stateToString(avState.getCurrentState()) << std::endl;

        // Break the loop if the response indicates the end
        if (strcmp(response, "end\n") == 0) {
            break;
        }
    }

    pclose(pipe);
    return EXIT_SUCCESS;
}