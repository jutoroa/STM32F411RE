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
ADC_Config_t 	configADC			= {0};	// ADC 1
TIMER_Handler_t handlerTimer2 		= {0};	// Timer2
TIMER_Handler_t	handlerTimer3		= {0};	// Timer3
GPIO_Handler_t 	handlerUSBRX 		= {0};	// Pin USB PA3 RX
GPIO_Handler_t 	handlerUSBTX 		= {0};	// Pin USB PA2 TX
USART_Handler_t handlerUsart2   	= {0};	// Usart RX

// Variables auxiliar.

uint8_t 	rxData 			= '\0';				// Datos provenientes del Rx
uint8_t		adcIsComplete   = false;			// Variable que nos permite saber el estado de si se completó la conversión ADC
uint16_t 	adcData			= 0;				// Valor obtenido por el ADC
uint16_t 	counter			= 0;				// Contador para cada iteración del ADC
char		bufferData[64]	= {0};				// Buffer de recepción del ADC
uint16_t	adcSignal[ADC_SIGNAL_SIZE]	= {0};	// Almacenamiento para los datos del ADC

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
					startContinuousADC();
				}
				if(rxData == 'p'){
					stopContinousADC();
				}
				// Limpiamos el valor de la variable que guarda los datos del RX
				rxData = '\0';
			}

			if(adcIsComplete == true){

				/*for/uint16_t j = 0; j < ADC_SIGNAL_SIZE; j++){
				 *
				 *
				 * */
				sprintf(bufferData, "ADC = %u, %u \n\r",(unsigned int ) adcData,(unsigned int )counter);
				// WriteMsg(&handlerCommTerminal,"Hola mundo!");
				writeMsg(&handlerUsart2, bufferData);
				//writeChar(&handlerUsart2, 'a');
				counter++;
				adcIsComplete = false;
				//}
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

	configADC.channel									= ADC_CHANNEL_0;
	configADC.resolution								= ADC_RESOLUTION_12_BIT;
	configADC.samplingPeriod							= ADC_SAMPLING_PERIOD_144_CYCLES;
	configADC.dataAlignment								= ADC_ALIGNMENT_RIGHT;


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
	handlerTimer3.timerConfig.Timer_period				= 140;

	//Cargamos la configuración
	GPIO_Config(&handlerStateLED);
	adc_Config(&configADC);
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

void USART2_Callback(void){
	// Lectura de los datos recibidos
	rxData = returnData();
}

void adc_Complete_Callback(void){
	// Cambiamos el valor de adcIsComplete
	adcData = getADC();
	adcIsComplete = true;
}
