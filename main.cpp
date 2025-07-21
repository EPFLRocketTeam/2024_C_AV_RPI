#include <iostream>
#include <memory>
#include <chrono>
#include <thread>
#include "sensors.h"
#include "data.h"
#include "av_state.h"
int main()
{
    try
    {
        // === Initialization ===
        std::cout << "[INFO] Setting AV state to READY.\n";
        DataDump init_dump = Data::get_instance().get();
        init_dump.av_state = State::READY;
        Data::get_instance().write(Data::AV_STATE, &init_dump.av_state);
        std::cout << "[INFO] AV state set to READY.\n";

        std::cout << "[INFO] Initializing sensors...\n";
        std::unique_ptr<Sensors> sensors = std::make_unique<Sensors>();
        std::cout << "[INFO] Sensors initialized.\n";

        std::cout << "[INFO] Beginning simulation loop...\n";

        // === Simulation Loop ===
        for (int i = 0; i < 100; ++i)
        {
            std::cout << "\n[INFO] --- Loop iteration " << i + 1 << " ---\n";

            // Timestamp
            auto now = std::chrono::steady_clock::now();
            uint32_t timestamp_gen = static_cast<uint32_t>(
                std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count());
            Data::get_instance().write(Data::AV_TIMESTAMP, &timestamp_gen);
            std::cout << "[DEBUG] Timestamp written: " << timestamp_gen << "\n";

            // Sensor update
            DataDump dump = Data::get_instance().get();
            sensors->check_policy(dump, 100);
                
            // Fetch updated values
            dump = Data::get_instance().get();

            // === ADXL375 ===
            std::cout << "[ADXL1] X: " << dump.sens.adxl.x
                      << ", Y: " << dump.sens.adxl.y
                      << ", Z: " << dump.sens.adxl.z << "\n";

            std::cout << "[ADXL2] X: " << dump.sens.adxl_aux.x
                      << ", Y: " << dump.sens.adxl_aux.y
                      << ", Z: " << dump.sens.adxl_aux.z << "\n";

            // === BMI088 ===
            std::cout << "[BMI1 ACC] X: " << dump.sens.bmi_accel.x
                      << ", Y: " << dump.sens.bmi_accel.y
                      << ", Z: " << dump.sens.bmi_accel.z << "\n";

            std::cout << "[BMI2 ACC] X: " << dump.sens.bmi_aux_accel.x
                      << ", Y: " << dump.sens.bmi_aux_accel.y
                      << ", Z: " << dump.sens.bmi_aux_accel.z << "\n";

            std::cout << "[BMI1 GYRO] X: " << dump.sens.bmi_gyro.x
                      << ", Y: " << dump.sens.bmi_gyro.y
                      << ", Z: " << dump.sens.bmi_gyro.z << "\n";

            std::cout << "[BMI2 GYRO] X: " << dump.sens.bmi_aux_gyro.x
                      << ", Y: " << dump.sens.bmi_aux_gyro.y
                      << ", Z: " << dump.sens.bmi_aux_gyro.z << "\n";

            // === Barometric Pressure ===
            std::cout << "[BMP1] Pressure: " << dump.sens.bmp.pressure
                      << ", Temp: " << dump.sens.bmp.temperature << "\n";

            std::cout << "[BMP2] Pressure: " << dump.sens.bmp_aux.pressure
                      << ", Temp: " << dump.sens.bmp_aux.temperature << "\n";

            // === Voltages & Temperature === //TODO: to uncomment once they are integrated 
            std::cout << "[TMP1075] FC Temperature: " << dump.av_fc_temp << " °C\n";
            std::cout << "[INA228] LPB Voltage: " << dump.bat.lpb_voltage << " V\n";
            std::cout << "[INA228] HPB Voltage: " << dump.bat.hpb_voltage << " V\n";
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        std::cout << "\n[INFO] Sensor test loop completed successfully.\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "[ERROR] Exception in sensors test: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
