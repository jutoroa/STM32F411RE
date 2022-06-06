/*
 * I2CMPU6050.c
 *
 *  Created on: 4/05/2022
 *  Author: Juan Pablo
 *
 */


// *************** // INCLUDES // *************** //

#include <stm32f4xx.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "GPIOxDriver.h"
#include "USARTxDriver.h"
#include "TIMxDriver.h"
#include "AdcDriver.h"
#include "EXTIxDriver.h"
#include "I2CxDriver.h"
#include "SysTickDriver.h"
#include "MPU6050Driver.h"
#include "rtcDS1307Driver.h"
#include "OLEDDriver.h"

// *************** // VARIABLES PROYECTO // *************** //

GPIO_Handler_t 	handlerStateLED 		= {0};	// StateLED
GPIO_Handler_t 	handlerPinTX 			= {0};	// handlerPinTX
GPIO_Handler_t 	handlerPinRX 			= {0};	// handlerPinRX
TIMER_Handler_t handlerTimer2 			= {0};	// Timer2
TIMER_Handler_t handlerTimer3 			= {0};	// Timer3
TIMER_Handler_t handlerTimer4 			= {0};	// Timer4
USART_Handler_t handlerCommTerminal		= {0};	// Usart para la terminal

/* Configuración para el I2C */
GPIO_Handler_t handlerI2CSDA =	{0};
GPIO_Handler_t handlerI2CSCL =	{0};
I2C_Handler_t  handlerOLED	 = {0};

/* Datos para la pantalla OLED */

// Variables auxiliar.
char 		bufferData[64] = "¡Hola! Soy el USART del STM32 y estoy funcionando.";
uint8_t 	rxData				= 0;

uint32_t	counterDelay	= 0;
/* Variables para los commands */

// *************** // Headers // *************** //

void initSystem(void);
//void OLED_Init(I2C_Handler_t *ptrHandlerI2C);
void delay(uint32_t time);

// *************** // MAIN // *************** //
int main(void)
{
	// Inicializamos el sistema
	initSystem();
	//OLED_Init(&handlerOLED);

	//initialMessage();
	writeMsg(&handlerCommTerminal, bufferData);
	writeMsg(&handlerCommTerminal, "\n");

	/* Main Loop */
	while(1){
		__NOP();
	}
}
//***********// *********** // Definición de Funciones // *********** //***********//

//***********// InitSystem //***********//

// Función que define la configuración de todos los pines y periféricos

void initSystem(void){

	// Configuración el State LED
	handlerStateLED.pGPIOx								= GPIOA;
	handlerStateLED.GPIO_PinConfig.GPIO_PinNumber		= PIN_5;
	handlerStateLED.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerStateLED.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerStateLED.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerStateLED.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;

	//Cargamos la configuración
	GPIO_Config(&handlerStateLED);

	// Prendemos el LED
	GPIO_WritePin(&handlerStateLED, SET);

	/* Configurando los pines sobre los que funciona el USART */
	handlerPinTX.pGPIOx									= GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber			= PIN_2;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerPinTX.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerPinTX.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode		= AF7;

	GPIO_Config(&handlerPinTX);

	handlerPinRX.pGPIOx									= GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber			= PIN_3;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerPinRX.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerPinRX.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode		= AF7;

	GPIO_Config(&handlerPinRX);

	// Configuración del USART del USB para RX
	handlerCommTerminal.ptrUSARTx							= USART2;
	handlerCommTerminal.USART_Config.USART_mode				= USART_MODE_RXTX;
	handlerCommTerminal.USART_Config.USART_baudrate			= USART_BAUDRATE_19200;
	handlerCommTerminal.USART_Config.USART_datasize			= USART_DATASIZE_8BIT;
	handlerCommTerminal.USART_Config.USART_parity			= USART_PARITY_ODD;
	handlerCommTerminal.USART_Config.USART_stopbits			= USART_STOPBIT_1;
	handlerCommTerminal.USART_Config.USART_interrupt		= USART_INTERRUPT_RX_ENABLE;

	USART_Config(&handlerCommTerminal);

	// Configuración del timer2
	handlerTimer2.ptrTIMx								= TIM2;
	handlerTimer2.timerConfig.Timer_mode				= TIMER_MODE_UP;
	handlerTimer2.timerConfig.Timer_speed				= TIMER_INCR_SPEED_1ms;
	handlerTimer2.timerConfig.Timer_period				= 250;

	Timer_Config(&handlerTimer2);

	// Configuración del timer3
	handlerTimer3.ptrTIMx								= TIM3;
	handlerTimer3.timerConfig.Timer_mode				= TIMER_MODE_UP;
	handlerTimer3.timerConfig.Timer_speed				= TIMER_INCR_SPEED_1ms;
	handlerTimer3.timerConfig.Timer_period				= 250;

	Timer_Config(&handlerTimer3);

	// Configuración del timer4
	handlerTimer4.ptrTIMx								= TIM4;
	handlerTimer4.timerConfig.Timer_mode				= TIMER_MODE_UP;
	handlerTimer4.timerConfig.Timer_speed				= TIMER_INCR_SPEED_1ms;
	handlerTimer4.timerConfig.Timer_period				= 1;

	Timer_Config(&handlerTimer4);

	// Configuramos los pines para el I2C SCL
	handlerI2CSCL.pGPIOx								= GPIOB;
	handlerI2CSCL.GPIO_PinConfig.GPIO_PinNumber			= PIN_6;
	handlerI2CSCL.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerI2CSCL.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_OPENDRAIN;
	handlerI2CSCL.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerI2CSCL.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerI2CSCL.GPIO_PinConfig.GPIO_PinAltFunMode		= AF4;

	GPIO_Config(&handlerI2CSCL);

	// Configuramos los pines para el I2C SDA
	handlerI2CSDA.pGPIOx								= GPIOB;
	handlerI2CSDA.GPIO_PinConfig.GPIO_PinNumber			= PIN_7;
	handlerI2CSDA.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerI2CSDA.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_OPENDRAIN;
	handlerI2CSDA.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerI2CSDA.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerI2CSDA.GPIO_PinConfig.GPIO_PinAltFunMode		= AF4;

	GPIO_Config(&handlerI2CSDA);

	handlerOLED.ptrI2Cx			= I2C1;
	handlerOLED.modeI2C			= I2C_MODE_FM;
	handlerOLED.slaveAddress	= 0x3C;
	I2C_Config(&handlerOLED);

}

/*
void OLED_Init(I2C_Handler_t *ptrHandlerI2C){

	OLED_CommandByte(ptrHandlerI2C, 0xAE);
	OLED_CommandByte(ptrHandlerI2C, 0xD5);
	OLED_CommandByte(ptrHandlerI2C, 0x80);
	OLED_CommandByte(ptrHandlerI2C, 0xA8);
	OLED_CommandByte(ptrHandlerI2C, 0x3F);
	OLED_CommandByte(ptrHandlerI2C, 0xD3);
	OLED_CommandByte(ptrHandlerI2C, 0x00);
	OLED_CommandByte(ptrHandlerI2C, 0x40);
	OLED_CommandByte(ptrHandlerI2C, 0xAD);
	OLED_CommandByte(ptrHandlerI2C, 0x8B);
	OLED_CommandByte(ptrHandlerI2C, 0xA1);
	OLED_CommandByte(ptrHandlerI2C, 0xC8);
	OLED_CommandByte(ptrHandlerI2C, 0xDA);
	OLED_CommandByte(ptrHandlerI2C, 0x12);
	OLED_CommandByte(ptrHandlerI2C, 0x81);
	OLED_CommandByte(ptrHandlerI2C, 0xCF);
	OLED_CommandByte(ptrHandlerI2C, 0xD9);
	OLED_CommandByte(ptrHandlerI2C, 0x22);
	OLED_CommandByte(ptrHandlerI2C, 0xDB);
	OLED_CommandByte(ptrHandlerI2C, 0x40);
	OLED_CommandByte(ptrHandlerI2C, 0x32);
	OLED_CommandByte(ptrHandlerI2C, 0xA6);
	OLED_CommandByte(ptrHandlerI2C, 0xAF);
	//delay(100);
	OLED_CommandByte(ptrHandlerI2C, 0x00);
	OLED_CommandByte(ptrHandlerI2C, 0x10);
	OLED_CommandByte(ptrHandlerI2C, 0x40);

	OLED_CommandByte(ptrHandlerI2C, 0xB0);
	OLED_CommandByte(ptrHandlerI2C, 0x02);
	OLED_CommandByte(ptrHandlerI2C, 0x10);

	OLED_DataByte(ptrHandlerI2C, dataOled);
	OLED_DataByte(ptrHandlerI2C, dataOled);
	OLED_DataByte(ptrHandlerI2C, dataOled);
	OLED_DataByte(ptrHandlerI2C, dataOled);
	OLED_DataByte(ptrHandlerI2C, dataOled);
	OLED_DataByte(ptrHandlerI2C, dataOled);
	OLED_DataByte(ptrHandlerI2C, dataOled);
	OLED_DataByte(ptrHandlerI2C, dataOled);
	OLED_DataByte(ptrHandlerI2C, dataOled);
	OLED_DataByte(ptrHandlerI2C, dataOled);
	OLED_DataByte(ptrHandlerI2C, dataOled);
	OLED_DataByte(ptrHandlerI2C, dataOled);
	OLED_DataByte(ptrHandlerI2C, dataOled);
	OLED_DataByte(ptrHandlerI2C, dataOled);
	OLED_DataByte(ptrHandlerI2C, dataOled);
	OLED_DataByte(ptrHandlerI2C, dataOled);

	OLED_CommandByte(ptrHandlerI2C, 0xB1);
	OLED_CommandByte(ptrHandlerI2C, 0x02);
	OLED_CommandByte(ptrHandlerI2C, 0x10);

	OLED_DataByte(&handlerOLED, OLEDA());
	OLED_DataByte(&handlerOLED, charOledB);
	OLED_DataByte(&handlerOLED, charOledC);
	OLED_DataByte(&handlerOLED, charOledD);
	OLED_DataByte(&handlerOLED, charOledE);
	OLED_DataByte(&handlerOLED, charOledF);
	OLED_DataByte(&handlerOLED, charOledG);
	OLED_DataByte(&handlerOLED, charOledH);
	OLED_DataByte(&handlerOLED, charOledI);
	OLED_DataByte(&handlerOLED, charOledJ);
	OLED_DataByte(&handlerOLED, charOledK);
	OLED_DataByte(&handlerOLED, charOledL);
	OLED_DataByte(&handlerOLED, charOledM);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);

	OLED_CommandByte(ptrHandlerI2C, 0xB2);
	OLED_CommandByte(ptrHandlerI2C, 0x02);
	OLED_CommandByte(ptrHandlerI2C, 0x10);

	OLED_DataByte(ptrHandlerI2C, charOledN);
	OLED_DataByte(ptrHandlerI2C, charOledO);
	OLED_DataByte(ptrHandlerI2C, charOledP);
	OLED_DataByte(ptrHandlerI2C, charOledQ);
	OLED_DataByte(ptrHandlerI2C, charOledR);
	OLED_DataByte(ptrHandlerI2C, charOledS);
	OLED_DataByte(ptrHandlerI2C, charOledT);
	OLED_DataByte(ptrHandlerI2C, charOledU);
	OLED_DataByte(ptrHandlerI2C, charOledV);
	OLED_DataByte(ptrHandlerI2C, charOledW);
	OLED_DataByte(ptrHandlerI2C, charOledX);
	OLED_DataByte(ptrHandlerI2C, charOledY);
	OLED_DataByte(ptrHandlerI2C, charOledZ);
	OLED_DataByte(ptrHandlerI2C, dataOled);
	OLED_DataByte(ptrHandlerI2C, dataOled);
	OLED_DataByte(ptrHandlerI2C, dataOled);

	OLED_CommandByte(ptrHandlerI2C, 0xB3);
	OLED_CommandByte(ptrHandlerI2C, 0x02);
	OLED_CommandByte(ptrHandlerI2C, 0x10);

	OLED_DataByte(&handlerOLED, charOled1);
	OLED_DataByte(&handlerOLED, charOled2);
	OLED_DataByte(&handlerOLED, charOled3);
	OLED_DataByte(&handlerOLED, charOled4);
	OLED_DataByte(&handlerOLED, charOled5);
	OLED_DataByte(&handlerOLED, charOled6);
	OLED_DataByte(&handlerOLED, charOled7);
	OLED_DataByte(&handlerOLED, charOled8);
	OLED_DataByte(&handlerOLED, charOled9);
	OLED_DataByte(&handlerOLED, charOled0);
	OLED_DataByte(&handlerOLED, charOledPlus);
	OLED_DataByte(&handlerOLED, charOledSus);
	OLED_DataByte(&handlerOLED, charOledExclamation);
	OLED_DataByte(&handlerOLED, charOledHeart);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);

	OLED_CommandByte(ptrHandlerI2C, 0xB4);
	OLED_CommandByte(ptrHandlerI2C, 0x02);
	OLED_CommandByte(ptrHandlerI2C, 0x10);

	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);

	OLED_CommandByte(ptrHandlerI2C, 0xB5);
	OLED_CommandByte(ptrHandlerI2C, 0x02);
	OLED_CommandByte(ptrHandlerI2C, 0x10);

	OLED_DataByte(&handlerOLED, charOledO);
	OLED_DataByte(&handlerOLED, charOledR);
	OLED_DataByte(&handlerOLED, charOledO);
	OLED_DataByte(&handlerOLED, charOledZ);
	OLED_DataByte(&handlerOLED, charOledC);
	OLED_DataByte(&handlerOLED, charOledO);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, charOledH);
	OLED_DataByte(&handlerOLED, charOledP);
	OLED_DataByte(&handlerOLED, charOledT);
	OLED_DataByte(&handlerOLED, charOledA);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);

	OLED_CommandByte(ptrHandlerI2C, 0xB6);
	OLED_CommandByte(ptrHandlerI2C, 0x02);
	OLED_CommandByte(ptrHandlerI2C, 0x10);

	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);

	OLED_CommandByte(ptrHandlerI2C, 0xB7);
	OLED_CommandByte(ptrHandlerI2C, 0x02);
	OLED_CommandByte(ptrHandlerI2C, 0x10);

	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
	OLED_DataByte(&handlerOLED, dataOled);
}
*/
void delay(uint32_t time){
	counterDelay = 0;
	while(counterDelay < time){
		__NOP();
	}
	counterDelay = 0;
}

void USART2_Callback(void){
	// Lectura de los datos recibidos
	rxData = returnData();
}
// Timer encargado del StateLED
void Timer2_Callback(void){
	handlerStateLED.pGPIOx -> ODR ^= GPIO_ODR_OD5;		// Encendido y apagado StateLED
}

// Timer encargado del delay
void Timer4_Callback(void){
	counterDelay++;
}
