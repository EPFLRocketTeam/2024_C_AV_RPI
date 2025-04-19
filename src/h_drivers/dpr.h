
#ifndef DPR_DRIVER_H
#define DPR_DRIVER_H

#include "h_driver.h"

// TODO change the DPR Address
#define DPR_ADDRESS 0xFF

#define DPR_FAIL_I2C_OPEN  -1
#define DPR_FAIL_I2C_CLOSE -2
#define DPR_FAIL_I2C_NOT_OPEN -3
#define DPR_FAIL_I2C_READ -4
#define DPR_FAIL_I2C_WRITE -5

enum class DPR_Commands : uint8_t {
    // WRITE
    TIMESTAMP_MAIN_FSM,
    WAKE_UP,
    PRESSURIZE,
    ABORT,

    // READ
    INTERNAL_FSM_STATUS,
    PRESSURE,
    DN_NC
};

class DPR_Driver : public HDriver {
public:
    DPR_Driver  () = default;
    ~DPR_Driver () = default;

    uint8_t init  ();
    uint8_t close ();

    void check_policy(Data::GoatReg reg, const DataDump& dump) override;

    float read_pressure ();
    bool  read_dn_nc ();
private:
    void *intf_ptr = nullptr;

    uint8_t  read_reg (uint8_t reg_addr, uint8_t* reg_data, uint32_t len);
    uint8_t write_reg (uint8_t reg_addr, const uint8_t* reg_data, uint32_t len);

    void processManualMode(const DataDump& dump);

    void wakeUp ();
    void abort  ();
    void pressurize ();
};

#endif
