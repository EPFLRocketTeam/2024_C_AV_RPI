
#include "av_state.h"
#include <cassert>



// Add more test cases as needed

int main(int argc, char** argv) {
    // Instantiate AvState
    AvState fsm;

    // Initialize a DataDump object to simulate different inputs
    DataDump dump;

    // Initial state should be INIT
    fsm.update(dump);
    assert(fsm.getCurrentState() == State::INIT);
    std::cout << "Initial state is INIT \n";
    // Calibrate command should trigger INIT -> CALIBRATION
    dump.telemetry_cmd.id = CMD_ID::AV_CMD_CALIBRATE;
    fsm.update(dump);
    assert(fsm.getCurrentState() == State::CALIBRATION);
    std::cout << "Calibrate command triggers INIT -> CALIBRATION \n";

    // Sensors calibrated should trigger CALIB -> MANUAL
    dump.stat.adxl_status = 1;
    dump.stat.adxl_aux_status = 1;
    dump.stat.bmi_accel_status = 1;
    dump.stat.bmi_aux_accel_status = 1;
    dump.stat.bmi_gyro_status = 1;
    dump.stat.bmi_aux_gyro_status = 1;
    fsm.update(dump);
    assert(fsm.getCurrentState() == State::MANUAL);
    std::cout << "Sensors calibrated triggers CALIB -> MANUAL \n";

    // MANUAL -> ARMED

}
