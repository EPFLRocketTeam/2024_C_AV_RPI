
#include "module.h"
#include "dynconf.h"
#include "logger.h"

Module::Module (std::string name, std::string config_target) {
    this->name = name;
    this->_M_is_failure = false;
    this->_M_is_enabled = ConfigManager::isEnabled(config_target, true);

    if (this->_M_is_enabled) {
        DataLogger::getInstance()
            .eventConvf("Module %s is enabled...", 0, this->get_name().c_str());
    } else {
        DataLogger::getInstance()
            .eventConvf("Module %s is disabled...", 0, this->get_name().c_str());
    }
}

void Module::init () {
    if (this->_M_is_enabled) {
        try {
            DataLogger::getInstance()
                .eventConvf("Init module %s...", 0, this->get_name().c_str());

            this->_M_is_failure = !this->run_init();
        } catch (std::exception &exc) {
            this->_M_is_failure = true;
            
            DataLogger::getInstance()
                .eventConvf("Module %s :: init failed", 0, this->get_name().c_str());
        }
    }
}

void Module::check_policy (const DataDump& dump, const uint32_t delta_ms) {
    if (this->_M_is_enabled && !this->_M_is_failure) {
        try {
            this->_M_is_failure = !run_check_policy(dump, delta_ms);
        } catch (std::exception &exc) {
            this->_M_is_failure = true;
            DataLogger::getInstance()
                .eventConvf("Module %s :: checkPolicy failed", delta_ms, this->get_name().c_str());
        }
    }
}

bool SensorModule::run_check_policy (const DataDump& dump, const uint32_t delta_ms) {
    int polling_time = -1;
    switch (dump.av_state) {
        case State::INIT:
        case State::MANUAL:
        case State::ARMED:
        case State::CALIBRATION:
            polling_time = low_polling_time();
            break;
        case State::READY:
        case State::THRUSTSEQUENCE:
        case State::LIFTOFF:
        case State::ASCENT:
        case State::DESCENT:
        case State::LANDED:
            polling_time = high_polling_time();
            break;
        case State::ERRORGROUND:
            break;
        case State::ERRORFLIGHT:
            break;
    }

    if (last_polling_time + polling_time <= dump.av_timestamp) {
        last_polling_time = dump.av_timestamp;

        try {
            if (!run_update()) {
                return false;
            }
        } catch (std::exception &exc) {
            DataLogger::getInstance()
                .eventConvf("Module %s :: update failed", delta_ms, this->get_name().c_str());

            throw exc;
        }
    }

    if (dump.av_state != State::CALIBRATION || dump.event.calibrated) {
        return true;
    }

    try {
        return run_calibration();
    } catch (std::exception &exc) {
        DataLogger::getInstance()
            .eventConvf("Module %s :: calibration failed", delta_ms, this->get_name().c_str());

        throw exc;
    }
}