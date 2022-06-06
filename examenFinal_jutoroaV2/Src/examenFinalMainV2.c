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

// *************** // VARIABLES PROYECTO // *************** //

// Handler para todos los periféricos que se van a implementar en el desarrollo del proyecto
GPIO_Handler_t 	handlerStateLED 		= {0};	// StateLED
GPIO_Handler_t 	handlerPinTX 			= {0};	// handlerPinTX
GPIO_Handler_t 	handlerPinRX 			= {0};	// handlerPinRX
TIMER_Handler_t handlerTimer2 			= {0};	// Timer2
TIMER_Handler_t handlerTimer3 			= {0};	// Timer3
USART_Handler_t handlerCommTerminal		= {0};	// Usart para la terminal en USART 6

/* Configuración para el I2C */
GPIO_Handler_t handlerI2CSDA_MPU6050 			= {0};	// SDA para el I2C	MPU6050
GPIO_Handler_t handlerI2CSCL_MPU6050			= {0};	// SCL para el I2C	MPU6050
I2C_Handler_t  handler_MPU6050	 				= {0};	// I2C (1) para el RTC	MPU6050

GPIO_Handler_t handlerI2CSDA_RTC 				= {0};	// SDA para el I2C	RTC DS1307
GPIO_Handler_t handlerI2CSCL_RTC 				= {0};	// SCL para el I2C	RTC DS1307
I2C_Handler_t  handlerRTC_RTC 	 				= {0};	// I2C (1) para el  RTC DS1307

GPIO_Handler_t handlerI2CSDA_OLED 				= {0};	// SDA para el I2C	OLED 1.3"
GPIO_Handler_t handlerI2CSCL_OLED 				= {0};	// SCL para el I2C	OLED 1.3"
I2C_Handler_t  handlerRTC_OLED	 				= {0};	// I2C (1) para el  OLED 1.3"

/* Configuración para el RTC */
rtc_t			dateAndTimeRTC = {0};

/* Configuración para el MPU6050 */
#define numberofsensor 	6								// Cantidad de sensores
int16_t		MPUBuffer[numberofsensor]		= {0};		// Buffer para almacenar los datos
uint8_t		MPU6050IsReady					= false;	// Bandera para controlar la lectura de datos
uint8_t		accelFull						= false;
uint8_t		gyrosFull						= false;
int16_t 	dataPosition 					= 0;
char		bufferPrintMPU[256];

/* Variables auxiliar. */
char 		bufferData[256] = "¡Hola! Soy el USART del STM32 y estoy funcionando.";		// Mensaje de Bienvenida
uint8_t 	rxData				= 0;				// Variable donde se guardarán los datos obtenidos por el RX
uint8_t 	i2cBuffer 			= 0;				// Buffer para el I2C

/* Variables para los commands */
char		bufferReception[256];
char 		cmd[64];
char 		userMsg[256];

uint32_t		counterReception 		= 0;
uint8_t			stringComplete			= false;
unsigned int 	firstParameter 			= 0;
unsigned int  	secondParameter 		= 0;

// *************** // Headers // *************** //

void initSystem(void);			// Función para inicializar el sistema
void sensorConfig();			// Función para cargar la configuración del sensor
void sensorData(void);
void parseCommands(char *ptrBufferReception);
void initialMessage(void);
void defineSetAndTimeRTC(void);

// *************** // MAIN // *************** //
int main(void)
{
	// Inicializamos el sistema
	initSystem();

	// Mandamos el mensaje de Bienvenida
	initialMessage();

	// Cargamos los valores iniciales para el RTC
	//defineSetAndTimeRTC();

	//RTC_SetDateTime(&handlerRTC, &dateAndTimeRTC);

	/* Main Loop */
	while(1){


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
			parseCommands(bufferReception);
			stringComplete = false;
		}

		// Comparaciones para obtener los datos del MPU en modo continuo

		/*
		if((MPU6050IsReady == true)){
			dataPosition = 0;
			MPUBuffer[dataPosition] = MPU6050_SensorValue(&handler_MPU6050,ACCEL_X)*(9.81/16384.0);
			dataPosition++;
			MPUBuffer[dataPosition] = MPU6050_SensorValue(&handler_MPU6050,ACCEL_Y)*(9.81/16384.0);
			dataPosition++;
			MPUBuffer[dataPosition] = MPU6050_SensorValue(&handler_MPU6050,ACCEL_Z)*(9.81/16384.0);
			dataPosition++;
			dataPosition = 0;

			sprintf(bufferData, "MPU6050 - ACCEL: %d | %d | %d \n",
					(int) MPUBuffer[0],(int) MPUBuffer[1],(int) MPUBuffer[2]);
			writeMsg(&handlerCommTerminal, bufferData);

			MPU6050IsReady = false;
		}*/
/*
		if((MPU6050IsReady == true) && (gyrosFull = true)){
			dataPosition = 3;
			MPUBuffer[dataPosition] = MPU6050_SensorValue(&handler_MPU6050,GYRO_X)*(250.0/32768.0);
			dataPosition++;
			MPUBuffer[dataPosition] = MPU6050_SensorValue(&handler_MPU6050,GYRO_Y)*(250.0/32768.0);
			dataPosition++;
			MPUBuffer[dataPosition] = MPU6050_SensorValue(&handler_MPU6050,GYRO_Z)*(250.0/32768.0);
			dataPosition = 3;

			sprintf(bufferData, "MPU6050 - GYROS: %d | %d | %d \n",
				(int) MPUBuffer[3],(int) MPUBuffer[4],(int) MPUBuffer[5]);
			writeMsg(&handlerCommTerminal, bufferData);

			MPU6050IsReady = false;
		}*/
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

//	// Llamamos la función para configurar y activar el SysTick
//
//	config_SysTickMs();

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
	handler_MPU6050.ptrI2Cx		= I2C1;
	handler_MPU6050.modeI2C		= I2C_MODE_FM;
	handler_MPU6050.slaveAddress	= 0b1101001;		// Dirección del MPU6050 con Logic 1 (0x69)

	I2C_Config(&handler_MPU6050);

	// Configuramos los pines para el I2C SCL para el RTC
	handlerI2CSCL_RTC.pGPIOx								= GPIOB;
	handlerI2CSCL_RTC.GPIO_PinConfig.GPIO_PinNumber			= PIN_10;
	handlerI2CSCL_RTC.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerI2CSCL_RTC.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_OPENDRAIN;
	handlerI2CSCL_RTC.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerI2CSCL_RTC.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerI2CSCL_RTC.GPIO_PinConfig.GPIO_PinAltFunMode		= AF9;

	GPIO_Config(&handlerI2CSCL_MPU6050);

	// Configuramos los pines para el I2C SDA para el RTC
	handlerI2CSDA_RTC.pGPIOx								= GPIOB;
	handlerI2CSDA_RTC.GPIO_PinConfig.GPIO_PinNumber			= PIN_3;
	handlerI2CSDA_RTC.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_ALTFN;
	handlerI2CSDA_RTC.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_OPENDRAIN;
	handlerI2CSDA_RTC.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_PULLUP;
	handlerI2CSDA_RTC.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_FAST;
	handlerI2CSDA_RTC.GPIO_PinConfig.GPIO_PinAltFunMode		= AF9;

	GPIO_Config(&handlerI2CSDA_MPU6050);

	// Creamos la configuración para el I2C (2) del RTC

	handlerRTC_RTC.ptrI2Cx		= I2C2;
	handlerRTC_RTC.modeI2C		= I2C_MODE_FM;
	handlerRTC_RTC.slaveAddress	= 0b1101000;		// Dirección del RTC con Logic 0 (0x68)

	I2C_Config(&handlerRTC_RTC);

}

//***********// defineSetAndTimeRTC //***********//

void defineSetAndTimeRTC(void){
	dateAndTimeRTC.seconds 	= 5;
	dateAndTimeRTC.minutes 	= 3;
	dateAndTimeRTC.hour 	= 3;
	dateAndTimeRTC.weekDay 	= 3;
	dateAndTimeRTC.date		= 18;
	dateAndTimeRTC.month 	= 5;
	dateAndTimeRTC.year		= 20;
}

//***********// parseCommands //***********//

void parseCommands(char *ptrBufferReception){

	// Esta funcion de C lee la cadena de caracteres a la que apunta el "ptr" y la divide
	// y almacena en tres elementos diferentes: un string llamado "cmd", y dos números integer
	// llamados "firstParameter" y "SecondParameter".
	// De esta forma, podemos introducir inf	ormación al micro desde el puerto serial.
	sscanf(ptrBufferReception, "%s %u %u %s", cmd, &firstParameter, &secondParameter, userMsg);

	if(strcmp(cmd, "help") == 0){
		stopTimer(&handlerTimer3);
//		accelFull = false;
//		gyrosFull = false;
//		MPU6050IsReady = false;

		writeMsg(&handlerCommTerminal, "Help Menu CMDs:\n");
		writeMsg(&handlerCommTerminal, "Seleccione el comando pertinente. \n");
		writeMsg(&handlerCommTerminal, "1) help             -- Imprime el menú de ayuda. \n");
		writeMsg(&handlerCommTerminal, "2) initmpu          -- Inicializa el MPU6050. \n");
		writeMsg(&handlerCommTerminal, "3) accelfull        -- Obtiene la información de todos los acelerómetros en m/s2 \n");
		writeMsg(&handlerCommTerminal, "                    en tiempo real. \n");
		writeMsg(&handlerCommTerminal, "4) accel #n         -- Obtiene la información del accelerómetro instantáneo en el eje #n. \n");
		writeMsg(&handlerCommTerminal, "                    siendo 1 = x, 2 = y, 3 = z. \n");
		writeMsg(&handlerCommTerminal, "5) gyrofull         -- Obtiene la información de todos los giroscopios en grados/s \n");
		writeMsg(&handlerCommTerminal, "                    en tiempo real. \n");
		writeMsg(&handlerCommTerminal, "6) angle #n         -- Obtiene el angulo de rotación con respecto al eje #n. \n");
		writeMsg(&handlerCommTerminal, "7) initrtc          -- Inicializa el RTC. \n");
		writeMsg(&handlerCommTerminal, "8) setrtc           -- Setea la configuración para el RTC. \n");
		writeMsg(&handlerCommTerminal, "8) showtime         -- Muestra la hora (tiempo) instantáneamente. \n");
		writeMsg(&handlerCommTerminal, "9) showdate         -- Muestra la fecha instantáneamente. \n");
		writeMsg(&handlerCommTerminal, "10) showtimecont    -- Muestra la hora (tiempo) cada 1s. \n");
		writeMsg(&handlerCommTerminal, "11) partyrgb        -- Activa el modo Party del rgb. \n");
		writeMsg(&handlerCommTerminal, "12) partyoled       -- Activa el modo Party de la pantalla oled. \n");
	}
	else if(strcmp(cmd, "initmpu") == 0){
		//stopTimer(&handlerTimer3);

		// Función para inicializar el MPU6050
		MPU6050_writeByte(&handler_MPU6050, MPU6050_RA_PWR_MGMT_1, 0x00);
		writeMsg(&handlerCommTerminal, "\n");
		writeMsg(&handlerCommTerminal, "MPU6050 ha sido inicializado correctamente. \n");

	}
	else if(strcmp(cmd, "accelfull") == 0){
//		stopTimer(&handlerTimer3);
//		accelFull = false;
//		gyrosFull = false;
//		MPU6050IsReady = false;
//
//		// Conversión de modo continua
//		startTimer(&handlerTimer3);
//		accelFull = true;
	}
	else if(strcmp(cmd, "accel") == 0){
		//stopTimer(&handlerTimer3);

		// Mostrar en eje X la aceleración
		if(firstParameter == 1){
			int16_t AccelX = MPU6050_SensorValue(&handler_MPU6050,ACCEL_X);
			sprintf(bufferData, "AccelX = %d \n",(int) AccelX);
			writeMsg(&handlerCommTerminal, bufferData);
		}
		else if(firstParameter == 2){
			int16_t AccelY = MPU6050_SensorValue(&handler_MPU6050,ACCEL_Y);
			sprintf(bufferData, "AccelY = %d \n",(int) AccelY);
			writeMsg(&handlerCommTerminal, bufferData);
		}
		else if(firstParameter == 3){
			int16_t AccelZ = MPU6050_SensorValue(&handler_MPU6050,ACCEL_Z);
			sprintf(bufferData, "AccelZ = %d \n",(int) AccelZ);
			writeMsg(&handlerCommTerminal, bufferData);
		}
	}
	else if(strcmp(cmd, "gyrofull") == 0){
//		stopTimer(&handlerTimer3);
//		accelFull = false;
//		gyrosFull = false;
//		MPU6050IsReady = false;
//
//
//		// Conversión de modo continua
//		gyrosFull = true;
//		startTimer(&handlerTimer3);
//		gyrosFull = true;
//		accelFull = false;
	}



	/*
	else if(strcmp(cmd, "dummy") == 0){
		writeMsg(&handlerCommTerminal, "CMD: dummy\n");
		// Cambiando el formato para presentar el número por el puerto serial
		sprintf(bufferData, "number A = %u \n", firstParameter);
		writeMsg(&handlerCommTerminal, bufferData);

		// Cambiando el formato para presentar el número por el puerto serial
		sprintf(bufferData, "number B = %u \n", secondParameter);
				writeMsg(&handlerCommTerminal, bufferData);
	}
	else if (strcmp(cmd, "usermsg") == 0){
		writeMsg(&handlerCommTerminal, "CMD: usermsg\n");
		writeMsg(&handlerCommTerminal, userMsg);
		writeMsg(&handlerCommTerminal, "\n");
	}*/
	else{
		// Se imprime el mensaje "Wrong CMD" si la escritura no corresponde a los CMD implementados.
		writeMsg(&handlerCommTerminal, "Wrong CMD");
	}
}

//***********// initialMessage //***********//

void initialMessage(void){
	writeMsg(&handlerCommTerminal,"// ***** // BIENVENIDO A LA NERIONETA // ***** // \n");
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

void USART2_Callback(void){
	// Lectura de los datos recibidos
	rxData = returnData();
}
// Timer encargado del StateLED
void Timer2_Callback(void){
	handlerStateLED.pGPIOx -> ODR ^= GPIO_ODR_OD5;		// Encendido y apagado StateLED
}

// Timer encargado del muestreo a través del MPU6050
void Timer3_Callback(void){
	MPU6050IsReady = true;
}
