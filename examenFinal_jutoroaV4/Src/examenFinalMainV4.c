/*
 * I2CMPU6050.c
 *
 *  Created on: 4/05/2022
 *  Author: Juan Pablo
 *
 * // *************** // Examen Final Taller V // *************** //
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
#include "FPUDriver.h"

// *************** // VARIABLES PROYECTO // *************** //

// Handler para todos los periféricos que se van a implementar en el desarrollo del proyecto
GPIO_Handler_t 	handlerStateLED 		= {0};	// StateLED
GPIO_Handler_t 	handlerPinTX 			= {0};	// handlerPinTX
GPIO_Handler_t 	handlerPinRX 			= {0};	// handlerPinRX

GPIO_Handler_t 	handlerPinSTART 			= {0};	// handlerPinSTART
GPIO_Handler_t 	handlerPinSTOP 				= {0};	// handlerPinSTOP
GPIO_Handler_t 	handlerPinBUZZER 			= {0};	// handlerPinBUZZER

TIMER_Handler_t handlerTimer2 			= {0};	// Timer2
TIMER_Handler_t handlerTimer5 			= {0};	// Timer5
TIMER_Handler_t handlerTimer4 			= {0};	// Timer4
USART_Handler_t handlerCommTerminal		= {0};	// Usart para la terminal en USART 6

/* Configuración para el I2C */
GPIO_Handler_t handlerI2CSDA_MPU6050 			= {0};	// SDA para el I2C	MPU6050
GPIO_Handler_t handlerI2CSCL_MPU6050			= {0};	// SCL para el I2C	MPU6050
I2C_Handler_t  handler_MPU6050	 				= {0};	// I2C (1) para el RTC	MPU6050

GPIO_Handler_t handlerI2CSDA_RTC 				= {0};	// SDA para el I2C	RTC DS1307
GPIO_Handler_t handlerI2CSCL_RTC 				= {0};	// SCL para el I2C	RTC DS1307
I2C_Handler_t  handler_RTC	 					= {0};	// I2C (2) para el  RTC DS1307

GPIO_Handler_t handlerI2CSDA_OLED 				= {0};	// SDA para el I2C	OLED 1.3"
GPIO_Handler_t handlerI2CSCL_OLED 				= {0};	// SCL para el I2C	OLED 1.3"
I2C_Handler_t  handler_OLED	 					= {0};	// I2C (3) para el  OLED 1.3"

/* Variables para la configuración del RTC */
rtc_t			dateAndTimeRTC = {0};					// Estructura que permite almacenar los valores iniciales para definir la fecha y el tiempo
uint8_t 		rtcContIsReady = false;					// Contador para el RTC
uint8_t			rtcContFlag	   = false;					// Flag para activar la conversión continua del RTC

/* Variables para la configuración del MPU6050 */
#define numberofsensor 	3								// Cantidad de sensores
char		MPUBufferAccel[256];						// Buffer para almacenar los datos
char		MPUBufferGyro[256];							// Buffer para almacenar los datos
uint8_t		MPU6050IsReady					= false;	// Bandera para controlar la lectura de datos
uint8_t		accelFull						= false;	// Flag para activar la conversión continua del acelerómetro
uint8_t		gyrosFull						= false;	// Flag para activar la conversión continua del Giroscopio

/* Configuración para la OLED */
char 		bufferDataOLED[128] = {0};

/* Variables auxiliar. */
char 		bufferData[256] = "¡Hola! Soy el USART del STM32 y estoy funcionando.";		// Mensaje de Bienvenida
uint8_t 	rxData					= 0;				// Variable donde se guardarán los datos obtenidos por el RX
uint8_t 	i2cBuffer 				= 0;				// Buffer para el I2C
uint8_t		buzzerFlagTimer			= false;			// Bandera del timer encargada del buzzer
uint8_t		buzzerFlagCommand		= false;			// Bandera de los comandos, encargada del buzzer
uint16_t	counterDestruction		= 60;				// Inicialización tiempo de destrucción.

/* Variables para los commands */
char		bufferReception[256];						// Buffer de recepción
char 		cmd[64];									// Buffer para el cmd
char 		userMsg[256];								// Buffer para el mensaje del usuario

uint32_t		counterReception 		= 0;
uint8_t			stringComplete			= false;
unsigned int 	firstParameter 			= 0;
unsigned int  	secondParameter 		= 0;

// *************** // Headers // *************** //

void initSystem(void);										// Función para inicializar el sistema
void sensorConfig();										// Función para cargar la configuración del sensor
void parseCommands(char *ptrBufferReception);				// Función para generar los comandos por consola
void initialMessage(void);									// Función que contiene el mensaje de bienvenida
void defineSetAndTimeRTC(uint8_t minutesx, uint8_t hourx);	// Función para definir los valores iniciales del RTC
void stopMPU(void);											// Función para parar todos los procesos de conversión continua del MPU
void controlLED(void);										// Función para controlar el START, STOP, y BUZZER Pin

// *************** // MAIN // *************** //
int main(void)
{
	// Inicializamos la FPU
	FPUInit();
	// Inicializamos el sistema
	initSystem();

	// Mandamos el mensaje de Bienvenida
	initialMessage();

	// Cargamos la configuración para el los LED
	controlLED();

	// Cargamos la configuración Inicial en la OLED
	OLED_Init(&handler_OLED);
	OLED_Clean(&handler_OLED);
	sprintf(bufferDataOLED, "<<<<<<<<<<<<<<<<  HOLA!          SOY EL STM32   <<<<<<<<<<<<<<<<");
	OLED_FPrint(&handler_OLED, bufferDataOLED);

	/* Main Loop */
	while(1){

		// Si se recibió un comando por consola haga
		if(rxData != '\0'){

			//Guardamos las cadenas para los comandos
			bufferReception[counterReception] = rxData;
			counterReception++;

			// Levantamos una bandera para indicar que se completó el comando
			if(rxData == '@'){
				stringComplete = true;
				// Agrego esta línea para crear el string con el null al final del arreglo
				bufferReception[counterReception] = '\0';
				counterReception = 0;
			}
			rxData = '\0';
		}

		// Hacemos un análisis de la cadena de datos obtenida
		if(stringComplete){
			// Llamamos a la función para implementar los comandos
			parseCommands(bufferReception);
			stringComplete = false;
		}

		// Comparaciones para obtener los datos del MPU en modo continuo
		if((MPU6050IsReady == true) && (accelFull == true)){

			// Leemos las aceleraciones del MPU6050 en x,y, y z.
			int16_t AccelX = MPU6050_SensorValue(&handler_MPU6050,ACCEL_X);
			int16_t AccelY = MPU6050_SensorValue(&handler_MPU6050,ACCEL_Y);
			int16_t AccelZ = MPU6050_SensorValue(&handler_MPU6050,ACCEL_Z);

			double fAccelX = AccelX*(9.81/16384.0);
			double fAccelY = AccelY*(9.81/16384.0);
			double fAccelZ = AccelZ*(9.81/16384.0);

			// NOTA: Descomentar si se quieren ver también los valores por consola
/*
			// Imprimimos los valores por consola
			sprintf(MPUBufferAccel, " %f | %f | %f \n \r",fAccelX,fAccelY,fAccelZ);
			writeMsg(&handlerCommTerminal, MPUBufferAccel);
*/
			// Imprimimos los valores por la pantalla OLED
			char bufferDataOLEDx[128] = {0};
			sprintf(bufferDataOLEDx, " ACCELX: %f ", fAccelX);
			OLED_FPrintPage(&handler_OLED, bufferDataOLEDx, PAGE_1);
			sprintf(bufferDataOLEDx, " ACCELY: %f ", fAccelY);
			OLED_FPrintPage(&handler_OLED, bufferDataOLEDx, PAGE_3);
			sprintf(bufferDataOLEDx, " ACCELZ: %f ", fAccelZ);
			OLED_FPrintPage(&handler_OLED, bufferDataOLEDx, PAGE_5);

			// Bajamos la bandera que controla el TIMER
			MPU6050IsReady = false;
		}
		// Comparaciones para obtener los datos del MPU en modo continuo
		else if((MPU6050IsReady == true) && (gyrosFull == true)){

			// Leemos los valores del giroscopio del MPU6050 en x,y, y z.
			int16_t GirosX = MPU6050_SensorValue(&handler_MPU6050,GYRO_X);
			int16_t GirosY = MPU6050_SensorValue(&handler_MPU6050,GYRO_Y);
			int16_t GirosZ = MPU6050_SensorValue(&handler_MPU6050,GYRO_Z);

			double fGirosX = GirosX*(250.0/32768.0);
			double fGirosY = GirosY*(250.0/32768.0);
			double fGirosZ = GirosZ*(250.0/32768.0);

			// NOTA: Descomentar si se quieren ver también los valores por consola
/*
			// Imprimimos los valores por consola
			sprintf(MPUBufferGyro, " %f | %f | %f \n \r",fGirosX,fGirosY,fGirosZ);
			writeMsg(&handlerCommTerminal, MPUBufferGyro);
*/
			// Imprimimos los valores por la pantalla OLED
			char bufferDataOLEDx[128] = {0};
			sprintf(bufferDataOLEDx, " GYROX: %f ", fGirosX);
			OLED_FPrintPage(&handler_OLED, bufferDataOLEDx, PAGE_1);
			sprintf(bufferDataOLEDx, " GYROY: %f ", fGirosY);
			OLED_FPrintPage(&handler_OLED, bufferDataOLEDx, PAGE_3);
			sprintf(bufferDataOLEDx, " GYROZ: %f ", fGirosZ);
			OLED_FPrintPage(&handler_OLED, bufferDataOLEDx, PAGE_5);

			// Bajamos la bandera que controla el TIMER
			MPU6050IsReady = false;
		}

		// Comparaciones para obtener los datos del RTC en modo continuo
		if((rtcContIsReady == true) && (rtcContFlag == true)){

			// Leemos los valores del giroscopio del RTC.
			uint8_t sec 	= RTC_readByte(&handler_RTC,0x00);
			uint8_t min 	= RTC_readByte(&handler_RTC,0x01);
			uint8_t hora 	= RTC_readByte(&handler_RTC,0x02);
			uint8_t day 	= RTC_readByte(&handler_RTC,0x04);
			uint8_t month 	= RTC_readByte(&handler_RTC,0x05);
			uint8_t year 	= RTC_readByte(&handler_RTC,0x06);

			// Imprimimos los valores por consola
			sprintf(bufferData, "| TIME: %d:%d:%d | DATE: %d/%d/%d |\n" , (int) hora, (int) min, (int) sec,
					(int) day, (int) month, (int) year);
			writeMsg(&handlerCommTerminal, bufferData);

			// Imprimimos los valores por la pantalla OLED
			char bufferDataOLEDx[128] = {0};
			sprintf(bufferDataOLEDx, " %d:%d:%d ",(int) hora, (int) min, (int) sec);
			OLED_FPrintPage(&handler_OLED, bufferDataOLEDx, PAGE_3);
			sprintf(bufferDataOLEDx, " %d/%d/%d ",(int) day, (int) month, (int) year);
			OLED_FPrintPage(&handler_OLED, bufferDataOLEDx, PAGE_7);

			// Bajamos la bandera que controla el TIMER
			rtcContIsReady = false;
		}
		// Comparaciones para el modo de auto destrucción
		if((buzzerFlagTimer == true) && (buzzerFlagCommand == true)){

			// Switcheamos el estado del LED STOP y del BUZZER
			handlerPinBUZZER.pGPIOx -> ODR ^= GPIO_ODR_OD0;
			handlerPinSTOP.pGPIOx -> ODR ^= GPIO_ODR_OD2;

			// Imprimimos mensaje de autodestrucción
			char bufferDataOLEDx[128] = {0};
			sprintf(bufferDataOLEDx, " EN %d S", counterDestruction);
			OLED_FPrintPage(&handler_OLED, bufferDataOLEDx, PAGE_5);
			// Si la autodestrucción llega a 0, se muere el LED y el BUZZER
			if(counterDestruction == 0){
				buzzerFlagCommand = false;
				char bufferDataOLEDx[128] = {0};
				sprintf(bufferDataOLEDx, "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
				OLED_FPrint(&handler_OLED, bufferDataOLEDx);
				GPIO_WritePin(&handlerPinSTOP, RESET);
				GPIO_WritePin(&handlerPinBUZZER, SET);
			}
			buzzerFlagTimer = false;
		}
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

	// Configuración el START LED
	handlerPinSTART.pGPIOx								= GPIOA;
	handlerPinSTART.GPIO_PinConfig.GPIO_PinNumber		= PIN_0;
	handlerPinSTART.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerPinSTART.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerPinSTART.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPinSTART.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;

	//Cargamos la configuración
	GPIO_Config(&handlerPinSTART);
	GPIO_WritePin(&handlerPinSTART, RESET);

	// Configuración el STOPLED
	handlerPinSTOP.pGPIOx								= GPIOC;
	handlerPinSTOP.GPIO_PinConfig.GPIO_PinNumber		= PIN_2;
	handlerPinSTOP.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerPinSTOP.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerPinSTOP.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPinSTOP.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;

	//Cargamos la configuración
	GPIO_Config(&handlerPinSTOP);
	GPIO_WritePin(&handlerPinSTOP, RESET);

	// Configuración PinBUZZER
	handlerPinBUZZER.pGPIOx								= GPIOB;
	handlerPinBUZZER.GPIO_PinConfig.GPIO_PinNumber		= PIN_0;
	handlerPinBUZZER.GPIO_PinConfig.GPIO_PinMode		= GPIO_MODE_OUT;
	handlerPinBUZZER.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerPinBUZZER.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerPinBUZZER.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;

	//Cargamos la configuración
	GPIO_Config(&handlerPinBUZZER);
	GPIO_WritePin(&handlerPinBUZZER, RESET);

	/* Configurando los pines sobre los que funciona el USART */
	handlerPinTX.pGPIOx									= GPIOA;
	handlerPinTX.GPIO_PinConfig.GPIO_PinNumber			= PIN_11;
	handlerPinTX.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerPinTX.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerPinTX.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerPinTX.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerPinTX.GPIO_PinConfig.GPIO_PinAltFunMode		= AF8;

	GPIO_Config(&handlerPinTX);

	handlerPinRX.pGPIOx									= GPIOA;
	handlerPinRX.GPIO_PinConfig.GPIO_PinNumber			= PIN_12;
	handlerPinRX.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerPinRX.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerPinRX.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_NOTHING;
	handlerPinRX.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerPinRX.GPIO_PinConfig.GPIO_PinAltFunMode		= AF8;

	GPIO_Config(&handlerPinRX);


	handlerCommTerminal.ptrUSARTx							= USART6;
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

	// Configuración del timer5
	handlerTimer5.ptrTIMx								= TIM5;
	handlerTimer5.timerConfig.Timer_mode				= TIMER_MODE_UP;
	handlerTimer5.timerConfig.Timer_speed				= TIMER_INCR_SPEED_1ms;
	handlerTimer5.timerConfig.Timer_period				= 150;

	Timer_Config(&handlerTimer5);

	// Configuración del timer4
	handlerTimer4.ptrTIMx								= TIM4;
	handlerTimer4.timerConfig.Timer_mode				= TIMER_MODE_UP;
	handlerTimer4.timerConfig.Timer_speed				= TIMER_INCR_SPEED_1ms;
	handlerTimer4.timerConfig.Timer_period				= 1000;

	Timer_Config(&handlerTimer4);

	// Configuramos los pines para el I2C SCL para el MPU 6050
	handlerI2CSCL_MPU6050.pGPIOx								= GPIOB;
	handlerI2CSCL_MPU6050.GPIO_PinConfig.GPIO_PinNumber			= PIN_6;
	handlerI2CSCL_MPU6050.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerI2CSCL_MPU6050.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_OPENDRAIN;
	handlerI2CSCL_MPU6050.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerI2CSCL_MPU6050.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerI2CSCL_MPU6050.GPIO_PinConfig.GPIO_PinAltFunMode		= AF4;

	GPIO_Config(&handlerI2CSCL_MPU6050);

	// Configuramos los pines para el I2C SDA para el MPU 6050
	handlerI2CSDA_MPU6050.pGPIOx								= GPIOB;
	handlerI2CSDA_MPU6050.GPIO_PinConfig.GPIO_PinNumber			= PIN_7;
	handlerI2CSDA_MPU6050.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerI2CSDA_MPU6050.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_OPENDRAIN;
	handlerI2CSDA_MPU6050.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerI2CSDA_MPU6050.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerI2CSDA_MPU6050.GPIO_PinConfig.GPIO_PinAltFunMode		= AF4;

	GPIO_Config(&handlerI2CSDA_MPU6050);

	// Creamos la configuración para el I2C (1) del MPU6050
	handler_MPU6050.ptrI2Cx			= I2C1;
	handler_MPU6050.modeI2C			= I2C_MODE_FM;
	handler_MPU6050.slaveAddress	= 0b1101001;		// Dirección del MPU6050 con Logic 1 (0x69)

	I2C_Config(&handler_MPU6050);

	// Configuramos los pines para el I2C SCL para el RTC
	handlerI2CSCL_RTC.pGPIOx								= GPIOB;
	handlerI2CSCL_RTC.GPIO_PinConfig.GPIO_PinNumber			= PIN_10;
	handlerI2CSCL_RTC.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerI2CSCL_RTC.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_OPENDRAIN;
	handlerI2CSCL_RTC.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerI2CSCL_RTC.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerI2CSCL_RTC.GPIO_PinConfig.GPIO_PinAltFunMode		= AF4;

	GPIO_Config(&handlerI2CSCL_RTC);

	// Configuramos los pines para el I2C SDA para el RTC
	handlerI2CSDA_RTC.pGPIOx								= GPIOB;
	handlerI2CSDA_RTC.GPIO_PinConfig.GPIO_PinNumber			= PIN_3;
	handlerI2CSDA_RTC.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerI2CSDA_RTC.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_OPENDRAIN;
	handlerI2CSDA_RTC.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerI2CSDA_RTC.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerI2CSDA_RTC.GPIO_PinConfig.GPIO_PinAltFunMode		= AF9;

	GPIO_Config(&handlerI2CSDA_RTC);

	// Creamos la configuración para el I2C (2) del RTC

	handler_RTC.ptrI2Cx		= I2C2;
	handler_RTC.modeI2C		= I2C_MODE_FM;
	handler_RTC.slaveAddress	= 0b1101000;		// Dirección del RTC con Logic 0 (0x68)

	I2C_Config(&handler_RTC);

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
}

//***********// defineSetAndTimeRTC //***********//

/* Función que permite setear la fecha y la hora*/

void defineSetAndTimeRTC(uint8_t hourx, uint8_t minutesx){
	dateAndTimeRTC.seconds 	= 0;
	dateAndTimeRTC.minutes 	= minutesx;
	dateAndTimeRTC.hour 	= hourx;
	dateAndTimeRTC.weekDay 	= 1;
	dateAndTimeRTC.date		= 6;
	dateAndTimeRTC.month 	= 6;
	dateAndTimeRTC.year		= 22;
}

//***********// parseCommands //***********//

// Esta funcion de C lee la cadena de caracteres a la que apunta el "ptr" y la divide
// y almacena en tres elementos diferentes: un string llamado "cmd", y dos números integer
// llamados "firstParameter" y "SecondParameter".
// De esta forma, podemos introducir inf	ormación al micro desde el puerto serial.

void parseCommands(char *ptrBufferReception){

	sscanf(ptrBufferReception, "%s %u %u %s", cmd, &firstParameter, &secondParameter, userMsg);

	// Desplega el menú de comandos
	if(strcmp(cmd, "help") == 0){
		// Sección en donde se apagan todas las conversiones continuas
		stopMPU();
		rtcContFlag = false;
		controlLED();

		// Menú de comandos
		writeMsg(&handlerCommTerminal, "Help Menu CMDs:\n");
		writeMsg(&handlerCommTerminal, "Seleccione el comando pertinente. \n");
		writeMsg(&handlerCommTerminal, "1) help             -- Imprime el menú de ayuda. \n");
		writeMsg(&handlerCommTerminal, "2) initmpu          -- Inicializa el MPU6050. \n");
		writeMsg(&handlerCommTerminal, "3) accel #n         -- Obtiene la información del accelerómetro instantáneo en el eje #n. \n");
		writeMsg(&handlerCommTerminal, "                    siendo 1 = x, 2 = y, 3 = z. \n");
		writeMsg(&handlerCommTerminal, "4) gyro #n          -- Obtiene el valor del acelerómetro con respecto al eje #n. \n");
		writeMsg(&handlerCommTerminal, "5) accelfull        -- Obtiene la información de todos los acelerómetros en m/s2 \n");
		writeMsg(&handlerCommTerminal, "                    en tiempo real. \n");
		writeMsg(&handlerCommTerminal, "6) gyrofull         -- Obtiene la información de todos los giroscopios en grados/s \n");
		writeMsg(&handlerCommTerminal, "                    en tiempo real. \n");
		writeMsg(&handlerCommTerminal, "7) initrtc          -- Inicializa el RTC. \n");
		writeMsg(&handlerCommTerminal, "8) setrtc           -- Setea la configuración para el RTC. \n");
		writeMsg(&handlerCommTerminal, "8) showtime         -- Muestra la hora (tiempo) instantáneamente. \n");
		writeMsg(&handlerCommTerminal, "9) showdate         -- Muestra la fecha instantáneamente. \n");
		writeMsg(&handlerCommTerminal, "10) showtimecont    -- Muestra la hora (tiempo) cada 1s. \n");
		writeMsg(&handlerCommTerminal, "11) selfdestruction -- Activa la alarma de autodestrucción del STM32. \n");

		// Imprimimos un mensaje para el modo HELP para la pantalla OLED
		OLED_Clean(&handler_OLED);
		char bufferDataOLEDx[128] = {0};
		sprintf(bufferDataOLEDx, " BIENVENIDO AL   MODO DE AYUDA   DEL STM32");
		OLED_FPrint(&handler_OLED, bufferDataOLEDx);
	}
	else if(strcmp(cmd, "initmpu") == 0){
		// Sección en donde se apagan todas las conversiones continuas
		stopMPU();
		rtcContFlag = false;
		controlLED();

		// Función para inicializar el MPU6050
		MPU6050_writeByte(&handler_MPU6050, MPU6050_RA_PWR_MGMT_1, 0x00);

		// Escribimos un mensaje de confirmación por consola
		writeMsg(&handlerCommTerminal, "\n");
		writeMsg(&handlerCommTerminal, "MPU6050 ha sido inicializado correctamente. \n");
		i2cBuffer = MPU6050_readByte(&handler_MPU6050, MPU6050_RA_PWR_MGMT_1);
		sprintf(bufferData, "Registro PWR MGMT = 0x%2x \n", (unsigned int) i2cBuffer);
		writeMsg(&handlerCommTerminal, bufferData);

		// Escribimos un mensaje de confirmación a través de la OLED
		OLED_Clean(&handler_OLED);
		char bufferDataOLEDx[128] = {0};
		sprintf(bufferDataOLEDx, "EL MPU6050 HA   SIDO INICIADO   CORRECTAMENTE");
		OLED_FPrint(&handler_OLED, bufferDataOLEDx);

	}
	else if(strcmp(cmd, "accel") == 0){
		// Sección en donde se apagan todas las conversiones continuas
		stopMPU();
		rtcContFlag = false;
		controlLED();

		// Mostrar en eje X la aceleración
		if(firstParameter == 1){
			// Mostrar por consola
			int16_t AccelX = MPU6050_SensorValue(&handler_MPU6050,ACCEL_X);
			sprintf(bufferData, "AccelX = %d \n",(int) AccelX);
			writeMsg(&handlerCommTerminal, bufferData);

			// Mostrar por OLED
			OLED_Clean(&handler_OLED);
			char bufferDataOLEDx[128] = {0};
			sprintf(bufferDataOLEDx, "  MODE SINGLE   ACCELX = %d \n",(int) AccelX);
			OLED_FPrint(&handler_OLED, bufferDataOLEDx);
		}
		// Mostrar en eje Y la aceleración
		else if(firstParameter == 2){
			// Mostrar por consolar
			int16_t AccelY = MPU6050_SensorValue(&handler_MPU6050,ACCEL_Y);
			sprintf(bufferData, "AccelY = %d \n",(int) AccelY);
			writeMsg(&handlerCommTerminal, bufferData);

			// Mostrar por OLED
			OLED_Clean(&handler_OLED);
			char bufferDataOLEDx[128] = {0};
			sprintf(bufferDataOLEDx, "  MODE SINGLE   ACCELY = %d \n",(int) AccelY);
			OLED_FPrint(&handler_OLED, bufferDataOLEDx);
		}
		// Mostrar en eje Z la aceleración
		else if(firstParameter == 3){
			// Mostrar por consolar
			int16_t AccelZ = MPU6050_SensorValue(&handler_MPU6050,ACCEL_Z);
			sprintf(bufferData, "AccelZ = %d \n",(int) AccelZ);
			writeMsg(&handlerCommTerminal, bufferData);

			// Mostrar por OLED
			OLED_Clean(&handler_OLED);
			char bufferDataOLEDx[128] = {0};
			sprintf(bufferDataOLEDx, "  MODE SINGLE   ACCELZ = %d \n",(int) AccelZ);
			OLED_FPrint(&handler_OLED, bufferDataOLEDx);
		}
		// Imprimir error si el usuario pone un valor no disponible
		else{
			writeMsg(&handlerCommTerminal, "Acelerómetro no disponible \n");

			OLED_Clean(&handler_OLED);
			char bufferDataOLEDx[128] = {0};
			sprintf(bufferDataOLEDx, "                !!!! ERROR !!!!");
			OLED_FPrint(&handler_OLED, bufferDataOLEDx);
		}
	}
	else if(strcmp(cmd, "gyro") == 0){
		// Sección en donde se apagan todas las conversiones continuas
		stopMPU();
		rtcContFlag = false;
		controlLED();

		// Mostrar en eje X la velocidad angular
		if(firstParameter == 1){
			// Se imprime por consola
			int16_t GirosX = MPU6050_SensorValue(&handler_MPU6050,GYRO_X);
			sprintf(bufferData, "GirosX = %d \n",(int) GirosX);
			writeMsg(&handlerCommTerminal, bufferData);

			// Se imprime por la pantalla OLED
			OLED_Clean(&handler_OLED);
			char bufferDataOLEDx[128] = {0};
			sprintf(bufferDataOLEDx, "  MODE SINGLE   GYROX = %d \n",(int) GirosX);
			OLED_FPrint(&handler_OLED, bufferDataOLEDx);
		}
		// Mostrar en eje X la velocidad angular
		else if(firstParameter == 2){
			// Se imprime por consola
			int16_t GirosY = MPU6050_SensorValue(&handler_MPU6050,GYRO_Y);
			sprintf(bufferData, "GirosY = %d \n",(int) GirosY);
			writeMsg(&handlerCommTerminal, bufferData);

			// Se imprime por OLED
			OLED_Clean(&handler_OLED);
			char bufferDataOLEDx[128] = {0};
			sprintf(bufferDataOLEDx, "  MODE SINGLE   GYROY = %d \n",(int) GirosY);
			OLED_FPrint(&handler_OLED, bufferDataOLEDx);
		}
		// Mostrar en eje Z la velocidad angular
		else if(firstParameter == 3){
			// Se imprime por consola
			int16_t GirosZ = MPU6050_SensorValue(&handler_MPU6050,GYRO_Z);
			sprintf(bufferData, "GirosZ = %d \n",(int) GirosZ);
			writeMsg(&handlerCommTerminal, bufferData);

			// Se imprime por OLED
			OLED_Clean(&handler_OLED);
			char bufferDataOLEDx[128] = {0};
			sprintf(bufferDataOLEDx, "  MODE SINGLE   GYROZ = %d \n",(int) GirosZ);
			OLED_FPrint(&handler_OLED, bufferDataOLEDx);
		}
		// Imprimir error si el usuario pone un valor no disponible
		else{
			writeMsg(&handlerCommTerminal, "Giroscopio no disponible \n");
			OLED_Clean(&handler_OLED);
			char bufferDataOLEDx[128] = {0};
			sprintf(bufferDataOLEDx, "                !!!! ERROR !!!!");
			OLED_FPrint(&handler_OLED, bufferDataOLEDx);
		}
	}
	// Actualizar constantemente la aceleración
	else if(strcmp(cmd, "accelfull") == 0){
		// Sección en donde se apagan todas las conversiones continuas
		stopMPU();
		rtcContFlag = false;
		controlLED();

		// Se imprime por consola
		writeMsg(&handlerCommTerminal, "Aceleración Modo Continuo \n");
		writeMsg(&handlerCommTerminal, "X | Y | Z \n");

		// Conversión de modo continua
		accelFull = true;
		OLED_Clean(&handler_OLED);
	}
	// Actualizar constantemente la velocidad angular
	else if(strcmp(cmd, "gyrofull") == 0){
		// Sección en donde se apagan todas las conversiones continuas
		stopMPU();
		rtcContFlag = false;
		controlLED();
		// Se imprime por consola
		writeMsg(&handlerCommTerminal, "Giroscopio Modo Continuo \n");
		writeMsg(&handlerCommTerminal, "X | Y | Z \n");

		// Conversión de modo continua
		gyrosFull = true;
		OLED_Clean(&handler_OLED);
	}
	else if(strcmp(cmd, "initrtc") == 0){
		// Sección en donde se apagan todas las conversiones continuas
		stopMPU();
		rtcContFlag = false;
		controlLED();

		// Función para inicializar el RTC
		RTC_init(&handler_RTC);
		writeMsg(&handlerCommTerminal, "\n");
		writeMsg(&handlerCommTerminal, "Su RTC DS1307 ha sido inicializado correctamente. \n");

		// Se imprime una confirmación por OLED
		OLED_Clean(&handler_OLED);
		char bufferDataOLEDx[128] = {0};
		sprintf(bufferDataOLEDx, " EL RTC HA       SIDO INICIADO   CORRECTAMENTE");
		OLED_FPrint(&handler_OLED, bufferDataOLEDx);
	}
	else if(strcmp(cmd, "setrtc") == 0){
		// Sección en donde se apagan todas las conversiones continuas
		stopMPU();
		rtcContFlag = false;
		controlLED();

		// Función para setear los valores iniciales del RTC
		defineSetAndTimeRTC(23, 42);
		RTC_SetDateTime(&handler_RTC, &dateAndTimeRTC);

		// Se imprime por consola
		writeMsg(&handlerCommTerminal, "\n");
		sprintf(bufferData, "Se ha configurado la hora a %d:%d:%d \n",(int) dateAndTimeRTC.hour,
				(int) dateAndTimeRTC.minutes, (int) dateAndTimeRTC.seconds);
		writeMsg(&handlerCommTerminal, bufferData);

		// Se imprime por OLED
		OLED_Clean(&handler_OLED);
		char bufferDataOLEDx[128] = {0};
		sprintf(bufferDataOLEDx, " HORA INICIAL:    %d:%d:%d \n",(int) dateAndTimeRTC.hour,
				(int) dateAndTimeRTC.minutes, (int) dateAndTimeRTC.seconds);
		OLED_FPrint(&handler_OLED, bufferDataOLEDx);
		}
	else if(strcmp(cmd, "showtime") == 0){
		// Sección en donde se apagan todas las conversiones continuas
		stopMPU();
		rtcContFlag = false;
		controlLED();

		// Función para leer, horas, minutos y segundos.
		uint8_t sec = RTC_readByte(&handler_RTC,0x00);
		uint8_t min = RTC_readByte(&handler_RTC,0x01);
		uint8_t hora = RTC_readByte(&handler_RTC,0x02);

		// Se imprime por consola
		writeMsg(&handlerCommTerminal, "\n");
		sprintf(bufferData, "La hora actual es %d:%d:%d \n", (int) hora, (int) min, (int) sec);
		writeMsg(&handlerCommTerminal, bufferData);

		// Se imprime por OLED
		OLED_Clean(&handler_OLED);
		char bufferDataOLEDx[128] = {0};
		sprintf(bufferDataOLEDx, " HORA ACTUAL:    %d:%d:%d \n",(int) hora, (int) min, (int) sec);
		OLED_FPrint(&handler_OLED, bufferDataOLEDx);
	}
	else if(strcmp(cmd, "showdate") == 0){
		// Sección en donde se apagan todas las conversiones continuas
		stopMPU();
		rtcContFlag = false;
		controlLED();

		// Función para leer la fecha en formato DD/MM/YY
		uint8_t day = RTC_readByte(&handler_RTC,0x04);
		uint8_t month = RTC_readByte(&handler_RTC,0x05);
		uint8_t year = RTC_readByte(&handler_RTC,0x06);

		// Se imprime por consola
		writeMsg(&handlerCommTerminal, "\n");
		sprintf(bufferData, "La fecha actual es %d/%d/%d \n", (int) day, (int) month, (int) year);
		writeMsg(&handlerCommTerminal, bufferData);

		// Se imprime por OLED
		OLED_Clean(&handler_OLED);
		char bufferDataOLEDx[128] = {0};
		sprintf(bufferDataOLEDx, " FECHA ACTUAL:   %d:%d:%d \n",(int) day, (int) month, (int) year);
		OLED_FPrint(&handler_OLED, bufferDataOLEDx);
	}
	else if(strcmp(cmd, "showtimecont") == 0){
		// Sección en donde se apagan todas las conversiones continuas
		stopMPU();
		controlLED();

		// Función para leer la hora en tiempo real
		rtcContFlag = true;
		writeMsg(&handlerCommTerminal, "// ***** // RTC MODO CONTINUO // ***** // \n");
		OLED_Clean(&handler_OLED);

		// Se imprime por OLED
		char bufferDataOLEDx[128] = {0};
		sprintf(bufferDataOLEDx, " HORA:");
		OLED_FPrintPage(&handler_OLED, bufferDataOLEDx, PAGE_1);
		sprintf(bufferDataOLEDx, " FECHA:");
		OLED_FPrintPage(&handler_OLED, bufferDataOLEDx, PAGE_5);
	}
	else if(strcmp(cmd, "selfdestruction") == 0){
		// Sección en donde se apagan todas las conversiones continuas
		stopMPU();
		rtcContFlag = false;
		controlLED();

		// Cargamos la configuración para los pines
		GPIO_WritePin(&handlerPinSTART, RESET);
		GPIO_WritePin(&handlerPinSTOP, SET);
		writeMsg(&handlerCommTerminal, "// ***** // MODO DE AUTODESTRUCCION // ***** // \n");
		OLED_Clean(&handler_OLED);

		// Se imprime por OLED
		char bufferDataOLEDx[128] = {0};
		sprintf(bufferDataOLEDx, " INICIANDO ");
		OLED_FPrintPage(&handler_OLED, bufferDataOLEDx, PAGE_1);
		sprintf(bufferDataOLEDx, " AUTODESTRUCCION");
		OLED_FPrintPage(&handler_OLED, bufferDataOLEDx, PAGE_3);

		// Se define el tiempo de autodestrucción
		counterDestruction = 10;
		buzzerFlagCommand = true;
		}
	// Comandos no válidos
	else{
		// Sección en donde se apagan todas las conversiones continuas
		stopMPU();
		rtcContFlag = false;
		controlLED();

		// Se imprime el mensaje "Wrong CMD" si la escritura no corresponde a los CMD implementados.
		writeMsg(&handlerCommTerminal, "Wrong CMD");
		char bufferDataOLEDx[128] = {0};
		sprintf(bufferDataOLEDx, "                !!!! ERROR !!!!");
		OLED_FPrint(&handler_OLED, bufferDataOLEDx);

	}
}

//***********// stopMPU //***********//

// Función encargada de apagar todas las conversiones continuas del MPU
void stopMPU(void){
	accelFull = false;
	gyrosFull = false;
	MPU6050IsReady = false;
}

//***********// controlLED //***********//

// Función encargada de controlar el STOP, START y BUZZER
void controlLED(void){
	GPIO_WritePin(&handlerPinSTOP, RESET);
	GPIO_WritePin(&handlerPinSTART, RESET);
	GPIO_WritePin(&handlerPinSTART, SET);
	GPIO_WritePin(&handlerPinBUZZER, RESET);
	buzzerFlagCommand = false;
}


//***********// initialMessage //***********//

// Función con el mensaje inicial
void initialMessage(void){
	writeMsg(&handlerCommTerminal,"// ***** // BIENVENIDO AL STM32 // ***** // \n");
	writeMsg(&handlerCommTerminal, "\n");
	writeMsg(&handlerCommTerminal,"─▌█──║─║╔═║─║─╔═╗─ \n");
	writeMsg(&handlerCommTerminal,"─███─╠═╣╠═║─║─║─║─ \n");
	writeMsg(&handlerCommTerminal,"─▐█▐─║─║╚═╚═╚═╚═╝─ \n");
	writeMsg(&handlerCommTerminal,"─▐▐─────────────── \n");
	writeMsg(&handlerCommTerminal,"─▐▐─────────────── \n");
	writeMsg(&handlerCommTerminal, "\n");
	writeMsg(&handlerCommTerminal, bufferData);
	writeMsg(&handlerCommTerminal, "\n");
	writeMsg(&handlerCommTerminal, "Escriba 'help @', para conocer el menú de ayuda. \n");
}
//***********// CallBacks //***********//

void USART6_Callback(void){
	// Lectura de los datos recibidos
	rxData = returnData();
}

// Timer encargado del StateLED
void Timer2_Callback(void){
	handlerStateLED.pGPIOx -> ODR ^= GPIO_ODR_OD5;		// Encendido y apagado StateLED
	buzzerFlagTimer = true;
}

// // Timer encargado del muestreo a través del RTC
void Timer4_Callback(void){
	rtcContIsReady = true;
	counterDestruction--;
}

// Timer encargado del muestreo a través del MPU6050
void Timer5_Callback(void){
	MPU6050IsReady = true;
}
