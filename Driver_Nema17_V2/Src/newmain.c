/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Juan Pablo Toro Arenas y Emmanuel Ángel Maya
 * @brief          : Desarrollo de la tarea 3
 ******************************************************************************
 * - Implementación de la programación orientada a interrupciones.
 * - El programa funciona de tal manera que:<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
 *
 ******************************************************************************
 */

// *************** // INCLUDES // *************** //
#include <stm32f4xx.h>
#include <stdint.h>
#include "GPIOxDriver.h"
#include "USARTxDriver.h"
#include "TIMxDriver.h"

// *************** // VARIABLES PROYECTO // *************** //

GPIO_Handler_t handlerStateLED 		= {0};	// StateLED
//GPIO_Handler_t handlerLEDPB9   	= {0};	// LED PB9
GPIO_Handler_t handlerUSB  			= {0};	// Pin USB PA3 RX
//GPIO_Handler_t handlerUSBTX  		= {0};	// Pin USB PA2 TX
TIMER_Handler_t handlerTimer2 		= {0};	// Timer2
//TIMER_Handler_t handlerTimer3 	= {0};	// Timer3
USART_Handler_t handlerUsartRX   	= {0};	// Usart RX
//USART_Handler_t handlerUsartTX   	= {0};	// Usart TX


GPIO_Handler_t handlerStep1Config 	= {0};
GPIO_Handler_t handlerDir1Config 	= {0};
GPIO_Handler_t handlerStep2Config 	= {0};
GPIO_Handler_t handlerDir2Config 	= {0};



uint32_t interruptor = 0;
uint32_t *ptr_interruptor;
char 	dataValue = '0';

uint32_t pasos = 1;								//Variable que define la cantidad de pasos
uint32_t delayTime = 5000;						//Tiempo de delay entre paso y paso del motor

// *************** // Headers // *************** //

void delay(int time);
void initSystem(void);
void motorConfig(uint32_t nMotor);

// *************** // MAIN // *************** //
int main(void)
{
	// Inicializamos el sistema
	initSystem();

	//Cargamos la configuración

	// Cargamos la configuración del PIN_B9 (Step1)
	GPIO_Config(&handlerStep1Config);

	// Cargamos la configuración del PIN_B8  (DIR1)
	GPIO_Config(&handlerDir1Config);

	// Cargamos la configuración del PIN_A6 (Step2)
	GPIO_Config(&handlerStep2Config);

	// Cargamos la configuración del PIN_A5  (DIR2)
	GPIO_Config(&handlerDir2Config);

	GPIO_Config(&handlerStateLED);
	//GPIO_Config(&handlerLEDPB9);

	GPIO_Config(&handlerUSB);
	//GPIO_Config(&handlerUSBTX);

	Timer_Config(&handlerTimer2);
	//Timer_Config(&handlerTimer3);

	USART_Config(&handlerUsartRX);
	//USART_Config(&handlerUsartTX);

	// Variable auxiliar para evaluar los casos de 'O'
	ptr_interruptor = &interruptor;
	*ptr_interruptor = 0;
	dataValue = '0';

    /* Ciclo principal */
	while(1){

		if(*ptr_interruptor == 1){

			handlerTimer2.timerConfig.Timer_period				= 1000;
			Timer_Config(&handlerTimer2);
			dataValue = '0';			// Borramos el valor de dataValue hasta que haya otra interrupción

		}else if(*ptr_interruptor == 2){							// Se oprimió dos veces 'O'

			handlerTimer2.timerConfig.Timer_period				= 500;
			Timer_Config(&handlerTimer2);
			dataValue = '0';			// Borramos el valor de dataValue hasta que haya otra interrupción

		}else if(*ptr_interruptor == 3){							// Se oprimió dos veces 'O'

			handlerTimer2.timerConfig.Timer_period				= 100;
			Timer_Config(&handlerTimer2);
			dataValue = '0';			// Borramos el valor de dataValue hasta que haya otra interrupción
		}
		//motorConfig(*ptr_interruptor);
	}
}

//***********// *********** // Definición de Funciones // *********** //***********//

//***********// Delay //***********//

void delay(int time){
	// Con este ciclo se genera un intervalo de tiempo en el programa (no tiene otro objetivo)
	for (int i = 0; i <= time; i++){
		__NOP();
	}
}

//***********// InitSystem //***********//

void initSystem(void){

	// Configuración el State LED
	handlerStateLED.pGPIOx								= GPIOA;
	handlerStateLED.GPIO_PinConfig.GPIO_PinNumber		= PIN_5;
	handlerStateLED.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerStateLED.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerStateLED.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerStateLED.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_MEDIUM;

	// Definimos el handler para la configuración del STEP del motor1

	handlerStep1Config.pGPIOx 									= GPIOB;					// Puerto a emplear: GPIOB
	handlerStep1Config.GPIO_PinConfig.GPIO_PinNumber			= PIN_9;					// Pin a emplear: PB9
	handlerStep1Config.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;			// Señal de salida
	handlerStep1Config.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerStep1Config.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_PULLUP;
	handlerStep1Config.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_MEDIUM;
	handlerStep1Config.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;						// Ninguna función

	// Definimos el handler para la dirección (DIR) del motor1

	handlerDir1Config.pGPIOx 									= GPIOB;					// Puerto a emplear: GPIOB
	handlerDir1Config.GPIO_PinConfig.GPIO_PinNumber				= PIN_8;					// Pin a emplear: PB8
	handlerDir1Config.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;			// Señal de salida
	handlerDir1Config.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerDir1Config.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_PULLUP;
	handlerDir1Config.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_MEDIUM;
	handlerDir1Config.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;						// Ninguna función


	// Definimos el handler para la configuración del STEP del motor2

	handlerStep2Config.pGPIOx 									= GPIOA;					// Puerto a emplear: GPIOA
	handlerStep2Config.GPIO_PinConfig.GPIO_PinNumber			= PIN_6;					// Pin a emplear: PA6
	handlerStep2Config.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;			// Señal de salida
	handlerStep2Config.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerStep2Config.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_PULLUP;
	handlerStep2Config.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_MEDIUM;
	handlerStep2Config.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;						// Ninguna función


	// Definimos el handler para la dirección del motor2

	handlerDir2Config.pGPIOx 									= GPIOA;					// Puerto a emplear: GPIOA
	handlerDir2Config.GPIO_PinConfig.GPIO_PinNumber				= PIN_5;					// Pin a emplear: PA5
	handlerDir2Config.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;			// Señal de salida
	handlerDir2Config.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerDir2Config.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_PULLUP;
	handlerDir2Config.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_MEDIUM;
	handlerDir2Config.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;						// Ninguna función

//	********************************************************************************************************************************

/*	// Configuración del led Pin B9
	handlerLEDPB9.pGPIOx								= GPIOB;
	handlerLEDPB9.GPIO_PinConfig.GPIO_PinNumber			= PIN_9;
	handlerLEDPB9.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerLEDPB9.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerLEDPB9.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerLEDPB9.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
*/

	// Configuración del pin PA3 USB
	handlerUSB.pGPIOx									= GPIOA;
	handlerUSB.GPIO_PinConfig.GPIO_PinNumber			= PIN_3;
	handlerUSB.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerUSB.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUSB.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerUSB.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_MEDIUM;
	handlerUSB.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF7;

/*
	// Configuración del pin PA2 USB
	handlerUSBTX.pGPIOx									= GPIOA;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinNumber			= PIN_2;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF7;
*/

	// Configuración del USART del USB para RX
	handlerUsartRX.ptrUSARTx							= USART2;
	handlerUsartRX.USART_Config.USART_mode				= USART_MODE_RX;
	handlerUsartRX.USART_Config.USART_baudrate			= USART_BAUDRATE_19200;
	handlerUsartRX.USART_Config.USART_datasize			= USART_DATASIZE_8BIT;
	handlerUsartRX.USART_Config.USART_parity			= USART_PARITY_EVEN;
	handlerUsartRX.USART_Config.USART_stopbits			= USART_STOPBIT_1;

/*	// Configuración del USART del USB para TX
	handlerUsartTX.ptrUSARTx							= USART2;
	handlerUsartTX.USART_Config.USART_mode				= USART_MODE_RXTX;
	handlerUsartTX.USART_Config.USART_baudrate			= USART_BAUDRATE_19200;
	handlerUsartTX.USART_Config.USART_datasize			= USART_DATASIZE_8BIT;
	handlerUsartTX.USART_Config.USART_parity			= USART_PARITY_NONE;
	handlerUsartTX.USART_Config.USART_stopbits			= USART_STOPBIT_1;
*/

	// Configuración del timer2
	handlerTimer2.ptrTIMx								= TIM2;
	handlerTimer2.timerConfig.Timer_mode				= TIMER_MODE_UP;
	handlerTimer2.timerConfig.Timer_speed				= TIMER_INCR_SPEED_1ms;
	handlerTimer2.timerConfig.Timer_period				= 10;

/*
	// Configuración del timer3
	handlerTimer3.ptrTIMx								= TIM3;
	handlerTimer3.timerConfig.Timer_mode				= TIMER_MODE_UP;
	handlerTimer3.timerConfig.Timer_speed				= TIMER_INCR_SPEED_1ms;
	handlerTimer3.timerConfig.Timer_period				= 300;
*/
}

//************// Función motor 1 //************//

//************// Inicio Motor 1 //************//


void motorConfig(uint32_t nMotor){

	// Configuración motor 1

	// Condición: Pulsador de Giro en sentido ++
	if((nMotor == 1) & (dataValue == 'd')){

		GPIO_WritePin(&handlerDir1Config, RESET);			//Rotación en sentido ++
		for(uint32_t i = 0; i < pasos; i++){
		GPIO_WritePin(&handlerStep1Config, SET);		//Enciende el motor
		delay(delayTime);								//Tiempo de espera entre paso y paso

		GPIO_WritePin(&handlerStep1Config, RESET);		//Apaga el motor
		delay(delayTime);								//Tiempo de espera entre paso y paso
		}
	}

	// Condición: Pulsador de Giro en sentido --
	if((nMotor == 1) & (dataValue == 'a')){

		GPIO_WritePin(&handlerDir1Config, SET);				//Rotación en sentido --
		for(uint32_t i = 0; i < pasos; i++){
		GPIO_WritePin(&handlerStep1Config, SET);		//Enciende el motor
		delay(delayTime);								//Tiempo de espera entre paso y paso

		GPIO_WritePin(&handlerStep1Config, RESET);		//Apaga el motor
		delay(delayTime);								//Tiempo de espera entre paso y paso

		}
	}

	//************// Fin Motor 1 //************//

	//************// Inicio Motor 2 //************//

	// Condición: Pulsador de Giro en sentido ++
	if((nMotor == 2) & (dataValue == 'd')){

		GPIO_WritePin(&handlerDir1Config, RESET);			//Rotación en sentido ++
		for(uint32_t i = 0; i < pasos; i++){
		GPIO_WritePin(&handlerStep1Config, SET);		//Enciende el motor
		delay(delayTime);								//Tiempo de espera entre paso y paso

		GPIO_WritePin(&handlerStep1Config, RESET);		//Apaga el motor
		delay(delayTime);								//Tiempo de espera entre paso y paso
		}
	}

	// Condición: Pulsador de Giro en sentido --
	if((nMotor == 2) & (dataValue == 'a')){

		GPIO_WritePin(&handlerDir1Config, SET);				//Rotación en sentido --
		for(uint32_t i = 0; i < pasos; i++){
		GPIO_WritePin(&handlerStep1Config, SET);		//Enciende el motor
		delay(delayTime);								//Tiempo de espera entre paso y paso

		GPIO_WritePin(&handlerStep1Config, RESET);		//Apaga el motor
		delay(delayTime);								//Tiempo de espera entre paso y paso
		}
	}else{

	}
}

//***********// CallBacks //***********//

void Timer2_Callback(void){
	handlerStateLED.pGPIOx -> ODR ^= GPIO_ODR_OD5;		// Encendido y apagado StateLED
}
/*
void Timer3_Callback(void){
	handlerLEDPB9.pGPIOx -> ODR ^= GPIO_ODR_OD9;		// Encendido y apagado LED PB9
}
*/
void USART2_Callback(void){
	// Lectura de los datos recibidos
	dataValue = readChar(&handlerUsartRX);

	// Comparación de los valores recibidos.
	if(dataValue == 'h'){
		*ptr_interruptor = 1;				// Acción cada vez que se oprima el interruptor
	}else if(dataValue == 'j'){
		*ptr_interruptor = 2;				// Acción cada vez que se oprima el interruptor
	}else if(dataValue == 'k'){
		*ptr_interruptor = 3;				// Acción cada vez que se oprima el interruptor
	}

	motorConfig(*ptr_interruptor);
	//writeChar(&handlerUsartTX, dataValue);
}
