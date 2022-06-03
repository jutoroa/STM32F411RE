/*
 * MPU6050Driver.c
 *
 *  Created on: Jun 1, 2022
 *      Author: jutoroa
 */

#include "MPU6050Driver.h"

uint8_t MPU6050_readByte(I2C_Handler_t *ptrHandlerI2C, uint8_t memAddr){

	startI2C(ptrHandlerI2C);

	sendSlaveAddressWriteI2C(ptrHandlerI2C);

	sendMemoryAddressI2C(ptrHandlerI2C,memAddr);

	reStartI2C(ptrHandlerI2C);

	sendSlaveAddressReadI2C(ptrHandlerI2C);

	nACKI2C(ptrHandlerI2C);

	stopI2C(ptrHandlerI2C);

	uint8_t dataI2C = recibeDataI2C(ptrHandlerI2C);

	return dataI2C;
}

void MPU6050_writeByte(I2C_Handler_t *ptrHandlerI2C, uint8_t memAddr, uint8_t dataToWrite){

	startI2C(ptrHandlerI2C);

	sendSlaveAddressWriteI2C(ptrHandlerI2C);

	sendMemoryAddressI2C(ptrHandlerI2C,memAddr);

	sendDataI2C(ptrHandlerI2C,dataToWrite);

	stopI2C(ptrHandlerI2C);
}

int16_t MPU6050_SensorValue(I2C_Handler_t *ptrHandlerI2C, uint8_t sensorAndAxis){

	uint8_t axisL;
	uint8_t axisH;

	switch(sensorAndAxis){
	case(ACCEL_X):
	{
		axisL = MPU6050_RA_GYRO_XOUT_L;
		axisH = MPU6050_RA_GYRO_XOUT_H;
		break;
	}
	case(ACCEL_Y):
	{
		axisL = MPU6050_RA_GYRO_YOUT_L;
		axisH = MPU6050_RA_GYRO_YOUT_H;
		break;
	}
	case(ACCEL_Z):
	{
		axisL = MPU6050_RA_GYRO_ZOUT_L;
		axisH = MPU6050_RA_GYRO_ZOUT_H;
		break;
	}
	case(TEMP):
	{
		axisL = MPU6050_RA_TEMP_OUT_L;
		axisH = MPU6050_RA_TEMP_OUT_H;
		break;
	}
	case(GYRO_X):
	{
		axisL = MPU6050_RA_GYRO_XOUT_L;
		axisH = MPU6050_RA_GYRO_XOUT_H;
		break;
	}
	case(GYRO_Y):
	{
		axisL = MPU6050_RA_GYRO_YOUT_L;
		axisH = MPU6050_RA_GYRO_YOUT_H;
		break;
	}
	case(GYRO_Z):
	{
		axisL = MPU6050_RA_GYRO_ZOUT_L;
		axisH = MPU6050_RA_GYRO_ZOUT_H;
		break;
	}
}
	uint8_t Value_low  = MPU6050_readByte(ptrHandlerI2C, axisL);
	uint8_t Value_high = MPU6050_readByte(ptrHandlerI2C, axisH);
	int16_t Value = Value_high << 8 | Value_low;
	return Value;
}

uint8_t getMPURegisterValue(I2C_Handler_t *ptrHandlerI2C, uint8_t address){
	uint8_t registerValue = I2C_readByte(ptrHandlerI2C, address);
	return registerValue;
}
