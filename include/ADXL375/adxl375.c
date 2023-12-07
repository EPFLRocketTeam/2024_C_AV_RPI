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

/***************************************************************************//**
 * @brief Reads the value of a register.
 *
 * @param registerAddress - Address of the register.
 *
 * @return registerValue - Value of the register.
*******************************************************************************/
unsigned char adxl375_get_reg_vals(unsigned char registerAddress)
{
	unsigned char readData[2]   = {0, 0};
    unsigned char registerValue = 0,i,t1;
    
	// for(i=0;i<100;i++) //For loop to perform multiple read attempts - if there is an issue with I2C communication
	//{
		I2C_Write(ADXL375_ADDRESS,  // Address of the slave device.
					&registerAddress, // Transmission data.
					1,                // Number of bytes to write.
					0);               // Stop condition control.
		
		t1 = I2C_Read(ADXL375_ADDRESS,   // Address of the slave device.
					&registerValue,    // Received data.
					1,                 // Number of bytes to read.
					1);                // Stop condition control.
		//if(0==t1)
		//	break;
	//}
	//printf("t1 status: %u, Iteration: %u \r\n",t1,i);

	return registerValue;
}

/***************************************************************************//**
 * @brief Writes data into a register.
 *
 * @param registerAddress - Address of the register.
 * @param registerValue - Data value to write.
 *
 * @return None.
*******************************************************************************/
void adxl375_set_reg_vals(unsigned char registerAddress,
							  unsigned char registerValue)
{
	unsigned char writeData[2] = {0, 0};

	writeData[0] = registerAddress;
	writeData[1] = registerValue;
	I2C_Write(ADXL375_ADDRESS, writeData, 2, 1);
    
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
unsigned char adxl375_init(struct adxl375_dev *adxl375, uint8_t addr)
{
	unsigned char status = 1,temp=0;

	status = adxl375_i2c_init(adxl375, addr);
	
	temp = adxl375_get_reg_vals(ADXL375_DEVID);
	
	if( temp!= ADXL375_ID)
	{
		status = 0;
	}
	//printf("ADXL375_DevID: 0x%x, Status: %u \r\n",temp,status);
	
	//Configure the ADXL375
	adxl375_set_reg_vals(ADXL375_BW_RATE, (0x00|ADXL375_RATE(0x0A))); // Normal Power Mode, 100Hz operation
 	adxl375_set_reg_vals(ADXL375_DATA_FORMAT, (0x00|ADXL375_RANGE(ADXL375_RANGE_PM_2G)|ADXL375_FULL_RES)); //SelfTest:Off, SPI: 4-wire, Interrupts: Active High, Full_Res: On, Justify: Right justified with sign extension
	adxl375_set_reg_vals(ADXL375_FIFO_CTL, (0x00|ADXL375_FIFO_MODE(0x2)|ADXL375_SAMPLES(0x1E))); //FIFO mode - Stream, Watermark interrupt set @ 30 (0x1E)samples - which at ~100Hz(or even greater) provides sufficient time for FIFO to be emptied
	adxl375_set_reg_vals(ADXL375_INT_MAP,(0x00|~ADXL375_WATERMARK));//Watermark interrupt to Int 1 - all others to Int 2
	adxl375_set_reg_vals(ADXL375_INT_ENABLE,(0x00|ADXL375_WATERMARK));//Enables Watermark interrupt on Int 1 - GPIO 17
	adxl375_set_power_mode(0x1);//Start the ADXL375
	
	return status;
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
void adxl375_set_power_mode(unsigned char pwrMode)
{
	unsigned char oldPowerCtl = 0;
	unsigned char newPowerCtl = 0;
    
	oldPowerCtl = adxl375_get_reg_vals(ADXL375_POWER_CTL);
	newPowerCtl = oldPowerCtl & ~ADXL375_PCTL_MEASURE;
	newPowerCtl = newPowerCtl | (pwrMode * ADXL375_PCTL_MEASURE);
	adxl375_set_reg_vals(ADXL375_POWER_CTL, newPowerCtl);
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
	*x = adxl375_get_reg_vals(ADXL375_DATAX1) << 8;
	*x += adxl375_get_reg_vals(ADXL375_DATAX0);
	*y = adxl375_get_reg_vals(ADXL375_DATAY1) << 8;
	*y += adxl375_get_reg_vals(ADXL375_DATAY0);
	*z = adxl375_get_reg_vals(ADXL375_DATAZ1) << 8;
	*z += adxl375_get_reg_vals(ADXL375_DATAZ0);
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
    
	oldTapAxes = adxl375_get_reg_vals(ADXL375_TAP_AXES);
	newTapAxes = oldTapAxes & ~(ADXL375_TAP_X_EN |
								ADXL375_TAP_Y_EN |
								ADXL375_TAP_Z_EN);
	newTapAxes = newTapAxes | tapAxes;
	adxl375_set_reg_vals(ADXL375_TAP_AXES, newTapAxes);
	adxl375_set_reg_vals(ADXL375_DUR, tapDur);
	adxl375_set_reg_vals(ADXL375_LATENT, tapLatent);
	adxl375_set_reg_vals(ADXL375_WINDOW, tapWindow);
	adxl375_set_reg_vals(ADXL375_THRESH_TAP, tapThresh);
	oldIntMap = adxl375_get_reg_vals(ADXL375_INT_MAP);
	newIntMap = oldIntMap & ~(ADXL375_SINGLE_TAP | ADXL375_DOUBLE_TAP);
	newIntMap = newIntMap | tapInt;
	adxl375_set_reg_vals(ADXL375_INT_MAP, newIntMap);
	oldIntEnable = adxl375_get_reg_vals(ADXL375_INT_ENABLE);
	newIntEnable = oldIntEnable & ~(ADXL375_SINGLE_TAP | ADXL375_DOUBLE_TAP);
	newIntEnable = newIntEnable | tapType;
	adxl375_set_reg_vals(ADXL375_INT_ENABLE, newIntEnable);
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
    
	oldActInactCtl = adxl375_get_reg_vals(ADXL375_INT_ENABLE);
	newActInactCtl = oldActInactCtl & ~(ADXL375_ACT_ACDC |
										ADXL375_ACT_X_EN |
										ADXL375_ACT_Y_EN |
										ADXL375_ACT_Z_EN);
	newActInactCtl = newActInactCtl | (actAcDc | actAxes);
	adxl375_set_reg_vals(ADXL375_ACT_INACT_CTL, newActInactCtl);
	adxl375_set_reg_vals(ADXL375_THRESH_ACT, actThresh);
	oldIntMap = adxl375_get_reg_vals(ADXL375_INT_MAP);
	newIntMap = oldIntMap & ~(ADXL375_ACTIVITY);
	newIntMap = newIntMap | actInt;
	adxl375_set_reg_vals(ADXL375_INT_MAP, newIntMap);
	oldIntEnable = adxl375_get_reg_vals(ADXL375_INT_ENABLE);
	newIntEnable = oldIntEnable & ~(ADXL375_ACTIVITY);
	newIntEnable = newIntEnable | (ADXL375_ACTIVITY * actOnOff);
	adxl375_set_reg_vals(ADXL375_INT_ENABLE, newIntEnable);
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
    
	oldActInactCtl = adxl375_get_reg_vals(ADXL375_INT_ENABLE);
	newActInactCtl = oldActInactCtl & ~(ADXL375_INACT_ACDC |
										ADXL375_INACT_X_EN |
										ADXL375_INACT_Y_EN |
										ADXL375_INACT_Z_EN);
	newActInactCtl = newActInactCtl | (inactAcDc | inactAxes);
	adxl375_set_reg_vals(ADXL375_ACT_INACT_CTL, newActInactCtl);
	adxl375_set_reg_vals(ADXL375_THRESH_INACT, inactThresh);
	adxl375_set_reg_vals(ADXL375_TIME_INACT, inactTime);
	oldIntMap = adxl375_get_reg_vals(ADXL375_INT_MAP);
	newIntMap = oldIntMap & ~(ADXL375_INACTIVITY);
	newIntMap = newIntMap | inactInt;
	adxl375_set_reg_vals(ADXL375_INT_MAP, newIntMap);
	oldIntEnable = adxl375_get_reg_vals(ADXL375_INT_ENABLE);
	newIntEnable = oldIntEnable & ~(ADXL375_INACTIVITY);
	newIntEnable = newIntEnable | (ADXL375_INACTIVITY * inactOnOff);
	adxl375_set_reg_vals(ADXL375_INT_ENABLE, newIntEnable);
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
    
	adxl375_set_reg_vals(ADXL375_THRESH_FF, ffThresh);
	adxl375_set_reg_vals(ADXL375_TIME_FF, ffTime);
	oldIntMap = adxl375_get_reg_vals(ADXL375_INT_MAP);
	newIntMap = oldIntMap & ~(ADXL375_FREE_FALL);
	newIntMap = newIntMap | ffInt;
	adxl375_set_reg_vals(ADXL375_INT_MAP, newIntMap);
	oldIntEnable = adxl375_get_reg_vals(ADXL375_INT_ENABLE);
	newIntEnable = oldIntEnable & ~ADXL375_FREE_FALL;
	newIntEnable = newIntEnable | (ADXL375_FREE_FALL * ffOnOff);
	adxl375_set_reg_vals(ADXL375_INT_ENABLE, newIntEnable);	
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
	adxl375_set_reg_vals(ADXL375_OFSX, xOffset);
	adxl375_set_reg_vals(ADXL375_OFSY, yOffset);
	adxl375_set_reg_vals(ADXL375_OFSZ, yOffset);
}


int8_t adxl375_i2c_init(struct adxl375_dev *adxl375, uint8_t addr) {
    return 0;
}