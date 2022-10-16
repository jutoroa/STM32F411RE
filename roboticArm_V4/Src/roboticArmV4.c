/*
 * I2CMPU6050.c
 *
 *  Created on: 4/05/2022
 *  Author: Juan Pablo
 *
 * // *************** // Proyecto: Brazo robótico // *************** //
 */


// *************** // INCLUDES // *************** //

#include <stm32f4xx.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "GPIOxDriver.h"
#include "USARTxDriver.h"
#include "TIMxDriver.h"
#include "AdcDriver.h"
#include "EXTIxDriver.h"
#include "I2CxDriver.h"
#include "SysTickDriver.h"
#include "PWMxDriver.h"
#include "MPU6050Driver.h"
#include "rtcDS1307Driver.h"
#include "OLEDDriver.h"
#include "FPUDriver.h"

// *************** // VARIABLES PROYECTO // *************** //

GPIO_Handler_t handlerStateLED 		= {0};	// StateLED
GPIO_Handler_t handlerStateLED2 		= {0};	// StateLED2
TIMER_Handler_t handlerTimer2 		= {0};	// Timer2
TIMER_Handler_t handlerTimer3 		= {0};	// Timer3
TIMER_Handler_t handlerTimer4 		= {0};	// Timer4

GPIO_Handler_t 	handlerUSBRX 			= {0};	// Pin USB PA2 RX
GPIO_Handler_t 	handlerUSBTX 			= {0};	// Pin USB PA3 RX
USART_Handler_t handlerCommTerminal 	= {0};	// Usart2

GPIO_Handler_t handlerUsartBluetoothRX 	= {0};	// Pin Bluetooth RX
GPIO_Handler_t handlerUsartBluetoothTX 	= {0};	// Pin Bluetooth TX
USART_Handler_t handlerUsartBluetooth 	= {0};	// Usart6 para la conexión Bluetooth

/* Configuración para los motores paso a paso */

GPIO_Handler_t handlerStep1Config 	= {0};		// Motor Base
GPIO_Handler_t handlerDir1Config 	= {0};
GPIO_Handler_t handlerStep2Config 	= {0};		// Motor articulación hombro x2
GPIO_Handler_t handlerDir2Config 	= {0};
GPIO_Handler_t handlerStep3Config 	= {0};		// Motor articulación codo x1
GPIO_Handler_t handlerDir3Config 	= {0};

// Variable auxiliar para evaluar los casos de cada motor

uint32_t 	nMotor 			= 0;
uint8_t 	dataValue 	 	= '\0';
uint32_t 	pasos 			= 5;						//Variable que define la cantidad de pasos
uint32_t 	delayTime 		= 1;						//Tiempo de delay entre paso y paso del motor en ms
uint32_t 	auxLedState 	= 0;


/* Configuración para los servomotores */
GPIO_Handler_t	handlerPinServo1		= {0};	// Pin para el servo1
GPIO_Handler_t	handlerPinServo2		= {0};	// Pin para el servo1
GPIO_Handler_t	handlerPinServo3		= {0};	// Pin para el servo1
PWM_Handler_t	handlerPWMServo1		= {0};	// PWM para los servos
PWM_Handler_t	handlerPWMServo2		= {0};	// PWM para los servos
PWM_Handler_t	handlerPWMServo3		= {0};	// PWM para los servos

ADC_Config_t 	handlerADC				= {0};	// ADC

uint8_t 	PWMBuffer 			= 0;
uint16_t	initialDuty			= 200;

/* Configuración para el I2C */

GPIO_Handler_t handlerI2CSDA_OLED 				= {0};	// SDA para el I2C	OLED 1.3"
GPIO_Handler_t handlerI2CSCL_OLED 				= {0};	// SCL para el I2C	OLED 1.3"
I2C_Handler_t  handler_OLED	 					= {0};	// I2C (3) para el  OLED 1.3"

/* Variables para la conversión ADC */

uint8_t		adcIsComplete   = false;			// Variable que nos permite saber el estado de si se completó la conversión ADC
uint16_t 	adcData			= 0;				// Valor obtenido por el ADC

/* Variables auxiliares */

#define numberOfData 		  3
unsigned int dataValueBluetoth[numberOfData] 	= {0};
uint16_t dataPosition 	= 0;
uint8_t MPU6050IsReady = false;
int dataPotX = 0;
int dataPotY = 0;
uint32_t counterAux = 0;

/* Configuración para la OLED */
char 		bufferDataOLED[128] = {0};

uint8_t 	i2cBuffer 				= 0;				// Buffer para el I2C
uint16_t counterStateLED			= 0;
int  counterDelay 					= 0;
char bufferData[256] 				= "¡Hola! Soy el USART del STM32 y estoy funcionando.";		// Mensaje de Bienvenida

/* Variables para los commands */
uint8_t 	rxData					= '\0';				// Variable donde se guardarán los datos obtenidos por el RX
char		bufferReception[256];						// Buffer de recepción
char 		cmd[64];									// Buffer para el cmd
char 		userMsg[256];								// Buffer para el mensaje del usuario
int duty_x			= 0;
int duty_y			= 0;
int duty_ADC		= 0;

uint32_t		counterReception 		= 0;
uint8_t			stringComplete			= false;

// *************** // Headers // *************** //

void delay(int time);
void initSystem(void);
void motorConfig(void);
void ledControl(void);

// *************** // MAIN // *************** //
int main(void)
{

	// Inicializamos la FPU
	FPUInit();
	// Inicializamos el sistema
	initSystem();
	writeMsg(&handlerCommTerminal,bufferData);

	// Cargamos la configuración Inicial en la OLED
	OLED_Init(&handler_OLED);
	OLED_Clean(&handler_OLED);
	sprintf(bufferDataOLED, "<<<<<<<<<<<<<<<<  HOLA!          SOY EL STM32   <<<<<<<<<<<<<<<<");
	OLED_FPrint(&handler_OLED, bufferDataOLED);

    /* Ciclo principal */
	while(1){

		if(counterStateLED >= 250){
			handlerStateLED2.pGPIOx -> ODR ^= GPIO_ODR_OD7;		// Encendido y apagado StateLED
			counterStateLED = 0;
		}

//		if(dataValue != '\0'){
//		writeChar(&handlerCommTerminal,dataValue);
//		// Comparación de los valores recibidos.
//		if(dataValue == '1'){
//			nMotor = 1;				// Acción cada vez que se oprima el interruptor
//			auxLedState = 1;
//		}else if(dataValue == '2'){
//			nMotor = 2;				// Acción cada vez que se oprima el interruptor
//			auxLedState = 2;
//		}else if(dataValue == '3'){
//			nMotor = 3;				// Acción cada vez que se oprima el interruptor
//			auxLedState = 3;
//		}
//	}
		if(rxData != '\0'){
//			writeChar(&handlerCommTerminal,rxData);
			//Guardamos las cadenas para los comandos
			bufferReception[counterReception] = rxData;
			counterReception++;

			// Levantamos una bandera para indicar que se completó el comando
			if(rxData == '='){
				stringComplete = true;
				// Agrego esta línea para crear el string con el null al final del arreglo
				bufferReception[counterReception] = '\0';
				counterReception = 0;
//				writeMsg(&handlerCommTerminal,bufferReception);

			}
			rxData = '\0';
		}

		// Hacemos un análisis de la cadena de datos obtenida
		if(stringComplete){
			// Llamamos a la función para implementar los comandos
			sscanf(bufferReception, "%d %d %d %d %d", &duty_x,&duty_y,&duty_ADC, &dataPotX, &dataPotY);

			sprintf(bufferData,"%d %d %d %d %d \n",duty_x, duty_y, duty_ADC, dataPotX, dataPotY);
			writeMsg(&handlerCommTerminal,bufferData);

			stringComplete = false;

			setDutty(&handlerPWMServo1, duty_x);
			setDutty(&handlerPWMServo2, duty_y);
			setDutty(&handlerPWMServo3, duty_ADC);

			counterAux++;
		}

		if(dataPotX != 5){
			nMotor = 2;				// Acción cada vez que se oprima el interruptor
			auxLedState = 2;

			if(dataPotX == 0){
				dataValue = 'a';
			}
			else if(dataPotX == 1){
				dataValue = 'd';
			}
			else{
				dataValue = '\0';
			}
		}

		if(dataPotY != 5){
			nMotor = 1;				// Acción cada vez que se oprima el interruptor
			auxLedState = 1;
			if(dataPotY == 0){
				dataValue = 'a';
			}
			else if(dataPotY == 1){
				dataValue = 'd';
			}
			else{
				dataValue = '\0';
			}
		}
		if(dataPotX != 5 && dataPotY != 5){
			nMotor = 3;				// Acción cada vez que se oprima el interruptor
			auxLedState = 3;
			if(dataPotX == 0 && dataPotY == 0){
				dataValue = 'a';
			}
			else if(dataPotX == 1 && dataPotY == 1){
				dataValue = 'd';
			}
			else{
				dataValue = '\0';
			}
		}
//			counterAux = 0;
		ledControl();

		if(counterAux >= 2){
			motorConfig();
			counterAux = 0;
		}

		dataValue = '\0';

	}
}

//***********// *********** // Definición de Funciones // *********** //***********//

//***********// Delay //***********//

void delay(int time){
	counterDelay = 0;
	while(counterDelay <= time){
		__NOP();
	}
	counterDelay = 0;
}

//***********// InitSystem //***********//

void initSystem(void){

	// Configuración el State LED
	handlerStateLED.pGPIOx								= GPIOA;
	handlerStateLED.GPIO_PinConfig.GPIO_PinNumber		= PIN_5;
	handlerStateLED.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerStateLED.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerStateLED.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerStateLED.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_HIGH;

	GPIO_Config(&handlerStateLED);

	// Configuración el State LED
	handlerStateLED2.pGPIOx								= GPIOB;
	handlerStateLED2.GPIO_PinConfig.GPIO_PinNumber		= PIN_7;
	handlerStateLED2.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_OUT;
	handlerStateLED2.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerStateLED2.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerStateLED2.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_HIGH;

	GPIO_Config(&handlerStateLED2);

	// Definimos el handler para la configuración del STEP del motor1

	handlerStep1Config.pGPIOx 									= GPIOB;					// Puerto a emplear: GPIOC
	handlerStep1Config.GPIO_PinConfig.GPIO_PinNumber			= PIN_9;					// Pin a emplear: PC9
	handlerStep1Config.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;			// Señal de salida
	handlerStep1Config.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerStep1Config.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_PULLUP;
	handlerStep1Config.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_HIGH;
	handlerStep1Config.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;						// Ninguna función

	// Cargamos la configuración del PIN_B9 (Step1)
	GPIO_Config(&handlerStep1Config);

	// Definimos el handler para la dirección (DIR) del motor1

	handlerDir1Config.pGPIOx 									= GPIOC;					// Puerto a emplear: GPIOC
	handlerDir1Config.GPIO_PinConfig.GPIO_PinNumber				= PIN_9;					// Pin a emplear: PC8
	handlerDir1Config.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;			// Señal de salida
	handlerDir1Config.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerDir1Config.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_PULLUP;
	handlerDir1Config.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_HIGH;
	handlerDir1Config.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;						// Ninguna función

	// Cargamos la configuración del PIN_B8  (DIR1)
	GPIO_Config(&handlerDir1Config);

	// Definimos el handler para la configuración del STEP del motor2

	handlerStep2Config.pGPIOx 									= GPIOC;					// Puerto a emplear: GPIOB
	handlerStep2Config.GPIO_PinConfig.GPIO_PinNumber			= PIN_8;					// Pin a emplear: PB9
	handlerStep2Config.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;			// Señal de salida
	handlerStep2Config.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerStep2Config.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_PULLUP;
	handlerStep2Config.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_HIGH;
	handlerStep2Config.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;						// Ninguna función

	// Cargamos la configuración del PIN_A6 (Step2)
	GPIO_Config(&handlerStep2Config);

	// Definimos el handler para la dirección del motor2

	handlerDir2Config.pGPIOx 									= GPIOC;
	handlerDir2Config.GPIO_PinConfig.GPIO_PinNumber				= PIN_6;
	handlerDir2Config.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerDir2Config.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerDir2Config.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_PULLUP;
	handlerDir2Config.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_HIGH;
	handlerDir2Config.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;

	// Cargamos la configuración del PIN_A5  (DIR2)
	GPIO_Config(&handlerDir2Config);

	// Definimos el handler para la configuración del STEP del motor3

	handlerStep3Config.pGPIOx 									= GPIOB;
	handlerStep3Config.GPIO_PinConfig.GPIO_PinNumber			= PIN_4;
	handlerStep3Config.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerStep3Config.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerStep3Config.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_PULLUP;
	handlerStep3Config.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_HIGH;
	handlerStep3Config.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;

	GPIO_Config(&handlerStep3Config);

	// Definimos el handler para la dirección del motor3

	handlerDir3Config.pGPIOx 									= GPIOB;
	handlerDir3Config.GPIO_PinConfig.GPIO_PinNumber				= PIN_5;
	handlerDir3Config.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerDir3Config.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerDir3Config.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_PULLUP;
	handlerDir3Config.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_HIGH;
	handlerDir3Config.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;

	GPIO_Config(&handlerDir3Config);

	// Definimos el handler para el servomotor 1 - Timer5 Channel 1

	handlerPinServo1.pGPIOx 									= GPIOA;
	handlerPinServo1.GPIO_PinConfig.GPIO_PinNumber				= PIN_6;
	handlerPinServo1.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinServo1.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerPinServo1.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerPinServo1.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_HIGH;
	handlerPinServo1.GPIO_PinConfig.GPIO_PinAltFunMode			= AF2;

	GPIO_Config(&handlerPinServo1);

	// Definimos el handler para el servomotor 2 - Timer5 Channel 2

	handlerPinServo2.pGPIOx 									= GPIOA;
	handlerPinServo2.GPIO_PinConfig.GPIO_PinNumber				= PIN_7;
	handlerPinServo2.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinServo2.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerPinServo2.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerPinServo2.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_HIGH;
	handlerPinServo2.GPIO_PinConfig.GPIO_PinAltFunMode			= AF2;

	GPIO_Config(&handlerPinServo2);

	// Definimos el handler para el servomotor 3 - Timer5 Channel 4

	handlerPinServo3.pGPIOx 									= GPIOB;
	handlerPinServo3.GPIO_PinConfig.GPIO_PinNumber				= PIN_1;
	handlerPinServo3.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinServo3.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerPinServo3.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerPinServo3.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_HIGH;
	handlerPinServo3.GPIO_PinConfig.GPIO_PinAltFunMode			= AF2;

	GPIO_Config(&handlerPinServo3);

	// Definimos el handler para el servomotor 1 - Timer5 Channel 1
	handlerPWMServo1.ptrTIMx								= TIM3;
	handlerPWMServo1.timerConfig.pwmMode					= TIMER_MODE_UP;
	handlerPWMServo1.timerConfig.pwmSpeed					= TIMER_INCR_SPEED_10us;
	handlerPWMServo1.timerConfig.pwmPeriod					= 2000;
	handlerPWMServo1.timerConfig.pwmDuty					= initialDuty;
	handlerPWMServo1.timerConfig.channel					= PWM_CHANNEL_1;

	PWM_Config(&handlerPWMServo1);

	// Definimos el handler para el servomotor 2 - Timer5 Channel 2
	handlerPWMServo2.ptrTIMx								= TIM3;
	handlerPWMServo2.timerConfig.pwmMode					= TIMER_MODE_UP;
	handlerPWMServo2.timerConfig.pwmSpeed					= TIMER_INCR_SPEED_10us;
	handlerPWMServo2.timerConfig.pwmPeriod					= 2000;
	handlerPWMServo2.timerConfig.pwmDuty					= initialDuty;
	handlerPWMServo2.timerConfig.channel					= PWM_CHANNEL_2;

	PWM_Config(&handlerPWMServo2);

	// Definimos el handler para el servomotor 3 - Timer5 Channel 3
	handlerPWMServo3.ptrTIMx								= TIM3;
	handlerPWMServo3.timerConfig.pwmMode					= TIMER_MODE_UP;
	handlerPWMServo3.timerConfig.pwmSpeed					= TIMER_INCR_SPEED_10us;
	handlerPWMServo3.timerConfig.pwmPeriod					= 2000;
	handlerPWMServo3.timerConfig.pwmDuty					= initialDuty;
	handlerPWMServo3.timerConfig.channel					= PWM_CHANNEL_4;

	PWM_Config(&handlerPWMServo3);

	// Configuración del ADC
	handlerADC.channel									= ADC_CHANNEL_8;
	handlerADC.resolution								= ADC_RESOLUTION_12_BIT;
	handlerADC.samplingPeriod							= ADC_SAMPLING_PERIOD_28_CYCLES;
	handlerADC.dataAlignment							= ADC_ALIGNMENT_RIGHT;
	handlerADC.numberOfChannels							= 1;
	handlerADC.channelMode								= ADC_SINGLE_CHANNEL;
	adc_Config(&handlerADC);

//	********************************************************************************************************************************


	// Configuración del pin PA3 USB RX
	handlerUSBRX.pGPIOx									= GPIOA;
	handlerUSBRX.GPIO_PinConfig.GPIO_PinNumber			= PIN_3;
	handlerUSBRX.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerUSBRX.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUSBRX.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerUSBRX.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerUSBRX.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF7;

	GPIO_Config(&handlerUSBRX);

	// Configuración del pin PA2 USB TX
	handlerUSBTX.pGPIOx									= GPIOA;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinNumber			= PIN_2;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerUSBTX.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF7;

	GPIO_Config(&handlerUSBTX);

	// Configuración del pin PA3 USB RX Bluetooth
	handlerUsartBluetoothRX.pGPIOx									= GPIOA;
	handlerUsartBluetoothRX.GPIO_PinConfig.GPIO_PinNumber			= PIN_12;
	handlerUsartBluetoothRX.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerUsartBluetoothRX.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUsartBluetoothRX.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerUsartBluetoothRX.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerUsartBluetoothRX.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF8;

	GPIO_Config(&handlerUsartBluetoothRX);

	// Configuración del pin PA2 USB TX Bluetooth
	handlerUsartBluetoothTX.pGPIOx									= GPIOA;
	handlerUsartBluetoothTX.GPIO_PinConfig.GPIO_PinNumber			= PIN_11;
	handlerUsartBluetoothTX.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerUsartBluetoothTX.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerUsartBluetoothTX.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerUsartBluetoothTX.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerUsartBluetoothTX.GPIO_PinConfig.GPIO_PinAltFunMode 		= AF8;

	GPIO_Config(&handlerUsartBluetoothTX);

	// Configuración del USART del USB para RXTX
	handlerCommTerminal.ptrUSARTx							= USART2;
	handlerCommTerminal.USART_Config.USART_mode				= USART_MODE_RXTX;
	handlerCommTerminal.USART_Config.USART_baudrate			= USART_BAUDRATE_38400;
	handlerCommTerminal.USART_Config.USART_datasize			= USART_DATASIZE_8BIT;
	handlerCommTerminal.USART_Config.USART_parity			= USART_PARITY_NONE;
	handlerCommTerminal.USART_Config.USART_stopbits			= USART_STOPBIT_1;
	handlerCommTerminal.USART_Config.USART_interrupt 		= USART_INTERRUPT_RX_ENABLE;

	USART_Config(&handlerCommTerminal);


	// Configuración del USART Bluetooth para la conexión a través del HC-05
	handlerUsartBluetooth.ptrUSARTx							= USART6;
	handlerUsartBluetooth.USART_Config.USART_mode			= USART_MODE_RXTX;
	handlerUsartBluetooth.USART_Config.USART_baudrate		= USART_BAUDRATE_38400;
	handlerUsartBluetooth.USART_Config.USART_datasize		= USART_DATASIZE_8BIT;
	handlerUsartBluetooth.USART_Config.USART_parity			= USART_PARITY_NONE;
	handlerUsartBluetooth.USART_Config.USART_stopbits		= USART_STOPBIT_1;
	handlerUsartBluetooth.USART_Config.USART_interrupt 		= USART_INTERRUPT_RX_ENABLE;

	USART_Config(&handlerUsartBluetooth);

	// Configuración del timer2
	handlerTimer2.ptrTIMx								= TIM2;
	handlerTimer2.timerConfig.Timer_mode				= TIMER_MODE_UP;
	handlerTimer2.timerConfig.Timer_speed				= TIMER_INCR_SPEED_1ms;
	handlerTimer2.timerConfig.Timer_period				= 10;

	// Configuramos los pines para el I2C SCL para el OLED
	handlerI2CSCL_OLED.pGPIOx								= GPIOA;
	handlerI2CSCL_OLED.GPIO_PinConfig.GPIO_PinNumber		= PIN_8;
	handlerI2CSCL_OLED.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerI2CSCL_OLED.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_OPENDRAIN;
	handlerI2CSCL_OLED.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerI2CSCL_OLED.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerI2CSCL_OLED.GPIO_PinConfig.GPIO_PinAltFunMode	= AF4;

	GPIO_Config(&handlerI2CSCL_OLED);

	// Configuramos los pines para el I2C SDA para el OLED
	handlerI2CSDA_OLED.pGPIOx								= GPIOB;
	handlerI2CSDA_OLED.GPIO_PinConfig.GPIO_PinNumber		= PIN_8;
	handlerI2CSDA_OLED.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerI2CSDA_OLED.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_OPENDRAIN;
	handlerI2CSDA_OLED.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerI2CSDA_OLED.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerI2CSDA_OLED.GPIO_PinConfig.GPIO_PinAltFunMode	= AF9;

	GPIO_Config(&handlerI2CSDA_OLED);

	// Creamos la configuración para el I2C (3) del OLED

	handler_OLED.ptrI2Cx		= I2C3;
	handler_OLED.modeI2C		= I2C_MODE_FM;
	handler_OLED.slaveAddress	= 0x3C;				// Dirección del OLED.

	I2C_Config(&handler_OLED);

	Timer_Config(&handlerTimer2);

	// Configuración del timer4
	handlerTimer4.ptrTIMx								= TIM4;
	handlerTimer4.timerConfig.Timer_mode				= TIMER_MODE_UP;
	handlerTimer4.timerConfig.Timer_speed				= TIMER_INCR_SPEED_100us;
	handlerTimer4.timerConfig.Timer_period				= 10;

	Timer_Config(&handlerTimer4);
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
				delay(delayTime);								//Tiempo de espera entre paso y paso
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

	// Prendemos el motor 1
	if(auxLedState == 1){
		handlerTimer2.timerConfig.Timer_period				= 600;
		Timer_Config(&handlerTimer2);
		auxLedState = 0;
	}
	// Prendemos los motores 2
	else if(auxLedState == 2){
		handlerTimer2.timerConfig.Timer_period				= 200;
		Timer_Config(&handlerTimer2);
		auxLedState = 0;
	}
	// Prendemos el motor 3
	else if(auxLedState == 3){
		handlerTimer2.timerConfig.Timer_period				= 50;
		Timer_Config(&handlerTimer2);
		auxLedState = 0;
	}
}
//***********// CallBacks //***********//

// Timer encargado del state LED
void Timer2_Callback(void){
	handlerStateLED.pGPIOx -> ODR ^= GPIO_ODR_OD5;		// Encendido y apagado StateLED
}


//void Timer3_Callback(void){
//	handlerStep1Config.pGPIOx -> ODR ^= GPIO_ODR_OD5;		// Encendido y apagado StateLED
//}

// Timer encargado de los delay y del muestreo del MPU6050
void Timer4_Callback(void){
	counterDelay++;
	counterStateLED++;
}

void adc_Complete_Callback(void){
	adcData		  = getADC();
	adcIsComplete = true;
}

void USART2_Callback(void){
	// Lectura de los datos recibidos
	dataValue = returnData();
}

void USART6_Callback(void){
	rxData = returnData();
//	dataValueBluetoth[dataPosition] = returnData();
//	dataPosition++;
//	if (dataPosition >= numberOfData){
//		dataPosition = 0;
//		MPU6050IsReady = true;
//	}
}
