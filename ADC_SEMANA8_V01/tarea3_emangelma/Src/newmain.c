/*
 ************************************************************************************************************************************************************
 * @file           : newmain.c
 * @author         : Juan Pablo Toro Arenas y Emmanuel Ángel Maya
 * @brief          : Desarrollo de la tarea 3
 ************************************************************************************************************************************************************
 * - - - - - Tarea 3: Implementación de la programación orientada a interrupciones - - - - -
 * El programa funciona de tal manera que se controlan 2 LEDs por medio de interrupciones generadas
 * por dos timers (TIM2 y TIM3) y el periférico USART2.
 * Un LED será llamado State LED, que indicará que el programa está corriendo.
 * El segundo LED, cambiará su frecuencia de encendido y apagado dependiendo del caracter leído por
 * el puerto serial a través del USART2.
 ************************************************************************************************************************************************************
 */

/***********************************| INCLUDES |***********************************/
#include <stm32f4xx.h>
#include <stdint.h>
#include "GPIOxDriver.h"
#include "USARTxDriver.h"
#include "TIMxDriver.h"

/***************************| DEFINICIÓN DE VARIABLES |****************************/
GPIO_Handler_t	handlerStateLED		= {0};	// StateLED (PA5)
GPIO_Handler_t	handlerLEDPB9		= {0};	// LED externo (PB9)
GPIO_Handler_t	handlerUSB			= {0};	// Pin USB RX (PA3)

TIM_Handler_t	handlerTIM2			= {0};	// TIM2
TIM_Handler_t	handlerTIM3			= {0};	// TIM3

USART_Handler_t handlerUsartRX		= {0};	// USART2: RX

uint8_t 		dataValue 			= '\0';

// uint8_t 		interruptor 		= 0;
// uint8_t 		*ptr_interruptor;

/************************************| HEADERS |***********************************/
// void delay(int time);
void initSystem(void);
void LEDConfig(void);

/*************************************| MAIN |*************************************/
int main(void)
{
	// Inicialización del sistema.
	initSystem();

	// Lectura de la configuración.
	GPIO_Config(&handlerStateLED);
	GPIO_Config(&handlerLEDPB9);
	GPIO_Config(&handlerUSB);
	TIM_Config(&handlerTIM2);
	TIM_Config(&handlerTIM3);
	USART_Config(&handlerUsartRX);

	// Variable auxiliar para evaluar los casos de 'O'.
	dataValue = '\0';

    /* Ciclo principal */
	while(1){
		LEDConfig();
	}
}

/****************************| Definición de Funciones |****************************/

/**********************| Configuración del LED externo (PB9) |**********************/
void LEDConfig(void){

	// El caracter leído es 'O'.
	if(dataValue == 'O'){

		// Limpieza del valor leído.
		dataValue = '\0';

		// Se cambia la configuración de las interrupciones para el TIM3 (On/Off).
		TIM3 -> DIER ^= TIM_DIER_UIE;
	}

	// Interrupciones desactivadas para el TIM3.
	if(!(TIM3 -> DIER & TIM_DIER_UIE)){

		// Se apaga el LED externo (PB9).
		GPIO_WritePin(&handlerLEDPB9, RESET);

		//	handlerStateLED.pGPIOx -> ODR &= ~(GPIO_ODR_OD9);
	}

	// Interrupciones activadas para el TIM3.
	else if(TIM3 -> DIER & TIM_DIER_UIE){

		// El caracter leído es 'n'.
		if(dataValue == 'n'){

			// Se cambia el periodo del TIM3 a 300 ms.
			handlerTIM3.TIM_Config.TIM_period = 300;

			// Se carga la nueva configuración (normal).
			TIM_Config(&handlerTIM3);

			// Limpieza del valor leído.
			dataValue = '\0';
		}

		// El caracter leído es 'f'.
		else if(dataValue == 'f'){

			// Se cambia el periodo del TIM3 a 200 ms.
			handlerTIM3.TIM_Config.TIM_period = 200;

			// Se carga la nueva configuración (fast).
			TIM_Config(&handlerTIM3);

			// Limpieza del valor leído.
			dataValue = '\0';
		}

		// El caracter leído es 'u'.
		else if(dataValue == 'u'){

			// Se cambia el periodo del TIM3 a 100 ms.
			handlerTIM3.TIM_Config.TIM_period = 100;

			// Se carga la nueva configuración (ultra fast).
			TIM_Config(&handlerTIM3);

			// Limpieza del valor leído.
			dataValue = '\0';
		}
	}
}
/******************************| InitSystem |******************************/
void initSystem(void){

	// Configuración del pin (PA5) para el State LED
	handlerStateLED.pGPIOx								= GPIOA;
	handlerStateLED.GPIO_PinConfig.GPIO_PinNumber		= PIN_5;
	handlerStateLED.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerStateLED.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerStateLED.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerStateLED.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_MEDIUM;

	// Configuración del pin (PB9) para el LED externo
	handlerLEDPB9.pGPIOx								= GPIOB;
	handlerLEDPB9.GPIO_PinConfig.GPIO_PinNumber			= PIN_9;
	handlerLEDPB9.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerLEDPB9.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerLEDPB9.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerLEDPB9.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;

	// Configuración del pin (PA3) para el USB (RX)
	handlerUSB.pGPIOx									= GPIOA;
	handlerUSB.GPIO_PinConfig.GPIO_PinNumber			= PIN_3;
	handlerUSB.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerUSB.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUSB.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerUSB.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_MEDIUM;
	handlerUSB.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF7;

	// Configuración USART2 USB para RX
	handlerUsartRX.ptrUSARTx							= USART2;
	handlerUsartRX.USART_Config.USART_mode				= USART_MODE_RX;
	handlerUsartRX.USART_Config.USART_baudrate			= USART_BAUDRATE_19200;
	handlerUsartRX.USART_Config.USART_datasize			= USART_DATASIZE_8BIT;
	handlerUsartRX.USART_Config.USART_parity			= USART_PARITY_EVEN;
	handlerUsartRX.USART_Config.USART_stopbits			= USART_STOPBIT_1;
	handlerUsartRX.USART_Config.USART_interrupt			= USART_RX_INTERRUPT_ENABLE;

	// Configuración del timer TIM2
	handlerTIM2.ptrTIMx									= TIM2;
	handlerTIM2.TIM_Config.TIM_mode						= TIM_MODE_UP;
	handlerTIM2.TIM_Config.TIM_speed					= TIM_INCR_SPEED_1ms;
	handlerTIM2.TIM_Config.TIM_period					= 250;

	// Configuración del timer TIM3
	handlerTIM3.ptrTIMx									= TIM3;
	handlerTIM3.TIM_Config.TIM_mode						= TIM_MODE_UP;
	handlerTIM3.TIM_Config.TIM_speed					= TIM_INCR_SPEED_1ms;
	handlerTIM3.TIM_Config.TIM_period					= 300;
}

/******************************| CallBacks |******************************/

// TIM2 Callback
void TIM2_Callback(void){
	handlerStateLED.pGPIOx -> ODR ^= GPIO_ODR_OD5;		// Encendido y apagado StateLED (PA5).
}

// TIM3 Callback
void TIM3_Callback(void){
	handlerLEDPB9.pGPIOx -> ODR ^= GPIO_ODR_OD9;		// Encendido y apagado LED externo (PB9).
}

// USART2 Callback
void USART2_Callback(void){

	// Carga de los datos recibidos.
	dataValue = returnDataUSART2();

	//auxData = dataValue;
	//LEDConfig();
}
