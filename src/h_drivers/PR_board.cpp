#include "PR_board.h"
#include "data.h"
#include "av_state.h"
#include <iostream>

// Enumeration for the command of the PR board

// FIXME: implement the valve logic later on
PR_board::PR_board() {
    // Currently does nothing
}

// fixme: implement the valve logic later on 
// PR_board::PR_board() {
//     // Initialize all valves to closed (0 degrees)
//     for (auto& valve_state : current_valve_states) {
//         valve_state = static_cast<uint8_t>(ValveOpenDegree::DEG_0);
//     }
//     // TODO: should modify the goat accordingly 
//     // inline function
// }

// // Write a single valve's degree
// void PR_board::write_valve(uint8_t valve_id, ValveOpenDegree degree) {
//     // If the valve ID is invalid, print an error message and return
//     if (valve_id >= NUM_VALVES) {
//         std::cerr << "Invalid valve ID: " << static_cast<int>(valve_id) << "\n";
//         return;
//     }

//     current_valve_states[valve_id] = static_cast<uint8_t>(degree) & 0x0F; // Mask to 4 bits

//     // TODO: implement actual hardware control if needed
// }

// // Read a single valve's degree
// ValveOpenDegree PR_board::read_valve(uint8_t valve_id) const {
//     // If the valve ID is invalid, print an error message and return 0 degrees
//     if (valve_id >= NUM_VALVES) {
//         std::cerr << "Invalid valve ID: " << static_cast<int>(valve_id) << "\n";
//         return ValveOpenDegree::DEG_0; // Default to 0 degrees on error
//     }

//     // Mask the 4 LSB to get the actual degree
//     return static_cast<ValveOpenDegree>(current_valve_states[valve_id] & 0x0F);
// }

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
        case State::ARMED: {
            // FIXME: this logic is only valid for the FT
            bool prb_ready = true;
            Data::get_instance().write(Data::EVENT_PRB_READY, &prb_ready);
            // For the ARMED state we do nothing
            break;
        }
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
