/*
 * BasicTimer.h
 *
 *  Created on: 21/04/2022
 *      Author: Juan Pablo
 */

#ifndef INC_TIMXDRIVER_H_
#define INC_TIMXDRIVER_H_

#include <stm32f4xx.h>

// Definiciones para la configuración
#define TIMER_MODE_UP		0
#define TIMER_MODE_DOWN		1

#define TIMER_INCR_SPEED_1ms	16000
#define TIMER_INCR_SPEED_100us	1600
#define TIMER_INCR_SPEED_10us	160
#define TIMER_INCR_SPEED_1us	16


typedef struct
{
	uint8_t Timer_mode;				// Selecciona si va Up or Down
	uint16_t Timer_speed;			// Configuración de que tan rapido aumenta
	uint32_t Timer_period;			// Cada cuanto se genera una  interrupción.
}TIMER_Config_t;

typedef struct
{
	TIM_TypeDef	*ptrTIMx;			// Estructura con los registros que componen el periférico
	TIMER_Config_t timerConfig;	// Configuración cargada por el usuario
}TIMER_Handler_t;

/* Prototipos de las funciones */ /* Son las las funciones pública */

void Timer_Config(TIMER_Handler_t *prtTimerConfig);
void Timer2_Callback(void);
void Timer3_Callback(void);
void Timer4_Callback(void);
void Timer5_Callback(void);

void startTimer(TIMER_Handler_t *ptrTimerConfig);
void stopTimer(TIMER_Handler_t *ptrTimerConfig);

#endif /* INC_TIMXDRIVER_H_ */
