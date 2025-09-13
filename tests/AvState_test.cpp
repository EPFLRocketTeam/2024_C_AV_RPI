
#include "av_state.h"
#include <cassert>
#include <string.h>
#include "logger.h"

#define assert_s(expectedState,fsm)                                                           \
    do {                                                                                   \
        if ((fsm.getCurrentState()) != (expectedState)) {                                            \
            std::cerr << "Assertion failed: Expected state: " << fsm.stateToString(expectedState)            \
                      << ", but got state: " << fsm.stateToString(fsm.getCurrentState()) << std::endl;\
            assert(fsm.getCurrentState() == expectedState);                                      \
        }                                          \
        /*
        else {                                                                                \
            std::cout << "Current state is " << fsm.stateToString(fsm.getCurrentState()) << std::endl; \
        } */                                       \
    } while (0) 

// ================== Transition functions ==================

// Verify that the state has not changed when nothing changes
void sameState(AvState &fsm, DataDump &dump) {
    State previous_state = fsm.getCurrentState();
    
    fsm.update(dump,0);
    assert_s(previous_state, fsm);
    std::cout << "State remains the same\n";
}

// Function to trigger the INIT -> CALIBRATION transition
void initToCalibration(AvState &fsm, DataDump &dump) {
    dump.telemetry_cmd.id = CMD_ID::AV_CMD_CALIBRATE;
    fsm.update(dump,0);
    assert_s(State::CALIBRATION, fsm);
    //sameState(fsm, dump);
}

// Function to trigger the ERRORGROUND -> INIT transition
void errorGroundToInit(AvState &fsm, DataDump &dump) {
    dump.telemetry_cmd.id = CMD_ID::AV_CMD_RECOVER;
    fsm.update(dump,0);
    assert_s(State::INIT, fsm);
    //sameState(fsm, dump);
}

// Function to trigger the CALIBRATION -> FILLING transition
void calibrationToFilling(AvState &fsm, DataDump &dump) {
    dump.event.calibrated = true;
    fsm.update(dump,0);
    assert_s(State::FILLING, fsm);
    //sameState(fsm, dump);
}

// TODO: implement the error() function in av_state.cpp
// // Function to trigger the CALIBRATION -> ERRORGROUND transition
void calibrationToErrorGround(AvState fsm, DataDump dump) {
    dump.telemetry_cmd.id = CMD_ID::AV_CMD_ABORT;
    fsm.update(dump,0);
    assert_s(State::ABORT_ON_GROUND, fsm);
    //sameState(fsm, dump);
}

// Function to trigger the FILLING -> ARMED transition
void fillingToArmed(AvState &fsm, DataDump &dump) {
    dump.telemetry_cmd.id = CMD_ID::AV_CMD_ARM;
    dump.valves.valve_dpr_pressure_lox = 1; 
    dump.valves.valve_dpr_pressure_fuel = 1; 
    dump.valves.valve_dpr_vent_copv = 1; 
    dump.valves.valve_dpr_vent_lox = 1; 
    dump.valves.valve_dpr_vent_fuel = 1;
    dump.valves.valve_prb_main_lox = 1;
    dump.valves.valve_prb_main_fuel = 1;
    fsm.update(dump,0);
    assert_s(State::ARMED, fsm);
    //sameState(fsm, dump);
}

// Function to trigger the FILLING -> ERRORGROUND transition
void fillingToErrorGround(AvState &fsm, DataDump &dump) {
    dump.telemetry_cmd.id = CMD_ID::AV_CMD_ABORT;
    // dump.event.calibrated = false;
    fsm.update(dump,0);
    assert_s(State::ABORT_ON_GROUND, fsm);
}

// Function to trigger the ARMED -> ERRORGROUND transition
void armedToErrorGround(AvState &fsm, DataDump &dump) {
    dump.telemetry_cmd.id = CMD_ID::AV_CMD_ABORT;
    dump.event.calibrated = false;
    fsm.update(dump,0);
    assert_s(State::ABORT_ON_GROUND, fsm);
    //sameState(fsm, dump);
}

// Function to trigger the ARMED -> PRESSURIZATION transition
void armedToPressurization(AvState &fsm, DataDump &dump) {
    dump.telemetry_cmd.id = CMD_ID::AV_CMD_LAUNCH;
    //dump.prop.fuel_pressure = FUEL_PRESSURE_WANTED;
    //dump.prop.LOX_pressure = LOX_PRESSURE_WANTED;
    fsm.update(dump,0);
    assert_s(State::PRESSURIZATION, fsm);
    //sameState(fsm, dump);
}

// Function to trigger the PRESSURIZATION -> IGNITION transition
void pressurizationToIgnition(AvState &fsm, DataDump &dump) {
    //dump.telemetry_cmd.id = CMD_ID::AV_CMD_IGNITION;
    dump.prop.LOX_pressure = PRESSURIZATION_CHECK_PRESSURE-1;
    dump.prop.fuel_pressure = PRESSURIZATION_CHECK_PRESSURE-1;

    fsm.update(dump,0);
    assert_s(State::IGNITION, fsm);
    //TODO check that this is correct
    //sameState(fsm, dump);
}

// Function to trigger the PRESSURIZATION -> ERRORGROUND transition
void pressurizationToErrorGround(AvState &fsm, DataDump &dump) {
    //dump.telemetry_cmd.id = CMD_ID::AV_CMD_ABORT;
    fsm.update(dump,0);
    assert_s(State::ABORT_ON_GROUND, fsm);
    //sameState(fsm, dump);
}

// Function to trigger the IGNITION -> FILLING transition
void ignitionToFilling(AvState &fsm, DataDump &dump) {
    dump.event.ignition_failed = true;
    dump.event.dpr_eth_pressure_ok = false;
    fsm.update(dump,0);
    assert_s(State::FILLING, fsm);
    //sameState(fsm, dump);
}

// Function to trigger the IGNITION -> ERRORGROUND transition
void ignitionToErrorGround(AvState &fsm, DataDump &dump) {
    dump.telemetry_cmd.id = CMD_ID::AV_CMD_ABORT;
    fsm.update(dump,0);
    assert_s(State::ABORT_ON_GROUND, fsm);
    //sameState(fsm, dump);
}

// Function to trigger the IGNITION -> BURN transition
void ignitionToBurn(AvState &fsm, DataDump &dump) {
    dump.event.ignited = true;
    //dump.nav.speed.z = SPEED_MIN_ASCENT + 1;
    //dump.nav.accel.z = ACCEL_ZERO + 1;
    //dump.nav.altitude = ALTITUDE_ZERO + 1;
    /*
    dump.prop.igniter_pressure = IGNITER_PRESSURE_WANTED + 1;
    dump.prop.chamber_pressure = CHAMBER_PRESSURE_WANTED + 1;
    dump.prop.fuel_inj_pressure = INJECTOR_PRESSURE_WANTED_MIN + 1;
    dump.prop.LOX_inj_pressure = INJECTOR_PRESSURE_WANTED_MIN + 1;
    dump.prop.fuel_pressure = FUEL_PRESSURE_WANTED + 1;
    dump.prop.LOX_pressure = LOX_PRESSURE_WANTED + 1;
    dump.prop.N2_pressure = N2_PRESSURE_ZERO + 1;
    */
    fsm.update(dump,0);
    assert_s(State::BURN, fsm);
    //sameState(fsm, dump);
}

// Function to trigger the BURN -> ERRORFLIGHT transition
void burnToErrorFlight(AvState &fsm, DataDump &dump) {
    dump.telemetry_cmd.id = CMD_ID::AV_CMD_ABORT;
    fsm.update(dump,0);
    assert_s(State::ABORT_IN_FLIGHT, fsm);
    //sameState(fsm, dump);
}

// Function to trigger the BURN -> ASCENT transition
void burnToAscent(AvState &fsm, DataDump &dump) {
    dump.nav.altitude = ALTITUDE_THRESHOLD + 1;
    fsm.update(dump,0);
    assert_s(State::ASCENT, fsm);
    //sameState(fsm, dump);
}

// Function to trigger the ASCENT -> ERRORFLIGHT transition
void ascentToErrorFlight(AvState &fsm, DataDump &dump) {
    dump.telemetry_cmd.id = CMD_ID::AV_CMD_ABORT;
    fsm.update(dump,0);
    assert_s(State::ABORT_IN_FLIGHT, fsm);
    //sameState(fsm, dump);
}

// Function to trigger the ASCENT -> DESCENT transition
void ascentToDescent(AvState &fsm, DataDump &dump) {
    //dump.nav.speed.z = SPEED_ZERO * 0.1;
    fsm.update(dump,0);
    assert_s(State::DESCENT, fsm);
    //sameState(fsm, dump);
}

// Function to trigger the DESCENT -> ERRORFLIGHT transition
void descentToErrorFlight(AvState &fsm, DataDump &dump) {
    dump.telemetry_cmd.id = CMD_ID::AV_CMD_ABORT;
    fsm.update(dump,0);
    assert_s(State::ABORT_IN_FLIGHT, fsm);
    //sameState(fsm, dump);
}

// Function to trigger the DESCENT -> LANDED transition
void descentToLanded(AvState &fsm, DataDump &dump) {
    /*dump.nav.speed.z = SPEED_ZERO * 0.1;
    dump.nav.speed.x = SPEED_ZERO * 0.1;
    dump.nav.speed.y = SPEED_ZERO * 0.1;
    dump.prop.chamber_pressure = CHAMBER_PRESSURE_ZERO - 1;
    dump.prop.fuel_pressure = FUEL_PRESSURE_ZERO - 1;
    dump.prop.LOX_pressure = LOX_PRESSURE_ZERO - 1;
    dump.prop.fuel_inj_pressure = INJECTOR_PRESSURE_ZERO - 1;
    dump.prop.LOX_inj_pressure = INJECTOR_PRESSURE_ZERO - 1;
    dump.prop.N2_pressure = N2_PRESSURE_ZERO - 1;
    dump.prop.igniter_pressure = IGNITER_PRESSURE_ZERO - 1;
    */
    fsm.update(dump,0);
    assert_s(State::LANDED, fsm);
    //sameState(fsm, dump);
}


// ================== Test functions ==================

// Function to test where there are no errors during the flight
void flightWithoutError() {
    // Instantiate AvState
    AvState fsm;

    // Initialize a DataDump object to simulate different inputs
    DataDump dump;
    memset(&dump, 0, sizeof(dump));
    dump.av_state = State::INIT;

    assert_s(State::INIT, fsm);
    initToCalibration(fsm, dump);
    calibrationToFilling(fsm, dump);
    fillingToArmed(fsm, dump);
    armedToPressurization(fsm, dump);
    pressurizationToIgnition(fsm, dump);
    ignitionToBurn(fsm, dump);
    burnToAscent(fsm, dump);
    ascentToDescent(fsm, dump);
    descentToLanded(fsm, dump);
    
    return;
}

// TODO: add later on
// Function to test if the ERRORGROUND state is triggered from the CALIBRATION state
void errorOnGroundFromCalibration() {
    // Instantiate AvState
    AvState fsm;
    // Initialize a DataDump object to simulate different inputs
    DataDump dump;
memset(&dump, 0, sizeof(dump));
    assert_s(State::INIT, fsm);
    initToCalibration(fsm, dump);
    calibrationToErrorGround(fsm, dump);
    return;
}

// Function to test if the ERRORGROUND state is triggered from the FILLING state
void errorOnGroundFromFilling() {
    // Instantiate AvState
    AvState fsm;
    // Initialize a DataDump object to simulate different inputs
    DataDump dump;
memset(&dump, 0, sizeof(dump));
    assert_s(State::INIT, fsm);
    initToCalibration(fsm, dump);
    calibrationToFilling(fsm, dump);
    fillingToErrorGround(fsm, dump);
    return;
}

// Function to test if the ERRORGROUND state is triggered from the ARMED state
void errorOnGroundFromArmed() {
    // Instantiate AvState
    AvState fsm;

    // Initialize a DataDump object to simulate different inputs
    DataDump dump;
memset(&dump, 0, sizeof(dump));
    dump.av_state = State::INIT;

    assert_s(State::INIT, fsm);
    initToCalibration(fsm, dump);
    calibrationToFilling(fsm, dump);
    //sameState(fsm, dump);
    fillingToArmed(fsm, dump);
    armedToErrorGround(fsm, dump);
    
    return;
}

// Function to test if the ERRORGROUND state is triggered from the PRESSURIZATION state
void errorOnGroundFromPressurization() {
    // Instantiate AvState
    AvState fsm;
    // Initialize a DataDump object to simulate different inputs
    DataDump dump;
memset(&dump, 0, sizeof(dump));
    assert_s(State::INIT, fsm);
    initToCalibration(fsm, dump);
    calibrationToFilling(fsm, dump);
    fillingToArmed(fsm, dump);
    armedToPressurization(fsm, dump);
    pressurizationToErrorGround(fsm, dump);
    return;
}

// Function to test if the ERRORGROUND state is triggered from the IGNITION state
void errorOnGroundFromIgnition() {
    // Instantiate AvState
    AvState fsm;

    // Initialize a DataDump object to simulate different inputs
    DataDump dump;
memset(&dump, 0, sizeof(dump));
    dump.av_state = State::INIT;

    assert_s(State::INIT, fsm);
    initToCalibration(fsm, dump);
    calibrationToFilling(fsm, dump);
    fillingToArmed(fsm, dump);
    armedToPressurization(fsm, dump);
    pressurizationToIgnition(fsm, dump);
    ignitionToErrorGround(fsm, dump);

    return;
}

// Function to test if the ERRORFLIGHT state is triggered from the BURN state
void errorInFlightFromBurn() {
    // Instantiate AvState
    AvState fsm;

    // Initialize a DataDump object to simulate different inputs
    DataDump dump;
memset(&dump, 0, sizeof(dump));
    dump.av_state = State::INIT;

    assert_s(State::INIT, fsm);
    initToCalibration(fsm, dump);
    calibrationToFilling(fsm, dump);
    fillingToArmed(fsm, dump);
    armedToPressurization(fsm, dump);
    pressurizationToIgnition(fsm, dump);
    ignitionToBurn(fsm, dump);
    burnToErrorFlight(fsm, dump);
    
    return;
}

// Function to test if the ERRORFLIGHT state is triggered from the ASCENT state
void errorInFlightFromAscent() {
    // Instantiate AvState
    AvState fsm;

    // Initialize a DataDump object to simulate different inputs
    DataDump dump;
memset(&dump, 0, sizeof(dump));
    dump.av_state = State::INIT;

    assert_s(State::INIT, fsm);
    initToCalibration(fsm, dump);
    calibrationToFilling(fsm, dump);
    fillingToArmed(fsm, dump);
    armedToPressurization(fsm, dump);
    pressurizationToIgnition(fsm, dump);
    ignitionToBurn(fsm, dump);
    burnToAscent(fsm, dump);
    ascentToErrorFlight(fsm, dump);
    
    return;
}

// Function to test if the ERRORFLIGHT state is triggered from the DESCENT state
void errorInFlightFromDescent() {
    // Instantiate AvState
    AvState fsm;

    // Initialize a DataDump object to simulate different inputs
    DataDump dump;
memset(&dump, 0, sizeof(dump));
    dump.av_state = State::INIT;

    assert_s(State::INIT, fsm);
    initToCalibration(fsm, dump);
    calibrationToFilling(fsm, dump);
    fillingToArmed(fsm, dump);
    armedToPressurization(fsm, dump);
    pressurizationToIgnition(fsm, dump);
    ignitionToBurn(fsm, dump);
    burnToAscent(fsm, dump);
    ascentToDescent(fsm, dump);
    descentToErrorFlight(fsm, dump);

    return;
}

// Function to test whether when the pressure is too low the transition from IGNITION to FILLING is triggered
void pressureTooLow() {
    // Instantiate AvState
    AvState fsm;

    // Initialize a DataDump object to simulate different inputs
    DataDump dump;
memset(&dump, 0, sizeof(dump));

    assert_s(State::INIT, fsm);
    initToCalibration(fsm, dump);
    calibrationToFilling(fsm, dump);
    fillingToArmed(fsm, dump);
    armedToPressurization(fsm, dump);
    pressurizationToIgnition(fsm, dump);
    ignitionToFilling(fsm, dump);
    
    return;
}

// Function to test whether recovering from an error on the ground works
void recoverFromErrorGround() {
    // Instantiate AvState
    AvState fsm;

    // Initialize a DataDump object to simulate different inputs
    DataDump dump;
memset(&dump, 0, sizeof(dump));
    dump.av_state = State::INIT;

    assert_s(State::INIT, fsm);
    initToCalibration(fsm, dump);
    calibrationToFilling(fsm, dump);
    fillingToArmed(fsm, dump);
    armedToErrorGround(fsm, dump);
    errorGroundToInit(fsm, dump);
    
    return;
}

// Function to test whether after recovering from an error on ground we can go through the normal flight process
void recoverFromErrorGroundAndFly() {
    // Instantiate AvState
    AvState fsm;

    // Initialize a DataDump object to simulate different inputs
    DataDump dump;
    memset(&dump, 0, sizeof(dump));
    dump.av_state = State::INIT;

    assert_s(State::INIT, fsm);
    initToCalibration(fsm, dump);
    calibrationToFilling(fsm, dump);
    fillingToArmed(fsm, dump);
    armedToErrorGround(fsm, dump);
    errorGroundToInit(fsm, dump);
    initToCalibration(fsm, dump);
    calibrationToFilling(fsm, dump);
    fillingToArmed(fsm, dump);
    armedToPressurization(fsm, dump);
    pressurizationToIgnition(fsm, dump);
    ignitionToBurn(fsm, dump);
    burnToAscent(fsm, dump);
    ascentToDescent(fsm, dump);
    descentToLanded(fsm, dump);
    
    return;
}


// Add more test cases as needed

int main(int argc, char** argv) {
    Logger::init();

    // We test that all transitions are working as expected in the case of a flight without errors
    flightWithoutError();
    std::cout << "Flight without error: OK\n"<<std::endl;

    // We test that the ERRORGROUND state can be triggered from the CALIBRATION state
    errorOnGroundFromCalibration();
    std::cout << "Error on ground from CALIBRATION: OK\n"<<std::endl;

    // We test that the ERRORGROUND state can be triggered from the FILLING state
    errorOnGroundFromFilling();
    std::cout << "Error on ground from FILLING: OK\n"<<std::endl;

    // We test that the ERRORGROUND state can be triggered from the ARMED state
    errorOnGroundFromArmed();
    std::cout << "Error on ground from ARMED: OK\n"<<std::endl;

    // We test that the ERRORGROUND state can be triggered from the PRESSURIZATION state
    errorOnGroundFromPressurization();
    std::cout << "Error on ground from PRESSURIZATION: OK\n"<<std::endl;

    // We test that the ERRORGROUND state can be triggered from the IGNITION state
    errorOnGroundFromIgnition();
    std::cout << "Error on ground from IGNITION: OK\n"<<std::endl;

    // We test that the ERRORFLIGHT state can be triggered from the BURN state
    errorInFlightFromBurn();
    std::cout << "Error in flight from BURN: OK\n"<<std::endl;

    // We test that the ERRORFLIGHT state can be triggered from the ASCENT state
    errorInFlightFromAscent();
    std::cout << "Error in flight from ASCENT: OK\n"<<std::endl;

    // We test that the ERRORFLIGHT state can be triggered from the DESCENT state
    errorInFlightFromDescent();
    std::cout << "Error in flight from DESCENT: OK\n"<<std::endl;

    // We test that when the pressure is too low in the IGNITION state we go back to the ARMED state
    pressureTooLow();
    std::cout << "Pressure too low in IGNITION: OK\n"<<std::endl;

    // We test that we can recover from an error on the ground
    recoverFromErrorGround();
    std::cout << "Recover from error on ground: OK\n"<<std::endl;

    // We test that we can recover from an error on the ground and go through the normal flight process
    recoverFromErrorGroundAndFly();
    std::cout << "Recover from error on ground and fly: OK\n"<<std::endl;
    
    Logger::terminate();
    return 0;

} 
