/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Juan Pablo Toro Arenas
 * @brief          : Controlador motores
 ******************************************************************************
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
TIMER_Handler_t handlerTimer2 		= {0};	// Timer2
GPIO_Handler_t 	handlerUSBRX 		= {0};	// Pin USB PA3 RX
GPIO_Handler_t 	handlerUSBTX 		= {0};	// Pin USB PA3 RX
USART_Handler_t handlerUsart2   	= {0};	// Usart RX

//GPIO_Handler_t handlerUSBTX  		= {0};	// Pin USB PA2 TX
//USART_Handler_t handlerUsartTX   	= {0};	// Usart TX


GPIO_Handler_t handlerStep1Config 	= {0};		// Motor Base
GPIO_Handler_t handlerDir1Config 	= {0};
GPIO_Handler_t handlerStep2Config 	= {0};		// Motor articulación hombro x2
GPIO_Handler_t handlerDir2Config 	= {0};
GPIO_Handler_t handlerStep3Config 	= {0};		// Motor articulación codo x1
GPIO_Handler_t handlerDir3Config 	= {0};

// Variable auxiliar para evaluar los casos de cada motor

uint32_t nMotor = 0;
char 	dataValue 	 = '\0';
uint32_t pasos = 5;								//Variable que define la cantidad de pasos
uint32_t delayTime = 2000;						//Tiempo de delay entre paso y paso del motor
uint32_t auxLedState = 0;

// *************** // Headers // *************** //


void delay(int time);
void initSystem(void);
void motorConfig(void);
void ledControl(void);

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
	GPIO_Config(&handlerUSBRX);
	GPIO_Config(&handlerUSBTX);
	Timer_Config(&handlerTimer2);
	USART_Config(&handlerUsart2);

	//GPIO_Config(&handlerUSBTX);
	//USART_Config(&handlerUsartTX);

    /* Ciclo principal */
	while(1){

		if(dataValue != '\0'){
			writeChar(&handlerUsart2,dataValue);
			ledControl();
			motorConfig();
			dataValue = '\0';
		}
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

	handlerStep1Config.pGPIOx 									= GPIOB;					// Puerto a emplear: GPIOC
	handlerStep1Config.GPIO_PinConfig.GPIO_PinNumber			= PIN_8;					// Pin a emplear: PC9
	handlerStep1Config.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;			// Señal de salida
	handlerStep1Config.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerStep1Config.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_PULLUP;
	handlerStep1Config.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_MEDIUM;
	handlerStep1Config.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;						// Ninguna función

	// Definimos el handler para la dirección (DIR) del motor1

	handlerDir1Config.pGPIOx 									= GPIOC;					// Puerto a emplear: GPIOC
	handlerDir1Config.GPIO_PinConfig.GPIO_PinNumber				= PIN_6;					// Pin a emplear: PC8
	handlerDir1Config.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;			// Señal de salida
	handlerDir1Config.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerDir1Config.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_PULLUP;
	handlerDir1Config.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_MEDIUM;
	handlerDir1Config.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;						// Ninguna función


	// Definimos el handler para la configuración del STEP del motor2

	handlerStep2Config.pGPIOx 									= GPIOA;					// Puerto a emplear: GPIOB
	handlerStep2Config.GPIO_PinConfig.GPIO_PinNumber			= PIN_12;					// Pin a emplear: PB8
	handlerStep2Config.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;			// Señal de salida
	handlerStep2Config.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerStep2Config.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_PULLUP;
	handlerStep2Config.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_MEDIUM;
	handlerStep2Config.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;						// Ninguna función


	// Definimos el handler para la dirección del motor2

	handlerDir2Config.pGPIOx 									= GPIOA;					// Puerto a emplear: GPIOC
	handlerDir2Config.GPIO_PinConfig.GPIO_PinNumber				= PIN_11;					// Pin a emplear: PC6
	handlerDir2Config.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;			// Señal de salida
	handlerDir2Config.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerDir2Config.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_PULLUP;
	handlerDir2Config.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_MEDIUM;
	handlerDir2Config.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;						// Ninguna función

	// Definimos el handler para la configuración del STEP del motor3

	handlerStep3Config.pGPIOx 									= GPIOC;					// Puerto a emplear: GPIOB
	handlerStep3Config.GPIO_PinConfig.GPIO_PinNumber			= PIN_1;					// Pin a emplear: PB9
	handlerStep3Config.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;			// Señal de salida
	handlerStep3Config.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerStep3Config.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_PULLUP;
	handlerStep3Config.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_MEDIUM;
	handlerStep3Config.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;						// Ninguna función


	// Definimos el handler para la dirección del motor2

	handlerDir3Config.pGPIOx 									= GPIOC;					// Puerto a emplear: GPIOC
	handlerDir3Config.GPIO_PinConfig.GPIO_PinNumber				= PIN_5;					// Pin a emplear: PC5
	handlerDir3Config.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;			// Señal de salida
	handlerDir3Config.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerDir3Config.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_PULLUP;
	handlerDir3Config.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_MEDIUM;
	handlerDir3Config.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;						// Ninguna función

//	********************************************************************************************************************************


	// Configuración del pin PA3 USB
	handlerUSBRX.pGPIOx									= GPIOA;
	handlerUSBRX.GPIO_PinConfig.GPIO_PinNumber			= PIN_3;
	handlerUSBRX.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerUSBRX.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUSBRX.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerUSBRX.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUSBRX.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF7;


	// Configuración del pin PA2 USB
	handlerUSBTX.pGPIOx									= GPIOA;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinNumber			= PIN_2;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_MEDIUM;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF7;

	// Configuración del USART del USB para RXTX
	handlerUsart2.ptrUSARTx								= USART2;
	handlerUsart2.USART_Config.USART_mode				= USART_MODE_RXTX;
	handlerUsart2.USART_Config.USART_baudrate			= USART_BAUDRATE_115200;
	handlerUsart2.USART_Config.USART_datasize			= USART_DATASIZE_8BIT;
	handlerUsart2.USART_Config.USART_parity				= USART_PARITY_ODD;
	handlerUsart2.USART_Config.USART_stopbits			= USART_STOPBIT_1;
	handlerUsart2.USART_Config.USART_interrupt 			= USART_INTERRUPT_RX_ENABLE;

	// Configuración del timer2
	handlerTimer2.ptrTIMx								= TIM2;
	handlerTimer2.timerConfig.Timer_mode				= TIMER_MODE_UP;
	handlerTimer2.timerConfig.Timer_speed				= TIMER_INCR_SPEED_1ms;
	handlerTimer2.timerConfig.Timer_period				= 10;
}

//************// Función motor 1 //************//

void motorConfig(void){

	//************// Inicio Motor 1 //************//

	// Configuración motor 1

	// Condición: Pulsador de Giro en sentido ++
	if((nMotor == 1) ){

		if(dataValue == 'a'){
			GPIO_WritePin(&handlerDir1Config, RESET);			//Rotación en sentido ++
			for(uint32_t i = 0; i < pasos; i++){
				GPIO_WritePin(&handlerStep1Config, SET);		//Enciende el motor
				delay(delayTime);								//Tiempo de espera entre paso y paso

				GPIO_WritePin(&handlerStep1Config, RESET);		//Apaga el motor
				//delay(delayTime);								//Tiempo de espera entre paso y paso
			}
		}
	// Condición: Pulsador de Giro en sentido --
		if(dataValue == 'd'){
			GPIO_WritePin(&handlerDir1Config, SET);				//Rotación en sentido --
			for(uint32_t i = 0; i < pasos; i++){
				GPIO_WritePin(&handlerStep1Config, SET);		//Enciende el motor
				delay(delayTime);								//Tiempo de espera entre paso y paso

				GPIO_WritePin(&handlerStep1Config, RESET);		//Apaga el motor
				delay(delayTime);								//Tiempo de espera entre paso y paso
			}
		}
	}
	//************// Fin Motor 1 //************//

	//************// Inicio Motor 2 //************//

	// Condición: Pulsador de Giro en sentido ++
	if(nMotor == 2){

		if(dataValue == 'a'){
			GPIO_WritePin(&handlerDir2Config, RESET);			//Rotación en sentido ++
			for(uint32_t i = 0; i < pasos; i++){
				GPIO_WritePin(&handlerStep2Config, SET);		//Enciende el motor
				delay(delayTime);								//Tiempo de espera entre paso y paso

				GPIO_WritePin(&handlerStep2Config, RESET);		//Apaga el motor
				delay(delayTime);								//Tiempo de espera entre paso y paso
			}
		}

		// Condición: Pulsador de Giro en sentido --
		if(dataValue == 'd'){

			GPIO_WritePin(&handlerDir2Config, SET);				//Rotación en sentido --
			for(uint32_t i = 0; i < pasos; i++){
				GPIO_WritePin(&handlerStep2Config, SET);		//Enciende el motor
				delay(delayTime);								//Tiempo de espera entre paso y paso

				GPIO_WritePin(&handlerStep2Config, RESET);		//Apaga el motor
				delay(delayTime);								//Tiempo de espera entre paso y paso
			}
		}
	}

	//************// Fin Motor 2 //************//

	//************// Inicio Motor 3 //************//

	if(nMotor == 3){

		if(dataValue == 'a'){
			GPIO_WritePin(&handlerDir3Config, RESET);			//Rotación en sentido ++
			for(uint32_t i = 0; i < pasos; i++){
				GPIO_WritePin(&handlerStep3Config, SET);		//Enciende el motor
				delay(delayTime);								//Tiempo de espera entre paso y paso

				GPIO_WritePin(&handlerStep3Config, RESET);		//Apaga el motor
				delay(delayTime);								//Tiempo de espera entre paso y paso
			}
		}

		// Condición: Pulsador de Giro en sentido --
		if(dataValue == 'd'){

			GPIO_WritePin(&handlerDir3Config, SET);				//Rotación en sentido --
			for(uint32_t i = 0; i < pasos; i++){
				GPIO_WritePin(&handlerStep3Config, SET);		//Enciende el motor
				delay(delayTime);								//Tiempo de espera entre paso y paso

				GPIO_WritePin(&handlerStep3Config, RESET);		//Apaga el motor
				delay(delayTime);								//Tiempo de espera entre paso y paso
			}
		}
	}

	//nMotor = 0;
	dataValue = '\0';
}

//***********// LED_CONTROL //***********//

void ledControl(void){
	if(auxLedState == 1){

		handlerTimer2.timerConfig.Timer_period				= 400;
		Timer_Config(&handlerTimer2);
		auxLedState = 0;

	}else if(auxLedState == 2){							// Se oprimió dos veces 'O'

		handlerTimer2.timerConfig.Timer_period				= 200;
		Timer_Config(&handlerTimer2);
		auxLedState = 0;


	}else if(auxLedState == 3){							// Se oprimió dos veces 'O'

		handlerTimer2.timerConfig.Timer_period				= 50;
		Timer_Config(&handlerTimer2);
		auxLedState = 0;

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
	dataValue = readChar(&handlerUsart2);

	// Comparación de los valores recibidos.
	if(dataValue == '1'){
		nMotor = 1;				// Acción cada vez que se oprima el interruptor
		auxLedState = 1;
	}else if(dataValue == '2'){
		nMotor = 2;				// Acción cada vez que se oprima el interruptor
		auxLedState = 2;
	}else if(dataValue == '3'){
		nMotor = 3;				// Acción cada vez que se oprima el interruptor
		auxLedState = 3;
	}

	//*********************
	//ledControl();
	//************************
	//motorConfig(*ptr_interruptor);
	//writeChar(&handlerUsartTX, dataValue);
}
