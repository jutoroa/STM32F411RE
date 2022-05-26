/*
 * I2CxDriver.h
 *
 *  Created on: 18/05/2022
 *      Author: Juan Pablo
 */

#ifndef I2CXDRIVER_H_
#define I2CXDRIVER_H_

#include "stm32f4xx.h"

#define I2C_WRITE_DATA		0
#define I2C_READ_DATA		1

#define MAIN_CLOCK_4_MHz_I2C	4
#define MAIN_CLOCK_16_MHz_I2C	16
#define MAIN_CLOCK_20_MHz_I2C	20

#define  I2C_MODE_SM		0
#define  I2C_MODE_FM		1

#define I2C_MODE_SM_SPEED_100KHz	80
#define I2C_MODE_FM_SPEED_400Khz	14

#define I2C_MAX_RISE_TIME_SM		17
#define I2C_MAX_RISE_TIME_FM		6

/*
#define		I2C_1	0
#define 	I2C_2	1
#define 	I2C_3	2
*/

typedef struct{
	I2C_TypeDef	*ptrI2Cx;
	uint8_t		slaveAddress;
	uint8_t		modeI2C;
	uint8_t		dataI2C;
//	uint8_t		pinNumber;
//	uint8_t		frecuency;	//I2C_CR2_FREQ_3
}I2C_Handler_t;

void I2C_Config(I2C_Handler_t *ptrHandlerI2C);
uint8_t I2C_readByte(I2C_Handler_t *ptrHandlerI2C, uint8_t memAddr);
void I2C_writeByte(I2C_Handler_t *ptrHandlerI2C, uint8_t memAddr, uint8_t dataToWrite);


//void configI2CPin(I2C_Handler_t *ptrHandlerI2C);

#endif /* I2CXDRIVER_H_ */
