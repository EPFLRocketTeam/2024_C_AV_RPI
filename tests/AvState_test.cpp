
#include "av_state.h"
#include <cassert>

#define assert_s(expectedState,fsm)                                                           \
    do {                                                                                   \
        if ((fsm.getCurrentState()) != (expectedState)) {                                            \
            std::cerr << "Assertion failed: Expected state: " << fsm.stateToString(expectedState)            \
                      << ", but got state: " << fsm.stateToString(fsm.getCurrentState()) << std::endl;\
            assert(fsm.getCurrentState() == expectedState);                                      \
        }                                                                                  \
    } while (0)

// ================== Transition functions ==================

// Function to trigger the INIT -> CALIBRATION transition
void initToCalibration(AvState fsm, DataDump dump) {
    dump.telemetry_cmd.id = CMD_ID::AV_CMD_CALIBRATE;
    fsm.update(dump);
}

// Function to trigger the ERRORGROUND -> INIT transition
void errorGroundToInit(AvState fsm, DataDump dump) {
    dump.telemetry_cmd.id = CMD_ID::AV_CMD_RECOVER;
    fsm.update(dump);
}

// Function to trigger the CALIBRATION -> MANUAL transition
void calibrationToManual(AvState fsm, DataDump dump) {
    dump.event.calibrated = true;
    fsm.update(dump);
}

// TODO: implement the error() function in av_state.cpp
// // Function to trigger the CALIBRATION -> ERRORGROUND transition
// void calibrationToErrorGround(AvState fsm, DataDump dump) {
//     fsm.update(dump);
// }

// Function to trigger the CALIBRATION -> INIT transition
void calibrationToInit(AvState fsm, DataDump dump) {
    dump.telemetry_cmd.id = CMD_ID::AV_CMD_RECOVER;
    fsm.update(dump);
}

// Function to trigger the MANUAL -> ARMED transition
void manualToArmed(AvState fsm, DataDump dump) {
    dump.telemetry_cmd.id = CMD_ID::AV_CMD_ARM;
    dump.valves.valve1 = 1; 
    dump.valves.valve2 = 1; 
    dump.valves.vent3 = 1; 
    dump.valves.vent4 = 1; 
    dump.prop.fuel_pressure = 1.0;
    dump.prop.LOX_pressure = 1.0;
    fsm.update(dump);
}

// Function to trigger the ARMED -> ERRORGROUND transition
// TODO: add another way to trigger this transition using the safety checks function failing
void armedToErrorGround(AvState fsm, DataDump dump) {
    dump.prop.fuel_pressure = 0;
    dump.prop.LOX_pressure = 0;
    fsm.update(dump);
}

// Function to trigger the ARMED -> READY transition
void armedToReady(AvState fsm, DataDump dump) {
    dump.prop.fuel_pressure = FUEL_PRESSURE_WANTED;
    dump.prop.LOX_pressure = LOX_PRESSURE_WANTED;
    fsm.update(dump);
}

// Function to trigger the READY -> THRUSTSEQUENCE transition
void readyToThrustSequence(AvState fsm, DataDump dump) {
    dump.telemetry_cmd.id = CMD_ID::AV_CMD_IGNITION;
    fsm.update(dump);
}

// Function to trigger the THRUSTSEQUENCE -> ARMED transition
void thrustSequenceToArmed(AvState fsm, DataDump dump) {
    dump.prop.igniter_pressure = IGNITER_PRESSURE_WANTED - 1;
    dump.prop.chamber_pressure = CHAMBER_PRESSURE_WANTED - 1;
    dump.prop.fuel_inj_pressure = INJECTOR_PRESSURE_WANTED_MIN - 1;
    fsm.update(dump);
}

// Function to trigger the THRUSTSEQUENCE -> ERRORFLIGHT transition
void thrustSequenceToErrorFlight(AvState fsm, DataDump dump) {
    dump.telemetry_cmd.id = CMD_ID::AV_CMD_ABORT;
    fsm.update(dump);
}

// Function to trigger the THRUSTSEQUENCE -> LIFTOFF transition
void thrustSequenceToLiftoff(AvState fsm, DataDump dump) {
    dump.event.ignited = true;
    dump.nav.speed.z = SPEED_ZERO + 1;
    dump.nav.altitude = ALTITUDE_ZERO + 1;
    fsm.update(dump);
}

// Function to trigger the LIFTOFF -> ERRORFLIGHT transition
void liftoffToErrorFlight(AvState fsm, DataDump dump) {
    dump.telemetry_cmd.id = CMD_ID::AV_CMD_ABORT;
    fsm.update(dump);
}

// Function to trigger the LIFTOFF -> ASCENT transition
void liftoffToAscent(AvState fsm, DataDump dump) {
    dump.nav.altitude = ALTITUDE_THRESHOLD + 1;
    fsm.update(dump);
}

// Function to trigger the ASCENT -> ERRORFLIGHT transition
void ascentToErrorFlight(AvState fsm, DataDump dump) {
    dump.telemetry_cmd.id = CMD_ID::AV_CMD_ABORT;
    fsm.update(dump);
}

// Function to trigger the ASCENT -> DESCENT transition
void ascentToDescent(AvState fsm, DataDump dump) {
    dump.nav.accel.z = ACCEL_ZERO - 1;
    fsm.update(dump);
}

// Function to trigger the DESCENT -> ERRORFLIGHT transition
void descentToErrorFlight(AvState fsm, DataDump dump) {
    dump.telemetry_cmd.id = CMD_ID::AV_CMD_ABORT;
    dump.telemetry_cmd.id =  CMD_ID::AV_CMD_MANUAL_DEPLOY;
    fsm.update(dump);
}

// Function to trigger the DESCENT -> LANDED transition
void descentToLanded(AvState fsm, DataDump dump) {
    dump.nav.speed.z = SPEED_ZERO - 1;
    fsm.update(dump);
}

// ================== Test functions ==================

// Function to test where there are no errors during the flight
void flightWithoutError() {
    // Instantiate AvState
    AvState fsm;

    // Initialize a DataDump object to simulate different inputs
    DataDump dump;

    assert_s(State::INIT, fsm);
    std::cout << "Initial state is INIT\n";

    initToCalibration(fsm, dump);
    assert_s(State::CALIBRATION, fsm);
    std::cout << "INIT -> CALIBRATION: OK\n";

    calibrationToManual(fsm, dump);
    assert_s(State::MANUAL, fsm);
    std::cout << "CALIBRATION -> MANUAL: OK\n";

    manualToArmed(fsm, dump);
    assert_s(State::ARMED, fsm);
    std::cout << "MANUAL -> ARMED: OK\n";

    armedToReady(fsm, dump);
    assert_s(State::READY, fsm);
    std::cout << "ARMED -> READY: OK\n";

    readyToThrustSequence(fsm, dump);
    assert_s(State::THRUSTSEQUENCE, fsm);
    std::cout << "READY -> THRUSTSEQUENCE: OK\n";

    thrustSequenceToLiftoff(fsm, dump);
    assert_s(State::LIFTOFF, fsm);
    std::cout << "THRUSTSEQUENCE -> LIFTOFF: OK\n";

    liftoffToAscent(fsm, dump);
    assert_s(State::ASCENT, fsm);
    std::cout << "LIFTOFF -> ASCENT: OK\n";
    
    ascentToDescent(fsm, dump);
    assert_s(State::DESCENT, fsm);
    std::cout << "ASCENT -> DESCENT: OK\n";

    descentToLanded(fsm, dump);
    assert_s(State::LANDED, fsm);
    std::cout << "DESCENT -> LANDED: OK\n";

    return;
}

// Function to test if the ERRORGROUND state is triggered from the ARMED state
void errorOnGroundFromArmed() {
    // Instantiate AvState
    AvState fsm;

    // Initialize a DataDump object to simulate different inputs
    DataDump dump;

    initToCalibration(fsm, dump);
    calibrationToManual(fsm, dump);
    assert(fsm.getCurrentState() == State::MANUAL);
    std::cout << "CALIBRATION -> MANUAL: OK\n";

    manualToArmed(fsm, dump);
    assert(fsm.getCurrentState() == State::ARMED);
    std::cout << "MANUAL -> ARMED: OK\n";

    armedToErrorGround(fsm, dump);
    assert(fsm.getCurrentState() == State::ERRORGROUND);
    std::cout << "ARMED -> ERRORGROUND: Error is triggered as expected\n";

    return;
}

// TODO: add later on
// // Function to test if the ERRORGROUND state is triggered from the CALIBRATION state
// void errorOnGroundFromCalibration() {
//     // Instantiate AvState
//     AvState fsm;

//     // Initialize a DataDump object to simulate different inputs
//     DataDump dump;

//     initToCalibration(fsm, dump);
//     calibrationToErrorGround(fsm, dump);
//     assert(fsm.getCurrentState() == State::ERRORGROUND);
//     std::cout << "CALIBRATION -> ERRORGROUND: Error is triggered as expected\n";

//     return;
// }

// Function to test if the ERRORFLIGHT state is triggered from the THRUSTSEQUENCE state
void errorInFlightFromThrustSequence() {
    // Instantiate AvState
    AvState fsm;

    // Initialize a DataDump object to simulate different inputs
    DataDump dump;

    initToCalibration(fsm, dump);
    calibrationToManual(fsm, dump);
    manualToArmed(fsm, dump);
    armedToReady(fsm, dump);
    readyToThrustSequence(fsm, dump);
    thrustSequenceToErrorFlight(fsm, dump);
    assert(fsm.getCurrentState() == State::ERRORFLIGHT);
    std::cout << "THRUSTSEQUENCE -> ERRORFLIGHT: Error is triggered as expected\n";

    return;
}

// Function to test if the ERRORFLIGHT state is triggered from the LIFTOFF state
void errorInFlightFromLiftoff() {
    // Instantiate AvState
    AvState fsm;

    // Initialize a DataDump object to simulate different inputs
    DataDump dump;

    initToCalibration(fsm, dump);
    calibrationToManual(fsm, dump);
    manualToArmed(fsm, dump);
    armedToReady(fsm, dump);
    readyToThrustSequence(fsm, dump);
    thrustSequenceToLiftoff(fsm, dump);
    liftoffToErrorFlight(fsm, dump);
    assert(fsm.getCurrentState() == State::ERRORFLIGHT);
    std::cout << "LIFTOFF -> ERRORFLIGHT: Error is triggered as expected\n";

    return;
}

// Function to test if the ERRORFLIGHT state is triggered from the ASCENT state
void errorInFlightFromAscent() {
    // Instantiate AvState
    AvState fsm;

    // Initialize a DataDump object to simulate different inputs
    DataDump dump;

    initToCalibration(fsm, dump);
    calibrationToManual(fsm, dump);
    manualToArmed(fsm, dump);
    armedToReady(fsm, dump);
    readyToThrustSequence(fsm, dump);
    thrustSequenceToLiftoff(fsm, dump);
    liftoffToAscent(fsm, dump);
    ascentToErrorFlight(fsm, dump);
    assert(fsm.getCurrentState() == State::ERRORFLIGHT);
    std::cout << "ASCENT -> ERRORFLIGHT: Error is triggered as expected\n";

    return;
}

// Function to test if the ERRORFLIGHT state is triggered from the DESCENT state
void errorInFlightFromDescent() {
    // Instantiate AvState
    AvState fsm;

    // Initialize a DataDump object to simulate different inputs
    DataDump dump;

    initToCalibration(fsm, dump);
    calibrationToManual(fsm, dump);
    manualToArmed(fsm, dump);
    armedToReady(fsm, dump);
    readyToThrustSequence(fsm, dump);
    thrustSequenceToLiftoff(fsm, dump);
    liftoffToAscent(fsm, dump);
    ascentToDescent(fsm, dump);
    descentToErrorFlight(fsm, dump);
    assert(fsm.getCurrentState() == State::ERRORFLIGHT);
    std::cout << "DESCENT -> ERRORFLIGHT: Error is triggered as expected\n";

    return;
}




// Add more test cases as needed

int main(int argc, char** argv) {
    // Instantiate AvState
    // AvState fsm;

    // Initialize a DataDump object to simulate different inputs
    // DataDump dump;

    // We test that all transitions are working as expected in the case of a flight without errors
    flightWithoutError();
    std::cout << "Flight without error: OK\n";

    // We test that the ERRORGROUND state can be triggered from the ARMED state
    errorOnGroundFromArmed();
    std::cout << "Error on ground from ARMED: OK\n";

    // We test that the ERRORFLIGHT state can be triggered from the THRUSTSEQUENCE state
    errorInFlightFromThrustSequence();
    std::cout << "Error in flight from THRUSTSEQUENCE: OK\n";

    // We test that the ERRORFLIGHT state can be triggered from the LIFTOFF state
    errorInFlightFromLiftoff();
    std::cout << "Error in flight from LIFTOFF: OK\n";

    // We test that the ERRORFLIGHT state can be triggered from the ASCENT state
    errorInFlightFromAscent();
    std::cout << "Error in flight from ASCENT: OK\n";

    return 0;
} 