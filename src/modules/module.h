
#include <string>

#include "data.h"

class Module {
private:
    std::string name;
    
    bool _M_is_enabled;
    bool _M_is_failure;
    
    /**
     * @return whether the initialization was successful.
     * If the value is false, then no run_check_policy will ever be called.
     */
    virtual bool run_init () { return true; };
    /**
     * @return whether the check_policy was successful.
     * If the value is false, it wont be run again.
     */
    virtual bool run_check_policy (const DataDump& dump, const uint32_t delta_ms) = 0;
public:
    bool is_enabled () { return _M_is_enabled; }
    bool is_failure () { return _M_is_failure; }

    Module (std::string name, std::string config_target);

    /**
     * Initializes the module. If the module isn't enabled, the run_init() method
     * wont be called.
     */
    void init ();
    /**
     * Runs the check_policy method on the module. If the module isn't enabled or
     * has failed, the method wont be called. In particular, if the method fails,
     * the method wont be called again.
     */
    void check_policy (const DataDump& dump, const uint32_t delta_ms);
};
