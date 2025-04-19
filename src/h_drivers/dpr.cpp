
#include "dpr.h"
#include "i2c_wrappers.h"
#include <PacketDefinition_Firehorn.h>

/*
 * I2C Init / Close
 */

uint8_t DPR_Driver::init () {
    if (i2c_open(DPR_ADDRESS) != 0)
        return DPR_FAIL_I2C_OPEN;

    if (get_intf_ptr(DPR_ADDRESS, &this->intf_ptr) != 0)
        return DPR_FAIL_I2C_OPEN;

    return 0;
}
uint8_t DPR_Driver::close () {
    if (this->intf_ptr == nullptr)
        return 0;

    if (i2c_close( DPR_ADDRESS ) != 0)
        return DPR_FAIL_I2C_CLOSE;

    this->intf_ptr = 0;
    return 0;
}

/*
 * I2C Read / Write
 */

uint8_t DPR_Driver::read_reg (uint8_t reg_addr, uint8_t* reg_data, uint32_t len) {
    if (this->intf_ptr == nullptr)
        return DPR_FAIL_I2C_NOT_OPEN;

    if (i2c_read( reg_addr, reg_data, len, this->intf_ptr ) != 0)
        return DPR_FAIL_I2C_READ;

    return 0;
}
uint8_t DPR_Driver::write_reg (uint8_t reg_addr, const uint8_t* reg_data, uint32_t len) {
    if (this->intf_ptr == nullptr)
        return DPR_FAIL_I2C_NOT_OPEN;

    if (i2c_write( reg_addr, reg_data, len, this->intf_ptr ) != 0)
        return DPR_FAIL_I2C_WRITE;

    return 0;
}

void DPR_Driver::check_policy(Data::GoatReg reg, const DataDump& dump) {
    switch (dump.av_state) {
        case State::CALIBRATION:
            wakeUp();
            break;
        case State::MANUAL:
            processManualMode(dump);
            break;
        case State::ARMED:
            if (dump.event.command_updated
             && dump.telemetry_cmd.id == CMD_ID::AV_CMD_PRESSURIZE)
                pressurize();

            break ;
        default:
            break;
    }
}

void DPR_Driver::processManualMode(const DataDump& dump) {
    if (dump.event.command_updated
     && dump.telemetry_cmd.id == CMD_ID::AV_CMD_ABORT) 
        abort();
}

void DPR_Driver::wakeUp () {
    uint8_t wakeUpBuffer[1] = { 42 };
    write_reg( (uint8_t) DPR_Commands::WAKE_UP, wakeUpBuffer, 1 );
}
void DPR_Driver::pressurize () {
    uint8_t pressurizeBuffer[1] = { 42 };
    write_reg( (uint8_t) DPR_Commands::PRESSURIZE, pressurizeBuffer, 1 );
}
void DPR_Driver::abort () {
    uint8_t abortBuffer[1] = { 42 };
    write_reg( (uint8_t) DPR_Commands::ABORT, abortBuffer, 1 );
}

float DPR_Driver::read_pressure () {
    float res;

    read_reg( (uint8_t) DPR_Commands::PRESSURE, (uint8_t*) (&res), 4);
    return res;
}
bool DPR_Driver::read_dn_nc () {
    uint8_t res;
    read_reg( (uint8_t) DPR_Commands::DN_NC, &res, 1 );

    return res != 0;
}