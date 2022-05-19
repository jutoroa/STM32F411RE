/*
 * mainADC.c
 *
 *  Created on: 4/05/2022
 *  Author: Juan Pablo
 */


// *************** // INCLUDES // *************** //
#include <stm32f4xx.h>
#include <stdint.h>
#include <stdbool.h>
#include "GPIOxDriver.h"
#include "USARTxDriver.h"
#include "TIMxDriver.h"
#include "AdcDriver.h"
#include "EXTIxDriver.h"

// *************** // VARIABLES PROYECTO // *************** //

GPIO_Handler_t 	handlerStateLED 	= {0};	// StateLED
TIMER_Handler_t handlerTimer2 		= {0};	// Timer2
EXTI_Config_t	handlerEXTI0		= {0};	// EXTI0


// Variables auxiliar.

uint16_t 	counter			= 0;				// Contador para cada iteración del botón
uint16_t 	auxCounter		= 0;

// *************** // Headers // *************** //

void initSystem(void);

// *************** // MAIN // *************** //
int main(void)
{
		// Inicializamos el sistema
		initSystem();

	    /* Ciclo principal */
		while(1){
				if(counter == 1){
				GPIO_WritePin(&handlerStateLED, RESET);
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
	handlerStateLED.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_MEDIUM;

	//Cargamos la configuración
	GPIO_Config(&handlerStateLED);

	// Configuración del timer2
	handlerTimer2.ptrTIMx								= TIM2;
	handlerTimer2.timerConfig.Timer_mode				= TIMER_MODE_UP;
	handlerTimer2.timerConfig.Timer_speed				= TIMER_INCR_SPEED_1ms;
	handlerTimer2.timerConfig.Timer_period				= 250;

	Timer_Config(&handlerTimer2);

	// Configuración del EXTI0

	handlerEXTI0.port 		= GPIOA;
	handlerEXTI0.pin		= PIN_0;
	handlerEXTI0.trigger	= TRIGGER_FALLING;
	handlerEXTI0.PinControl	= GPIO_PUPDR_PULLUP;

	EXTI_Config(&handlerEXTI0);
}
//***********// CallBacks //***********//
void Timer2_Callback(void){
	handlerStateLED.pGPIOx -> ODR ^= GPIO_ODR_OD5;		// Encendido y apagado StateLED
}

void EXTI0_Callback(void){
	TIM2 -> DIER ^= TIM_DIER_UIE;
	counter ^= 1;
}
