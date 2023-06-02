#include <math.h>
#include "arm_math.h"

#include "GPIOxDriver.h"
#include "USARTxDriver.h"
#include "TIMxDriver.h"
#include "FPUDriver.h"
#include "AdcDriverWorkshopTaller5.h"

/* Defines for the USART and BLINKY pheripheral */

// Handler para todos los periféricos que se van a implementar en el desarrollo del proyecto
GPIO_Handler_t 	handlerStateLED 		= {0};	// StateLED
GPIO_Handler_t 	handlerPinTX 			= {0};	// handlerPinTX
GPIO_Handler_t 	handlerPinRX 			= {0};	// handlerPinRX

TIMER_Handler_t handlerTimer2 			= {0};	// Timer2
TIMER_Handler_t handlerTimer5 			= {0};	// Timer2

USART_Handler_t handlerCommTerminal		= {0};	// Usart para la terminal en USART 2

uint8_t 	rxData					= 0;				// Variable donde se guardarán los datos obtenidos por el RX

void initSystem(void);

ADC_Config_t channel_1				= {0};

char bufferData[128] = {0};

#define numberOfChannels	2
uint8_t adcIsComplete		= 0;
uint8_t adcCounter			= 0;
uint16_t dataADC = 0;

int main(void)
{
	initSystem();
	writeMsg(&handlerCommTerminal, "Hola soy el STM32F411 \n\r");

    while(1){

    	if(rxData != '\0'){
			// Realizamos un ECO
			writeChar(&handlerCommTerminal,rxData);

			if(rxData == 'a'){
				// Iniciamos una conversión ADC
				adcCounter = 0;
				startSingleADC();
				}

			// Limpiamos el dato de RX
			rxData = '\0';
    	}

		if(adcIsComplete == 1){
			// Enviamos los datos por consola
			sprintf(bufferData,"Data_Channel_1: %u \n",dataADC);
			writeMsg(&handlerCommTerminal, bufferData);
			// Bajamos la bandera del ADC
			adcIsComplete = 0;
		}
    }
}

void initSystem(void){
	FPUInit();

	// Configuración el State LED
	handlerStateLED.pGPIOx								= GPIOA;
	handlerStateLED.GPIO_PinConfig.GPIO_PinNumber		= PIN_5;
	handlerStateLED.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
	handlerStateLED.GPIO_PinConfig.GPIO_PinOPType		= GPIO_OTYPE_PUSHPULL;
	handlerStateLED.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_PUPDR_NOTHING;
	handlerStateLED.GPIO_PinConfig.GPIO_PinSpeed		= GPIO_OSPEED_FAST;

	//Cargamos la configuraciónif(rxData != '\0'){

	GPIO_Config(&handlerStateLED);

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
	startTimer(&handlerTimer2);


	// Configuración del timer2
	handlerTimer5.ptrTIMx								= TIM5;
	handlerTimer5.timerConfig.Timer_mode				= TIMER_MODE_UP;
	handlerTimer5.timerConfig.Timer_speed				= TIMER_INCR_SPEED_1ms;
	handlerTimer5.timerConfig.Timer_period				= 50;

	Timer_Config(&handlerTimer5);
	startTimer(&handlerTimer5);

	// Configuración del ADC
	channel_1.channel 			= ADC_CHANNEL_0;
	channel_1.dataAlignment 	= ADC_ALIGNMENT_RIGHT;
	channel_1.samplingPeriod 	= ADC_SAMPLING_PERIOD_84_CYCLES;
	channel_1.resolution		= ADC_RESOLUTION_12_BIT;

	adc_Config(&channel_1);
}

// Timer encargado del StateLED
void Timer2_Callback(void){
	handlerStateLED.pGPIOx -> ODR ^= GPIO_ODR_OD5;		// Encendido y apagado StateLED
}

void Timer5_Callback(void){
	startSingleADC();
}

void USART2_Callback(void){
	// Lectura de los datos recibidos
	rxData = returnData();
}

void adcComplete_Callback(void){
	dataADC = getADC();
	adcIsComplete = 1;
}
