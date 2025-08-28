#include "module.h"
#include "dynconf.h"
#include "logger.h"
#include "data.h"

Module::Module(std::string name, std::string config_target) {
    this->name = name;
    this->_M_is_failure = false;
    this->_M_is_enabled = ConfigManager::isEnabled(config_target, true);

    if (this->_M_is_enabled) {
        Logger::log_eventf("Module %s is enabled...", this->get_name().c_str());
    }else {
        Logger::log_eventf("Module %s is disabled...", this->get_name().c_str());
    }
}

void Module::init() {
    if (this->_M_is_enabled) {
        try {
            Logger::log_eventf("Init module %s...", this->get_name().c_str());

            this->_M_is_failure = !this->run_init();
        }catch (std::exception &exc) {
            this->_M_is_failure = true;
            
            Logger::log_eventf(Logger::ERROR, "Module %s :: init failed", this->get_name().c_str());
        }
    }
}

void Module::check_policy(const DataDump& dump, const uint32_t delta_ms) {
    if (this->_M_is_enabled && !this->_M_is_failure) {
        try {
            this->_M_is_failure = !run_check_policy(dump, delta_ms);
        } catch (std::exception &exc) {
            this->_M_is_failure = true;
            Logger::log_eventf(Logger::ERROR, "Module %s :: checkPolicy failed", this->get_name().c_str());
        }
    }
}

bool SensorModule::run_check_policy(const DataDump& dump, const uint32_t delta_ms) {
    int polling_time = -1;
    switch (dump.av_state) {
        case State::INIT:
        case State::FILLING:
        case State::ARMED:
        case State::CALIBRATION:
            polling_time = low_polling_time();
            break;
        case State::PRESSURIZED:
        case State::IGNITION:
        case State::BURN:
        case State::ASCENT:
        case State::DESCENT:
        case State::LANDED:
            polling_time = high_polling_time();
            break;
        case State::ABORT_ON_GROUND:
            break;
        case State::ABORT_IN_FLIGHT:
            break;
    }

    if (last_polling_time + polling_time <= dump.av_timestamp) {
        last_polling_time = dump.av_timestamp;

        try {
            if (!run_update()) {
                return false;
            }
        }catch (std::exception &exc) {
            Logger::log_eventf(Logger::ERROR, "Module %s :: update failed", this->get_name().c_str());

            throw exc;
        }
    }

    if (dump.av_state != State::CALIBRATION || dump.event.calibrated) {
        return true;
    }

    try {
        return run_calibration();
    }catch (std::exception &exc) {
        Logger::log_eventf(Logger::ERROR, "Module %s :: calibration failed", this->get_name().c_str());

        throw exc;
    }
}
