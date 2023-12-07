/*******************************************************************************
 *   @file   ADXL375.h
 *   @brief  Header file of ADXL375 Driver.
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
#include <stdint.h>

#ifndef __ADXL375_H__
#define __ADXL375_H__

/******************************************************************************/
/******************************** ADXL375 *************************************/
/******************************************************************************/

/* Options for communicating with the device. */
#define ADXL375_SPI_COMM       0
#define ADXL375_I2C_COMM       1

/* SPI slave device ID */
#define ADXL375_SLAVE_ID       1

/* I2C address of the device */
#define ADXL375_ADDR_I2C_PRIM	0x53
#define ADXL375_ADDR_I2C_SEC	0x1D

/* ADXL375 Register Map */
#define	ADXL375_DEVID			0x00 // R   Device ID.
#define ADXL375_THRESH_TAP		0x1D // R/W Tap threshold.
#define ADXL375_OFSX			0x1E // R/W X-axis offset.
#define ADXL375_OFSY			0x1F // R/W Y-axis offset.
#define ADXL375_OFSZ			0x20 // R/W Z-axis offset.
#define ADXL375_DUR				0x21 // R/W Tap duration.
#define ADXL375_LATENT			0x22 // R/W Tap latency.
#define ADXL375_WINDOW			0x23 // R/W Tap window.
#define ADXL375_THRESH_ACT		0x24 // R/W Activity threshold.
#define ADXL375_THRESH_INACT	0x25 // R/W Inactivity threshold.
#define ADXL375_TIME_INACT		0x26 // R/W Inactivity time.
#define ADXL375_ACT_INACT_CTL	0x27 // R/W Axis enable control for activity.
									 //     and inactivity detection.
#define ADXL375_THRESH_FF		0x28 // R/W Free-fall threshold.
#define ADXL375_TIME_FF			0x29 // R/W Free-fall time.
#define ADXL375_TAP_AXES		0x2A // R/W Axis control for tap/double tap.
#define ADXL375_ACT_TAP_STATUS	0x2B // R   Source of tap/double tap.
#define ADXL375_BW_RATE			0x2C // R/W Data rate and power mode control.
#define ADXL375_POWER_CTL		0x2D // R/W Power saving features control.
#define ADXL375_INT_ENABLE		0x2E // R/W Interrupt enable control.
#define ADXL375_INT_MAP			0x2F // R/W Interrupt mapping control.
#define ADXL375_INT_SOURCE		0x30 // R   Source of interrupts.
#define ADXL375_DATA_FORMAT		0x31 // R/W Data format control.
#define ADXL375_DATAX0			0x32 // R   X-Axis Data 0.
#define ADXL375_DATAX1			0x33 // R   X-Axis Data 1.
#define ADXL375_DATAY0			0x34 // R   Y-Axis Data 0.
#define ADXL375_DATAY1			0x35 // R   Y-Axis Data 1.
#define ADXL375_DATAZ0			0x36 // R   Z-Axis Data 0.
#define ADXL375_DATAZ1			0x37 // R   Z-Axis Data 1.
#define ADXL375_FIFO_CTL		0x38 // R/W FIFO control.
#define ADXL375_FIFO_STATUS		0x39 // R   FIFO status.

/* ADXL375_ACT_INACT_CTL Bits */
#define ADXL375_ACT_ACDC		(1 << 7)
#define ADXL375_ACT_X_EN		(1 << 6)
#define ADXL375_ACT_Y_EN		(1 << 5)
#define ADXL375_ACT_Z_EN		(1 << 4)
#define ADXL375_INACT_ACDC		(1 << 3)
#define ADXL375_INACT_X_EN		(1 << 2)
#define ADXL375_INACT_Y_EN		(1 << 1)
#define ADXL375_INACT_Z_EN		(1 << 0)

/* ADXL375_TAP_AXES Bits */
#define ADXL375_SUPPRESS		(1 << 3)
#define ADXL375_TAP_X_EN		(1 << 2)
#define ADXL375_TAP_Y_EN		(1 << 1)
#define ADXL375_TAP_Z_EN		(1 << 0)

/* ADXL375_ACT_TAP_STATUS Bits */
#define ADXL375_ACT_X_SRC		(1 << 6)
#define ADXL375_ACT_Y_SRC		(1 << 5)
#define ADXL375_ACT_Z_SRC		(1 << 4)
#define ADXL375_ASLEEP			(1 << 3)
#define ADXL375_TAP_X_SRC		(1 << 2)
#define ADXL375_TAP_Y_SRC		(1 << 1)
#define ADXL375_TAP_Z_SRC		(1 << 0)

/* ADXL375_BW_RATE Bits */
#define ADXL375_LOW_POWER		(1 << 4)
#define ADXL375_RATE(x)			((x) & 0xF)

/* ADXL375_POWER_CTL Bits */
#define ADXL375_PCTL_LINK       (1 << 5)
#define ADXL375_PCTL_AUTO_SLEEP (1 << 4)
#define ADXL375_PCTL_MEASURE    (1 << 3)
#define ADXL375_PCTL_SLEEP      (1 << 2)
#define ADXL375_PCTL_WAKEUP(x)  ((x) & 0x3)

/* ADXL375_INT_ENABLE / ADXL375_INT_MAP / ADXL375_INT_SOURCE Bits */
#define ADXL375_DATA_READY      (1 << 7)
#define ADXL375_SINGLE_TAP      (1 << 6)
#define ADXL375_DOUBLE_TAP      (1 << 5)
#define ADXL375_ACTIVITY        (1 << 4)
#define ADXL375_INACTIVITY      (1 << 3)
#define ADXL375_FREE_FALL       (1 << 2)
#define ADXL375_WATERMARK       (1 << 1)
#define ADXL375_OVERRUN         (1 << 0)

/* ADXL375_DATA_FORMAT Bits */
#define ADXL375_SELF_TEST       (1 << 7)
#define ADXL375_SPI             (1 << 6)
#define ADXL375_INT_INVERT      (1 << 5)
#define ADXL375_FULL_RES        (1 << 3)
#define ADXL375_JUSTIFY         (1 << 2)
#define ADXL375_RANGE(x)        ((x) & 0x3)
#define ADXL375_RANGE_PM_2G     0
#define ADXL375_RANGE_PM_4G     1
#define ADXL375_RANGE_PM_8G     2
#define ADXL375_RANGE_PM_16G    3

/* ADXL375_FIFO_CTL Bits */
#define ADXL375_FIFO_MODE(x)    (((x) & 0x3) << 6)
#define ADXL375_FIFO_BYPASS     0
#define ADXL375_FIFO_FIFO       1
#define ADXL375_FIFO_STREAM     2
#define ADXL375_FIFO_TRIGGER    3
#define ADXL375_TRIGGER         (1 << 5)
#define ADXL375_SAMPLES(x)      ((x) & 0x1F)

/* ADXL375_FIFO_STATUS Bits */
#define ADXL375_FIFO_TRIG       (1 << 7)
#define ADXL375_ENTRIES(x)      ((x) & 0x3F)

/* ADXL375 ID */
#define ADXL375_ID				0xE5

/********************************************************/

/*!
 * @brief Type definitions
 */

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
typedef int8_t (*adxl375_read_fptr_t)(uint8_t reg_addr, uint8_t *read_data, uint32_t len, void *intf_ptr);

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
typedef int8_t (*adxl375_write_fptr_t)(uint8_t reg_addr, const uint8_t *read_data, uint32_t len,
                                                void *intf_ptr);

/*!
 * @brief Delay function pointer which should be mapped to
 * delay function of the user
 *
 * @param[in] period              : Delay in microseconds.
 * @param[in, out] intf_ptr       : Void pointer that can enable the linking of descriptors
 *                                  for interface related call backs
 *
 */
typedef void (*adxl375_delay_us_fptr_t)(uint32_t period, void *intf_ptr);

/********************************************************/

/*!
 * @brief adxl375 device structure
 */
struct adxl375_dev
{
    /*! Chip Id */
    uint8_t chip_id;

    /*!
     * The interface pointer is used to enable the user
     * to link their interface descriptors for reference during the
     * implementation of the read and write interfaces to the
     * hardware.
     */
    void *intf_ptr;
    
    /*! Read function pointer */
    adxl375_read_fptr_t read;

    /*! Write function pointer */
    adxl375_write_fptr_t write;

    /*! Delay function pointer */
    adxl375_delay_us_fptr_t delay_us;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Reads the value of a register. */
unsigned char adxl375_get_reg_vals(unsigned char registerAddress);

/*! Writes data into a register. */
void adxl375_set_reg_vals(unsigned char registerAddress,
							  unsigned char registerValue);

/*! Initializes the I2C peripheral and checks if the ADXL375 part is present. */
unsigned char adxl375_init(struct adxl375_dev *adxl375, uint8_t addr);

/*! Places the device into standby/measure mode. */
void adxl375_set_power_mode(unsigned char pwrMode);

/*! Reads the output data of each axis. */
void adxl375_get_xyz(int16_t* x,
					int16_t* y,
					int16_t* z);

/*! Enables/disables the tap detection. */					
void adxl375_set_tap_detection(unsigned char tapType,
							 unsigned char tapAxes,
							 unsigned char tapDur,
							 unsigned char tapLatent,
							 unsigned char tapWindow,
							 unsigned char tapThresh,
							 unsigned char tapInt);

/*! Enables/disables the activity detection. */							 
void adxl375_set_activity_detection(unsigned char actOnOff,
								  unsigned char actAxes,
								  unsigned char actAcDc,
								  unsigned char actThresh,
								  unsigned char actInt);

/*! Enables/disables the inactivity detection. */								  
void adxl375_set_inactivity_detection(unsigned char inactOnOff,
									unsigned char inactAxes,
									unsigned char inactAcDc,
									unsigned char inactThresh,
									unsigned char inactTime,
									unsigned char inactInt);

/*! Enables/disables the free-fall detection. */									
void adxl375_set_freefall_detection(unsigned char ffOnOff,
								  unsigned char ffThresh,
								  unsigned char ffTime,
								  unsigned char ffInt);

/*! Sets an offset value for each axis (Offset Calibration). */
void adxl375_set_offset(unsigned char xOffset,
					   unsigned char yOffset,
					   unsigned char zOffset);

/*!
 *  @brief Function to initialise the I2C interface, with functions from i2c_common.h
 *
 *  @param[in] adxl375      : Structure instance of adxl375_dev
 *  @param[in] addr     : Selection between ADXL375_ADDR_I2C_PRIM and ADXL375_ADDR_I2C_SEC
 *
 *  @return Status of execution
 *  @retval 0 -> Success
 *  @retval < 0 -> Failure Info
 */
int8_t adxl375_i2c_init(struct adxl375_dev *adxl375, uint8_t addr);

#endif	/* __ADXL375_H__ */
