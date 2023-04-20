/*
 * SpiDriver.h
 *
 *  Created on: Jun 13, 2022
 *      Author: namontoy
 */

#ifndef SPIDRIVER_H_
#define SPIDRIVER_H_

#include "stm32f4xx.h"
#include "stdint.h"
#include "GPIOxDriver.h"

#define SPI_MODE_0		0
#define SPI_MODE_1		1
#define SPI_MODE_2		2
#define SPI_MODE_3		3

#define SPI_BAUDRATE_FPCLK_2		0
#define SPI_BAUDRATE_FPCLK_4		1
#define SPI_BAUDRATE_FPCLK_8		2
#define SPI_BAUDRATE_FPCLK_16		3
#define SPI_BAUDRATE_FPCLK_32		4
#define SPI_BAUDRATE_FPCLK_64		5
#define SPI_BAUDRATE_FPCLK_128		6
#define SPI_BAUDRATE_FPCLK_256		7

#define SPI_RECEIVE_ONLY	0
#define SPI_FULL_DUPPLEX	1

#define SPI_DATASIZE_8_BIT		0
#define SPI_DATASIZE_16_BIT		1

/**/
typedef struct
{
	uint8_t SPI_mode;				// Define los 4 diferentes modos que puede configurar el SPI.
	uint8_t SPI_baudrate;			// Define la velocidad que maneja el SPI con respecto al PCLK (Peripheral clock)
	uint8_t SPI_fullDupplexEnable;	// Configura si solo recepción o bi-direccional
	uint8_t	SPI_datasize;
	uint8_t	SPI_enableIntRX;
	uint8_t	SPI_enableIntTX;
}SPI_Config_t;

/**/
typedef struct
{
	SPI_TypeDef		*ptrSPIx;
	SPI_Config_t	SPI_Config;
	GPIO_Handler_t	SPI_slavePin;
}SPI_Handler_t;

/* Prototipos de las funciones */
void spi_config(SPI_Handler_t ptrHandlerSPI);
void spi_transmit(SPI_Handler_t ptrHandlerSPI, uint8_t * ptrData, uint32_t dataSize);
void spi_receive(SPI_Handler_t ptrHandlerSPI, uint8_t * ptrData, uint32_t dataSize);
void spi_selectSlave(SPI_Handler_t* ptrHandlerSPI);
void spi_unSelectSlave(SPI_Handler_t* ptrHandlerSPI);

#endif /* SPIDRIVER_H_ */
