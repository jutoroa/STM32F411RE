/*
 * Tarea5Main.c
 *
 *  Created on: 20/06/22
 *  Author: Juan Pablo Toro
 *
 *  // *************** // Tarea 5 // *************** //
 *
 *  Código que permite la adquisición de datos de una señal a 1.1 kHz aproximadamente. Estos se dan a través del
 *  puerto PA0. Para realizar el muestreo de datos se debe presionar la tecla 'c', y se imprimirán todos los datos 'de golpe'
 *  por la terminal.
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
ADC_Config_t 	handlerADC			= {0};	// ADC
TIMER_Handler_t handlerTimer2 		= {0};	// Timer2 (Timer encargado del StateLED)
TIMER_Handler_t	handlerTimer3		= {0};	// Timer3 (Timer encargado del muestreo de datos del ADC)
GPIO_Handler_t 	handlerUSBRX 		= {0};	// Pin USB PA3 RX
GPIO_Handler_t 	handlerUSBTX 		= {0};	// Pin USB PA2 TX
USART_Handler_t handlerUsart2   	= {0};	// Usart RX

#define ADC_SAMPLING_SIZE	100				// Número de datos a muestrear

// Variables auxiliar.

uint8_t 	rxData 								= '\0';		// Datos provenientes del Rx
uint8_t		adcIsComplete   					= false;	// Variable que nos permite saber el estado de si se completó la conversión ADC
uint16_t 	adcData								= 0;		// Valor obtenido por el ADC
uint16_t 	counterADC							= 0;		// Contador para cada iteración del ADC
uint16_t	ADCBufferSignal[ADC_SAMPLING_SIZE]	= {0};		// Almacenamiento para los datos del ADC
char		bufferData[64]						= {0};		// Buffer para envío de datos del ADC
uint16_t	dataPosition						= 0;		// Contador (índice) para poder barrer cada uno de los datos de ADCBufferSignal

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
					startTimer(&handlerTimer3);
				}
				if(rxData == 'p'){
					stopContinousADC();
					stopTimer(&handlerTimer3);
				}
				// Limpiamos el valor de la variable que guarda los datos del RX
				rxData = '\0';
			}

			// Si la conversión ADC se realizó n veces (donde n es la cantidad de datos del buffer)
			if (dataPosition >= ADC_SAMPLING_SIZE){
				// Reinicie el contador
				dataPosition = 0;
				// Levante la bandera de que se completaron todas las conversiones
				adcIsComplete = true;
				// Detenga el timer3 encargado de la conversión ADC
				stopTimer(&handlerTimer3);
			}

			// Si la conversión ADC se completó con todos los datos dados en ADC_SAMPLING_SIZE
			if(adcIsComplete == true){

				// Imprima cada dato por el terminal
				for(uint16_t j = 0; j < ADC_SAMPLING_SIZE; j++){
					sprintf(bufferData, "%u\n\r",(unsigned int) ADCBufferSignal[j]);
					writeMsg(&handlerUsart2, bufferData);
				}
				// Se baja la bandera de la conversión ADC
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

	GPIO_Config(&handlerStateLED);

	// Configuración del ADC
	handlerADC.channel									= ADC_CHANNEL_0;
	handlerADC.resolution								= ADC_RESOLUTION_12_BIT;
	handlerADC.samplingPeriod							= ADC_SAMPLING_PERIOD_28_CYCLES;
	handlerADC.dataAlignment							= ADC_ALIGNMENT_RIGHT;
	handlerADC.numberOfChannels							= 1;
	handlerADC.channelMode								= ADC_SINGLE_CHANNEL;
	adc_Config(&handlerADC);

	// Configuración del pin PA3 USB RX
	handlerUSBRX.pGPIOx									= GPIOA;
	handlerUSBRX.GPIO_PinConfig.GPIO_PinNumber			= PIN_3;
	handlerUSBRX.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerUSBRX.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUSBRX.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerUSBRX.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUSBRX.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF7;

	GPIO_Config(&handlerUSBRX);

	// Configuración del pin PA2 USB TX
	handlerUSBTX.pGPIOx									= GPIOA;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinNumber			= PIN_2;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF7;

	GPIO_Config(&handlerUSBTX);

	// Configuración del USART del USB para RXTX
	handlerUsart2.ptrUSARTx								= USART2;
	handlerUsart2.USART_Config.USART_mode				= USART_MODE_RXTX;
	handlerUsart2.USART_Config.USART_baudrate			= USART_BAUDRATE_19200;
	handlerUsart2.USART_Config.USART_datasize			= USART_DATASIZE_8BIT;
	handlerUsart2.USART_Config.USART_parity				= USART_PARITY_ODD;
	handlerUsart2.USART_Config.USART_stopbits			= USART_STOPBIT_1;
	handlerUsart2.USART_Config.USART_interrupt 			= USART_INTERRUPT_RX_ENABLE;

	USART_Config(&handlerUsart2);

	// Configuración del timer2
	handlerTimer2.ptrTIMx								= TIM2;
	handlerTimer2.timerConfig.Timer_mode				= TIMER_MODE_UP;
	handlerTimer2.timerConfig.Timer_speed				= TIMER_INCR_SPEED_1ms;
	handlerTimer2.timerConfig.Timer_period				= 250;

	Timer_Config(&handlerTimer2);

	// Configuración del timer3
	handlerTimer3.ptrTIMx								= TIM3;
	handlerTimer3.timerConfig.Timer_mode				= TIMER_MODE_UP;
	handlerTimer3.timerConfig.Timer_speed				= TIMER_INCR_SPEED_1us;
	handlerTimer3.timerConfig.Timer_period				= 12;

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
// Leemos los datos del ADC y los guardamos en la iésima posición del ADCBufferSignal. Posteriormente avanzamos a la siguiente posición
// para la próxima conversión.
	ADCBufferSignal[dataPosition] = getADC();
	dataPosition++;
}
