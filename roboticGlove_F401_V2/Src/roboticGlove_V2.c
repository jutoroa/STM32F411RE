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

GPIO_Handler_t handlerStateLED 			= {0};	// StateLED
TIMER_Handler_t handlerTimer2 			= {0};	// Timer2
TIMER_Handler_t handlerTimer3 			= {0};	// Timer3
TIMER_Handler_t handlerTimer4 			= {0};	// Timer4

GPIO_Handler_t 	handlerUSBRX 			= {0};	// Pin USB PA2 RX
GPIO_Handler_t 	handlerUSBTX 			= {0};	// Pin USB PA3 RX
USART_Handler_t handlerCommTerminal 	= {0};	// Usart2

GPIO_Handler_t handlerUsartBluetoothRX 	= {0};	// Pin Bluetooth RX
GPIO_Handler_t handlerUsartBluetoothTX 	= {0};	// Pin Bluetooth TX
USART_Handler_t handlerUsartBluetooth 	= {0};	// Usart6 para la conexión Bluetooth

ADC_Config_t 	handlerADC				= {0};	// ADC

/* Configuración para el I2C */
GPIO_Handler_t handlerI2CSDA_MPU6050 	= {0};	// SDA para el I2C	MPU6050
GPIO_Handler_t handlerI2CSCL_MPU6050	= {0};	// SCL para el I2C	MPU6050
I2C_Handler_t  handler_MPU6050	 		= {0};	// I2C (1) para el RTC	MPU6050

/* Variables para la configuración del MPU6050 */
#define numberofsensor 	3								// Cantidad de sensores
uint8_t		MPU6050IsReady					= false;	// Bandera para controlar la lectura de datos
uint16_t	counterMPU6050					= 0;		// COUNTER MPU6050

uint32_t counterMillis						= 0;				// Contador para medir el tiempo
uint32_t timePrev							= 0;				// Variable que almacenará el tiempo inmediatamente anterior
uint32_t dt									= 0;				// Variable para medir el diferencial de tiempo
double girosc_ang_x, girosc_ang_y;						// Variables para calcular los angulos a través del giroscopio
double girosc_ang_x_prev = 0, girosc_ang_y_prev = 0;

double ang_x,ang_y;										// Variables para calcular los angulos a través del filtro complementario
double ang_x_prev, ang_y_prev;
double duty_x, duty_y;

/* Variables para la conversión ADC */

uint8_t		adcIsComplete   = false;			// Variable que nos permite saber el estado de si se completó la conversión ADC
uint16_t 	adcData			= 0;				// Valor obtenido por el ADC
double 		fAdcData 		= 0;

uint16_t adcDataFlexSensor	= 0;
uint16_t adcDataPotX		= 0;
uint16_t adcDataPotY		= 0;

uint8_t dataPotX			= 0;
uint8_t dataPotY			= 0;

#define ADC_SIGNAL_SIZE 	3

uint16_t	adcSignal[ADC_SIGNAL_SIZE]	= {0};	// Almacenamiento para los datos del ADC
uint16_t	dataPosition		= 0;

/* Variables auxiliares */

uint32_t counterFilter = 0;

/* Configuración para la OLED */
char 		bufferDataOLED[128] 	= {0};
uint8_t 	i2cBuffer 				= 0;				// Buffer para el I2C
uint16_t counterStateLED			= 0;
char bufferData[256] 				= "¡Hola! Soy el USART del STM32 y estoy funcionando.";		// Mensaje de Bienvenida

// *************** // Headers // *************** //

void delay(int time);
void initSystem(void);

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

		if(counterFilter >= 10){
			startSingleADC();									// Lanzamos la conversión ADC

			// Leemos las aceleraciones del MPU6050 en x,y, y z.
			int16_t AccelX = MPU6050_SensorValue(&handler_MPU6050,ACCEL_X);
			int16_t AccelY = MPU6050_SensorValue(&handler_MPU6050,ACCEL_Y);
			int16_t AccelZ = MPU6050_SensorValue(&handler_MPU6050,ACCEL_Z);

			// Leemos los valores del giroscopio del MPU6050 en x,y, y z.
			int16_t GirosX = MPU6050_SensorValue(&handler_MPU6050,GYRO_X);
			int16_t GirosY = MPU6050_SensorValue(&handler_MPU6050,GYRO_Y);
//			int16_t GirosZ = MPU6050_SensorValue(&handler_MPU6050,GYRO_Z);

			double fGirosX = GirosX*(250.0/32768.0);		// Convertimos los datos de ADC a grados/s
			double fGirosY = GirosY*(250.0/32768.0);		// Convertimos los datos de ADC a grados/s
//			double fGirosZ = GirosZ*(250.0/32768.0);

//			double fAccelX = ((AccelX*85)/17000)+145;
//			double fAccelY = ((AccelY*85)/17000)+145;

			dt = (counterMillis - timePrev)/10000.0;			// Calculamos el dt
			timePrev = counterMillis;						// Actualizamos el to

			//Calculamos los ángulos con el acelerometro
			double accel_ang_x = atan(AccelY/sqrt(pow(AccelX,2) + pow(AccelZ,2)))*(180.0/3.14159);
			double accel_ang_y = atan(-AccelX/sqrt(pow(AccelY,2) + pow(AccelZ,2)))*(180.0/3.14159);

			//Calculamos el angulo de rotación con giroscopio y filtro complemento
			ang_x = 0.98*(ang_x_prev+(fGirosX)*dt) + 0.02*accel_ang_x;
			ang_y = 0.98*(ang_y_prev+(fGirosY)*dt) + 0.02*accel_ang_y;

			ang_x_prev = ang_x;
			ang_y_prev = ang_y;

			duty_x = ((ang_x + 80.0) * (100.0/160.0)) + 80.0;
			duty_y = ((ang_y + 80.0) * (100.0/160.0)) + 80.0;

			// Bajamos la bandera
			counterFilter  = 0;
		}

		if(counterMPU6050 >= 350){
			// Activamos la lectura del MPU y del ADC cada 5 ms

			char bufferDataBluetooth[128] = {0};
			// Mandamos para X (Servo 1, Servo2, Servo3, PotX, PotY)
			sprintf(bufferDataBluetooth, "%d %d %d %d %d =", (int) duty_x,(int) duty_y,(int) fAdcData,
					(int) dataPotX,(int) dataPotY);
			writeMsg(&handlerUsartBluetooth, bufferDataBluetooth);


//			char bufferDataOLEDx[256] = {0};
//			sprintf(bufferDataOLEDx, "POTX: %d POTY: %d \n",(int) dataPotX,
//					(int) dataPotY);
//			writeMsg(&handlerCommTerminal, bufferDataOLEDx);

//			char bufferDataOLEDx[256] = {0};
//			sprintf(bufferDataOLEDx, "ANGX:%d,ANGY:%d,ADC:%d POTX: %d POTY: %d \n",(int) duty_x,
//					(int) duty_y,(int) adcSignal[0],(int) adcSignal[1],(int) adcSignal[2]);
//			writeMsg(&handlerCommTerminal, bufferDataOLEDx);

			counterMPU6050 = 0;
		}



		if(adcIsComplete == true){
			// Calculamos la escala para convertir la conversión ADC en movimiento del servo
			fAdcData = (((adcData - 2100.0)*100.0)/1500.0) + 80.0;

			//******************************
			adcDataFlexSensor = adcSignal[0];
			adcDataPotX		  = adcSignal[1];
			adcDataPotY		  = adcSignal[2];

			fAdcData = (((adcDataFlexSensor - 2100.0)*100.0)/1500.0) + 80.0;

			// Analizar los datos a enviar para el eje X - Potenciómetro
			if(adcDataPotX > 2100){
				dataPotX = 1;
			}else if(adcDataPotX <= 1900){
				dataPotX = 0;
			}
			else{
				dataPotX = 5;
			}
			// Analizar los datos a enviar para el eje Y - Potenciómetro
			if(adcDataPotY > 2100){
				dataPotY = 1;
			}else if(adcDataPotY <= 1900){
				dataPotY = 0;
			}
			else{
				dataPotY = 5;
			}

			//*********************************

			// Bajamos la bandera del ADC
			adcIsComplete = false;

		}

	}
}

//***********// *********** // Definición de Funciones // *********** //***********//

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

	// Configuración del ADC
//	handlerADC.channel									= ADC_CHANNEL_8;
	handlerADC.resolution								= ADC_RESOLUTION_12_BIT;
	handlerADC.samplingPeriod							= ADC_SAMPLING_PERIOD_56_CYCLES;
	handlerADC.dataAlignment							= ADC_ALIGNMENT_RIGHT;
//	handlerADC.numberOfChannels							= 1;
//	handlerADC.channelMode								= ADC_SINGLE_CHANNEL;

	handlerADC.numberOfChannels							= 3;
	handlerADC.channelMode								= ADC_MULTI_CHANNEL;
	handlerADC.channel_First							= ADC_CHANNEL_8;
	handlerADC.channel_Second							= ADC_CHANNEL_0;
	handlerADC.channel_Third							= ADC_CHANNEL_1;

	adc_Config(&handlerADC);

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


	// Configuración del USART Bluetooth para la conexión a través del HC-05
	handlerUsartBluetooth.ptrUSARTx							= USART6;
	handlerUsartBluetooth.USART_Config.USART_mode			= USART_MODE_RXTX;
	handlerUsartBluetooth.USART_Config.USART_baudrate		= USART_BAUDRATE_38400;
	handlerUsartBluetooth.USART_Config.USART_datasize		= USART_DATASIZE_8BIT;
	handlerUsartBluetooth.USART_Config.USART_parity			= USART_PARITY_NONE;
	handlerUsartBluetooth.USART_Config.USART_stopbits		= USART_STOPBIT_1;
	handlerUsartBluetooth.USART_Config.USART_interrupt 		= USART_INTERRUPT_RX_ENABLE;

	USART_Config(&handlerUsartBluetooth);

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
	handlerCommTerminal.USART_Config.USART_baudrate			= USART_BAUDRATE_38400;
	handlerCommTerminal.USART_Config.USART_datasize			= USART_DATASIZE_8BIT;
	handlerCommTerminal.USART_Config.USART_parity			= USART_PARITY_NONE;
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

	// Configuración del timer2 (STATE LED)
	handlerTimer2.ptrTIMx								= TIM2;
	handlerTimer2.timerConfig.Timer_mode				= TIMER_MODE_UP;
	handlerTimer2.timerConfig.Timer_speed				= TIMER_INCR_SPEED_1ms;
	handlerTimer2.timerConfig.Timer_period				= 200;

	Timer_Config(&handlerTimer2);

	// Configuración del timer4
	handlerTimer4.ptrTIMx								= TIM4;
	handlerTimer4.timerConfig.Timer_mode				= TIMER_MODE_UP;
	handlerTimer4.timerConfig.Timer_speed				= TIMER_INCR_SPEED_10us;
	handlerTimer4.timerConfig.Timer_period				= 10;

	Timer_Config(&handlerTimer4);
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
	counterStateLED++;
	counterMPU6050++;
	counterMillis++;
	counterFilter++;
}

void adc_Complete_Callback(void){
//	adcData		  = getADC();
//	adcIsComplete = true;

	adcSignal[dataPosition] = getADC();
	dataPosition++;
	if (dataPosition >= ADC_SIGNAL_SIZE){
		dataPosition = 0;
		adcIsComplete = true;
		ADC1 -> CR2 &= ~(ADC_CR2_SWSTART);
	}
}
