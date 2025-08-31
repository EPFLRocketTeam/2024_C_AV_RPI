#include <iostream>
#include <string>
#include <unistd.h>
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
        Logger::log_eventf(Logger::ERROR, "Error during DPR_%s I2C initialization: %s", m_code.c_str(), e.what());
    }
}

DPR::~DPR() {
    try {
        I2CInterface::getInstance().close(m_address);
    }catch(const I2CInterfaceException& e) {
        Logger::log_eventf(Logger::ERROR, "Error during DPR_%s I2C deinitialization: %s", m_code.c_str(), e.what());
    }
}

void DPR::write_timestamp() {
    uint32_t tmsp(Data::get_instance().get().av_timestamp);
    try {
        I2CInterface::getInstance().write(m_address, AV_NET_DPR_TIMESTAMP, (uint8_t*)&tmsp,
        AV_NET_XFER_SIZE);
    }catch(I2CInterfaceException& e) {
        std::string msg("DPR " + m_code + " write_timestamp error: ");
        throw DPRException(msg + e.what());
    }
    Logger::log_eventf(Logger::DEBUG, "Writing TIMESTAMP to DPR_%s: %f", m_code.c_str(), tmsp);
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
    Logger::log_eventf(Logger::DEBUG, "Sending PRESSURIZE to DPR_%s", m_code.c_str());
}

void DPR::send_abort() {
    const uint32_t order(AV_NET_CMD_ON);
    try {
        I2CInterface::getInstance().write(m_address, AV_NET_DPR_ABORT, (uint8_t*)&order, AV_NET_XFER_SIZE);
    }catch(I2CInterfaceException& e) {
        std::string msg("DPR " + m_code + " send_abort error: ");
        throw DPRException(msg + e.what());
    }
    Logger::log_eventf(Logger::DEBUG, "Sending ABORT to DPR_%s", m_code.c_str());
}

/*
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

    Logger::log_eventf(Logger::DEBUG, "Reading %s tank level from DPR_%s: %f", m_code.c_str(), m_code.c_str(), rslt);
}
*/

float DPR::read_tank_pressure() {
    float rslt(0);
    try {
        I2CInterface::getInstance().read(m_address, AV_NET_DPR_P_XTA, (uint8_t*)&rslt, AV_NET_XFER_SIZE);
    }catch(I2CInterfaceException& e) {
        std::string msg("DPR " + m_code + " read_tank_pressure error: ");
        throw DPRException(msg + e.what());
    }

    Data::GoatReg gr(m_address == AV_NET_ADDR_DPR_ETH ? Data::PR_SENSOR_P_ETA : Data::PR_SENSOR_P_OTA);
    Data::get_instance().write(gr, &rslt);
    const char* prefix(m_address == AV_NET_ADDR_DPR_ETH ? "E" : "O");
    Logger::log_eventf(Logger::DEBUG, "Reading P_%sTA from DPR_%s: %f", prefix, m_code.c_str(), rslt);

    return rslt;
}

float DPR::read_tank_temperature() {
    float rslt(0);
    try {
        I2CInterface::getInstance().read(m_address, AV_NET_DPR_T_XTA, (uint8_t*)&rslt, AV_NET_XFER_SIZE);
    }catch(I2CInterfaceException& e) {
        std::string msg("DPR " + m_code + " read_tank_temperature error: ");
        throw DPRException(msg + e.what());
    }

    Data::GoatReg gr(m_address == AV_NET_ADDR_DPR_ETH ? Data::PR_SENSOR_T_ETA : Data::PR_SENSOR_T_OTA);
    Data::get_instance().write(gr, &rslt);
    const char* prefix(m_address == AV_NET_ADDR_DPR_ETH ? "E" : "O");
    Logger::log_eventf(Logger::DEBUG, "Reading T_%sTA from DPR_%s: %f", prefix, m_code.c_str(), rslt);

    return rslt;
}

float DPR::read_copv_pressure() {
    float rslt(0);
    try {
        I2CInterface::getInstance().read(m_address, AV_NET_DPR_P_NCO, (uint8_t*)&rslt, AV_NET_XFER_SIZE);
    }catch(I2CInterfaceException& e) {
        std::string msg("DPR " + m_code + " read_copv_pressure error: ");
        throw DPRException(msg + e.what());
    }

    Data::get_instance().write(Data::PR_SENSOR_P_NCO, &rslt);
    Logger::log_eventf(Logger::DEBUG, "Reading P_NCO from DPR_%s: %f", m_code.c_str(), rslt);

    return rslt;
}

float DPR::read_copv_temperature() {
    float rslt(0);
    try {
        I2CInterface::getInstance().read(m_address, AV_NET_DPR_T_NCO, (uint8_t*)&rslt, AV_NET_XFER_SIZE);
    }catch(I2CInterfaceException& e) {
        std::string msg("DPR " + m_code + " read_copv_temperature error: ");
        throw DPRException(msg + e.what());
    }

    Data::get_instance().write(Data::PR_SENSOR_T_NCO, &rslt);
    Logger::log_eventf(Logger::DEBUG, "Reading T_NCO from DPR_%s: %f", m_code.c_str(), rslt);

    return rslt;
}

void DPR::write_valves(const uint32_t cmd) {
	uint32_t valves_state(cmd);
    try {
        I2CInterface::getInstance().write(m_address, AV_NET_DPR_VALVES_STATE, (uint8_t*)&valves_state,AV_NET_XFER_SIZE);
    }catch (I2CInterfaceException& e) {
        std::string msg("DPR " + m_code + " write_valves error: ");
        throw DPRException(msg + e.what());
    }
    Logger::log_eventf(Logger::DEBUG, "Writing valves to DPR_%s: %x", m_code.c_str(), valves_state);
}

uint32_t DPR::read_valves() {
	uint32_t dpr_valves(0);
	try {
		I2CInterface::getInstance().read(m_address, AV_NET_DPR_VALVES_STATE, (uint8_t*)&dpr_valves, AV_NET_XFER_SIZE);
	}catch (I2CInterfaceException& e) {
		std::string msg("DPR " + m_code + " read_valves error: ");
		throw DPRException(msg + e.what());
	}
    uint8_t vent_copv(0);
    if (m_address == AV_NET_ADDR_DPR_ETH) {
        vent_copv = (dpr_valves & AV_NET_DPR_VALVE_DN_NC) >> AV_NET_SHIFT_DN_NC;
    }
	uint8_t pressure_tank((dpr_valves & AV_NET_DPR_VALVE_PX_NC) >> AV_NET_SHIFT_PX_NC);
	uint8_t vent_tank((dpr_valves & AV_NET_DPR_VALVE_VX_NO) >> AV_NET_SHIFT_VX_NO);

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

    if (m_address == AV_NET_ADDR_DPR_ETH) {
        if (pressure_tank == AV_NET_CMD_ON) {
            valves.valve_dpr_pressure_fuel = 1;
        }else if (pressure_tank == AV_NET_CMD_OFF) {
            valves.valve_dpr_pressure_fuel = 1;
        }
        if (vent_tank == AV_NET_CMD_ON) {
            valves.valve_dpr_vent_fuel = 1;
        }else if (vent_tank == AV_NET_CMD_OFF) {
            valves.valve_dpr_vent_fuel = 0;
        }
    }

	Logger::log_eventf(Logger::DEBUG, "Reading valves from DPR_%s: %x", m_code.c_str(), dpr_valves);
	Data::get_instance().write(Data::VALVES, &valves);

    return dpr_valves;
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
        case State::FILLING:
            handle_filling(dump);
            break;
        case State::ARMED:
            handle_armed(dump);
            break ;
        case State::PRESSURIZED:
            handle_pressurized(dump);
            break;
        case State::IGNITION:
            handle_thrustsequence(dump);
            break;
        case State::BURN:
            handle_burn(dump);
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
        case State::ABORT_ON_GROUND:
            handle_abort_ground(dump);
            break;
        case State::ABORT_IN_FLIGHT:
            handle_abort_flight(dump);
            break;
        default:
            handle_abort_ground(dump);
            break;
    }

    read_tank_pressure();
    read_tank_temperature();
    read_copv_pressure();
    read_copv_temperature();
    read_valves();
}

void DPR::handle_init(const DataDump& dump) {
    // Write timestamp at a freq of 1Hz
    periodic_timestamp(1000);
    uint32_t default_valves(AV_NET_CMD_OFF << AV_NET_SHIFT_DN_NC
            | AV_NET_CMD_OFF << AV_NET_SHIFT_PX_NC
            | AV_NET_CMD_OFF << AV_NET_SHIFT_VX_NO);
    write_valves(default_valves);
}

void DPR::handle_calibration(const DataDump& dump) {
    // Write timestamp at a freq of 1Hz
    periodic_timestamp(1000);

    // DPR will be in idle mode (ready)
    bool dpr_ready(true);
    Data::GoatReg gr(m_address == AV_NET_ADDR_DPR_ETH ? Data::EVENT_DPR_ETH_READY : Data::EVENT_DPR_LOX_READY);
    Data::get_instance().write(gr, &dpr_ready);
}

void DPR::handle_filling(const DataDump& dump) {
    periodic_timestamp(500);

    uint32_t valves(read_valves());
    if (dump.event.command_updated) {
        uint32_t cmd(0);
        const bool value(dump.telemetry_cmd.value);
        switch (dump.telemetry_cmd.id) {
            // TODO: add PE, PO and DN valves to RF_Protocol_Interface CMD_ID enum
		case CMD_ID::AV_CMD_P_LOX:
			if (m_address == AV_NET_ADDR_DPR_LOX) {
				if (value ^ 0) {
					cmd = AV_NET_CMD_ON << AV_NET_SHIFT_PX_NC;
				}else {
					cmd = AV_NET_CMD_OFF << AV_NET_SHIFT_PX_NC;
				}
                valves &= ~(0xFF << AV_NET_SHIFT_PX_NC);
                valves |= cmd;
				write_valves(valves);
			}
			break;
		case CMD_ID::AV_CMD_P_FUEL:
			if (m_address == AV_NET_ADDR_DPR_ETH) {
                if (value ^ 0) {
                    cmd = AV_NET_CMD_ON << AV_NET_SHIFT_PX_NC;
                }else {
                    cmd = AV_NET_CMD_OFF << AV_NET_SHIFT_PX_NC;
                }
                valves &= ~(0xFF << AV_NET_SHIFT_PX_NC);
                valves |= cmd;
				write_valves(valves);
			}
			break;
        case CMD_ID::AV_CMD_VENT_LOX:
			if (m_address == AV_NET_ADDR_DPR_LOX) {
				if (value ^ 1) {
					cmd = AV_NET_CMD_ON << AV_NET_SHIFT_VX_NO;
				}else {
					cmd = AV_NET_CMD_OFF << AV_NET_SHIFT_VX_NO;
				}
                valves &= ~(0xFF << AV_NET_SHIFT_VX_NO);
                valves |= cmd;
				write_valves(valves);
			}
			break;
        case CMD_ID::AV_CMD_VENT_FUEL:
			if (m_address == AV_NET_ADDR_DPR_ETH) {
				if (value ^ 1) {
					cmd = AV_NET_CMD_ON << AV_NET_SHIFT_VX_NO;
				}else {
					cmd = AV_NET_CMD_OFF << AV_NET_SHIFT_VX_NO;
				}
                valves &= ~(0xFF << AV_NET_SHIFT_VX_NO);
                valves |= cmd;
				write_valves(valves);
			}
			break;
        case CMD_ID::AV_CMD_VENT_N2:
            if (m_address == AV_NET_ADDR_DPR_ETH) {
                if (value ^ 0) {
                    cmd = AV_NET_CMD_ON << AV_NET_SHIFT_DN_NC;
                }else {
                    cmd = AV_NET_CMD_OFF << AV_NET_SHIFT_DN_NC;
                }
                valves &= ~(0xFF << AV_NET_SHIFT_DN_NC);
                valves |= cmd;
                write_valves(valves);
            }
        default:
            break;
        }
    }
}

void DPR::handle_armed(const DataDump& dump) {
    // Write timestamp at a freq of 1Hz
    periodic_timestamp(1000);

    const bool dpr_ready(m_address == AV_NET_ADDR_DPR_ETH ? dump.event.dpr_eth_ready : dump.event.dpr_lox_ready);
    if (dpr_ready && dump.event.command_updated) {
        if (dump.telemetry_cmd.id == CMD_ID::AV_CMD_PRESSURIZE) {
            send_pressurize();
        }
    }
}

void DPR::handle_pressurized(const DataDump& dump) {
    // Write timestamp at a freq of 1Hz
    periodic_timestamp(1000);
}

void DPR::handle_thrustsequence(const DataDump& dump) {
    // Write timestamp at a freq of 1Hz
    periodic_timestamp(1000);
}

void DPR::handle_burn(const DataDump& dump) {
    // Write timestamp at a freq of 1Hz
    periodic_timestamp(1000);
}

void DPR::handle_ascent(const DataDump& dump) {
    // Write timestamp at a freq of 1Hz
    periodic_timestamp(1000);
}

void DPR::handle_descent(const DataDump& dump) {
    // Write timestamp at a freq of 1Hz
    periodic_timestamp(1000);
}

void DPR::handle_landed(const DataDump& dump) {
    // Write timestamp at a freq of 1Hz
    periodic_timestamp(1000);
}

void DPR::handle_abort_ground(const DataDump& dump) {
    // Write timestamp at a freq of 1Hz
    periodic_timestamp(1000);
}

void DPR::handle_abort_flight(const DataDump& dump) {
    // Write timestamp at a freq of 1Hz
    periodic_timestamp(1000);
}

inline void DPR::periodic_timestamp(const uint32_t period) {
    if (count_ms >= period) {
        write_timestamp();
        count_ms = 0;
    }
}
