/*
 * MPE280.h
 *
 *  Created on: Jun 14, 2022
 *      Author: namontoy
 */

#ifndef MPE280DRIVER_H_
#define MPE280DRIVER_H_

#include <stdint.h>
#include "GPIOxDriver.h"
#include "SpiDriver.h"

#define READ_OPERATION		0x80

#define MPE280_CHIP_ID		0xD0
#define MPE280_TEMP_XLSB	0xFC
#define MPE280_TEMP_LSB		0xFB
#define MPE280_TEMP_MSB		0xFA
#define MPE280_PRESS_XLSB	0xF9
#define MPE280_PRESS_LSB	0xF8
#define MPE280_PRESS_MSB	0xF7
#define MPE280_CONFIG		0xF5
#define MPE280_CTRL_MEAS	0xF4
#define MPE280_STATUS		0xF4
#define MPE280_RESET		0xE0
#define MPE280_CALIBRATION	0x88

#define MPE280_I2C_ADDRESS	0x76
#define MPE280_ID_VALUE		0x60

#define BMP280_OK						0
#define BMP280_ERROR_32BIT_COMP_TEMP	1
#define BMP280_ERROR_32BIT_COMP_PRESS	2

/* Prototipos de las funciones */
void mpe280_init(void);

void mpe280_readData(uint8_t address, uint8_t * ptrBufferData, uint8_t dataSize);
void mpe280_writeData(uint8_t address, uint8_t value);


#endif /* MPE280DRIVER_H_ */
