/*
 * PLLDriver.h
 *
 *  Created on: Nov 18, 2022
 *      Author: namontoy
 */

#ifndef PLLDRIVER_H_
#define PLLDRIVER_H_

#include "stm32f4xx.h"

#define MCO1_PRESCALER_DIV_3	0b101
#define MCO1_PRESCALER_DIV_4	0b110
#define MCO1_PRESCALER_DIV_5	0b111

#define MCO1_HSI_CHANNEL	0b00
#define MCO1_LSE_CHANNEL	0b01
#define MCO1_HSE_CHANNEL	0b10
#define MCO1_PLL_CHANNEL	0b11

#define HSI_CLOCK_CONFIGURED	0
#define HSE_CLOCK_CONFIGURED	1
#define PLL_CLOCK_CONFIGURED	2


#define MAIN_CLOCK

void configPLL(void);
void configMCO1(uint8_t preescalerMCO, uint8_t channelMCO);
uint8_t getConfigMianClock(void);



#endif /* PLLDRIVER_H_ */
