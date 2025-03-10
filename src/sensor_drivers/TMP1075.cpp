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

TMP1075::TMP1075(uint8_t addr)
    : i2cAddress(addr), configRegister(0x00FF),
      _read(nullptr), _write(nullptr), _delay_us(nullptr), _intf_ptr(nullptr), initialized(false)
{
    if (i2c_open(i2cAddress) != 0){
        return;
    }
    _read = i2c_read;
    _write = i2c_write;
    _delay_us = i2c_delay_us;

    if (get_intf_ptr(i2cAddress, &_intf_ptr) != 0){
        return;
    }
    this->configRegister = (uint8_t)(this->readRegister(0x01) >> 8);
    this->initialized = true;
}

bool TMP1075::isInitialized() const
{
    return this->initialized;
}
void TMP1075::writeRegister(const uint8_t pointerRegister, const uint16_t value)
{
    uint8_t buffer[2];
    buffer[0] = (value >> 8) & 0xFF;
    buffer[1] = value & 0xFF;
    _write(pointerRegister, buffer, 2, _intf_ptr);
}

void TMP1075::writeRegister(const uint8_t pointerRegister, const uint8_t value)
{
    _write(pointerRegister, &value, 1, _intf_ptr);
}

uint16_t TMP1075::readRegister(const uint8_t pointerRegister)
{
    uint8_t buffer[2] = {0};
    _write(pointerRegister, nullptr, 0, _intf_ptr);
    _read(pointerRegister, buffer, 2, _intf_ptr);
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
    this->writeRegister(0x01, (uint8_t)(this->configRegister | (1 << Offsets::OS)));
}

bool TMP1075::getConversionMode() const
{
    return (this->configRegister >> Offsets::SD) & 0b1;
}

void TMP1075::setConversionMode(const bool isSingleShot)
{
    if (((configRegister >> Offsets::SD) & 0b1) != isSingleShot)
    {
        this->configRegister = (this->configRegister & ~(1 << Offsets::SD)) | (isSingleShot << Offsets::SD);
        this->writeRegister(0x01, this->configRegister);
    }
}

ConversionTime TMP1075::getConversionTime() const
{
    return ConversionTime((this->configRegister >> Offsets::R) & 0b11);
}

void TMP1075::setConversionTime(const ConversionTime value)
{
    if (ConversionTime((configRegister >> Offsets::R) & 0b11) != value)
    {
        this->configRegister = (this->configRegister & ~(11 << Offsets::R)) | (value << Offsets::R);
        this->writeRegister(0x01, this->configRegister);
    }
}

ConsecutiveFaults TMP1075::getFaultsUntilAlert() const
{
    return ConsecutiveFaults((this->configRegister >> Offsets::F) & 0b11);
}

void TMP1075::setFaultsUntilAlert(const ConsecutiveFaults value)
{
    if (ConsecutiveFaults((configRegister >> Offsets::F) & 0b11) != value)
    {
        this->configRegister = (this->configRegister & ~(11 << Offsets::F)) | (value << Offsets::F);
        this->writeRegister(0x01, this->configRegister);
    }
}

bool TMP1075::getAlertPolarity() const
{
    return (this->configRegister >> Offsets::POL) & 0b1;
}

void TMP1075::setAlertPolarity(const bool isHigh)
{
    if (((configRegister >> Offsets::POL) & 0b1) != isHigh)
    {
        this->configRegister = (this->configRegister & ~(1 << Offsets::POL)) | (isHigh << Offsets::POL);
        this->writeRegister(0x01, this->configRegister);
    }
}

bool TMP1075::getAlertMode() const
{
    return (this->configRegister >> Offsets::TM) & 0b1;
}

void TMP1075::setAlertMode(const bool isInterrupt)
{
    if (((configRegister >> Offsets::TM) & 0b1) != isInterrupt)
    {
        this->configRegister = (this->configRegister & ~(1 << Offsets::TM)) | (isInterrupt << Offsets::TM);
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