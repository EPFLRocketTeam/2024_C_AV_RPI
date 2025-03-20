
#include <iostream>
#include <data.h>
#include <av_state.h>
#include <sensors.h>

#include <telecom.h>
#include <unistd.h>

int main(void){
    AvState state;
    DataDump dump = Data::get_instance().get();
    std::cout << "Current state: " << state.stateToString(state.getCurrentState()) << std::endl;
    Telecom telecom;
    Sensors sens;
    telecom.begin();
    
    //here we should have a loop that sends in telemetry a counter to show that the system is working

    for (int i = 0; i < 10; i++)
    {
        dump.av_state = state.getCurrentState();
        telecom.check_policy(dump);
    }
    
    //sleep for 1 second
    sleep(1);

    
    

    
    while (1)
    {
        dump = Data::get_instance().get();

        sens.check_policy(dump);
        
    }
    

    return 0;
}