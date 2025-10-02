#include "data.h"
#include "av_state.h"
#include "telecom.h"
#include "logger.h"
#include "av_timer.h"
#include <pigpio.h>

void actuate_valves(const DataDump& dump) {
    if (!dump.event.command_updated) {
        return;
    }
    const uint8_t cmd(dump.telemetry_cmd.id);
    Valves valves(dump.valves);
    switch (cmd) {
        case CMD_ID::AV_CMD_P_LOX:
            valves.valve_dpr_pressure_lox = !valves.valve_dpr_pressure_lox;
            break;
        case CMD_ID::AV_CMD_P_FUEL:
            valves.valve_dpr_pressure_fuel = !valves.valve_dpr_pressure_fuel;
            break;
        case CMD_ID::AV_CMD_VENT_LOX:
            valves.valve_dpr_vent_lox = !valves.valve_dpr_vent_lox;
            break;
        case CMD_ID::AV_CMD_VENT_FUEL:
            valves.valve_dpr_vent_fuel = !valves.valve_dpr_vent_fuel;
            break;
        case CMD_ID::AV_CMD_VENT_N2:
            valves.valve_dpr_vent_copv = !valves.valve_dpr_vent_copv;
            break;
        case CMD_ID::AV_CMD_MAIN_LOX:
            valves.valve_prb_main_lox = !valves.valve_prb_main_lox;
            break;
        case CMD_ID::AV_CMD_MAIN_FUEL:
            valves.valve_prb_main_fuel = !valves.valve_prb_main_fuel;
            break;
        }
    Data::get_instance().write(Data::VALVES, &valves);
}

int main() {
    gpioInitialise();
    Logger::init();

    AvState fsm;

    Telecom telecom;
    telecom.begin();

    uint32_t now_ms(AvTimer::tick());
    uint32_t old_ms, delta_ms(0);
    while (1) {
        old_ms = now_ms;
        now_ms = AvTimer::tick();
        delta_ms = now_ms - old_ms;

        const DataDump dump(Data::get_instance().get());
        actuate_valves(dump);

        telecom.check_policy(dump, delta_ms);

        if (delta_ms < 50) {
            AvTimer::sleep(50 - delta_ms);
        }

    }

    Logger::terminate();
    gpioTerminate();
}
