 /*
 * DriverNemaV02.c
 *
 *  Created on: 12/04/2022
 *      Author: Juan Pablo
 */


// ************************************** // DriverNemaV02 // ************************************** //

/*Controla el giro del motor en sentido horario y antihorario con dos botones
 */

// ************************************** // DriverNemaV02 // ************************************** //


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

	// Definimos el handler para el PIN relacionado con el user button
	GPIO_Handler_t handlerUserButtonPin = {0};

	handlerUserButtonPin.pGPIOx 									= GPIOC;					// Puerto a emplear: GPIOC
	handlerUserButtonPin.GPIO_PinConfig.GPIO_PinNumber				= PIN_13;					// Pin a emplear: PC13
	handlerUserButtonPin.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_IN;				// Señal de entrada
	handlerUserButtonPin.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerUserButtonPin.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerUserButtonPin.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_MEDIUM;
	handlerUserButtonPin.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;						// Ninguna función


	// Definimos el handler para el PIN relacionado con el boton 1
	GPIO_Handler_t handlerExternalButton1Pin = {0};

	handlerExternalButton1Pin.pGPIOx 								= GPIOB;					// Puerto a emplear: GPIOC
	handlerExternalButton1Pin.GPIO_PinConfig.GPIO_PinNumber			= PIN_8;					// Pin a emplear: PC8
	handlerExternalButton1Pin.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;				// Señal de entrada
	handlerExternalButton1Pin.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerExternalButton1Pin.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;		// Configuración de resistencia Pull up
	handlerExternalButton1Pin.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerExternalButton1Pin.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;						// Ninguna función

	// Definimos el handler para el PIN relacionado con el boton 2
	GPIO_Handler_t handlerExternalButton2Pin = {0};

	handlerExternalButton2Pin.pGPIOx 								= GPIOC;					// Puerto a emplear: GPIOC
	handlerExternalButton2Pin.GPIO_PinConfig.GPIO_PinNumber			= PIN_6;					// Pin a emplear: PC6
	handlerExternalButton2Pin.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;				// Señal de entrada
	handlerExternalButton2Pin.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerExternalButton2Pin.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;		// Configuración de resistencia Pull up
	handlerExternalButton2Pin.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerExternalButton2Pin.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;						// Ninguna función

	// Definimos el handler para la configuración del STEP del motor1
	GPIO_Handler_t handlerStep1Config = {0};

	handlerStep1Config.pGPIOx 									= GPIOC;					// Puerto a emplear: GPIOB
	handlerStep1Config.GPIO_PinConfig.GPIO_PinNumber			= PIN_9;					// Pin a emplear: PB9
	handlerStep1Config.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;			// Señal de salida
	handlerStep1Config.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerStep1Config.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_PULLUP;
	handlerStep1Config.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_MEDIUM;
	handlerStep1Config.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;						// Ninguna función


	// Definimos el handler para la dirección del motor1
	GPIO_Handler_t handlerDir1Config = {0};

	handlerDir1Config.pGPIOx 									= GPIOC;					// Puerto a emplear: GPIOB
	handlerDir1Config.GPIO_PinConfig.GPIO_PinNumber				= PIN_8;					// Pin a emplear: PB8
	handlerDir1Config.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;			// Señal de salida
	handlerDir1Config.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerDir1Config.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_PULLUP;
	handlerDir1Config.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_MEDIUM;
	handlerDir1Config.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;						// Ninguna función


	// Definimos el handler para la configuración del STEP del motor2
	GPIO_Handler_t handlerStep2Config = {0};

	handlerStep2Config.pGPIOx 									= GPIOA;					// Puerto a emplear: GPIOA
	handlerStep2Config.GPIO_PinConfig.GPIO_PinNumber			= PIN_6;					// Pin a emplear: PA6
	handlerStep2Config.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;			// Señal de salida
	handlerStep2Config.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerStep2Config.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_PULLUP;
	handlerStep2Config.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_MEDIUM;
	handlerStep2Config.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;						// Ninguna función


	// Definimos el handler para la dirección del motor2
	GPIO_Handler_t handlerDir2Config = {0};

	handlerDir2Config.pGPIOx 									= GPIOA;					// Puerto a emplear: GPIOA
	handlerDir2Config.GPIO_PinConfig.GPIO_PinNumber				= PIN_5;					// Pin a emplear: PA5
	handlerDir2Config.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;			// Señal de salida
	handlerDir2Config.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerDir2Config.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_PULLUP;
	handlerDir2Config.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_MEDIUM;
	handlerDir2Config.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;						// Ninguna función


	// Cargamos la configuración del PIN_A5  (User LED)
	GPIO_Config(&handlerUserLedPin);

	// Cargamos la configuración del PIN_C13 (User button)
	GPIO_Config(&handlerUserButtonPin);

	// Cargamos la configuración del PIN_C6   button 1
	GPIO_Config(&handlerExternalButton1Pin);

	// Cargamos la configuración del PIN_C6   button 2
	GPIO_Config(&handlerExternalButton2Pin);

	// Cargamos la configuración del PIN_B9 (Step1)
	GPIO_Config(&handlerStep1Config);

	// Cargamos la configuración del PIN_B8  (DIR1)
	GPIO_Config(&handlerDir1Config);

	// Cargamos la configuración del PIN_A6 (Step2)
	GPIO_Config(&handlerStep2Config);

	// Cargamos la configuración del PIN_A5  (DIR2)
	GPIO_Config(&handlerDir2Config);

	GPIO_WritePin(&handlerUserLedPin, SET);

	// Este es el ciclo principal, donde se ejecuta todo el programa
	while(1){

		uint32_t pasos = 1
				;								//Variable que define la cantidad de pasos
		uint32_t delayTime = 5000;							//Tiempo de delay entre paso y paso del motor


//************// Inicio Motor 1 //************//

		//Configuración para el motor 1 (No se presiona el User Button)
		if((GPIO_ReadPin(&handlerUserButtonPin) == SET)){

			// Condición: Pulsador de Giro en sentido ++
			if((GPIO_ReadPin(&handlerExternalButton1Pin) == CLEAR) & (GPIO_ReadPin(&handlerExternalButton2Pin) == SET)){

				GPIO_WritePin(&handlerDir1Config, CLEAR);			//Rotación en sentido ++
				for(uint32_t i = 0; i < pasos; i++){
					GPIO_WritePin(&handlerStep1Config, SET);		//Enciende el motor
					delay(delayTime);								//Tiempo de espera entre paso y paso

					GPIO_WritePin(&handlerStep1Config, CLEAR);		//Apaga el motor
					delay(delayTime);								//Tiempo de espera entre paso y paso
				}
			}

			// Condición: Pulsador de Giro en sentido --
			else if((GPIO_ReadPin(&handlerExternalButton1Pin) == SET) & (GPIO_ReadPin(&handlerExternalButton2Pin) == CLEAR )){

				GPIO_WritePin(&handlerDir1Config, SET);				//Rotación en sentido --
				for(uint32_t i = 0; i < pasos; i++){
					GPIO_WritePin(&handlerStep1Config, SET);		//Enciende el motor
					delay(delayTime);								//Tiempo de espera entre paso y paso

					GPIO_WritePin(&handlerStep1Config, CLEAR);		//Apaga el motor
					delay(delayTime);								//Tiempo de espera entre paso y paso

				}
			}

			else{
				delay(20000);
				}
		}

//************// Fin Motor 1 //************//

//************// Inicio Motor 2 //************//

		else if((GPIO_ReadPin(&handlerUserButtonPin) == CLEAR)){
			//*****

			// Condición: Pulsador de Giro en sentido ++
			if((GPIO_ReadPin(&handlerExternalButton1Pin) == CLEAR) & (GPIO_ReadPin(&handlerExternalButton2Pin) == SET)){

				GPIO_WritePin(&handlerDir2Config, CLEAR);			//Rotación en sentido ++
				for(uint32_t i = 0; i < pasos; i++){
					GPIO_WritePin(&handlerStep2Config, SET);		//Enciende el motor
					delay(delayTime);								//Tiempo de espera entre paso y paso

					GPIO_WritePin(&handlerStep2Config, CLEAR);		//Apaga el motor
					delay(delayTime);								//Tiempo de espera entre paso y paso
				}
			}

			// Condición: Pulsador de Giro en sentido --
			else if((GPIO_ReadPin(&handlerExternalButton1Pin) == SET) & (GPIO_ReadPin(&handlerExternalButton2Pin) == CLEAR )){

				GPIO_WritePin(&handlerDir2Config, SET);				//Rotación en sentido --
				for(uint32_t i = 0; i < pasos; i++){
					GPIO_WritePin(&handlerStep2Config, SET);		//Enciende el motor
					delay(delayTime);								//Tiempo de espera entre paso y paso

					GPIO_WritePin(&handlerStep2Config, CLEAR);		//Apaga el motor
					delay(delayTime);								//Tiempo de espera entre paso y paso

				}
			}

			else{
				delay(20000);
				}


			//*****
		}
//************// Fin Motor 1 //************//

	}	//************// Fin ciclo while //************//

}		//************// Fin MAIN //************//

//***********// Definición de Funciones //***********//

void delay(uint32_t time){
	// Con este ciclo se genera un intervalo de tiempo en el programa (no tiene otro objetivo)
	for (uint32_t i = 0; i <= time; i++){
	}
}
