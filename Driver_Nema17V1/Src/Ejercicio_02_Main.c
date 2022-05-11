/*
 * Punto2.c
 *
 *  Created on: 6/04/2022
 *      Author: Juan Pablo Toro y Emmanuel Ángel Maya
 */

// ************************************** // PUNTO 2 // ************************************** //

/*Utilice el “User Button” (azul) de forma que si el botón no está pulsado, el LED del MCU hace un Blinky de 400ms para cada estado (ON y OFF)...
 y si está presionado el Blinky pasa a tener la mitad del periodo.
 *Tenga en cuenta la configuración del botón (ver diagrama de conexión en el plano de referencia de la board Nucleo), para que pueda seleccionar...
 adecuadamente que tipo de resistencia (pull-up o pull-down) debe configurar a la entrada del equipo.
 */

// ************************************** // PUNTO 2 // ************************************** //

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
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;		// Pin modo: salida
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;					// Ninguna función

	// Definimos el handler para el PIN relacionado con el user button
	GPIO_Handler_t handlerUserButtonPin = {0};

	handlerUserButtonPin.pGPIOx = GPIOC;												// Puerto a emplear: GPIOC
	handlerUserButtonPin.GPIO_PinConfig.GPIO_PinNumber			= PIN_13;				// Pin a emplear: PC13
	handlerUserButtonPin.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;			// Pin modo: entrada
	handlerUserButtonPin.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserButtonPin.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerUserButtonPin.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUserButtonPin.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;					// Ninguna función

	// Cargamos la configuración del PIN_A5 (User LED)
	GPIO_Config(&handlerUserLedPin);

	// Cargamos la configuración del PIN_C13 (User button)
	GPIO_Config(&handlerUserButtonPin);


	// Este es el ciclo principal, donde se ejecuta todo el programa
	while(1){
		uint32_t periodo = 600000;							//Variable que define la duración del pulso de encendido/apagado.

		if (GPIO_ReadPin(&handlerUserButtonPin) == SET){	// Condición: El botón NO está presionado (Boton normalmente abierto).

			GPIO_WritePin(&handlerUserLedPin, SET);			// Hacemos que el PIN_A5 quede encendido
			delay(periodo);									// Se define un intervalo de tiempo de 412 milisegundos
			GPIO_WritePin(&handlerUserLedPin, CLEAR);		// Hacemos que el PIN_A5 quede apagado
			delay(periodo);									// Se define un intervalo de tiempo de 412 milisegundos
		}

		else {												// Condición: El botón SÍ está presionado.

			GPIO_WritePin(&handlerUserLedPin, SET);			// Hacemos que el PIN_A5 quede encendido
			delay(periodo/2);								// Se define un intervalo de tiempo de 206 milisegundos

			GPIO_WritePin(&handlerUserLedPin, CLEAR);		// Hacemos que el PIN_A5 quede apagado
			delay(periodo/2);								// Se define un intervalo de tiempo de 206 milisegundos
		}
	}
}

//***********// Definición de Funciones //***********//

void delay(uint32_t time){
	// Con este ciclo se genera un intervalo de tiempo en el programa (no tiene otro objetivo)
	for (uint32_t i = 0; i <= time; i++){
	}
}
