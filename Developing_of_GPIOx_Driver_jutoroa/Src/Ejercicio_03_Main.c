/*
 * Punto3.c
 *
 *  Created on: 6/04/2022
 *      Author: Juan Pablo Toro y Emmanuel Ángel Maya
 */

// ************************************** // PUNTO 3 // ************************************** //

/*Agregue un LED externo conectado al PIN_B9 y un botón externo al PIN_C6 (estos dos nuevos elementos deben extender los resultados del punto 2,
por lo cual debe hacer una copia de este archivo y comenzar desde allí).
	- Haga que el LED externo cambie de forma opuesta al userLED (led verde) si se presiona el userButton (botón azul).
	- Si se presiona el botón externo, solo el LED externo debe hacer el blinky, y el userLED debe permanecer encendido.
	- Si se presionan ambos botones, ambos LEDs deben permanecer apagados.
 */

// ************************************** // PUNTO 3 // ************************************** //


#include <stdint.h>

#include "stm32f411xx_hal.h"
#include "GPIOxDriver.h"

//***********// HEADERS //***********//
void delay(uint32_t time);

//***********// MAIN //***********//
int main(void){

	// Definimos el handler para el PIN relacionado con el user LED
	GPIO_Handler_t handlerUserLedPin = {0};

	handlerUserLedPin.pGPIOx 										= GPIOA;					// Puerto a emplear: GPIOA
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinNumber					= PIN_5;					// Pin a emplear: PA5
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinMode					= GPIO_MODE_OUT;			// Señal de salida
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinOPType					= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinSpeed					= GPIO_OSPEED_MEDIUM;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinAltFunMode				= AF0;						// Ninguna función

	// Definimos el handler para el PIN relacionado con el external LED
	GPIO_Handler_t handlerExternalLedPin = {0};

	handlerExternalLedPin.pGPIOx 									= GPIOB;					// Puerto a emplear: GPIOB
	handlerExternalLedPin.GPIO_PinConfig.GPIO_PinNumber				= PIN_9;					// Pin a emplear: PB9
	handlerExternalLedPin.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;			// Señal de salida
	handlerExternalLedPin.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerExternalLedPin.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerExternalLedPin.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_MEDIUM;
	handlerExternalLedPin.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;						// Ninguna función


	// Definimos el handler para el PIN relacionado con el user button
	GPIO_Handler_t handlerUserButtonPin = {0};

	handlerUserButtonPin.pGPIOx 									= GPIOC;					// Puerto a emplear: GPIOC
	handlerUserButtonPin.GPIO_PinConfig.GPIO_PinNumber				= PIN_13;					// Pin a emplear: PC13
	handlerUserButtonPin.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_IN;				// Señal de entrada
	handlerUserButtonPin.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerUserButtonPin.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerUserButtonPin.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_MEDIUM;
	handlerUserButtonPin.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;						// Ninguna función

	// Definimos el handler para el PIN relacionado con el external button
	GPIO_Handler_t handlerExternalButtonPin = {0};

	handlerExternalButtonPin.pGPIOx 								= GPIOC;					// Puerto a emplear: GPIOC
	handlerExternalButtonPin.GPIO_PinConfig.GPIO_PinNumber			= PIN_6;					// Pin a emplear: PC6
	handlerExternalButtonPin.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;				// Señal de entrada
	handlerExternalButtonPin.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerExternalButtonPin.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_PULLUP;		// Configuración de resistencia Pull up
	handlerExternalButtonPin.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerExternalButtonPin.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;						// Ninguna función


	// Cargamos la configuración del PIN_A5  (User LED)
	GPIO_Config(&handlerUserLedPin);

	// Cargamos la configuración del PIN_C13 (User button)
	GPIO_Config(&handlerUserButtonPin);

	// Cargamos la configuración del PIN_B9  (External LED)
	GPIO_Config(&handlerExternalLedPin);

	// Cargamos la configuración del PIN_C6  (External button)
	GPIO_Config(&handlerExternalButtonPin);


	// Este es el ciclo principal, donde se ejecuta todo el programa
	while(1){

		uint32_t periodo = 600000;							//Variable que define la duración del pulso de encendido/apagado.

		// Condición: Ningún botón está presionado
		if ((GPIO_ReadPin(&handlerUserButtonPin) == SET) & (GPIO_ReadPin(&handlerExternalButtonPin) == SET)){

			// Se encienden y apagan ambos LEDs al tiempo
			GPIO_WritePin(&handlerUserLedPin, SET);			// Hacemos que el PIN_A5 (user LED) quede encendido
			GPIO_WritePin(&handlerExternalLedPin, SET);		// Hacemos que el PIN_B9 (external LED) quede encendido
			delay(periodo);									// Se define un intervalo de tiempo de 412 milisegundos

			GPIO_WritePin(&handlerUserLedPin, CLEAR);		// Hacemos que el PIN_A5 (user LED) quede apagado
			GPIO_WritePin(&handlerExternalLedPin, CLEAR);	// Hacemos que el PIN_B9 (external LED) quede apagado
			delay(periodo);									// Se define un intervalo de tiempo de 412 milisegundos
		}

		// Condición: Solo el user button está presionado
		else if ((GPIO_ReadPin(&handlerUserButtonPin) == CLEAR) & (GPIO_ReadPin(&handlerExternalButtonPin) == SET)){

			// Se encienden y apagan ambos LEDs de forma opuesta
			GPIO_WritePin(&handlerUserLedPin, SET);			// Hacemos que el PIN_A5 (user LED) quede encendido
			GPIO_WritePin(&handlerExternalLedPin, CLEAR);	// Hacemos que el PIN_B9 (external LED) quede apagado
			delay(periodo/2);								// Se define un intervalo de tiempo de 206 milisegundos

			GPIO_WritePin(&handlerUserLedPin, CLEAR);		// Hacemos que el PIN_A5 quede apagado
			GPIO_WritePin(&handlerExternalLedPin, SET);		// Hacemos que el PIN_B9 quede encendido
			delay(periodo/2);								// Se define un intervalo de tiempo de 206 milisegundos
		}

		// Condición: Solo el external button está presionado
		else if ((GPIO_ReadPin(&handlerUserButtonPin) == SET) & (GPIO_ReadPin(&handlerExternalButtonPin) == CLEAR)){

			// Se enciende y apaga solo el LED externo, y el userLED permanece encendido.

			GPIO_WritePin(&handlerUserLedPin, SET);			// Hacemos que el PIN_A5 quede encendido

			GPIO_WritePin(&handlerExternalLedPin, SET);		// Hacemos que el PIN_B9 (external LED) quede encendido
			delay(periodo);									// Se define un intervalo de tiempo de 412 milisegundos

			GPIO_WritePin(&handlerExternalLedPin, CLEAR);	// Hacemos que el PIN_B9 (external LED) quede apagado
			delay(periodo);									// Se define un intervalo de tiempo de 412 milisegundos
		}

		// Condición: Ambos botones están presionados
		else {
			// Se apagan ambos LEDs
			GPIO_WritePin(&handlerUserLedPin, CLEAR);		// Hacemos que el PIN_A5 (user LED) quede apagado
			GPIO_WritePin(&handlerExternalLedPin, CLEAR);	// Hacemos que el PIN_B9 (external LED) quede apagado
		}
	}
}

//***********// Definición de Funciones //***********//

void delay(uint32_t time){
	// Con este ciclo se genera un intervalo de tiempo en el programa (no tiene otro objetivo)
	for (uint32_t i = 0; i <= time; i++){
	}
}

