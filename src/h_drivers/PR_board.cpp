#include "PR_board.h"
#include "data.h"
#include "av_state.h"

// TODO: coté hardware, voir i2_interface.h
// if adress missing put a fix me and set the adress to 0x00 in the meantime 

// Enumeration for the command of the PR board
enum class PRBCommand {
    // Write Commands
    TIMESTAMP_MAIN_FSM,
    WAKE_UP,
    CLEAR_TO_IGNITE,
    // Read Commands
    IS_WOKEN_UP,
    FSM_PRB,
    P_OIN,
    T_OIN,
    P_EIN,
    T_EIN,
    P_CCC,
    T_CCC,
    P_CIG,
    T_CIG,
    // Read/Write Command
    STATES_VANNES
};

// Define constants for valve bit positions
// TODO: Check whether the order is ok or not
#define VE_NO  (1 << 5)  
#define VO_NOC (1 << 4)
#define ME_B   (1 << 3)
#define MO_BC  (1 << 2)
#define IE_NC  (1 << 1)
#define IO_NCC (1 << 0)

PR_board::PR_board() {
    // Initialize the board by setting all the valves to closed
    current_valve_state = 0;
    // TODO: should modify the goat accordingly 
    // inline function
}

void PR_board::write_valves(uint8_t valve_states) {
    current_valve_state = valve_states;  // Store the valve state
    // TODO: Implement hardware control logic if needed
}

uint8_t PR_board::read_valves() const {
    return current_valve_state;  // Return the stored valve state
}

void PR_board::check_policy(Data::GoatReg reg, const DataDump& dump) {
    switch (dump.av_state) {
        case State::INIT:
            // For the INIT state we do nothing
            break;
        case State::ERRORGROUND:
            handleErrorGround(dump);
            // TODO: check whether it should be the following instead
            // listenToValves(dump);
            break;
        case State::DESCENT:
            handleDescent(dump);
            // TODO: check whether it should be the following instead
            // pressureChecks(dump);
            break;
        case State::MANUAL:
            processManualMode(dump);
            // TODO: check whether it should be the following instead
            // listenToValves(dump);
            break;
        case State::ERRORFLIGHT:
            handleErrorFlight(dump);
            break;
        case State::ARMED:
            // For the ARMED state we do nothing
            break;
        case State::READY:
            processReadyState(dump);
            // TODO: check whether it should be the following instead
            // ignition(dump); // fixme: should ignite event be here?
            break;
        case State::THRUSTSEQUENCE:
            // TODO: Implement thrust sequence logic
            break;
        default:
            break;
    }
}

void PR_board::handleErrorGround(const DataDump& dump) {
    // Handle errors on the ground
}

void PR_board::handleDescent(const DataDump& dump) {
    // Handle logic for descent phase
}

// eteindre et allumer chacune des valves
void PR_board::processManualMode(const DataDump& dump) {
    // Process commands in manual mode
    if (dump.event.command_updated) {
        switch (dump.telemetry_cmd.id) {
            case CMD_ID::AV_CMD_IGNITION:
                triggerIgnition();
                break;
            case CMD_ID::AV_CMD_ABORT:
                executeAbort();  // <-- FIXED: Replaced incorrect constructor call
                break;
            case CMD_ID::AV_CMD_DEPLOY_RECOVERY:
                deployRecoverySystem();
                break;
            default:
                break;
        }
    }
}

// message groupe av pour les fonctions 
// interface av pr
void PR_board::handleErrorFlight(const DataDump& dump) {
    // Handle in-flight errors
}

void PR_board::processReadyState(const DataDump& dump) {
    // Handle logic for READY state
}

void PR_board::triggerIgnition() {
    // Code to trigger ignition
}

void PR_board::executeAbort() {
    // Code to abort
}

void PR_board::deployRecoverySystem() {
    // Code to deploy recovery system
}
