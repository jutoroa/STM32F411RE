/*
 ************************************************************************************************************************************************************
 * @file           : USARxDriver.c
 * @author         : Juan Pablo Toro Arenas y Emmanuel Ángel Maya
 * @brief          : Archivo .c periférico USARTx
 ************************************************************************************************************************************************************
 * En este programa se escribe el código .c necesario para el periférico USARTx
 * Contiene la configuración necesaria para trabajar con el registro USARTx:
 * 	- Mode
 * 	- Baudrate
 * 	- Data size
 * 	- Parity
 * 	- Stop Bits
 ************************************************************************************************************************************************************
 */
#include <stm32f4xx.h>
#include "USARTxDriver.h"

/* Configuración del puerto serial
 * Nota: Siempre se debe comenzar con activar la señal de reloj
 * del periférico que se está utilizando. */
uint8_t dataUSART1;
uint8_t dataUSART2;
uint8_t dataUSART6;

void USART_Config(USART_Handler_t *ptrUsartHandler){

	/* 0. Se desactivan las interrupciones globales */
	__disable_irq();

	/* 1. Activación de la señal de reloj que viene desde el bus al que pertenece el periférico.
	 * Se debe hacer para cada una de las posibles opciones (USART1, USART2, USART6). */
	/* 1.1 Se configura el USART1 */
	if(ptrUsartHandler -> ptrUSARTx == USART1){

		// Se escribe 1 (SET) en la posición correspondiente al USART1.
		RCC -> APB2ENR |= (RCC_APB2ENR_USART1EN);
	}
	/*1.2 Se configura el USART2 */
	else if(ptrUsartHandler -> ptrUSARTx == USART2){

		// Se escribe 1 (SET) en la posición correspondiente al USART2.
		RCC -> APB1ENR |= (RCC_APB1ENR_USART2EN);
	}
	/*1.3 Se configura el USART6 */
	else if(ptrUsartHandler -> ptrUSARTx == USART6){

		// Se escribe 1 (SET) en la posición correspondiente al USART6.
		RCC -> APB2ENR |= (RCC_APB2ENR_USART6EN);
	}

	/* 2. Configuración del tamaño del dato, la paridad y los bits de parada
	 * En el CR1 está parity (PCE y PS) y tamaño del dato (M)
	 * Mientras que en CR2 están los stopbit (STOP)
	 * Configuración del Baudrate (registro BRR)
	 * Condifugración del modo: only TX, only RX, o RXTX
	 * Por último se activa el módulo USART cuando todo está correctamente configurado.
	 */

	/* 2.1 Se empieza por limpiar los registros, para cargar la configuración desde cero. */
	ptrUsartHandler -> ptrUSARTx -> CR1 = 0;
	ptrUsartHandler -> ptrUSARTx -> CR2 = 0;

	/* 2.2 Configuración del Parity: */
	// Se verifica si el parity está activado o no.
	if(ptrUsartHandler -> USART_Config.USART_parity != USART_PARITY_NONE){

		// Se verifica si se ha seleccionado ODD or EVEN.
		// Se selecciona EVEN.
		if(ptrUsartHandler -> USART_Config.USART_parity == USART_PARITY_EVEN){

			// Se escribe 0b0 en la posicion 9 del registo CR1 para escoger Even.
			ptrUsartHandler -> ptrUSARTx -> CR1 &= ~(USART_CR1_PS);

			// Además, se seleccionan 9 bits.
			ptrUsartHandler -> USART_Config.USART_datasize = USART_DATASIZE_9BIT;
			ptrUsartHandler -> ptrUSARTx -> CR1 |= (USART_CR1_M);
		}

		// Se selecciona ODD.
		else{

			// Se escribe 0b1 en la posicion 9 del registo CR1 para escoger Odd.
			ptrUsartHandler -> ptrUSARTx -> CR1 |= (USART_CR1_PS);

			// Además, se seleccionan 9 bits.
			ptrUsartHandler -> USART_Config.USART_datasize = USART_DATASIZE_9BIT;
			ptrUsartHandler -> ptrUSARTx -> CR1 |= (USART_CR1_M);
		}
	}

	// No se desea activar el parity-check.
	else{

		// Se escribe 0b0 en la posicion 10 del registro CR1 para desactivar el Parity..
		ptrUsartHandler -> ptrUSARTx -> CR1 &= ~(USART_CR1_PCE);
	}

	/* 2.3 Configuración del tamaño del dato: */
	// Se verifica si el dato es de 8 o 9 bits.
	if(ptrUsartHandler -> USART_Config.USART_datasize == USART_DATASIZE_8BIT){

		// Se escribe 0b0 en la posicion 12 del registro CR1 para escoger 8 bits.
		ptrUsartHandler -> ptrUSARTx -> CR1 &= ~(USART_CR1_M);
	}else{

		// Se escribe 0b1 en la posicion 12 del registro CR1 para escoger 9 bits.
		ptrUsartHandler -> ptrUSARTx -> CR1 |= (USART_CR1_M);
	}

	/* 2.4 Configuración de los stop bits (SFR USART_CR2). */
	switch(ptrUsartHandler -> USART_Config.USART_stopbits){
	case USART_STOPBIT_1: {

		// Se escribe el valor 0b00 en los dos bits de STOP.
		ptrUsartHandler -> ptrUSARTx -> CR2 &= ~(USART_CR2_STOP);
		break;
	}
	case USART_STOPBIT_0_5:{

		// Se limpia escribiendo el valor 0b00 en los dos bits de STOP.
		ptrUsartHandler -> ptrUSARTx -> CR2 &= ~(USART_CR2_STOP);

		// Se escribe el valor 0b01 en los dos bits de STOP.
		ptrUsartHandler -> ptrUSARTx -> CR2 |= (USART_CR2_STOP_0);
		break;
	}
	case USART_STOPBIT_2:{

		// Se limpia escribiendo el valor 0b00 en los dos bits de STOP.
		ptrUsartHandler -> ptrUSARTx -> CR2 &= ~(USART_CR2_STOP);

		// Se escribe el valor 0b10 en los dos bits de STOP.
		ptrUsartHandler -> ptrUSARTx -> CR2 |= (USART_CR2_STOP_1);
		break;
	}
	case USART_STOPBIT_1_5:{

		// Se limpia escribiendo el valor 0b00 en los dos bits de STOP.
		ptrUsartHandler -> ptrUSARTx -> CR2 &= ~(USART_CR2_STOP);

		// Se escribe el valor 0b11 en los dos bits de STOP.
		ptrUsartHandler -> ptrUSARTx -> CR2 |= (USART_CR2_STOP);
		break;
	}
	default:{

		// Se escribe el valor 0b00 en los dos bits de STOP. Configuración por defecto.
		ptrUsartHandler -> ptrUSARTx -> CR2 &= ~(USART_CR2_STOP);
		break;
	}
	}

	/* 2.4 Configuración del Baudrate (SFR USART_BRR). */
	// Ver tabla de valores (Tabla 75), Frec = 16MHz, overr = 0.
	if(ptrUsartHandler -> USART_Config.USART_baudrate == USART_BAUDRATE_9600){

		// El valor a cargar es 104.1875 -> Mantisa = 104, fracción = 0.1875
		// Mantisa = 104 = 0x68, fracción = 16 * 0.1875 = 3
		// Valor a cargar 0x683
		// Configuración del Baudrate generator para una velocidad de 9600 bps.
		ptrUsartHandler -> ptrUSARTx -> BRR = 0x0683;
	}

	else if(ptrUsartHandler -> USART_Config.USART_baudrate == USART_BAUDRATE_19200){

		// El valor a cargar es 52.0625 -> Mantisa = 52, fracción = 0.0625
		// Mantisa = 52 = 0x34, fracción = 16 * 0.0625 = 1
		// Valor a cargar 0x0341
		// Configuración del Baudrate generator para una velocidad de 19200 bps.
		ptrUsartHandler -> ptrUSARTx -> BRR = 0x0341;
	}

	else if(ptrUsartHandler -> USART_Config.USART_baudrate == USART_BAUDRATE_115200){

		// El valor a cargar es 8.6875 -> Mantisa = 8, fracción = 0.6875
		// Mantisa = 8 = 0x8, fracción = 16 * 0.6875 = 11
		// Valor a cargar 0x0081
		// Configuración del Baudrate generator para una velocidad de 115200bps.
		ptrUsartHandler -> ptrUSARTx -> BRR = 0x0081;
	}

	/* 2.5 Configuración del modo: TX only, RX only, RXTX, disable */
	switch(ptrUsartHandler -> USART_Config.USART_mode){
	case USART_MODE_TX:
	{
		// Se activa la parte del sistema encargada de enviar.
		// Se escribe el valor 0b1 en el bit TE..
		ptrUsartHandler -> ptrUSARTx -> CR1 |= (USART_CR1_TE);
		break;
	}
	case USART_MODE_RX:
	{
		// Se activa la parte del sistema encargada de recibir.
		// Se escribe el valor 0b1 en el bit RE.
		ptrUsartHandler -> ptrUSARTx -> CR1 |= (USART_CR1_RE);
		break;
	}
	case USART_MODE_RXTX:
	{
		// Se activan ambas partes, tanto transmisión como recepción.
		// Se escribe el valor 0b1 en el bit TE y RE respectivamente.
		ptrUsartHandler -> ptrUSARTx -> CR1 |= (USART_CR1_TE);
		ptrUsartHandler -> ptrUSARTx -> CR1 |= (USART_CR1_RE);
		break;
	}
	case USART_MODE_DISABLE:
	{
		// Se desactivan ambos canales.
		// Se escribe el valor 0b0 en el bit TE y RE respectivamente.
		ptrUsartHandler -> ptrUSARTx -> CR1 &= ~(USART_CR1_TE);
		ptrUsartHandler -> ptrUSARTx -> CR1 &= ~(USART_CR1_RE);
		break;
	}
	default:
	{
		// Actuando por defecto, se desactivan ambos canales.
		// Se escribe el valor 0b0 en el bit TE y RE respectivamente.
		ptrUsartHandler -> ptrUSARTx -> CR1 &= ~(USART_CR1_TE);
		ptrUsartHandler -> ptrUSARTx -> CR1 &= ~(USART_CR1_RE);
		break;
	}
	}

	/* 2.6 Activación del modulo serial. */
	if(ptrUsartHandler -> USART_Config.USART_mode != USART_MODE_DISABLE){

		// Se escribe el valor de 0b1 en el bit UE.
		ptrUsartHandler -> ptrUSARTx -> CR1 |= (USART_CR1_UE);
	}

	/* 3. Activación de la interrupción para el USART */
	switch(ptrUsartHandler -> USART_Config.USART_interrupt){
		case USART_RX_INTERRUPT_ENABLE:
		{
			// Se permite que se levante la bandera RX.
			ptrUsartHandler -> ptrUSARTx -> CR1 |= (USART_CR1_RXNEIE);
		/*
			// NO se permite que se levante la bandera TX.
			ptrUsartHandler -> ptrUSARTx -> CR1 &= ~(USART_CR1_TXEIE);
		*/
			break;
		}
		case USART_TX_INTERRUPT_ENABLE:
		{
			// Se permite que se levante la bandera TX.
			ptrUsartHandler -> ptrUSARTx -> CR1 |= (USART_CR1_TXEIE);
			break;
		}
		case USART_RXTX_INTERRUPT_ENABLE:
		{
			// Se permite que se levante la bandera RX.
			ptrUsartHandler -> ptrUSARTx -> CR1 |= (USART_CR1_RXNEIE);

			// Se permite que se levante la bandera TX.
			ptrUsartHandler -> ptrUSARTx -> CR1 |= (USART_CR1_TXEIE);
			break;
		}
		case USART_INTERRUPT_DISABLE:
		{
			// Se desactiva el levantamiento de la bandera RX.
			ptrUsartHandler -> ptrUSARTx -> CR1 &= ~(USART_CR1_RXNEIE);

			// Se desactiva el levantamiento de la bandera TX.
			ptrUsartHandler -> ptrUSARTx -> CR1 &= ~(USART_CR1_TXEIE);
			break;
		}
		default:
		{
			// Actuando por defecto, se desactivan ambos canales.
			ptrUsartHandler -> ptrUSARTx -> CR1 &= ~(USART_CR1_RXNEIE);
			ptrUsartHandler -> ptrUSARTx -> CR1 &= ~(USART_CR1_TXEIE);
			break;
		}
		}

	/* 4. Se registra la interrupción en el NVIC. */
	if(ptrUsartHandler -> ptrUSARTx == USART1){

		// Se activa la IRQ del USART1.
		__NVIC_EnableIRQ(USART1_IRQn);

	}else if(ptrUsartHandler -> ptrUSARTx == USART2){

		// Se activa la IRQ del USART2.
		__NVIC_EnableIRQ(USART2_IRQn);

	}else if(ptrUsartHandler -> ptrUSARTx == USART6){

		// Se activa la IRQ del USART6..
		__NVIC_EnableIRQ(USART6_IRQn);
	}

	// 5. Se activan de nuevo las interrupciones globales.
	__enable_irq();
}

//**********//  CallBacks atributo 'weak' //**********//
// Callback USART1
__attribute__((weak)) void USART1_Callback(void){
	__NOP();
}

// Callback USART2
__attribute__((weak)) void USART2_Callback(void){
	__NOP();
}

// Callback USART6
__attribute__((weak)) void USART6_Callback(void){
	__NOP();
}

/* Hay que hacer dos cosas:
 * 1) Atender la interrupción, bajar la bandera.
 * 2) Llamada al Callback */
/* Rutina de atención a la interrupción del USART */
void USART1_IRQHandler(void){

	// Se lee el dato. Al hacer esto se baja la bandera automáticamente.
	dataUSART1 = (USART1 -> DR);
	USART1_Callback();

/*	// Se baja la bandera manualmente.
	USART1 -> SR &= ~(USART_SR_RXNE); */
}
void USART2_IRQHandler(void){

	// Se lee el dato. Al hacer esto se baja la bandera automáticamente.
	dataUSART2 = (USART2 -> DR);
	USART2_Callback();

/*	// Se baja la bandera manualmente.
	USART2 -> SR &= ~(USART_SR_RXNE); */
}

void USART6_IRQHandler(void){

	// Se lee el dato. Al hacer esto se baja la bandera automáticamente.
	dataUSART6 = (USART6 -> DR);
	USART6_Callback();

/*	// Se baja la bandera manualmente.
	USART6 -> SR &= ~(USART_SR_RXNE); */
}

//**********// Definición de funciones //**********//

/* Función para leer un solo char */
char readChar(USART_Handler_t *ptrUsartHandler){

	// Se lee el valor recibido en el data register.
	char dataToRead;
	dataToRead = ((char) (ptrUsartHandler -> ptrUSARTx -> DR));
	return dataToRead;
}

/* Información leída USART1 */
uint8_t returnDataUSART1(void){

	// Se retorna el valor leído por el USART1.
	return dataUSART1;
}

/* Información leída USART2 */
uint8_t returnDataUSART2(void){

	// Se retorna el valor leído por el USART2.
	return dataUSART2;
}

/* Información leída USART6 */
uint8_t returnDataUSART6(void){

	// Se retorna el valor leído por el USART6.
	return dataUSART2;
}

/* Función para escribir un solo char */
int writeChar(USART_Handler_t *ptrUsartHandler, int dataToSend){

	/*while(!(ptrUsartHandler -> ptrUSARTx -> SR & USART_SR_TXE)){
		__NOP();
	}*/

	// Se permite que se levante la bandera del TX para el USART2.
	//ptrUsartHandler -> ptrUSARTx -> CR1 |= (USART_CR1_TXEIE);
	USART2 -> SR |= (USART_SR_TXE);

	// Se carga el valor de dataToSend en el USER DATA Register.
	ptrUsartHandler -> ptrUSARTx -> DR |= (dataToSend);

	//ptrUsartHandler -> ptrUSARTx -> CR1 &= ~(USART_CR1_TXEIE);
	USART2 -> SR &= ~(USART_SR_TXE);		// Se desactiva la bandera para el TX.
	return dataToSend;
}
