/**
 **************************************************************************************************
 * @file           : MainIntroADC.c
 * @author         : Nerio Montoya - namontoy@unal.edu.co
 * @brief          : Introduccion a la conversion ADC:
 * 					 - Hacer un blinky de 250ms con el USER_LED
 * 					 - Enviar un mensaje cada vez que se presiona la tecla 'm'
 * 					 - Lanza una nueva conversion ADC cada vez que se presiona la tecla 's'
 * 					 - Cuando termina la ADC, el sistema lanza una interrupción que finalmente
 * 					   presenta el resultado de la conversion ADC como un uint16
 *					video
 *					https://youtu.be/F9xYe11t_Gs
 **************************************************************************************************
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "stm32f4xx.h"
#include "GPIOxDriver.h"
#include "TIMxDriver.h"
#include "USARTxDriver.h"
#include "AdcDriver.h"

// Definicion de los handlers necesarios
GPIO_Handler_t handlerBlinkyPin = {0};
GPIO_Handler_t handlerUserButton = {0};
GPIO_Handler_t handlerTxPin		= {0};
GPIO_Handler_t handlerRxPin		= {0};

// Timer encargado del Blinky
TIMER_Handler_t handlerBlinkyTimer = {0};

// Comunicación RS-232 con el PC, ya habilitada en la board Nucleo
// Utiliza la conexion USB
USART_Handler_t handlerUsart2 = {0};
uint8_t rxData = 0;
char bufferData[64] = {0};

// Elementos necesarios para configurar el ADC
ADC_Config_t	adcConfig = {0};
uint16_t adcData = 0;
bool adcIsComplete = false;

// Mensaje que se imprime
char greetingMsg[] = "Taller V Rocks!\n";

// Definicion de las cabeceras de las funciones del main
void initSystem(void);

/**
 * Funcion principal del programa.
 * Esta funcion es el corazón del programa!!
 *
 * */
int main(void) {

	// Inicializamos todos los elementos del sistema
	initSystem();

	writeChar(&handlerUsart2, ' ');

	/* Loop forever */
	while (1) {

		// El sistema siempre está verificando si el valor de rxData ha cambiado
        // (lo cual sucede en la ISR de la recepcion (RX).
        // Si este valor deja de ser '\0' significa que se recibio un caracter
        // por lo tanto entra en el bloque if para analizar que se recibio
		if(rxData != '\0'){
			// Imprimimos el caracter recibido
            writeChar(&handlerUsart2, rxData);

			if(rxData == 'm'){
				// Presentamos un mensaje
				writeMsg(&handlerUsart2, greetingMsg);
			}

			if(rxData == 's'){
				// Lanzamos una nueva conversión ADC
				startSingleADC();
			}

			rxData = '\0';
		}

		// Esta otra bandera corresponde a cuando se dio una nueva conversion ADC,
		// entonces el sistema entra a este bloque if para dar un formato al dato
		// y enviarlo por la comunicación serial
		if(adcIsComplete == true ){
			sprintf(bufferData, "ADC = %u \n\r",(unsigned int ) adcData);
			//writeMsg(&handlerCommTerminal, "Hola Mundo!");
			writeMsg(&handlerUsart2, bufferData);
			adcIsComplete = false;
		}
	}

	return 0;
}

/*
 * Funcion encargada de la inicializacion de los elementos del sistema
 */
void initSystem(void){

	// Configurando el pin para el Led_Blinky
	handlerBlinkyPin.pGPIOx 							= GPIOA;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinNumber		= PIN_5;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_OUT;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;
	handlerBlinkyPin.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerBlinkyPin);

	// Configurando el pin para el boton azul
	handlerUserButton.pGPIOx 								= GPIOC;
	handlerUserButton.GPIO_PinConfig.GPIO_PinNumber			= PIN_13;
	handlerUserButton.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_IN;
	handlerUserButton.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	GPIO_Config(&handlerUserButton);

	/* Configurando los pines sobre los que funciona el USART2 (TX) */
	handlerTxPin.pGPIOx 							= GPIOA;
	handlerTxPin.GPIO_PinConfig.GPIO_PinNumber		= PIN_2;
	handlerTxPin.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	handlerTxPin.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerTxPin.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;
	handlerTxPin.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerTxPin.GPIO_PinConfig.GPIO_PinAltFunMode	= AF7;
	GPIO_Config(&handlerTxPin);

	/* Configurando los pines sobre los que funciona el USART2 (RX) */
	handlerRxPin.pGPIOx 							= GPIOA;
	handlerRxPin.GPIO_PinConfig.GPIO_PinNumber		= PIN_3;
	handlerRxPin.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_ALTFN;
	handlerRxPin.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerRxPin.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerRxPin.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;
	handlerRxPin.GPIO_PinConfig.GPIO_PinAltFunMode	= AF7;
	GPIO_Config(&handlerRxPin);

	// Configurando el Timer2 para que funcione con el blinky
	handlerBlinkyTimer.ptrTIMx 						= TIM2;
	handlerBlinkyTimer.timerConfig.Timer_mode		= TIMER_MODE_UP;
	handlerBlinkyTimer.timerConfig.Timer_speed		= TIMER_INCR_SPEED_100us;
	handlerBlinkyTimer.timerConfig.Timer_period		= 2500;
//	handlerBlinkyTimer.timerConfig.TIMx_interruptEnable	= BTIMER_INTERRUP_ENABLE;
	Timer_Config(&handlerBlinkyTimer);

	// Configurando la comunicación serial (Cable verde es TX, Cable Blanco es RX)
	handlerUsart2.ptrUSARTx 					= USART2;
	handlerUsart2.USART_Config.USART_baudrate	= USART_BAUDRATE_115200;
	handlerUsart2.USART_Config.USART_datasize	= USART_DATASIZE_8BIT;
	handlerUsart2.USART_Config.USART_parity		= USART_PARITY_NONE;
	handlerUsart2.USART_Config.USART_stopbits	= USART_STOPBIT_1;
	handlerUsart2.USART_Config.USART_mode		= USART_MODE_RXTX;
	USART_Config(&handlerUsart2);

	/* Cargando la configuración para la conversión ADC */
	adcConfig.channel 			= ADC_CHANNEL_0;
	adcConfig.dataAlignment		= ADC_ALIGNMENT_RIGHT;
	adcConfig.resolution		= ADC_RESOLUTION_12_BIT;
	adcConfig.samplingPeriod 	= ADC_SAMPLING_PERIOD_28_CYCLES;

	/* Configuramos el micro, la interrupción esta activa por defecto.*/
	adc_Config(&adcConfig);

}

/*
 * Callback del Timer2 - Hacemos un blinky....
 */
void Timer2_Callback(void){
	handlerBlinkyPin.pGPIOx -> ODR ^= GPIO_ODR_OD5;		// Encendido y apagado StateLED
}

/* Callback relacionado con la recepción del USART2
 * El puerto es leido en la ISR (para bajar la bandera de la interrupción)
 * El caracter que se lee es devuelto por la función getRxData
 */
void USART2_Callback(void){
	// Leemos el valor del registro DR, donde se almacena el dato que llega.
	// Esto además debe bajar la bandera de la interrupción
	rxData = returnData();
}


/* Esta función se ejecuta luego de una conversion ADC
 * (es llamada por la ISR de la conversion ADC)
 */
void adc_Complete_Callback(void){
	adcData = getADC();
	adcIsComplete = true;
}
