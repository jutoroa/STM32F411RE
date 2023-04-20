/*
 * SpiDriver.c
 *
 *  Created on: Jun 13, 2022
 *      Author: namontoy
 *
 * Recordar que debemos configurar los pines GPIO de forma que coincidan
 */

#include "SpiDriver.h"

/**/
void spi_config(SPI_Handler_t ptrHandlerSPI){
	/* 1. Activamos la señal de reloj del periférico */
	if(ptrHandlerSPI.ptrSPIx == SPI1){
		RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	}
	else if(ptrHandlerSPI.ptrSPIx == SPI2){
		RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
	}
	else if(ptrHandlerSPI.ptrSPIx == SPI3){
		RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;
	}
	else if(ptrHandlerSPI.ptrSPIx == SPI4){
		RCC->APB2ENR |= RCC_APB2ENR_SPI4EN;
	}
	else if(ptrHandlerSPI.ptrSPIx == SPI5){
		RCC->APB2ENR |= RCC_APB2ENR_SPI5EN;
	}

	/* 2. Limpiamos el registro de configuración para comenzar de cero */
	ptrHandlerSPI.ptrSPIx->CR1 = 0x00;

	/* 3. Configuramos la velocidad a la que se maneja el reloj del SPI */
	ptrHandlerSPI.ptrSPIx->CR1 |= (ptrHandlerSPI.SPI_Config.SPI_baudrate << SPI_CR1_BR_Pos);

	/* 4. Configuramos el modo */
	switch(ptrHandlerSPI.SPI_Config.SPI_mode){
	case 0:
		ptrHandlerSPI.ptrSPIx->CR1 &= ~(SPI_CR1_CPHA);
		ptrHandlerSPI.ptrSPIx->CR1 &= ~(SPI_CR1_CPOL);
		break;

	case 1:
		ptrHandlerSPI.ptrSPIx->CR1 |=  (SPI_CR1_CPHA);
		ptrHandlerSPI.ptrSPIx->CR1 &= ~(SPI_CR1_CPOL);
		break;

	case 2:
		ptrHandlerSPI.ptrSPIx->CR1 &= ~(SPI_CR1_CPHA);
		ptrHandlerSPI.ptrSPIx->CR1 |=  (SPI_CR1_CPOL);
		break;

	case 3:
		ptrHandlerSPI.ptrSPIx->CR1 |= (SPI_CR1_CPHA);
		ptrHandlerSPI.ptrSPIx->CR1 |= (SPI_CR1_CPOL);
		break;

	default:
		// Configuramos el mode_00 como caso por defecto
		ptrHandlerSPI.ptrSPIx->CR1 &= ~(SPI_CR1_CPHA);
		ptrHandlerSPI.ptrSPIx->CR1 &= ~(SPI_CR1_CPOL);
		break;
	}

	/* 5. Configuramos si fullDupplex a solo recepción */
	if(ptrHandlerSPI.SPI_Config.SPI_fullDupplexEnable == SPI_FULL_DUPPLEX){
		// Selecciona full-dupplex
		ptrHandlerSPI.ptrSPIx->CR1 &= ~SPI_CR1_RXONLY;
		// Selecciona modo uni-direccional (cada línea solo tiene una dirección)
		ptrHandlerSPI.ptrSPIx->CR1 &= ~SPI_CR1_BIDIMODE;
	}
	else{
		// Selecciona solo RX activado
		ptrHandlerSPI.ptrSPIx->CR1 |= SPI_CR1_RXONLY;
	}

	/* 6. Modo de transferencia en MSB-first */
	ptrHandlerSPI.ptrSPIx->CR1 &= ~SPI_CR1_LSBFIRST;

	/* 7. Activamos para que el equipo se comporte como el maestro de la red*/
	ptrHandlerSPI.ptrSPIx->CR1 |= SPI_CR1_MSTR;

	/* 8. Configuramos el formato del dato (tamaño) para que sea de 8-bit*/
	if(ptrHandlerSPI.SPI_Config.SPI_datasize == SPI_DATASIZE_8_BIT){
		// framde de datos de 8-bit
		ptrHandlerSPI.ptrSPIx->CR1 &= ~SPI_CR1_DFF;
	}else{
		// framde de datos de 16-bit
		ptrHandlerSPI.ptrSPIx->CR1 |= SPI_CR1_DFF;
	}

	/* 9. Configuramos para que el control del pin SS (selección del slave
	 *    sea controlado por software (nosotros debemos hacer ese control),
	 *    de la otra forma, será el hardware el que controla la selección del slave */
	ptrHandlerSPI.ptrSPIx->CR1 |= SPI_CR1_SSM;
	ptrHandlerSPI.ptrSPIx->CR1 |= SPI_CR1_SSI;

	/* 10. Activamos el periférico SPI */
	ptrHandlerSPI.ptrSPIx->CR1 |= SPI_CR1_SPE;

}

/**/
void spi_transmit(SPI_Handler_t ptrHandlerSPI, uint8_t * ptrData, uint32_t dataSize){

	uint8_t auxData;
	(void) auxData;

	while(dataSize > 0){

		// Esperamos a que el buffer esté vacío
		while(!(ptrHandlerSPI.ptrSPIx->SR & SPI_SR_TXE)){
			__NOP();
		}

		// Enviamos el dato al que apunta el puntero
		ptrHandlerSPI.ptrSPIx->DR = (uint8_t) (0xFF & *ptrData);

		// Actualizamos el puntero y el número de datos que faltan por enviar
		ptrData++;
		dataSize--;
	}

	// Esperamos de nuevo a que el buffer esté vacío
	while (!(ptrHandlerSPI.ptrSPIx->SR & SPI_SR_TXE)) {
		__NOP();
	}

	// Esperamos a que la bandera de ocupado (busy) baje (observar que la lógica cambia)
	while ((ptrHandlerSPI.ptrSPIx->SR & SPI_SR_BSY)) {
		__NOP();
	}

	/* Debemos limpiar la bandera de OverRun (que a veces se levanta).
	 * Para esto debemos leer el DR y luego leer el SR del módulo SPI (pag 599) */
	auxData = ptrHandlerSPI.ptrSPIx->DR;
	auxData = ptrHandlerSPI.ptrSPIx->SR;
}


/**/
void spi_receive(SPI_Handler_t ptrHandlerSPI, uint8_t * ptrData, uint32_t dataSize){

	while(dataSize){
		// Esperamos de nuevo a que el buffer esté vacío
		while (!(ptrHandlerSPI.ptrSPIx->SR & SPI_SR_TXE)) {
			__NOP();
		}
		// Enviamos un valor dummy
		ptrHandlerSPI.ptrSPIx->DR = 0x00;

		// Esperamos de nuevo a que el buffer tenga un dato que leer
		while (!(ptrHandlerSPI.ptrSPIx->SR & SPI_SR_RXNE)) {
			__NOP();
		}

		// Cargamos el valor en el puntero
		*ptrData = ptrHandlerSPI.ptrSPIx->DR;

		// Actualizamos el puntero y el tamaño de los datos
		ptrData++;
		dataSize--;

	}
}

/* Seleccionamos el esclavo llevando el pin SS a GND */
void spi_selectSlave(SPI_Handler_t* ptrHandlerSPI){
	GPIO_WritePin(&ptrHandlerSPI->SPI_slavePin, RESET);
	//GPIOA->BSRR |= (SET << 25);
}

/* Seleccionamos el esclavo llevando el pin SS a Vcc */
void spi_unSelectSlave(SPI_Handler_t* ptrHandlerSPI){
	GPIO_WritePin(&ptrHandlerSPI->SPI_slavePin, SET);
	//GPIOA->BSRR |= (SET << 9);
}




