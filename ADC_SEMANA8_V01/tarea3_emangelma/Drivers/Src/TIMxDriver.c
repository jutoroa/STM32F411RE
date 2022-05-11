/*
 ************************************************************************************************************************************************************
 * @file           : TIMxDriver.c
 * @author         : Juan Pablo Toro Arenas y Emmanuel Ángel Maya
 * @brief          : Archivo .c periférico TIMx
 ************************************************************************************************************************************************************
 * En este programa se escribe el código .c necesario para el uso de Timers.
 * Se configuran las características necesarias para trabajar con los registros TIMx:
 * 	- Mode
 * 	- Speed
 * 	- Period
 ************************************************************************************************************************************************************
 */
#include "TIMxDriver.h"

/* Función que configura un Timer básico para que genere interrupciones de forma
 * periódica. */

//NOTA: Hay que desactivar las interrupciones globales y luego volver a activarlas.
void TIM_Config(TIM_Handler_t *ptrTIM_Config){

	/* 0. Se desactivan las interrupciones globales */
	__disable_irq();

	/* 1. Se activa la señal de reloj para el periférico específico */
	if(ptrTIM_Config -> ptrTIMx == TIM2){

		// Se activa la señal de reloj del TIM2.
		RCC -> APB1ENR |= RCC_APB1ENR_TIM2EN;

	}else if(ptrTIM_Config -> ptrTIMx == TIM3){

		// Se activa la señal de reloj del TIM3.
		RCC -> APB1ENR |= RCC_APB1ENR_TIM3EN;

	}else if(ptrTIM_Config -> ptrTIMx == TIM4){

		// Se activa la señal de reloj del TIM4.
		RCC -> APB1ENR |= RCC_APB1ENR_TIM4EN;

	}else if(ptrTIM_Config -> ptrTIMx == TIM5){

		// Se activa la señal de reloj del TIM5.
		RCC -> APB1ENR |= RCC_APB1ENR_TIM5EN;
	}

	/* 2. Configuración del modo del timer (UP or DOWN) */
	if(ptrTIM_Config -> TIM_Config.TIM_mode == TIM_MODE_UP){

		// Configuración del timer en modo UP.
		ptrTIM_Config -> ptrTIMx -> CR1 &= ~(TIM_CR1_DIR);

		// Reseteo del contador (CNT) en cero.
		ptrTIM_Config -> ptrTIMx -> CNT = 0;
	}
	else{

		// Configuración del timer en modo DOWN.
		ptrTIM_Config -> ptrTIMx -> CR1 |= ~(TIM_CR1_DIR);

		// Se pone en el contador (CNT) el valor del periodo.
		ptrTIM_Config -> ptrTIMx -> CNT = ptrTIM_Config -> TIM_Config.TIM_period -1;
	}

	/* 3. Configuración del Prescaler */
	ptrTIM_Config -> ptrTIMx -> PSC = ptrTIM_Config -> TIM_Config.TIM_speed -1;

	/* 4. Configuración del periodo de las interrupciones */
	ptrTIM_Config -> ptrTIMx -> ARR = ptrTIM_Config -> TIM_Config.TIM_period -1;

	/* 5. Activación del timer para que comience a incrementarse */
	ptrTIM_Config -> ptrTIMx -> CR1 |= TIM_CR1_CEN;

	/* 6. Activación de la interrupción debida a un "update event" */
	ptrTIM_Config -> ptrTIMx -> DIER |= TIM_DIER_UIE;

	/* 7. Activación de la señal de la interrupcuón en el NVIC */

	if(ptrTIM_Config -> ptrTIMx == TIM2){

		// Activación de la IRQ del TIM2.
		__NVIC_EnableIRQ(TIM2_IRQn);

	}else if(ptrTIM_Config -> ptrTIMx == TIM3){

		// Activación de la IRQ del TIM3.
		__NVIC_EnableIRQ(TIM3_IRQn);

	}else if(ptrTIM_Config -> ptrTIMx == TIM4){

		// Activación de la IRQ del TIM4.
		__NVIC_EnableIRQ(TIM4_IRQn);

	}else if(ptrTIM_Config -> ptrTIMx == TIM5){

		// Activación de la IRQ del TIM5.
		__NVIC_EnableIRQ(TIM5_IRQn);
	}

	/* 8. Se activan de nuevo todas las interrupciones. */
	__enable_irq();
}

//**********// CallBacks //**********//

// Callback Timer 2.
__attribute__((weak)) void TIM2_Callback(void){
	__NOP();
}

// Callback Timer 3.
__attribute__((weak)) void TIM3_Callback(void){
	__NOP();
}

// Callback Timer 4.
__attribute__((weak)) void TIM4_Callback(void){
	__NOP();
}

// Callback Timer 5.
__attribute__((weak)) void TIM5_Callback(void){
	__NOP();
}
/* Rutina de atención a la interrupción de los TIMx */
// Hay que hacer dos cosas:
// 1) Atender la interrupción.
// 2) Llamada al Callback.

/* Rutina de atención a la interrupción del TIM2 */
void TIM2_IRQHandler(void){
	TIM2 -> SR &= ~(TIM_SR_UIF);
	TIM2_Callback();
}

/* Rutina de atención a la interrupción del TIM3 */
void TIM3_IRQHandler(void){
	TIM3 -> SR &= ~(TIM_SR_UIF);
	TIM3_Callback();
}

/* Rutina de atención a la interrupción del TIM4 */
void TIM4_IRQHandler(void){
	TIM4 -> SR &= ~(TIM_SR_UIF);
	TIM4_Callback();
}

/* Rutina de atención a la interrupción del TIM5 */
void TIM5_IRQHandler(void){
	TIM5 -> SR &= ~(TIM_SR_UIF);
	TIM5_Callback();
}
