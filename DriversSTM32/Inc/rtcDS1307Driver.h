/*
 * rtcDS1307Driver.h
 *
 *  Created on: Jun 3, 2022
 *      Author: jutoroa
 */
#include <stdbool.h>
#include "I2CxDriver.h"


#ifndef RTCDS1307DRIVER_H_
#define RTCDS1307DRIVER_H_

// Driver que contendrá todas las definiciones para el RTC DS1307

#define RTC_DS1307_SECONDS				0x00
#define RTC_DS1307_MINUTES				0x01
#define RTC_DS1307_HOURS				0x02
#define RTC_DS1307_DAY					0x03
#define RTC_DS1307_DATE					0x04
#define RTC_DS1307_MONTH				0x05
#define RTC_DS1307_YEAR					0x06
#define RTC_DS1307_CONTROLREGISTER		0x07

// Estructura para definir los valores iniciales
typedef struct{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hour;
	uint8_t weekDay;
	uint8_t date;
	uint8_t month;
	uint8_t year;
}rtc_t;

// Estructura para obtener los datos
typedef struct{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hour;
	uint8_t weekDay;
	uint8_t date;
	uint8_t month;
	uint8_t year;
}getTime_t;

// Funciones para escribir un Byte en el RTC
void RTC_writeByte(I2C_Handler_t *ptrHandlerI2C, uint8_t memAddr, uint8_t dataToWrite);

// Funciones para leer un Byte en el RTC
uint8_t RTC_readByte(I2C_Handler_t *ptrHandlerI2C, uint8_t memAddr);

// Función para inicializar el RTC
void RTC_init(I2C_Handler_t *ptrHandlerI2C);

// Función para definir el tiempo y la fecha
void RTC_SetDateTime(I2C_Handler_t *ptrHandlerI2C, rtc_t *rtc);

// Función para leer todos los registros
void RTC_ReadDateTime(I2C_Handler_t *ptrHandlerI2C, getTime_t *ptrGetTime);//int *GetDateAndTime);
void RTC_ReadDateTimeFull(I2C_Handler_t *ptrHandlerI2C, uint8_t *GetDateAndTime);
// Función para convertir de decimal a BCD
uint8_t decToBCD(int val);

// Función para convertir de BCD a decimal
int BCDToDec(uint8_t val);

#endif /* RTCDS1307DRIVER_H_ */
