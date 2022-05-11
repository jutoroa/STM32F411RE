/*
 * Punto2.c
 *
 *  Created on: 5/04/2022
 *      Author: if420_01
 */
/*
 * Punto1.c
 *
 *  Created on: 5/04/2022
 *      Author: if420_01
 */
/*
Con este programa se desea mostrar el uso básico de los registros que controlan
al micro (SFR) y la forma adecuada para utilizar los operadores &, |, ~ y =, para
cambiar la configuración de algún registro.
También es importante para entender la utilidad de los números BIN y HEX

Es necesario tener a la mano tanto el manual de referencia del micro como la hoja de
datos del micro (ambos son documentos diferentes y complementarios el uno del otro).

HAL -> Hardware Abstraction Layer.

Este programa introduce el periférico más simple que tiene el micro, que es el
encargado de manejar los pines de cada puerto del micro.

Cada PINx de cada puerto GPIO puede desarrollar funciones básicas de tipo entrada
y salida de datos digitales (o sea 1s y 0s), además se les puede asignar funciones
especiales que generalmente están ligadas a otro periférico adicional (se verá más
adelante).

De igual forma, varias características pueden ser configuradas para cada PINx
específico como son:
- La velocidad a la que puede responder
- Tipo de entrada (Pull up, pull down, open drain (flotante))
- Tipo de salida (push-pull, open drain...)
- Entrada análoga

Debemos definir entonces todos los registros que manejan el periférico GPIOx y luego
crear algunas funciones para utilizar adecuadamente el equipo.

********************************************************************************
*/

#include <stdint.h>

#include "stm32f411xx_hal.h"
#include "GPIOxDriver.h"

/* ------ HEADERS ------- */
void delay(uint32_t time);

/* ------ Punto 2 ------- */
// pegar el enunciado del punto 2 <<<<<<<<<<<<<<<

// Acá se ejecuta la función principal
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

	// Definimos el handler para el PIN relacionado con el user button
	GPIO_Handler_t handlerUserButtonPin = {0};

	handlerUserLedPin.pGPIOx = GPIOC;												// Puerto a emplear: GPIOC
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinNumber			= PIN_13;				// Pin a emplear: PC13
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;			// Señal de entrada
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUserLedPin.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;					// Ninguna función

	// Cargamos la configuración del PIN_A5 (User LED)
	GPIO_Config(&handlerUserLedPin);

	// Cargamos la configuración del PIN_C13 (User button)
	GPIO_Config(&handlerUserButtonPin);


	// Este es el ciclo principal, donde se ejecuta todo el programa
	while(1){

		if (GPIO_ReadPin(&handlerUserLedPin) == SET){		// Condición: El botón NO está presionado

			GPIO_WritePin(&handlerUserLedPin, SET);		// Hacemos que el PIN_A5 quede encendido
			delay(6700000);									// Se define un intervalo de tiempo de XX segundos <<<<<<<<<<<<<<<<

			GPIO_WritePin(&handlerUserLedPin, CLEAR);		// Hacemos que el PIN_A5 quede apagado
			delay(6700000);									// Se define un intervalo de tiempo de XX segundos <<<<<<<<<<<<<<<<
		}

		else {												// Condición: El botón SÍ está presionado

			GPIO_WritePin(&handlerUserLedPin, SET);		// Hacemos que el PIN_A5 quede encendido
			delay(6700000/2);								// Se define un intervalo de tiempo de XX segundos <<<<<<<<<<<<<<<<

			GPIO_WritePin(&handlerUserLedPin, CLEAR);		// Hacemos que el PIN_A5 quede apagado
			delay(6700000/2);								// Se define un intervalo de tiempo de XX segundos <<<<<<<<<<<<<<<<
		}
	}
}

/* ----- Definición de funciones ----- */

void delay(uint32_t time){

	// Con este ciclo se genera un intervalo de tiempo en el programa (no tiene otro objetivo)
	for (uint32_t i = 0; i <= time; i++);
}


