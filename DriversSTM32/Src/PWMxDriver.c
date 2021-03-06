/*
 * PWMxDriver.c
 *
 *  Created on: May 24, 2022
 *      Author: jutoroa
 */



#include <stm32f4xx.h>

#include "PWMxDriver.h"

/*
 * Función que configura un Timer básico para que genere interrupciones de forma
 * periódica.
 *
 * */

//NOTA: Hay que desactivar las interrupciones globales y luego volver a activarlas

void PWM_Config(PWM_Handler_t *prtPWMConfig){

	/* 1. Activamos la señal de reloj para el periférico específico */
	if(prtPWMConfig -> ptrTIMx == TIM2){
		// Activamos la señal de reloj del TIM2
		RCC -> APB1ENR |= RCC_APB1ENR_TIM2EN;

	}else if(prtPWMConfig -> ptrTIMx == TIM3){
		// Activamos la señal de reloj del TIM3
		RCC -> APB1ENR |= RCC_APB1ENR_TIM3EN;

	}else if(prtPWMConfig -> ptrTIMx == TIM4){
		// Activamos la señal de reloj del TIM4
		RCC -> APB1ENR |= RCC_APB1ENR_TIM4EN;

	}else if(prtPWMConfig -> ptrTIMx == TIM5){
		// Activamos la señal de reloj del TIM5
		RCC -> APB1ENR |= RCC_APB1ENR_TIM5EN;
	}

	/* 2. Configuración del modo del timer (UP or DOWN) */

	if(prtPWMConfig -> timerConfig.pwmMode == TIMER_MODE_UP){
		// Configuramos el timer en modo UP
		prtPWMConfig -> ptrTIMx -> CR1 &= ~(TIM_CR1_DIR);
		// Ponemos el contador (CNT) en cero.
		prtPWMConfig -> ptrTIMx -> CNT = 0;
	}
	else{
		// Configuramos el timer en modo DOWN
		prtPWMConfig -> ptrTIMx -> CR1 |= (TIM_CR1_DIR);
		// Ponemos en el contador (CNT) el valor del periodo
		prtPWMConfig -> ptrTIMx -> CNT = prtPWMConfig -> timerConfig.pwmPeriod -1;
	}

	/* 3. Configuración del Prescaler */
	prtPWMConfig -> ptrTIMx -> PSC = prtPWMConfig -> timerConfig.pwmSpeed -1;

	/* 4. Configuramos el periodo*/
	prtPWMConfig -> ptrTIMx -> ARR = prtPWMConfig -> timerConfig.pwmPeriod -1;

	/* 5. Activamos al timer para que comience a incrementarse  */
	prtPWMConfig -> ptrTIMx -> CR1 |= TIM_CR1_CEN;

	/* 6. Configuramos el Duty */
	prtPWMConfig -> ptrTIMx -> CCR1 = prtPWMConfig -> timerConfig.pwmDuty -1;

	/* 7. Configuramos el OutPut Register CCMR1 y CCMR2 */

	if(prtPWMConfig -> timerConfig.channel == PWM_CHANNEL_1){
		// Escogemos el canal 1 como salida
		prtPWMConfig -> ptrTIMx -> CCMR1 &= ~TIM_CCMR1_CC1S;	// Ponemos 00: CC1 channel is configured as output.
		// Escoger si el Preload está activado
		prtPWMConfig -> ptrTIMx -> CCMR1 |= TIM_CCMR1_OC1PE;
		// Configurar el Output compare 1 mode
		prtPWMConfig -> ptrTIMx -> CCMR1 |= (0b110 << TIM_CCMR1_OC1M_Pos);
		// Activamos el enable a través del CCER
		prtPWMConfig -> ptrTIMx -> CCER |= TIM_CCER_CC1E;

	} else if(prtPWMConfig -> timerConfig.channel == PWM_CHANNEL_2){
		// Escogemos el canal 2 como salida
		prtPWMConfig -> ptrTIMx -> CCMR1 &= ~TIM_CCMR1_CC2S;	// Ponemos 00: CC1 channel is configured as output.
		// Escoger si el Preload está activado
		prtPWMConfig -> ptrTIMx -> CCMR1 |= TIM_CCMR1_OC2PE;
		// Configurar el Output compare 1 mode
		prtPWMConfig -> ptrTIMx -> CCMR1 |= (0b110 << TIM_CCMR1_OC2M_Pos);
		// Activamos el enable a través del CCER
		prtPWMConfig -> ptrTIMx -> CCER |= TIM_CCER_CC2E;

	} else if(prtPWMConfig -> timerConfig.channel == PWM_CHANNEL_3){
		// Escogemos el canal 3 como salida
		prtPWMConfig -> ptrTIMx -> CCMR2 &= ~TIM_CCMR2_CC3S;	// Ponemos 00: CC1 channel is configured as output.
		// Escoger si el Preload está activado
		prtPWMConfig -> ptrTIMx -> CCMR2 |= TIM_CCMR2_OC3PE;
		// Configurar el Output compare 1 mode
		prtPWMConfig -> ptrTIMx -> CCMR2 |= (0b110 << TIM_CCMR2_OC3M_Pos);
		// Activamos el enable a través del CCER
		prtPWMConfig -> ptrTIMx -> CCER |= TIM_CCER_CC3E;

	} else if(prtPWMConfig -> timerConfig.channel == PWM_CHANNEL_4){
		// Escogemos el canal 4 como salida
		prtPWMConfig -> ptrTIMx -> CCMR2 &= ~TIM_CCMR2_CC4S;	// Ponemos 00: CC1 channel is configured as output.
		// Escoger si el Preload está activado
		prtPWMConfig -> ptrTIMx -> CCMR2 |= TIM_CCMR2_OC4PE;
		// Configurar el Output compare 1 mode
		prtPWMConfig -> ptrTIMx -> CCMR2 |= (0b110 << TIM_CCMR2_OC4M_Pos);
		// Activamos el enable a través del CCER
		prtPWMConfig -> ptrTIMx -> CCER |= TIM_CCER_CC4E;
	}
}


void setDutty(PWM_Handler_t *ptrPWMHandler, uint16_t newDuty){
	ptrPWMHandler -> timerConfig.pwmDuty = newDuty;

	if(ptrPWMHandler -> timerConfig.channel == PWM_CHANNEL_1){
		/* Se configura el dutty cycle para el canal 1*/
		ptrPWMHandler -> ptrTIMx -> CCR1 = ptrPWMHandler -> timerConfig.pwmDuty - 1;
	}
	else if(ptrPWMHandler-> timerConfig.channel == PWM_CHANNEL_2){
		/* Se configura el dutty cycle para el canal2*/
		ptrPWMHandler -> ptrTIMx -> CCR2 = ptrPWMHandler -> timerConfig.pwmDuty - 1;
	}
	else if(ptrPWMHandler-> timerConfig.channel == PWM_CHANNEL_3){
		/* Se configura el dutty cycle para el canal3*/
		ptrPWMHandler -> ptrTIMx -> CCR3 = ptrPWMHandler -> timerConfig.pwmDuty - 1;
	}
	else if(ptrPWMHandler-> timerConfig.channel == PWM_CHANNEL_4){
		/* Se configura el dutty cycle para el canal4*/
		ptrPWMHandler -> ptrTIMx -> CCR4 = ptrPWMHandler -> timerConfig.pwmDuty - 1;
	}

}
