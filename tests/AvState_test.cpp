
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
    dump.prop.fuel_inj_pressure = INJECTOR_PRESSURE_WANTED_MIN;
    dump.prop.chamber_pressure = CHAMBER_PRESSURE_WANTED;
    fsm.update(dump);
    assert(fsm.getCurrentState() == State::THRUSTSEQUENCE);
    std::cout << "Ignition command triggers ARMED -> THRUSTSEQUENCE\n";


    // Simulate too low fuel pressure
    dump.event.ignited = true;
    dump.nav.speed.z = SPEED_ZERO + 1;
    dump.nav.altitude = ALTITUDE_ZERO + 1;
    dump.prop.fuel_inj_pressure = INJECTOR_PRESSURE_WANTED_MIN*0.9;
    fsm.update(dump);
    assert(fsm.getCurrentState() == State::ARMED);
    std::cout << "Low fuel pressure triggers THRUSTSEQUENCE -> ARMED\n";
    dump.prop.fuel_inj_pressure = INJECTOR_PRESSURE_WANTED_MIN;
    fsm.update(dump);
    assert(fsm.getCurrentState() == State::THRUSTSEQUENCE);

    // Simulate too low chamber pressure
    dump.prop.chamber_pressure = CHAMBER_PRESSURE_WANTED*0.9;
    fsm.update(dump);
    assert(fsm.getCurrentState() == State::ARMED);
    std::cout << "Low chamber pressure triggers THRUSTSEQUENCE -> ARMED\n";
    dump.prop.chamber_pressure = CHAMBER_PRESSURE_WANTED;
    fsm.update(dump);
    assert(fsm.getCurrentState() == State::THRUSTSEQUENCE);

    // Simulate too low igniter pressure
    dump.prop.igniter_pressure = IGNITER_PRESSURE_WANTED*0.9;
    fsm.update(dump);
    assert(fsm.getCurrentState() == State::ARMED);
    std::cout << "Low igniter pressure triggers THRUSTSEQUENCE -> ARMED\n";
    dump.prop.igniter_pressure = IGNITER_PRESSURE_WANTED;
    fsm.update(dump);
    assert(fsm.getCurrentState() == State::THRUSTSEQUENCE);

    // Simulate not ignited 
    dump.event.ignited = false;
    fsm.update(dump);
    assert(fsm.getCurrentState() == State::THRUSTSEQUENCE);
    std::cout << "Not ignited triggers THRUSTSEQUENCE -> THRUSTSEQUENCE\n";

    // Simulate THRUSTSEQUENCE -> LIFTOFF
    dump.event.ignited = true;
    dump.nav.accel.z = ACCEL_ZERO + 1;
    fsm.update(dump);
    assert(fsm.getCurrentState() == State::LIFTOFF);
    std::cout << "Engine ignition triggers THRUSTSEQUENCE -> LIFTOFF\n";

    // Simulate LIFTOFF -> LIFTOFF 
    fsm.update(dump);
    assert(fsm.getCurrentState() == State::LIFTOFF);
    std::cout << "Threshold not reached triggers LIFTOFF -> LIFTOFF\n";


    // Simulate LIFTOFF -> ASCENT
    dump.nav.altitude = ALTITUDE_THRESHOLD + 1;
    fsm.update(dump);
    assert(fsm.getCurrentState() == State::ASCENT);
    std::cout << "Altitude threshold triggers LIFTOFF -> ASCENT\n";
    
    // Simulate ASCENT -> ASCENT
    fsm.update(dump);
    assert(fsm.getCurrentState() == State::ASCENT);
    std::cout << "Threshold not reached triggers ASCENT -> ASCENT\n";

    // Simulate ASCENT -> DESCENT
    dump.nav.accel.z = ACCEL_ZERO - 1;
    fsm.update(dump);
    assert(fsm.getCurrentState() == State::DESCENT);
    std::cout << "Negative acceleration triggers ASCENT -> DESCENT\n";

    // Simulate DESCENT -> DESCENT
    fsm.update(dump);
    assert(fsm.getCurrentState() == State::DESCENT);
    std::cout << "Threshold not reached triggers DESCENT -> DESCENT\n";

    // Simulate DESCENT -> DESCENT 
    dump.nav.speed.z = SPEED_ZERO - 1;
    fsm.update(dump);
    assert(fsm.getCurrentState() == State::DESCENT);
    std::cout << "Too high pressure triggers DESCENT -> LANDED\n";

    // Simulate DESCENT -> LANDED
    dump.prop.chamber_pressure = CHAMBER_PRESSURE_ZERO - 1;
    fsm.update(dump);
    assert(fsm.getCurrentState() == State::LANDED);
    std::cout << "Low chamber pressure + low speed triggers DESCENT -> LANDED\n";

    return 0;
} 
