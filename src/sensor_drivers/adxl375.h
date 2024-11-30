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

#ifndef __ADXL375_H__
#define __ADXL375_H__

#include <stdint.h>
#include <stddef.h>
#include <exception>
#include <string>
#include "adxl375_interface.h"

/*! CPP guard */
#ifdef __cplusplus
extern "C" {
#endif

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

/**\name API success code */
#define ADXL375_OK                                 INT8_C(0)

/**\name API error codes */
#define ADXL375_E_NULL_PTR                         INT8_C(-1)
#define ADXL375_E_COMM_FAIL                        INT8_C(-2)
#define ADXL375_E_INVALID_ODR_OSR_SETTINGS         INT8_C(-3)
#define ADXL375_E_CMD_EXEC_FAILED                  INT8_C(-4)
#define ADXL375_E_CONFIGURATION_ERR                INT8_C(-5)
#define ADXL375_E_INVALID_LEN                      INT8_C(-6)
#define ADXL375_E_DEV_NOT_FOUND                    INT8_C(-7)

/* ADXL375 mg/LSB scale for output data. Std of 0.00167 */
#define ADXL375_OUTPUT_SCALE					   0.049

#define GRAVITY_CONSTANT                            9.807

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
struct adxl375_dev {
    /*! Chip Id */
    uint8_t chip_id;

    /*!
     * The interface pointer is used to enable the user
     * to link their interface descriptors for reference during the
     * implementation of the read and write interfaces to the
     * hardware.
     */
    void *intf_ptr;
    
	/*! To store interface pointer error */
    int8_t intf_rslt;

    /*! Read function pointer */
    adxl375_read_fptr_t read;

    /*! Write function pointer */
    adxl375_write_fptr_t write;

    /*! Delay function pointer */
    adxl375_delay_us_fptr_t delay_us;
};

/*!
 * @brief adxl375 data structure
 */


/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*!
 *  @brief This API reads the data from the given register address of the sensor.
 *
 *  @param[in] reg_addr  : Register address from where the data to be read
 *  @param[out] reg_data : Pointer to data buffer to store the read data.
 *  @param[in] len       : No. of bytes of data to be read.
 *  @param[in] dev       : Structure instance of adxl375_dev.
 *
 *  @return Result of API execution status
 *  @retval 0  -> Success
 *  @retval >0 -> Warning
 *  @retval <0 -> Error
 */
int8_t adxl375_get_regs(uint8_t reg_addr, uint8_t *reg_data, uint32_t len,
						struct adxl375_dev *dev);

/*!
 *  @details This API writes the given data to the register address
 *  of the sensor.
 *
 *  @param[in] reg_addr  : Register address to where the data to be written.
 *  @param[in] reg_data  : Pointer to data buffer which is to be written
 *                         in the sensor.
 *  @param[in] len       : No. of bytes of data to write.
 *  @param[in] dev       : Structure instance of adxl375_dev.
 *
 *  @return Result of API execution status
 *  @retval 0  -> Success
 *  @retval >0 -> Warning
 *  @retval <0 -> Error
 */
int8_t adxl375_set_regs(uint8_t *reg_addr, const uint8_t *reg_data,
						int32_t len, struct adxl375_dev *dev);

/*!
 *  @details Initializes the communication peripheral, checks if the ADXL375
 *		  part is present and sets configuration values.
 *  @param[in] dev       : Structure instance of adxl375_dev.
 *  @param[in] addr      : Selection between ADXL375_ADDR_I2C_PRIM and
 *                         ADXL375_ADDR_I2C_SEC
 *
 *  @return Result of API execution status
 *  @retval 0  -> Success
 *  @retval >0 -> Warning
 *  @retval <0 -> Error
 */
int8_t adxl375_init(struct adxl375_dev *dev, uint8_t addr);

/*!
 * @details Places the device into standby/measure mode.
 *
 *  @param[in] pwr_mode  : Selection between standy(0x0)/measure(0x1).
 *  @param[in] dev      : Structure instance of adxl375_dev
 *
 *  @return Result of API execution status
 *  @retval 0  -> Success
 *  @retval >0 -> Warning
 *  @retval <0 -> Error
 */
int8_t adxl375_set_power_mode(uint8_t pwr_mode, struct adxl375_dev *dev);

/*!
 * @details Gets the interrupt status from register (INT_SOURCE)
 *
 *  @param[in] dev      : Structure instance of adxl375_dev
 *  @param[out] status  : Where the interrupt status is stored
 *
 *  @return Result of API execution status
 *  @retval 0  -> Success
 *  @retval >0 -> Warning
 *  @retval <0 -> Error
 */
int8_t get_int_status(struct adxl375_dev *dev, uint8_t *status);

/*!
 * @details Reads the output data of each axis. Gives the value in g unit.
 *
 *  @param[in] dev      : Structure instance of adxl375_dev
 *  @param[out] data.x  : Where x value is stored
 *  @param[out] data.y  : Where y value is stored
 *  @param[out] data.z  : Where z value is stored
 *
 *  @return Result of API execution status
 *  @retval 0  -> Success
 *  @retval >0 -> Warning
 *  @retval <0 -> Error
 */
int8_t adxl375_get_xyz(struct adxl375_dev *dev, struct adxl375_data *data);

/*!
 *  @details Enables/disables the tap detection.
 *
 *  @param[in] dev      : Structure instance of adxl375_dev
 *  @param[in] tapType  : Tap type (none, single, double).
 *					Example: 0x0 - disables tap detection.	
 *							 ADXL375_SINGLE_TAP - enables single tap detection.
 *							 ADXL375_DOUBLE_TAP - enables double tap detection.
 *  @param[in] tapAxes  : Axes which participate in tap detection.
 *					Example: 0x0 - disables axes participation.
 *							 ADXL375_TAP_X_EN - enables x-axis participation.
 *							 ADXL375_TAP_Y_EN - enables y-axis participation.
 *							 ADXL375_TAP_Z_EN - enables z-axis participation.
 *  @param[in] tapDur 	: Tap duration. The scale factor is 625us is/LSB.
 *  @param[in] tapLatent : Tap latency. The scale factor is 1.25 ms/LSB.
 *  @param[in] tapWindow : Tap window. The scale factor is 1.25 ms/LSB.
 *  @param[in] tapThresh : Tap threshold. The scale factor is 62.5 mg/LSB.
 *  @param[in] tapInt 	: Interrupts pin.
 *				   Example: 0x0 - interrupts on INT1 pin.
 *							ADXL375_SINGLE_TAP - single tap interrupts on
 *												 INT2 pin.
 *							ADXL375_DOUBLE_TAP - double tap interrupts on
 *												 INT2 pin.
 *
 * @return Result of API execution status
 * @retval 0  -> Success
 * @retval >0 -> Warning
 * @retval <0 -> Error
 */	
int8_t adxl375_set_tap_detection(struct adxl375_dev *dev,
							 uint8_t tapType,
							 uint8_t tapAxes,
							 uint8_t tapDur,
							 uint8_t tapLatent,
							 uint8_t tapWindow,
							 uint8_t tapThresh,
							 uint8_t tapInt) ;

/*!
 * @brief Enables/disables the activity detection.
 *
 * @param[in] adxl375	: Structure instance of adxl375_dev
 * @param[in] actOnOff 	: Enables/disables the activity detection.
 *					 	Example: 0x0 - disables the activity detection.
 *							  0x1 - enables the activity detection.
 * @param[in] actAxes	: Axes which participate in detecting activity.
 *					    Example: 0x0 - disables axes participation.
 *							 ADXL375_ACT_X_EN - enables x-axis participation.
 *							 ADXL375_ACT_Y_EN - enables y-axis participation.
 *							 ADXL375_ACT_Z_EN - enables z-axis participation.
 * @param[in] actAcDc 	: Selects dc-coupled or ac-coupled operation.
 *						Example: 0x0 - dc-coupled operation.
 *							 ADXL375_ACT_ACDC - ac-coupled operation.
 * @param[in] actThresh : Threshold value for detecting activity. The scale
 * 						  factor is 62.5 mg/LSB.
 * @param[in] actInt 	: Interrupts pin.
 *				  		Example: 0x0 - activity interrupts on INT1 pin.
 *							ADXL375_ACTIVITY - activity interrupts on INT2 pin.
 *
 *  @return Result of API execution status
 *  @retval 0  -> Success
 *  @retval >0 -> Warning
 *  @retval <0 -> Error
 */					 
int8_t adxl375_set_activity_detection(struct adxl375_dev *dev,
								  uint8_t actOnOff,
								  uint8_t actAxes,
								  uint8_t actAcDc,
								  uint8_t actThresh,
								  uint8_t actInt);

/*!
 * @brief Enables/disables the inactivity detection.
 *
 *  @param[in] adxl375      : Structure instance of adxl375_dev
 *  @param[in] inactOnOff   : Enables/disables the inactivity detection.
 *					   		Example: 0x0 - disables the inactivity detection.
 *							    0x1 - enables the inactivity detection.
 *  @param[in] inactAxes 	: Axes which participate in detecting inactivity.
 *					        Example: 0x0 - disables axes participation.
 *						  	   ADXL375_INACT_X_EN - enables x-axis.
 *							   ADXL375_INACT_Y_EN - enables y-axis.
 *							   ADXL375_INACT_Z_EN - enables z-axis.
 *  @param[in] inactAcDc 	: Selects dc-coupled or ac-coupled operation.
 *					        Example: 0x0 - dc-coupled operation.
 *							   ADXL375_INACT_ACDC - ac-coupled operation.
 *  @param[in] inactThresh 	: Threshold value for detecting inactivity.
 * 							  The scale factor is 62.5 mg/LSB.
 *  @param[in] inactTime 	: Inactivity time. The scale factor is 1 sec/LSB.
 *  @param[in] inactInt 	: Interrupts pin.
 *				            Example: 0x0 - inactivity interrupts on INT1 pin.
 *							  ADXL375_INACTIVITY - inactivity interrupts on
 *												   INT2 pin.
 *
 *  @return Result of API execution status
 *  @retval 0  -> Success
 *  @retval >0 -> Warning
 *  @retval <0 -> Error
 */
int8_t adxl375_set_inactivity_detection(struct adxl375_dev *dev,
									uint8_t inactOnOff,
									uint8_t inactAxes,
									uint8_t inactAcDc,
									uint8_t inactThresh,
									uint8_t inactTime,
									uint8_t inactInt);

/*!
 *  @brief Enables/disables the free-fall detection.
 *
 *  @param[in] adxl375      : Structure instance of adxl375_dev
 *  @param[in] ffOnOff 		: Enables/disables the free-fall detection.
 *					        Example: 0x0 - disables the free-fall detection.
 *							 0x1 - enables the free-fall detection.
 *  @param[in] ffThresh 	: Threshold value for free-fall detection. The scale factor 
    					      is 62.5 mg/LSB.
 *  @param[in] ffTime 		: Time value for free-fall detection. The scale factor is 
                   			  5 ms/LSB.
 *  @param[in] ffInt 		: Interrupts pin.
 *				           Example: 0x0 - free-fall interrupts on INT1 pin.
 *						   ADXL375_FREE_FALL - free-fall interrupts on INT2 pin.
 *
 *  @return Result of API execution status
 *  @retval 0  -> Success
 *  @retval >0 -> Warning
 *  @retval <0 -> Error
 */									
int8_t adxl375_set_freefall_detection(struct adxl375_dev *dev,
								  uint8_t ffOnOff,
								  uint8_t ffThresh,
								  uint8_t ffTime,
								  uint8_t ffInt);

/*!
 *  @brief Sets an offset value for each axis (Offset Calibration).
 *
 *  @param[in] adxl375      : Structure instance of adxl375_dev
 *  @param[in] xOffset      : X-axis' offset.
 *  @param[in] yOffset 		: Y-axis' offset.
 *  @param[in] zOffset 		: Z-axis' offset.
 *
 *  @return Result of API execution status
 *  @retval 0  -> Success
 *  @retval >0 -> Warning
 *  @retval <0 -> Error
 */	
int8_t adxl375_set_offset(struct adxl375_dev *dev,
					   uint8_t xOffset,
					   uint8_t yOffset,
					   uint8_t zOffset) ;

/*!
 *  @brief Function to initialise the I2C interface, with functions from
 *         i2c_wrappers.h
 *
 *  @param[in] adxl375      : Structure instance of adxl375_dev
 *  @param[in] addr      : Selection between ADXL375_ADDR_I2C_PRIM and
 *                         ADXL375_ADDR_I2C_SEC
 *
 *  @return Status of execution
 *  @retval 0 -> Success
 *  @retval < 0 -> Failure Info
 */
int8_t adxl375_i2c_init(struct adxl375_dev *adxl375, uint8_t addr);

/*!
 *  @brief Function to deinitialise the I2C interface, with functions from
 *         i2c_wrappers.h
 *
 *  @param[in] addr      : Selection between ADXL375_ADDR_I2C_PRIM and
 *                         ADXL375_ADDR_I2C_SEC
 *
 *  @return Status of execution
 *  @retval 0 -> Success
 *  @retval < 0 -> Failure Info
 */
int8_t adxl375_i2c_deinit(uint8_t addr);

#ifdef __cplusplus
}
#endif /* End of CPP guard */

class Adxl375 : public Adxl375_Interface {
private:
	uint8_t addr;
	adxl375_dev dev;
    adxl375_data offset={0,0,0};
    float scale=ADXL375_OUTPUT_SCALE;
	adxl375_data data;
	uint8_t status = 0;
public:
    Adxl375(uint8_t addr);
	~Adxl375() noexcept override;
    uint8_t get_status()override;
    adxl375_data get_data() override;
    void set_offset(adxl375_data offset_in) override;
    void set_scale(float scale_in) override;
    void calibrate() override;
} ;

class Adxl375Exception : public std::exception {
private:
    std::string message;

public:
    Adxl375Exception(int8_t errorCode) {
        switch (errorCode) {
            case ADXL375_E_NULL_PTR:
                message = "Null pointer error.";
                break;
            case ADXL375_E_COMM_FAIL:
                message = "Communication failure.";
                break;
            case ADXL375_E_INVALID_ODR_OSR_SETTINGS:
                message = "Invalid ODR/OSR settings.";
                break;
            case ADXL375_E_CMD_EXEC_FAILED:
                message = "Command execution failed.";
                break;
            case ADXL375_E_CONFIGURATION_ERR:
                message = "Configuration error.";
                break;
            case ADXL375_E_INVALID_LEN:
                message = "Invalid length error.";
                break;
            case ADXL375_E_DEV_NOT_FOUND:
                message = "Device not found.";
                break;
            default:
                message = "Unknown error.";
                break;
        }
    }

    virtual const char* what() const throw () {
        return message.c_str();
    }
};

#endif	/* __ADXL375_H__ */
