/*
 * I2CMPU6050.c
 *
 *  Created on: 4/05/2022
 *  Author: Juan Pablo
 *
 * // *************** // Examen Final Taller V // *************** //
 */


// *************** // INCLUDES // *************** //

#include <stm32f4xx.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "GPIOxDriver.h"
#include "EXTIxDriver.h"


GPIO_Handler_t 	handlerStateLED 	= {0};	// StateLED
EXTI_Config_t	handlerEXTI0		= {0};	// EXTI0 Botón
uint8_t			LEDState = 0;

int main(void){

	// Configuración el State LED
	handlerStateLED.pGPIOx								= GPIOA;
	handlerStateLED.GPIO_PinConfig.GPIO_PinNumber		= PIN_5;
	handlerStateLED.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerStateLED.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerStateLED.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerStateLED.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_MEDIUM;

	//Cargamos la configuración
	GPIO_Config(&handlerStateLED);

	// Configuración del EXTI0

	handlerEXTI0.port 		= GPIOA;
	handlerEXTI0.pin		= PIN_0;
	handlerEXTI0.trigger	= TRIGGER_FALLING;
	handlerEXTI0.PinControl	= GPIO_PUPDR_PULLUP;

	EXTI_Config(&handlerEXTI0);
}

// Interrupción externa para el botón.
void EXTI0_Callback(void){
	LEDState ^= 1;
	GPIO_WritePin(&handlerStateLED, LEDState);
}
