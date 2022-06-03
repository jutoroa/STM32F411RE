/*
 * rtcDS1307Driver.c
 *
 *  Created on: Jun 3, 2022
 *      Author: jutoroa
 */

#include "rtcDS1307Driver.h"

// Funcion para inicializar el RTC. Escribimos en el Control Register para desactivar el SQW - Out
void RTC_init(I2C_Handler_t *ptrHandlerI2C){

	RTC_writeByte(ptrHandlerI2C,0x07,0b00000000);
	RTC_writeByte(ptrHandlerI2C,0x00,0b00000000);

}

void RTC_SetDateTime(I2C_Handler_t *ptrHandlerI2C, rtc_t *rtc){

	startI2C(ptrHandlerI2C);

	sendSlaveAddressWriteI2C(ptrHandlerI2C);

	uint8_t memAddr = 0x00;

	sendMemoryAddressI2C(ptrHandlerI2C,memAddr);

	sendDataI2C(ptrHandlerI2C, decToBCD(rtc -> seconds));

	sendDataI2C(ptrHandlerI2C, decToBCD(rtc -> minutes));

	sendDataI2C(ptrHandlerI2C, decToBCD(rtc -> hour));

	sendDataI2C(ptrHandlerI2C, decToBCD(rtc -> weekDay));

	sendDataI2C(ptrHandlerI2C, decToBCD(rtc -> date));

	sendDataI2C(ptrHandlerI2C, decToBCD(rtc -> month));

	sendDataI2C(ptrHandlerI2C, decToBCD(rtc -> year));

	stopI2C(ptrHandlerI2C);
}

int RTC_ReadDateTime(I2C_Handler_t *ptrHandlerI2C, rtc_t *rtc){

	startI2C(ptrHandlerI2C);

	sendSlaveAddressWriteI2C(ptrHandlerI2C);

	uint8_t memAddr = 0x00;

	sendMemoryAddressI2C(ptrHandlerI2C,memAddr);

	stopI2C(ptrHandlerI2C);

	startI2C(ptrHandlerI2C);

	sendSlaveAddressReadI2C(ptrHandlerI2C);

	int DateAndTime[7];

	DateAndTime[0] = BCDToDec(recibeDataI2C(ptrHandlerI2C));

	DateAndTime[1] = BCDToDec(recibeDataI2C(ptrHandlerI2C));

	DateAndTime[2] = BCDToDec(recibeDataI2C(ptrHandlerI2C));

	DateAndTime[3] = BCDToDec(recibeDataI2C(ptrHandlerI2C));

	DateAndTime[4] = BCDToDec(recibeDataI2C(ptrHandlerI2C));

	DateAndTime[5] = BCDToDec(recibeDataI2C(ptrHandlerI2C));

	DateAndTime[6] = BCDToDec(recibeDataI2C(ptrHandlerI2C));

	nACKI2C(ptrHandlerI2C);

	stopI2C(ptrHandlerI2C);

	return *DateAndTime;
}

uint8_t RTC_readByte(I2C_Handler_t *ptrHandlerI2C, uint8_t memAddr){

	startI2C(ptrHandlerI2C);

	sendSlaveAddressWriteI2C(ptrHandlerI2C);

	sendMemoryAddressI2C(ptrHandlerI2C,memAddr);

	stopI2C(ptrHandlerI2C);

	startI2C(ptrHandlerI2C);

	//reStartI2C(ptrHandlerI2C);

	sendSlaveAddressReadI2C(ptrHandlerI2C);

	uint8_t dataI2C = recibeDataI2C(ptrHandlerI2C);

	nACKI2C(ptrHandlerI2C);

	stopI2C(ptrHandlerI2C);

	//uint8_t dataI2C = recibeDataI2C(ptrHandlerI2C);

	return dataI2C;

}

void RTC_writeByte(I2C_Handler_t *ptrHandlerI2C, uint8_t memAddr, uint8_t dataToWrite){

	startI2C(ptrHandlerI2C);

	sendSlaveAddressWriteI2C(ptrHandlerI2C);

	sendMemoryAddressI2C(ptrHandlerI2C,memAddr);

	sendDataI2C(ptrHandlerI2C,dataToWrite);

	stopI2C(ptrHandlerI2C);
}

// Función para convertir de numeros decimales a código BCD
uint8_t decToBCD(int val){
	return (uint8_t) ((val/10*16) + (val%10));
}

// Función para convertir de código BCD a numeros decimales

int BCDToDec(uint8_t val){
	return (int) ((val/16*10) + (val%16));
}
