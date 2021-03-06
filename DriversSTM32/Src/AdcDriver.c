/*
 * AdcDriver.c
 *
 *  Created on: 28/04/2022
 *      Author: Juan Pablo
 */


#include "AdcDriver.h"
#include "GPIOxDriver.h"

GPIO_Handler_t handlerAdcPin = {0};
uint16_t 		adcRawData 	 = {0};

void adc_Config(ADC_Config_t *adcConfig){
	// Desactivamos las interrupciones globales

	__disable_irq();

	/* 1. Configuramos el PinX para que cumpla la función de canal análogo deseado. */

	/*1. Verificamos si es single Channel, o multichannel */
	if(adcConfig -> channelMode == ADC_SINGLE_CHANNEL){
		configAnalogPin(adcConfig -> channel);
	}
	else if(adcConfig -> channelMode == ADC_MULTI_CHANNEL){
		if(adcConfig -> numberOfChannels == 3){
			configAnalogPin(adcConfig -> channel_First);
			configAnalogPin(adcConfig -> channel_Second);
			configAnalogPin(adcConfig -> channel_Third);
		}
	}

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

	case ADC_RESOLUTION_8_BIT:
	{
		ADC1 -> CR1 &= ~ADC_CR1_RES_0;
		ADC1 -> CR1 |= ADC_CR1_RES_1;
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

	/* 4. Configuramos el modo Scan mode */
	if(adcConfig -> channelMode == ADC_SINGLE_CHANNEL){
		//Configuramos el modo Scan como desactivado
		ADC1 -> CR1 &= ~(ADC_CR1_SCAN);
	}
	else if(adcConfig -> channelMode == ADC_MULTI_CHANNEL){
		//Configuramos el modo Scan como activado
		ADC1 -> CR1  |= (ADC_CR1_SCAN);
	}
	else{
		__NOP();
	}

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

	if(adcConfig -> channelMode == ADC_SINGLE_CHANNEL){
		if(adcConfig -> channel < ADC_CHANNEL_9){
			ADC1 -> SMPR2 |= (adcConfig -> samplingPeriod << (3 * (adcConfig -> channel)));
		}
		else{
			ADC1 -> SMPR1 |= (adcConfig -> samplingPeriod << (3 * (adcConfig -> channel)));
		}
	}
	else if(adcConfig -> channelMode == ADC_MULTI_CHANNEL){

		if(adcConfig -> numberOfChannels <= ADC_CHANNEL_9){
			ADC1 -> SMPR2 |= (adcConfig -> samplingPeriod << (3 * (adcConfig -> channel_First)));
			ADC1 -> SMPR2 |= (adcConfig -> samplingPeriod << (3 * (adcConfig -> channel_Second)));
			ADC1 -> SMPR2 |= (adcConfig -> samplingPeriod << (3 * (adcConfig -> channel_Third)));
		}
		else{
			__NOP();//ADC1 -> SMPR1 |= (adcConfig -> samplingPeriod << (3 * (adcConfig -> channel)));
		}
	}
	else{
		__NOP();
	}


	/* 8. Configuramos la secuencia y cuantos elementos hay en la secuencia */

	if(adcConfig -> channelMode == ADC_SINGLE_CHANNEL){
		// Al hacerlo todo 0, estamos seleccionando solo 1 elemento en el conteo de la secuencia
		ADC1 -> SQR1 = 0;
		// Asignamos el canal de la conversión a la primera posición en la secuencia
		ADC1 -> SQR3 |= (adcConfig -> channel << 0);
	}
	else if(adcConfig -> channelMode == ADC_MULTI_CHANNEL){

		// Activamos la interrupción al final de cada conversión single.
		ADC1 -> CR2 |= ADC_CR2_EOCS;

		// Activamos cuantos elementos hay en la secuencia
		ADC1 -> SQR1 |= ((adcConfig -> numberOfChannels-1) << ADC_SQR1_L_Pos);

		if((adcConfig -> numberOfChannels >= 1) & (adcConfig -> numberOfChannels <= 6)){
			ADC1 -> SQR3 |= adcConfig -> channel_First << 0;
			ADC1 -> SQR3 |= adcConfig -> channel_Second << 5;
			ADC1 -> SQR3 |= adcConfig -> channel_Third << 10;
			//ADC1 -> SQR3 |= (adcConfig -> channel << ((adcConfig -> sequencePos -1)*5));
		}
		else if((adcConfig -> numberOfChannels >= 7) & (adcConfig -> numberOfChannels <= 12)){
			ADC1 -> SQR2 |= (adcConfig -> channel << ((adcConfig -> sequencePos -7)*5));
		}
		else if((adcConfig -> numberOfChannels >= 13) & (adcConfig -> numberOfChannels <= 16)){
			ADC1 -> SQR1 |= (adcConfig -> channel << ((adcConfig -> sequencePos -13)*5));
		}
		else{
			__NOP();
		}
	}

	/* 9. Configuramos el preescaler del ADC en 2:1 (el más rápido que se puede tener) */
	ADC -> CCR = 0;

	/* 10. Desactivamos las interrupciones */

	ADC1 -> CR1 |= ADC_CR1_EOCIE;

	/* 11a. Matriculamos la interrupción en el NVIC */
	__NVIC_EnableIRQ(ADC_IRQn);

	/* 11.b Establecemos la importancia de la interrupción */

	__NVIC_SetPriority(ADC_IRQn,4);

	/* 12. Activamos el modulo ADC */
	ADC1 -> CR2 |= ADC_CR2_ADON;

	/* 13. Activamos las interrupciones globales */
	__enable_irq();
}

/**/

void startSingleADC(void){
	/* Iniciamos un ciclo de conversión ADC */
	ADC1 -> CR2 |= ADC_CR2_SWSTART;
}

void startContinuousADC(void){
	/* Conversión continua */
	ADC1 -> CR2 |= (ADC_CR2_CONT);
	/* Iniciamos un ciclo de conversión ADC */
	ADC1 -> CR2 |= ADC_CR2_SWSTART;
}

void stopContinousADC(void){
	ADC1 -> CR2 &= ~(ADC_CR2_SWSTART);
}

/* Función que me retorna el último dato adquirido por la ADC */
uint16_t getADC(void){
	return adcRawData;
}

void multiChannelADC(ADC_Config_t *adcConfig){
	ADC1 -> SQR1 |= (adcConfig -> numberOfChannels << ADC_SQR1_L_Pos);
	ADC1 -> CR1  |= (ADC_CR1_SCAN);
}
/* Esta es la ISR de la interrupción por conversión ADC */
void ADC_IRQHandler(void){
	if(ADC1 -> SR & ADC_SR_EOC){
		// Leemos el resultado de la conversión ADC y lo cargamos en un valor auxiliar
		adcRawData = ADC1 -> DR;

		// Hacemos el llamado a la función que se ejecutará en el main
		adc_Complete_Callback();
	}
}

/* Función débil, que debe ser sobreescrita en el main */
__attribute__((weak)) void adc_Complete_Callback(void){
	__NOP();
}

/* Con esta función configuramos que pin deseamos que funcione como ADC */
void configAnalogPin(uint8_t adcChannel){

	// Con este switch seleccionamos el canal y lo configuramos como analogo.
	switch(adcChannel){

	case ADC_CHANNEL_0:
	{
		// Es el pin PA0
		handlerAdcPin.pGPIOx						= GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_0;
		break;
	};

	// Configuración ADC1_1
	case ADC_CHANNEL_1:
	{
		// Es el pin PA1.
		handlerAdcPin.pGPIOx = GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_1;
		break;
	}

	// Configuración ADC1_2
	case ADC_CHANNEL_2:
	{
		// Es el pin PA2.
		handlerAdcPin.pGPIOx = GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_2;
		break;
	}

	// Configuración ADC1_3
	case ADC_CHANNEL_3:
	{
		// Es el pin PA3.
		handlerAdcPin.pGPIOx = GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_3;
		break;
	}

	// Configuración ADC1_4
	case ADC_CHANNEL_4:
	{
		// Es el pin PA4.
		handlerAdcPin.pGPIOx = GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_4;
		break;
	}

	// Configuración ADC1_5
	case ADC_CHANNEL_5:
	{
		// Es el pin PA5.
		handlerAdcPin.pGPIOx = GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
		break;
	}

	// Configuración ADC1_6
	case ADC_CHANNEL_6:
	{
		// Es el pin PA6.
		handlerAdcPin.pGPIOx = GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_6;
		break;
	}

	// Configuración ADC1_7
	case ADC_CHANNEL_7:
	{
		// Es el pin PA7.
		handlerAdcPin.pGPIOx = GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_7;
		break;
	}

	// Configuración ADC1_8
	case ADC_CHANNEL_8:
	{
		// Es el pin PB0.
		handlerAdcPin.pGPIOx = GPIOB;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_0;
		break;
	}

	// Configuración ADC1_9
	case ADC_CHANNEL_9:
	{
		// Es el pin PB1.
		handlerAdcPin.pGPIOx = GPIOB;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_1;
		break;
	}

	// Configuración ADC1_10
	case ADC_CHANNEL_10:
	{
		// Es el pin PC0.
		handlerAdcPin.pGPIOx = GPIOC;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_0;
		break;
	}

	// Configuración ADC1_11
	case ADC_CHANNEL_11:
	{
		// Es el pin PC1.
		handlerAdcPin.pGPIOx = GPIOC;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_1;
		break;
	}

	// Configuración ADC1_12
	case ADC_CHANNEL_12:
	{
		// Es el pin PC2.
		handlerAdcPin.pGPIOx = GPIOC;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_2;
		break;
	}

	// Configuración ADC1_13
	case ADC_CHANNEL_13:
	{
		// Es el pin PC3.
		handlerAdcPin.pGPIOx = GPIOC;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_3;
		break;
	}

	// Configuración ADC1_14
	case ADC_CHANNEL_14:
	{
		// Es el pin PC4.
		handlerAdcPin.pGPIOx = GPIOC;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_4;
		break;
	}

	// Configuración ADC1_15
	case ADC_CHANNEL_15:
	{
		// Es el pin PC5.
		handlerAdcPin.pGPIOx = GPIOC;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber = PIN_5;
		break;
	}

	default:
	{
		// Se termina el ciclo por defecto.
		break;
	}
	}

	// Se configura el pin en modo análogo.
	handlerAdcPin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
	GPIO_Config(&handlerAdcPin);
	}
