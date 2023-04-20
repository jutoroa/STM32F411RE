/*
 * USARTxDriver.c
 *
 *  Created on: Apr 6, 2022
 *      Author: namontoy
 */

#include <stm32f4xx.h>
#include "USARTxDriver.h"
#include "PLLDriver.h"

uint8_t auxRxData = 0;
//USART_Handler_t *ptrAuxUsartHandler = {0};



/**
 * Configurando el puerto Serial...
 * Recordar que siempre se debe comenzar con activar la señal de reloj
 * del periferico que se está utilizando.
 */
void USART_Config(USART_Handler_t *ptrUsartHandler){

	//*ptrAuxUsartHandler = *ptrUsartHandler;

	/* 0. Desactivamos las interrupciones globales mientras configuramos el sistema.*/
	__disable_irq();

	/* 1. Activamos la señal de reloj que viene desde el BUS al que pertenece el periférico */
	/* Lo debemos hacer para cada uno de las posibles opciones que tengamos (USART1, USART2, USART6) */
	if(ptrUsartHandler->ptrUSARTx == USART1){
		RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	}

	else if(ptrUsartHandler->ptrUSARTx == USART2){
			RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
		}

	else if(ptrUsartHandler->ptrUSARTx == USART6){
		RCC->APB2ENR |= RCC_APB2ENR_USART6EN;
	}

	/* 2. Configuramos el tamaño del dato, la paridad y los bit de parada */
	/* En el CR1 estan parity (PCE y PS) y tamaño del dato (M) */
	/* Mientras que en CR2 estan los stopbit (STOP)*/
	/* Configuracion del Baudrate (registro BRR) */
	/* Configuramos el modo: only TX, only RX, o RXTX */
	/* Por ultimo activamos el modulo USART cuando todo esta correctamente configurado */

	// 2.1 Comienzo por limpiar los registros, para cargar la configuración desde cero
	ptrUsartHandler->ptrUSARTx->CR1 = 0;
	ptrUsartHandler->ptrUSARTx->CR2 = 0;

	// Limpiamos el registro DR
	//ptrUsartHandler->ptrUSARTx->DR = 0;

	// 2.2 Configuracion del Parity:
	// Verificamos si el parity esta activado o no
	if(ptrUsartHandler->USART_Config.USART_parity != USART_PARITY_NONE){

		// Verificamos si se ha seleccionado ODD or EVEN
		if(ptrUsartHandler->USART_Config.USART_parity == USART_PARITY_EVEN){
			// Es even, entonces cargamos la configuracion adecuada
			ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_PCE;
			ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_PS;

		}else{
			// Si es "else" significa que la paridad seleccionada es ODD, y cargamos esta configuracion
			ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_PCE;
			ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_PS;
		}
	}else{
		// Si llegamos aca, es porque no deseamos tener el parity-check
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_PCE;
	}

	// 2.3 Configuramos el tamaño del dato
	// Verificamos cual es el tamaño de dato que deseamos
	if(ptrUsartHandler->USART_Config.USART_datasize == USART_DATASIZE_8BIT){

		// Verificamos si se esta trabajando o no con paridad
		if(ptrUsartHandler->USART_Config.USART_parity == USART_PARITY_NONE){
			// Deseamos trabajar con datos de 8 bits
			ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_M;
		}
		else{
			// Si la paridad esta activa, debemos incluir un bit adicional
			ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_M;
		}

	}else{
		// Deseamos trabajar con datos de 9 bits
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_M;
	}

	// 2.4 Configuramos los stop bits (SFR USART_CR2)
	switch(ptrUsartHandler->USART_Config.USART_stopbits){
	case USART_STOPBIT_1: {
		// Debemoscargar el valor 0b00 en los dos bits de STOP
		ptrUsartHandler->ptrUSARTx->CR2 &= ~USART_CR2_STOP_0;
		ptrUsartHandler->ptrUSARTx->CR2 &= ~USART_CR2_STOP_1;
		break;
	}
	case USART_STOPBIT_0_5: {
		// Debemoscargar el valor 0b01 en los dos bits de STOP
		ptrUsartHandler->ptrUSARTx->CR2 |= USART_CR2_STOP_0;
		ptrUsartHandler->ptrUSARTx->CR2 &= ~USART_CR2_STOP_1;
		break;
	}
	case USART_STOPBIT_2: {
		// Debemoscargar el valor 0b10 en los dos bits de STOP
		ptrUsartHandler->ptrUSARTx->CR2 &= ~USART_CR2_STOP_0;
		ptrUsartHandler->ptrUSARTx->CR2 |= USART_CR2_STOP_1;
		break;
	}
	case USART_STOPBIT_1_5: {
		// Debemoscargar el valor 0b11 en los dos bits de STOP
		ptrUsartHandler->ptrUSARTx->CR2 |= USART_CR2_STOP_0;
		ptrUsartHandler->ptrUSARTx->CR2 |= USART_CR2_STOP_1;
		break;
	}
	default: {
		// En el casopor defecto seleccionamos 1 bit de parada
		ptrUsartHandler->ptrUSARTx->CR2 &= ~USART_CR2_STOP_0;
		ptrUsartHandler->ptrUSARTx->CR2 &= ~USART_CR2_STOP_1;
		break;
	}
	}

	// 2.5 Configuracion del Baudrate (SFR USART_BRR)
	// Ver tabla de valores (Tabla 73), Frec = 16MHz, overr = 0;

	// Caso para configurar cuando se trabaja con el Cristal Interno
	switch (getConfigMianClock()) {
	case HSI_CLOCK_CONFIGURED:
	{
		switch(ptrUsartHandler->USART_Config.USART_baudrate){
		case USART_BAUDRATE_9600:
		{
			// El valor a cargar es 104.1875 -> Mantiza = 104,fraction = 0.1875
			// Mantiza = 104 = 0x68, fraction = 16 * 0.1875 = 3
			// Valor a cargar 0x0683
			// Configurando el Baudrate generator para una velocidad de 9600bps
			ptrUsartHandler->ptrUSARTx->BRR = 0x0683;
			break;
		}
		case USART_BAUDRATE_19200:
		{
			// El valor a cargar es 52.0625 -> Mantiza = 52,fraction = 0.0625
			// Mantiza = 52 = 0x34, fraction = 16 * 0.1875 = 1
			// Valor a cargar 0x0341
			// Configurando el Baudrate generator para una velocidad de 19200bps
			ptrUsartHandler->ptrUSARTx->BRR = 0x0341;
			break;
		}
		case USART_BAUDRATE_115200:
		{
			// El valor a cargar es 8.6875 -> Mantiza = 8,fraction = 0.6875
			// Mantiza = 8 = 0x08, fraction = 16 * 0.6875 = 11 = 0xB
			// Valor a cargar 0x008B
			// Configurando el Baudrate generator para una velocidad de 115200bps
			ptrUsartHandler->ptrUSARTx->BRR = 0x008B;
			break;
		}
		case USART_BAUDRATE_230400:
		{
			// Configurando el Baudrate generator para una velocidad de 230400bps
			ptrUsartHandler->ptrUSARTx->BRR = 0x0045;
			break;
		}
		case USART_BAUDRATE_921600:
		{
			// Configurando el Baudrate generator para una velocidad de 921600bps
			ptrUsartHandler->ptrUSARTx->BRR = 0x0011;
			break;
		}

		default:
			// Configurando el Baudrate generator para una velocidad de 115200bps
			ptrUsartHandler->ptrUSARTx->BRR = 0x008B;
			break;
		}

		break;
	}

	// Caso para configurar cuando se trabaja con el Cristal Externo
	case HSE_CLOCK_CONFIGURED:
	{
		switch(ptrUsartHandler->USART_Config.USART_baudrate){
		case USART_BAUDRATE_9600:
		{
			break;
		}
		case USART_BAUDRATE_19200:
		{
			break;
		}
		case USART_BAUDRATE_115200:
		{
			break;
		}
		case USART_BAUDRATE_230400:
		{
			break;
		}
		case USART_BAUDRATE_921600:
		{
			break;
		}

		default:
			break;
		}
		break;
	}

	// Caso para configurar cuando se trabaja con el PLL
	case PLL_CLOCK_CONFIGURED:
	{

		// Configuracion para los USART en el BUS APB2 (solo el USART2)
		if(ptrUsartHandler->ptrUSARTx == USART2){
			switch(ptrUsartHandler->USART_Config.USART_baudrate){
			case USART_BAUDRATE_9600:
			{
				// El valor a cargar es 104.1875 -> Mantiza = 104,fraction = 0.1875
				// Mantiza = 104 = 0x68, fraction = 16 * 0.1875 = 3
				// Valor a cargar 0x0683
				// Configurando el Baudrate generator para una velocidad de 9600bps
				ptrUsartHandler->ptrUSARTx->BRR = 0x1458;
				break;
			}
			case USART_BAUDRATE_19200:
			{
				// El valor a cargar es 52.0625 -> Mantiza = 52,fraction = 0.0625
				// Mantiza = 52 = 0x34, fraction = 16 * 0.1875 = 1
				// Valor a cargar 0x0341
				// Configurando el Baudrate generator para una velocidad de 19200bps
				ptrUsartHandler->ptrUSARTx->BRR = 0x0A2C;
				break;
			}
			case USART_BAUDRATE_115200:
			{
				// El valor a cargar es 27.1267 -> Mantiza = 27,fraction = 0.1267
				// Mantiza = 27 = 0x1B, fraction = 16 * 0.1267 = 2 = 0x2
				// Valor a cargar 0x01B2
				// Configurando el Baudrate generator para una velocidad de 115200bps
				ptrUsartHandler->ptrUSARTx->BRR = 0x01B2;
				break;
			}
			case USART_BAUDRATE_230400:
			{
				// Configurando el Baudrate generator para una velocidad de 230400bps
				ptrUsartHandler->ptrUSARTx->BRR = 0x00D9;
				break;
			}
			case USART_BAUDRATE_921600:
			{
				// Configurando el Baudrate generator para una velocidad de 921600bps
				ptrUsartHandler->ptrUSARTx->BRR = 0x0036;
				break;
			}

			default:
				// Configurando el Baudrate generator para una velocidad de 115200bps
				ptrUsartHandler->ptrUSARTx->BRR = 0x01B2;
				break;
			}

		}// Fin del if para bus APB2

		// Configuracion para los USART en el BUS APB1 (USART1 y USART6)
		else{
			switch(ptrUsartHandler->USART_Config.USART_baudrate){
			case USART_BAUDRATE_9600:
			{
				// Configurando el Baudrate generator para una velocidad de 9600bps
				ptrUsartHandler->ptrUSARTx->BRR = 0x28B1;
				break;
			}
			case USART_BAUDRATE_19200:
			{
				// Configurando el Baudrate generator para una velocidad de 19200bps
				ptrUsartHandler->ptrUSARTx->BRR = 0x1458;
				break;
			}
			case USART_BAUDRATE_115200:
			{
				// Configurando el Baudrate generator para una velocidad de 115200bps
				ptrUsartHandler->ptrUSARTx->BRR = 0x0364;
				break;
			}
			case USART_BAUDRATE_230400:
			{
				// Configurando el Baudrate generator para una velocidad de 230400bps
				ptrUsartHandler->ptrUSARTx->BRR = 0x0364;
				break;
			}
			case USART_BAUDRATE_921600:
			{
				// Configurando el Baudrate generator para una velocidad de 921600bps
				ptrUsartHandler->ptrUSARTx->BRR = 0x01B2;
				break;
			}

			default:
				// Configurando el Baudrate generator para una velocidad de 115200bps
				ptrUsartHandler->ptrUSARTx->BRR = 0x006C;
				break;
			}

		}// Fin del else para bus APAB1

		break;
	}// fin del switch-case externo

	default:
		break;
	}


	// 2.6 Configuramos el modo: TX only, RX only, RXTX, disable
	switch(ptrUsartHandler->USART_Config.USART_mode){
	case USART_MODE_TX:
	{
		// Activamos la parte del sistema encargada de enviar
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_TE;
		break;
	}
	case USART_MODE_RX:
	{
		// Activamos la parte del sistema encargada de recibir
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_RE;
		break;
	}
	case USART_MODE_RXTX:
	{
		// Activamos ambas partes, tanto transmision como recepcion
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_RE;
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_TE;
		break;
	}
	case USART_MODE_DISABLE:
	{
		// Desactivamos ambos canales
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_RE;
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_TE;
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_UE;
		break;
	}
	default:
	{
		// Actuando por defecto, desactivamos ambos canales
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_RE;
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_TE;
		ptrUsartHandler->ptrUSARTx->CR1 &= ~USART_CR1_UE;
		break;
	}
	}

	// 2.8 Verificamos la configuración de las interrupciones
	// 2.8a Interrupción por recepción
	if(ptrUsartHandler->USART_Config.USART_enableIntRX == USART_RX_INTERRUP_ENABLE){
		// Como está activada, debemos configurar la interrupción por recepción
		/* Debemos activar la interrupción RX en la configuración del USART */
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_RXNEIE;

		/* Debemos matricular la interrupción en el NVIC */
		/* Lo debemos hacer para cada uno de las posibles opciones que tengamos (USART1, USART2, USART6) */
		if(ptrUsartHandler->ptrUSARTx == USART1){
			__NVIC_EnableIRQ(USART1_IRQn);
			__NVIC_SetPriority(USART1_IRQn, 2);
		}

		else if(ptrUsartHandler->ptrUSARTx == USART2){
			__NVIC_EnableIRQ(USART2_IRQn);
			__NVIC_SetPriority(USART2_IRQn, 2);
			}

		else if(ptrUsartHandler->ptrUSARTx == USART6){
			__NVIC_EnableIRQ(USART6_IRQn);
			__NVIC_SetPriority(USART6_IRQn, 2);
		}
	}
	else{

	}

	// 2.7 Activamos el modulo serial.
	if(ptrUsartHandler->USART_Config.USART_mode != USART_MODE_DISABLE){
		ptrUsartHandler->ptrUSARTx->CR1 |= USART_CR1_UE;
	}

	/* x. Volvemos a activar las interrupciones del sistema */
	__enable_irq();
}

/* función para escribir un solo char */
int writeChar(USART_Handler_t *ptrUsartHandler, int dataToSend ){
	while( !(ptrUsartHandler->ptrUSARTx->SR & USART_SR_TXE)){
		__NOP();
	}

	ptrUsartHandler->ptrUSARTx->DR = dataToSend;

	return dataToSend;
}

/**/
void writeMsg(USART_Handler_t *ptrUsartHandler, char *msgToSend ){
	while(*msgToSend != '\0'){
		writeChar(ptrUsartHandler, *msgToSend);
		msgToSend++;
	}
}

uint8_t getRxData(void){
	return auxRxData;
}

/* Handler de la interrupción del USART
 * Acá deben estar todas las interrupciones asociadas: TX, RX, PE...
 */
void USART2_IRQHandler(void){
	// Evaluamos si la interrupción que se dio es por RX
	if(USART2->SR & USART_SR_RXNE){
		auxRxData = (uint8_t) USART2->DR;
		usart2Rx_Callback();
	}
}

/* Handler de la interrupción del USART
 * Acá deben estar todas las interrupciones asociadas: TX, RX, PE...
 */
void USART6_IRQHandler(void){
	// Evaluamos si la interrupción que se dio es por RX
	if(USART6->SR & USART_SR_RXNE){
		auxRxData = (uint8_t) USART6->DR;
		usart6Rx_Callback();
	}
}

/* Handler de la interrupción del USART
 * Acá deben estar todas las interrupciones asociadas: TX, RX, PE...
 */
void USART1_IRQHandler(void){
	// Evaluamos si la interrupción que se dio es por RX
	if(USART1->SR & USART_SR_RXNE){
		auxRxData = (uint8_t) USART1->DR;
		usart1Rx_Callback();
	}
}


__attribute__((weak)) void usart1Rx_Callback(void){
	  /* NOTE : This function should not be modified, when the callback is needed,
	            the BasicTimer_Callback could be implemented in the main file
	   */
	__NOP();
}

__attribute__((weak)) void usart2Rx_Callback(void){
	  /* NOTE : This function should not be modified, when the callback is needed,
	            the BasicTimer_Callback could be implemented in the main file
	   */
	__NOP();
}

__attribute__((weak)) void usart6Rx_Callback(void){
	  /* NOTE : This function should not be modified, when the callback is needed,
	            the BasicTimer_Callback could be implemented in the main file
	   */
	__NOP();
}
