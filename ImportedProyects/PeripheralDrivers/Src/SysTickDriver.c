/*
 * SysTickDriver.c
 *
 *  Created on: May 2, 2022
 *      Author: namontoy
 *
 * Este driver controla el Timer que trae por defecto todo procesador ARM Cortex Mx,
 * el cual hace parte del sistema independiente de la empresa fabricaten del MCU.
 *
 * Para encontrar cual es su registro de configuración, debemos utilizar el manual
 * genérico del procesador Cortex-M4, ya que es allí donde se encuentra la documentación
 * para este periférico.
 *
 * En el archivo core_cm4.h, la estructura que define el periférico se llama SysTick_Type
 *
 */

#include "SysTickDriver.h"

#define SYSTICK_LOAD_VALUE_16MHz_1ms		16000	// Número de ciclos en 1ms.
#define SYSTICK_LOAD_VALUE_100MHz_1ms		100000	// Número de ciclos en 1ms.

uint64_t ticks;

/**/
void config_SysTickMs(void){
	// Reiniciamos el valor de la variable que cuenta tiempo
	ticks = 0;

	// Cargando el valor del limite de incrementos que representan 1ms.
	SysTick->LOAD = SYSTICK_LOAD_VALUE_16MHz_1ms;

	// Limpiamos el valor actual del SysTick
	SysTick->VAL = 0;

	// Configuramos el reloj interno como el reloj para el Timer
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;

	//Desactivamos las interrupciones globales
	__disable_irq();

	// Matriculamos la interrupción en el NVIC
	__NVIC_EnableIRQ(SysTick_IRQn);

	// Activamos la interrupción debida al conteo a cero del SysTick
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;

	// Activamos el Timer
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

	// Activamos de nuevo las interrupciones globales
	__enable_irq();



}

uint64_t getTicksMs(void){
	return ticks;
}

/**/
void SysTick_Handler(void){
	// Verificamos que la interrupción se lanzo
	if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk){

		// Limpiamos la bandera
		SysTick->CTRL &= ~SysTick_CTRL_COUNTFLAG_Msk;

		// Incrementamos en 1 el contador.
		ticks++;
	}

}

