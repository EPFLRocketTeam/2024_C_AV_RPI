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
#include "TMP1075.h"
#include "i2c_wrappers.h"
#include "config.h"
#include <memory>

TMP1075::TMP1075(uint8_t addr)
    : i2cAddress(addr), configRegister(0x00FF),
      _read(nullptr), _write(nullptr), _delay_us(nullptr), _intf_ptr(nullptr)
{
    if (i2c_open(i2cAddress) != 0)
    {
        throw TMP1075Exception("TMP1075 error during I2C initialization");
        return;
    }
    _read = i2c_read;
    _write = i2c_write;
    _delay_us = i2c_delay_us;

    if (get_intf_ptr(i2cAddress, &_intf_ptr) != 0)
    {
        throw TMP1075Exception("TMP1075 null pointer error");
        return;
    }
    this->configRegister = (uint8_t)(this->readRegister(0x01) >> 8);
    setHighTemperatureLimitCelsius(TMP1075_ALERT_TEMPERATURE);
}

void TMP1075::writeRegister(const uint8_t pointerRegister, const uint16_t value)
{
    uint8_t buffer[2];
    buffer[0] = (value >> 8) & 0xFF;
    buffer[1] = value & 0xFF;
    if (_write(pointerRegister, buffer, 2, _intf_ptr) != 0) {
        std::string msg("TMP1075 I2C error: failed writing to reg: ");
        throw TMP1075Exception(msg + std::to_string(pointerRegister));
    }
}

void TMP1075::writeRegister(const uint8_t pointerRegister, const uint8_t value)
{
    if (_write(pointerRegister, &value, 1, _intf_ptr) != 0) {
        std::string msg("TMP1075 I2C error: failed writing to reg: ");
        throw TMP1075Exception(msg + std::to_string(pointerRegister));
    }
}

uint16_t TMP1075::readRegister(const uint8_t pointerRegister)
{
    uint8_t buffer[2] = {0};
    if (_read(pointerRegister, buffer, 2, _intf_ptr) != 0) {
        std::string msg("TMP1075 I2C error: failed reading from reg: ");
        throw TMP1075Exception(msg + std::to_string(pointerRegister));
    }
    return ((uint16_t)buffer[0] << 8) | buffer[1];
}

int16_t TMP1075::getTemperatureRaw()
{
    return (int16_t)this->readRegister(0x00);
}

float TMP1075::getTemperatureCelsius()
{
    return TMP1075::convertToCelsius(this->getTemperatureRaw());
}

void TMP1075::startConversion()
{
    // There is no need to save the OS bit to the cache, because it will always be zero when read.
    this->writeRegister(0x01, (uint8_t)(this->configRegister | (1 << TMP1075_Offsets::OS)));
}

bool TMP1075::getConversionMode() const
{
    return (this->configRegister >> TMP1075_Offsets::SD) & 0b1;
}

void TMP1075::setConversionMode(const bool isSingleShot)
{
    if (((configRegister >> TMP1075_Offsets::SD) & 0b1) != isSingleShot)
    {
        this->configRegister = (this->configRegister & ~(1 << TMP1075_Offsets::SD)) | (isSingleShot << TMP1075_Offsets::SD);
        this->writeRegister(0x01, this->configRegister);
    }
}

TMP1075_ConversionTime TMP1075::getConversionTime() const
{
    return TMP1075_ConversionTime((this->configRegister >> TMP1075_Offsets::R) & 0b11);
}

void TMP1075::setConversionTime(const TMP1075_ConversionTime value)
{
    if (TMP1075_ConversionTime((configRegister >> TMP1075_Offsets::R) & 0b11) != value)
    {
        this->configRegister = (this->configRegister & ~(11 << TMP1075_Offsets::R)) | (value << TMP1075_Offsets::R);
        this->writeRegister(0x01, this->configRegister);
    }
}

TMP1075_ConsecutiveFaults TMP1075::getFaultsUntilAlert() const
{
    return TMP1075_ConsecutiveFaults((this->configRegister >> TMP1075_Offsets::F) & 0b11);
}

void TMP1075::setFaultsUntilAlert(const TMP1075_ConsecutiveFaults value)
{
    if (TMP1075_ConsecutiveFaults((configRegister >> TMP1075_Offsets::F) & 0b11) != value)
    {
        this->configRegister = (this->configRegister & ~(11 << TMP1075_Offsets::F)) | (value << TMP1075_Offsets::F);
        this->writeRegister(0x01, this->configRegister);
    }
}

bool TMP1075::getAlertPolarity() const
{
    return (this->configRegister >> TMP1075_Offsets::POL) & 0b1;
}

void TMP1075::setAlertPolarity(const bool isHigh)
{
    if (((configRegister >> TMP1075_Offsets::POL) & 0b1) != isHigh)
    {
        this->configRegister = (this->configRegister & ~(1 << TMP1075_Offsets::POL)) | (isHigh << TMP1075_Offsets::POL);
        this->writeRegister(0x01, this->configRegister);
    }
}

bool TMP1075::getAlertMode() const
{
    return (this->configRegister >> TMP1075_Offsets::TM) & 0b1;
}

void TMP1075::setAlertMode(const bool isInterrupt)
{
    if (((configRegister >> TMP1075_Offsets::TM) & 0b1) != isInterrupt)
    {
        this->configRegister = (this->configRegister & ~(1 << TMP1075_Offsets::TM)) | (isInterrupt << TMP1075_Offsets::TM);
        this->writeRegister(0x01, this->configRegister);
    }
}

int16_t TMP1075::getLowTemperatureLimitRaw()
{
    return (int16_t)(this->readRegister(0x02));
}

void TMP1075::setLowTemperatureLimitRaw(const int16_t value)
{
    this->writeRegister(0x02, (uint16_t)value);
}

void TMP1075::setLowTemperatureLimitCelsius(const float value)
{
    this->setLowTemperatureLimitRaw(TMP1075::convertFromCelsius(value));
}

float TMP1075::getLowTemperatureLimitCelsius()
{
    return TMP1075::convertToCelsius(this->getLowTemperatureLimitRaw());
}

int16_t TMP1075::getHighTemperatureLimitRaw()
{
    return (int16_t)this->readRegister(0x03);
}

float TMP1075::getHighTemperatureLimitCelsius()
{
    return TMP1075::convertToCelsius(this->getHighTemperatureLimitRaw());
}

void TMP1075::setHighTemperatureLimitRaw(const int16_t value)
{
    this->writeRegister(0x03, (uint16_t)value);
}

void TMP1075::setHighTemperatureLimitCelsius(const float value)
{
    this->setHighTemperatureLimitRaw(TMP1075::convertFromCelsius(value));
}

uint16_t TMP1075::getDeviceId()
{
    return (uint16_t)(this->readRegister(0x0F));
}

float TMP1075::convertToCelsius(const int16_t value)
{
    return 0.0625f * float(value) / 16;
}

int16_t TMP1075::convertFromCelsius(const float value)
{
    return (int16_t)(value * 16 / 0.0625f);
}
