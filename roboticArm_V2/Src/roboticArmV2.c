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
TIMER_Handler_t handlerTimer2 		= {0};	// Timer2
TIMER_Handler_t handlerTimer3 		= {0};	// Timer3
TIMER_Handler_t handlerTimer4 		= {0};	// Timer4

GPIO_Handler_t 	handlerUSBRX 		= {0};	// Pin USB PA2 RX
GPIO_Handler_t 	handlerUSBTX 		= {0};	// Pin USB PA3 RX
USART_Handler_t handlerCommTerminal = {0};	// Usart2

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
uint32_t 	delayTime 		= 50;						//Tiempo de delay entre paso y paso del motor en ms
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
GPIO_Handler_t handlerI2CSDA_MPU6050 			= {0};	// SDA para el I2C	MPU6050
GPIO_Handler_t handlerI2CSCL_MPU6050			= {0};	// SCL para el I2C	MPU6050
I2C_Handler_t  handler_MPU6050	 				= {0};	// I2C (1) para el RTC	MPU6050

/* Variables para la configuración del MPU6050 */
#define numberofsensor 	3								// Cantidad de sensores
char		MPUBufferAccel[256];						// Buffer para almacenar los datos
char		MPUBufferGyro[256];							// Buffer para almacenar los datos
uint8_t		MPU6050IsReady					= false;	// Bandera para controlar la lectura de datos
uint16_t	counterMPU6050					= 0;		// COUNTER MPU6050

/* Variables para la conversión ADC */

uint8_t		adcIsComplete   = false;			// Variable que nos permite saber el estado de si se completó la conversión ADC
uint16_t 	adcData			= 0;				// Valor obtenido por el ADC

/* Variables auxiliares */
uint32_t counterMillis				= 0;				// Contador para medir el tiempo
uint32_t timePrev					= 0;
uint32_t dt							= 0;
double girosc_ang_x, girosc_ang_y;
double girosc_ang_x_prev = 0, girosc_ang_y_prev = 0;
uint8_t 	i2cBuffer 				= 0;				// Buffer para el I2C
uint16_t counterStateLED			= 0;
int  counterDelay 					= 0;
char bufferData[256] 				= "¡Hola! Soy el USART del STM32 y estoy funcionando.";		// Mensaje de Bienvenida

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

	// Función para inicializar el MPU6050
	MPU6050_writeByte(&handler_MPU6050, MPU6050_RA_PWR_MGMT_1, 0x00);
	// Escribimos un mensaje de confirmación por consola
	i2cBuffer = MPU6050_readByte(&handler_MPU6050, MPU6050_RA_PWR_MGMT_1);
	sprintf(bufferData, "Registro PWR MGMT = 0x%2x \n", (unsigned int) i2cBuffer);
	writeMsg(&handlerCommTerminal, bufferData);
	writeMsg(&handlerCommTerminal, "MPU6050 ha sido inicializado correctamente. \n");

	timePrev = counterMillis;					// Actualizamos el to

    /* Ciclo principal */
	while(1){

		/*
		if(counterStateLED >= 250){
			handlerStateLED.pGPIOx -> ODR ^= GPIO_ODR_OD5;		// Encendido y apagado StateLED
			counterStateLED = 0;
		}*/
		if(counterMPU6050 >= 100){
			// Activamos la lectura del MPU y del ADC cada 150 ms
			MPU6050IsReady = true;
			counterMPU6050 = 0;
			startSingleADC();									// Lanzamos la conversión ADC
		}

		if(MPU6050IsReady == true){
			// Leemos las aceleraciones del MPU6050 en x,y, y z.
//			int16_t AccelX = MPU6050_SensorValue(&handler_MPU6050,ACCEL_X);
//			int16_t AccelY = MPU6050_SensorValue(&handler_MPU6050,ACCEL_Y);
//			int16_t AccelZ = MPU6050_SensorValue(&handler_MPU6050,ACCEL_Z);

//			double fAccelX = ((AccelX*85)/17000)+145;
//			double fAccelY = ((AccelY*85)/17000)+145;
//			double fAccelZ = AccelZ*(9.81/16384.0);



			// Leemos los valores del giroscopio del MPU6050 en x,y, y z.
			int16_t GirosX = MPU6050_SensorValue(&handler_MPU6050,GYRO_X);
			int16_t GirosY = MPU6050_SensorValue(&handler_MPU6050,GYRO_Y);
//			int16_t GirosZ = MPU6050_SensorValue(&handler_MPU6050,GYRO_Z);

			double fGirosX = GirosX*(250.0/32768.0);
			double fGirosY = GirosY*(250.0/32768.0);
//			double fGirosZ = GirosZ*(250.0/32768.0);

			dt = counterMillis - timePrev;			// Calculamos el dt
			timePrev = counterMillis;				// Actualizamos el to

			// Cálculamos los ángulos con el factor de escala
			girosc_ang_x = ((fGirosX)*dt)/1000.0 + girosc_ang_x_prev;      // Cálculamos los ángulos con el factor de escala
			girosc_ang_y = ((fGirosY)*dt)/1000.0 + girosc_ang_y_prev;

			// Actualizamos para el siguiente ángulo

//			if((girosc_ang_x_prev - 2 <= girosc_ang_x) && (girosc_ang_x_prev + 2 >= girosc_ang_x)){
//				girosc_ang_x = girosc_ang_x_prev;      // Cálculamos los ángulos con el factor de escala
//				girosc_ang_y = girosc_ang_y_prev;
//			}
//			else{
//				girosc_ang_x = ((fGirosX)*dt)/1000.0 + girosc_ang_x_prev;      // Cálculamos los ángulos con el factor de escala
//				girosc_ang_y = ((fGirosY)*dt)/1000.0 + girosc_ang_y_prev;
//			}

			girosc_ang_x_prev = girosc_ang_x;
			girosc_ang_y_prev = girosc_ang_y;



			char bufferDataOLEDx[128] = {0};
			sprintf(bufferDataOLEDx, "%f \n", girosc_ang_x);
			writeMsg(&handlerCommTerminal, bufferDataOLEDx);


//			setDutty(&handlerPWMServo1, girosc_ang_x);
//			setDutty(&handlerPWMServo2, girosc_ang_y);

			// Bajamos la bandera del MPU6050
			MPU6050IsReady	= false;

		}



		if(adcIsComplete == true){
			// Calculamos la escala para convertir la conversión ADC en movimiento del servo
			double fAdcData = ((adcData*100)/3500)+80;
			setDutty(&handlerPWMServo3, fAdcData);

			// Bajamos la bandera del ADC
			adcIsComplete = false;

//			char bufferDataOLEDx[128] = {0};
//			sprintf(bufferDataOLEDx, "%f ", fAdcData);
//			writeMsg(&handlerCommTerminal, bufferDataOLEDx);
		}

		if(dataValue != '\0'){
			writeChar(&handlerCommTerminal,dataValue);
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
			ledControl();
			motorConfig();
			dataValue = '\0';
		}
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

	// Definimos el handler para la configuración del STEP del motor1

	handlerStep1Config.pGPIOx 									= GPIOA;					// Puerto a emplear: GPIOC
	handlerStep1Config.GPIO_PinConfig.GPIO_PinNumber			= PIN_6;					// Pin a emplear: PC9
	handlerStep1Config.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;			// Señal de salida
	handlerStep1Config.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerStep1Config.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_PULLUP;
	handlerStep1Config.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_HIGH;
	handlerStep1Config.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;						// Ninguna función

	// Cargamos la configuración del PIN_B9 (Step1)
	GPIO_Config(&handlerStep1Config);

	// Definimos el handler para la dirección (DIR) del motor1

	handlerDir1Config.pGPIOx 									= GPIOA;					// Puerto a emplear: GPIOC
	handlerDir1Config.GPIO_PinConfig.GPIO_PinNumber				= PIN_11;					// Pin a emplear: PC8
	handlerDir1Config.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;			// Señal de salida
	handlerDir1Config.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerDir1Config.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_PULLUP;
	handlerDir1Config.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_HIGH;
	handlerDir1Config.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;						// Ninguna función

	// Cargamos la configuración del PIN_B8  (DIR1)
	GPIO_Config(&handlerDir1Config);

	// Definimos el handler para la configuración del STEP del motor2

	handlerStep2Config.pGPIOx 									= GPIOA;					// Puerto a emplear: GPIOB
	handlerStep2Config.GPIO_PinConfig.GPIO_PinNumber			= PIN_7;					// Pin a emplear: PB9
	handlerStep2Config.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;			// Señal de salida
	handlerStep2Config.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerStep2Config.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_PULLUP;
	handlerStep2Config.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_HIGH;
	handlerStep2Config.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;						// Ninguna función

	// Cargamos la configuración del PIN_A6 (Step2)
	GPIO_Config(&handlerStep2Config);

	// Definimos el handler para la dirección del motor2

	handlerDir2Config.pGPIOx 									= GPIOB;
	handlerDir2Config.GPIO_PinConfig.GPIO_PinNumber				= PIN_12;
	handlerDir2Config.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerDir2Config.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerDir2Config.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_PULLUP;
	handlerDir2Config.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_HIGH;
	handlerDir2Config.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;

	// Cargamos la configuración del PIN_A5  (DIR2)
	GPIO_Config(&handlerDir2Config);

	// Definimos el handler para la configuración del STEP del motor3

	handlerStep3Config.pGPIOx 									= GPIOA;
	handlerStep3Config.GPIO_PinConfig.GPIO_PinNumber			= PIN_9;
	handlerStep3Config.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerStep3Config.GPIO_PinConfig.GPIO_PinOPType			= GPIO_OTYPE_PUSHPULL;
	handlerStep3Config.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_PULLUP;
	handlerStep3Config.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_HIGH;
	handlerStep3Config.GPIO_PinConfig.GPIO_PinAltFunMode		= AF0;

	GPIO_Config(&handlerStep3Config);

	// Definimos el handler para la dirección del motor3

	handlerDir3Config.pGPIOx 									= GPIOB;
	handlerDir3Config.GPIO_PinConfig.GPIO_PinNumber				= PIN_2;
	handlerDir3Config.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_OUT;
	handlerDir3Config.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerDir3Config.GPIO_PinConfig.GPIO_PinPuPdControl		= GPIO_PUPDR_PULLUP;
	handlerDir3Config.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_HIGH;
	handlerDir3Config.GPIO_PinConfig.GPIO_PinAltFunMode			= AF0;

	GPIO_Config(&handlerDir3Config);

	// Definimos el handler para el servomotor 1 - Timer5 Channel 1

	handlerPinServo1.pGPIOx 									= GPIOA;
	handlerPinServo1.GPIO_PinConfig.GPIO_PinNumber				= PIN_0;
	handlerPinServo1.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinServo1.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerPinServo1.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerPinServo1.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_HIGH;
	handlerPinServo1.GPIO_PinConfig.GPIO_PinAltFunMode			= AF2;

	GPIO_Config(&handlerPinServo1);

	// Definimos el handler para el servomotor 2 - Timer5 Channel 2

	handlerPinServo2.pGPIOx 									= GPIOA;
	handlerPinServo2.GPIO_PinConfig.GPIO_PinNumber				= PIN_1;
	handlerPinServo2.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinServo2.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerPinServo2.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerPinServo2.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_HIGH;
	handlerPinServo2.GPIO_PinConfig.GPIO_PinAltFunMode			= AF2;

	GPIO_Config(&handlerPinServo2);

	// Definimos el handler para el servomotor 3 - Timer5 Channel 3

	handlerPinServo3.pGPIOx 									= GPIOA;
	handlerPinServo3.GPIO_PinConfig.GPIO_PinNumber				= PIN_3;
	handlerPinServo3.GPIO_PinConfig.GPIO_PinMode				= GPIO_MODE_ALTFN;
	handlerPinServo3.GPIO_PinConfig.GPIO_PinOPType				= GPIO_OTYPE_PUSHPULL;
	handlerPinServo3.GPIO_PinConfig.GPIO_PinPuPdControl			= GPIO_PUPDR_NOTHING;
	handlerPinServo3.GPIO_PinConfig.GPIO_PinSpeed				= GPIO_OSPEED_HIGH;
	handlerPinServo3.GPIO_PinConfig.GPIO_PinAltFunMode			= AF2;

	GPIO_Config(&handlerPinServo3);

	// Definimos el handler para el servomotor 1 - Timer5 Channel 1
	handlerPWMServo1.ptrTIMx								= TIM5;
	handlerPWMServo1.timerConfig.pwmMode					= TIMER_MODE_UP;
	handlerPWMServo1.timerConfig.pwmSpeed					= TIMER_INCR_SPEED_10us;
	handlerPWMServo1.timerConfig.pwmPeriod					= 2000;
	handlerPWMServo1.timerConfig.pwmDuty					= initialDuty;
	handlerPWMServo1.timerConfig.channel					= PWM_CHANNEL_1;

	PWM_Config(&handlerPWMServo1);

	// Definimos el handler para el servomotor 2 - Timer5 Channel 2
	handlerPWMServo2.ptrTIMx								= TIM5;
	handlerPWMServo2.timerConfig.pwmMode					= TIMER_MODE_UP;
	handlerPWMServo2.timerConfig.pwmSpeed					= TIMER_INCR_SPEED_10us;
	handlerPWMServo2.timerConfig.pwmPeriod					= 2000;
	handlerPWMServo2.timerConfig.pwmDuty					= initialDuty;
	handlerPWMServo2.timerConfig.channel					= PWM_CHANNEL_2;

	PWM_Config(&handlerPWMServo2);

	// Definimos el handler para el servomotor 3 - Timer5 Channel 3
	handlerPWMServo3.ptrTIMx								= TIM5;
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

	// Configuración del USART del USB para RXTX
	handlerCommTerminal.ptrUSARTx							= USART2;
	handlerCommTerminal.USART_Config.USART_mode				= USART_MODE_RXTX;
	handlerCommTerminal.USART_Config.USART_baudrate			= USART_BAUDRATE_19200;
	handlerCommTerminal.USART_Config.USART_datasize			= USART_DATASIZE_8BIT;
	handlerCommTerminal.USART_Config.USART_parity			= USART_PARITY_ODD;
	handlerCommTerminal.USART_Config.USART_stopbits			= USART_STOPBIT_1;
	handlerCommTerminal.USART_Config.USART_interrupt 		= USART_INTERRUPT_RX_ENABLE;

	USART_Config(&handlerCommTerminal);

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

	// Configuración del timer2
	handlerTimer2.ptrTIMx								= TIM2;
	handlerTimer2.timerConfig.Timer_mode				= TIMER_MODE_UP;
	handlerTimer2.timerConfig.Timer_speed				= TIMER_INCR_SPEED_1ms;
	handlerTimer2.timerConfig.Timer_period				= 10;

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
		handlerTimer2.timerConfig.Timer_period				= 400;
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

/*
void Timer3_Callback(void){
	handlerStateLED.pGPIOx -> ODR ^= GPIO_ODR_OD5;		// Encendido y apagado StateLED
}*/

// Timer encargado de los delay y del muestreo del MPU6050
void Timer4_Callback(void){
	counterDelay++;
	counterStateLED++;
	counterMPU6050++;
	counterMillis++;
}

void adc_Complete_Callback(void){
	adcData		  = getADC();
	adcIsComplete = true;
}

void USART2_Callback(void){
	// Lectura de los datos recibidos
	dataValue = returnData();
}
