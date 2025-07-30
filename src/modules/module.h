#ifndef MODULE_H
#define MODULE_H

#include <string>

struct DataDump;

class Module {
public:
    Module(std::string name, std::string config_target);

    inline bool is_enabled() const { return _M_is_enabled; }
    inline bool is_failure() const { return _M_is_failure; }

    inline std::string get_name() const { return name; }

    /**
     * Initializes the module. If the module isn't enabled, the run_init() method
     * wont be called.
     */
    void init();
    /**
     * Runs the check_policy method on the module. If the module isn't enabled or
     * has failed, the method wont be called. In particular, if the method fails,
     * the method wont be called again.
     */
    void check_policy(const DataDump& dump, const uint32_t delta_ms);

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
};

class SensorModule : public Module {
private:
    uint32_t last_polling_time = 0;

    virtual bool run_update () = 0;
    virtual bool run_calibration () = 0;

    // No cooldown.
    virtual int low_polling_time  () { return 0; };
    virtual int high_polling_time () { return 0; };
    
    bool run_check_policy (const DataDump& dump, const uint32_t delta_ms) override;
public:
    SensorModule (std::string name, std::string config_target) 
    : Module(name, config_target) {}
};

#endif /* MODULE_H */
