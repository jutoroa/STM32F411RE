/*
 * I2CxDriver.h
 *
 *  Created on: 18/05/2022
 *      Author: Juan Pablo
 */

#ifndef I2CXDRIVER_H_
#define I2CXDRIVER_H_

#include "stm32f4xx.h"

#define		I2C_1	0
#define 	I2C_2	1
#define 	I2C_3	2

typedef struct{
	uint8_t		i2cnumber;
	uint8_t		pinNumber;
	uint8_t		frecuency;	//I2C_CR2_FREQ_3
}I2C_Config_t;

void I2C_Config(I2C_Config_t *i2cConfig);
void configI2CPin(I2C_Config_t *i2cConfig);

#endif /* I2CXDRIVER_H_ */
