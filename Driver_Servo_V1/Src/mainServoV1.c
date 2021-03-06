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
#include "PWMxDriver.h"

// *************** // VARIABLES PROYECTO // *************** //

GPIO_Handler_t 	handlerStateLED 		= {0};	// StateLED
GPIO_Handler_t 	handlerPinTX 			= {0};	// handlerPinTX
GPIO_Handler_t 	handlerPinRX 			= {0};	// handlerPinRX
GPIO_Handler_t	handlerPinServo1		= {0};	// Pin para el servo1

TIMER_Handler_t handlerTimer2 			= {0};	// Timer2
USART_Handler_t handlerCommTerminal		= {0};	// Usart para la terminal
PWM_Handler_t	handlerPWM1				= {0};	// PWM1 para el servo1


// *************** // VARIABLES AUXILIARES // *************** //

char 		bufferData[64] 		= "esto es una pequeña prueba";
uint8_t		rxData 				= 0;
uint8_t 	dutyCounter 		= 5;
uint8_t 	PWM1Buffer 			= 0;

// *************** // Headers // *************** //

void initSystem(void);
void delay(int time);

// *************** // MAIN // *************** //
int main(void)
{
	// Inicializamos el sistema
	initSystem();
	/* Main Loop */
	while(1){

		// Hacemos un "eco" con el valor que nos llega por el serial
		if(rxData != '\0'){
			writeChar(&handlerCommTerminal, rxData);

			if(rxData == 'd'){
				dutyCounter += 1;
				handlerPWM1.timerConfig.pwmDuty = dutyCounter;
				PWM_Config(&handlerPWM1);
				// *****  //
				PWM1Buffer = dutyCounter;
				sprintf(bufferData, "DutyValue= %2x \n", (unsigned int) PWM1Buffer);
				writeMsg(&handlerCommTerminal, bufferData);
				// ***** //
				rxData = '\0';
			}
			else if(rxData == 'a'){
				dutyCounter -= 1;
				handlerPWM1.timerConfig.pwmDuty = dutyCounter;
				PWM_Config(&handlerPWM1);
				// *****  //
				PWM1Buffer = dutyCounter;
				sprintf(bufferData, "DutyValue= %2x \n", (unsigned int) PWM1Buffer);
				writeMsg(&handlerCommTerminal, bufferData);
				// ***** //
				rxData = '\0';
			}
			else if(rxData == 'c'){
				while(1){
					dutyCounter = 1;
					handlerPWM1.timerConfig.pwmDuty = dutyCounter;
					PWM_Config(&handlerPWM1);
					delay(3000000);
					// *****  //
					PWM1Buffer = dutyCounter;
					sprintf(bufferData, "DutyValue= %2x \n", (unsigned int) PWM1Buffer);
					writeMsg(&handlerCommTerminal, bufferData);
					// ***** //
					dutyCounter = 2;
					handlerPWM1.timerConfig.pwmDuty = dutyCounter;
					PWM_Config(&handlerPWM1);
					delay(3000000);
					// *****  //
					PWM1Buffer = dutyCounter;
					sprintf(bufferData, "DutyValue= %2x \n", (unsigned int) PWM1Buffer);
					writeMsg(&handlerCommTerminal, bufferData);
					// ***** //
				}
				rxData = '\0';d
			}
			else if(rxData == 'l'){
				dutyCounter = 1;
				handlerPWM1.timerConfig.pwmDuty = dutyCounter;
				PWM_Config(&handlerPWM1);
				// *****  //
				PWM1Buffer = dutyCounter;
				sprintf(bufferData, "DutyValue= %2x \n", (unsigned int) PWM1Buffer);
				writeMsg(&handlerCommTerminal, bufferData);
				// ***** //
				rxData = '\0';
			}
			else if(rxData == 'j'){
				dutyCounter = 2;
				handlerPWM1.timerConfig.pwmDuty = dutyCounter;
				PWM_Config(&handlerPWM1);
				// *****  //
				PWM1Buffer = dutyCounter;
				sprintf(bufferData, "DutyValue= %2x \n", (unsigned int) PWM1Buffer);
				writeMsg(&handlerCommTerminal, bufferData);
				// ***** //
				rxData = '\0';
			}
			else{
				rxData = '\0';
			}
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

//	// Configuración del timer2
//	handlerTimer2.ptrTIMx								= TIM2;
//	handlerTimer2.timerConfig.Timer_mode				= TIMER_MODE_UP;
//	handlerTimer2.timerConfig.Timer_speed				= TIMER_INCR_SPEED_1ms;
//	handlerTimer2.timerConfig.Timer_period				= 250;

//	Timer_Config(&handlerTimer2);

	GPIO_WritePin(&handlerStateLED, SET);

	// Configuración del PWM
	handlerPinServo1.pGPIOx								= GPIOA;
	handlerPinServo1.GPIO_PinConfig.GPIO_PinNumber		= PIN_0;
	handlerPinServo1.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	handlerPinServo1.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerPinServo1.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPinServo1.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_MEDIUM;
	handlerPinServo1.GPIO_PinConfig.GPIO_PinAltFunMode	= AF1;

	//Cargamos la configuración
	GPIO_Config(&handlerPinServo1);

	handlerPWM1.ptrTIMx								= TIM2;
	handlerPWM1.timerConfig.pwmMode					= TIMER_MODE_UP;
	handlerPWM1.timerConfig.pwmSpeed				= TIMER_INCR_SPEED_1ms;
	handlerPWM1.timerConfig.pwmPeriod				= 20;
	handlerPWM1.timerConfig.pwmDuty					= dutyCounter;
	handlerPWM1.timerConfig.channel					= PWM_CHANNEL_1;

	PWM_Config(&handlerPWM1);
}

//***********// Delay //***********//

void delay(int time){
	// Con este ciclo se genera un intervalo de tiempo en el programa (no tiene otro objetivo)
	for (int i = 0; i <= time; i++){
		__NOP();
	}
}

//***********// CallBacks //***********//

void USART2_Callback(void){
	// Lectura de los datos recibidos
	rxData = returnData();
}
//// Timer encargado del StateLED
//void Timer2_Callback(void){
//	handlerStateLED.pGPIOx -> ODR ^= GPIO_ODR_OD5;		// Encendido y apagado StateLED
//}
