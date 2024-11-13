
#include "av_state.h"



// Add more test cases as needed

int main(int argc, char** argv) {

    
}

#include "av_state.h"
#include <cassert>



// Add more test cases as needed

int main(int argc, char** argv) {
    // Instantiate AvState
    AvState fsm;

    // Initialize a DataDump object to simulate different inputs
    DataDump dump;

    // Initial state should be INIT
    assert(fsm.getCurrentState() == State::INIT);
    std::cout << "Initial state is INIT\n";

    // Simulate INIT -> CALIBRATION
    dump.telemetry_cmd.id = CMD_ID::AV_CMD_CALIBRATE;
    fsm.update(dump);
    assert(fsm.getCurrentState() == State::CALIBRATION);
    std::cout << "Calibrate command triggers INIT -> CALIBRATION\n";

    // Simulate CALIBRATION -> MANUAL (Sensors calibrated)
    dump.event.calibrated = true;
    fsm.update(dump);
    assert(fsm.getCurrentState() == State::MANUAL);
    std::cout << "Sensors calibrated triggers CALIBRATION -> MANUAL\n";

    // Simulate MANUAL -> ARMED
    dump.telemetry_cmd.id = CMD_ID::AV_CMD_ARM;
    dump.valves.valve1 = 1; 
    dump.valves.valve2 = 1; 
    dump.valves.vent3 = 1; 
    dump.valves.vent4 = 1; 
    dump.prop.fuel_pressure = 1.0;
    dump.prop.LOX_pressure = 1.0;
    fsm.update(dump);
    assert(fsm.getCurrentState() == State::ARMED);
    std::cout << "Arm command triggers MANUAL -> ARMED\n";

    // Simulate ARMED -> THRUSTSEQUENCE
    dump.telemetry_cmd.id = CMD_ID::AV_CMD_IGNITION;
    dump.prop.fuel_inj_pressure = IGNITER_PRESSURE_WANTED;
    dump.prop.chamber_pressure = CHAMBER_PRESSURE_WANTED;
    fsm.update(dump);
    assert(fsm.getCurrentState() == State::THRUSTSEQUENCE);
    std::cout << "Ignition command triggers ARMED -> THRUSTSEQUENCE\n";

    // Simulate THRUSTSEQUENCE -> LIFTOFF
    dump.event.ignited = true;
    dump.nav.speed.z = SPEED_ZERO + 1;
    dump.nav.altitude = ALTITUDE_ZERO + 1;
    fsm.update(dump);
    assert(fsm.getCurrentState() == State::LIFTOFF);
    std::cout << "Engine ignition triggers THRUSTSEQUENCE -> LIFTOFF\n";

    // // Simulate LIFTOFF -> ASCENT
    // dump.nav.altitude += 100; // Rocket ascending
    // dump.nav.speed.z += 10;
    // fsm.update(dump);
    // assert(fsm.getCurrentState() == State::ASCENT);
    // std::cout << "Ascent dynamics trigger LIFTOFF -> ASCENT\n";

    // // Simulate ASCENT -> DESCENT
    // dump.nav.accel.z = ACCEL_ZERO - 1; // Simulate rocket reaching apogee
    // fsm.update(dump);
    // assert(fsm.getCurrentState() == State::DESCENT);
    // std::cout << "Negative acceleration triggers ASCENT -> DESCENT\n";

    // // Simulate DESCENT -> LANDED
    // dump.nav.speed.z = SPEED_ZERO - 1; // Simulate landing dynamics
    // fsm.update(dump);
    // assert(fsm.getCurrentState() == State::LANDED);
    // std::cout << "Zero speed triggers DESCENT -> LANDED\n";

    // // Final check
    // std::cout << "FSM successfully completed a full cycle from INIT to LANDED.\n";

    return 0;
}

