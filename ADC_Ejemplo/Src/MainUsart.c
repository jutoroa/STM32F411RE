/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Juan Pablo Toro Arenas
 * @brief          : Desarrollo de la tarea 3
 ******************************************************************************
 * - Implementación de la programación orientada a interrupciones.
 * - El programa funciona de tal manera que al presionar:
 *	Caracter ‘n’: genera una blinky normal (300 ms).
 *	Caracter ‘f’: genera una blinky “rápida” (200 ms,”fast”).
 *	Caracter ‘u’: genera una blinky “ultra-rapida” (100 ms).
 *	Carácter ‘O’: Activa y desactiva el blinky. Viene de “On-Off”.
 *	Si se pulsa una vez la tecla “O” (Observe que es “O” mayúscula, es una letra y no un cero...
 *	ni tampoco un círculo), el led se queda apagado hasta que se vuelve a presionar “O”, momento en el...
 *	... cual vuelve a tener su ultimo estado blinky configurado.
 *
 *
 ******************************************************************************
 */

// *************** // INCLUDES // *************** //
#include <stm32f4xx.h>
#include <stdint.h>
#include "GPIOxDriver.h"
#include "USARTxDriver.h"
#include "TIMxDriver.h"

// *************** // VARIABLES PROYECTO // *************** //

GPIO_Handler_t handlerStateLED 		= {0};	// StateLED
GPIO_Handler_t handlerUSBTX  		= {0};	// Pin USB PA2 TX
GPIO_Handler_t handlerUSB  			= {0};	// Pin USB PA3 RX
TIMER_Handler_t handlerTimer2 		= {0};	// Timer2
USART_Handler_t handlerUsart2	 	= {0};	// Usart 2

uint8_t 	dataValue = '0';
// *************** // Headers // *************** //

void initSystem(void);
void LEDConfig(void);

// *************** // MAIN // *************** //
int main(void)
{
	// Inicializamos el sistema
	initSystem();

	//Cargamos la configuración
	GPIO_Config(&handlerStateLED);
	GPIO_Config(&handlerUSB);
	GPIO_Config(&handlerUSBTX);
	Timer_Config(&handlerTimer2);
	USART_Config(&handlerUsart2);

	// Variable auxiliar para evaluar los casos de 'O'
	dataValue = '0';

    /* Ciclo principal */
	while(1){
		LEDConfig();
		//writeChar(&handlerUsartTX, ('h'));
	}
}

//***********// *********** // Definición de Funciones // *********** //***********//


//***********// LEDConfig //***********//

// Función que tiene toda la configuración para el LED en el pin PB9

void LEDConfig(void){

	// Comparación de los valores recibidos.
	if(dataValue == 'O'){
		dataValue = '0';
		// Apagamos la interrupción del timer
		TIM2 -> DIER ^= TIM_DIER_UIE;							// Acción cada vez que se oprima el interruptor

		if(!(TIM2 -> DIER & TIM_DIER_UIE)){						// Se oprimió una vez 'O'
			__NVIC_DisableIRQ(TIM2_IRQn);						// Desactivamos las interrupciones
			handlerStateLED.pGPIOx -> ODR &= ~(GPIO_ODR_OD5);	// Forzamos a que el LED quede apagado
			GPIO_WritePin(&handlerStateLED, RESET);
		}else{													// Se oprimió dos veces 'O'
			__NVIC_EnableIRQ(TIM2_IRQn);						// Activamos las interrupciones
		}
	}

	if(TIM2 -> DIER & TIM_DIER_UIE){	// Se oprimió dos veces 'O'

		// Se oprimió 'n'
		if(dataValue == 'n'){
			handlerTimer2.timerConfig.Timer_period			= 300;
			Timer_Config(&handlerTimer2);
			dataValue = '0';			// Borramos el valor de dataValue hasta que haya otra interrupción

		// Se oprimió 'f'
		}else if(dataValue == 'f'){
			handlerTimer2.timerConfig.Timer_period			= 200;
			Timer_Config(&handlerTimer2);
			dataValue = '0';			// Borramos el valor de dataValue hasta que haya otra interrupción

		// Se oprimió 'u'
		}else if(dataValue == 'u'){
			handlerTimer2.timerConfig.Timer_period			= 100;
			Timer_Config(&handlerTimer2);
			dataValue = '0';			// Borramos el valor de dataValue hasta que haya otra interrupción
		}
	}
}

//***********// InitSystem //***********//

// Función que define la configuración de todos los pines y periféricos

void initSystem(void){

	// Configuración el State LED
	handlerStateLED.pGPIOx								= GPIOA;
	handlerStateLED.GPIO_PinConfig.GPIO_PinNumber		= PIN_5;
	handlerStateLED.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerStateLED.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerStateLED.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerStateLED.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_MEDIUM;

	// Configuración del pin PA3 USB RX
	handlerUSB.pGPIOx									= GPIOA;
	handlerUSB.GPIO_PinConfig.GPIO_PinNumber			= PIN_3;
	handlerUSB.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerUSB.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUSB.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerUSB.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_MEDIUM;
	handlerUSB.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF7;

	// Configuración del pin PA2 USB TX
	handlerUSBTX.pGPIOx									= GPIOA;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinNumber			= PIN_2;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF7;

	// Configuración del USART del USB para RXTX
	handlerUsart2.ptrUSARTx								= USART2;
	handlerUsart2.USART_Config.USART_mode				= USART_MODE_RXTX;
	handlerUsart2.USART_Config.USART_baudrate			= USART_BAUDRATE_19200;
	handlerUsart2.USART_Config.USART_datasize			= USART_DATASIZE_8BIT;
	handlerUsart2.USART_Config.USART_parity				= USART_PARITY_ODD;
	handlerUsart2.USART_Config.USART_stopbits			= USART_STOPBIT_1;
	handlerUsart2.USART_Config.USART_interrupt 			= USART_INTERRUPT_RX_ENABLE;

	// Configuración del timer2
	handlerTimer2.ptrTIMx								= TIM2;
	handlerTimer2.timerConfig.Timer_mode				= TIMER_MODE_UP;
	handlerTimer2.timerConfig.Timer_speed				= TIMER_INCR_SPEED_1ms;
	handlerTimer2.timerConfig.Timer_period				= 10;
}
//***********// CallBacks //***********//

void Timer2_Callback(void){
	handlerStateLED.pGPIOx -> ODR ^= GPIO_ODR_OD5;		// Encendido y apagado StateLED
}

void Timer3_Callback(void){
	__NOP();
}

void USART2_Callback(void){
	// Lectura de los datos recibidos
	dataValue = returnData();
	writeChar(&handlerUsart2, dataValue);
}

