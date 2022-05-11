/*
 * BasicTimer.c
 *
 *  Created on: 21/04/2022
 *      Author: Juan Pablo Toro.
 */


#include <stm32f4xx.h>

#include "BasicTimer.h"

/*
 * Función que configura un Timer básico para que genere interrupciones de forma
 * periódica.
 *
 * */

//NOTA: Hay que desactivar las interrupciones globales y luego volver a activarlas

void BTimer_Config(BTIMER_Handler_t *ptrTimerConfig){

	/* 0. Desactivamos las interrupciones globales */

	__disable_irq();

	/* 1. Activamos la señal de reloj para el periférico específico */
	if(ptrTimerConfig -> ptrTIMx == TIM2){
		// Activamos la señal de reloj del TIM2
		RCC -> APB1ENR |= RCC_APB1ENR_TIM2EN;
	}

	if(ptrTimerConfig -> ptrTIMx == TIM3){
		// Activamos la señal de reloj del TIM3
		RCC -> APB1ENR |= RCC_APB1ENR_TIM3EN;
	}

	/* 2. Configuración del modo del timer (UP or DOWN) */

	if(ptrTimerConfig -> timerConfig.Timer_mode == TIMER_MODE_UP){
		// Configuramos el timer en modo UP
		ptrTimerConfig -> ptrTIMx -> CR1 &= ~(TIM_CR1_DIR);

		// Ponemos el contador (CNT) en cero.
		ptrTimerConfig -> ptrTIMx -> CNT = 0;

	}
	else{
		// Configuramos el timer en modo DOWN
		ptrTimerConfig -> ptrTIMx -> CR1 |= ~(TIM_CR1_DIR);

		// Ponemos en el contador (CNT) el valor del periodo
		ptrTimerConfig -> ptrTIMx -> CNT = ptrTimerConfig -> timerConfig.Timer_period -1;
	}

	/* 3. Configuración del Prescaler */
	ptrTimerConfig -> ptrTIMx -> PSC = ptrTimerConfig -> timerConfig.Timer_speed -1;

	/* 4. Configuramos el periodo de las interrupciones */
	ptrTimerConfig -> ptrTIMx -> ARR = ptrTimerConfig -> timerConfig.Timer_period -1;

	/* 5. Activamos al timer para que comience a incrementarse  */
	ptrTimerConfig -> ptrTIMx -> CR1 |= TIM_CR1_CEN;

	/* 6. Activamos la interrupción debida a un "update event" */
	ptrTimerConfig -> ptrTIMx -> DIER |= TIM_DIER_UIE;

	/* 7. Activamos la señal de la interrupcuón en el NVIC */

	if(ptrTimerConfig -> ptrTIMx == TIM2){
		// Activamos la IRQ del TIM2
		__NVIC_EnableIRQ(TIM2_IRQn);
	}

	if(ptrTimerConfig -> ptrTIMx == TIM3){
		// Activamos la IRQ del TIM3
		__NVIC_EnableIRQ(TIM3_IRQn);
	}
	/* 8. Activamos de nuevo todas las interrupciones. */
	__enable_irq();
}

__attribute__((weak)) void BTimer_Callback(void){
	__NOP();
}

// Hay que hacer dos cosas: 1) Atender la interrupción. 2) Llamamos al Callback
/* Rutina de atención a la interrupción del TIM2 */
void TIM2_IRQHandler(void){
	TIM2 -> SR &= ~(TIM_SR_UIF);

	BTimer_Callback();
}
