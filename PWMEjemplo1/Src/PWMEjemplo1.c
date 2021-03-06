/*
 * mainTarea4.c
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

// *************** // PWM EJEMPLO1 // *************** //

// Configuración para el TIM2

void PWD_config(void){
	// Activamos la señal de reloj del TIM2
	RCC -> APB1ENR |= RCC_APB1ENR_TIM2EN;

	/* 2. Configuración del modo del timer (UP or DOWN) */

	// Configuramos el timer en modo UP
	TIM2 -> CR1 &= ~(TIM_CR1_DIR);

	/* 3. Configuración del Prescaler */
	TIM2 -> PSC =  16000 -1;		// Valor en ms

	/* 4. Configuramos el periodo */
	TIM2 -> ARR = 20;

	/* 5. Activamos al timer para que comience a incrementarse  */
	TIM2 -> CR1 |= TIM_CR1_CEN;

	// Configuramos el duty
	TIM2 -> CCR1 = 1;

	// Ponemos el contador (CNT) en cero.
	TIM2 -> CNT = 0;

	/* 6. Configuramos el OutPut Register CCMR1 y CCMR2 */

	// Escoger el canal como salida
	TIM2 -> CCMR1 &= ~TIM_CCMR1_CC1S;		// Ponemos 00: CC1 channel is configured as output.

	// Escoger si el Preload está activado
	TIM2 -> CCMR1 |= TIM_CCMR1_OC1PE;

	// Configurar el Output compare 1 mode
	TIM2 -> CCMR1 |= (0b110 << TIM_CCMR1_OC1M_Pos);	//(~TIM_CCMR1_OC1M_0)

	/* 7. Activamos el enable a través del CCER */
	TIM2 -> CCER |= TIM_CCER_CC1E;
}

void delay(int time){
	// Con este ciclo se genera un intervalo de tiempo en el programa (no tiene otro objetivo)
	for (int i = 0; i <= time; i++){
		__NOP();
	}
}

GPIO_Handler_t 	handlerStateLED 	= {0};	// StateLED
GPIO_Handler_t	handlerPWDTimer		= {0};
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

	GPIO_WritePin(&handlerStateLED, SET);

	// Configuración del PWD
	handlerPWDTimer.pGPIOx								= GPIOA;
	handlerPWDTimer.GPIO_PinConfig.GPIO_PinNumber		= PIN_0;
	handlerPWDTimer.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerPWDTimer.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerPWDTimer.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPWDTimer.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_MEDIUM;
	handlerPWDTimer.GPIO_PinConfig.GPIO_PinAltFunMode	= AF1;

	//Cargamos la configuración
	GPIO_Config(&handlerPWDTimer);

	PWD_config();

	while(1){
		TIM2 -> CCR1 = 2;

		delay(10000);

		TIM2 -> CCR1 = 1;
		__NOP();
	}
}
