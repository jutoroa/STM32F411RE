/*
 ************************************************************************************************************************************************************
 * @file           : TIMxDriver.h
 * @author         : Juan Pablo Toro Arenas y Emmanuel Ángel Maya
 * @brief          : Archivo .h periférico TIMx
 ************************************************************************************************************************************************************
 * En este programa se escribe el código .h necesario para el uso de Timers.
 * Contiene la documentación necesaria para trabajar con los registros TIMx.
 * 	- Mode
 * 	- Speed
 * 	- Period
 ************************************************************************************************************************************************************
 */

#ifndef INC_TIMXDRIVER_H_
#define INC_TIMXDRIVER_H_

#include "stm32f4xx.h"

// Definiciones para la configuración de los timers.
#define TIM_MODE_UP					0
#define TIM_MODE_DOWN				1

#define TIM_INCR_SPEED_1ms			16000
#define TIM_INCR_SPEED_100us		1600
#define TIM_INCR_SPEED_10us			160

typedef struct
{
	uint8_t			TIM_mode;				// Selecciona si va Up or Down.
	uint16_t		TIM_speed;				// Configuración de qué tan rapido aumenta.
	uint32_t		TIM_period;				// Cada cuanto se genera una  interrupción.
}TIM_Config_t;

typedef struct
{
	TIM_TypeDef		*ptrTIMx;				// Estructura con los registros que componen el periférico.
	TIM_Config_t	TIM_Config;				// Configuración cargada por el usuario.
}TIM_Handler_t;

/* Prototipos de las funciones */
/* Son las funciones públicas */
void TIM_Config(TIM_Handler_t *prtTIM_Config);
void TIM2_Callback(void);
void TIM3_Callback(void);
void TIM4_Callback(void);
void TIM5_Callback(void);

#endif /* INC_TIMXDRIVER_H_ */
