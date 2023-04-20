/*
 * PwmDriver.c
 *
 *  Created on: May 14, 2022
 *      Author: namontoy
 */
#include "CaptureFrecDriver.h"

/**/
void capture_Config(Capture_Handler_t *ptrCaptureHandler){

	/* 1. Activar la señal de reloj del periférico requerido */
	if(ptrCaptureHandler->ptrTIMx == TIM2){
		RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	}
	else if(ptrCaptureHandler->ptrTIMx == TIM3){
		RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	}
	else if(ptrCaptureHandler->ptrTIMx == TIM4){
		RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	}
	else if(ptrCaptureHandler->ptrTIMx == TIM5){
		RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
	}
	else{
		__NOP();
	}

	/* 2a. Estamos en UP_Mode, el limite se carga en ARR y se comienza en 0 */
	ptrCaptureHandler->ptrTIMx->CR1 &= ~TIM_CR1_DIR;

	/* 3. Configuramos los bits CCxS del registro TIMy_CCMR1, de forma que sea modo salida
	 * (para cada canal hay un conjunto CCxS)
	 *
	 * 4. Además, en el mismo "case" podemos configurar el modo del PWM, su polaridad...
	 *
	 * 5. Y además activamos el preload bit, para que cada vez que exista un update-event
	 * el valor cargado en el CCRx será recargado en el registro "shadow" del PWM */
	switch(ptrCaptureHandler->config.channel){
	case CAPTURE_CHANNEL_1:{
		// Borramos el posible valor cargado
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(TIM_CCMR1_CC1S);
		// Configuramos como entrada en el mismo canal
		ptrCaptureHandler->ptrTIMx->CCMR1 |= (1 << TIM_CCMR1_CC1S_Pos);

		// Configuramos el filtro (filtro 8N)
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(0xF << TIM_CCMR1_IC1F_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR1 |= (0x3 << TIM_CCMR1_IC1F_Pos);

		// Configuramos el preescaler
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(0x3 << TIM_CCMR1_IC1PSC_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR1 |= (ptrCaptureHandler->config.prescalerCapture << TIM_CCMR1_IC1PSC_Pos);

		// Configuramos el flanco que deseamos capturar
		if(ptrCaptureHandler->config.edgeSignal == CAPTURE_RISING_EDGE){
			// Configuracion 00, para rising edge
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC1P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC1NP);
		}
		else{
			// Configuracion 01, para falling edge
			ptrCaptureHandler->ptrTIMx->CCER |= (TIM_CCER_CC1P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC1NP);
		}

		// Activamos el modulo de captura
		ptrCaptureHandler->ptrTIMx->CCER |= TIM_CCER_CC1E;

		break;
	}

	case CAPTURE_CHANNEL_2:{
		// Borramos el posible valor cargado
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(TIM_CCMR1_CC2S);
		// Configuramos como entrada en el mismo canal
		ptrCaptureHandler->ptrTIMx->CCMR1 |= (1 << TIM_CCMR1_CC2S_Pos);

		// Configuramos el filtro (sin filtro)
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(0xF << TIM_CCMR1_IC2F_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR1 |= (0x3 << TIM_CCMR1_IC2F_Pos);

		// Configuramos el preescaler
		ptrCaptureHandler->ptrTIMx->CCMR1 &= ~(0x3 << TIM_CCMR1_IC2PSC_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR1 |= (ptrCaptureHandler->config.prescalerCapture << TIM_CCMR1_IC2PSC_Pos);

		// Configuramos el flanco que deseamos capturar
		if (ptrCaptureHandler->config.edgeSignal == CAPTURE_RISING_EDGE) {
			// Configuracion 00, para rising edge
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC2P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC2NP);
		} else {
			// Configuracion 01, para falling edge
			ptrCaptureHandler->ptrTIMx->CCER |= (TIM_CCER_CC2P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC2NP);
		}

		// Activamos el modulo de captura
		ptrCaptureHandler->ptrTIMx->CCER |= TIM_CCER_CC2E;

		break;
	}

	case CAPTURE_CHANNEL_3:{
		// Borramos el posible valor cargado
		ptrCaptureHandler->ptrTIMx->CCMR2 &= ~(TIM_CCMR2_CC3S);
		// Configuramos como entrada en el mismo canal
		ptrCaptureHandler->ptrTIMx->CCMR2 |= (1 << TIM_CCMR2_CC3S_Pos);

		// Configuramos el filtro (sin filtro)
		ptrCaptureHandler->ptrTIMx->CCMR2 &= ~(0xF << TIM_CCMR2_IC3F_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR2 |= (0x3 << TIM_CCMR2_IC3F_Pos);

		// Configuramos el preescaler
		ptrCaptureHandler->ptrTIMx->CCMR2 &= ~(0x3 << TIM_CCMR2_IC3PSC_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR2 |= (ptrCaptureHandler->config.prescalerCapture << TIM_CCMR2_IC3PSC_Pos);

		// Configuramos el flanco que deseamos capturar
		if (ptrCaptureHandler->config.edgeSignal == CAPTURE_RISING_EDGE) {
			// Configuracion 00, para rising edge
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC3P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC3NP);
		} else {
			// Configuracion 01, para falling edge
			ptrCaptureHandler->ptrTIMx->CCER |= (TIM_CCER_CC3P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC3NP);
		}

		// Activamos el modulo de captura
		ptrCaptureHandler->ptrTIMx->CCER |= TIM_CCER_CC3E;

		break;
	}

	case CAPTURE_CHANNEL_4:{
		// Borramos el posible valor cargado
		ptrCaptureHandler->ptrTIMx->CCMR2 &= ~(TIM_CCMR2_CC4S);
		// Configuramos como entrada en el mismo canal
		ptrCaptureHandler->ptrTIMx->CCMR2 |= (1 << TIM_CCMR2_CC4S_Pos);

		// Configuramos el filtro (sin filtro)
		ptrCaptureHandler->ptrTIMx->CCMR2 &= ~(0xF << TIM_CCMR2_IC4F_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR2 |= (0x3 << TIM_CCMR2_IC4F_Pos);

		// Configuramos el preescaler
		ptrCaptureHandler->ptrTIMx->CCMR2 &= ~(0x3 << TIM_CCMR2_IC4PSC_Pos);
		ptrCaptureHandler->ptrTIMx->CCMR2 |= (ptrCaptureHandler->config.prescalerCapture << TIM_CCMR2_IC4PSC_Pos);

		// Configuramos el flanco que deseamos capturar
		if (ptrCaptureHandler->config.edgeSignal == CAPTURE_RISING_EDGE) {
			// Configuracion 00, para rising edge
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC4P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC4NP);
		} else {
			// Configuracion 01, para falling edge
			ptrCaptureHandler->ptrTIMx->CCER |= (TIM_CCER_CC4P);
			ptrCaptureHandler->ptrTIMx->CCER &= ~(TIM_CCER_CC4NP);
		}

		// Activamos el modulo de captura
		ptrCaptureHandler->ptrTIMx->CCER |= TIM_CCER_CC4E;
		break;
	}


	default:{
		__NOP();
		break;
	}

	}// fin del switch-case

	// Configuramos el preescaler del timer, el cual define a que velocidad se incrementa nuestro timer.
	ptrCaptureHandler->ptrTIMx->PSC = ptrCaptureHandler->config.timerSpeed;

}

/* Esta funcion se encarga de lanzar la captura de la frecuencia... en este caso funciona
 * con pulling, examinando cuando se levanta la bandera del evento de captura.
 * */
uint32_t getPeriodFrec(Capture_Handler_t *ptrCaptureHandler){

	uint32_t timestamp1 = 0;
	uint32_t timestamp2 = 0;
	uint32_t deltaTimestamp = 0;

	// reiniciamos el contador del timer
	ptrCaptureHandler->ptrTIMx->CNT = 0;

	/* Todo lo que sigue de aca en adelante depende del canal que se ha seleccionado,
	 * por lo cual se escribe cada código dentro del "case" especifico.
	 * Antes de lanzar la captura (encender el Timer para que cuente),
	 * debemos borrar el valor de los registros CCP, de forma que arranquen limpios
	 * */
	switch (ptrCaptureHandler->config.channel) {
	case CAPTURE_CHANNEL_1: {
		// Borramos el valor inicial del CCP
		ptrCaptureHandler->ptrTIMx->CCR1 = 0;

		// Bajamos la bandera que indica que existe un evento de captura
		ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC1IF;

		// Encendemos el timer para que comience a contar
		ptrCaptureHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;

		// Esperamos a que se de el primer evento
		while(!(ptrCaptureHandler->ptrTIMx->SR & TIM_SR_CC1IF)){}

		// Detenemos el timer
		ptrCaptureHandler->ptrTIMx->CR1 &= ~TIM_CR1_CEN;

		// Capturamos el valor del tiempo almacenado en el CCRx
		timestamp1 = ptrCaptureHandler->ptrTIMx->CCR1;

		// Bajamos la bandera que indica que existe un evento de captura
		ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC1IF;

		// Encendemos el timer para que comience a contar
		ptrCaptureHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;

		// Esperamos a que se de el primer evento
		while (!(ptrCaptureHandler->ptrTIMx->SR & TIM_SR_CC1IF)) {}

		// Detenemos el timer
		ptrCaptureHandler->ptrTIMx->CR1 &= ~TIM_CR1_CEN;

		// Capturamos el valor del tiempo almacenado en el CCRx (sin haber reiniciado despues de la
		// primer captura)
		timestamp2 = ptrCaptureHandler->ptrTIMx->CCR1;

		deltaTimestamp = timestamp2 - timestamp1;

		break;
	}
	case CAPTURE_CHANNEL_2: {
		// Borramos el valor inicial del CCP
		ptrCaptureHandler->ptrTIMx->CCR2 = 0;

		// Encendemos el timer para que comience a contar
		ptrCaptureHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;

		// Bajamos la bandera que indica que existe un evento de captura
		ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC2IF;

		// Esperamos a que se de el primer evento
		while(!(ptrCaptureHandler->ptrTIMx->SR & TIM_SR_CC2IF)){}

		// Capturamos el valor del tiempo almacenado en el CCRx
		timestamp1 = ptrCaptureHandler->ptrTIMx->CCR2;

		// Bajamos la bandera que indica que existe un evento de captura
		ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC2IF;

		// Esperamos a que se de el primer evento
		while (!(ptrCaptureHandler->ptrTIMx->SR & TIM_SR_CC2IF)) {}

		// Detenemos el timer
		ptrCaptureHandler->ptrTIMx->CR1 &= ~TIM_CR1_CEN;

		// Capturamos el valor del tiempo almacenado en el CCRx (sin haber reiniciado despues de la
		// primer captura)
		timestamp2 = ptrCaptureHandler->ptrTIMx->CCR2;

		deltaTimestamp = timestamp2 - timestamp1;

		break;
	}
	case CAPTURE_CHANNEL_3: {
		// Borramos el valor inicial del CCP
		ptrCaptureHandler->ptrTIMx->CCR3 = 0;

		// Encendemos el timer para que comience a contar
		ptrCaptureHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;

		// Bajamos la bandera que indica que existe un evento de captura
		ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC3IF;

		// Esperamos a que se de el primer evento
		while(!(ptrCaptureHandler->ptrTIMx->SR & TIM_SR_CC3IF)){}

		// Capturamos el valor del tiempo almacenado en el CCRx
		timestamp1 = ptrCaptureHandler->ptrTIMx->CCR3;

		// Bajamos la bandera que indica que existe un evento de captura
		ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC3IF;

		// Esperamos a que se de el primer evento
		while (!(ptrCaptureHandler->ptrTIMx->SR & TIM_SR_CC3IF)) {}

		// Detenemos el timer
		ptrCaptureHandler->ptrTIMx->CR1 &= ~TIM_CR1_CEN;

		// Capturamos el valor del tiempo almacenado en el CCRx (sin haber reiniciado despues de la
		// primer captura)
		timestamp2 = ptrCaptureHandler->ptrTIMx->CCR3;

		deltaTimestamp = timestamp2 - timestamp1;
		break;
	}
	case CAPTURE_CHANNEL_4: {
		// Borramos el valor inicial del CCP
		ptrCaptureHandler->ptrTIMx->CCR4 = 0;

		// Encendemos el timer para que comience a contar
		ptrCaptureHandler->ptrTIMx->CR1 |= TIM_CR1_CEN;

		// Bajamos la bandera que indica que existe un evento de captura
		ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC4IF;

		// Esperamos a que se de el primer evento
		while(!(ptrCaptureHandler->ptrTIMx->SR & TIM_SR_CC4IF)){}

		// Capturamos el valor del tiempo almacenado en el CCRx
		timestamp1 = ptrCaptureHandler->ptrTIMx->CCR4;

		// Bajamos la bandera que indica que existe un evento de captura
		ptrCaptureHandler->ptrTIMx->SR &= ~TIM_SR_CC4IF;

		// Esperamos a que se de el primer evento
		while (!(ptrCaptureHandler->ptrTIMx->SR & TIM_SR_CC4IF)) {}

		// Detenemos el timer
		ptrCaptureHandler->ptrTIMx->CR1 &= ~TIM_CR1_CEN;

		// Capturamos el valor del tiempo almacenado en el CCRx (sin haber reiniciado despues de la
		// primer captura)
		timestamp2 = ptrCaptureHandler->ptrTIMx->CCR4;

		deltaTimestamp = timestamp2 - timestamp1;
		break;
	}
	default: {
		break;
	}
	}

	return deltaTimestamp;

}
