/*
 * EXTIxDriver.h
 *
 *  Created on: 12/05/2022
 *      Author: Juan Pablo
 */

#ifndef EXTIXDRIVER_H_
#define EXTIXDRIVER_H_

#include "stm32f4xx.h"

// NOTA: Primero definimos los pines PYX (Y = A,B,C,D,E y X = 0,1,2,15

// Macros para seleccionar el tipo de flanco
#define TRIGGER_FALLING		0
#define TRIGGER_RISING		1
#define TRIGGER_BOTH		2

// Macros para seleccionar el puerto
#define EXTI_PA		0x0000
#define EXTI_PB		0x0001
#define EXTI_PC		0x0010

typedef struct{
	GPIO_TypeDef	*port;			// Puerto para el GPIOx (A,B,C,D,E)
	uint8_t			pin;			// # Pin (0,1,2...15)
	uint8_t 		trigger;		// Tipo de flanco
	uint8_t 		PinControl;		// Seleccionamos si deseamos una salida pull-up, pull-down o "libre"
}EXTI_Config_t;


void EXTI_Config(EXTI_Config_t *extiConfig);
//void configEXTIPin(uint8_t EXTIPort, uint8_t EXTIPin, uint8_t PinPudControl);
void EXTI0_Callback(void);
void EXTI1_Callback(void);
void EXTI2_Callback(void);
void EXTI3_Callback(void);
void EXTI4_Callback(void);
void EXTI5_Callback(void);
void EXTI6_Callback(void);
void EXTI7_Callback(void);
void EXTI8_Callback(void);
void EXTI9_Callback(void);
void EXTI10_Callback(void);
void EXTI11_Callback(void);
void EXTI12_Callback(void);
void EXTI13_Callback(void);
void EXTI14_Callback(void);
void EXTI15_Callback(void);


#endif /* EXTIXDRIVER_H_ */
