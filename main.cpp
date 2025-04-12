
#include <iostream>
#include <data.h>
#include <memory>
#include <av_state.h>
#include <sensors.h>
#include <optional>
#include <telecom.h>
#include <unistd.h>
#include <trigger_board.h>
#include <chrono>
#include <logger.h>
#include <pigpio.h>
#include <PR_board.h>
#include "config.h"
int main(void)
{
    // Record time of start
    // TODO: check if steady or high resolution clock is better
    auto now = std::chrono::steady_clock::now();
    auto last = now;
    uint32_t timestamp_gen = 0;

    AvState state;
    DataDump dump = Data::get_instance().get();
    PR_board prb = PR_board();
    std::cout << "Current state: " << state.stateToString(state.getCurrentState()) << std::endl;
    // FIXME: on peut pas vrmt catch tt les init sinon on peut pas utiliser la var

    std::unique_ptr<Telecom> telecom;
    try
    {
        telecom = std::make_unique<Telecom>();
        telecom->begin();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error initializing Telecom: " << e.what() << std::endl;
    }

    std::unique_ptr<Sensors> sens;
    try
    {
        sens = std::make_unique<Sensors>();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error initializing Sensors: " << e.what() << std::endl;
    }

    std::optional<TriggerBoard> trigger;

    try
    {
        trigger = TriggerBoard();
    }
    catch (const std::exception &e)
    {
        DataLogger::getInstance().eventConv("Error initializing TriggerBoard: " + std::string(e.what()), timestamp_gen);
    }

    // here we should have a loop that sends in telemetry a counter to show that the system is working

    for (int i = 0; i < 10; i++)
    // in a uint32_t
    {
        last = now;
        now = std::chrono::steady_clock::now();
        timestamp_gen = (uint32_t)std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - last);
        Data::get_instance().write(Data::AV_TIMESTAMP, &timestamp_gen);
        dump = Data::get_instance().get();
        DataLogger::getInstance().conv(dump);

        if (telecom)
            telecom->check_policy(dump, delta.count());
    }

    // sleep for 1 second
    sleep(1);

    gpioWrite(LED_STATUS, 1);

    while (1)
    {
        last = now;
        now = std::chrono::steady_clock::now();
        timestamp_gen = (uint32_t)std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - last);
        Data::get_instance().write(Data::AV_TIMESTAMP, &timestamp_gen);
        dump = Data::get_instance().get();

        state.update(dump);
        prb.check_policy(dump, (uint32_t)delta.count());
        if (sens)
            sens->check_policy(dump, (uint32_t)delta.count());
        if (telecom)
            telecom->check_policy(dump, (uint32_t)delta.count());
        if (trigger)
        {
            try
            {
                trigger->check_policy(dump, (uint32_t)delta.count());
            }

            catch (TriggerBoardException &e)
            {
                DataLogger::getInstance().eventConv(e.what(), timestamp_gen);
            }
        }

        DataLogger::getInstance().conv(dump);
    }

    return 0;
}