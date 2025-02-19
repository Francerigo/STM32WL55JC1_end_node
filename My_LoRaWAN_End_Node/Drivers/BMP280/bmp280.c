/*
 * bmp280.c
 *
 *  Created on: Feb 13, 2025
 *      Author: LabMeas
 */

#include "bmp280.h"

config _configReg;
ctrl_meas _measReg;
bmp280_calib_data _bmp280_calib;
int32_t t_fine;
int32_t _sensorID;

/**
 * @brief Inizializza il sensore BMP280 usando I2C.
 * @param dev Puntatore alla struttura BMP280.
 * @param i2chandle Puntatore all'I2C_HandleTypeDef (già inizializzato a livello di sistema).
 * @return HAL_OK se l’inizializzazione ha successo, HAL_ERROR altrimenti.
 */
HAL_StatusTypeDef BMP280_init(BMP280 *dev, I2C_HandleTypeDef *i2cHandle) {

    dev->i2cHandle = i2cHandle;
    dev->temp_C = 0.0f;
    dev->pressure= 0.0f;

    _sensorID = read8(dev, BMP280_REGISTER_CHIPID);

    if (_sensorID != BMP280_CHIPID) return HAL_ERROR;

    readCoefficients(dev);
    // write8(BMP280_REGISTER_CONTROL, 0x3F); /* needed? */
    setSamplingWithDefaults(dev);

    //delay(100);
    return HAL_OK;
}

/**************************************************************************/
/*!
    @brief  Writes an 8 bit value over I2C
*/
/**************************************************************************/

HAL_StatusTypeDef write8(BMP280 *dev, uint8_t reg, uint8_t value) {
    return HAL_I2C_Mem_Write(dev->i2cHandle, BMP280_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, &value, 1, HAL_MAX_DELAY);
}
/*!
 *  @brief  Reads an 8 bit value over I2C
 *  @param  reg
 *          selected register
 *  @return value from selected register
 */

uint8_t read8(BMP280 *dev, uint8_t reg) {
    uint8_t value;
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(dev->i2cHandle, BMP280_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, &value, 1, HAL_MAX_DELAY);
    if(status != HAL_OK) {
    	return 0;
    }
    return value;
}

/*!
 *  @brief  Reads a 16 bit value over I2C
 */
uint16_t read16 ( BMP280 *dev, uint8_t reg) {
    uint8_t buffer[2];
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read ( dev->i2cHandle, BMP280_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, buffer, 2, HAL_MAX_DELAY);
    if(status != HAL_OK) {
    	return 0;
    }
    return ((uint16_t)buffer[0] << 8) | buffer[1];
}

uint16_t read16_LE(BMP280 *dev, uint8_t reg) {
    uint8_t buffer[2];
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(dev->i2cHandle, BMP280_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, buffer, 2, HAL_MAX_DELAY);
    if(status != HAL_OK) {
    	return 0;
    }
    return ((uint16_t)buffer[1] << 8) | buffer[0];
}

/*!
 *   @brief  Reads a signed 16 bit value over I2C
 */

int16_t readS16(BMP280 *dev, uint8_t reg) {
    uint8_t buffer[2];
    HAL_StatusTypeDef status;
    status = HAL_I2C_Mem_Read(dev->i2cHandle, BMP280_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, buffer, 2, HAL_MAX_DELAY);
    if(status != HAL_OK) {
    	return 0;
    }
    return (int16_t)(((uint16_t)buffer[0] << 8) | buffer[1]);
}

int16_t readS16_LE(BMP280 *dev, uint8_t reg) {
    uint8_t buffer[2];
    HAL_StatusTypeDef status;
    status = HAL_I2C_Mem_Read(dev->i2cHandle, BMP280_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, buffer, 2, HAL_MAX_DELAY);
    if(status != HAL_OK) {
    	return 0;
    }
    return (int16_t)(((uint16_t)buffer[1] << 8) | buffer[0]);
}

/*!
 *  @brief  Reads a 24 bit value over I2C
 */

uint32_t read24(BMP280 *dev, uint8_t reg) {
    uint8_t buffer[3];
    HAL_StatusTypeDef status;
    status = HAL_I2C_Mem_Read(dev->i2cHandle, BMP280_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, buffer, 3, HAL_MAX_DELAY);
    if(status != HAL_OK) {
    	return 0;
    }
    return ((uint32_t)buffer[0] << 16) | ((uint32_t)buffer[1] << 8) | buffer[2];
}

/*!
 * Reads the temperature from the device.
 * @return The temperature in degrees celsius in a float pointer.
 */

float readTemperature(BMP280 *dev) {
  int32_t var1, var2;
  if (!_sensorID)
    return 0; // begin() not called yet

  int32_t adc_T = read24(dev, BMP280_REGISTER_TEMPDATA);
  adc_T >>= 4;

  var1 = ((((adc_T >> 3) - ((int32_t)_bmp280_calib.dig_T1 << 1))) *
          ((int32_t)_bmp280_calib.dig_T2)) >>
         11;

  var2 = (((((adc_T >> 4) - ((int32_t)_bmp280_calib.dig_T1)) *
            ((adc_T >> 4) - ((int32_t)_bmp280_calib.dig_T1))) >>
           12) *
          ((int32_t)_bmp280_calib.dig_T3)) >>
         14;

  t_fine = var1 + var2;

  float T = (t_fine * 5 + 128) >> 8;
  return T / 100;
}

/*!
 * Reads the barometric pressure from the device.
 * @return Barometric pressure in Pa  in a float pointer.
 */
float readPressure(BMP280 *dev) {
  int64_t var1, var2, p;
  if (!_sensorID)
    return 0; // begin() not called yet

  // Must be done first to get the t_fine variable set up
  readTemperature(dev);

  int32_t adc_P = read24(dev, BMP280_REGISTER_PRESSUREDATA);
  adc_P >>= 4;

  var1 = ((int64_t)t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)_bmp280_calib.dig_P6;
  var2 = var2 + ((var1 * (int64_t)_bmp280_calib.dig_P5) << 17);
  var2 = var2 + (((int64_t)_bmp280_calib.dig_P4) << 35);
  var1 = ((var1 * var1 * (int64_t)_bmp280_calib.dig_P3) >> 8) +
         ((var1 * (int64_t)_bmp280_calib.dig_P2) << 12);
  var1 =
      (((((int64_t)1) << 47) + var1)) * ((int64_t)_bmp280_calib.dig_P1) >> 33;

  if (var1 == 0) {
    return 0; // avoid exception caused by division by zero
  }
  p = 1048576 - adc_P;
  p = (((p << 31) - var2) * 3125) / var1;
  var1 = (((int64_t)_bmp280_calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
  var2 = (((int64_t)_bmp280_calib.dig_P8) * p) >> 19;

  p = ((p + var1 + var2) >> 8) + (((int64_t)_bmp280_calib.dig_P7) << 4);
  return (float)p / 256;
}


/*!
 *  @brief  Reads the factory-set coefficients
 */
void readCoefficients(BMP280 *dev) {
  _bmp280_calib.dig_T1 = read16_LE(dev, BMP280_REGISTER_DIG_T1);
  _bmp280_calib.dig_T2 = readS16_LE(dev, BMP280_REGISTER_DIG_T2);
  _bmp280_calib.dig_T3 = readS16_LE(dev, BMP280_REGISTER_DIG_T3);
  _bmp280_calib.dig_P1 = read16_LE(dev, BMP280_REGISTER_DIG_P1);
  _bmp280_calib.dig_P2 = readS16_LE(dev, BMP280_REGISTER_DIG_P2);
  _bmp280_calib.dig_P3 = readS16_LE(dev, BMP280_REGISTER_DIG_P3);
  _bmp280_calib.dig_P4 = readS16_LE(dev, BMP280_REGISTER_DIG_P4);
  _bmp280_calib.dig_P5 = readS16_LE(dev, BMP280_REGISTER_DIG_P5);
  _bmp280_calib.dig_P6 = readS16_LE(dev, BMP280_REGISTER_DIG_P6);
  _bmp280_calib.dig_P7 = readS16_LE(dev, BMP280_REGISTER_DIG_P7);
  _bmp280_calib.dig_P8 = readS16_LE(dev, BMP280_REGISTER_DIG_P8);
  _bmp280_calib.dig_P9 = readS16_LE(dev, BMP280_REGISTER_DIG_P9);
}


/*
// functions to get and set the sampling setup of the sensor
*/

SensorSetup getDefaultSetup(void) {
    SensorSetup defaultSetup;
    defaultSetup.mode = MODE_NORMAL;
    defaultSetup.tempSampling = SAMPLING_X16;
    defaultSetup.pressSampling = SAMPLING_X16;
    defaultSetup.filter = FILTER_OFF;
    defaultSetup.duration = STANDBY_MS_1;
    return defaultSetup;
}

/*!
 * Sets the sampling config for the device.
 * @param mode
 *        The operating mode of the sensor.
 * @param tempSampling
 *        The sampling scheme for temp readings.
 * @param pressSampling
 *        The sampling scheme for pressure readings.
 * @param filter
 *        The filtering mode to apply (if any).
 * @param duration
 *        The sampling duration.
 */

void setSampling(SensorSetup setup, BMP280 *dev) {

	if (!_sensorID)
	    return; // begin() not called yet

    _configReg.t_sb = setup.duration;      // Tempo di standby
    _configReg.filter = setup.filter;      // Impostazioni del filtro
    _configReg.none = 0;
    _configReg.spi3w_en = 0;

    _measReg.osrs_t = setup.tempSampling;  // Oversampling per la temperatura
    _measReg.osrs_p = setup.pressSampling; // Oversampling per la pressione
    _measReg.mode   = setup.mode;          // Modalità operativa

    // Merge the bit parts into bytes
    uint8_t configByte = config_get(&_configReg);
    uint8_t ctrlByte   = ctrl_meas_get(&_measReg);

    write8(dev, BMP280_REGISTER_CONFIG, configByte);
    write8(dev, BMP280_REGISTER_CONTROL, ctrlByte);

}

void setSamplingWithDefaults(BMP280 *dev) {
    SensorSetup stp = getDefaultSetup();
    setSampling(stp, dev);
}

/*
// functions to get and set the configuration of the sensor
*/

config getDefaultConfig (void) {
	config defaultConfig;
	defaultConfig.t_sb = STANDBY_MS_1;
	defaultConfig.filter = FILTER_OFF;
	defaultConfig.none = 0;
	defaultConfig.spi3w_en = 0;
	return defaultConfig;
}

void setConfig (config cfg, BMP280 *dev){

}

void setConfigWithDefaults(BMP280 *dev){
    config cfg = getDefaultConfig();
    setConfig(cfg, dev);
}

/*
// functions to get and set the measurement control
*/

ctrl_meas getDefaultCtrlMeas (void) {
	ctrl_meas defaultCtrlMeas;
	defaultCtrlMeas.osrs_t = SAMPLING_NONE;
	defaultCtrlMeas.osrs_p = SAMPLING_NONE;
	defaultCtrlMeas.mode = MODE_SLEEPY;
	return defaultCtrlMeas;
}

void setControl (ctrl_meas ctms, BMP280 *dev){
}

void setControlWithDefaults(BMP280 *dev){
    ctrl_meas ctms = getDefaultCtrlMeas();
    setControl(ctms, dev);
}


