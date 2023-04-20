/*
 * Adxl345Driver.c
 *
 *  Created on: Jun 14, 2022
 *      Author: namontoy
 */
#include <MPE280Driver.h>


// Pines SPI
GPIO_Handler_t	handlerSpiSCLK	= {0};
GPIO_Handler_t	handlerSpiMISO	= {0};
GPIO_Handler_t	handlerSpiMOSI	= {0};
GPIO_Handler_t	handlerSpiSS	= {0};

SPI_Handler_t	handlerSpiPort	= {0};

/**/
void mpe280_readData(uint8_t address, uint8_t * ptrBufferData, uint8_t dataSize){
	/* Indicamos que queremos leer */
	address |= READ_OPERATION;

	/* Activamos al esclavo */
	spi_selectSlave(&handlerSpiPort);

	/* Enviamos la dirección desde donde deseamos leer */
	spi_transmit(handlerSpiPort, &address, 1);

	/* recibimos los 6 bytes */
	spi_receive(handlerSpiPort, ptrBufferData, dataSize);

	/* Desactivamos el esclavo */
	spi_unSelectSlave(&handlerSpiPort);
}

/**/
void mpe280_writeData(uint8_t address, uint8_t value){

	uint8_t data[2];

	/* Dirección de memoria que deseamos escribir */
	data[0] = (address & ~READ_OPERATION);

	/* Cargamos el dato que se desea enviar */
	data[1] = value;

	/* Activamos al esclavo */
	spi_selectSlave(&handlerSpiPort);

	/* Enviamos los datos */
	spi_transmit(handlerSpiPort, data, 2);

	/* Desactivamos el esclavo */
	spi_unSelectSlave(&handlerSpiPort);
}

/**/
void mpe280_init(void){
	/* Configuramos el SPI */
	/* GPIO para el SCLK */
	handlerSpiSCLK.pGPIOx	 							= GPIOA;
	handlerSpiSCLK.GPIO_PinConfig.GPIO_PinNumber		= PIN_5;
	handlerSpiSCLK.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerSpiSCLK.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerSpiSCLK.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerSpiSCLK.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_HIGH;
	handlerSpiSCLK.GPIO_PinConfig.GPIO_PinAltFunMode	= AF5;

	GPIO_Config(&handlerSpiSCLK);

	/* GPIO para el MISO */
	handlerSpiMISO.pGPIOx	 							= GPIOA;
	handlerSpiMISO.GPIO_PinConfig.GPIO_PinNumber		= PIN_6;
	handlerSpiMISO.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerSpiMISO.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerSpiMISO.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerSpiMISO.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_HIGH;
	handlerSpiMISO.GPIO_PinConfig.GPIO_PinAltFunMode	= AF5;

	GPIO_Config(&handlerSpiMISO);

	/* GPIO para el MOSI */
	handlerSpiMOSI.pGPIOx	 							= GPIOA;
	handlerSpiMOSI.GPIO_PinConfig.GPIO_PinNumber		= PIN_7;
	handlerSpiMOSI.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerSpiMOSI.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerSpiMOSI.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerSpiMOSI.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_HIGH;
	handlerSpiMOSI.GPIO_PinConfig.GPIO_PinAltFunMode	= AF5;

	GPIO_Config(&handlerSpiMOSI);

	/* GPIO para el SS */
	handlerSpiSS.pGPIOx	 							= GPIOA;
	handlerSpiSS.GPIO_PinConfig.GPIO_PinNumber		= PIN_9;
	handlerSpiSS.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_OUT;
	handlerSpiSS.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerSpiSS.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerSpiSS.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_HIGH;
	handlerSpiSS.GPIO_PinConfig.GPIO_PinAltFunMode	= AF0;

	GPIO_Config(&handlerSpiSS);

	handlerSpiPort.ptrSPIx 							= SPI1;
	handlerSpiPort.SPI_Config.SPI_mode				= SPI_MODE_3;
	handlerSpiPort.SPI_Config.SPI_fullDupplexEnable	= SPI_FULL_DUPPLEX;
	handlerSpiPort.SPI_Config.SPI_datasize			= SPI_DATASIZE_8_BIT;
	handlerSpiPort.SPI_Config.SPI_baudrate			= SPI_BAUDRATE_FPCLK_16; // queda a 1MHz
	handlerSpiPort.SPI_slavePin 					= handlerSpiSS;

	spi_config(handlerSpiPort);
}

