//
// Created by marin on 17.11.2024.
//

#include "Prop.h"
#include "thresholds.h"

void Prop::checkPolicy(DataDump dump) {

    switch (dump.av_state) {
        case State::INIT:
            // For the state INIT we do nothing
            break;
        case State::ERRORGROUND:
            listenToValves(dump);
            break;
        case State::DESCENT:
            pressureChecks(dump);
            break;
        case State::MANUAL:
            listenToValves(dump);
            break;
        case State::ERRORFLIGHT:
            handleErrorFlight(dump);
            break;
        case State::ARMED:
            // For the state ARMED we do nothing
            break;
        case State::READY:
            // fixme: should ignite event be here?
            ignition(dump);
            break;
        case State::THRUSTSEQUENCE:
            // TODO: implement THRUSTRSEQUENCE with Marin
            break;
        default:
            break;
    }
}

void Prop::pressureChecks(DataDump dump) {
    // We checks the tanks pressure and if they ate too high we open the valves
    if (dump.prop.LOX_pressure > LOX_PRESSURE_ZERO) {
        ventLOX();
    }
    if (dump.prop.fuel_pressure > FUEL_PRESSURE_ZERO) {
        ventFuel();
    }
    if (dump.prop.N2_pressure > N2_PRESSURE_ZERO) {
        ventN2();
    }

}

void Prop::ignition(DataDump dump) {
    return;
}

void Prop::safetyChecks(DataDump dump) {
    // We checks that the valves are open

    // We checks that the vents are open

    // We checks that the pressure is at 0
    return;
}

void Prop::listenToValves(DataDump dump) {
    // We check that the command has changed
    if (dump.event.command_updated)
    {
        // We check the command ID to call the right function
        switch(dump.telemetry_cmd.id) {
            // TODO: implement the right function calls for each state
            case CMD_ID::AV_CMD_IGNITION:
                // We ignite the engine
                startEngine();
                break;
            case CMD_ID::AV_CMD_ABORT:
                // We lock transitions and deploy the recovery system
                break;
            case CMD_ID::AV_CMD_IGNITER_LOX:
                // Open/close the igniter LOX valve
                valveIgniterLOX();
                break;
            case CMD_ID::AV_CMD_IGNITER_FUEL:
                // Open/close the igniter Ethanol valve
                valveIgniterEthanol();
                break;
            case CMD_ID::AV_CMD_MAIN_LOX:
                // Open/close the LOX main valve
                valveMainLOX();
                break;
            case CMD_ID::AV_CMD_MAIN_FUEL:
                // Open/close the Ethanol main valve
                valveMainEthanol();
                break;
            case CMD_ID::AV_CMD_VENT_LOX:
                // Open/close the LOX vent valve
                valveVentLOX();
                break;
            case CMD_ID::AV_CMD_VENT_FUEL:
                // Open/close the Ethanol vent valve
                valveVentEthanol();
                break;
            case CMD_ID::AV_CMD_PRESSURIZE:
                // We start the pressurization process
                break;
            default:
                break;
        }
    }
    return;
}

void Prop::handleErrorFlight(DataDump dump) {
    // Check if the engine is still ignited
    if (dump.prop.igniter_pressure > IGNITER_PRESSURE_WANTED) {
        killIgnitor();
        return;
    }
    // Check if engine is still running
    if (dump.prop.chamber_pressure > CHAMBER_PRESSURE_ZERO) {
        killEngine();
        return;
    }
    // Check if tanks vented properly
    // TODO: define if we check pressure or state of valves
    if (dump.prop.LOX_pressure > LOX_PRESSURE_ZERO) {
        ventLOX();
    }
    if (dump.prop.fuel_pressure > FUEL_PRESSURE_ZERO) {
        ventFuel();
    }
    if (dump.prop.N2_pressure > N2_PRESSURE_ZERO) {
        ventN2();
    }
}

void killIgnitor() {

}



