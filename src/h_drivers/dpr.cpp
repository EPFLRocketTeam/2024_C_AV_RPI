#include <iostream>
#include <string>
#include "PacketDefinition_Firehorn.h"
#include "data.h"
#include "logger.h"
#include "dpr.h"
#include "i2c_interface.h"
#include "intranet_commands.h"


DPR::DPR(const uint8_t address) : m_address(address) {
    m_code = (m_address == AV_NET_ADDR_DPR_ETH ? "ETH" : "LOX");
    try {
        I2CInterface::getInstance().open(m_address);
    }catch(const I2CInterfaceException& e) {
        Logger::log_eventf(Logger::ERROR, "Error during DPR %s I2C initialization: %s", m_code, e.what());
    }
}

DPR::~DPR() {
    try {
        I2CInterface::getInstance().close(m_address);
    }catch(const I2CInterfaceException& e) {
        Logger::log_eventf(Logger::ERROR, "Error during DPR %s I2C deinitialization: %s", m_code, e.what());
    }
}

void DPR::write_timestamp(const uint32_t tmsp) {
    try {
        I2CInterface::getInstance().write(m_address, AV_NET_DPR_TIMESTAMP, (uint8_t*)&tmsp,
        AV_NET_XFER_SIZE);
    }catch(I2CInterfaceException& e) {
        std::string msg("DPR " + m_code + " write_timestamp error: ");
        throw DPRException(msg + e.what());
    }
}

void DPR::wake_up() {
    const uint32_t order(AV_NET_CMD_ON);
    try {
        I2CInterface::getInstance().write(m_address, AV_NET_DPR_WAKE_UP, (uint8_t*)&order, AV_NET_XFER_SIZE);
    }catch(I2CInterfaceException& e) {
        std::string msg("DPR " + m_code + " wake_up error: ");
        throw DPRException(msg + e.what());
    }
}

bool DPR::read_is_woken_up() {
    uint32_t rslt(0);
    try {
        I2CInterface::getInstance().read(m_address, AV_NET_DPR_IS_WOKEN_UP, (uint8_t*)&rslt, AV_NET_XFER_SIZE);
    }catch(I2CInterfaceException& e) {
        std::string msg("DPR " + m_code + " read_is_woken_up error: ");
        throw DPRException(msg + e.what());
    }

    bool dpr_woken_up(rslt == AV_NET_CMD_ON);
    Data::GoatReg gr(m_address == AV_NET_ADDR_DPR_ETH ? Data::EVENT_DPR_ETH_READY : Data::EVENT_DPR_LOX_READY);

    Data::get_instance().write(gr, &dpr_woken_up);

    return dpr_woken_up;
}

void DPR::send_pressurize() {
    const uint32_t order(AV_NET_CMD_ON);
    try {
        I2CInterface::getInstance().write(m_address, AV_NET_DPR_PRESSURIZE, (uint8_t*)&order, AV_NET_XFER_SIZE);
    }catch(I2CInterfaceException& e) {
        std::string msg("DPR " + m_code + " send_pressurize error: ");
        throw DPRException(msg + e.what());
    }
}

void DPR::send_abort() {
    const uint32_t order(AV_NET_CMD_ON);
    try {
        I2CInterface::getInstance().write(m_address, AV_NET_DPR_ABORT, (uint8_t*)&order, AV_NET_XFER_SIZE);
    }catch(I2CInterfaceException& e) {
        std::string msg("DPR " + m_code + " send_abort error: ");
        throw DPRException(msg + e.what());
    }
}

void DPR::read_tank_level() {
    float rslt(0);
    try {
        I2CInterface::getInstance().read(m_address, AV_NET_DPR_L_XTA, (uint8_t*)&rslt, AV_NET_XFER_SIZE);
    }catch(I2CInterfaceException& e) {
        std::string msg("DPR " + m_code + " read_tank_level error: ");
        throw DPRException(msg + e.what());
    }

    Data::GoatReg gr(m_address == AV_NET_ADDR_DPR_ETH ? Data::PR_SENSOR_L_ETA : Data::PR_SENSOR_L_OTA);
    Data::get_instance().write(gr, &rslt);
}

void DPR::read_tank_pressure() {
    float rslt(0);
    try {
        I2CInterface::getInstance().read(m_address, AV_NET_DPR_P_XTA, (uint8_t*)&rslt, AV_NET_XFER_SIZE);
    }catch(I2CInterfaceException& e) {
        std::string msg("DPR " + m_code + " read_tank_pressure error: ");
        throw DPRException(msg + e.what());
    }

    Data::GoatReg gr(m_address == AV_NET_ADDR_DPR_ETH ? Data::PR_SENSOR_P_ETA : Data::PR_SENSOR_P_OTA);
    Data::get_instance().write(gr, &rslt);
}

void DPR::read_tank_temperature() {
    float rslt(0);
    try {
        I2CInterface::getInstance().read(m_address, AV_NET_DPR_T_XTA, (uint8_t*)&rslt, AV_NET_XFER_SIZE);
    }catch(I2CInterfaceException& e) {
        std::string msg("DPR " + m_code + " read_tank_temperature error: ");
        throw DPRException(msg + e.what());
    }

    Data::GoatReg gr(m_address == AV_NET_ADDR_DPR_ETH ? Data::PR_SENSOR_T_ETA : Data::PR_SENSOR_T_OTA);
    Data::get_instance().write(gr, &rslt);
}

void DPR::read_copv_pressure() {
    float rslt(0);
    try {
        I2CInterface::getInstance().read(m_address, AV_NET_DPR_P_NCO, (uint8_t*)&rslt, AV_NET_XFER_SIZE);
    }catch(I2CInterfaceException& e) {
        std::string msg("DPR " + m_code + " read_copv_pressure error: ");
        throw DPRException(msg + e.what());
    }

    Data::get_instance().write(Data::PR_SENSOR_P_NCO, &rslt);
}

void DPR::read_copv_temperature() {
    float rslt(0);
    try {
        I2CInterface::getInstance().read(m_address, AV_NET_DPR_T_NCO, (uint8_t*)&rslt, AV_NET_XFER_SIZE);
    }catch(I2CInterfaceException& e) {
        std::string msg("DPR " + m_code + " read_copv_temperature error: ");
        throw DPRException(msg + e.what());
    }

    Data::get_instance().write(Data::PR_SENSOR_T_NCO, &rslt);
}

void DPR::write_valves(const uint32_t cmd) {
	uint32_t valves_state(cmd);
    try {
        I2CInterface::getInstance().write(m_address, AV_NET_DPR_VALVES_STATE, (uint8_t*)&valves_state,AV_NET_XFER_SIZE);
    }catch (I2CInterfaceException& e) {
        std::string msg("DPR " + m_code + " write_valves error: ");
        throw DPRException(msg + e.what());
    }
    Logger::log_eventf("Writing DPR valves: %x\n", valves_state);
}

void DPR::read_valves() {
	uint32_t dpr_valves(0);
	try {
		I2CInterface::getInstance().read(m_address, AV_NET_DPR_VALVES_STATE, (uint8_t*)&dpr_valves, AV_NET_XFER_SIZE);
	}catch (I2CInterfaceException& e) {
		std::string msg("DPR " + m_code + " read_valves error: ");
		throw DPRException(msg + e.what());
	}
	uint8_t vent_copv((dpr_valves & 0xFF0000) >> 16);
	uint8_t pressure_tank((dpr_valves & 0x00FF00) >> 8);
	uint8_t vent_tank(dpr_valves & 0xFF);

	Valves valves(Data::get_instance().get().valves);

	if (vent_copv == AV_NET_CMD_ON) {
		valves.valve_dpr_vent_copv = 1;
	}else if (vent_copv == AV_NET_CMD_OFF) {
		valves.valve_dpr_vent_copv = 0;
	}

	if (m_address == AV_NET_ADDR_DPR_LOX) {
		if (pressure_tank == AV_NET_CMD_ON) {
			valves.valve_dpr_pressure_lox = 1;
		}else if (pressure_tank == AV_NET_CMD_OFF) {
			valves.valve_dpr_pressure_lox = 0;
		}
		if (vent_tank == AV_NET_CMD_ON) {
			valves.valve_dpr_vent_lox = 1;
		}else if (vent_tank == AV_NET_CMD_OFF) {
			valves.valve_dpr_vent_lox = 0;
		}
	}

	Logger::log_eventf("Reading DPR valves: %x\n", dpr_valves);
	Data::get_instance().write(Data::VALVES, &valves);
}

void DPR::check_policy(const DataDump& dump, const uint32_t delta_ms) {
    this->delta_ms = delta_ms;
    count_ms += delta_ms;
    switch (dump.av_state) {
        case State::INIT:
            handle_init(dump);
            break;
        case State::CALIBRATION:
            handle_calibration(dump);
            break;
        case State::MANUAL:
            handle_manual(dump);
            break;
        case State::ARMED:
            handle_armed(dump);
            break ;
        case State::READY:
            handle_ready(dump);
            break;
        case State::THRUSTSEQUENCE:
            handle_thrustsequence(dump);
            break;
        case State::LIFTOFF:
            handle_liftoff(dump);
            break;
        case State::ASCENT:
            handle_ascent(dump);
            break;
        case State::DESCENT:
            handle_descent(dump);
            break;
        case State::LANDED:
            handle_landed(dump);
            break;
        case State::ERRORGROUND:
            handle_errorground(dump);
            break;
        case State::ERRORFLIGHT:
            handle_errorflight(dump);
            break;
        default:
            handle_errorground(dump);
            break;
    }

    //read_tank_level();
    //read_tank_pressure();
    //read_tank_temperature();
    //read_copv_pressure();
    //read_copv_temperature();
    read_valves();
}

void DPR::handle_init(const DataDump& dump) {
    // Write timestamp at a freq of 1Hz
    if (count_ms >= 1000) {
        write_timestamp(dump.av_timestamp);
        count_ms = 0;
    }

    // Wake up DPR at power on for sensors polling
    const bool dpr_ready(m_address == AV_NET_ADDR_DPR_ETH ? dump.event.dpr_eth_ready : dump.event.dpr_lox_ready);
    if (!dpr_ready) {
        static uint32_t count_wkp(0);
        static uint8_t wkp_attempts(0);
        count_wkp += delta_ms;

        if (count_wkp >= 500) {
            wake_up();
            read_is_woken_up();
            count_wkp = 0;
            ++wkp_attempts;
        }

        if (wkp_attempts > 10) {
            // Log error msg
            // FSM -> ERRORGROUND ?
        }
    }
}

void DPR::handle_calibration(const DataDump& dump) {
    // Write timestamp at a freq of 1Hz
    if (count_ms >= 1000) {
        write_timestamp(dump.av_timestamp);
        count_ms = 0;
    }
}

void DPR::handle_manual(const DataDump& dump) {
    // Write timestamp at a freq of 1Hz
    // if (count_ms >= 1000) {
    //    write_timestamp(dump.av_timestamp);
    //    count_ms = 0;
    //}

    if (dump.event.command_updated) {
        switch (dump.telemetry_cmd.id) {
            // TODO: add PE, PO and DN valves to RF_Protocol_Interface CMD_ID enum
		case CMD_ID::AV_CMD_P_LOX:
			if (m_address == AV_NET_ADDR_DPR_LOX) {
				std::cout << "Command AV_CMD_P_LOX\n";
				uint32_t cmd(0);
				if (dump.telemetry_cmd.value) {
					cmd = AV_NET_CMD_ON << 8;
				}else {
					cmd = AV_NET_CMD_OFF << 8;
				}
				write_valves(cmd);
			}
			break;
		case CMD_ID::AV_CMD_P_FUEL:
			if (m_address == AV_NET_ADDR_DPR_ETH) {
				uint32_t cmd(dump.telemetry_cmd.value);
				write_valves(cmd);
			}
			break;
            default:
                break;
        }
    }
}

void DPR::handle_armed(const DataDump& dump) {
    // Write timestamp at a freq of 1Hz
    if (count_ms >= 1000) {
        write_timestamp(dump.av_timestamp);
        count_ms = 0;
    }

    const bool dpr_ready(m_address == AV_NET_ADDR_DPR_ETH ? dump.event.dpr_eth_ready : dump.event.dpr_lox_ready);
    if (dpr_ready && dump.event.command_updated) {
        if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_PRESSURIZE) {
            send_pressurize();
        }
    }
}

void DPR::handle_ready(const DataDump& dump) {
    // Write timestamp at a freq of 1Hz
    if (count_ms >= 1000) {
        write_timestamp(dump.av_timestamp);
        count_ms = 0;
    }
}

void DPR::handle_thrustsequence(const DataDump& dump) {
    // Write timestamp at a freq of 1Hz
    if (count_ms >= 1000) {
        write_timestamp(dump.av_timestamp);
        count_ms = 0;
    }
}

void DPR::handle_liftoff(const DataDump& dump) {
    // Write timestamp at a freq of 1Hz
    if (count_ms >= 1000) {
        write_timestamp(dump.av_timestamp);
        count_ms = 0;
    }
}

void DPR::handle_ascent(const DataDump& dump) {
    // Write timestamp at a freq of 1Hz
    if (count_ms >= 1000) {
        write_timestamp(dump.av_timestamp);
        count_ms = 0;
    }
}

void DPR::handle_descent(const DataDump& dump) {
    // Write timestamp at a freq of 1Hz
    if (count_ms >= 1000) {
        write_timestamp(dump.av_timestamp);
        count_ms = 0;
    }
}

void DPR::handle_landed(const DataDump& dump) {
    // Write timestamp at a freq of 1Hz
    if (count_ms >= 1000) {
        write_timestamp(dump.av_timestamp);
        count_ms = 0;
    }
}

void DPR::handle_errorground(const DataDump& dump) {
    // Write timestamp at a freq of 1Hz
    if (count_ms >= 1000) {
        write_timestamp(dump.av_timestamp);
        count_ms = 0;
    }
}

void DPR::handle_errorflight(const DataDump& dump) {
    // Write timestamp at a freq of 1Hz
    if (count_ms >= 1000) {
        write_timestamp(dump.av_timestamp);
        count_ms = 0;
    }
}
