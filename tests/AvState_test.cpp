
#include "av_state.h"
#include <cassert>

#define assert_s(expectedState,fsm)                                                           \
    do {                                                                                   \
        if ((fsm.getCurrentState()) != (expectedState)) {                                            \
            std::cerr << "Assertion failed: Expected state: " << fsm.stateToString(expectedState)            \
                      << ", but got state: " << fsm.stateToString(fsm.getCurrentState()) << std::endl;\
            assert(fsm.getCurrentState() == expectedState);                                      \
        }                                          \
        else {                                                                                \
            std::cout << "Current state is " << fsm.stateToString(fsm.getCurrentState()) << std::endl; \
        }                                        \
    } while (0) 

// ================== Transition functions ==================

// Verify that the state has not changed when nothing changes
void sameState(AvState &fsm, DataDump &dump) {
    State previous_state = fsm.getCurrentState();
    fsm.update(dump);
    assert_s(previous_state, fsm);
    std::cout << "State remains the same\n";
}

// Function to trigger the INIT -> CALIBRATION transition
void initToCalibration(AvState &fsm, DataDump &dump) {
    dump.telemetry_cmd.id = CMD_ID::AV_CMD_CALIBRATE;
    fsm.update(dump);
    assert_s(State::CALIBRATION, fsm);
    sameState(fsm, dump);
}

// Function to trigger the ERRORGROUND -> INIT transition
void errorGroundToInit(AvState &fsm, DataDump &dump) {
    dump.telemetry_cmd.id = CMD_ID::AV_CMD_RECOVER;
    fsm.update(dump);
    assert_s(State::INIT, fsm);
    sameState(fsm, dump);
}

// Function to trigger the CALIBRATION -> MANUAL transition
void calibrationToManual(AvState &fsm, DataDump &dump) {
    dump.event.calibrated = true;
    fsm.update(dump);
    assert_s(State::MANUAL, fsm);
    sameState(fsm, dump);
}

// TODO: implement the error() function in av_state.cpp
// // Function to trigger the CALIBRATION -> ERRORGROUND transition
void calibrationToErrorGround(AvState fsm, DataDump dump) {
    dump.telemetry_cmd.id = CMD_ID::AV_CMD_ABORT;
    fsm.update(dump);
    assert_s(State::ERRORGROUND, fsm);
    sameState(fsm, dump);
}

// Function to trigger the CALIBRATION -> INIT transition
void calibrationToInit(AvState &fsm, DataDump &dump) {
    dump.telemetry_cmd.id = CMD_ID::AV_CMD_RECOVER;
    fsm.update(dump);
    assert_s(State::INIT, fsm);
    sameState(fsm, dump);
}

// Function to trigger the MANUAL -> ARMED transition
void manualToArmed(AvState &fsm, DataDump &dump) {
    dump.telemetry_cmd.id = CMD_ID::AV_CMD_ARM;
    dump.valves.valve1 = 1; 
    dump.valves.valve2 = 1; 
    dump.valves.vent3 = 1; 
    dump.valves.vent4 = 1; 
    fsm.update(dump);
    assert_s(State::ARMED, fsm);
    sameState(fsm, dump);
}

// Function to trigger the ARMED -> ERRORGROUND transition
void armedToErrorGround(AvState &fsm, DataDump &dump) {
    dump.telemetry_cmd.id = CMD_ID::AV_CMD_ABORT;
    dump.event.calibrated = false;
    fsm.update(dump);
    assert_s(State::ERRORGROUND, fsm);
    sameState(fsm, dump);
}

// Function to trigger the ARMED -> READY transition
void armedToReady(AvState &fsm, DataDump &dump) {
    dump.event.armed = true;
    dump.prop.fuel_pressure = FUEL_PRESSURE_WANTED;
    dump.prop.LOX_pressure = LOX_PRESSURE_WANTED;
    fsm.update(dump);
    assert_s(State::READY, fsm);
    sameState(fsm, dump);
}

// Function to trigger the READY -> THRUSTSEQUENCE transition
void readyToThrustSequence(AvState &fsm, DataDump &dump) {
    dump.telemetry_cmd.id = CMD_ID::AV_CMD_IGNITION;
    fsm.update(dump);
    assert_s(State::THRUSTSEQUENCE, fsm);
    //TODO check that this is correct
    sameState(fsm, dump);
}

// Function to trigger the THRUSTSEQUENCE -> ARMED transition
void thrustSequenceToArmed(AvState &fsm, DataDump &dump) {
    dump.event.ignition_failed = true;
    dump.event.armed = false;
    fsm.update(dump);
    assert_s(State::ARMED, fsm);
    sameState(fsm, dump);
}

// Function to trigger the THRUSTSEQUENCE -> ERRORFLIGHT transition
void thrustSequenceToErrorFlight(AvState &fsm, DataDump &dump) {
    dump.telemetry_cmd.id = CMD_ID::AV_CMD_ABORT;
    fsm.update(dump);
    assert_s(State::ERRORFLIGHT, fsm);
    sameState(fsm, dump);
}

// Function to trigger the THRUSTSEQUENCE -> LIFTOFF transition
void thrustSequenceToLiftoff(AvState &fsm, DataDump &dump) {
    dump.event.ignited = true;
    dump.nav.speed.z = SPEED_ZERO + 1;
    dump.nav.altitude = ALTITUDE_ZERO + 1;
    dump.prop.igniter_pressure = IGNITER_PRESSURE_WANTED + 1;
    dump.prop.chamber_pressure = CHAMBER_PRESSURE_WANTED + 1;
    dump.prop.fuel_inj_pressure = INJECTOR_PRESSURE_WANTED_MIN + 1;
    fsm.update(dump);
    assert_s(State::LIFTOFF, fsm);
    sameState(fsm, dump);
}

// Function to trigger the LIFTOFF -> ERRORFLIGHT transition
void liftoffToErrorFlight(AvState &fsm, DataDump &dump) {
    dump.telemetry_cmd.id = CMD_ID::AV_CMD_ABORT;
    fsm.update(dump);
    assert_s(State::ERRORFLIGHT, fsm);
    sameState(fsm, dump);
}

// Function to trigger the LIFTOFF -> ASCENT transition
void liftoffToAscent(AvState &fsm, DataDump &dump) {
    dump.nav.altitude = ALTITUDE_THRESHOLD + 1;
    fsm.update(dump);
    assert_s(State::ASCENT, fsm);
    sameState(fsm, dump);
}

// Function to trigger the ASCENT -> ERRORFLIGHT transition
void ascentToErrorFlight(AvState &fsm, DataDump &dump) {
    dump.telemetry_cmd.id = CMD_ID::AV_CMD_ABORT;
    fsm.update(dump);
    assert_s(State::ERRORFLIGHT, fsm);
    sameState(fsm, dump);
}

// Function to trigger the ASCENT -> DESCENT transition
void ascentToDescent(AvState &fsm, DataDump &dump) {
    dump.nav.speed.z = SPEED_ZERO * 0.1;
    fsm.update(dump);
    assert_s(State::DESCENT, fsm);
    sameState(fsm, dump);
}

// Function to trigger the DESCENT -> ERRORFLIGHT transition
void descentToErrorFlight(AvState &fsm, DataDump &dump) {
    dump.telemetry_cmd.id = CMD_ID::AV_CMD_ABORT;
    dump.telemetry_cmd.id =  CMD_ID::AV_CMD_MANUAL_DEPLOY;
    fsm.update(dump);
    assert_s(State::ERRORFLIGHT, fsm);
    sameState(fsm, dump);
}

// Function to trigger the DESCENT -> LANDED transition
void descentToLanded(AvState &fsm, DataDump &dump) {
    dump.nav.speed.z = SPEED_ZERO * 0.1;
    dump.nav.speed.x = SPEED_ZERO * 0.1;
    dump.nav.speed.y = SPEED_ZERO * 0.1;
    dump.prop.chamber_pressure = CHAMBER_PRESSURE_ZERO - 1;
    fsm.update(dump);
    assert_s(State::LANDED, fsm);
    sameState(fsm, dump);
}


// ================== Test functions ==================

// Function to test where there are no errors during the flight
void flightWithoutError() {
    // Instantiate AvState
    AvState fsm;

    // Initialize a DataDump object to simulate different inputs
    DataDump dump;

    assert_s(State::INIT, fsm);
    initToCalibration(fsm, dump);
    calibrationToManual(fsm, dump);
    manualToArmed(fsm, dump);
    armedToReady(fsm, dump);
    readyToThrustSequence(fsm, dump);
    thrustSequenceToLiftoff(fsm, dump);
    liftoffToAscent(fsm, dump);
    ascentToDescent(fsm, dump);
    descentToLanded(fsm, dump);
    
    return;
}

// Function to test if the ERRORGROUND state is triggered from the ARMED state
void errorOnGroundFromArmed() {
    // Instantiate AvState
    AvState fsm;

    // Initialize a DataDump object to simulate different inputs
    DataDump dump;

    assert_s(State::INIT, fsm);
    initToCalibration(fsm, dump);
    calibrationToManual(fsm, dump);
    sameState(fsm, dump);
    manualToArmed(fsm, dump);
    armedToErrorGround(fsm, dump);
    
    return;
}

// TODO: add later on
// Function to test if the ERRORGROUND state is triggered from the CALIBRATION state
void errorOnGroundFromCalibration() {
    // Instantiate AvState
    AvState fsm;
    // Initialize a DataDump object to simulate different inputs
    DataDump dump;
    assert_s(State::INIT, fsm);
    initToCalibration(fsm, dump);
    calibrationToErrorGround(fsm, dump);
    return;
}

// Function to test if the ERRORFLIGHT state is triggered from the THRUSTSEQUENCE state
void errorInFlightFromThrustSequence() {
    // Instantiate AvState
    AvState fsm;

    // Initialize a DataDump object to simulate different inputs
    DataDump dump;

    assert_s(State::INIT, fsm);
    initToCalibration(fsm, dump);
    calibrationToManual(fsm, dump);
    manualToArmed(fsm, dump);
    armedToReady(fsm, dump);
    readyToThrustSequence(fsm, dump);
    thrustSequenceToErrorFlight(fsm, dump);

    return;
}

// Function to test if the ERRORFLIGHT state is triggered from the LIFTOFF state
void errorInFlightFromLiftoff() {
    // Instantiate AvState
    AvState fsm;

    // Initialize a DataDump object to simulate different inputs
    DataDump dump;

    assert_s(State::INIT, fsm);
    initToCalibration(fsm, dump);
    calibrationToManual(fsm, dump);
    manualToArmed(fsm, dump);
    armedToReady(fsm, dump);
    readyToThrustSequence(fsm, dump);
    thrustSequenceToLiftoff(fsm, dump);
    liftoffToErrorFlight(fsm, dump);
    
    return;
}

// Function to test if the ERRORFLIGHT state is triggered from the ASCENT state
void errorInFlightFromAscent() {
    // Instantiate AvState
    AvState fsm;

    // Initialize a DataDump object to simulate different inputs
    DataDump dump;

    assert_s(State::INIT, fsm);
    initToCalibration(fsm, dump);
    calibrationToManual(fsm, dump);
    manualToArmed(fsm, dump);
    armedToReady(fsm, dump);
    readyToThrustSequence(fsm, dump);
    thrustSequenceToLiftoff(fsm, dump);
    liftoffToAscent(fsm, dump);
    ascentToErrorFlight(fsm, dump);
    
    return;
}

// Function to test if the ERRORFLIGHT state is triggered from the DESCENT state
void errorInFlightFromDescent() {
    // Instantiate AvState
    AvState fsm;

    // Initialize a DataDump object to simulate different inputs
    DataDump dump;

    assert_s(State::INIT, fsm);
    initToCalibration(fsm, dump);
    calibrationToManual(fsm, dump);
    manualToArmed(fsm, dump);
    armedToReady(fsm, dump);
    readyToThrustSequence(fsm, dump);
    thrustSequenceToLiftoff(fsm, dump);
    liftoffToAscent(fsm, dump);
    ascentToDescent(fsm, dump);
    descentToErrorFlight(fsm, dump);

    return;
}

// Function to test whether when the pressure is too low the transition from THRUSTSEQUENCE to ARMED is triggered
void pressureTooLow() {
    // Instantiate AvState
    AvState fsm;

    // Initialize a DataDump object to simulate different inputs
    DataDump dump;

    assert_s(State::INIT, fsm);
    initToCalibration(fsm, dump);
    calibrationToManual(fsm, dump);
    manualToArmed(fsm, dump);
    armedToReady(fsm, dump);
    readyToThrustSequence(fsm, dump);
    thrustSequenceToArmed(fsm, dump);
    
    return;
}

// Function to test whether recovering from an error on the ground works
void recoverFromErrorGround() {
    // Instantiate AvState
    AvState fsm;

    // Initialize a DataDump object to simulate different inputs
    DataDump dump;

    assert_s(State::INIT, fsm);
    initToCalibration(fsm, dump);
    calibrationToManual(fsm, dump);
    manualToArmed(fsm, dump);
    armedToErrorGround(fsm, dump);
    errorGroundToInit(fsm, dump);
    
    return;
}

// Function to test whether recovering from calibration works
void recoverFromCalibration() {
    // Instantiate AvState
    AvState fsm;

    // Initialize a DataDump object to simulate different inputs
    DataDump dump;

    assert_s(State::INIT, fsm);
    initToCalibration(fsm, dump);
    calibrationToInit(fsm, dump);
    
    return;
}

// Function to test whether after recovering from an error on ground we can go through the normal flight process
void recoverFromErrorGroundAndFly() {
    // Instantiate AvState
    AvState fsm;

    // Initialize a DataDump object to simulate different inputs
    DataDump dump;

    assert_s(State::INIT, fsm);
    initToCalibration(fsm, dump);
    calibrationToManual(fsm, dump);
    manualToArmed(fsm, dump);
    armedToErrorGround(fsm, dump);
    errorGroundToInit(fsm, dump);
    initToCalibration(fsm, dump);
    calibrationToManual(fsm, dump);
    manualToArmed(fsm, dump);
    armedToReady(fsm, dump);
    readyToThrustSequence(fsm, dump);
    thrustSequenceToLiftoff(fsm, dump);
    liftoffToAscent(fsm, dump);
    ascentToDescent(fsm, dump);
    descentToLanded(fsm, dump);
    
    return;
}




// Add more test cases as needed

int main(int argc, char** argv) {
    // We test that all transitions are working as expected in the case of a flight without errors
    flightWithoutError();
    std::cout << "Flight without error: OK\n"<<std::endl;

    // We test that the ERRORGROUND state can be triggered from the ARMED state
    errorOnGroundFromArmed();
    std::cout << "Error on ground from ARMED: OK\n"<<std::endl;

    // We test that the ERRORGROUND state can be triggered from the CALIBRATION state
    errorOnGroundFromCalibration();
    std::cout << "Error on ground from CALIBRATION: OK\n"<<std::endl;

    // We test that the ERRORFLIGHT state can be triggered from the THRUSTSEQUENCE state
    errorInFlightFromThrustSequence();
    std::cout << "Error in flight from THRUSTSEQUENCE: OK\n"<<std::endl;

    // We test that the ERRORFLIGHT state can be triggered from the LIFTOFF state
    errorInFlightFromLiftoff();
    std::cout << "Error in flight from LIFTOFF: OK\n"<<std::endl;

    // We test that the ERRORFLIGHT state can be triggered from the ASCENT state
    errorInFlightFromAscent();
    std::cout << "Error in flight from ASCENT: OK\n"<<std::endl;

    // We test that the ERRORFLIGHT state can be triggered from the DESCENT state
    errorInFlightFromDescent();
    std::cout << "Error in flight from DESCENT: OK\n"<<std::endl;

    // We test that when the pressure is too low in the THRUSTSEQUENCE state we go back to the ARMED state
    pressureTooLow();
    std::cout << "Pressure too low in THRUSTSEQUENCE: OK\n"<<std::endl;

    // We test that we can recover from an error on the ground
    recoverFromErrorGround();
    std::cout << "Recover from error on ground: OK\n"<<std::endl;

    // We test that we can recover from calibration
    recoverFromCalibration();
    std::cout << "Recover from calibration: OK\n"<<std::endl;
    
    // We test that we can recover from an error on the ground and go through the normal flight process
    recoverFromErrorGroundAndFly();
    std::cout << "Recover from error on ground and fly: OK\n"<<std::endl;
    
    return 0;

} 