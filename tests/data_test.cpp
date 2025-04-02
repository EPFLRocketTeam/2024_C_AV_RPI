#include "sensors.h"
#include "data.h"
#include <cassert>
#include <iostream>

void test_simulation_mode()
{
    std::cout << "Testing simulation mode...\n";

    Sensors sensors;
    bool update_result = sensors.update();
    assert(update_result == true);
    
    DataDump dump = Data::get_instance().get();
    assert(Data::get_instance().is_written(Data::NAV_SENSOR_ADXL1_DATA));
    assert(Data::get_instance().is_written(Data::NAV_SENSOR_BMI1_ACCEL_DATA));
    assert(Data::get_instance().is_written(Data::NAV_SENSOR_BMP1_DATA));
    
    std::cout << "Simulation mode test passed!\n";
}

void test_sensor_status()
{
    std::cout << "Testing sensor status updates...\n";

    Sensors sensors;
    sensors.update_status();
    DataDump dump = Data::get_instance().get();
    assert(dump.stat.adxl_status == 1);
    assert(dump.stat.adxl_aux_status == 1);
    assert(dump.stat.bmi_accel_status == 1);
    assert(dump.stat.bmi_gyro_status == 1);
    //assert(dump.stat.bmp_status == 1);
    std::cout << "Sensor status test passed!\n";
}

int main(int argc, char **argv)
{
    Data &goatData = Data::get_instance();

    test_simulation_mode();
    test_sensor_status();

    std::cout << "All sensors tests passed!\n";
    return 0;
}