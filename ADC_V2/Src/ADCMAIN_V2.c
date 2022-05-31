/*
 * mainADC.c
 *
 *  Created on: 4/05/2022
 *  Author: Juan Pablo
 */


// *************** // INCLUDES // *************** //
#include <stm32f4xx.h>
#include <stdint.h>
#include <stdbool.h>
#include "GPIOxDriver.h"
#include "USARTxDriver.h"
#include "TIMxDriver.h"
#include "AdcDriver.h"

// *************** // VARIABLES PROYECTO // *************** //

GPIO_Handler_t 	handlerStateLED 	= {0};	// StateLED
ADC_Config_t 	configADC1			= {0};	// ADC 1
ADC_Config_t 	configADC2			= {0};	// ADC 2
ADC_Config_t 	configADC3			= {0};	// ADC 3
TIMER_Handler_t handlerTimer2 		= {0};	// Timer2
TIMER_Handler_t	handlerTimer3		= {0};	// Timer3
GPIO_Handler_t 	handlerUSBRX 		= {0};	// Pin USB PA3 RX
GPIO_Handler_t 	handlerUSBTX 		= {0};	// Pin USB PA2 TX
USART_Handler_t handlerUsart2   	= {0};	// Usart RX

#define ADC_SIGNAL_SIZE 	3

// Variables auxiliar.

uint8_t 	rxData 			= '\0';				// Datos provenientes del Rx
uint8_t		adcIsComplete   = false;			// Variable que nos permite saber el estado de si se completó la conversión ADC
uint16_t 	adcData			= 0;				// Valor obtenido por el ADC
uint16_t 	counter			= 0;				// Contador para cada iteración del ADC
uint16_t 	auxCounter		= 0;
char		bufferData[64]	= {0};				// Buffer de recepción del ADC
uint16_t	adcSignal[ADC_SIGNAL_SIZE]	= {0};	// Almacenamiento para los datos del ADC
uint16_t	dataPosition		= 0;

// *************** // Headers // *************** //

void initSystem(void);

// *************** // MAIN // *************** //
int main(void)
{
		// Inicializamos el sistema
		initSystem();

	    /* Ciclo principal */
		while(1){

			// Hacemos un "eco" con el valor que nos llega por el serial
			if((rxData != '\0')){
				writeChar(&handlerUsart2, rxData);

				if(rxData == 's'){
					//Lanzamos una nueva conversión ADC de un sólo valor
					startSingleADC();
				}
				if(rxData == 'c'){
					// Conversión de modo continua
					//startContinuousADC();
					startTimer(&handlerTimer3);
				}
				if(rxData == 'm'){
					// Conversión de modo continua
					startContinuousADC();
				}
				if(rxData == 'p'){
					stopContinousADC();
					stopTimer(&handlerTimer3);
				}
				// Limpiamos el valor de la variable que guarda los datos del RX
				rxData = '\0';
			}

			if(adcIsComplete == true){
				ADC1 -> CR2 &= ~(ADC_CR2_SWSTART);
				stopContinousADC();
				//sprintf(bufferData, "ADC = %u, %u \n\r",(unsigned int ) adcData,(unsigned int )counter);
				for(uint16_t j = 0; j < ADC_SIGNAL_SIZE; j++){
					sprintf(bufferData, "%u\n\r",(unsigned int) adcSignal[j]);
					writeMsg(&handlerUsart2, bufferData);
				}
//				writeMsg(&handlerUsart2, bufferData);
//				counter++;
				adcIsComplete = false;
		}
	}
}
//***********// *********** // Definición de Funciones // *********** //***********//

//***********// InitSystem //***********//

// Función que define la configuración de todos los pines y periféricos

void initSystem(void){

	// Configuración el State LED
	handlerStateLED.pGPIOx								= GPIOA;
	handlerStateLED.GPIO_PinConfig.GPIO_PinNumber		= PIN_5;
	handlerStateLED.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerStateLED.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerStateLED.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerStateLED.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_MEDIUM;

	// Configuración del ADC1

	//configADC1.channel									= ADC_CHANNEL_4;
	configADC1.resolution								= ADC_RESOLUTION_12_BIT;
	configADC1.samplingPeriod							= ADC_SAMPLING_PERIOD_144_CYCLES;
	configADC1.dataAlignment							= ADC_ALIGNMENT_RIGHT;
	configADC1.numberOfChannels							= 3;
	configADC1.channelMode								= ADC_MULTI_CHANNEL;
	configADC1.channel_First							= ADC_CHANNEL_4;
	configADC1.channel_Second							= ADC_CHANNEL_6;
	configADC1.channel_Third							= ADC_CHANNEL_7;

	adc_Config(&configADC1);

	//multiChannelADC(&configADC1);

	// Configuración del pin PA3 USB RX
	handlerUSBRX.pGPIOx									= GPIOA;
	handlerUSBRX.GPIO_PinConfig.GPIO_PinNumber			= PIN_3;
	handlerUSBRX.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerUSBRX.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUSBRX.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerUSBRX.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUSBRX.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF7;

	// Configuración del pin PA2 USB TX
	handlerUSBTX.pGPIOx									= GPIOA;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinNumber			= PIN_2;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF7;

	// Configuración del USART del USB para RXTX
	handlerUsart2.ptrUSARTx								= USART2;
	handlerUsart2.USART_Config.USART_mode				= USART_MODE_RXTX;
	handlerUsart2.USART_Config.USART_baudrate			= USART_BAUDRATE_19200;
	handlerUsart2.USART_Config.USART_datasize			= USART_DATASIZE_8BIT;
	handlerUsart2.USART_Config.USART_parity				= USART_PARITY_ODD;
	handlerUsart2.USART_Config.USART_stopbits			= USART_STOPBIT_1;
	handlerUsart2.USART_Config.USART_interrupt 			= USART_INTERRUPT_RX_ENABLE;


	// Configuración del timer2
	handlerTimer2.ptrTIMx								= TIM2;
	handlerTimer2.timerConfig.Timer_mode				= TIMER_MODE_UP;
	handlerTimer2.timerConfig.Timer_speed				= TIMER_INCR_SPEED_1ms;
	handlerTimer2.timerConfig.Timer_period				= 250;

	// Configuración del timer3
	handlerTimer3.ptrTIMx								= TIM3;
	handlerTimer3.timerConfig.Timer_mode				= TIMER_MODE_UP;
	handlerTimer3.timerConfig.Timer_speed				= TIMER_INCR_SPEED_1ms;
	handlerTimer3.timerConfig.Timer_period				= 500;

	//Cargamos la configuración
	GPIO_Config(&handlerStateLED);
	GPIO_Config(&handlerUSBRX);
	GPIO_Config(&handlerUSBTX);
	USART_Config(&handlerUsart2);
	Timer_Config(&handlerTimer2);
	Timer_Config(&handlerTimer3);
}
//***********// CallBacks //***********//
void Timer2_Callback(void){
	handlerStateLED.pGPIOx -> ODR ^= GPIO_ODR_OD5;		// Encendido y apagado StateLED
}

void Timer3_Callback(void){
	startSingleADC();									// Lanzamos la conversión ADC
}

void USART2_Callback(void){
	// Lectura de los datos recibidos
	rxData = returnData();
}

void adc_Complete_Callback(void){
	// Cambiamos el valor de adcIsComplete
//	adcData = getADC();
//	adcIsComplete = true;
	adcSignal[dataPosition] = getADC();
	dataPosition++;
	if (dataPosition >= ADC_SIGNAL_SIZE){
		dataPosition = 0;
		adcIsComplete = true;
	}
}
