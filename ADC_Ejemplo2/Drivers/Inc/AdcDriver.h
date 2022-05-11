/*
 * AdcDriver.h
 *
 *  Created on: 28/04/2022
 *      Author: Juan Pablo
 */

#ifndef INC_ADCDRIVER_H_
#define INC_ADCDRIVER_H_

#include "stm32f4xx.h"

#define ADC_SIGNAL_SIZE	256

#define ADC_CHANNEL_0		0
#define ADC_CHANNEL_1		1
#define ADC_CHANNEL_2		2
#define ADC_CHANNEL_3		3
#define ADC_CHANNEL_4		4
#define ADC_CHANNEL_5		5
#define ADC_CHANNEL_6		6
#define ADC_CHANNEL_7		7
#define ADC_CHANNEL_8		8
#define ADC_CHANNEL_9		9
#define ADC_CHANNEL_10		10
#define ADC_CHANNEL_11		11
#define ADC_CHANNEL_12		12
#define ADC_CHANNEL_13		13
#define ADC_CHANNEL_14		14
#define ADC_CHANNEL_15		15

#define ADC_RESOLUTION_12_BIT	0
#define ADC_RESOLUTION_10_BIT	1
#define ADC_RESOLUTION_8_BIT	2
#define ADC_RESOLUTION_6_BIT	3

#define ADC_ALIGNMENT_RIGHT		0
#define ADC_ALIGNMENT_LEFT		1

#define ADC_SAMPLING_PERIOD_3_CYCLES		0b000;
#define ADC_SAMPLING_PERIOD_15_CYCLES		0b001;
#define ADC_SAMPLING_PERIOD_28_CYCLES		0b010;
#define ADC_SAMPLING_PERIOD_56_CYCLES		0b011;
#define ADC_SAMPLING_PERIOD_84_CYCLES		0b100;
#define ADC_SAMPLING_PERIOD_112_CYCLES		0b101;
#define ADC_SAMPLING_PERIOD_144_CYCLES		0b110;
#define ADC_SAMPLING_PERIOD_480_CYCLES		0b111;

typedef struct
{
	uint8_t 	channel;			// Canal ADC que será utilizado para la conversión ADC
	uint8_t 	resolution;			// Precisión con la que el ADC hace la adquisición del dato
	uint16_t 	samplingPeriod;		// Tiempo deseado para hacer la adquisición del dato
	uint8_t		dataAlignment;		// Alineación a la izquierda o a la derecha
}ADC_Config_t;

void adc_Config(ADC_Config_t *adcConfig);
void configAnalogPin(uint8_t adcChannel);
void adc_Complete_Callback(void);
void startSingleADC(void);
void startContinuousADC(void);
void stopContinousADC(void);
uint16_t getADC(void);

#endif /* INC_ADCDRIVER_H_ */
