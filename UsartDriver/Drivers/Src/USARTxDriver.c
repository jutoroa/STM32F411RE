/*
 * USARTxDriver.c
 *
 *  Created on: 13/04/2022
 *      Author: Juan Pablo
 */

#include <stm32f4xx.h>
#include "USARTxDriver.h"

/*
 * Configurando el puerto serial
 * Recordar que siempre se debe comenzar con activar la señal de reloj
 * del periférico que se está utilizando.
 */

void USART_Config(USART_Handler_t *ptrUsartHandler){
	/* 1. Activamos la señal de reloj que viene desde el bus al que pertenece el periférico.
	 * Lo debemos hacer para cada uno de las posibles opciones que tengamos (USART1, USART2, USART6)
	 */
	/*1.1 Configuramos el USART1*/
	if(ptrUsartHandler -> ptrUSARTx == USART1){
		// Escribimos 1 (SET) en la posición correspondiente al USART1
		RCC -> APB2ENR |= (RCC_APB2ENR_USART1EN);
	}
	/*1.2 Configuramos el USART2*/
	else if(ptrUsartHandler -> ptrUSARTx == USART2){
		// Escribimos 1 (SET) en la posición correspondiente al USART1
		RCC -> APB1ENR |= (RCC_APB1ENR_USART2EN);
	}
	/*1.3 Configuramos el USART6*/
	else if(ptrUsartHandler -> ptrUSARTx == USART6){
		// Escribimos 1 (SET) en la posición correspondiente al USART1
		RCC -> APB2ENR |= (RCC_APB2ENR_USART6EN);
	}

	/* 2. Configuramos el tamaño del dato, la paridad y los bits de parada
	 * En el CR1 están parity (PCE y PS) y tamaño del dato (M)
	 * Mientras que en CR2 están los stopbit (STOP)
	 * Configuración del Baudrate (registro BRR)
	 * Configuramos el modo: only TX, only RX, o RXTX
	 * Por último activamos el modulo USART cuando todo está correctamente configurado
	 * */

	// 2.1 Comienzo por limpiar los registros, para cargar la configuración desde cero.
	ptrUsartHandler -> ptrUSARTx -> CR1 = 0;
	ptrUsartHandler -> ptrUSARTx -> CR2 = 0;

	// 2.2 Configuración del Parity:
	// Verificamos si el parity está activado o no
	if(ptrUsartHandler -> USART_Config.USART_parity != USART_PARITY_NONE){

		// Verificamos si se ha seleccionado ODD or EVEN

		//Se selecciona EVEN
		if(ptrUsartHandler -> USART_Config.USART_parity == USART_PARITY_EVEN){
			//Ponemos 0b0 en la posicion 9 del registo CR1 para escoger Even
			ptrUsartHandler -> ptrUSARTx -> CR1 &= ~(USART_CR1_PS);	//
		}
		//Se selecciona ODD
		else{
			//Ponemos 0b1 en la posicion 9 del registo CR1 para escoger Even
			ptrUsartHandler -> ptrUSARTx -> CR1 |= (USART_CR1_PS);
		}
	}
	//No deseamos activar el parity-check
	else{
		//Ponemos 0b0 en la posicion 10 del registro CR1 para desactivar el Parity
		ptrUsartHandler -> ptrUSARTx -> CR1 &= ~(USART_CR1_PCE);
	}

	// 2.3 Configuramos el tamaño del dato:

	//Verificamos si va a ser de 8 o 9 bits

	if(ptrUsartHandler -> USART_Config.USART_datasize == USART_DATASIZE_8BIT){
		//Ponemos 0b0 en la posicion 12 del registro CR1 para escoger 8 bits
		ptrUsartHandler -> ptrUSARTx -> CR1 &= ~(USART_CR1_M);
	}else{
		//Ponemos 0b1 en la posicion 12 del registro CR1 para escoger 8 bits
		ptrUsartHandler -> ptrUSARTx -> CR1 |= (USART_CR1_M);
	}

	// 2.4 Configuramos los stop bits (SFR USART_CR2)


	switch(ptrUsartHandler -> USART_Config.USART_stopbits){
	case USART_STOPBIT_1: {
		//Cargamos el valor 0b00 en los dos bits de STOP
		ptrUsartHandler -> ptrUSARTx -> CR2 &= ~(USART_CR2_STOP);
		break;
	}
	case USART_STOPBIT_0_5:{
		//Limpiamos: cargamos el valor 0b00 en los dos bits de STOP
		ptrUsartHandler -> ptrUSARTx -> CR2 &= ~(USART_CR2_STOP);

		// Cargamos el valor de 0b01 en los dos bits de STOP
		ptrUsartHandler -> ptrUSARTx -> CR2 |= (USART_CR2_STOP_0);
		break;
	}
	case USART_STOPBIT_2:{
		//Limpiamos: cargamos el valor 0b00 en los dos bits de STOP
		ptrUsartHandler -> ptrUSARTx -> CR2 &= ~(USART_CR2_STOP);

		// Cargamos el valor de 0b10 en los dos bits de STOP
		ptrUsartHandler -> ptrUSARTx -> CR2 |= (USART_CR2_STOP_1);
		break;
	}
	case USART_STOPBIT_1_5: {
		//Limpiamos: cargamos el valor 0b00 en los dos bits de STOP
		ptrUsartHandler -> ptrUSARTx -> CR2 &= ~(USART_CR2_STOP);

		// Cargamos el valor de 0b11 en los dos bits de STOP
		ptrUsartHandler -> ptrUSARTx -> CR2 |= (USART_CR2_STOP);
		break;
	}
	default:{
		//Cargamos el valor 0b00 en los dos bits de STOP
		ptrUsartHandler -> ptrUSARTx -> CR2 &= ~(USART_CR2_STOP);
		break;
	}
	}

	// 2.4 Configuración del Baudrate (SFR USART_BRR)
	// Ver tabla de valores (Tabla 75), Frec = 16MHz, overr = 0;
	if(ptrUsartHandler -> USART_Config.USART_baudrate == USART_BAUDRATE_9600){
		// El valor a cargar es 104.1875 -> Mantiza = 104, fraction = 0.1875
		// Mantiza = 104 = 0x68, fraction = 16 * 0.1875 = 3
		// Valor a cargar 0x683
		// Configurando el Baudrate generator para una velocidad de 9600bps
		ptrUsartHandler -> ptrUSARTx -> BRR = 0x0683;
	}

	else if(ptrUsartHandler -> USART_Config.USART_baudrate == USART_BAUDRATE_19200){
		// El valor a cargar es 52.0625 -> Mantiza = 52, fraction = 0.0625
		// Mantiza = 52 = 0x34, fraction = 16 * 0.0625 = 1
		// Valor a cargar 0x0341
		// Configurando el Baudrate generator para una velocidad de 19200bps
		ptrUsartHandler -> ptrUSARTx -> BRR = 0x0341;
	}

	else if(ptrUsartHandler -> USART_Config.USART_baudrate == USART_BAUDRATE_115200){	//**************************************DUDAAAAAA
		// El valor a cargar es 8.6875 -> Mantiza = 8, fraction = 0.6875
		// Mantiza = 8 = 0x8, fraction = 16 * 0.6875 = 11
		// Valor a cargar 0x0081
		// Configurando el Baudrate generator para una velocidad de 115200bps
		ptrUsartHandler -> ptrUSARTx -> BRR = 0x0081;
	}

	// 2.5 Configuramos el modo: TX only, RX only, RXTX, disable
	switch(ptrUsartHandler -> USART_Config.USART_mode){
	case USART_MODE_TX:
	{
		// Activamos la parte del sistema encargada de enviar
		//Cargamos el valor 0b1 en el bit TE
		ptrUsartHandler -> ptrUSARTx -> CR1 |= (USART_CR1_TE);
		break;
	}
	case USART_MODE_RX:
	{
		// Activamos la parte del sistema encargada de recibir
		//Cargamos el valor 0b1 en el bit RE
		ptrUsartHandler -> ptrUSARTx -> CR1 |= (USART_CR1_RE);
		break;
	}
	case USART_MODE_RXTX:
	{
		// Activamos ambas partes, tanto transmisión como recepción
		//Cargamos el valor 0b1 en el bit TE y RE respectivamente
		ptrUsartHandler -> ptrUSARTx -> CR1 |= (USART_CR1_TE);
		ptrUsartHandler -> ptrUSARTx -> CR1 |= (USART_CR1_RE);
		break;
	}
	case USART_MODE_DISABLE:
	{
		// Desactivamos ambos canales
		//Cargamos el valor 0b0 en el bit TE y RE respectivamente
		ptrUsartHandler -> ptrUSARTx -> CR1 &= ~(USART_CR1_TE);
		ptrUsartHandler -> ptrUSARTx -> CR1 &= ~(USART_CR1_RE);
		break;
	}
	default:
	{
		// Actuando por defecto, desactivamos ambos canales
		//Cargamos el valor 0b0 en el bit TE y RE respectivamente
		ptrUsartHandler -> ptrUSARTx -> CR1 &= ~(USART_CR1_TE);
		ptrUsartHandler -> ptrUSARTx -> CR1 &= ~(USART_CR1_RE);
		break;
	}
	}

	// 2.6 Activamos el modulo serial.

	if(ptrUsartHandler -> USART_Config.USART_mode != USART_MODE_DISABLE){
		//Cargamos el valor de 0b1 en el bit UE 										//******************************** DUDAAAA
		ptrUsartHandler -> ptrUSARTx -> CR1 |= (USART_CR1_UE);
	}

}
/*	Función para escribir un solo char */

int writeChar(USART_Handler_t *ptrUsartHandler, int dataToSend){						//******************************** DUDAAAA
	while(!(ptrUsartHandler -> ptrUSARTx -> SR & USART_SR_TXE)){
		__NOP();
	}

	//Limpiamos el USER DATA Register
	ptrUsartHandler -> ptrUSARTx -> DR &= ~(USART_DR_DR);

	//Cargamos el valor de dataToSend en el USER DATA Register
	ptrUsartHandler -> ptrUSARTx -> DR |= (dataToSend);

	return dataToSend;
}
