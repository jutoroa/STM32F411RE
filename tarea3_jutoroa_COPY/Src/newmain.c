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
GPIO_Handler_t handlerLEDPB9   		= {0};	// LED PB9
GPIO_Handler_t handlerUSB  			= {0};	// Pin USB PA3 RX
TIMER_Handler_t handlerTimer2 		= {0};	// Timer2
TIMER_Handler_t handlerTimer3 		= {0};	// Timer3
USART_Handler_t handlerUsartRX   	= {0};	// Usart RX

uint8_t interruptor = 0;
uint8_t *ptr_interruptor;

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
	GPIO_Config(&handlerLEDPB9);
	GPIO_Config(&handlerUSB);
	Timer_Config(&handlerTimer2);
	Timer_Config(&handlerTimer3);
	USART_Config(&handlerUsartRX);

	// Variable auxiliar para evaluar los casos de 'O'
	dataValue = '0';

    /* Ciclo principal */
	while(1){
		LEDConfig();
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
		TIM3 -> DIER ^= TIM_DIER_UIE;							// Acción cada vez que se oprima el interruptor

		if(!(TIM3 -> DIER & TIM_DIER_UIE)){						// Se oprimió una vez 'O'
			__NVIC_DisableIRQ(TIM3_IRQn);						// Desactivamos las interrupciones
			handlerStateLED.pGPIOx -> ODR &= ~(GPIO_ODR_OD6);	// Forzamos a que el LED quede apagado
			GPIO_WritePin(&handlerLEDPB9, RESET);
		}else{													// Se oprimió dos veces 'O'
			__NVIC_EnableIRQ(TIM3_IRQn);						// Activamos las interrupciones
		}
	}

	if(TIM3 -> DIER & TIM_DIER_UIE){	// Se oprimió dos veces 'O'

		// Se oprimió 'n'
		if(dataValue == 'n'){
			handlerTimer3.timerConfig.Timer_period			= 300;
			Timer_Config(&handlerTimer3);
			dataValue = '0';			// Borramos el valor de dataValue hasta que haya otra interrupción

		// Se oprimió 'f'
		}else if(dataValue == 'f'){
			handlerTimer3.timerConfig.Timer_period			= 200;
			Timer_Config(&handlerTimer3);
			dataValue = '0';			// Borramos el valor de dataValue hasta que haya otra interrupción

		// Se oprimió 'u'
		}else if(dataValue == 'u'){
			handlerTimer3.timerConfig.Timer_period			= 100;
			Timer_Config(&handlerTimer3);
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

	// Configuración del led Pin B9
	handlerLEDPB9.pGPIOx								= GPIOC;
	handlerLEDPB9.GPIO_PinConfig.GPIO_PinNumber			= PIN_6;
	handlerLEDPB9.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerLEDPB9.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerLEDPB9.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerLEDPB9.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;


	// Configuración del pin PA3 USB
	handlerUSB.pGPIOx									= GPIOA;
	handlerUSB.GPIO_PinConfig.GPIO_PinNumber			= PIN_3;
	handlerUSB.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerUSB.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUSB.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerUSB.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_MEDIUM;
	handlerUSB.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF7;

	// Configuración del USART del USB para RX
	handlerUsartRX.ptrUSARTx							= USART2;
	handlerUsartRX.USART_Config.USART_mode				= USART_MODE_RX;
	handlerUsartRX.USART_Config.USART_baudrate			= USART_BAUDRATE_19200;
	handlerUsartRX.USART_Config.USART_datasize			= USART_DATASIZE_8BIT;
	handlerUsartRX.USART_Config.USART_parity			= USART_PARITY_EVEN;
	handlerUsartRX.USART_Config.USART_stopbits			= USART_STOPBIT_1;
	handlerUsartRX.USART_Config.USART_interrupt 		= USART_INTERRUPT_RX_ENABLE;

	// Configuración del timer2
	handlerTimer2.ptrTIMx								= TIM2;
	handlerTimer2.timerConfig.Timer_mode				= TIMER_MODE_UP;
	handlerTimer2.timerConfig.Timer_speed				= TIMER_INCR_SPEED_1ms;
	handlerTimer2.timerConfig.Timer_period				= 250;


	// Configuración del timer3
	handlerTimer3.ptrTIMx								= TIM3;
	handlerTimer3.timerConfig.Timer_mode				= TIMER_MODE_UP;
	handlerTimer3.timerConfig.Timer_speed				= TIMER_INCR_SPEED_1ms;
	handlerTimer3.timerConfig.Timer_period				= 300;

}
//***********// CallBacks //***********//

void Timer2_Callback(void){
	handlerStateLED.pGPIOx -> ODR ^= GPIO_ODR_OD5;		// Encendido y apagado StateLED
}

void Timer3_Callback(void){
	handlerLEDPB9.pGPIOx -> ODR ^= GPIO_ODR_OD6;		// Encendido y apagado LED PB9
}

void USART2_Callback(void){
	// Lectura de los datos recibidos
	dataValue = returnData();
}
