/*
 * bmp280.h
 *
 *  Created on: Feb 13, 2025
 *      Author: LabMeas
 */

#ifndef BMP280_H_
#define BMP280_H_

#include "stm32wlxx_hal.h"
#include <stdbool.h>
#include <stdint.h>

/*!
 *  I2C ADDRESS/BITS/SETTINGS
 */
#define BMP280_ADDRESS 0x76 << 1 /**< The default I2C address for the sensor. */
#define BMP280_CHIPID (0x58) /**< Default chip ID. */

/*!
 * Registers available on the sensor.
 */
enum {
  BMP280_REGISTER_DIG_T1 = 0x88,
  BMP280_REGISTER_DIG_T2 = 0x8A,
  BMP280_REGISTER_DIG_T3 = 0x8C,
  BMP280_REGISTER_DIG_P1 = 0x8E,
  BMP280_REGISTER_DIG_P2 = 0x90,
  BMP280_REGISTER_DIG_P3 = 0x92,
  BMP280_REGISTER_DIG_P4 = 0x94,
  BMP280_REGISTER_DIG_P5 = 0x96,
  BMP280_REGISTER_DIG_P6 = 0x98,
  BMP280_REGISTER_DIG_P7 = 0x9A,
  BMP280_REGISTER_DIG_P8 = 0x9C,
  BMP280_REGISTER_DIG_P9 = 0x9E,
  BMP280_REGISTER_CHIPID = 0xD0,
  BMP280_REGISTER_VERSION = 0xD1,
  BMP280_REGISTER_SOFTRESET = 0xE0,
  BMP280_REGISTER_CAL26 = 0xE1, /**< R calibration = 0xE1-0xF0 */
  BMP280_REGISTER_STATUS = 0xF3,
  BMP280_REGISTER_CONTROL = 0xF4,
  BMP280_REGISTER_CONFIG = 0xF5,
  BMP280_REGISTER_PRESSUREDATA = 0xF7,
  BMP280_REGISTER_TEMPDATA = 0xFA,
};

typedef enum {
    MODE_SLEEPY = 0x00,
    MODE_FORCED = 0x01,
    MODE_NORMAL = 0x03,
    MODE_SOFT_RESET_CODE = 0xB6
} sensor_mode;

typedef enum {
    SAMPLING_NONE = 0x00,
    SAMPLING_X1   = 0x01,
    SAMPLING_X2   = 0x02,
    SAMPLING_X4   = 0x03,
    SAMPLING_X8   = 0x04,
    SAMPLING_X16  = 0x05
} sensor_sampling;

typedef enum {
    FILTER_OFF = 0x00,
    FILTER_X2  = 0x01,
    FILTER_X4  = 0x02,
    FILTER_X8  = 0x03,
    FILTER_X16 = 0x04
} sensor_filter;

typedef enum {
    STANDBY_MS_1    = 0x00,
    STANDBY_MS_63   = 0x01,
    STANDBY_MS_125  = 0x02,
    STANDBY_MS_250  = 0x03,
    STANDBY_MS_500  = 0x04,
    STANDBY_MS_1000 = 0x05,
    STANDBY_MS_2000 = 0x06,
    STANDBY_MS_4000 = 0x07
} standby_duration;

typedef struct {
    I2C_HandleTypeDef *i2cHandle;  // i2c handle pointer
    float temp_C;
    float pressure;
} BMP280;

/*!
 *  Struct to hold calibration data.
 */
typedef struct {
  uint16_t dig_T1; /**< dig_T1 cal register. */
  int16_t dig_T2;  /**<  dig_T2 cal register. */
  int16_t dig_T3;  /**< dig_T3 cal register. */
  uint16_t dig_P1; /**< dig_P1 cal register. */
  int16_t dig_P2;  /**< dig_P2 cal register. */
  int16_t dig_P3;  /**< dig_P3 cal register. */
  int16_t dig_P4;  /**< dig_P4 cal register. */
  int16_t dig_P5;  /**< dig_P5 cal register. */
  int16_t dig_P6;  /**< dig_P6 cal register. */
  int16_t dig_P7;  /**< dig_P7 cal register. */
  int16_t dig_P8;  /**< dig_P8 cal register. */
  int16_t dig_P9;  /**< dig_P9 cal register. */
} bmp280_calib_data;


typedef struct {
    sensor_mode mode;
    sensor_sampling tempSampling;
    sensor_sampling pressSampling;
    sensor_filter filter;
    standby_duration duration;
} SensorSetup;


HAL_StatusTypeDef BMP280_init (BMP280 *dev, I2C_HandleTypeDef *i2cHandle);
float readTemperature(BMP280 *dev);
float readPressure(BMP280 *dev);


bool begin(uint8_t addr, uint8_t chipid);
void reset(void);
uint8_t getStatus(void);
uint8_t sensorID(void);


/**
 * @brief Returns the default configuration.
 *
 * @return SensorConfig with the predefined values.
 */
SensorSetup getDefaultSetup(void);

/**
 * @brief Sets the sampling configuration to the sensor.
 *
 * @param config Configuration to apply.
 */
void setSampling(SensorSetup setup, BMP280 *dev);

/**
 * @brief Wrapper which applies the configuration, using the default values in 'config' is NULL.
 *
 * @param config Pointer to the personalized configuration or NULL to use default.
 */
void setSamplingWithDefaults(BMP280 *dev);


typedef struct {
    unsigned int t_sb : 3;
    unsigned int filter : 3;
    unsigned int none : 1;
    unsigned int spi3w_en : 1;
} config;

static inline uint8_t config_get(const config *cfg) {
    return (cfg->t_sb << 5) | (cfg->filter << 2) | (cfg->spi3w_en);
}

typedef struct {
    unsigned int osrs_t : 3;
    unsigned int osrs_p : 3;
    unsigned int mode : 2;
} ctrl_meas;

static inline uint8_t ctrl_meas_get(const ctrl_meas *cm) {
    return (cm->osrs_t << 5) | (cm->osrs_p << 2) | (cm->mode);
}

void readCoefficients(BMP280 *dev);
HAL_StatusTypeDef write8(BMP280 *dev, uint8_t reg, uint8_t value);
uint8_t read8(BMP280 *dev, uint8_t reg);
uint16_t read16(BMP280 *dev, uint8_t reg);
uint16_t read16_LE(BMP280 *dev, uint8_t reg);
int16_t readS16(BMP280 *dev, uint8_t reg);
int16_t readS16_LE(BMP280 *dev, uint8_t reg);
uint32_t read24(BMP280 *dev, uint8_t reg);

//uint8_t _i2caddr;

extern int32_t _sensorID;

extern int32_t t_fine;

extern bmp280_calib_data _bmp280_calib;

extern config _configReg;

extern ctrl_meas _measReg;


#endif /* BMP280_H_ */
