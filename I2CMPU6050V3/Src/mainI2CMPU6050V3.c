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
I2C_Handler_t  handlerAccelerometer = {0};



// Variables auxiliar.
char 		bufferData[256] 	= "¡Hola! Soy el USART del STM32 y estoy funcionando. \n";
uint8_t 	rxData				= '\0';
uint8_t 	i2cBuffer 			= 0;
uint8_t		MPU6050IsReady		= false;

#define numberofsensor 	6
int16_t		MPUBuffer[numberofsensor]		= {0};
// *************** // Headers // *************** //

void initSystem(void);
void sensorConfig(void);
void sensorData(void);

// *************** // MAIN // *************** //
int main(void)
{
	// Inicializamos el sistema
	initSystem();
	writeMsg(&handlerCommTerminal, bufferData);
	/* Main Loop */
	while(1){

		// Hacemos un "eco" con el valor que nos llega por el serial
		if(rxData != '\0'){
			writeChar(&handlerCommTerminal, rxData);

			if(rxData == 'c'){
				// Conversión de modo continua
				startTimer(&handlerTimer3);
			}
			if(rxData == 's'){
				stopTimer(&handlerTimer3);
			}
			// Limpiamos el valor de la variable que guarda los datos del RX
			sensorConfig();
			sensorData();
			rxData = '\0';
		}

		if(MPU6050IsReady == true){
			int16_t dataPosition = 0;
			MPUBuffer[dataPosition] = MPU6050_SensorValue(&handlerAccelerometer,ACCEL_X)*(9.81/16384.0);
			dataPosition++;
			MPUBuffer[dataPosition] = MPU6050_SensorValue(&handlerAccelerometer,ACCEL_Y)*(9.81/16384.0);
			dataPosition++;
			MPUBuffer[dataPosition] = MPU6050_SensorValue(&handlerAccelerometer,ACCEL_Z)*(9.81/16384.0);
			dataPosition++;
			MPUBuffer[dataPosition] = MPU6050_SensorValue(&handlerAccelerometer,GYRO_X)*(250.0/32768.0);
			dataPosition++;
			MPUBuffer[dataPosition] = MPU6050_SensorValue(&handlerAccelerometer,GYRO_Y)*(250.0/32768.0);
			dataPosition++;
			MPUBuffer[dataPosition] = MPU6050_SensorValue(&handlerAccelerometer,GYRO_Z)*(250.0/32768.0);
			dataPosition++;
			if (dataPosition >= numberofsensor){
				dataPosition = 0;
			}

/*
			sprintf(bufferData, "MPU6050: = ACCEL: %d | %d | %d GYROS: %d | %d | %d \n\r",
					(int) MPUBuffer[0],(int) MPUBuffer[1],(int) MPUBuffer[2],(int) MPUBuffer[3],
					(int) MPUBuffer[4],(int) MPUBuffer[5]);
			writeMsg(&handlerCommTerminal, bufferData);
*/
			sprintf(bufferData, "X: %d Y: %d Z: %d \n\r",
					(int) MPUBuffer[0],(int) MPUBuffer[1],(int) MPUBuffer[2]);
			writeMsg(&handlerCommTerminal, bufferData);

			/*
			for(uint16_t j = 0; j < numberofsensor; j++){
				sprintf(bufferData, "Giros %u = %u\n\r",(uint16_t) j,(unsigned int) MPUBuffer[j]);
				writeMsg(&handlerCommTerminal, bufferData);
			}*/
			MPU6050IsReady = false;
		}
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

	handlerAccelerometer.ptrI2Cx		= I2C1;
	handlerAccelerometer.modeI2C		= I2C_MODE_FM;
	handlerAccelerometer.slaveAddress	= 0b1101000;		// Dirección del Accel con Logic 0 (0x68)

	I2C_Config(&handlerAccelerometer);

}

void sensorConfig(void){
	// Devolvemos la dirección que posee el sensor
	if(rxData == 'd'){
		i2cBuffer = MPU6050_readByte(&handlerAccelerometer, MPU6050_RA_WHO_AM_I);
		sprintf(bufferData, "dataRead = 0x%2x \n", (unsigned int) i2cBuffer);
		writeMsg(&handlerCommTerminal, bufferData);
		rxData = '\0';
	}
	// Obtenemos el valor del registro Reset
	else if(rxData == 'p'){
		i2cBuffer = MPU6050_readByte(&handlerAccelerometer, MPU6050_RA_PWR_MGMT_1);
		sprintf(bufferData, "dataRead = 0x%2x \n", (unsigned int) i2cBuffer);
		writeMsg(&handlerCommTerminal, bufferData);
		rxData = '\0';
	}
	// Escribimos 0x0 (reset) en todos los registros del MPU6050
	else if(rxData == 'r'){
		MPU6050_writeByte(&handlerAccelerometer, MPU6050_RA_PWR_MGMT_1, 0x00);
		rxData = '\0';
	}
}

void sensorData(void){
	// Leemos los valores del acelerómetro para x
	if(rxData == 'x'){
		int16_t AccelX = MPU6050_SensorValue(&handlerAccelerometer,ACCEL_X);
		sprintf(bufferData, "AccelX = %d \n",(int) AccelX);
		writeMsg(&handlerCommTerminal, bufferData);
		rxData = '\0';
	}
	// Leemos los valores del acelerómetro para y
	else if(rxData == 'y'){
		int16_t AccelY = MPU6050_SensorValue(&handlerAccelerometer,ACCEL_Y);
		sprintf(bufferData, "AccelY = %d \n",(int) AccelY);
		writeMsg(&handlerCommTerminal, bufferData);
		rxData = '\0';
	}
	// Leemos los valores del acelerómetro para z
	else if(rxData == 'z'){
		int16_t AccelZ = MPU6050_SensorValue(&handlerAccelerometer,ACCEL_Z);
		sprintf(bufferData, "AccelZ = %d \n",(int) AccelZ);
		writeMsg(&handlerCommTerminal, bufferData);
		rxData = '\0';
	}
	// Leemos los valores del giroscopio para x
	else if(rxData == 'i'){
		int16_t GirosX = MPU6050_SensorValue(&handlerAccelerometer,GYRO_X);
		sprintf(bufferData, "GirosX = %d \n",(int) GirosX);
		writeMsg(&handlerCommTerminal, bufferData);
		rxData = '\0';
	}
	else if(rxData == 'j'){
		int16_t GirosY = MPU6050_SensorValue(&handlerAccelerometer,GYRO_Y);
		sprintf(bufferData, "GirosY = %d \n",(int) GirosY);
		writeMsg(&handlerCommTerminal, bufferData);
		rxData = '\0';
	}
	else if(rxData == 'k'){
		int16_t GirosZ = MPU6050_SensorValue(&handlerAccelerometer,GYRO_Z);
		sprintf(bufferData, "GirosZ = %d \n",(int) GirosZ);
		writeMsg(&handlerCommTerminal, bufferData);
		rxData = '\0';
	}
	else if(rxData == 't'){
		int16_t Temp = MPU6050_SensorValue(&handlerAccelerometer,TEMP);
		sprintf(bufferData, "Temp = %d \n",(int) Temp);
		writeMsg(&handlerCommTerminal, bufferData);
		rxData = '\0';
	}
	else{
		rxData = '\0';
	}
}

//***********// CallBacks //***********//

void USART2_Callback(void){
	// Lectura de los datos recibidos
	rxData = returnData();
}
// Timer encargado del StateLED
void Timer2_Callback(void){
	handlerStateLED.pGPIOx -> ODR ^= GPIO_ODR_OD5;		// Encendido y apagado StateLED
}

// Timer encargado del muestreo a través del MPU6050
void Timer3_Callback(void){
	MPU6050IsReady = true;
}
