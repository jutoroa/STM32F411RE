/*
 * AdcDriver.c
 *
 *  Created on: Apr 28, 2022
 *      Author: namontoy
 */
#include "AdcDriver.h"
#include "GPIOxDriver.h"

GPIO_Handler_t handlerAdcPin = {0};
uint16_t	adcRawData = 0;

void adc_Config(ADC_Config_t *adcConfig){
	/* 1. Configuramos el PinX para que cumpla la función de canal análogo deseado. */
	configAnalogPin(adcConfig->channel);

	/* 2. Activamos la señal de reloj para el periférico ADC1 (bus APB2)*/
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

	// Limpiamos los registros antes de comenzar a configurar
	ADC1->CR1 = 0;
	ADC1->CR2 = 0;

	/* Comenzamos la configuración del ADC1 */
	/* 3. Resolución del ADC */
	switch(adcConfig->resolution){
	case ADC_RESOLUTION_12_BIT:
	{
		ADC1->CR1 &= ~ADC_CR1_RES_0;
		ADC1->CR1 &= ~ADC_CR1_RES_1;
		break;
	}

	case ADC_RESOLUTION_10_BIT:
	{
		ADC1->CR1 |= ADC_CR1_RES_0;
		ADC1->CR1 &= ~ADC_CR1_RES_1;
		break;
	}

	case ADC_RESOLUTION_8_BIT:
	{
		ADC1->CR1 &= ~ADC_CR1_RES_0;
		ADC1->CR1 |= ADC_CR1_RES_1;
		break;
	}

	case ADC_RESOLUTION_6_BIT:
	{
		ADC1->CR1 |= ADC_CR1_RES_0;
		ADC1->CR1 |= ADC_CR1_RES_1;
		break;
	}

	default:
	{
		break;
	}
	}

	/* 4. Configuramos el modo Scan como desactivado */
	ADC1->CR1 &= ~ADC_CR1_SCAN;

	/* 5. Configuramos la alineación de los datos (derecha o izquierda) */
	if(adcConfig->dataAlignment == ADC_ALIGNMENT_RIGHT){
		// Alineación a la derecha (esta es la forma "natural")
		ADC1->CR2 &= ~ADC_CR2_ALIGN;
	}
	else{

		// Alineación a la izquierda (para algunos cálculos matemáticos)
		ADC1->CR2 |= ADC_CR2_ALIGN;
	}

	/* 6. Desactivamos el "continuos mode" */
	ADC1->CR2 &= ~ADC_CR2_CONT;

	/* 7. Acá se debería configurar el sampling...*/
	if(adcConfig->channel < ADC_CHANNEL_9){
		ADC1->SMPR2 |=  (adcConfig->samplingPeriod << (3*(adcConfig->channel)));
	}
	else{
		ADC1->SMPR1 |=  (adcConfig->samplingPeriod << (3*(adcConfig->channel)));
	}

	/* 8. Configuramos la secuencia y cuantos elementos hay en la secuencia */
	// Al hacerlo todo 0, estamos seleccionando solo 1 elemento en el conteo de la secuencia
	ADC1->SQR1 = 0;

	// Asignamos el canal de la conversión a la primera posición en la secuencia
	ADC1->SQR3 |= (adcConfig->channel << 0);

	/* 9. Configuramos el preescaler del ADC en 2:1 (el mas rápido que se puede tener */
	ADC->CCR |= ADC_CCR_ADCPRE_0;
	//ADC->CCR &= ~ADC_CCR_ADCPRE_1;

	/* 10. Desactivamos las interrupciones globales */
	__disable_irq();

	/* 11. Activamos la interrupción debida a la finalización de una conversión EOC */
	ADC1->CR1 |= ADC_CR1_EOCIE;

	/* 11a. Matriculamos la interrupción en el NVIC*/
	__NVIC_EnableIRQ(ADC_IRQn);
	__NVIC_SetPriority(ADC_IRQn, 4);

	/* 12. Activamos el modulo ADC */
	ADC1->CR2 |= ADC_CR2_ADON;

	/* 13. Activamos las interrupciones globales */
	__enable_irq();
}

/**/
void startSingleADC(void){
	/* Iniciamos un ciclo de conversión ADC */
	ADC1->SR &= ~ADC_SR_OVR;
	ADC1->CR2 |= ADC_CR2_SWSTART;

}

/**/
void startContinousADC(void){

	/* Activamos el modo continuo de ADC */
	ADC1->CR2 |= ADC_CR2_CONT;

	/* Iniciamos un ciclo de conversión ADC */
	ADC1->CR2 |= ADC_CR2_SWSTART;

}

/* Función que me retorna el ultimo dato adquirido por la ADC*/
uint16_t getADC(void){
	return adcRawData;
}

/* Esta es la ISR de la interrupción por conversión ADC */
void ADC_IRQHandler(void){
	// Evaluamos que se dio la interrupción por conversión ADC
	if(ADC1->SR & ADC_SR_EOC){
		// Leemos el resultado de la conversión ADC y lo cargamos en un valor auxiliar
		adcRawData = ADC1->DR;

		// Hacemos el llamado a la función que se ejecutará en el main
		adcComplete_Callback();
	}

}

/* Función debil, que debe ser sobreescrita en el main. */
__attribute__ ((weak)) void adcComplete_Callback(void){
	__NOP();
}

/* Con esta función configuramos que pin deseamos que funcione como ADC*/
void configAnalogPin(uint8_t adcChannel) {

	// Con este switch seleccionamos el canal y lo configuramos como análogo.
	switch (adcChannel) {

	case ADC_CHANNEL_0: {
		// Es el pin PA0
		handlerAdcPin.pGPIOx 						= GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_0;
		break;
	}
		;

	case ADC_CHANNEL_1: {
		// Es el pin PA1
		handlerAdcPin.pGPIOx 						= GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_1;
		break;
	}

	case ADC_CHANNEL_2: {
		// Es el pin PA2
		handlerAdcPin.pGPIOx						 = GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_2;
		break;
	}

	case ADC_CHANNEL_3: {
		// Es el pin PA3
		handlerAdcPin.pGPIOx 						= GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_3;
		break;
	}

	case ADC_CHANNEL_4: {
		// Es el pin PA4
		handlerAdcPin.pGPIOx 						= GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_4;
		break;
	}

	case ADC_CHANNEL_5: {
		// Es el pin PA5
		handlerAdcPin.pGPIOx 						= GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
		break;
	}
	case ADC_CHANNEL_6: {
		//Es el pin PA6
		handlerAdcPin.pGPIOx 						= GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_6;
		break;
	}
	case ADC_CHANNEL_7: {
		//Es el pin PA7
		handlerAdcPin.pGPIOx 						= GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_7;
		break;
	}
	case ADC_CHANNEL_8: {
		//Es el pin PB0
		handlerAdcPin.pGPIOx 						= GPIOB;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_0;
		break;
	}
	case ADC_CHANNEL_9: {
		//Es el pin PB1
		handlerAdcPin.pGPIOx 						= GPIOB;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_1;
		break;
	}
	case ADC_CHANNEL_10: {
		//Es el pin PC0
		handlerAdcPin.pGPIOx 						= GPIOC;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_0;
		break;
	}
	case ADC_CHANNEL_11: {
		//Es el pin PC1
		handlerAdcPin.pGPIOx 						= GPIOC;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_1;
		break;
	}
	case ADC_CHANNEL_12: {
		//Es el pin PC2
		handlerAdcPin.pGPIOx 						= GPIOC;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_2;
		break;
	}
	case ADC_CHANNEL_13: {
		//Es el pin PC3
		handlerAdcPin.pGPIOx 						= GPIOC;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_3;
		break;
	}
	case ADC_CHANNEL_14: {
		//Es el pin PC4
		handlerAdcPin.pGPIOx						= GPIOC;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_4;
		break;
	}
	case ADC_CHANNEL_15: {
		//Es el pin PC5
		handlerAdcPin.pGPIOx					 	= GPIOC;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
		break;
	}
	default: {
		break;
	}

	}
	handlerAdcPin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
	GPIO_Config(&handlerAdcPin);

}


void ADC_ConfigMultichannel (ADC_Config_t *adcConfig, uint8_t numeroDeCanales){

	/* 2. Activamos la senal de reloj para el periferico ADC1(bus APB2) */
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

	//Limpiamos los registros antes de comenzar a configurar
	ADC1->CR1 = 0;
	ADC1->CR2 = 0;

	/* 6. Desactivamos el "continuos mode" */
	ADC1 -> CR2 &= ~ADC_CR2_CONT;

	/* 4. Configuramos el modo Scan como activado */
	ADC1 -> CR1 |= ADC_CR1_SCAN;

	/* 8. Configuramos la secuencia y cuantos elementos hay en la secuencias */
	ADC1->SQR1 |= (numeroDeCanales - 1) << ADC_SQR1_L_Pos;

	//Ciclo para recorrer cada elemento de arreglo y configurarlo

	for (uint8_t i = 0; i<= numeroDeCanales; i++){
		/* 1. Configuramos el PinX para que cumpla la funcion del canal analogo deseado */
		//configAnalogPin(adcConfig->channel);
		configAnalogPin(adcConfig[i].channel);

		/*Comenzamos la configuracion de ADC1*/

		/* 3. Resolucion del ADC */

		switch (adcConfig[i].resolution)
		{
		case ADC_RESOLUTION_12_BIT:
		{
			ADC1->CR1 &= ~ADC_CR1_RES_0;
			ADC1->CR1 &= ~ADC_CR1_RES_1;
			break;
		}
		case ADC_RESOLUTION_10_BIT:
		{
			ADC1->CR1 |= ADC_CR1_RES_0;
			ADC1->CR1 &= ~ADC_CR1_RES_1;
			break;
		}
		case ADC_RESOLUTION_8_BIT:
		{
			ADC1->CR1 &= ~ADC_CR1_RES_0;
			ADC1->CR1 |= ADC_CR1_RES_1;
			break;
		}
		case ADC_RESOLUTION_6_BIT:
		{
			ADC1->CR1 |= ADC_CR1_RES_0;
			ADC1->CR1 |= ADC_CR1_RES_1;
			break;
		}
		default:
		{
			break;
		}
		}

		/* 5. Configuramos la alineacion de los datos (derecha o izquierda)*/

		if (adcConfig[i]. dataAlignment  == ADC_ALIGNMENT_RIGHT){
			//Alineacion a la derecha (esta es la forma 'natural'
			ADC1 -> CR2 &= ~ADC_CR2_ALIGN;
		}else{
			//Alineacion a la izquierda (para alguno calculos matematicos
			ADC1 -> CR2 |= ADC_CR2_ALIGN;
		}

	//Configuración del sampling rate
	switch(adcConfig[i].channel){
		case ADC_CHANNEL_0: {
			ADC1->SMPR2 |= (adcConfig[i].samplingPeriod << 3*ADC_CHANNEL_0);
			break;
		}
		case ADC_CHANNEL_1: {
			ADC1->SMPR2 |= (adcConfig[i].samplingPeriod << 3*ADC_CHANNEL_1);
			break;
		}
		case ADC_CHANNEL_2: {
			ADC1->SMPR2 |= (adcConfig[i].samplingPeriod << 3*ADC_CHANNEL_2);
			break;
		}
		case ADC_CHANNEL_3: {
			ADC1->SMPR2 |= (adcConfig[i].samplingPeriod << 3*ADC_CHANNEL_3);
			break;
		}
		case ADC_CHANNEL_4: {
			ADC1->SMPR2 |= (adcConfig[i].samplingPeriod << 3*ADC_CHANNEL_4);
			break;
		}
		case ADC_CHANNEL_5: {
			ADC1->SMPR2 |= (adcConfig[i].samplingPeriod << 3*ADC_CHANNEL_5);
			break;
		}
		case ADC_CHANNEL_6: {
			ADC1->SMPR2 |= (adcConfig[i].samplingPeriod << 3*ADC_CHANNEL_6);
			break;
		}

		case ADC_CHANNEL_7: {
			ADC1->SMPR2 |= (adcConfig[i].samplingPeriod << 3*ADC_CHANNEL_7);
			break;
		}
		case ADC_CHANNEL_8: {
			ADC1->SMPR2 |= (adcConfig[i].samplingPeriod << 3*ADC_CHANNEL_8);
			break;
		}
		case ADC_CHANNEL_9: {
			ADC1->SMPR2 |= (adcConfig[i].samplingPeriod << 3*ADC_CHANNEL_9);
			break;
		}
		case ADC_CHANNEL_10: {
			ADC1->SMPR1 |= (adcConfig[i].samplingPeriod << 3*(ADC_CHANNEL_10-ADC_CHANNEL_10));
			break;
		}
		case ADC_CHANNEL_11: {
			ADC1->SMPR1 |= (adcConfig[i].samplingPeriod << 3*(ADC_CHANNEL_11-ADC_CHANNEL_10));
			break;
		}
		case ADC_CHANNEL_12: {
			ADC1->SMPR1 |= (adcConfig[i].samplingPeriod << 3*(ADC_CHANNEL_12-ADC_CHANNEL_10));
			break;
		}
		case ADC_CHANNEL_13: {
			ADC1->SMPR1 |= (adcConfig[i].samplingPeriod << 3*(ADC_CHANNEL_13-ADC_CHANNEL_10));
			break;
		}
		case ADC_CHANNEL_14: {
			ADC1->SMPR1 |= (adcConfig[i].samplingPeriod << 3*(ADC_CHANNEL_14-ADC_CHANNEL_10));
			break;
		}
		case ADC_CHANNEL_15: {
			ADC1->SMPR1 |= (adcConfig[i].samplingPeriod << 3*(ADC_CHANNEL_15-ADC_CHANNEL_10));
			break;
		}
		default:
			break;
		}

		//Asignamos el canal de la conversión a la primera posición en la secuencia
		if (numeroDeCanales <= 6){
			ADC1->SQR3 |= (adcConfig[i].channel <<	i*5);
		}

	}


	/* 9. Configuramos el prescaler del ADC en 2:1 (el mas rápido que se puede tener */
	ADC->CCR &= ~ADC_CCR_ADCPRE;

	/* 10. Desactivamos las interrupciones globales */
	__disable_irq();

	/* 11. Activamos la interrupción debida a la finalización de una conversión EOC */
	ADC1->CR1 |= ADC_CR1_EOCIE;

	/* 11a Interrupción al final de cada conversión de canal  */
	ADC1->CR2 |= ADC_CR2_EOCS;

	/* 11b. Matriculamos la interrupción en el NVIC */
	__NVIC_EnableIRQ(ADC_IRQn);
	__NVIC_SetPriority(ADC_IRQn, 4);

	/* 12. Activamos el moduloADC */
	ADC1 -> CR2 |= ADC_CR2_ADON;

	/* 13. Activamos las interrupciones globales */
	__enable_irq();
}
