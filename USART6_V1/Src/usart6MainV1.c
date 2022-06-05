/*
 * I2CMPU6050.c
 *
 *  Created on: 4/05/2022
 *  Author: Juan Pablo
 *
 *	BLANCO = RX
 *	VERDE  = TX
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
#include "rtcDS1307Driver.h"

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

/* Configuración para el RTC */
rtc_t			dateAndTimeRTC = {0};
uint8_t 			dataRTC[7];
getTime_t		dataRTCtest = {0};

// Variables auxiliar.
char 		bufferData[64] = "¡Hola! Soy el USART del STM32 y estoy funcionando.";
uint32_t	counterReception 	= 0;
char		bufferReception[256];
uint8_t		stringComplete		= false;

uint8_t 	rxData				= 0;
uint8_t 	i2cBuffer 			= 0;
uint32_t 	systemTicks 		= 0;
uint32_t 	systemTicksStart 	= 0;
uint32_t 	systemTicksEnd		= 0;
uint8_t		MPU6050IsReady		= false;

/* Variables para los commands */

char 			cmd[16];
char 			userMsg[256];
unsigned int 	firstParameter 		= 0;
unsigned int  	secondParameter 	= 0;

#define numberofsensor 	3
int16_t		MPUBuffer[numberofsensor]		= {0};
// *************** // Headers // *************** //

void initSystem(void);
void sensorConfig();
void sensorData(void);
void parseCommands(char *ptrBufferReception);
//void initialMessage(void);

// *************** // MAIN // *************** //
int main(void)
{
	// Inicializamos el sistema
	initSystem();

	//initialMessage();
	writeMsg(&handlerCommTerminal, bufferData);
	writeMsg(&handlerCommTerminal, "\n");
	// Función para inicializar el RTC
	RTC_init(&handlerRTC);

	dateAndTimeRTC.seconds 	= 0;
	dateAndTimeRTC.minutes 	= 15;
	dateAndTimeRTC.hour 	= 3;
	dateAndTimeRTC.weekDay 	= 3;
	dateAndTimeRTC.date		= 11;
	dateAndTimeRTC.month 	= 5;
	dateAndTimeRTC.year		= 7;

	RTC_SetDateTime(&handlerRTC, &dateAndTimeRTC);

	/* Main Loop */
	while(1){


		if(rxData != '\0'){
			// Hacemos un "eco" con el valor que nos llega por el serial
			writeChar(&handlerCommTerminal, rxData);

			//sensorConfig();
			//sensorData();

			// Leemos los valores del los segundos transcurridos
			if(rxData == 's'){
				uint8_t sec = RTC_readByte(&handlerRTC,0x00);
				//uint8_t seconds = (sec & 0b00001111);
				sprintf(bufferData, "Segundos = %d \n",(int) sec);
				writeMsg(&handlerCommTerminal, bufferData);
				rxData = '\0';
			}
			// Leemos los valores del acelerómetro para y
			else if(rxData == 'm'){
				uint8_t min = RTC_readByte(&handlerRTC,0x01);
				//uint8_t minutes = (min & 0b00001111);
				sprintf(bufferData, "Minutos = %d \n",(int) min);
				writeMsg(&handlerCommTerminal, bufferData);
				rxData = '\0';
			}
			else if(rxData == 'f'){
				//RTC_ReadDateTimeFull(&handlerRTC,dataRTC);
				dataRTC[0] = BCDToDec(RTC_readByte(&handlerRTC, 0x01));
				dataRTC[1] = BCDToDec(RTC_readByte(&handlerRTC, 0x02));
				dataRTC[2] = BCDToDec(RTC_readByte(&handlerRTC, 0x03));
				dataRTC[3] = BCDToDec(RTC_readByte(&handlerRTC, 0x04));
				dataRTC[4] = BCDToDec(RTC_readByte(&handlerRTC, 0x05));
				dataRTC[5] = BCDToDec(RTC_readByte(&handlerRTC, 0x06));
				dataRTC[6] = BCDToDec(RTC_readByte(&handlerRTC, 0x07));
				sprintf(bufferReception, "Seconds: %d | Minutes: %d | Hours: %d \n",
				(int) dataRTC[0],(int) dataRTC[1],(int) dataRTC[2]);
				writeMsg(&handlerCommTerminal, bufferReception);
				sprintf(bufferReception, "Date: %d/%d/%d \n",
				(int) dataRTC[4],(int) dataRTC[5],(int) dataRTC[6]);
				writeMsg(&handlerCommTerminal, bufferReception);
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
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber			= PIN_9;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerPinTX.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerPinTX.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode		= AF7;

	GPIO_Config(&handlerPinTX);

	handlerPinRX.pGPIOx									= GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber			= PIN_10;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerPinRX.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerPinRX.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode		= AF7;

	GPIO_Config(&handlerPinRX);

	// Configuración del USART del USB para RX
	handlerCommTerminal.ptrUSARTx							= USART1;
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

void parseCommands(char *ptrBufferReception){

	// Esta funcion de C lee la cadena de caracteres a la que apunta el "ptr" y la divide
	// y almacena en tres elementos diferentes: un string llamado "cmd", y dos números integer
	// llamados "firstParameter" y "SecondParameter".
	// De esta forma, podemos introducir inf	ormación al micro desde el puerto serial.
	sscanf(ptrBufferReception, "%s %u %u %s", cmd, &firstParameter, &secondParameter, userMsg);

	if(strcmp(cmd, "help") == 0){
		writeMsg(&handlerCommTerminal, "Help Menu CMDs:\n");
		writeMsg(&handlerCommTerminal, "1) help				-- Print this menu\n");
		writeMsg(&handlerCommTerminal, "2) dummy #A #B		-- Dummy cmd, #A and #B are uint32_t\n");
		writeMsg(&handlerCommTerminal, "3) usermsg # # msg	-- msg is a string comming from outside\n");
	}
	else if(strcmp(cmd, "dummy") == 0){
		writeMsg(&handlerCommTerminal, "CMD: dummy\n");
		// Cambiando el formato para presentar el número por el puerto serial
		sprintf(bufferData, "number A = %u \n", firstParameter);
		writeMsg(&handlerCommTerminal, bufferData);

		// Cambiando el formato para presentar el número por el puerto serial
		printf(bufferData, "number B = %u \n", secondParameter);
				writeMsg(&handlerCommTerminal, bufferData);
	}
	else if (strcmp(cmd, "usermsg") == 0){
		writeMsg(&handlerCommTerminal, "CMD: usermsg\n");
		writeMsg(&handlerCommTerminal, userMsg);
		writeMsg(&handlerCommTerminal, "\n");
	}
	else{
		// Se imprime el mensaje "Wrong CMD" si la escritura no corresponde a los CMD implementados.
		writeMsg(&handlerCommTerminal, "Wrong CMD");
	}
}

void initialMessage(void){
	writeMsg(&handlerCommTerminal,"// ***** // BIENVENIDO A LA NERIONETA // ***** // \n");
	writeMsg(&handlerCommTerminal,"─▌█──║─║╔═║─║─╔═╗─ \n");
	writeMsg(&handlerCommTerminal,"─███─╠═╣╠═║─║─║─║─ \n");
	writeMsg(&handlerCommTerminal,"─▐█▐─║─║╚═╚═╚═╚═╝─ \n");
	writeMsg(&handlerCommTerminal,"─▐▐─────────────── \n");
	writeMsg(&handlerCommTerminal,"─▐▐─────────────── \n");
	writeMsg(&handlerCommTerminal, "\n");
	writeMsg(&handlerCommTerminal, bufferData);
	writeMsg(&handlerCommTerminal, "\n");
	writeMsg(&handlerCommTerminal, "Escriba 'help @', para conocer el menú de ayuda. \n");
}
//***********// CallBacks //***********//

void USART1_Callback(void){
	// Lectura de los datos recibidos
	rxData = returnData();
}
// Timer encargado del StateLED

void Timer2_Callback(void){
	handlerStateLED.pGPIOx -> ODR ^= GPIO_ODR_OD5;		// Encendido y apagado StateLED
}
