/*
 * SysTickDriver.c
 *
 *  Created on: May 12, 2022
 *      Author: Juan Pablo
 */

#include "SysTickDriver.h"

#define SYSTICK_LOAD_VALUE		16000	// Número de ciclos de 1ms.

uint64_t ticks;

/**/

void config_SysTickMs(void){
	// Reiniciamos el valor de la variable que cuenta tiempo
	ticks = 0;

	//Cargando el valor del limite de incrementos que representan 1ms.
	SysTick -> LOAD = SYSTICK_LOAD_VALUE;

	// Limpiamos el valor actual del SysTick
	SysTick -> VAL = 0;

	// Configuramos el reloj interno como el reloj para el Timer
	SysTick -> CTRL |= SysTick_CTRL_CLKSOURCE_Msk;

	// Desactivamos las interrupciones globales
	__disable_irq();

	// Activamos la interrupción debida al conteo a ceo del SysTick
	SysTick -> CTRL |= SysTick_CTRL_TICKINT_Msk;

	//Matriculamos la interrupción en el NVIC
	__NVIC_EnableIRQ(SysTick_IRQn);

	// Activamos el Timer
	SysTick -> CTRL |= SysTick_CTRL_ENABLE_Msk;

	// Activamos de nuevo las interrupciones globales
	__enable_irq();
}

uint64_t getTicksMs(void){
	return ticks;
}

/**/

void SysTick_Handler(void){

	// Verificamos que la interrupción se lanza
	if(SysTick -> CTRL & SysTick_CTRL_COUNTFLAG_Msk){

		// Limpiamos la bandera
		SysTick -> CTRL &= ~SysTick_CTRL_COUNTFLAG_Msk;

		// Incrementamos en 1 el contador.
		ticks++;
	}
}
