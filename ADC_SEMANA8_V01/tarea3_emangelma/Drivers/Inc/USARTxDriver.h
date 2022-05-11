/*
 ************************************************************************************************************************************************************
 * @file           : USARxDriver.h
 * @author         : Juan Pablo Toro Arenas y Emmanuel Ángel Maya
 * @brief          : Archivo .h periférico USARTx
 ************************************************************************************************************************************************************
 * Este programa contiene la documentación necesaria para trabajar con el perfiérico USARTx.
 ************************************************************************************************************************************************************
 */
#include <stdio.h>

#ifndef INC_USARTXDRIVER_H_
#define INC_USARTXDRIVER_H_

#define USART_MODE_TX					0
#define USART_MODE_RX					1
#define USART_MODE_RXTX					2
#define USART_MODE_DISABLE				3

#define USART_RX_INTERRUPT_ENABLE		1
#define USART_TX_INTERRUPT_ENABLE		2
#define USART_RXTX_INTERRUPT_ENABLE		3
#define USART_RX_INTERRUPT_DISABLE		0
#define USART_TX_INTERRUPT_DISABLE		0
#define USART_RXTX_INTERRUPT_DISABLE	0
#define USART_INTERRUPT_DISABLE			0

#define USART_BAUDRATE_9600				0
#define USART_BAUDRATE_19200			1
#define USART_BAUDRATE_115200			2

#define USART_DATASIZE_8BIT				0
#define USART_DATASIZE_9BIT				1

#define USART_PARITY_NONE 				0
#define USART_PARITY_ODD				1
#define USART_PARITY_EVEN				2

#define USART_STOPBIT_1					0
#define USART_STOPBIT_0_5				1
#define USART_STOPBIT_2					2
#define USART_STOPBIT_1_5				3

/* Estructura para la configuración de la comunicación:
 * 	- Velocidad (baudrate)
 * 	- Tamaño de los datos
 * 	- Control de errores
 * 	- Bit de parada
*/

typedef struct
{
	uint8_t USART_mode;
	uint8_t USART_baudrate;
	uint8_t USART_datasize;
	uint8_t USART_parity;
	uint8_t USART_stopbits;
	uint8_t USART_interrupt;

}USART_Config_t;

/*Definicion del handler para un USART:
 *  - Estructura que contiene los SFR que controlan el periferico
 *  - Estructura que contiene la configuración especifica del objeto
 *  - Buffer de recepcion de datos
 *  - Elemento que indica cuántos datos se recibieron
 *  - Buffer de transmisión de datos
 *  - Elemento que indica cuántos datos se deben enviar
 */

typedef struct
{
	USART_TypeDef	*ptrUSARTx;
	USART_Config_t	USART_Config;
	uint8_t			receptionBuffer[64];
	uint8_t			dataInputSize;
	uint8_t			transmisionBuffer[64];
	uint8_t			dataOutputSize;

}USART_Handler_t;

/* Definicion de los prototipos para las funciones del USART */
void USART_Config(USART_Handler_t *ptrUsartHandler);

/* Callbacks */
void USART1_Callback(void);
void USART2_Callback(void);
void USART6_Callback(void);

/* Función que lee un solo char */
char readChar(USART_Handler_t *ptrUsartHandler);

/* Función que escribe un solo char */
int writeChar(USART_Handler_t *ptrUsartHandler, int dataToSend);

/* Funciones que retornan información para cada USARTx */
uint8_t returnDataUSART1(void);
uint8_t returnDataUSART2(void);
uint8_t returnDataUSART6(void);

#endif /* INC_USARTXDRIVER_H_ */
