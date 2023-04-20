/**
 ******************************************************************************
 * @file           : main.c
 * @author         : namontoy
 * @brief          : Pressure sensor MPE280
 ******************************************************************************
 *
 ******************************************************************************
 */



#include "stdint.h"

#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "BasicTimer.h"
#include "USARTxDriver.h"
#include "MPE280Driver.h"

/* Creación de los objetos que controlan algún elemento del proyecto */
GPIO_Handler_t	handlerLedState	= {0};
GPIO_Handler_t	handlerUsartRx	= {0};
GPIO_Handler_t	handlerUsartTx	= {0};

BasicTimer_Handler_t handlerTimerOkState = {0};

USART_Handler_t	commTerm = {0};
uint8_t rxData = '\0';
char printMsg[64] = {0};
uint8_t periodicMsg = 0;

/* Para el SPI */
uint8_t spiRxBuffer[6] = {0};

/* Prototipos de las funciones */
void initSystems(void);


/* Función principal */
int main(void){

	/* Configuramos todos los módulos del sistema */
	initSystems();

	mpe280_init();

	/* Imprimimos mensaje de bienvenida */
	writeChar(&commTerm, ' ');
	writeMsg(&commTerm, "--- Press MPE280 - V0.01 ---\n\r");

	/* Main loop */
	while(1){
		if(periodicMsg == 4){

			mpe280_readData(MPE280_CHIP_ID, spiRxBuffer, 1);

			mpe280_writeData(MPE280_CTRL_MEAS, 0x00);

			mpe280_readData(MPE280_CTRL_MEAS, spiRxBuffer, 1);

			mpe280_writeData(MPE280_CTRL_MEAS, 0b01101111);

			mpe280_readData(MPE280_CTRL_MEAS, spiRxBuffer, 1);

			mpe280_readData(MPE280_PRESS_MSB, spiRxBuffer, 3);

			periodicMsg = 0;
		}



	}

	return 0;
}

/* Inicialización de todos los periféricos que componen el proyecto */
void initSystems(void){

	/* Inicialización */
	handlerLedState.pGPIOx								= GPIOA;
	handlerLedState.GPIO_PinConfig.GPIO_PinNumber		= PIN_8;
	handlerLedState.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerLedState.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerLedState.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerLedState.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_MEDIUM;
	handlerLedState.GPIO_PinConfig.GPIO_PinAltFunMode	= AF14;

	// Cargamos la configuración del pin LedState.
	GPIO_Config(&handlerLedState);

	// Configuramos el TimerOK, para que genere una interrupción periódica.
	handlerTimerOkState.ptrTIMx							= TIM2;
	handlerTimerOkState.TIMx_Config.TIMx_mode			= BTIMER_MODE_UP;
	handlerTimerOkState.TIMx_Config.TIMx_period			= 2500;		// Me debe generar una int cada 250ms
	handlerTimerOkState.TIMx_Config.TIMx_speed			= BTIMER_SPEED_100us;
	handlerTimerOkState.TIMx_Config.TIMx_interruptEnable = BTIMER_INTERRUP_ENABLE;

	// cargamos la configuración del Timer OK
	BasicTimer_Config(&handlerTimerOkState);

	// Configuramos los pines relacionados al puerto serial
	/* USART Rx */
	handlerUsartRx.pGPIOx								= GPIOA;
	handlerUsartRx.GPIO_PinConfig.GPIO_PinNumber		= PIN_3;
	handlerUsartRx.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerUsartRx.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerUsartRx.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUsartRx.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_HIGH;
	handlerUsartRx.GPIO_PinConfig.GPIO_PinAltFunMode	= AF7;

	GPIO_Config(&handlerUsartRx);

	/* USART Tx */
	handlerUsartTx.pGPIOx 								= GPIOA;
	handlerUsartTx.GPIO_PinConfig.GPIO_PinNumber		= PIN_2;
	handlerUsartTx.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerUsartTx.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerUsartTx.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerUsartTx.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_HIGH;
	handlerUsartTx.GPIO_PinConfig.GPIO_PinAltFunMode	= AF7;

	GPIO_Config(&handlerUsartTx);

	/* Ahora configuramos el periférico */
	commTerm.ptrUSARTx						= USART2;
	commTerm.USART_Config.USART_mode		= USART_MODE_RXTX;
	commTerm.USART_Config.USART_baudrate	= USART_BAUDRATE_115200;
	commTerm.USART_Config.USART_parity		= USART_PARITY_NONE;
	commTerm.USART_Config.USART_stopbits	= USART_STOPBIT_1;
	commTerm.USART_Config.USART_enableIntRX	= USART_RX_INTERRUP_ENABLE;
	commTerm.USART_Config.USART_enableIntTX	= USART_TX_INTERRUP_DISABLE;

	USART_Config(&commTerm);

}

/* Función Callback ejecutada por la interrupción del TIM2*/
void BasicTimer2_Callback(void){
	GPIO_TooglePin(&handlerLedState);
	periodicMsg++;
}

/* Función Callback que se ejecuta cada vez que se recibe un carácter */
void usart2Rx_Callback(void){
	rxData = getRxData();
}
