/*
 * mainTarea4.c
 *
 *  Created on: 4/05/2022
 *  Author: Juan Pablo
 *
 * Esta tarea pretende implementar un sumador de dos dígitos, en donde cada unidad se aumenta debido a una interrupción
 * del EXTI, la cuál podrá estar dada por un botón, o una fotocompuerta. Los resultados de la suma se pondrán observar
 * en un Display 7 segmentos.
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
GPIO_Handler_t	handlerSwitchLED	= {0};	// Switch LED (LED que prende y apaga con cada interrupción del EXTI).
TIMER_Handler_t handlerTimer2 		= {0};	// Timer2
TIMER_Handler_t handlerTimer4 		= {0};	// Timer4
EXTI_Config_t	handlerEXTI0		= {0};	// EXTI0 Botón
EXTI_Config_t	handlerEXTI3		= {0};	// EXTI2 Fotocompuerta

// Configuración para cada uno de los segmentos //

GPIO_Handler_t	handlerSevenSegmentsA	=	{0};	// Segmento A
GPIO_Handler_t	handlerSevenSegmentsB	=	{0};	// Segmento B
GPIO_Handler_t	handlerSevenSegmentsC	=	{0};	// Segmento C
GPIO_Handler_t	handlerSevenSegmentsD	=	{0};	// Segmento D
GPIO_Handler_t	handlerSevenSegmentsE	=	{0};	// Segmento E
GPIO_Handler_t	handlerSevenSegmentsF	=	{0};	// Segmento F
GPIO_Handler_t	handlerSevenSegmentsG	=	{0};	// Segmento G
GPIO_Handler_t	handlerSevenSegmentsP	=	{0};	// Segmento P

GPIO_Handler_t	handlerTransistorUnidades			=	{0};	// Transistor unidades
GPIO_Handler_t	handlerTransistorDecenas			=	{0};	// Transistor decenas


// Variables auxiliar.

uint32_t 	counterUnidades	= 0;				// Contador para cada iteración del botón - Unidades
uint32_t 	counterDecenas	= 0;				// Contador para cada iteración del botón - Decenas
uint32_t 	auxCounter		= 0;				// Variable auxiliar para el SwitchLED
uint8_t		buttonControl	= 0;				// Variable para controlar el Debounce
uint8_t		LEDState		= 1;				// State del SwitchLED

// *************** // Headers // *************** //

void initSystem(void);
void setNumber(uint32_t number);

// *************** // MAIN // *************** //
int main(void)
{
		// Inicializamos el sistema
		initSystem();

	    /* Ciclo principal */
		while(1){

			// Se prende y se apaga el SwitchLED cada vez que se oprime el botón con una máscara de comprobación.
			if(auxCounter == 1 && buttonControl == 1){
				// Se apaga el SwitchLED
				LEDState ^= 1;
				GPIO_WritePin(&handlerSwitchLED, LEDState);
				// Se reinicia el buttonControl para la máscara de comprobación
				buttonControl = 0;
				// Se aumenta en 1 el contador de unidades
				counterUnidades += 1;

				// Si el contador de unidades llega a 10, se reinician las unidades a 0 y se aumenta el acarreo
				if(counterUnidades > 9){
					counterUnidades = 0;
					counterDecenas += 1;
				}
			// Se prende y se apaga el SwitchLED cada vez que se oprime el botón con una máscara de comprobación.
			}else if(auxCounter == 0 && buttonControl == 1){
				// Se prende el SwitchLED
				LEDState ^= 1;
				GPIO_WritePin(&handlerSwitchLED, LEDState);
				// Se reinicia el buttonControl para la máscara de comprobación
				buttonControl = 0;
				// Se aumenta en 1 el contador de unidades
				counterUnidades += 1;

				// Si el contador de unidades llega a 10, se reinician las unidades a 0 y se aumenta el acarreo
				if(counterUnidades > 9){
					counterUnidades = 0;
					counterDecenas += 1;
				}

			}
			// Si el contador de decenas llega a más de 9 (Número mayor a 99) se reinicia el contador a 00.
			if(counterDecenas > 9 && counterUnidades == 0){
				counterUnidades = 0;
				counterDecenas = 0;
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

	// Configuración el Switch LED
	handlerSwitchLED.pGPIOx								= GPIOA;
	handlerSwitchLED.GPIO_PinConfig.GPIO_PinNumber		= PIN_6;
	handlerSwitchLED.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_OUT;
	handlerSwitchLED.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerSwitchLED.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerSwitchLED.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_MEDIUM;

	//Cargamos la configuración
	GPIO_Config(&handlerSwitchLED);

	// Prendemos el Switch LED
	GPIO_WritePin(&handlerSwitchLED, SET);

	// Configuración del Transistor Unidades
	handlerTransistorUnidades.pGPIOx								= GPIOB;
	handlerTransistorUnidades.GPIO_PinConfig.GPIO_PinNumber			= PIN_5;
	handlerTransistorUnidades.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerTransistorUnidades.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerTransistorUnidades.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerTransistorUnidades.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;

	//Cargamos la configuración
	GPIO_Config(&handlerTransistorUnidades);

	// Configuración del Transistor Decenas
	handlerTransistorDecenas.pGPIOx									= GPIOC;
	handlerTransistorDecenas.GPIO_PinConfig.GPIO_PinNumber			= PIN_5;
	handlerTransistorDecenas.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerTransistorDecenas.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerTransistorDecenas.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerTransistorDecenas.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;

	//Cargamos la configuración
	GPIO_Config(&handlerTransistorDecenas);

	// Desincronizamos los dos display
	handlerTransistorDecenas.pGPIOx -> ODR ^= GPIO_ODR_OD5;

	// Configuración del timer2
	handlerTimer2.ptrTIMx								= TIM2;
	handlerTimer2.timerConfig.Timer_mode				= TIMER_MODE_UP;
	handlerTimer2.timerConfig.Timer_speed				= TIMER_INCR_SPEED_1ms;
	handlerTimer2.timerConfig.Timer_period				= 250;

	Timer_Config(&handlerTimer2);

	// Configuración del timer 4

	handlerTimer4.ptrTIMx								= TIM4;
	handlerTimer4.timerConfig.Timer_mode				= TIMER_MODE_UP;
	handlerTimer4.timerConfig.Timer_speed				= TIMER_INCR_SPEED_1ms;
	handlerTimer4.timerConfig.Timer_period				= 10;

	Timer_Config(&handlerTimer4);

	// Configuración del EXTI0

	handlerEXTI0.port 		= GPIOA;
	handlerEXTI0.pin		= PIN_0;
	handlerEXTI0.trigger	= TRIGGER_FALLING;
	handlerEXTI0.PinControl	= GPIO_PUPDR_PULLUP;

	EXTI_Config(&handlerEXTI0);

	// Configuración del EXTI3

	handlerEXTI3.port 		= GPIOC;
	handlerEXTI3.pin		= PIN_3;
	handlerEXTI3.trigger	= TRIGGER_FALLING;
	handlerEXTI3.PinControl	= GPIO_PUPDR_PULLUP;

	EXTI_Config(&handlerEXTI3);

	// Configuración Segmento A

	handlerSevenSegmentsA.pGPIOx								= GPIOB;
	handlerSevenSegmentsA.GPIO_PinConfig.GPIO_PinNumber			= PIN_4;
	handlerSevenSegmentsA.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerSevenSegmentsA.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerSevenSegmentsA.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerSevenSegmentsA.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;

	//Cargamos la configuración
	GPIO_Config(&handlerSevenSegmentsA);

	// Configuración Segmento B

	handlerSevenSegmentsB.pGPIOx								= GPIOB;
	handlerSevenSegmentsB.GPIO_PinConfig.GPIO_PinNumber			= PIN_3;
	handlerSevenSegmentsB.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerSevenSegmentsB.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerSevenSegmentsB.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerSevenSegmentsB.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;

	//Cargamos la configuración
	GPIO_Config(&handlerSevenSegmentsB);

	// Configuración Segmento C

	handlerSevenSegmentsC.pGPIOx								= GPIOC;
	handlerSevenSegmentsC.GPIO_PinConfig.GPIO_PinNumber			= PIN_2;
	handlerSevenSegmentsC.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerSevenSegmentsC.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerSevenSegmentsC.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerSevenSegmentsC.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;

	//Cargamos la configuración
	GPIO_Config(&handlerSevenSegmentsC);

	// Configuración Segmento D

	handlerSevenSegmentsD.pGPIOx								= GPIOC;
	handlerSevenSegmentsD.GPIO_PinConfig.GPIO_PinNumber			= PIN_13;
	handlerSevenSegmentsD.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerSevenSegmentsD.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerSevenSegmentsD.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerSevenSegmentsD.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;

	//Cargamos la configuración
	GPIO_Config(&handlerSevenSegmentsD);

	// Configuración Segmento E

	handlerSevenSegmentsE.pGPIOx								= GPIOC;
	handlerSevenSegmentsE.GPIO_PinConfig.GPIO_PinNumber			= PIN_1;
	handlerSevenSegmentsE.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerSevenSegmentsE.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerSevenSegmentsE.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerSevenSegmentsE.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;

	//Cargamos la configuración
	GPIO_Config(&handlerSevenSegmentsE);

	// Configuración Segmento F

	handlerSevenSegmentsF.pGPIOx								= GPIOB;
	handlerSevenSegmentsF.GPIO_PinConfig.GPIO_PinNumber			= PIN_2;
	handlerSevenSegmentsF.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerSevenSegmentsF.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerSevenSegmentsF.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerSevenSegmentsF.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;

	//Cargamos la configuración
	GPIO_Config(&handlerSevenSegmentsF);

	// Configuración Segmento G

	handlerSevenSegmentsG.pGPIOx								= GPIOB;
	handlerSevenSegmentsG.GPIO_PinConfig.GPIO_PinNumber			= PIN_1;
	handlerSevenSegmentsG.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerSevenSegmentsG.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerSevenSegmentsG.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerSevenSegmentsG.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;

	//Cargamos la configuración
	GPIO_Config(&handlerSevenSegmentsG);

	//Cargamos la configuración inicial

	GPIO_WritePin(&handlerSevenSegmentsA, SET);
	GPIO_WritePin(&handlerSevenSegmentsB, SET);
	GPIO_WritePin(&handlerSevenSegmentsC, SET);
	GPIO_WritePin(&handlerSevenSegmentsD, SET);
	GPIO_WritePin(&handlerSevenSegmentsE, SET);
	GPIO_WritePin(&handlerSevenSegmentsF, SET);
	GPIO_WritePin(&handlerSevenSegmentsG, SET);
	GPIO_WritePin(&handlerSevenSegmentsP, SET);
}

//***********// setNumber //***********//

// Función que tiene la configuración para cada uno de los números del 0 al 9.

void setNumber(uint32_t number){

	switch(number){
	// Número 0
	case(0):
	{
		GPIO_WritePin(&handlerSevenSegmentsA, RESET);
		GPIO_WritePin(&handlerSevenSegmentsB, RESET);
		GPIO_WritePin(&handlerSevenSegmentsC, RESET);
		GPIO_WritePin(&handlerSevenSegmentsD, RESET);
		GPIO_WritePin(&handlerSevenSegmentsE, RESET);
		GPIO_WritePin(&handlerSevenSegmentsF, RESET);
		GPIO_WritePin(&handlerSevenSegmentsG, SET);
		break;
	}

	// Número 1
	case(1):
	{
		GPIO_WritePin(&handlerSevenSegmentsA, SET);
		GPIO_WritePin(&handlerSevenSegmentsB, RESET);
		GPIO_WritePin(&handlerSevenSegmentsC, RESET);
		GPIO_WritePin(&handlerSevenSegmentsD, SET);
		GPIO_WritePin(&handlerSevenSegmentsE, SET);
		GPIO_WritePin(&handlerSevenSegmentsF, SET);
		GPIO_WritePin(&handlerSevenSegmentsG, SET);
		break;
	}

	// Número 2
	case(2):
	{
		GPIO_WritePin(&handlerSevenSegmentsA, RESET);
		GPIO_WritePin(&handlerSevenSegmentsB, RESET);
		GPIO_WritePin(&handlerSevenSegmentsC, SET);
		GPIO_WritePin(&handlerSevenSegmentsG, RESET);
		GPIO_WritePin(&handlerSevenSegmentsE, RESET);
		GPIO_WritePin(&handlerSevenSegmentsD, RESET);
		GPIO_WritePin(&handlerSevenSegmentsF, SET);
		break;
	}

	// Número 3
	case(3):
	{
		GPIO_WritePin(&handlerSevenSegmentsA, RESET);
		GPIO_WritePin(&handlerSevenSegmentsB, RESET);
		GPIO_WritePin(&handlerSevenSegmentsC, RESET);
		GPIO_WritePin(&handlerSevenSegmentsD, RESET);
		GPIO_WritePin(&handlerSevenSegmentsE, SET);
		GPIO_WritePin(&handlerSevenSegmentsF, SET);
		GPIO_WritePin(&handlerSevenSegmentsG, RESET);
		break;
	}

	// Número 4
	case(4):
	{
		GPIO_WritePin(&handlerSevenSegmentsA, SET);
		GPIO_WritePin(&handlerSevenSegmentsB, RESET);
		GPIO_WritePin(&handlerSevenSegmentsC, RESET);
		GPIO_WritePin(&handlerSevenSegmentsD, SET);
		GPIO_WritePin(&handlerSevenSegmentsE, SET);
		GPIO_WritePin(&handlerSevenSegmentsF, RESET);
		GPIO_WritePin(&handlerSevenSegmentsG, RESET);
		break;
	}

	// Número 5
	case(5):
	{
		GPIO_WritePin(&handlerSevenSegmentsA, RESET);
		GPIO_WritePin(&handlerSevenSegmentsB, SET);
		GPIO_WritePin(&handlerSevenSegmentsC, RESET);
		GPIO_WritePin(&handlerSevenSegmentsD, RESET);
		GPIO_WritePin(&handlerSevenSegmentsE, SET);
		GPIO_WritePin(&handlerSevenSegmentsF, RESET);
		GPIO_WritePin(&handlerSevenSegmentsG, RESET);
		break;
	}

	// Número 6
	case(6):
	{
		GPIO_WritePin(&handlerSevenSegmentsA, RESET);
		GPIO_WritePin(&handlerSevenSegmentsB, SET);
		GPIO_WritePin(&handlerSevenSegmentsC, RESET);
		GPIO_WritePin(&handlerSevenSegmentsD, RESET);
		GPIO_WritePin(&handlerSevenSegmentsE, RESET);
		GPIO_WritePin(&handlerSevenSegmentsF, RESET);
		GPIO_WritePin(&handlerSevenSegmentsG, RESET);
		break;
	}

	// Número 7
	case(7):
	{
		GPIO_WritePin(&handlerSevenSegmentsA, RESET);
		GPIO_WritePin(&handlerSevenSegmentsB, RESET);
		GPIO_WritePin(&handlerSevenSegmentsC, RESET);
		GPIO_WritePin(&handlerSevenSegmentsD, SET);
		GPIO_WritePin(&handlerSevenSegmentsE, SET);
		GPIO_WritePin(&handlerSevenSegmentsF, SET);
		GPIO_WritePin(&handlerSevenSegmentsG, SET);
		break;
	}

	// Número 8
	case(8):
	{
		GPIO_WritePin(&handlerSevenSegmentsA, RESET);
		GPIO_WritePin(&handlerSevenSegmentsB, RESET);
		GPIO_WritePin(&handlerSevenSegmentsC, RESET);
		GPIO_WritePin(&handlerSevenSegmentsD, RESET);
		GPIO_WritePin(&handlerSevenSegmentsE, RESET);
		GPIO_WritePin(&handlerSevenSegmentsF, RESET);
		GPIO_WritePin(&handlerSevenSegmentsG, RESET);
		break;
	}

	// Número 9
	case(9):
	{
		GPIO_WritePin(&handlerSevenSegmentsA, RESET);
		GPIO_WritePin(&handlerSevenSegmentsB, RESET);
		GPIO_WritePin(&handlerSevenSegmentsC, RESET);
		GPIO_WritePin(&handlerSevenSegmentsD, RESET);
		GPIO_WritePin(&handlerSevenSegmentsE, SET);
		GPIO_WritePin(&handlerSevenSegmentsF, RESET);
		GPIO_WritePin(&handlerSevenSegmentsG, RESET);
		break;
	}
	}
}

//***********// CallBacks //***********//

// Timer encargado del StateLED
void Timer2_Callback(void){
	handlerStateLED.pGPIOx -> ODR ^= GPIO_ODR_OD5;		// Encendido y apagado StateLED
}

// Timer encargado de genera la frecuencia de oscilación de los dos transistores para los display
void Timer4_Callback(void){

	handlerTransistorUnidades.pGPIOx -> ODR ^= GPIO_ODR_OD5;	// Switcheamos el estado del Transistor Unidades

	handlerTransistorDecenas.pGPIOx -> ODR ^= GPIO_ODR_OD5;		// Switcheamos el estado del Transistor Decenas


	// Si el transistor Unidades está encendido, se muestra el valor de unidades
	if(((handlerTransistorUnidades.pGPIOx -> ODR) &= GPIO_ODR_OD5) !=  GPIO_ODR_OD5 ){
		setNumber(counterUnidades);
	// Si el transistor Decenas está encendido, se muestra el valor de Decenas
	}else if((handlerTransistorDecenas.pGPIOx -> ODR &= GPIO_ODR_OD5) != GPIO_ODR_OD5){
		setNumber(counterDecenas);
	}
}

// Interrupción externa para el botón.
void EXTI0_Callback(void){
	// Cambiamos el estado del Switch LED cada vez que se genera una interrupción.
	auxCounter ^= 1;
	// Variable para la comprobación del Bounce.
	buttonControl = 1;
}

// Interrupción externa para la fotocompuerta.
void EXTI3_Callback(void){
	// Cambiamos el estado del Switch LED cada vez que se genera una interrupción.
	auxCounter ^= 1;
	// Variable para la comprobación del Bounce.
	buttonControl = 1;
}
