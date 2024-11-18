/*******************************************************************************
 *   @file   ADXL375.c
 *   @brief  Implementation of ADXL375 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
********************************************************************************
 *   SVN Revision: 795
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "adxl375.h"        // ADXL375 definitions.
#include "i2c_wrappers.h"    // Communication definitions.
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/


/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/*!
 * @brief This internal API is used to validate the device structure pointer for
 * null conditions.
 */
static int8_t null_ptr_check(const struct adxl375_dev *dev)
{
    int8_t rslt;

    if ((dev == NULL) || (dev->read == NULL) || (dev->write == NULL) ||
		(dev->delay_us == NULL) || (dev->intf_ptr == NULL))
    {
        /* Device structure pointer is not valid */
        rslt = ADXL375_E_NULL_PTR;
    }
    else
    {
        /* Device structure is fine */
        rslt = ADXL375_OK;
    }

    return rslt;
}

/*!
 * @brief This internal API interleaves the register address between the
 * register data buffer for burst write operation.
 */
static void interleave_reg_addr(const uint8_t *reg_addr, uint8_t *temp_buff,
								const uint8_t *reg_data, uint32_t len)
{
    uint32_t index;

    for (index = 1; index < len; index++)
    {
        temp_buff[(index * 2) - 1] = reg_addr[index];
        temp_buff[index * 2] = reg_data[index];
    }
}

/*!
 * @brief This API reads the data from the given register address of the sensor.
 */
int8_t adxl375_get_regs(uint8_t reg_addr, uint8_t *reg_data, uint32_t len,
						struct adxl375_dev *dev) {
	int8_t rslt;
    
	/* Check for null pointer in the device structure */
	rslt = null_ptr_check(dev);
	
	/* Proceed if null check is fine */
    if ((rslt != ADXL375_OK) || (reg_data == NULL)) {
		return ADXL375_E_NULL_PTR;
	}

	/* Read the data using I2C */
	dev->intf_rslt = dev->read(reg_addr, reg_data, len, dev->intf_ptr);
	
	/* Check for communication error */
	if (dev->intf_rslt != ADXL375_OK) {
		return ADXL375_E_COMM_FAIL;
	}

	return ADXL375_OK;
}

/*!
 * @brief This API writes the data from the given register address of the sensor.
 */
int8_t adxl375_set_regs(uint8_t *reg_addr, const uint8_t *reg_data,
					    int32_t len, struct adxl375_dev *dev) {
	int8_t rslt;
	uint8_t temp_buff[len * 2];
    uint32_t temp_len;
	/* Check for null pointer in the device structure */
    rslt = null_ptr_check(dev);

	/* Proceed if null check is fine */
    if ((rslt != ADXL375_OK) || (reg_addr == NULL) || (reg_data == NULL)) {
		return ADXL375_E_NULL_PTR;
	}

	if (len == 0) return ADXL375_E_INVALID_LEN;

	temp_buff[0] = reg_data[0];
	
	/* Burst write mode */
	if (len > 1) {
		/* Interleave register address w.r.t data for burst write */
		interleave_reg_addr(reg_addr, temp_buff, reg_data, len);
		temp_len = len * 2;
	} else {
		temp_len = len;
	}

	dev->intf_rslt = dev->write(reg_addr[0],temp_buff,temp_len,dev->intf_ptr);

	/* Check for communication error */
	if (dev->intf_rslt != ADXL375_OK) return ADXL375_E_COMM_FAIL;
    
	return ADXL375_OK;
}

/*!
 * @brief Initializes the communication peripheral, checks if the ADXL375
 *		  part is present and sets configuration values.
 */
int8_t adxl375_init(struct adxl375_dev *dev, uint8_t addr) {
	int8_t rslt;
    uint8_t chip_id = 0;

	/* Initialize the I2C interface */
	rslt = adxl375_i2c_init(dev, addr);
	if (rslt) return rslt;

	/* Check for null pointer in the device structure */
    rslt = null_ptr_check(dev);
	if (rslt) return rslt;

	/* Read the chip-id of adxl375 sensor */
	rslt = adxl375_get_regs(ADXL375_DEVID, &chip_id, 1, dev);
	if (rslt) return ADXL375_E_DEV_NOT_FOUND;
	
	//Configure the ADXL375
	
	// Normal Power Mode, 1600Hz operation
	uint8_t reg_addr = ADXL375_BW_RATE;
	uint8_t reg_val = (0x00|ADXL375_RATE(0x0E));
	rslt = adxl375_set_regs(&reg_addr, &reg_val,1,dev);
	if (rslt) return rslt;

	//SelfTest:Off, SPI: 4-wire, Interrupts: Active High, Full_Res: On,
	//Justify: Right justified with sign extension
	reg_addr = ADXL375_DATA_FORMAT;
	reg_val=(0x00|ADXL375_RANGE(ADXL375_RANGE_PM_2G)|ADXL375_FULL_RES);
	rslt = adxl375_set_regs(&reg_addr, &reg_val,1,dev);
	if (rslt) return rslt;

	//FIFO mode - Stream, Watermark interrupt set @ 30 (0x1E)samples -
	//which at ~100Hz(or even greater) provides sufficient time for FIFO to
	//be emptied
	reg_addr = ADXL375_FIFO_CTL;
	reg_val = (0x00|ADXL375_FIFO_MODE(ADXL375_FIFO_STREAM)|ADXL375_SAMPLES(0x1E));
	rslt = adxl375_set_regs(&reg_addr, &reg_val,1,dev);
	if (rslt) return rslt;

	//Watermark interrupt to Int 1 - all others to Int 2
	reg_addr = ADXL375_INT_MAP;
	reg_val = (0x00|~ADXL375_WATERMARK);
	rslt = adxl375_set_regs(&reg_addr, &reg_val,1,dev);
	if (rslt) return rslt;

	//Enables Watermark interrupt on Int 1 - GPIO 17
	reg_addr = ADXL375_INT_ENABLE;
	reg_val = (0x00|ADXL375_WATERMARK);
	rslt = adxl375_set_regs(&reg_addr, &reg_val,1,dev);
	if (rslt) return rslt;

	//Start the ADXL375
	rslt = adxl375_set_power_mode(0x1, dev);
	if (rslt) return rslt;

	return ADXL375_OK;
}

/*!
 * @brief Places the device into standby/measure mode.
 */
int8_t adxl375_set_power_mode(uint8_t pwr_mode, struct adxl375_dev *dev) {
	uint8_t oldPowerCtl = 0;
	uint8_t newPowerCtl = 0;
	int8_t rslt;
    
    rslt = adxl375_get_regs(ADXL375_POWER_CTL, &oldPowerCtl, 1, dev);
	if (rslt) return rslt;
	newPowerCtl = oldPowerCtl & ~ADXL375_PCTL_MEASURE;
	newPowerCtl = newPowerCtl | (pwr_mode * ADXL375_PCTL_MEASURE);

	uint8_t reg_addr = ADXL375_POWER_CTL;
	rslt = adxl375_set_regs(&reg_addr, &newPowerCtl, 1, dev);
	if (rslt) return rslt;

	return ADXL375_OK;
}

/*!
 * @brief This API gets the interrupt (fifo watermark, fifo full, data ready)
 * status from the sensor.
 */
int8_t adxl375_get_int_status(struct adxl375_dev *dev, uint8_t *status)
{
    int8_t rslt;
    uint8_t tmp;

    rslt = adxl375_get_regs(ADXL375_INT_SOURCE, &tmp, 1, dev);

    if (rslt) return rslt;
    *status = tmp;

    return rslt;
}

/*!
 * @brief Reads the output data of each axis.
 */
int8_t adxl375_get_xyz(struct adxl375_dev *dev, struct adxl375_data *data) {
	int8_t rslt;
	uint8_t values[6];
	int16_t tmp;

	/* Reading all the data at once so that the FIFO doesn't replace with newer
	 * data (read datasheet page 24). Have to first convert the values into 
	 * int16_t, then to float with the scale, hence the temporary variable.
	 */
	rslt = adxl375_get_regs(ADXL375_DATAX0, values, 6, dev);
	if (rslt) return rslt;
	tmp = (values[0] + (values[1] << 8));
	data->x = tmp;
	tmp = (values[2] + (values[3] << 8));
	data->y = tmp;
	tmp = values[4] + (values[5] << 8);
	data->z = tmp;

	return ADXL375_OK;	
}


/*!
 * @brief Enables/disables the tap detection.
 */
int8_t adxl375_set_tap_detection(struct adxl375_dev *dev,
							 uint8_t tapType,
							 uint8_t tapAxes,
							 uint8_t tapDur,
							 uint8_t tapLatent,
							 uint8_t tapWindow,
							 uint8_t tapThresh,
							 uint8_t tapInt) {
	uint8_t oldTapAxes   = 0;
	uint8_t newTapAxes   = 0;
	uint8_t oldIntMap    = 0;
	uint8_t newIntMap    = 0;
	uint8_t oldIntEnable = 0;
	uint8_t newIntEnable = 0;
    int8_t rslt;

	rslt = adxl375_get_regs(ADXL375_TAP_AXES, &oldTapAxes, 1, dev);
	if (rslt) return rslt;
	newTapAxes = oldTapAxes & ~(ADXL375_TAP_X_EN |
								ADXL375_TAP_Y_EN |
								ADXL375_TAP_Z_EN);
	newTapAxes = newTapAxes | tapAxes;

	uint8_t reg_addr = ADXL375_TAP_AXES;
	rslt = adxl375_set_regs(&reg_addr, &newTapAxes, 1, dev);
	if (rslt) return rslt;

	reg_addr = ADXL375_DUR;
	rslt = adxl375_set_regs(&reg_addr, &tapDur, 1, dev);
	if (rslt) return rslt;

	reg_addr = ADXL375_LATENT;
	rslt = adxl375_set_regs(&reg_addr, &tapLatent, 1, dev);
	if (rslt) return rslt;

	reg_addr = ADXL375_WINDOW;
	rslt = adxl375_set_regs(&reg_addr, &tapWindow, 1, dev);
	if (rslt) return rslt;

	reg_addr = ADXL375_THRESH_TAP;
	rslt = adxl375_set_regs(&reg_addr, &tapThresh, 1, dev);
	if (rslt) return rslt;

	rslt = adxl375_get_regs(ADXL375_INT_MAP, &oldIntMap, 1, dev);
	if (rslt) return rslt;

	newIntMap = oldIntMap & ~(ADXL375_SINGLE_TAP | ADXL375_DOUBLE_TAP);
	newIntMap = newIntMap | tapInt;

	reg_addr = ADXL375_INT_MAP;
	rslt = adxl375_set_regs(&reg_addr, &newIntMap, 1, dev);
	if (rslt) return rslt;

	rslt = adxl375_get_regs(ADXL375_INT_ENABLE, &oldIntEnable, 1, dev);
	if (rslt) return rslt;

	newIntEnable = oldIntEnable & ~(ADXL375_SINGLE_TAP | ADXL375_DOUBLE_TAP);
	newIntEnable = newIntEnable | tapType;
	reg_addr = ADXL375_INT_ENABLE;
	rslt = adxl375_set_regs(&reg_addr, &newIntEnable, 1, dev);
	if (rslt) return rslt;

	return ADXL375_OK;
}

/*!
 * @brief Enables/disables the activity detection.
 */
int8_t adxl375_set_activity_detection(struct adxl375_dev *dev,
								  uint8_t actOnOff,
								  uint8_t actAxes,
								  uint8_t actAcDc,
								  uint8_t actThresh,
								  uint8_t actInt) {
	uint8_t oldActInactCtl = 0;
	uint8_t newActInactCtl = 0;
	uint8_t oldIntMap      = 0;
	uint8_t newIntMap      = 0;
	uint8_t oldIntEnable   = 0;
	uint8_t newIntEnable   = 0;
    int8_t rslt;

	rslt = adxl375_get_regs(ADXL375_INT_ENABLE, &oldActInactCtl, 1, dev);
	if (rslt) return rslt;

	newActInactCtl = oldActInactCtl & ~(ADXL375_ACT_ACDC |
										ADXL375_ACT_X_EN |
										ADXL375_ACT_Y_EN |
										ADXL375_ACT_Z_EN);
	newActInactCtl = newActInactCtl | (actAcDc | actAxes);
	uint8_t reg_addr = ADXL375_ACT_INACT_CTL;
	rslt = adxl375_set_regs(&reg_addr, &newActInactCtl, 1, dev);
	if (rslt) return rslt;

	reg_addr = ADXL375_THRESH_ACT;
	rslt = adxl375_set_regs(&reg_addr, &actThresh, 1, dev);
	if (rslt) return rslt;


	rslt = adxl375_get_regs(ADXL375_INT_MAP, &oldIntMap, 1, dev);
	if (rslt) return rslt;

	newIntMap = oldIntMap & ~(ADXL375_ACTIVITY);
	newIntMap = newIntMap | actInt;
	reg_addr = ADXL375_INT_MAP;
	rslt = adxl375_set_regs(&reg_addr, &newIntMap, 1, dev);
	if (rslt) return rslt;

	rslt = adxl375_get_regs(ADXL375_INT_ENABLE, &oldIntEnable, 1, dev);
	if (rslt) return rslt;

	newIntEnable = oldIntEnable & ~(ADXL375_ACTIVITY);
	newIntEnable = newIntEnable | (ADXL375_ACTIVITY * actOnOff);
	reg_addr = ADXL375_INT_ENABLE;
	rslt = adxl375_set_regs(&reg_addr, &newIntEnable, 1, dev);
	if (rslt) return rslt;

	return ADXL375_OK;
}

/*!
 * @brief Enables/disables the inactivity detection.
 */
int8_t adxl375_set_inactivity_detection(struct adxl375_dev *dev,
									uint8_t inactOnOff,
									uint8_t inactAxes,
									uint8_t inactAcDc,
									uint8_t inactThresh,
									uint8_t inactTime,
									uint8_t inactInt) {
	uint8_t oldActInactCtl = 0;
	uint8_t newActInactCtl = 0;
	uint8_t oldIntMap      = 0;
	uint8_t newIntMap      = 0;
	uint8_t oldIntEnable   = 0;
	uint8_t newIntEnable   = 0;
    int8_t rslt;

	rslt = adxl375_get_regs(ADXL375_INT_ENABLE, &oldActInactCtl, 1, dev);
	if (rslt) return rslt;

	newActInactCtl = oldActInactCtl & ~(ADXL375_ACT_ACDC |
										ADXL375_ACT_X_EN |
										ADXL375_ACT_Y_EN |
										ADXL375_ACT_Z_EN);
	newActInactCtl = newActInactCtl | (inactAcDc | inactAxes);
	uint8_t reg_addr = ADXL375_ACT_INACT_CTL;
	rslt = adxl375_set_regs(&reg_addr, &newActInactCtl, 1, dev);
	if (rslt) return rslt;

	reg_addr = ADXL375_THRESH_ACT;
	rslt = adxl375_set_regs(&reg_addr, &inactThresh, 1, dev);
	if (rslt) return rslt;

	reg_addr = ADXL375_TIME_INACT;
	rslt = adxl375_set_regs(&reg_addr, &inactTime, 1, dev);
	if (rslt) return rslt;

	rslt = adxl375_get_regs(ADXL375_INT_MAP, &oldIntMap, 1, dev);
	if (rslt) return rslt;

	newIntMap = oldIntMap & ~(ADXL375_ACTIVITY);
	newIntMap = newIntMap | inactInt;
	reg_addr = ADXL375_INT_MAP;
	rslt = adxl375_set_regs(&reg_addr, &newIntMap, 1, dev);
	if (rslt) return rslt;

	rslt = adxl375_get_regs(ADXL375_INT_ENABLE, &oldIntEnable, 1, dev);
	if (rslt) return rslt;

	newIntEnable = oldIntEnable & ~(ADXL375_ACTIVITY);
	newIntEnable = newIntEnable | (ADXL375_ACTIVITY * inactOnOff);
	reg_addr = ADXL375_INT_ENABLE;
	rslt = adxl375_set_regs(&reg_addr, &newIntEnable, 1, dev);
	if (rslt) return rslt;

	return ADXL375_OK;
}

/*!
 * @brief Enables/disables the free-fall detection.
 */
int8_t adxl375_set_freefall_detection(struct adxl375_dev *dev,
								  uint8_t ffOnOff,
								  uint8_t ffThresh,
								  uint8_t ffTime,
								  uint8_t ffInt) {
	uint8_t oldIntMap    = 0;
	uint8_t newIntMap    = 0;
	uint8_t oldIntEnable = 0;
	uint8_t newIntEnable = 0;
    int8_t rslt;

	uint8_t reg_addr = ADXL375_THRESH_FF;
	rslt = adxl375_set_regs(&reg_addr, &ffThresh, 1, dev);
	if (rslt) return rslt;

	reg_addr = ADXL375_TIME_FF;
	rslt = adxl375_set_regs(&reg_addr, &ffTime, 1, dev);
	if (rslt) return rslt;

	rslt = adxl375_get_regs(ADXL375_INT_MAP, &oldIntMap, 1, dev);
	if (rslt) return rslt;

	newIntMap = oldIntMap & ~(ADXL375_FREE_FALL);
	newIntMap = newIntMap | ffInt;
	reg_addr = ADXL375_INT_MAP;
	rslt = adxl375_set_regs(&reg_addr, &newIntMap, 1, dev);
	if (rslt) return rslt;

	rslt = adxl375_get_regs(ADXL375_INT_ENABLE, &oldIntEnable, 1, dev);
	if (rslt) return rslt;

	newIntEnable = oldIntEnable & ~ADXL375_FREE_FALL;
	newIntEnable = newIntEnable | (ADXL375_FREE_FALL * ffOnOff);
	reg_addr = ADXL375_INT_ENABLE;
	rslt = adxl375_set_regs(&reg_addr, &newIntEnable, 1, dev);
	if (rslt) return rslt;

	return ADXL375_OK;
}

/*!
 * @brief Sets an offset value for each axis (Offset Calibration).
 */
int8_t adxl375_set_offset(struct adxl375_dev *dev,
					   uint8_t xOffset,
					   uint8_t yOffset,
					   uint8_t zOffset) {
	int8_t rslt;
	uint8_t reg_addr = ADXL375_OFSX;
	rslt = adxl375_set_regs(&reg_addr, &xOffset, 1, dev);
	if (rslt) return rslt;

	reg_addr = ADXL375_OFSY;
	rslt = adxl375_set_regs(&reg_addr, &yOffset, 1, dev);
	if (rslt) return rslt;

	reg_addr = ADXL375_OFSZ;
	rslt = adxl375_set_regs(&reg_addr, &zOffset, 1, dev);
	if (rslt) return rslt;

	return ADXL375_OK; 
}

/*!
 *  @brief Function to initialise the I2C interface, with functions from
 *         i2c_wrappers.h
 */
int8_t adxl375_i2c_init(struct adxl375_dev *dev, uint8_t addr) {
    if (i2c_open(addr) != 0) {
        return ADXL375_E_COMM_FAIL;
    }

    /* To initialize the user I2C function */
    dev->read = i2c_read;
    dev->write = i2c_write;
    
    /* Configure delay in microseconds */
    dev->delay_us = i2c_delay_us;

    /* Assign device address to interface pointer */
    if (get_intf_ptr(addr, &(dev->intf_ptr)) != 0) {
        return ADXL375_E_COMM_FAIL;
    }

    return ADXL375_OK;
}

/*!
 *  @brief Function to deinitialise the I2C interface, with functions from
 *         i2c_wrappers.h
 */
int8_t adxl375_i2c_deinit(uint8_t addr) {
    if(i2c_close(addr) != 0) {
        return ADXL375_E_COMM_FAIL;
    }

    return ADXL375_OK;
}

//Initialises device.
Adxl375::Adxl375(uint8_t addr) : addr(addr) {
	int8_t rslt = adxl375_init(&dev, addr);
	if (rslt != ADXL375_OK) {
		throw Adxl375Exception(rslt);
	}
}

Adxl375::~Adxl375() noexcept {
	int8_t rslt = adxl375_i2c_deinit(addr);
	if (rslt != ADXL375_OK) {
		printf("Error during ADXL375 I2C deinitialization: %d\n", rslt);
	}
}

//Returns interrupt status
uint8_t Adxl375::get_status() {
	int8_t rslt = adxl375_get_int_status(&dev, &status);
	if (rslt != ADXL375_OK) {
		throw Adxl375Exception(rslt);
	}
	return status;
}

//Returns data in m/s^2
adxl375_data Adxl375::get_data() {
	adxl375_data xyz;
	int8_t rslt = adxl375_get_xyz(&dev, &xyz);
	if (rslt != ADXL375_OK) {
		throw Adxl375Exception(rslt);
	}

	xyz.x = (xyz.x - offset.x)*scale*GRAVITY_CONSTANT;
	xyz.y = (xyz.y - offset.y)*scale*GRAVITY_CONSTANT;
	xyz.z = (xyz.z - offset.z)*scale*GRAVITY_CONSTANT;
	data = xyz;
	
	return data;
}

void Adxl375::set_offset(adxl375_data offset_in) {
	offset = offset_in;
}

void Adxl375::set_scale(float scale_in) {
	scale = scale_in;
}

// Calibrate the offset. The scale is not calibrated.
void Adxl375::calibrate() {

	float samples = 0;
    adxl375_data sum = {0, 0, 0};
	int8_t rslt;

    // Sum up samples for averaging
    while (samples < 1000) {
		if (this->get_status() & ADXL375_DATA_READY) {
			int8_t rslt = adxl375_get_xyz(&dev, &data);
			if (rslt != ADXL375_OK) {
				throw Adxl375Exception(rslt);
			}
			sum.x += data.x;
			sum.y += data.y;
			sum.z += data.z - 1/scale; //removing gravity
			samples++;
		}
		usleep(5);
    }
	// Compute average offset
    offset.x = sum.x / samples;
	offset.y = sum.y / samples;
	offset.z = sum.z / samples;
}