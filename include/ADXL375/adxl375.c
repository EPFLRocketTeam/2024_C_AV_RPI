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
#include "adxl375.h"		// ADXL375 definitions.
#include "i2c_common.h"	// Communication definitions.
#include <stdint.h>
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

/***************************************************************************//**
 * @brief Initializes the communication peripheral and checks if the ADXL375
 *		  part is present.
 *
 * @param addr - device I2C address
 *
 * @return status - Result of the initialization procedure.
 *                  Example: 0x0 - I2C/SPI peripheral was not initialized or
 *                                 ADXL375 part is not present.
 *                           0x1 - I2C/SPI peripheral is initialized and ADXL375
 *                                 part is present.
*******************************************************************************/
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
	
	// Normal Power Mode, 100Hz operation
	uint8_t reg_addr = ADXL375_BW_RATE;
	uint8_t reg_val = (0x00|ADXL375_RATE(0x0A));
	adxl375_set_regs(&reg_addr, &reg_val,1,dev);

	//SelfTest:Off, SPI: 4-wire, Interrupts: Active High, Full_Res: On,
	//Justify: Right justified with sign extension
	reg_addr = ADXL375_DATA_FORMAT;
	reg_val=(0x00|ADXL375_RANGE(ADXL375_RANGE_PM_2G)|ADXL375_FULL_RES);
	adxl375_set_regs(&reg_addr, &reg_val,1,dev);

	//FIFO mode - Stream, Watermark interrupt set @ 30 (0x1E)samples -
	//which at ~100Hz(or even greater) provides sufficient time for FIFO to
	//be emptied
	reg_addr = ADXL375_FIFO_CTL;
	reg_val = (0x00|ADXL375_FIFO_MODE(0x2)|ADXL375_SAMPLES(0x1E));
	adxl375_set_regs(&reg_addr, &reg_val,1,dev);

	//Watermark interrupt to Int 1 - all others to Int 2
	reg_addr = ADXL375_INT_MAP;
	reg_val = (0x00|~ADXL375_WATERMARK);
	adxl375_set_regs(&reg_addr, &reg_val,1,dev);

	//Enables Watermark interrupt on Int 1 - GPIO 17
	reg_addr = ADXL375_INT_ENABLE;
	reg_val = (0x00|ADXL375_WATERMARK);
	adxl375_set_regs(&reg_addr, &reg_val,1,dev);

	//Start the ADXL375
	adxl375_set_power_mode(0x1, dev);

	return ADXL375_OK;
}

/***************************************************************************//**
 * @brief Places the device into standby/measure mode.
 *
 * @param pwrMode - Power mode.
 *					Example: 0x0 - standby mode.
 *							 0x1 - measure mode.
 *
 * @return None.
*******************************************************************************/
int8_t adxl375_set_power_mode(uint8_t pwrMode, struct adxl375_dev *dev)
{
	uint8_t oldPowerCtl = 0;
	uint8_t newPowerCtl = 0;
	int8_t rslt;
    
    rslt = adxl375_get_regs(ADXL375_POWER_CTL, &oldPowerCtl, 1, dev);
	if (rslt) return rslt;
	newPowerCtl = oldPowerCtl & ~ADXL375_PCTL_MEASURE;
	newPowerCtl = newPowerCtl | (pwrMode * ADXL375_PCTL_MEASURE);

	uint8_t reg_addr = ADXL375_POWER_CTL;
	rslt = adxl375_set_regs(&reg_addr, &newPowerCtl, 1, dev);
	if (rslt) return rslt;
}

/***************************************************************************//**
 * @brief Reads the output data of each axis.
 *
 * @param x - X-axis's output data.
 * @param y - Y-axis's output data.
 * @param z - Z-axis's output data.
 *
 * @return None.
*******************************************************************************/
void adxl375_get_xyz(int16_t* x,
					int16_t* y,
					int16_t* z)
{
	*x = adxl375_get_regs(ADXL375_DATAX1) << 8;
	*x += adxl375_get_regs(ADXL375_DATAX0);
	*y = adxl375_get_regs(ADXL375_DATAY1) << 8;
	*y += adxl375_get_regs(ADXL375_DATAY0);
	*z = adxl375_get_regs(ADXL375_DATAZ1) << 8;
	*z += adxl375_get_regs(ADXL375_DATAZ0);
}


/***************************************************************************//**
 * @brief Enables/disables the tap detection.
 *
 * @param tapType - Tap type (none, single, double).
 *					Example: 0x0 - disables tap detection.	
 *							 ADXL375_SINGLE_TAP - enables single tap detection.
 *							 ADXL375_DOUBLE_TAP - enables double tap detection.
 * @param tapAxes - Axes which participate in tap detection.
 *					Example: 0x0 - disables axes participation.
 *							 ADXL375_TAP_X_EN - enables x-axis participation.
 *							 ADXL375_TAP_Y_EN - enables y-axis participation.
 *							 ADXL375_TAP_Z_EN - enables z-axis participation.
 * @param tapDur - Tap duration. The scale factor is 625us is/LSB.
 * @param tapLatent - Tap latency. The scale factor is 1.25 ms/LSB.
 * @param tapWindow - Tap window. The scale factor is 1.25 ms/LSB.
 * @param tapThresh - Tap threshold. The scale factor is 62.5 mg/LSB.
 * @param tapInt - Interrupts pin.
 *				   Example: 0x0 - interrupts on INT1 pin.
 *							ADXL375_SINGLE_TAP - single tap interrupts on
 *												 INT2 pin.
 *							ADXL375_DOUBLE_TAP - double tap interrupts on
 *												 INT2 pin.
 *
 * @return None.
*******************************************************************************/
void adxl375_set_tap_detection(unsigned char tapType,
							 unsigned char tapAxes,
							 unsigned char tapDur,
							 unsigned char tapLatent,
							 unsigned char tapWindow,
							 unsigned char tapThresh,
							 unsigned char tapInt)
{
	unsigned char oldTapAxes   = 0;
	unsigned char newTapAxes   = 0;
	unsigned char oldIntMap    = 0;
	unsigned char newIntMap    = 0;
	unsigned char oldIntEnable = 0;
	unsigned char newIntEnable = 0;
    
	oldTapAxes = adxl375_get_regs(ADXL375_TAP_AXES);
	newTapAxes = oldTapAxes & ~(ADXL375_TAP_X_EN |
								ADXL375_TAP_Y_EN |
								ADXL375_TAP_Z_EN);
	newTapAxes = newTapAxes | tapAxes;
	adxl375_set_regs(ADXL375_TAP_AXES, newTapAxes);
	adxl375_set_regs(ADXL375_DUR, tapDur);
	adxl375_set_regs(ADXL375_LATENT, tapLatent);
	adxl375_set_regs(ADXL375_WINDOW, tapWindow);
	adxl375_set_regs(ADXL375_THRESH_TAP, tapThresh);
	oldIntMap = adxl375_get_regs(ADXL375_INT_MAP);
	newIntMap = oldIntMap & ~(ADXL375_SINGLE_TAP | ADXL375_DOUBLE_TAP);
	newIntMap = newIntMap | tapInt;
	adxl375_set_regs(ADXL375_INT_MAP, newIntMap);
	oldIntEnable = adxl375_get_regs(ADXL375_INT_ENABLE);
	newIntEnable = oldIntEnable & ~(ADXL375_SINGLE_TAP | ADXL375_DOUBLE_TAP);
	newIntEnable = newIntEnable | tapType;
	adxl375_set_regs(ADXL375_INT_ENABLE, newIntEnable);
}

/***************************************************************************//**
 * @brief Enables/disables the activity detection.
 *
 * @param actOnOff - Enables/disables the activity detection.
 *					 Example: 0x0 - disables the activity detection.
 *							  0x1 - enables the activity detection.
 * @param actAxes - Axes which participate in detecting activity.
 *					Example: 0x0 - disables axes participation.
 *							 ADXL375_ACT_X_EN - enables x-axis participation.
 *							 ADXL375_ACT_Y_EN - enables y-axis participation.
 *							 ADXL375_ACT_Z_EN - enables z-axis participation.
 * @param actAcDc - Selects dc-coupled or ac-coupled operation.
 *					Example: 0x0 - dc-coupled operation.
 *							 ADXL375_ACT_ACDC - ac-coupled operation.
 * @param actThresh - Threshold value for detecting activity. The scale factor 
                      is 62.5 mg/LSB.
 * @patam actInt - Interrupts pin.
 *				   Example: 0x0 - activity interrupts on INT1 pin.
 *							ADXL375_ACTIVITY - activity interrupts on INT2 pin.
 *
 * @return None.
*******************************************************************************/
void adxl375_set_activity_detection(unsigned char actOnOff,
								  unsigned char actAxes,
								  unsigned char actAcDc,
								  unsigned char actThresh,
								  unsigned char actInt)
{
	unsigned char oldActInactCtl = 0;
	unsigned char newActInactCtl = 0;
	unsigned char oldIntMap      = 0;
	unsigned char newIntMap      = 0;
	unsigned char oldIntEnable   = 0;
	unsigned char newIntEnable   = 0;
    
	oldActInactCtl = adxl375_get_regs(ADXL375_INT_ENABLE);
	newActInactCtl = oldActInactCtl & ~(ADXL375_ACT_ACDC |
										ADXL375_ACT_X_EN |
										ADXL375_ACT_Y_EN |
										ADXL375_ACT_Z_EN);
	newActInactCtl = newActInactCtl | (actAcDc | actAxes);
	adxl375_set_regs(ADXL375_ACT_INACT_CTL, newActInactCtl);
	adxl375_set_regs(ADXL375_THRESH_ACT, actThresh);
	oldIntMap = adxl375_get_regs(ADXL375_INT_MAP);
	newIntMap = oldIntMap & ~(ADXL375_ACTIVITY);
	newIntMap = newIntMap | actInt;
	adxl375_set_regs(ADXL375_INT_MAP, newIntMap);
	oldIntEnable = adxl375_get_regs(ADXL375_INT_ENABLE);
	newIntEnable = oldIntEnable & ~(ADXL375_ACTIVITY);
	newIntEnable = newIntEnable | (ADXL375_ACTIVITY * actOnOff);
	adxl375_set_regs(ADXL375_INT_ENABLE, newIntEnable);
}

/***************************************************************************//**
 * @brief Enables/disables the inactivity detection.
 *
 * @param inactOnOff - Enables/disables the inactivity detection.
 *					   Example: 0x0 - disables the inactivity detection.
 *							    0x1 - enables the inactivity detection.
 * @param inactAxes - Axes which participate in detecting inactivity.
 *					  Example: 0x0 - disables axes participation.
 *						  	   ADXL375_INACT_X_EN - enables x-axis.
 *							   ADXL375_INACT_Y_EN - enables y-axis.
 *							   ADXL375_INACT_Z_EN - enables z-axis.
 * @param inactAcDc - Selects dc-coupled or ac-coupled operation.
 *					  Example: 0x0 - dc-coupled operation.
 *							   ADXL375_INACT_ACDC - ac-coupled operation.
 * @param inactThresh - Threshold value for detecting inactivity. The scale 
                        factor is 62.5 mg/LSB.
 * @param inactTime - Inactivity time. The scale factor is 1 sec/LSB.
 * @patam inactInt - Interrupts pin.
 *				     Example: 0x0 - inactivity interrupts on INT1 pin.
 *							  ADXL375_INACTIVITY - inactivity interrupts on
 *												   INT2 pin.
 *
 * @return None.
*******************************************************************************/
void adxl375_set_inactivity_detection(unsigned char inactOnOff,
									unsigned char inactAxes,
									unsigned char inactAcDc,
									unsigned char inactThresh,
									unsigned char inactTime,
									unsigned char inactInt)
{
	unsigned char oldActInactCtl = 0;
	unsigned char newActInactCtl = 0;
	unsigned char oldIntMap      = 0;
	unsigned char newIntMap      = 0;
	unsigned char oldIntEnable   = 0;
	unsigned char newIntEnable   = 0;
    
	oldActInactCtl = adxl375_get_regs(ADXL375_INT_ENABLE);
	newActInactCtl = oldActInactCtl & ~(ADXL375_INACT_ACDC |
										ADXL375_INACT_X_EN |
										ADXL375_INACT_Y_EN |
										ADXL375_INACT_Z_EN);
	newActInactCtl = newActInactCtl | (inactAcDc | inactAxes);
	adxl375_set_regs(ADXL375_ACT_INACT_CTL, newActInactCtl);
	adxl375_set_regs(ADXL375_THRESH_INACT, inactThresh);
	adxl375_set_regs(ADXL375_TIME_INACT, inactTime);
	oldIntMap = adxl375_get_regs(ADXL375_INT_MAP);
	newIntMap = oldIntMap & ~(ADXL375_INACTIVITY);
	newIntMap = newIntMap | inactInt;
	adxl375_set_regs(ADXL375_INT_MAP, newIntMap);
	oldIntEnable = adxl375_get_regs(ADXL375_INT_ENABLE);
	newIntEnable = oldIntEnable & ~(ADXL375_INACTIVITY);
	newIntEnable = newIntEnable | (ADXL375_INACTIVITY * inactOnOff);
	adxl375_set_regs(ADXL375_INT_ENABLE, newIntEnable);
}

/***************************************************************************//**
 * @brief Enables/disables the free-fall detection.
 *
 * @param ffOnOff - Enables/disables the free-fall detection.
 *					Example: 0x0 - disables the free-fall detection.
 *							 0x1 - enables the free-fall detection.
 * @param ffThresh - Threshold value for free-fall detection. The scale factor 
                     is 62.5 mg/LSB.
 * @param ffTime - Time value for free-fall detection. The scale factor is 
                   5 ms/LSB.
 * @param ffInt - Interrupts pin.
 *				  Example: 0x0 - free-fall interrupts on INT1 pin.
 *						   ADXL375_FREE_FALL - free-fall interrupts on INT2 pin.
 *
 * @return None.
*******************************************************************************/
void adxl375_set_freefall_detection(unsigned char ffOnOff,
								  unsigned char ffThresh,
								  unsigned char ffTime,
								  unsigned char ffInt)
{
	unsigned char oldIntMap    = 0;
	unsigned char newIntMap    = 0;
	unsigned char oldIntEnable = 0;
	unsigned char newIntEnable = 0;
    
	adxl375_set_regs(ADXL375_THRESH_FF, ffThresh);
	adxl375_set_regs(ADXL375_TIME_FF, ffTime);
	oldIntMap = adxl375_get_regs(ADXL375_INT_MAP);
	newIntMap = oldIntMap & ~(ADXL375_FREE_FALL);
	newIntMap = newIntMap | ffInt;
	adxl375_set_regs(ADXL375_INT_MAP, newIntMap);
	oldIntEnable = adxl375_get_regs(ADXL375_INT_ENABLE);
	newIntEnable = oldIntEnable & ~ADXL375_FREE_FALL;
	newIntEnable = newIntEnable | (ADXL375_FREE_FALL * ffOnOff);
	adxl375_set_regs(ADXL375_INT_ENABLE, newIntEnable);	
}

/***************************************************************************//**
 * @brief Sets an offset value for each axis (Offset Calibration).
 *
 * @param xOffset - X-axis's offset.
 * @param yOffset - Y-axis's offset.
 * @param zOffset - Z-axis's offset.
 *
 * @return None.
*******************************************************************************/
void adxl375_set_offset(unsigned char xOffset,
					   unsigned char yOffset,
					   unsigned char zOffset)
{
	adxl375_set_regs(ADXL375_OFSX, xOffset);
	adxl375_set_regs(ADXL375_OFSY, yOffset);
	adxl375_set_regs(ADXL375_OFSZ, yOffset);
}


int8_t adxl375_i2c_init(struct adxl375_dev *adxl375, uint8_t addr) {
    return 0;
}