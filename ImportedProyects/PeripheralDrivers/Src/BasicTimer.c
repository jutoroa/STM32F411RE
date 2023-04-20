/*
 * BasicTimer.c
 *
 *  Created on: Apr 18, 2022
 *      Author: namontoy
 */

#include "BasicTimer.h"

/* Variable que guarda la referencia del periférico que se esta utilizando*/
TIM_TypeDef	*ptrTimerUsed;

/* Función en la que cargamos la configuración del Timer
 * Recordar que siempre se debe comenzar con activar la señal de reloj
 * del periférico que se está utilizando.
 * Además, en este caso, debemos ser cuidadosos al momento de utilizar las interrupciones.
 * Los Timer están conectados directamente al elemento NVIC del Cortex-Mx
 * Debemos configurar y/o utilizar:
 *  - TIMx_CR1  (control Register 1)
 *  - TIMx_SMCR ( slave mode control register) -> mantener en 0 para modo Timer Básico
 *  - TIMx_DIER (DMA and Interrupt enable register)
 *  - TIMx_SR (Status register)
 *  - TIMx_CNT (Counter)
 *  - TIMx_PSC (Pre-scaler)
 *  - TIMx_ARR  (Auto-reload register)
 *
 *  Como vamos a trabajar con interrupciones, antes de configurar una nueva, debemos desactivar
 *  el sistema global de interrupciones, activar la IRQ específica y luego volver a encender
 *  el sistema.
 */
void BasicTimer_Config(BasicTimer_Handler_t *ptrBTimerHandler){
	// Guardamos una referencia al periferico que estamos utilizando...
	ptrTimerUsed = ptrBTimerHandler->ptrTIMx;

	/* 0. Desactivamos las interrupciones globales mientras configuramos el sistema.*/
	__disable_irq();

	/* 1. Activar la señal de reloj del periférico requerido */
	if(ptrBTimerHandler->ptrTIMx == TIM2){
		RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	}
	else if(ptrBTimerHandler->ptrTIMx == TIM3){
		RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	}
	else if(ptrBTimerHandler->ptrTIMx == TIM4){
		RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	}
	else if(ptrBTimerHandler->ptrTIMx == TIM5){
			RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
	}
	else{
		__NOP();
	}

	/* 2. Configuramos el Pre-scaler*/
	ptrBTimerHandler->ptrTIMx->PSC = ptrBTimerHandler->TIMx_Config.TIMx_speed - 1; // (16000000 / 1600) = 10000

	/* 4a. Configuramos la dirección del counter (up/down)*/
	if(ptrBTimerHandler->TIMx_Config.TIMx_mode == BTIMER_MODE_UP){

		/* 2a. Estamos en UP_Mode, el limite se carga en ARR y se comienza en 0 */
		ptrBTimerHandler->ptrTIMx->CR1 &= ~TIM_CR1_DIR;

		/* 3. Configuramos el Auto-reload. Este es el "limite" hasta donde el CNT va a contar */
		ptrBTimerHandler->ptrTIMx->ARR = ptrBTimerHandler->TIMx_Config.TIMx_period - 1;

		/* 4. Reiniciamos el registro counter*/
		ptrBTimerHandler->ptrTIMx->CNT = 0;

	}else{
		/* 2b. Estamos en DOWN_Mode, el limite se carga en ARR (0) y se comienza en un valor alto */
		ptrBTimerHandler->ptrTIMx->CR1 |= TIM_CR1_DIR;

		/* 3. Configuramos el Auto-reload. Este es el "limite" hasta donde el CNT va a contar */
		ptrBTimerHandler->ptrTIMx->ARR = ptrBTimerHandler->TIMx_Config.TIMx_period - 1;

		/* 4. Reiniciamos el registro counter*/
		ptrBTimerHandler->ptrTIMx->CNT = ptrBTimerHandler->TIMx_Config.TIMx_period - 1;
	}

	/* 5. Verificamos si las interrupciones estan activas y las configuramos */
	if (ptrBTimerHandler->TIMx_Config.TIMx_interruptEnable
			== BTIMER_INTERRUP_ENABLE) {

		/* 6. Activamos la interrupción debida al Timerx Utilizado*/
		ptrBTimerHandler->ptrTIMx->DIER |= TIM_DIER_UIE;

		/* 7. Activamos el canal del sistema NVIC para que lea la interrupción*/
		if (ptrBTimerHandler->ptrTIMx == TIM2) {
			// Activando en NVIC para la interrupción del TIM2
			__NVIC_EnableIRQ(TIM2_IRQn);
			__NVIC_SetPriority(TIM2_IRQn, 5);

		} else if (ptrBTimerHandler->ptrTIMx == TIM3) {
			// Activando en NVIC para la interrupción del TIM3
			__NVIC_EnableIRQ(TIM3_IRQn);
			__NVIC_SetPriority(TIM3_IRQn, 5);

		} else if (ptrBTimerHandler->ptrTIMx == TIM4) {
			// Activando en NVIC para la interrupción del TIM3
			__NVIC_EnableIRQ(TIM4_IRQn);
			__NVIC_SetPriority(TIM4_IRQn, 5);

		} else if (ptrBTimerHandler->ptrTIMx == TIM5) {
			// Activando en NVIC para la interrupción del TIM3
			__NVIC_EnableIRQ(TIM5_IRQn);
			__NVIC_SetPriority(TIM5_IRQn, 5);

		}
		else {
			__NOP();
		}
	}

	/* x. Volvemos a activar las interrupciones del sistema */
	__enable_irq();

	/* 5. Activamos el Timer (el CNT debe comenzar a contar*/
	ptrBTimerHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;
}

/**/
void startTimer(BasicTimer_Handler_t *ptrBTimerHandler){
	/* 5. Activamos el Timer (el CNT debe comenzar a contar*/
	ptrBTimerHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;
}

/**/
void stopTimer(BasicTimer_Handler_t *ptrBTimerHandler){
	/* 5. Activamos el Timer (el CNT debe comenzar a contar*/
	ptrBTimerHandler->ptrTIMx->CR1 &= ~TIM_CR1_CEN;
}



/**/
__attribute__((weak)) void BasicTimer2_Callback(void){
	  /* NOTE : This function should not be modified, when the callback is needed,
	            the BasicTimer_Callback could be implemented in the main file
	   */
	__NOP();
}

/**/
__attribute__((weak)) void BasicTimer3_Callback(void){
	  /* NOTE : This function should not be modified, when the callback is needed,
	            the BasicTimer_Callback could be implemented in the main file
	   */
	__NOP();
}

/**/
__attribute__((weak)) void BasicTimer4_Callback(void){
	  /* NOTE : This function should not be modified, when the callback is needed,
	            the BasicTimer_Callback could be implemented in the main file
	   */
	__NOP();
}

/**/
__attribute__((weak)) void BasicTimer5_Callback(void){
	  /* NOTE : This function should not be modified, when the callback is needed,
	            the BasicTimer_Callback could be implemented in the main file
	   */
	__NOP();
}


/* Esta es la función a la que apunta el sistema en el vector de interrupciones.
 * Se debe utilizar usando exactamente el mismo nombre definido en el vector de interrupciones,
 * Al hacerlo correctamente, el sistema apunta a esta función y cuando la interrupción se lanza
 * el sistema inmediatamente salta a este lugar en la memoria*/
void TIM2_IRQHandler(void){
	/* Limpiamos la bandera que indica que la interrupción se ha generado */
	TIM2->SR &= ~TIM_SR_UIF;

	/* LLamamos a la función que se debe encargar de hacer algo con esta interrupción*/
	BasicTimer2_Callback();

}

/* Esta es la función a la que apunta el sistema en el vector de interrupciones.
 * Se debe utilizar usando exactamente el mismo nombre definido en el vector de interrupciones,
 * Al hacerlo correctamente, el sistema apunta a esta función y cuando la interrupción se lanza
 * el sistema inmediatamente salta a este lugar en la memoria*/
void TIM3_IRQHandler(void){
	/* Limpiamos la bandera que indica que la interrupción se ha generado */
	TIM3->SR &= ~TIM_SR_UIF;

	/* LLamamos a la función que se debe encargar de hacer algo con esta interrupción*/
	BasicTimer3_Callback();

}

/* Esta es la función a la que apunta el sistema en el vector de interrupciones.
 * Se debe utilizar usando exactamente el mismo nombre definido en el vector de interrupciones,
 * Al hacerlo correctamente, el sistema apunta a esta función y cuando la interrupción se lanza
 * el sistema inmediatamente salta a este lugar en la memoria*/
void TIM4_IRQHandler(void){
	/* Limpiamos la bandera que indica que la interrupción se ha generado */
	TIM4->SR &= ~TIM_SR_UIF;

	/* LLamamos a la función que se debe encargar de hacer algo con esta interrupción*/
	BasicTimer4_Callback();

}


/* Esta es la función a la que apunta el sistema en el vector de interrupciones.
 * Se debe utilizar usando exactamente el mismo nombre definido en el vector de interrupciones,
 * Al hacerlo correctamente, el sistema apunta a esta función y cuando la interrupción se lanza
 * el sistema inmediatamente salta a este lugar en la memoria*/
void TIM5_IRQHandler(void){
	/* Limpiamos la bandera que indica que la interrupción se ha generado */
	TIM5->SR &= ~TIM_SR_UIF;

	/* LLamamos a la función que se debe encargar de hacer algo con esta interrupción*/
	BasicTimer5_Callback();

}
