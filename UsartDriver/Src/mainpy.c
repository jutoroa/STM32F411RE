/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Juan Pablo Toro Arenas
 * @brief          : Desarrollo del driver para el USART
 ******************************************************************************
 * - Implementación de las liberías CMSIS
 * - Migración del GPIO driver
 * - Desarrollo de las librerías para el manejo del USART
 ******************************************************************************
 */


#include <stm32f4xx.h>
#include <stdint.h>
#include "GPIOxDriver.h"
#include "USARTxDriver.h"

/* Variables del proyecto */

	GPIO_Handler_t handlerPinUSB = {0};
	USART_Handler_t handlerUSARTCharTest01 = {0};

// *************** // Headers // *************** //

void delay(int time);
void pytry(int n_times);

// *************** // Main // *************** //
int main(void)
{

	//	Configuramos el PIN
	handlerPinUSB.pGPIOx								= GPIOA;
	handlerPinUSB.GPIO_PinConfig.GPIO_PinNumber			= PIN_2;
	handlerPinUSB.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerPinUSB.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerPinUSB.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPinUSB.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerPinUSB.GPIO_PinConfig.GPIO_PinAltFunMode 	= AF7;

	// Configuramos la información para mandar en el USART

	handlerUSARTCharTest01.ptrUSARTx					= USART2;
	handlerUSARTCharTest01.USART_Config.USART_mode		= USART_MODE_TX;
	handlerUSARTCharTest01.USART_Config.USART_baudrate	= USART_BAUDRATE_19200;
	handlerUSARTCharTest01.USART_Config.USART_datasize	= USART_DATASIZE_8BIT;
	handlerUSARTCharTest01.USART_Config.USART_parity	= USART_PARITY_NONE;
	handlerUSARTCharTest01.USART_Config.USART_stopbits	= USART_STOPBIT_1;


	//Cargamos la configuración
	USART_Config(&handlerUSARTCharTest01);
	GPIO_Config(&handlerPinUSB);

    /* Ciclo principal */
	while(1){

		pytry(5);
		delay(10000);
	}

}

//***********// Definición de Funciones //***********//

void pytry(int n_times){

	for(int i = 0; i <= n_times; i++){
		writeChar(&handlerUSARTCharTest01, 'i');
	}
	for(int i = n_times; i >= 0; i--){
		writeChar(&handlerUSARTCharTest01, i);
	}
}

void delay(int time){
	// Con este ciclo se genera un intervalo de tiempo en el programa (no tiene otro objetivo)
	for (int i = 0; i <= time; i++){
		__NOP();
	}
}

