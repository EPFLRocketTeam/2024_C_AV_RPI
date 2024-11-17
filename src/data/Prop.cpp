//
// Created by marin on 17.11.2024.
//

#include "Prop.h"
#include "thresholds.h"




void Prop::checkPolicy(DataDump dump) {

    switch (dump.av_state) {
        case State::ERRORFLIGHT:
            handleErrorFlight(dump);
            break;
    }
}

void Prop::handleErrorFlight(DataDump dump) {
    //check if the engine is still ignited
    if (dump.prop.igniter_pressure > IGNITER_PRESSURE_WANTED) {
        killIgnitor();
        return;
    }
    //check if engine is still running
    if (dump.prop.chamber_pressure > CHAMBER_PRESSURE_ZERO) {
        killEngine();
        return;
    }
    //check if tanks vented properly
    //TODO: define if we check pressure or state of valves
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



