#ifndef _I2C_COMMON_H
#define _I2C_COMMON_H

/* Header includes */

/*! CPP guard */
#ifdef __cplusplus
extern "C" {
#endif

/*!
 *  @brief Macros for error management
 */
#define COMM_FAIL -2;

/*!
 *  @brief Function to initialise GPIO with pigpio.
 *
 *  @return Status of execution
 *  @retval 0 -> Success
 *  @retval < 0 -> Failure
 */
int8_t gpio_initialise();

/*!
 *  @brief Function to deinitialise GPIO with pigpio.
 */
void gpio_terminate();

/*!
 *  @brief Function to return the unique pointer to an address.
 *
 *  @param[in] addr         : Device I2C address
 *  @param[in] intfptr      : pointer to return
 *
 *  @return Status of execution
 *  @retval 0 -> Success
 *  @retval < 0 -> Failure
 */
int8_t get_intf_ptr(uint8_t addr, void **intf_ptr);

/*!
 *  @brief Function to open an I2C channel for a device at a specific address.
 *
 *  @param[in] addr         : Device I2C address
 *
 *  @return Status of execution
 *  @retval 0 -> Success
 *  @retval < 0 -> Failure Info
 */
int8_t i2c_open(uint8_t addr);

/*!
 *  @brief Function to close an I2C channel for a device at a specific address.
 *
 *  @param[in] addr         : Device I2C address
 *
 *  @return Status of execution
 *  @retval 0 -> Success
 *  @retval < 0 -> Failure Info
 */
int8_t i2c_close(uint8_t addr);

/*!
 *  @brief Function for reading the sensor's registers through I2C bus.
 *
 *  @param[in] reg_addr     : Register address.
 *  @param[out] reg_data    : Pointer to the data buffer to store the read data.
 *  @param[in] len          : No of bytes to read.
 *  @param[in] intf_ptr     : Interface pointer
 *
 *  @return Status of execution
 *  @retval = INTF_RET_SUCCESS  -> Success
 *  @retval != NTF_RET_SUCCESS  -> Failure Info
 *
 */
int8_t i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr);

/*!
 *  @brief Function for writing the sensor's registers through I2C bus.
 *
 *  @param[in] reg_addr     : Register address.
 *  @param[in] reg_data     : Pointer to the data buffer whose value is to be written.
 *  @param[in] len          : No of bytes to write.
 *  @param[in] intf_ptr     : Interface pointer
 *
 *  @return Status of execution
 *  @retval = INTF_RET_SUCCESS   -> Success
 *  @retval != INTF_RET_SUCCESS  -> Failure Info
 *
 */
int8_t i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr);

/*!
 * @brief This function provides the delay for required time (Microsecond) as per the input
 * provided in some of the APIs.
 *
 *  @param[in] period_us    : The required wait time in microsecond.
 *  @param[in] intf_ptr     : Interface pointer
 *
 *  @return void.
 *
 */
void i2c_delay_us(uint32_t period, void *intf_ptr);

#ifdef __cplusplus
}
#endif /* End of CPP guard */

#endif /* _BMP3_H */
