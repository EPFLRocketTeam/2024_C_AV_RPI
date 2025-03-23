#include "PR_board.h"
#include "data.h"
#include "av_state.h"
#include <iostream>

// Enumeration for the command of the PR board

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

void PR_board::check_policy(const DataDump& dump, const uint32_t delta_ms) {
    switch (dump.av_state) {
        case State::INIT:
            // For the INIT state we do nothing
            break;
        case State::ERRORGROUND:
            handleErrorGround(dump);
            break;
        case State::DESCENT:
            handleDescent(dump);
            break;
        case State::MANUAL:
            processManualMode(dump);
            break;
        case State::ERRORFLIGHT:
            handleErrorFlight(dump);
            break;
        case State::ARMED:
            // For the ARMED state we do nothing
            break;
        case State::READY:
            processReadyState(dump);
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

// TODO: eteindre et allumer chacune des valves
void PR_board::processManualMode(const DataDump& dump) {
    // Process commands in manual mode
    if (dump.event.command_updated) {
        switch (dump.telemetry_cmd.id) {
            case CMD_ID::AV_CMD_ABORT:
                executeAbort();  // <-- FIXED: Replaced incorrect constructor call
                break;
            case CMD_ID::AV_CMD_MANUAL_DEPLOY:
                deployRecoverySystem();
                break;
            default:
                break;
        }
    }
}

// message groupe av pour les fonctions 
// si pas de reponse, message groupe interface av pr
void PR_board::handleErrorFlight(const DataDump& dump) {
    // Handle in-flight error
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
