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
    assert(dump.stat.adxl_status == 1);
    assert(dump.stat.adxl_aux_status == 1);
    assert(dump.stat.bmi_accel_status == 1);
    assert(dump.stat.bmi_gyro_status == 1);
    assert(dump.stat.bmp_status == 1);
    assert(dump.stat.bmp_aux_status == 1);
    assert(dump.stat.adxl1_x == 0.0);
    assert(dump.stat.adxl1_y == 0.0);
    assert(dump.stat.adxl1_z == 0.0);
    assert(dump.stat.adxl2_x == 0.0);
    assert(dump.stat.adxl2_y == 0.0);
    assert(dump.stat.adxl2_z == 0.0);
    assert(dump.stat.bmi1_accel_x == 0.0);
    assert(dump.stat.bmi1_accel_y == 0.0);
    assert(dump.stat.bmi1_accel_z == 0.0);
    assert(dump.stat.bmi1_gyro_x == 0.0);
    assert(dump.stat.bmi1_gyro_y == 0.0);
    assert(dump.stat.bmi1_gyro_z == 0.0);
    assert(dump.stat.bmi2_accel_x == 0.0);
    assert(dump.stat.bmi2_accel_y == 0.0);
    assert(dump.stat.bmi2_accel_z == 0.0);
    assert(dump.stat.bmi2_gyro_x == 0.0);
    assert(dump.stat.bmi2_gyro_y == 0.0);
    assert(dump.stat.bmi2_gyro_z == 0.0);
    assert(dump.stat.bmp1_pressure == 0.0);
    assert(dump.stat.bmp1_temperature == 0.0);
    assert(dump.stat.bmp2_pressure == 0.0);
    assert(dump.stat.bmp2_temperature == 0.0);

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