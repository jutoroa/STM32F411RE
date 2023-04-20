/**
 **************************************************************************************************
 * @file           : MainTaller 7.c
 * @authors        : Alejandro Vélez Patrouilleau - alvelezp@unal.edu.co
 * 				   : Juan Pablo Toro Arenas		  - jutoroa@unal.edu.co
 * @brief          : Introduccion al sprintF y a la conversión ADC:
 **************************************************************************************************
 */

#include <stm32f4xx.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "TIMxDriver.h"
#include "USARTxDriver.h"
#include "RTCDriver.h"


// Definicion de los handlers necesarios
GPIO_Handler_t handlerStateLed = {0};
GPIO_Handler_t handlerPinTx		= {0};
GPIO_Handler_t handlerPinRx		= {0};

// Timer encargado del Blinky
TIMER_Handler_t handlerStateTimer 	= {0};

// Utiliza la conexion USB
USART_Handler_t handlerUsart2		= {0};


// Variables USART
uint8_t rxData = 0;
char bufferData[64];

// Variables ADC


/* Definición de prototipos de funciones */
void InitSystem(void);

/**
 * Funcion principal del programa.
 * Esta funcion es el corazón del programa!!
 *
 * */
int main(void) {

	// Inicializamos todos los elementos del sistema
	InitSystem();

	RTC_t dateAndTimeRTC = {0};

	dateAndTimeRTC.seconds 	= 0;
	dateAndTimeRTC.minutes 	= 15;
	dateAndTimeRTC.hour 	= 3;
	dateAndTimeRTC.weekDay 	= RTC_WEEKDAY_TUESDAY;
	dateAndTimeRTC.date		= 6;
	dateAndTimeRTC.month 	= 6;
	dateAndTimeRTC.year		= 22;

	config_RTC(&dateAndTimeRTC);

	/* Loop forever */
	while (1) {

		// El sistema siempre está verificando si el valor de rxData ha cambiado
        // (lo cual sucede en la ISR de la recepcion (RX).
        // Si este valor deja de ser '\0' significa que se recibio un caracter
        // por lo tanto entra en el bloque if para analizar que se recibio
		if(rxData != '\0'){
			// Imprimimos el caracter recibido
            writeChar(&handlerUsart2, rxData);

			// Iniciamos muestreo del ADC
			if(rxData == 'c'){
				// Activamos el TIM4
			// Paramos muestreo del ADC
			}
			else if(rxData == 'p'){
				// Desactivamos el TIM4
			}
			rxData = '\0';
		}

	}

	return 0;
}

/*
 * Funcion encargada de la inicializacion de los elementos del sistema
 */
void InitSystem(void){

	// Configurando el pin para el Led_Blinky
	handlerStateLed.pGPIOx 								= GPIOA;
	handlerStateLed.GPIO_PinConfig.GPIO_PinNumber		= PIN_5;
	handlerStateLed.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerStateLed.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerStateLed.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;
	handlerStateLed.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;

	// Cargamos la configuración del Pin del led de estado
	GPIO_Config(&handlerStateLed);

	// Llevamos el Led a un estado de encendido
	GPIO_WritePin(&handlerStateLed, RESET);

	/* Configurando los pines sobre los que funciona el USART2 (TX) */
	handlerPinTx.pGPIOx 							= GPIOA;
	handlerPinTx.GPIO_PinConfig.GPIO_PinNumber		= PIN_2;
	handlerPinTx.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	handlerPinTx.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerPinTx.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;
	handlerPinTx.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPinTx.GPIO_PinConfig.GPIO_PinAltFunMode	= AF7;
	GPIO_Config(&handlerPinTx);

	/* Configurando los pines sobre los que funciona el USART2 (RX) */
	handlerPinRx.pGPIOx 							= GPIOA;
	handlerPinRx.GPIO_PinConfig.GPIO_PinNumber		= PIN_3;
	handlerPinRx.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	handlerPinRx.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerPinRx.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPinRx.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;
	handlerPinRx.GPIO_PinConfig.GPIO_PinAltFunMode	= AF7;
	GPIO_Config(&handlerPinRx);

	// Configurando la comunicación serial (Cable verde es TX, Cable Blanco es RX)
	handlerUsart2.ptrUSARTx 					= USART2;
	handlerUsart2.USART_Config.USART_baudrate	= USART_BAUDRATE_115200;
	handlerUsart2.USART_Config.USART_datasize	= USART_DATASIZE_8BIT;
	handlerUsart2.USART_Config.USART_parity		= USART_PARITY_NONE;
	handlerUsart2.USART_Config.USART_stopbits	= USART_STOPBIT_1;
	handlerUsart2.USART_Config.USART_mode		= USART_MODE_RXTX;
	handlerUsart2.USART_Config.USART_interrupt	= USART_INTERRUPT_RX_ENABLE;

	// Cargamos la configuración del USART
	USART_Config(&handlerUsart2);

	// Configurando el Timer2 para que funcione con el blinky
	handlerStateTimer.ptrTIMx 						= TIM2;
	handlerStateTimer.timerConfig.Timer_mode		= TIMER_MODE_UP;
	handlerStateTimer.timerConfig.Timer_speed		= TIMER_INCR_SPEED_1ms;
	handlerStateTimer.timerConfig.Timer_period		= 250;

	// Cargamos la configuración del timer
	Timer_Config(&handlerStateTimer);

	// Activamos el TIM2
	startTimer(&handlerStateTimer);

}


/* Callback del Timer2 - Hacemos un blinky... */
void Timer2_Callback(void){
	handlerStateLed.pGPIOx -> ODR ^= GPIO_ODR_OD5;		// Encendido y apagado StateLED
}

/* Callback relacionado con la recepción del USART2
 * El puerto es leido en la ISR (para bajar la bandera de la interrupción)
 * El caracter que se lee es devuelto por la función getRxData
 */
void USART2_Callback(void){
	// Leemos el valor del registro DR, donde se almacena el dato que llega.
	// Esto además debe bajar la bandera de la interrupción
	rxData = returnData();
}
