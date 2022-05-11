/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Juan Pablo Toro Arenas
 * @brief          : Desarrollo del driver para el USART
 ******************************************************************************
 * - Implementación de las liberías CMSIS
 * - Migración del GPIO driver
 * - Desarrollo de las librerías para el manejo del USART
 ******************************************************************************
 */


#include <stm32f4xx.h>
#include <stdint.h>
#include "GPIOxDriver.h"
#include "USARTxDriver.h"
#include "BasicTimer.h"

/* Variables del proyecto */

GPIO_Handler_t handlerLedOk = {0};
BTIMER_Handler_t handlerTimerEjemplo = {0};

// *************** // Headers // *************** //

void delay(int time);


// *************** // Headers // *************** //
int main(void)
{

	//	Configuramos el PIN
	handlerLedOk.pGPIOx								= GPIOA;
	handlerLedOk.GPIO_PinConfig.GPIO_PinNumber		= PIN_5;
	handlerLedOk.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_OUT;
	handlerLedOk.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerLedOk.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerLedOk.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_MEDIUM;


	// Configuración del timer
	handlerTimerEjemplo.ptrTIMx						= TIM2;
	handlerTimerEjemplo.timerConfig.Timer_mode		= TIMER_MODE_UP;
	handlerTimerEjemplo.timerConfig.Timer_speed		= TIMER_INCR_SPEED_1ms;
	handlerTimerEjemplo.timerConfig.Timer_period	= 500;


	//Cargamos la configuración
	GPIO_Config(&handlerLedOk);
	BTimer_Config(&handlerTimerEjemplo);

    /* Ciclo principal */
	while(1){
	}
}

//***********// Definición de Funciones //***********//

void delay(int time){
	// Con este ciclo se genera un intervalo de tiempo en el programa (no tiene otro objetivo)
	for (int i = 0; i <= time; i++){
		__NOP();
	}
}

void BTimer_Callback(void){
	handlerLedOk.pGPIOx -> ODR ^= GPIO_ODR_OD5;
}
