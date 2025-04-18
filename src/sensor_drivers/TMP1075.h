/**
# ##### BEGIN GPL LICENSE BLOCK #####
#
# Copyright (C) 2020  Patrick Baus
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# ##### END GPL LICENSE BLOCK #####
*/
#ifndef TMP1075_H
#define TMP1075_H

#include <stdint.h> // uint16_t, etc.
#include <exception>
#include <string>

#define TMP1075_ADDR_I2C 0x4E

enum TMP1075_ConsecutiveFaults : uint8_t {
    Fault_1 = 0b00,
    Fault_2 = 0b01,
    Fault_3 = 0b10,
    Fault_4 = 0b11,
};

enum TMP1075_ConversionTime : uint8_t {
    ConversionTime27_5ms = 0b00,
    ConversionTime55ms = 0b01,
    ConversionTime110ms = 0b10,
    ConversionTime220ms = 0b11,
};

enum TMP1075_Offsets : uint8_t {
    OS = 7,
    R = 5,
    F = 3,
    POL = 2,
    TM = 1,
    SD = 0,
};

/*!
* @brief Bus communication function pointer which should be mapped to
* the platform specific read functions of the user
*
* @param[in]     reg_addr : 8bit register address of the sensor
* @param[out]    reg_data : Data from the specified address
* @param[in]     length   : Length of the reg_data array
* @param[in,out] intf_ptr : Void pointer that can enable the linking of descriptors
*                           for interface related callbacks
* @retval 0 for Success
* @retval Non-zero for Failure
*/
typedef int8_t (*tmp1075_read_fptr_t)(uint8_t reg, uint8_t *data, uint32_t len, void *intf_ptr);

/*!
* @brief Bus communication function pointer which should be mapped to
* the platform specific write functions of the user
*
* @param[in]     reg_addr : 8bit register address of the sensor
* @param[out]    reg_data : Data to the specified address
* @param[in]     length   : Length of the reg_data array
* @param[in,out] intf_ptr : Void pointer that can enable the linking of descriptors
*                           for interface related callbacks
* @retval 0 for Success
* @retval Non-zero for Failure
*
*/
typedef int8_t (*tmp1075_write_fptr_t)(uint8_t reg, const uint8_t *data, uint32_t len, void *intf_ptr);

/*!
* @brief Delay function pointer which should be mapped to
* delay function of the user
*
* @param[in] period              : Delay in microseconds.
* @param[in, out] intf_ptr       : Void pointer that can enable the linking of descriptors
*                                  for interface related call backs
*
*/
typedef void (*tmp1075_delay_us_fptr_t)(uint32_t period, void *intf_ptr);

class TMP1075
{
public:
    // i2cAddress is the default address when A0, A1 and A2 is tied low
    explicit TMP1075(uint8_t addr = 0x48);
    
    int16_t getTemperatureRaw();
    float getTemperatureCelsius();

    void startConversion();
    bool getConversionMode() const;
    void setConversionMode(const bool isSingleShot = false);
    TMP1075_ConversionTime getConversionTime() const;
    void setConversionTime(TMP1075_ConversionTime value = ConversionTime27_5ms);

    TMP1075_ConsecutiveFaults getFaultsUntilAlert() const;
    void setFaultsUntilAlert(TMP1075_ConsecutiveFaults value = Fault_1);
    bool getAlertPolarity() const;
    void setAlertPolarity(const bool isHigh = false);
    bool getAlertMode() const;
    void setAlertMode(const bool isInterrupt = false);

    int16_t getLowTemperatureLimitRaw();
    float getLowTemperatureLimitCelsius();
    void setLowTemperatureLimitRaw(int16_t value = 0x4b00);
    void setLowTemperatureLimitCelsius(float value = 75.f);

    int16_t getHighTemperatureLimitRaw();
    float getHighTemperatureLimitCelsius();
    void setHighTemperatureLimitRaw(int16_t value = 0x5000);
    void setHighTemperatureLimitCelsius(float value = 80.f);

    uint16_t getDeviceId();
    
private:
    uint16_t readRegister(uint8_t reg);
    void writeRegister(uint8_t reg, uint16_t value);
    void writeRegister(uint8_t reg, uint8_t value);
    
    uint8_t i2cAddress;
    uint8_t configRegister;
    
    static float convertToCelsius(int16_t value);
    static int16_t convertFromCelsius(float value);
    
    tmp1075_read_fptr_t _read;
    tmp1075_write_fptr_t _write;
    tmp1075_delay_us_fptr_t _delay_us;
    
    void *_intf_ptr;
};

class TMP1075Exception : public std::exception {
public:
    TMP1075Exception(const std::string& msg) {
        message = msg;
    }
    
    virtual const char *what() const throw() {
        return message.c_str();
    }

private:
    std::string message;
};

#endif // TMP1075_H
