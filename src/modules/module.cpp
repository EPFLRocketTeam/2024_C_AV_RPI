
#include "module.h"
#include "config.h"

Module::Module (std::string name, std::string config_target) {
    this->name = name;
    this->_M_is_failure = false;
    this->_M_is_enabled = ConfigManager::isEnabled(config_target, true);
}

void Module::init () {
    if (this->_M_is_enabled) {
        this->_M_is_failure = !this->run_init();
    }
}

void Module::check_policy (const DataDump& dump, const uint32_t delta_ms) {
    if (this->_M_is_enabled && !this->_M_is_failure) {
        this->_M_is_failure = !run_check_policy(dump, delta_ms);
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

        if (!run_update()) {
            return false;
        }
    }

    if (dump.av_state != State::CALIBRATION || dump.event.calibrated) {
        return true;
    }

    return run_calibration();
}