/*
 * AdcDriver.c
 *
 *  Created on: 28/04/2022
 *      Author: Juan Pablo
 */


#include "AdcDriver.h"
#include "GPIOxDriver.h"

GPIO_Handler_t handlerAdcPin = {0};
uint16_t adcRawData 		 = {0};

void adc_Config(ADC_Config_t *adcConfig){
	/* 1. Configuramos el PinX para que cumpla la función de canal análogo deseado. */
	configAnalogPin(adcConfig -> channel);

	/* 2. Activamos la señal de reloj para el periférico ADC1 (bus APB2) */
	RCC -> APB2ENR |= RCC_APB2ENR_ADC1EN;

	// Limpiamos los registros antes de comenzar a configurar
	ADC1 -> CR1 = 0;
	ADC1 -> CR2 = 0;

	/* Comenzamos la configuración del ADC1 */
	/* 3. Resolución del ADC */
	switch(adcConfig -> resolution){
	case ADC_RESOLUTION_12_BIT:
	{
		ADC1 -> CR1 &= ~ADC_CR1_RES_0;
		ADC1 -> CR1 &= ~ADC_CR1_RES_1;
		break;
	}

	case ADC_RESOLUTION_10_BIT:
	{
		ADC1 -> CR1 |= ADC_CR1_RES_0;
		ADC1 -> CR1 &= ~ADC_CR1_RES_1;
		break;
	}

	case ADC_RESOLUTION_6_BIT:
	{
		ADC1 -> CR1 |= ADC_CR1_RES_0;
		ADC1 -> CR1 |= ADC_CR1_RES_1;
		break;
	}

	default:
	{
		break;
	}
	}

	/* 4. Configuramos el modo Scan como desactivado */
	ADC1 -> CR1 &= ~(ADC_CR1_SCAN);

	/* 5. Configuramos la alineación de los datos (derecha o izquierda) */
	if(adcConfig -> dataAlignment == ADC_ALIGNMENT_RIGHT){
		// Alineación a la derecha (esta es la forma "natural")
		ADC1 -> CR2 &= ~(ADC_CR2_ALIGN);
	}
	else{

		// Alineación a la izquierda (para algunos cálculos matemáticos).
		ADC1 -> CR2 |= ADC_CR2_ALIGN;
	}

	/* 6. Desactivamos el "continuos mode" */
	ADC1 -> CR2 &= ~(ADC_CR2_CONT);

	/* 7. Acá se debería configurar el sampling */
	if(adcConfig -> channel < ADC_CHANNEL_9){
		ADC1 -> SMPR2 |= (adcConfig -> samplingPeriod << (3 * (adcConfig -> channel)));
	}
	else{
		ADC1 -> SMPR1 |= (adcConfig -> samplingPeriod << (3 * (adcConfig -> channel)));
	}

	/* 8. Configuramos la secuencia y cuantos elementos hay en la secuencia */
	// Al hacerlo todo 0, estamos seleccionando solo 1 elemento en el conteo de la secuencia
	ADC1 -> SQR1 = 0;

	// Asignamos el canal de la conversión a la primera posición en la secuencia
	ADC1 -> SQR3 |= (adcConfig -> channel << 0);

	/* 9. Configuramos el preescaler del ADC en 2:1 (el más rápido que se puede tener) */
	ADC -> CCR = 0;

	/* 10. Desactivamos las interrupciones */
}
