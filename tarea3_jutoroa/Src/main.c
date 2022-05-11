/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Juan Pablo Toro Arenas y Emmanuel Ángel Maya
 * @brief          : Desarrollo de la tarea 3
 ******************************************************************************
 * - Implementación de la programación orientada a interrupciones.
 * - El programa funciona de tal manera que:
 *
 ******************************************************************************
 */

// *************** // INCLUDES // *************** //
#include <stm32f4xx.h>
#include <stdint.h>
#include "GPIOxDriver.h"
#include "USARTxDriver.h"
#include "TIMxDriver.h"

// *************** // VARIABLES PROYECTO // *************** //

GPIO_Handler_t handlerStateLED 		= {0};	// StateLED
GPIO_Handler_t handlerLEDPB9   		= {0};	// LED PB9
GPIO_Handler_t handlerUSB  			= {0};	// Pin USB PA3 RX
GPIO_Handler_t handlerUSBTX  		= {0};	// Pin USB PA2 TX
TIMER_Handler_t handlerTimer2 		= {0};	// Timer2
TIMER_Handler_t handlerTimer3 		= {0};	// Timer3
USART_Handler_t handlerUsartRX   	= {0};	// Usart RX
USART_Handler_t handlerUsartTX   	= {0};	// Usart TX

uint8_t interruptor = 0;
uint8_t *ptr_interruptor;
// *************** // Headers // *************** //

void delay(int time);
void __initSystem(void);

// *************** // MAIN // *************** //
int main(void)
{
	// Inicializamos el sistema
	__initSystem();

	//Cargamos la configuración
	GPIO_Config(&handlerStateLED);
	GPIO_Config(&handlerLEDPB9);
	GPIO_Config(&handlerUSB);
	GPIO_Config(&handlerUSBTX);
	Timer_Config(&handlerTimer2);
	Timer_Config(&handlerTimer3);
	USART_Config(&handlerUsartRX);
	USART_Config(&handlerUsartTX);

	// Variable auxiliar para evaluar los casos de 'O'
	ptr_interruptor = &interruptor;


    /* Ciclo principal */
	while(1){
		__NOP();
	}
}

//***********// *********** // Definición de Funciones // *********** //***********//

//***********// Delay //***********//

void delay(int time){
	// Con este ciclo se genera un intervalo de tiempo en el programa (no tiene otro objetivo)
	for (int i = 0; i <= time; i++){
		__NOP();
	}
}

//***********// InitSystem //***********//

void __initSystem(void){

	// Configuración el State LED
	handlerStateLED.pGPIOx								= GPIOA;
	handlerStateLED.GPIO_PinConfig.GPIO_PinNumber		= PIN_5;
	handlerStateLED.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerStateLED.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerStateLED.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerStateLED.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_MEDIUM;

	// Configuración del led Pin B9
	handlerLEDPB9.pGPIOx								= GPIOB;
	handlerLEDPB9.GPIO_PinConfig.GPIO_PinNumber			= PIN_9;
	handlerLEDPB9.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerLEDPB9.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerLEDPB9.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerLEDPB9.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;

	// Configuración del pin PA3 USB
	handlerUSB.pGPIOx									= GPIOA;
	handlerUSB.GPIO_PinConfig.GPIO_PinNumber			= PIN_3;
	handlerUSB.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerUSB.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUSB.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerUSB.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_MEDIUM;
	handlerUSB.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF7;

	// Configuración del pin PA2 USB
	handlerUSBTX.pGPIOx									= GPIOA;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinNumber			= PIN_2;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF7;

	// Configuración del USART del USB para RX
	handlerUsartRX.ptrUSARTx							= USART2;
	handlerUsartRX.USART_Config.USART_mode				= USART_MODE_RXTX;
	handlerUsartRX.USART_Config.USART_baudrate			= USART_BAUDRATE_19200;
	handlerUsartRX.USART_Config.USART_datasize			= USART_DATASIZE_8BIT;
	handlerUsartRX.USART_Config.USART_parity			= USART_PARITY_NONE;
	handlerUsartRX.USART_Config.USART_stopbits			= USART_STOPBIT_1;

	// Configuración del USART del USB para TX
	handlerUsartTX.ptrUSARTx							= USART2;
	handlerUsartTX.USART_Config.USART_mode				= USART_MODE_RXTX;
	handlerUsartTX.USART_Config.USART_baudrate			= USART_BAUDRATE_19200;
	handlerUsartTX.USART_Config.USART_datasize			= USART_DATASIZE_8BIT;
	handlerUsartTX.USART_Config.USART_parity			= USART_PARITY_NONE;
	handlerUsartTX.USART_Config.USART_stopbits			= USART_STOPBIT_1;

	// Configuración del timer2
	handlerTimer2.ptrTIMx								= TIM2;
	handlerTimer2.timerConfig.Timer_mode				= TIMER_MODE_UP;
	handlerTimer2.timerConfig.Timer_speed				= TIMER_INCR_SPEED_1ms;
	handlerTimer2.timerConfig.Timer_period				= 300;

	// Configuración del timer3
	handlerTimer3.ptrTIMx								= TIM3;
	handlerTimer3.timerConfig.Timer_mode				= TIMER_MODE_UP;
	handlerTimer3.timerConfig.Timer_speed				= TIMER_INCR_SPEED_1ms;
	handlerTimer3.timerConfig.Timer_period				= 300;
}
//***********// CallBacks //***********//

void Timer2_Callback(void){
	handlerStateLED.pGPIOx -> ODR ^= GPIO_ODR_OD5;		// Encendido y apagado StateLED
}

void Timer3_Callback(void){
	handlerLEDPB9.pGPIOx -> ODR ^= GPIO_ODR_OD9;		// Encendido y apagado LED PB9
}

void USART2_Callback(void){

	// Lectura de los datos recibidos
	char dataValue = readChar(&handlerUsartRX);
	writeChar(&handlerUsartTX, dataValue);

	// Comparación de los valores recibidos.
	if(dataValue == 'O'){
		*ptr_interruptor ^= 1;				// Acción cada vez que se oprima el interruptor
	}

	if(*ptr_interruptor == 1)				// Se oprimió una vez 'O'
	{
		__NVIC_DisableIRQ(TIM2_IRQn);		// Apagamos la interrupción del timer

		handlerStateLED.pGPIOx -> ODR &= ~(GPIO_ODR_OD5);		// Forzamos a que el LED quede apagado

	}else if(*ptr_interruptor == 0){							// Se oprimió dos veces 'O'

		__NVIC_EnableIRQ(TIM2_IRQn);							// Activamos la interrupción del timer

		if(dataValue == 'n'){
			handlerTimer2.timerConfig.Timer_period				= 300;
			Timer_Config(&handlerTimer2);

		}else if(dataValue == 'f'){
			handlerTimer2.timerConfig.Timer_period				= 200;
			Timer_Config(&handlerTimer2);

		}else if(dataValue == 'u'){
			handlerTimer2.timerConfig.Timer_period				= 100;
			Timer_Config(&handlerTimer2);

		}else{
			__NOP();
		}
	}
}
