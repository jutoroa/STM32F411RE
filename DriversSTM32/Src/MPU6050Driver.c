/*
 * MPU6050Driver.c
 *
 *  Created on: Jun 1, 2022
 *      Author: jutoroa
 */

#include "I2CxDriver.h"
#include "MPU6050Driver.h"

int16_t getSensorValue(I2C_Handler_t *ptrHandlerI2C, uint8_t axisL, uint8_t axisH){
	uint8_t Value_low  = I2C_readByte(ptrHandlerI2C, axisL);
	uint8_t Value_high = I2C_readByte(ptrHandlerI2C, axisH);
	int16_t Value = Value_high << 8 | Value_low;
	return Value;
}

uint8_t getMPURegisterValue(I2C_Handler_t *ptrHandlerI2C, uint8_t address){
	uint8_t registerValue = I2C_readByte(ptrHandlerI2C, address);
	return registerValue;
}

/*
int16_t controlI2CMPU6050(I2C_Handler_t *ptrHandlerI2C, uint8_t rxData){
	// Devolvemos la dirección que posee el sensor
	if(rxData == 'd'){
		uint8_t WHO_AM_I = getMPURegisterValue(ptrHandlerI2C,MPU6050_RA_WHO_AM_I);
		return WHO_AM_I;
	}
	// Obtenemos el valor del registro Reset
	else if(rxData == 'p'){
		uint8_t PWR_MGMT_1 = getMPURegisterValue(ptrHandlerI2C, MPU6050_RA_PWR_MGMT_1);
		return PWR_MGMT_1;
	}
	// Escribimos 0x0 (reset) en todos los registros del MPU6050
	else if(rxData == 'r'){
		I2C_writeByte(&ptrHandlerI2C, MPU6050_RA_PWR_MGMT_1, 0x00);
	}
	// Leemos los valores del acelerómetro para x
	else if(rxData == 'x'){
		int16_t AccelX = getSensorValue(ptrHandlerI2C, MPU6050_RA_ACCEL_XOUT_L, MPU6050_RA_ACCEL_XOUT_H);
		return AccelX;
	}
	// Leemos los valores del acelerómetro para y
	else if(rxData == 'y'){
		int16_t AccelY = getSensorValue(ptrHandlerI2C, MPU6050_RA_ACCEL_YOUT_L, MPU6050_RA_ACCEL_YOUT_H);
		return AccelY;
	}
	// Leemos los valores del acelerómetro para z
	else if(rxData == 'z'){
		int16_t AccelZ = getSensorValue(ptrHandlerI2C, MPU6050_RA_ACCEL_ZOUT_L, MPU6050_RA_ACCEL_ZOUT_H);
		return AccelZ;
	}
	else{
		return rxData;
	}
}*/
