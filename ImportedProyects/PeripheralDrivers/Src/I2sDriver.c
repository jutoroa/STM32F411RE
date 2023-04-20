/*
 * I2sDriver.c
 *
 *  Created on: Nov 16, 2022
 *      Author: namontoy
 */

#include "I2sDriver.h"

int16_t auxAudioData = 0;
int32_t tempAudioData;
int32_t audioData;
uint8_t counterData = 0;
I2S_Handler_t * handlerMic;
int16_t debugData[64] = {0};

/*
 * Configura al periferico I2S con respecto a los datos entregados en el handler.
 *
 */
void i2s_config(I2S_Handler_t *ptrHandlerI2S) {

	uint32_t auxConfig = 0;
	(void)auxConfig;
	// Creo un puntero a los datos de la configuracion
	handlerMic = ptrHandlerI2S;

	/* 1. Activamos la señal de reloj del periférico.
	 * Estamos activando el modulo SPI porque el I2S comparte los mismos
	 * registros y posiciones de memoria que el SPI
	 */

	if (ptrHandlerI2S->ptrSPIx == SPI1) {
		RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

	} else if (ptrHandlerI2S->ptrSPIx == SPI2) {
		RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

	} else if (ptrHandlerI2S->ptrSPIx == SPI3) {
		RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;

	} else if (ptrHandlerI2S->ptrSPIx == SPI4) {
		RCC->APB2ENR |= RCC_APB2ENR_SPI4EN;

	} else if (ptrHandlerI2S->ptrSPIx == SPI5) {
		RCC->APB2ENR |= RCC_APB2ENR_SPI5EN;
	}

	/*2. Reiniciamos el valor del registro de configuracion del I2S*/
	ptrHandlerI2S->ptrSPIx->I2SCFGR = 0;

	/* 3. Cambiamos el modo del periferico a I2S, de forma que el periferico SPI queda desactivado */
	ptrHandlerI2S->ptrSPIx->I2SCFGR |= SPI_I2SCFGR_I2SMOD;

	/* 4. Configuramos el modo en el que el I2S va a funcionar */
	ptrHandlerI2S->ptrSPIx->I2SCFGR |= (ptrHandlerI2S->I2S_Config.I2S_mode << SPI_I2SCFGR_I2SCFG_Pos);

	/* 5. Configuramos que tipo de protocolo se va a utilizar */
	ptrHandlerI2S->ptrSPIx->I2SCFGR |= (ptrHandlerI2S->I2S_Config.I2S_audioProtocole << SPI_I2SCFGR_I2SSTD_Pos);

	/* 6. Configurando en que estado descansa el reloj del sistema (por defecto descansa en alto...) */
	ptrHandlerI2S->ptrSPIx->I2SCFGR |= SPI_I2SCFGR_CKPOL;

	/* 7. Configuramos la longitud del dato que se va a manejar (16, 24 o 32 bit) */
	ptrHandlerI2S->ptrSPIx->I2SCFGR |= (ptrHandlerI2S->I2S_Config.I2S_dataLength << SPI_I2SCFGR_DATLEN_Pos);

	/* 8. Configuración del tamaño del canal de los datos (ya sea 16 o 32 bit) */
	if (ptrHandlerI2S->I2S_Config.I2S_dataLength == I2S_DATA_LENGTH_16BIT) {
		ptrHandlerI2S->ptrSPIx->I2SCFGR &= ~SPI_I2SCFGR_CHLEN;

	} else {
		ptrHandlerI2S->ptrSPIx->I2SCFGR |= SPI_I2SCFGR_CHLEN;
	}

	/* 9. Configuramos la frecuencia a la que el sistema hace el muestreo. EL pin Master clock es desactivado
	 * ya que no se requiere de una señal para varios dispositivos */

	// 9.1 Asigno el PLLI2S como la fuente para generar la señal de reloj del I2S
	RCC->CFGR &= ~RCC_CFGR_I2SSRC;

	// 9.2 Reinicio el registro a su valor por defecto (tomado de la hoja de datos)
	RCC->PLLI2SCFGR = 0x24003000;

	// 9.3 Configuramos el VCO, de forma que sea a 1MHz
	RCC->PLLI2SCFGR |= (16 << RCC_PLLI2SCFGR_PLLI2SM_Pos);

	// 9.3 Configuramos los valores del PLL con respecto a la velocidad de muestreo que se desea obtener
	// Todo es copnfigurado sin el master clock, porque no es necesario

	ptrHandlerI2S->ptrSPIx->I2SPR &= ~SPI_I2SPR_MCKOE;

	switch (ptrHandlerI2S->I2S_Config.I2S_audioFrequency) {
	case I2S_AUDIO_FREQ_8000_HZ: {
		if (ptrHandlerI2S->I2S_Config.I2S_dataLength == I2S_DATA_LENGTH_16BIT) {
			// Configuracion para 16KHz, tamaño de 16bit, en modo master-mode disable
			// Valores de configuracion tomados de la tabla 90, pag 594
			RCC->PLLI2SCFGR |= (192 << RCC_PLLI2SCFGR_PLLI2SN_Pos);
			RCC->PLLI2SCFGR |= (2 << RCC_PLLI2SCFGR_PLLI2SR_Pos);
			ptrHandlerI2S->ptrSPIx->I2SPR |= (187 << SPI_I2SPR_I2SDIV_Pos);
			ptrHandlerI2S->ptrSPIx->I2SPR |= SPI_I2SPR_ODD;
		} else {
			// Configuracion para 16KHz, tamaño de 32bit, en modo master-mode disable
			// Valores de configuracion tomados de la tabla 90, pag 594
			RCC->PLLI2SCFGR |= (192 << RCC_PLLI2SCFGR_PLLI2SN_Pos);
			RCC->PLLI2SCFGR |= (3 << RCC_PLLI2SCFGR_PLLI2SR_Pos);
			ptrHandlerI2S->ptrSPIx->I2SPR |= (62 << SPI_I2SPR_I2SDIV_Pos);
			ptrHandlerI2S->ptrSPIx->I2SPR |= SPI_I2SPR_ODD;
		}
		break;
	}

	case I2S_AUDIO_FREQ_16000_HZ: {
		if (ptrHandlerI2S->I2S_Config.I2S_dataLength == I2S_DATA_LENGTH_16BIT) {
			// Configuracion para 16KHz, tamaño de 16bit, en modo master-mode disable
			// Valores de configuracion tomados de la tabla 90, pag 594
			RCC->PLLI2SCFGR |= (192 << RCC_PLLI2SCFGR_PLLI2SN_Pos);
			RCC->PLLI2SCFGR |= (3 << RCC_PLLI2SCFGR_PLLI2SR_Pos);
			ptrHandlerI2S->ptrSPIx->I2SPR |= (62 << SPI_I2SPR_I2SDIV_Pos);
			ptrHandlerI2S->ptrSPIx->I2SPR |= SPI_I2SPR_ODD;
		} else {
			// Configuracion para 16KHz, tamaño de 32bit, en modo master-mode disable
			// Valores de configuracion tomados de la tabla 90, pag 594
			RCC->PLLI2SCFGR |= (256 << RCC_PLLI2SCFGR_PLLI2SN_Pos);
			RCC->PLLI2SCFGR |= (2 << RCC_PLLI2SCFGR_PLLI2SR_Pos);
			ptrHandlerI2S->ptrSPIx->I2SPR |= (62 << SPI_I2SPR_I2SDIV_Pos);
			ptrHandlerI2S->ptrSPIx->I2SPR |= SPI_I2SPR_ODD;
		}
		break;
	}
	case I2S_AUDIO_FREQ_48000_HZ: {

		if (ptrHandlerI2S->I2S_Config.I2S_dataLength == I2S_DATA_LENGTH_16BIT) {
			// Configuracion para 48KHz, tamaño de 16bit, en modo master-mode disable
			// Valores de configuracion tomados de la tabla 90, pag 594
			RCC->PLLI2SCFGR |= (192 << RCC_PLLI2SCFGR_PLLI2SN_Pos);
			RCC->PLLI2SCFGR |= (5 << RCC_PLLI2SCFGR_PLLI2SR_Pos);
			ptrHandlerI2S->ptrSPIx->I2SPR |= (12 << SPI_I2SPR_I2SDIV_Pos);
			ptrHandlerI2S->ptrSPIx->I2SPR |= SPI_I2SPR_ODD;
		} else {
			// Configuracion para 48KHz, tamaño de 32bit, en modo master-mode disable
			// Valores de configuracion tomados de la tabla 90, pag 594
			RCC->PLLI2SCFGR |= (384 << RCC_PLLI2SCFGR_PLLI2SN_Pos);
			RCC->PLLI2SCFGR |= (5 << RCC_PLLI2SCFGR_PLLI2SR_Pos);
			ptrHandlerI2S->ptrSPIx->I2SPR |= (12 << SPI_I2SPR_I2SDIV_Pos);
			ptrHandlerI2S->ptrSPIx->I2SPR |= SPI_I2SPR_ODD;
		}

		break;
	}

	case I2S_AUDIO_FREQ_44100_HZ: {

		if (ptrHandlerI2S->I2S_Config.I2S_dataLength == I2S_DATA_LENGTH_16BIT) {
			// Configuracion para 44.1KHz, tamaño de 16bit, en modo master-mode disable
			// Valores de configuracion tomados de la tabla 90, pag 594
			RCC->PLLI2SCFGR |= (302 << RCC_PLLI2SCFGR_PLLI2SN_Pos);
			RCC->PLLI2SCFGR |= (2 << RCC_PLLI2SCFGR_PLLI2SR_Pos);
			ptrHandlerI2S->ptrSPIx->I2SPR |= (53 << SPI_I2SPR_I2SDIV_Pos);
			ptrHandlerI2S->ptrSPIx->I2SPR |= SPI_I2SPR_ODD;
		} else {
			// Configuracion para 44.1KHz, tamaño de 32bit, en modo master-mode disable
			// Valores de configuracion tomados de la tabla 90, pag 594
			RCC->PLLI2SCFGR |= (429 << RCC_PLLI2SCFGR_PLLI2SN_Pos);
			RCC->PLLI2SCFGR |= (4 << RCC_PLLI2SCFGR_PLLI2SR_Pos);
			ptrHandlerI2S->ptrSPIx->I2SPR |= (19 << SPI_I2SPR_I2SDIV_Pos);
			ptrHandlerI2S->ptrSPIx->I2SPR &= ~SPI_I2SPR_ODD;
		}

		break;
	}
	default: {
		// Cargo por defecto os valores de muestreo mas lento
		if (ptrHandlerI2S->I2S_Config.I2S_dataLength == I2S_DATA_LENGTH_16BIT) {
			// Configuracion para 16KHz, tamaño de 16bit, en modo master-mode disable
			// Valores de configuracion tomados de la tabla 90, pag 594
			RCC->PLLI2SCFGR |= (256 << RCC_PLLI2SCFGR_PLLI2SN_Pos);
			RCC->PLLI2SCFGR |= (2 << RCC_PLLI2SCFGR_PLLI2SR_Pos);
			ptrHandlerI2S->ptrSPIx->I2SPR |= (62 << SPI_I2SPR_I2SDIV_Pos);
			ptrHandlerI2S->ptrSPIx->I2SPR |= SPI_I2SPR_ODD;
		} else {
			// Configuracion para 16KHz, tamaño de 32bit, en modo master-mode disable
			// Valores de configuracion tomados de la tabla 90, pag 594
			RCC->PLLI2SCFGR |= (192 << RCC_PLLI2SCFGR_PLLI2SN_Pos);
			RCC->PLLI2SCFGR |= (3 << RCC_PLLI2SCFGR_PLLI2SR_Pos);
			ptrHandlerI2S->ptrSPIx->I2SPR |= (62 << SPI_I2SPR_I2SDIV_Pos);
			ptrHandlerI2S->ptrSPIx->I2SPR |= SPI_I2SPR_ODD;
		}
		break;
	}
	}

	/* 9.4 Encendemos el PLLI2S y esperamos que se ponga estable */
	RCC->CR |= RCC_CR_PLLI2SON;

	// Esperamos a que el PLL se ponga estable
	while(!(RCC->CR & RCC_CR_PLLI2SRDY)){
		__NOP();
	}

	/* 10.Configuramos la interrupcion por recepcion */
	/* 10.a Desactivamos las interrupciones globales mientras configuramos el sistema.*/
	__disable_irq();

	if (ptrHandlerI2S->I2S_Config.I2S_enableIntRX == I2S_RX_INTERRUP_ENABLE) {
		// 10.b Como está activada, debemos configurar la interrupción por recepción
		/* Debemos activar la interrupción RX en la configuración del SPI */
		ptrHandlerI2S->ptrSPIx->CR2 |= SPI_CR2_RXNEIE;

		/* 10.c Matriculamos la interrupcion en el NVIC */
		/* Lo debemos hacer para cada uno de las posibles opciones que tengamos (SPI1, ..., SPI5) */
		if (ptrHandlerI2S->ptrSPIx == SPI1) {
			__NVIC_EnableIRQ(SPI1_IRQn);

		} else if (ptrHandlerI2S->ptrSPIx == SPI2) {
			__NVIC_EnableIRQ(SPI2_IRQn);
			__NVIC_SetPriority(SPI2_IRQn, 5);

		} else if (ptrHandlerI2S->ptrSPIx == SPI3) {
			__NVIC_EnableIRQ(SPI3_IRQn);

		} else if (ptrHandlerI2S->ptrSPIx == SPI4) {
			__NVIC_EnableIRQ(SPI4_IRQn);

		} else if (ptrHandlerI2S->ptrSPIx == SPI5) {
			__NVIC_EnableIRQ(SPI5_IRQn);
		}

	}

	/* 11. Activamos el periferico I2S*/
	ptrHandlerI2S->ptrSPIx->I2SCFGR |= SPI_I2SCFGR_I2SE;

	/* x. Volvemos a activar las interrupciones del sistema */
	__enable_irq();

	// Para bajar el estado de cualquier posible bandera
	auxConfig = ptrHandlerI2S->ptrSPIx->DR;
	auxConfig = ptrHandlerI2S->ptrSPIx->SR;

}

/**/
void startSampling(I2S_Handler_t *ptrHandlerI2S){
	ptrHandlerI2S->ptrSPIx->I2SCFGR |= SPI_I2SCFGR_I2SE;
}

/**/
void stopSampling(I2S_Handler_t *ptrHandlerI2S){
	ptrHandlerI2S->ptrSPIx->I2SCFGR &= ~SPI_I2SCFGR_I2SE;
}

int32_t getAudioData(void) {
	return audioData;
}

void SPI1_IRQHandler(void) {
	// Evaluamos si la interrupción que se dio es por RX
	if (SPI1->SR & SPI_SR_RXNE) {
		auxAudioData = (uint16_t) SPI1->DR;

		// Este contador es para analizar si se recibio la parte alta o la parte baja del dato.
		counterData++;

		// Si es valor es 1, se recibio la parte alta del dato
		if (counterData == 1) {
			tempAudioData = (int32_t) (auxAudioData << 16);

		} else {
			// Si el valor es 2, se recibio la parte baja del dato
			// Se termina de organizar el dato y se
			tempAudioData |= (int32_t) (0x00FF & auxAudioData);
			audioData = tempAudioData;
			counterData = 0;
		}
		i2s1_Rx_Callback();
	}
}

void SPI2_IRQHandler(void) {
	uint32_t auxInterrup;
	(void) auxInterrup;
	// Evaluamos si la interrupción que se dio es por RX
	if (SPI2->SR & SPI_SR_RXNE) {
		auxAudioData = (int16_t) SPI2->DR;
		auxInterrup =  (uint16_t) SPI2->SR;

		if(SPI2->SR & SPI_SR_CHSIDE){
			__NOP();
		}

		if(counterData == 64){
			//SPI2->I2SCFGR &= ~SPI_I2SCFGR_I2SE;
			__NOP();
			counterData = 0;
		}

		debugData[counterData] = auxAudioData;
		// Este contador es para analizar si se recibio la parte alta o la parte baja del dato.
		counterData++;



//		switch(counterData){
//		case 1:
//		{
//			handlerMic->I2S_Config.audioBuffer[0] = (int16_t) (auxAudioData);
//			break;
//		}
//		case 2:
//		{
//			handlerMic->I2S_Config.audioBuffer[1] = (int16_t) (auxAudioData);
//			break;
//		}
//		case 3:
//		{
//			handlerMic->I2S_Config.audioBuffer[2] = (int16_t) (auxAudioData);
//			break;
//		}
//		case 4:
//		{
//			handlerMic->I2S_Config.audioBuffer[3] = (int16_t) (auxAudioData);
//			counterData = 0;
//			i2s2_Rx_Callback();
//			break;
//		}
//		default:
//			break;
//		}


	}
}

void SPI3_IRQHandler(void) {
	// Evaluamos si la interrupción que se dio es por RX
	if (SPI3->SR & SPI_SR_RXNE) {
		auxAudioData = (uint16_t) SPI3->DR;
		// Este contador es para analizar si se recibio la parte alta o la parte baja del dato.
		counterData++;

		// Si es valor es 1, se recibio la parte alta del dato
		if (counterData == 1) {
			tempAudioData = (int32_t) (auxAudioData << 16);

		} else {
			// Si el valor es 2, se recibio la parte baja del dato
			// Se termina de organizar el dato y se
			tempAudioData |= (int32_t) (0x00FF & auxAudioData);
			audioData = tempAudioData;
			counterData = 0;
		}
		i2s3_Rx_Callback();
	}
}

void SPI4_IRQHandler(void) {
	// Evaluamos si la interrupción que se dio es por RX
	if (SPI4->SR & SPI_SR_RXNE) {
		auxAudioData = (uint16_t) SPI4->DR;
		// Este contador es para analizar si se recibio la parte alta o la parte baja del dato.
		counterData++;

		switch(counterData){
		case 1:
		{
			handlerMic->I2S_Config.audioBuffer[0] = (int16_t) (auxAudioData << 16);
			break;
		}
		case 2:
		{
			handlerMic->I2S_Config.audioBuffer[1] = (int16_t) (auxAudioData);
			break;
		}
		case 3:
		{
			handlerMic->I2S_Config.audioBuffer[2] = (int16_t) (auxAudioData << 16);
			break;
		}
		case 4:
		{
			handlerMic->I2S_Config.audioBuffer[3] = (int16_t) (auxAudioData);
			break;
		}
		default:
			break;
		}

		i2s4_Rx_Callback();
	}
}

void SPI5_IRQHandler(void) {
	// Evaluamos si la interrupción que se dio es por RX
	if (SPI5->SR & SPI_SR_RXNE) {
		auxAudioData = (uint16_t) SPI5->DR;
		// Este contador es para analizar si se recibio la parte alta o la parte baja del dato.
		counterData++;

		// Si es valor es 1, se recibio la parte alta del dato
		if (counterData == 1) {
			tempAudioData = (int32_t) (auxAudioData << 16);

		} else {
			// Si el valor es 2, se recibio la parte baja del dato
			// Se termina de organizar el dato y se
			tempAudioData |= (int32_t) (0x00FF & auxAudioData);
			audioData = tempAudioData;
			counterData = 0;
		}
		i2s5_Rx_Callback();
	}
}

__attribute__((weak)) void i2s1_Rx_Callback(void) {
	/* NOTE : This function should not be modified, when the callback is needed,
	 the BasicTimer_Callback could be implemented in the main file
	 */
	__NOP();
}

__attribute__((weak)) void i2s2_Rx_Callback(void) {
	/* NOTE : This function should not be modified, when the callback is needed,
	 the BasicTimer_Callback could be implemented in the main file
	 */
	__NOP();
}

__attribute__((weak)) void i2s3_Rx_Callback(void) {
	/* NOTE : This function should not be modified, when the callback is needed,
	 the BasicTimer_Callback could be implemented in the main file
	 */
	__NOP();
}

__attribute__((weak)) void i2s4_Rx_Callback(void) {
	/* NOTE : This function should not be modified, when the callback is needed,
	 the BasicTimer_Callback could be implemented in the main file
	 */
	__NOP();
}

__attribute__((weak)) void i2s5_Rx_Callback(void) {
	/* NOTE : This function should not be modified, when the callback is needed,
	 the BasicTimer_Callback could be implemented in the main file
	 */
	__NOP();
}
