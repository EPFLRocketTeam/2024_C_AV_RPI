
#include <iostream>
#include <data.h>
#include <av_state.h>
#include <sensors.h>

#include <telecom.h>
#include <unistd.h>
#include <trigger_board.h>
#include <chrono>
#include <logger.h>
int main(void){
    // Record time of start
    //TODO: check if steady or high resolution clock is better
    auto now = std::chrono::steady_clock::now();
    auto last = now;
    uint32_t timestamp_gen = 0;


    AvState state;
    DataDump dump = Data::get_instance().get();
    std::cout << "Current state: " << state.stateToString(state.getCurrentState()) << std::endl;
    //FIXME: on peut pas vrmt catch tt les init sinon on peut pas utiliser la var
    Telecom telecom;
    Sensors sens;
    telecom.begin();
    TriggerBoard trigger;
    
    //here we should have a loop that sends in telemetry a counter to show that the system is working

    for (int i = 0; i < 10; i++)
    //in a uint32_t
    {   last = now;
        now = std::chrono::steady_clock::now();
        timestamp_gen = (uint32_t)std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - last);
        Data::get_instance().write(Data::AV_TIMESTAMP, &timestamp_gen);

        dump.av_state = state.getCurrentState();
        telecom.check_policy(dump,delta.count());
    }
    
    //sleep for 1 second
    sleep(1);

    
    

    
    while (1)
    {
        last = now;
        now = std::chrono::steady_clock::now();
        timestamp_gen = (uint32_t)std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - last);
        Data::get_instance().write(Data::AV_TIMESTAMP, &timestamp_gen);
        dump = Data::get_instance().get();

        state.update(dump);
        sens.check_policy(dump,(uint32_t)delta.count());
        telecom.check_policy(dump,(uint32_t)delta.count());
        trigger.check_policy(dump,(uint32_t)delta.count());
        
        DataLogger::getInstance().conv(dump);

    

    
        
    }
    

    return 0;
}