/*
 ************************************************************************************************************************************************************
 * @file           : AdcDriver.c
 * @author         : Emmanuel Ángel Maya
 * @brief          : Archivo .c periférico ADC
 ************************************************************************************************************************************************************
 * En este programa se escribe el código .c necesario para el periférico ADC
 * Contiene la configuración para trabajar con el mismo:
 * 	- Channel
 * 	- Resolution
 * 	- Sampling period
 * 	- Data alignment
 ************************************************************************************************************************************************************
 */
#include "AdcDriver.h"
#include "GPIOxDriver.h"

GPIO_Handler_t	handlerAdcPin	= {0};
uint16_t		adcRawData		= {0};

void adc_Config(ADC_Config_t *adcConfig){

	/* 1. Se configura el PinX para que cumpla la función de canal análogo. */
	configAnalogPin(adcConfig -> channel);

	/* 2. Se activa la señal de reloj para el periférico ADC1 (bus APB2). */
	RCC -> APB2ENR |= RCC_APB2ENR_ADC1EN;

	// Se limpian los registros antes de empezar a configurar.
	ADC1 -> CR1 = 0;
	ADC1 -> CR2 = 0;

	/* Se comienza la configuración del ADC1. */
	/* 3. Resolución del ADC */
	switch(adcConfig -> resolution){

	// 12 Bits
	case ADC_RESOLUTION_12_BIT:
	{
		ADC1 -> CR1 &= ~ADC_CR1_RES_0;
		ADC1 -> CR1 &= ~ADC_CR1_RES_1;
		break;
	}

	// 10 Bits
	case ADC_RESOLUTION_10_BIT:
	{
		ADC1 -> CR1 |= ADC_CR1_RES_0;
		ADC1 -> CR1 &= ~ADC_CR1_RES_1;
		break;
	}

	// 8 Bits
	case ADC_RESOLUTION_8_BIT:
	{
		ADC1 -> CR1 &= ~ADC_CR1_RES_0;
		ADC1 -> CR1 |= ADC_CR1_RES_1;
		break;
	}

	// 6 Bits
	case ADC_RESOLUTION_6_BIT:
	{
		ADC1 -> CR1 |= ADC_CR1_RES_0;
		ADC1 -> CR1 |= ADC_CR1_RES_1;
		break;
	}

	// Se interrumpe por defecto sin realizar otra acción.
	default:
	{
		break;
	}
	}

	/* 4. Se configura el modo Scan como desactivado. */
	ADC1 -> CR1 &= ~(ADC_CR1_SCAN);

	/* 5. Se configura la alineación de los datos (derecha o izquierda). */
	if(adcConfig -> dataAlignment == ADC_ALIGNMENT_RIGHT){

		// Alineación a la derecha (esta es la forma "natural").
		ADC1 -> CR2 &= ~(ADC_CR2_ALIGN);
	}
	else{

		// Alineación a la izquierda (para algunos cálculos matemáticos).
		ADC1 -> CR2 |= ADC_CR2_ALIGN;
	}

	/* 6. Se desactiva el "continuos mode". */
	ADC1 -> CR2 &= ~(ADC_CR2_CONT);

	/* 7. Acá se configura el sampling. */
	if(adcConfig -> channel < ADC_CHANNEL_10){
		ADC1 -> SMPR2 |= (adcConfig -> samplingPeriod << (3 * (adcConfig -> channel)));
	}
	else{
		ADC1 -> SMPR1 |= (adcConfig -> samplingPeriod << (3 * (adcConfig -> channel-9)));
	}

	/* 8. Se configura la secuencia y cuántos elementos hay en la secuencia. */
	// Al hacer todo 0, se selecciona solo un elemento en el conteo de la secuencia.
	ADC1 -> SQR1 = 0;

	// Se asigna el canal de la conversión a la primera posición en la secuencia.
	ADC1 -> SQR3 |= (adcConfig -> channel << 0);

	/* 9. Se configura el preescaler del ADC en 2:1 (el más rápido que se puede tener). */
	ADC -> CCR = 0;

	/* 10. Se desactivan las interrupciones globales. */
	__disable_irq();

	/* 11. Se Activa la interrupción debida a la finalización de una conversión EOC. */
	ADC1 -> CR1 |= ADC_CR1_EOCIE;

	/* 11.1 Se Matricula la interrupción en el NVIC. */
	__NVIC_EnableIRQ(ADC_IRQn);

	/* 12- Se Activa el módulo ADC. */
	ADC1 -> CR2 |= ADC_CR2_ADON;

	/* 13. Se activan las interrupciones globales. */
	__enable_irq();
}

/**/
void startConversion(void){

	/* Se inicia un ciclo de conversión ADC. */
	ADC1 -> CR2 |= ADC_CR2_SWSTART;
}

/* Función que retorna el último dato adquirido por la ADC. */
void ADC_IRQHandler(void){

	// Se evalúa si se dio la interrupción por conversión ADC.
	if(ADC1 -> SR & ADC_SR_EOC){

		// Se lee el resultado de la conversión ADC y se carga en un valor auxiliar.
		adcRawData = ADC1 -> DR;

		// Se hace el llamado a la función que se ejecutará en el main.
		adcComplete_Callback();
	}
}

/* Función débil. Ésta deber ser sobreescrita en el main. */
__attribute__ ((weak)) void adcComplete_Callback(void){
	__NOP();
}

/* Con esta función se configura el pin que funcionará como ADC. */
void configAnalogPin(uint8_t adcChannel){

	// Con este switch se selecciona el canal y se configura como análogo. */
	switch (adcChannel){

	// Configuración ADC1_0
	case ADC_CHANNEL_0:
	{
		// Es el pin PA0.
		handlerAdcPin.pGPIOx							= GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber		= PIN_0;
		break;
	}

	// Configuración ADC1_1
	case ADC_CHANNEL_1:
	{
		// Es el pin PA1.
		handlerAdcPin.pGPIOx							= GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber		= PIN_1;
		break;
	}

	// Configuración ADC1_2
	case ADC_CHANNEL_2:
	{
		// Es el pin PA2.
		handlerAdcPin.pGPIOx							= GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber		= PIN_2;
		break;
	}

	// Configuración ADC1_3
	case ADC_CHANNEL_3:
	{
		// Es el pin PA3.
		handlerAdcPin.pGPIOx							= GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber		= PIN_3;
		break;
	}

	// Configuración ADC1_4
	case ADC_CHANNEL_4:
	{
		// Es el pin PA4.
		handlerAdcPin.pGPIOx							= GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber		= PIN_4;
		break;
	}

	// Configuración ADC1_5
	case ADC_CHANNEL_5:
	{
		// Es el pin PA5.
		handlerAdcPin.pGPIOx							= GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber		= PIN_5;
		break;
	}

	// Configuración ADC1_6
	case ADC_CHANNEL_6:
	{
		// Es el pin PA6.
		handlerAdcPin.pGPIOx							= GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber		= PIN_6;
		break;
	}

	// Configuración ADC1_7
	case ADC_CHANNEL_7:
	{
		// Es el pin PA7.
		handlerAdcPin.pGPIOx							= GPIOA;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber		= PIN_7;
		break;
	}

	// Configuración ADC1_8
	case ADC_CHANNEL_8:
	{
		// Es el pin PB0.
		handlerAdcPin.pGPIOx							= GPIOB;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber		= PIN_0;
		break;
	}

	// Configuración ADC1_9
	case ADC_CHANNEL_9:
	{
		// Es el pin PB1.
		handlerAdcPin.pGPIOx							= GPIOB;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber		= PIN_1;
		break;
	}

	// Configuración ADC1_10
	case ADC_CHANNEL_10:
	{
		// Es el pin PC0.
		handlerAdcPin.pGPIOx							= GPIOC;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber		= PIN_0;
		break;
	}

	// Configuración ADC1_11
	case ADC_CHANNEL_11:
	{
		// Es el pin PC1.
		handlerAdcPin.pGPIOx							= GPIOC;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber		= PIN_1;
		break;
	}

	// Configuración ADC1_12
	case ADC_CHANNEL_12:
	{
		// Es el pin PC2.
		handlerAdcPin.pGPIOx							= GPIOC;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber		= PIN_2;
		break;
	}

	// Configuración ADC1_13
	case ADC_CHANNEL_13:
	{
		// Es el pin PC3.
		handlerAdcPin.pGPIOx							= GPIOC;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber		= PIN_3;
		break;
	}

	// Configuración ADC1_14
	case ADC_CHANNEL_14:
	{
		// Es el pin PC4.
		handlerAdcPin.pGPIOx							= GPIOC;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber		= PIN_4;
		break;
	}

	// Configuración ADC1_15
	case ADC_CHANNEL_15:
	{
		// Es el pin PC5.
		handlerAdcPin.pGPIOx							= GPIOC;
		handlerAdcPin.GPIO_PinConfig.GPIO_PinNumber		= PIN_5;
		break;
	}

	default:
	{
		// Se termina el ciclo por defecto.
		break;
	}
	}

	// Se configura el pin en modo análogo.
	handlerAdcPin.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ANALOG;
	GPIO_Config(&handlerAdcPin);
}
