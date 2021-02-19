/*
 * Copyright (c) 2018, Sensirion AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of Sensirion AG nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \file
 *
 * \brief Sensirion SHT driver interface
 *
 * This module provides access to the SHT functionality over a generic I2C
 * interface. It supports measurements without clock stretching only.
 */

#ifndef SHT3X_H
#define SHT3X_H

#include "sensirion_arch_config.h"
#include "sensirion_i2c.h"
#include "sht_git_version.h"

#ifdef __cplusplus
extern "C" {
#endif

#define STATUS_OK 0
#define STATUS_ERR_BAD_DATA (-1)
#define STATUS_CRC_FAIL (-2)
#define STATUS_UNKNOWN_DEVICE (-3)
#define STATUS_ERR_INVALID_PARAMS (-4)
#define SHT3X_MEASUREMENT_DURATION_USEC 15000

/* status word macros */
#define SHT3X_IS_ALRT_PENDING(status) (bool)(((status)&0x8000U) != 0U)
#define SHT3X_IS_ALRT_RH_TRACK(status) (bool)(((status)&0x0800) != 0U)
#define SHT3X_IS_ALRT_T_TRACK(status) (bool)(((status)&0x0400U) != 0U)
#define SHT3X_IS_SYSTEM_RST_DETECT(status) (bool)(((status)&0x0010U) != 0U)
#define SHT3X_IS_LAST_CRC_FAIL(status) (bool)(((status)&0x0001U) != 0U)

/**
 * @brief SHT3x I2C 7-bit address option
 */
typedef enum _sht3x_i2c_addr {
    SHT3X_I2C_ADDR_DFLT = 0x44,
    SHT3X_I2C_ADDR_ALT = 0x45
} sht3x_i2c_addr_t;

/**
 * @brief SHT3x I2C 7-bit address option
 */
typedef enum _sht3x_measurement_mode {
    SHT3X_MEAS_MODE_LPM, /*low power mode*/
    SHT3X_MEAS_MODE_MPM, /*medium power mode*/
    SHT3X_MEAS_MODE_HPM  /*high power mode*/
} sht3x_measurement_mode_t;

/**
 * @brief SHT3x Alert Thresholds
 */
typedef enum _sht3x_alert_thd {
    SHT3X_HIALRT_SET,
    SHT3X_HIALRT_CLR,
    SHT3X_LOALRT_CLR,
    SHT3X_LOALRT_SET,
} sht3x_alert_thd_t;

/**
 * Detects if a sensor is connected by reading out the ID register.
 * If the sensor does not answer or if the answer is not the expected value,
 * the test fails.
 *
 * @param[in] addr the sensor address
 *
 * @return 0 if a sensor was detected
 */
int16_t sht3x_probe(sht3x_i2c_addr_t addr);

/**
 * Read the sensot status word
 *
 * @param[in] addr the sensor address
 * @param[out] status  the address for the result of the status word
 *
 * @return 0 if a sensor was detected
 */
int16_t sht3x_get_status(sht3x_i2c_addr_t addr, uint16_t* status);

/**
 * Clear the status register alert flags
 *
 * @param[in] addr the sensor address
 *
 * @return 0 if a sensor was detected
 */
int16_t sht3x_clear_status(sht3x_i2c_addr_t addr);

/**
 * Starts a measurement and then reads out the results. This function blocks
 * while the measurement is in progress. The duration of the measurement depends
 * on the sensor in use, please consult the datasheet.
 * Temperature is returned in [degree Celsius], multiplied by 1000,
 * and relative humidity in [percent relative humidity], multiplied by 1000.
 *
 * @param[in]  addr the sensor address
 * @param[out] temperature   the address for the result of the temperature
 * measurement
 * @param[out] humidity      the address for the result of the relative humidity
 * measurement
 *
 * @return              0 if the command was successful, else an error code.
 */
int16_t sht3x_measure_blocking_read(sht3x_i2c_addr_t addr, int32_t* temperature,
                                    int32_t* humidity);

/**
 * Starts a measurement in high precision mode. Use sht3x_read() to read out the
 * values, once the measurement is done. The duration of the measurement depends
 * on the sensor in use, please consult the datasheet.
 *
 * @param[in]  addr the sensor address
 *
 * @return     0 if the command was successful, else an error code.
 */
int16_t sht3x_measure(sht3x_i2c_addr_t addr);

/**
 * Reads out the results of a measurement that was previously started by
 * sht3x_measure(). If the measurement is still in progress, this function
 * returns an error.
 * Temperature is returned in [degree Celsius], multiplied by 1000,
 * and relative humidity in [percent relative humidity], multiplied by 1000.
 *
 * @param[in]  addr the sensor address
 * @param[out] temperature   the address for the result of the temperature
 * measurement
 * @param[out] humidity      the address for the result of the relative humidity
 * measurement
 *
 * @return              0 if the command was successful, else an error code.
 */
int16_t sht3x_read(sht3x_i2c_addr_t addr, int32_t* temperature,
                   int32_t* humidity);

/**
 * Enable or disable the SHT's low power mode
 *
 * @param[in] enable_low_power_mode 1 to enable low power mode, 0 to disable
 */
void sht3x_enable_low_power_mode(uint8_t enable_low_power_mode);

/**
 * Enable or disable the SHT's low power mode
 *
 * @param[in] mode power mode selector
 */
void sht3x_set_power_mode(sht3x_measurement_mode_t mode);

/**
 * Read out the serial number
 *
 * @param[in]  addr the sensor address
 * @param[out] serial    the address for the result of the serial number
 *
 * @return          0 if the command was successful, else an error code.
 */
int16_t sht3x_read_serial(sht3x_i2c_addr_t addr, uint32_t* serial);

/**
 * Return the driver version
 *
 * @param[in]  addr the sensor address
 *
 * @return Driver version string
 */
const char* sht3x_get_driver_version(sht3x_i2c_addr_t addr);

/**
 * Returns the configured SHT3x address.
 *
 * @param[in]  addr the sensor address
 *
 * @return SHT3x_ADDRESS
 */
uint8_t sht3x_get_configured_address(sht3x_i2c_addr_t addr);

/**
 * Set target temperature and humidity alrt threshold
 *
 * @param[in] addr the sensor address
 * @param[in] thd target alert threshold to be edited
 * @param[in] humidity humidity threshold in 10*%RH
 * @param[in] temperature temperature threshold in 10*%°C
 *
 * @return          0 if the command was successful, else an error code.
 */
int16_t sht3x_set_alert_thd(sht3x_i2c_addr_t addr, sht3x_alert_thd_t thd,
                            uint16_t humidity, int16_t temperature);

/**
 * Get target temperature and humidity alrt threshold
 *
 * @param[in]  addr the sensor address
 *  @param[in] thd target alert threshold to be edited
 * @param[out] humidity address for the result humidity thd in 10*%RH
 * @param[out] temperature address for the result temperature thd in 10*°C
 *
 * @return          0 if the command was successful, else an error code.
 */
int16_t sht3x_get_alert_thd(sht3x_i2c_addr_t addr, sht3x_alert_thd_t thd,
                            uint16_t* humidity, int16_t* temperature);

#ifdef __cplusplus
}
#endif

#endif /* SHT3X_H */
