/*
 * I2CMPU6050.c
 *
 *  Created on: 4/05/2022
 *  Author: Juan Pablo
 *
 */


// *************** // INCLUDES // *************** //

#include <stm32f4xx.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "GPIOxDriver.h"
#include "USARTxDriver.h"
#include "TIMxDriver.h"
#include "AdcDriver.h"
#include "EXTIxDriver.h"
#include "I2CxDriver.h"
#include "SysTickDriver.h"
#include "MPU6050Driver.h"

// *************** // VARIABLES PROYECTO // *************** //

GPIO_Handler_t 	handlerStateLED 		= {0};	// StateLED
GPIO_Handler_t 	handlerPinTX 			= {0};	// handlerPinTX
GPIO_Handler_t 	handlerPinRX 			= {0};	// handlerPinRX
TIMER_Handler_t handlerTimer2 			= {0};	// Timer2
TIMER_Handler_t handlerTimer3 			= {0};	// Timer3
USART_Handler_t handlerCommTerminal		= {0};	// Usart para la terminal

/* Configuración para el I2C */
GPIO_Handler_t handlerI2CSDA =	{0};
GPIO_Handler_t handlerI2CSCL =	{0};
I2C_Handler_t  handlerRTC	 = {0};



// Variables auxiliar.
char 		bufferData[64] 		= "esto es una pequeña prueba";
uint8_t 	rxData				= 0;
uint8_t 	i2cBuffer 			= 0;
uint32_t 	systemTicks 		= 0;
uint32_t 	systemTicksStart 	= 0;
uint32_t 	systemTicksEnd		= 0;
uint8_t		MPU6050IsReady		= false;

#define numberofsensor 	3
int16_t		MPUBuffer[numberofsensor]		= {0};
// *************** // Headers // *************** //

void initSystem(void);
void sensorConfig();
void sensorData(void);

// *************** // MAIN // *************** //
int main(void)
{
	// Inicializamos el sistema
	initSystem();
	writeMsg(&handlerCommTerminal, bufferData);
	I2C_writeByte_RTC(&handlerRTC,0x0,0b00000000);
	/* Main Loop */
	while(1){

		// Hacemos un "eco" con el valor que nos llega por el serial
		if(rxData != '\0'){
			writeChar(&handlerCommTerminal, rxData);

			//sensorConfig();
			//sensorData();

			// Leemos los valores del los segundos transcurridos
			if(rxData == 's'){
				uint8_t sec = I2C_readByte_RTC(&handlerRTC,0x0);
				uint8_t seconds = (sec & 0b00001111);
				sprintf(bufferData, "Segundos = %d \n",(int) seconds);
				writeMsg(&handlerCommTerminal, bufferData);
				rxData = '\0';
			}
			// Leemos los valores del acelerómetro para y
			else if(rxData == 'm'){
				uint8_t min = I2C_readByte_RTC(&handlerRTC,0x01);
				uint8_t minutes = (min & 0b00001111);
				sprintf(bufferData, "Minutos = %d \n",(int) minutes);
				writeMsg(&handlerCommTerminal, bufferData);
				rxData = '\0';
			}
			rxData = '\0';
		}

//		if(MPU6050IsReady == true){
//			//sprintf(bufferData, "ADC = %u, %u \n\r",(unsigned int ) adcData,(unsigned int )counter);
//			for(uint16_t j = 0; j < numberofsensor; j++){
//				sprintf(bufferData, "Giros %u = %u\n\r",(uint16_t) j,(unsigned int) MPUBuffer[j]);
//				writeMsg(&handlerCommTerminal, bufferData);
//			}
//			MPU6050IsReady = false;
//		}
	}
}
//***********// *********** // Definición de Funciones // *********** //***********//

//***********// InitSystem //***********//

// Función que define la configuración de todos los pines y periféricos

void initSystem(void){

	// Configuración el State LED
	handlerStateLED.pGPIOx								= GPIOA;
	handlerStateLED.GPIO_PinConfig.GPIO_PinNumber		= PIN_5;
	handlerStateLED.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerStateLED.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerStateLED.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerStateLED.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;

	//Cargamos la configuración
	GPIO_Config(&handlerStateLED);

	// Prendemos el LED
	GPIO_WritePin(&handlerStateLED, SET);

	/* Configurando los pines sobre los que funciona el USART */
	handlerPinTX.pGPIOx									= GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber			= PIN_2;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerPinTX.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerPinTX.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode		= AF7;

	GPIO_Config(&handlerPinTX);

	handlerPinRX.pGPIOx									= GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber			= PIN_3;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerPinRX.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerPinRX.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode		= AF7;

	GPIO_Config(&handlerPinRX);

	// Configuración del USART del USB para RX
	handlerCommTerminal.ptrUSARTx							= USART2;
	handlerCommTerminal.USART_Config.USART_mode				= USART_MODE_RXTX;
	handlerCommTerminal.USART_Config.USART_baudrate			= USART_BAUDRATE_19200;
	handlerCommTerminal.USART_Config.USART_datasize			= USART_DATASIZE_8BIT;
	handlerCommTerminal.USART_Config.USART_parity			= USART_PARITY_ODD;
	handlerCommTerminal.USART_Config.USART_stopbits			= USART_STOPBIT_1;
	handlerCommTerminal.USART_Config.USART_interrupt		= USART_INTERRUPT_RX_ENABLE;

	USART_Config(&handlerCommTerminal);

	// Configuración del timer2
	handlerTimer2.ptrTIMx								= TIM2;
	handlerTimer2.timerConfig.Timer_mode				= TIMER_MODE_UP;
	handlerTimer2.timerConfig.Timer_speed				= TIMER_INCR_SPEED_1ms;
	handlerTimer2.timerConfig.Timer_period				= 250;

	Timer_Config(&handlerTimer2);

	// Configuración del timer3
	handlerTimer3.ptrTIMx								= TIM3;
	handlerTimer3.timerConfig.Timer_mode				= TIMER_MODE_UP;
	handlerTimer3.timerConfig.Timer_speed				= TIMER_INCR_SPEED_1ms;
	handlerTimer3.timerConfig.Timer_period				= 250;

	Timer_Config(&handlerTimer3);

	// Llamamos la función para configurar y activar el SysTick

	config_SysTickMs();

	// Configuramos los pines para el I2C SCL
	handlerI2CSCL.pGPIOx								= GPIOB;
	handlerI2CSCL.GPIO_PinConfig.GPIO_PinNumber			= PIN_6;
	handlerI2CSCL.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerI2CSCL.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_OPENDRAIN;
	handlerI2CSCL.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerI2CSCL.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerI2CSCL.GPIO_PinConfig.GPIO_PinAltFunMode		= AF4;

	GPIO_Config(&handlerI2CSCL);

	// Configuramos los pines para el I2C SDA
	handlerI2CSDA.pGPIOx								= GPIOB;
	handlerI2CSDA.GPIO_PinConfig.GPIO_PinNumber			= PIN_7;
	handlerI2CSDA.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerI2CSDA.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_OPENDRAIN;
	handlerI2CSDA.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerI2CSDA.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerI2CSDA.GPIO_PinConfig.GPIO_PinAltFunMode		= AF4;

	GPIO_Config(&handlerI2CSDA);

	handlerRTC.ptrI2Cx		= I2C1;
	handlerRTC.modeI2C		= I2C_MODE_FM;
	handlerRTC.slaveAddress	= 0b1101000;		// Dirección del Accel con Logic 0 (0x68)

	I2C_Config(&handlerRTC);

}
/*
void sensorConfig(){
	// Devolvemos la dirección que posee el sensor
	if(rxData == 'd'){
		i2cBuffer = I2C_readByte(&handlerAccelerometer, MPU6050_RA_WHO_AM_I);
		sprintf(bufferData, "dataRead = 0x%2x \n", (unsigned int) i2cBuffer);
		writeMsg(&handlerCommTerminal, bufferData);
		rxData = '\0';
	}
	// Obtenemos el valor del registro Reset
	else if(rxData == 'p'){
		i2cBuffer = I2C_readByte(&handlerAccelerometer, MPU6050_RA_PWR_MGMT_1);
		sprintf(bufferData, "dataRead = 0x%2x \n", (unsigned int) i2cBuffer);
		writeMsg(&handlerCommTerminal, bufferData);
		rxData = '\0';
	}
	// Escribimos 0x0 (reset) en todos los registros del MPU6050
	else if(rxData == 'r'){
		I2C_writeByte(&handlerAccelerometer, MPU6050_RA_PWR_MGMT_1, 0x00);
		rxData = '\0';
	}
}

void sensorData(void){
	// Leemos los valores del acelerómetro para x
	if(rxData == 'x'){
		int16_t AccelX = getSensorValue(&handlerAccelerometer,
				MPU6050_RA_ACCEL_XOUT_L, MPU6050_RA_ACCEL_XOUT_H);
		sprintf(bufferData, "AccelX = %d \n",(int) AccelX);
		writeMsg(&handlerCommTerminal, bufferData);
		rxData = '\0';
	}
	// Leemos los valores del acelerómetro para y
	else if(rxData == 'y'){
		int16_t AccelY = getSensorValue(&handlerAccelerometer,
				MPU6050_RA_ACCEL_YOUT_L, MPU6050_RA_ACCEL_YOUT_H);
		sprintf(bufferData, "AccelY = %d \n",(int) AccelY);
		writeMsg(&handlerCommTerminal, bufferData);
		rxData = '\0';
	}
	// Leemos los valores del acelerómetro para z
	else if(rxData == 'z'){
		int16_t AccelZ = getSensorValue(&handlerAccelerometer,
				MPU6050_RA_ACCEL_ZOUT_L, MPU6050_RA_ACCEL_ZOUT_H);
		sprintf(bufferData, "AccelX = %d \n",(int) AccelZ);
		writeMsg(&handlerCommTerminal, bufferData);
		rxData = '\0';
	}
	// Leemos los valores del giroscopio para x
	else if(rxData == 'i'){
		int16_t GirosX = getSensorValue(&handlerAccelerometer,
				MPU6050_RA_GYRO_XOUT_L, MPU6050_RA_GYRO_XOUT_H);
		sprintf(bufferData, "GirosX = %d \n",(int) GirosX);
		writeMsg(&handlerCommTerminal, bufferData);
		rxData = '\0';
	}
	else if(rxData == 'j'){
		int16_t GirosY = getSensorValue(&handlerAccelerometer,
				MPU6050_RA_GYRO_YOUT_L, MPU6050_RA_GYRO_YOUT_H);
		sprintf(bufferData, "GirosY = %d \n",(int) GirosY);
		writeMsg(&handlerCommTerminal, bufferData);
		rxData = '\0';
	}
	else if(rxData == 'k'){
		int16_t GirosZ = getSensorValue(&handlerAccelerometer,
				MPU6050_RA_GYRO_ZOUT_L, MPU6050_RA_GYRO_ZOUT_H);
		sprintf(bufferData, "GirosZ = %d \n",(int) GirosZ);
		writeMsg(&handlerCommTerminal, bufferData);
		rxData = '\0';
	}
	else if(rxData == 't'){
		int16_t Temp = getSensorValue(&handlerAccelerometer,
				MPU6050_RA_TEMP_OUT_L, MPU6050_RA_TEMP_OUT_H);
		sprintf(bufferData, "Temp = %d \n",(int) Temp);
		writeMsg(&handlerCommTerminal, bufferData);
		rxData = '\0';
	}
	else{
		rxData = '\0';
	}
}
*/

//***********// CallBacks //***********//

void USART2_Callback(void){
	// Lectura de los datos recibidos
	rxData = returnData();
}
// Timer encargado del StateLED
void Timer2_Callback(void){
	handlerStateLED.pGPIOx -> ODR ^= GPIO_ODR_OD5;		// Encendido y apagado StateLED
}
