/*
 * PWMxDriver.h
 *
 *  Created on: May 24, 2022
 *      Author: jutoroa
 */

#ifndef PWMXDRIVER_H_
#define PWMXDRIVER_H_

#include <stm32f4xx.h>

// Definiciones para la configuración

#define TIMER_MODE_UP		0
#define TIMER_MODE_DOWN		1

#define TIMER_INCR_SPEED_1ms	16000
#define TIMER_INCR_SPEED_100us	1600
#define TIMER_INCR_SPEED_10us	160

#define PWD_CHANNEL_1		0
#define PWD_CHANNEL_2		1
#define PWD_CHANNEL_3		2
#define PWD_CHANNEL_4		3


typedef struct
{
	uint8_t 	pwmMode;			// Selecciona si va Up or Down
	uint16_t 	pwmSpeed;			// Configuración de que tan rapido aumenta
	uint32_t 	pwmPeriod;			// Cada cuanto se genera una  interrupción.
	uint32_t	pwmDuty;			// Porcentaje en el que está activo (Define el ángulo para servo motores)
	uint32_t	channel;			// Definimos el canal que va a ser activado
}PWM_Config_t;

typedef struct
{
	TIM_TypeDef	*ptrTIMx;			// Estructura con los registros que componen el periférico
	PWM_Config_t timerConfig;	// Configuración cargada por el usuario
}PWM_Handler_t;

/* Prototipos de las funciones */ /* Son las las funciones pública */

void PWM_Config(PWM_Handler_t *prtPWMConfig);


#endif /* PWMXDRIVER_H_ */
