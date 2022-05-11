/*
 * Punto3.c
 *
 *  Created on: 6/04/2022
 *      Author: Juan Pablo Toro
 */

// ************************************** // DriverNemaV01 // ************************************** //

/*Controla el giro del motor en sentido horario y antihorario
 */

// ************************************** // DriverNemaV01 // ************************************** //


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



	// Definimos el handler para la configuración del STEP del motor
	GPIO_Handler_t handlerStepConfig = {0};

	handlerStepConfig.pGPIOx 									= GPIOB;					// Puerto a emplear: GPIOB
	handlerStepConfig.GPIO_PinConfig.GPIO_PinNumber				= PIN_9;					// Pin a emplear: PB9
	handlerStepConfig.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;			// Señal de salida
	handlerStepConfig.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerStepConfig.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_PULLUP;
	handlerStepConfig.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_MEDIUM;
	handlerStepConfig.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;						// Ninguna función


	// Definimos el handler para la dirección del motor
	GPIO_Handler_t handlerDirConfig = {0};

	handlerDirConfig.pGPIOx 									= GPIOB;					// Puerto a emplear: GPIOB
	handlerDirConfig.GPIO_PinConfig.GPIO_PinNumber				= PIN_8;					// Pin a emplear: PB8
	handlerDirConfig.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;			// Señal de salida
	handlerDirConfig.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerDirConfig.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_PULLUP;
	handlerDirConfig.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_MEDIUM;
	handlerDirConfig.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;						// Ninguna función


	// Cargamos la configuración del PIN_A5  (User LED)
	GPIO_Config(&handlerUserLedPin);

	// Cargamos la configuración del PIN_C6 (Step)
	GPIO_Config(&handlerStepConfig);

	// Cargamos la configuración del PIN_B9  (Motor)
	GPIO_Config(&handlerDirConfig);

	GPIO_WritePin(&handlerUserLedPin, SET);

	// Este es el ciclo principal, donde se ejecuta todo el programa
	while(1){

		uint32_t pasos = 200;								//Variable que define la cantidad de pasos
		uint32_t delayTime = 20000;							//Tiempo de delay entre paso y paso del motor

		delay(200000);

		GPIO_WritePin(&handlerDirConfig, CLEAR);				//Rotación en sentido ++
		for(uint32_t i = 0; i < pasos; i++){
			GPIO_WritePin(&handlerStepConfig, SET);			//Enciende el motor
			delay(delayTime);								//Tiempo de espera entre paso y paso

			GPIO_WritePin(&handlerStepConfig, CLEAR);		//Apaga el motor
			delay(delayTime);								//Tiempo de espera entre paso y paso
		}

		delay(200000);

		GPIO_WritePin(&handlerDirConfig, SET);			//Rotación en sentido --
		for(uint32_t i = 0; i < pasos; i++){
			GPIO_WritePin(&handlerStepConfig, SET);			//Enciende el motor
			delay(delayTime);								//Tiempo de espera entre paso y paso

			GPIO_WritePin(&handlerStepConfig, CLEAR);		//Apaga el motor
			delay(delayTime);								//Tiempo de espera entre paso y paso

		}
	}
}

//***********// Definición de Funciones //***********//

void delay(uint32_t time){
	// Con este ciclo se genera un intervalo de tiempo en el programa (no tiene otro objetivo)
	for (uint32_t i = 0; i <= time; i++){
	}
}

