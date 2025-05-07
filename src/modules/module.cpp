
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
