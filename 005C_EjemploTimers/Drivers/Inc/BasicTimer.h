/*
 * BasicTimer.h
 *
 *  Created on: 21/04/2022
 *      Author: Juan Pablo
 */

#ifndef INC_BASICTIMER_H_
#define INC_BASICTIMER_H_

#include <stm32f4xx.h>

// Definiciones para la configuración
#define TIMER_MODE_UP		0
#define TIMER_MODE_DOWN		1

#define TIMER_INCR_SPEED_1ms	16000
#define TIMER_INCR_SPEED_100us	1600
#define TIMER_INCR_SPEED_10us	160



typedef struct
{
	uint8_t Timer_mode;				// Selecciona si va Up or Down
	uint16_t Timer_speed;			// Configuración de que tan rapido aumenta
	uint32_t Timer_period;			// Cada cuanto se genera una  interrupción.
}BTIMER_Config_t;

typedef struct
{
	TIM_TypeDef	*ptrTIMx;			// Estructura con los registros que componen el periférico
	BTIMER_Config_t timerConfig;	// Configuración cargada por el usuario
}BTIMER_Handler_t;

/* Prototipos de las funciones */ /* Son las las funciones pública */

void BTimer_Config(BTIMER_Handler_t *prtTimerConfig);
void BTimer_Callback(void);


#endif /* INC_BASICTIMER_H_ */
