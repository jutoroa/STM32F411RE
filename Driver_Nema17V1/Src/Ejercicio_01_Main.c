/*
 * Punto1.c
 *
 *  Created on: Apr 6, 2022
 *      Author: Juan Pablo Toro y Emmanuel Ángel Maya
 */

// ************************************** // PUNTO 1 // ************************************** //

/*Utilice el primer ejemplo para crear un blinky (encender y apagar periódicamente un LED). Para tal efecto, solo...
 * puede utilizar como ciclo infinito principal un “while(1)” y dos ciclos for() bastante largos, en los cuales...
 * “no se hace nada” y los cuales solo sirven como “delays”.
 * Garantice que su periodo ON es de 500ms (utilice un osciloscopio para esto) y un periodo de apagado de 350ms.
 * */

// ************************************** // PUNTO 1 // ************************************** //

#include <stdint.h>

#include "stm32f411xx_hal.h"
#include "GPIOxDriver.h"

//***********// HEADERS //***********//
void delay(uint32_t time);

//***********// MAIN //***********//
int main(void){

	// Definimos el handler para el PIN relacionado con el user LED
	GPIO_Handler_t handlerUserLedPin = {0};

	handlerUserLedPin.pGPIOx = GPIOA;												// Puerto a emplear: GPIOA
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinNumber			= PIN_5;				// Pin a emplear: PA5
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;		// Señal de salida
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;					// Ninguna función

	// Este es el ciclo principal, donde se ejecuta todo el programa
	while(1){

		uint32_t periodo = 710000;						//Variable que define la duración del pulso de encendido/apagado.

		GPIO_Config(&handlerUserLedPin);				// Cargamos la configuración del PIN específico

		GPIO_WritePin(&handlerUserLedPin, SET);			// Hacemos que el PIN_A5 quede encendido
		delay(periodo);

		GPIO_WritePin(&handlerUserLedPin, CLEAR);		// Hacemos que el PIN_A5 quede apagado
		delay(500000);
	}
}

//***********// Definición de Funciones //***********//

void delay(uint32_t time){
	// Con este ciclo se genera un intervalo de tiempo en el programa (no tiene otro objetivo)
	for (uint32_t i = 0; i <= time; i++){
	}
}
