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

    // Check if the data is being written correctly
    assert(dump.sens.adxl.x == 0.0);
    assert(dump.sens.adxl.y == 0.0);
    assert(dump.sens.adxl.z == 0.0);

    std::cout << "Simulation mode test passed!\n";
}

int main(int argc, char **argv)
{
    Data &goatData = Data::get_instance();

    test_simulation_mode();

    std::cout << "All sensors tests passed!\n";
    return 0;
}