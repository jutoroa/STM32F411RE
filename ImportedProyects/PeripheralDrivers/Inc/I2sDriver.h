/*
 * I2sDriver.h
 *
 *  Created on: Nov 16, 2022
 *      Author: namontoy
 */
#include "stm32f4xx.h"

#ifndef I2SDRIVER_H_
#define I2SDRIVER_H_

#define I2S_AUDIO_PHILLIPS_STD			0
#define I2S_AUDIO_MSB_JUSTIFIED_STD		1
#define I2S_AUDIO_LSB_JUSTIFIED_STD		2
#define I2S_AUDIO_PCM_STD				3

#define I2S_MODE_SLAVE_TX		0
#define I2S_MODE_SLAVE_RX		1
#define I2S_MODE_MASTER_TX		2
#define I2S_MODE_MASTER_RX		3

#define I2S_DATA_LENGTH_16BIT	0
#define I2S_DATA_LENGTH_24BIT	1
#define I2S_DATA_LENGTH_32BIT	2

// Estos elementos se relacionan con la tabla 90, sección 20.4.4, pag 594
// y deben ser utilizados para configurar adecuadamente el PLL_I2S
#define I2S_AUDIO_FREQ_8000_HZ		0
#define I2S_AUDIO_FREQ_16000_HZ		1
#define I2S_AUDIO_FREQ_48000_HZ		2
#define I2S_AUDIO_FREQ_44100_HZ		3

#define I2S_RX_INTERRUP_ENABLE	1
#define I2S_RX_INTERRUP_DISABLE	0
#define I2S_TX_INTERRUP_ENABLE	1
#define I2S_TX_INTERRUP_DISABLE	0


/**/
typedef struct
{
	uint8_t	I2S_audioProtocole;		// Define los diferentes modos protocolos que se pueden configurar.
	uint8_t	I2S_audioFrequency;		// Define la frecuencia de muestreo (relacionado con la velocidad del reloj)
	uint8_t	I2S_dataLength;			// Define si la muestra es de 16 bit, 24 bit o 32 bit...
	uint8_t	I2S_mode;				// Configura si solo recepción o bi-direccional
	uint8_t	dataAvailable;			// Indicación que un nuevo dato esta disponible
	uint8_t	I2S_enableIntRX;
	int16_t	audioBuffer[4];			// Datos que ingresan
}I2S_Config_t;

/**/
typedef struct
{
	SPI_TypeDef		*ptrSPIx;		// El I2S comparte el mismo modulo del SPI, por lo cual es el modulo que se debe llamar.
	I2S_Config_t	I2S_Config;
}I2S_Handler_t;


void i2s_config(I2S_Handler_t *ptrHandlerI2S);
int32_t getAudioData(void);
void startSampling(I2S_Handler_t *ptrHandlerI2S);
void stopSampling(I2S_Handler_t *ptrHandlerI2S);
void i2s1_Rx_Callback(void);
void i2s2_Rx_Callback(void);
void i2s3_Rx_Callback(void);
void i2s4_Rx_Callback(void);
void i2s5_Rx_Callback(void);


#endif /* I2SDRIVER_H_ */
