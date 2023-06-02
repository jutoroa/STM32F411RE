#include <math.h>
#include "arm_math.h"

#include "GPIOxDriver.h"
#include "USARTxDriver.h"
#include "TIMxDriver.h"
#include "FPUDriver.h"
#include "EXTIxDriver.h"

/* Defines for the USART and BLINKY pheripheral */

// Handler para todos los periféricos que se van a implementar en el desarrollo del proyecto
GPIO_Handler_t 	handlerStateLED 		= {0};	// StateLED
GPIO_Handler_t 	handlerPinTX 			= {0};	// handlerPinTX
GPIO_Handler_t 	handlerPinRX 			= {0};	// handlerPinRX
EXTI_Config_t   handlerUserButton		= {0};	// handler del PC13

TIMER_Handler_t handlerTimer2 			= {0};	// Timer2
TIMER_Handler_t handlerTimer3 			= {0};	// Timer3

USART_Handler_t handlerCommTerminal		= {0};	// Usart para la terminal en USART 2

uint8_t 	rxData					= 0;				// Variable donde se guardarán los datos obtenidos por el RX

uint8_t 	flagData                = 0;
uint8_t		userButtonFlag			= 0;
uint8_t 	counterX				= 5;
uint8_t 	counterY				= 13;
uint8_t 	counterZ				= 25;

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
	handlerCommTerminal.USART_Config.USART_baudrate			= USART_BAUDRATE_230400;
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

	// Configuración del timer3
	handlerTimer3.ptrTIMx								= TIM3;
	handlerTimer3.timerConfig.Timer_mode				= TIMER_MODE_UP;
	handlerTimer3.timerConfig.Timer_speed				= TIMER_INCR_SPEED_10us;
	handlerTimer3.timerConfig.Timer_period				= 100;

	Timer_Config(&handlerTimer3);
	startTimer(&handlerTimer3);


//	handlerUserButton.port = GPIOC;
//	handlerUserButton.pin  = PIN_13;
//	handlerUserButton.PinControl = GPIO_PUPDR_PULLUP;
//	handlerUserButton.trigger	 = TRIGGER_FALLING;
//
//	EXTI_Config(&handlerUserButton);
}

#define MAX_BLOCKSIZE   32

const float32_t testInput_f32[MAX_BLOCKSIZE] ={
        -1.244916875853235400,  -4.793533929171324800,  0.360705030233248850,   0.827929644170887320,   -3.299532218312426900,  3.427441903227623800,   3.422401784294607700,   -0.108308165334010680,
        0.941943896490312180,   0.502609575000365850,   -0.537345278736373500,  2.088817392965764500,   -1.693168684143455700,  6.283185307179590700,   -0.392545884746175080,  0.327893095115825040,
        3.070147440456292300,   0.170611405884662230,   -0.275275082396073010,  -2.395492805446796300,  0.847311163536506600,   -3.845517018083148800,  2.055818378415868300,   4.672594161978930800,
        -1.990923030266425800,  2.469305197656249500,   3.609002606064021000,   -4.586736582331667500,  -4.147080139136136300,  1.643756718868359500,   -1.150866392366494800,  1.985805026477433800
};

/******* Variables Globales *******/
uint8_t i = 0;
uint32_t counterNData = 1;
uint8_t flagDataReady = 1;
#define numberOfData	10000

int main(void)
{
	char bufferData[128] = {0};

	initSystem();



    while(1){
    	if(counterNData <= numberOfData){
			if(flagData){
		    	if(i >= 28){
		    		i = 0;
		    	}
				// Imprimimos los valores por consola
				sprintf(bufferData, "%d %d %d \n",(int) (testInput_f32[i+1]+counterX),
						(int) (testInput_f32[i+2]+counterY),(int) (testInput_f32[i+3]+counterZ));
				writeMsg(&handlerCommTerminal, bufferData);
				// Bajamos la bandera
				flagData = 0;
			}
    	}
    	else{
    		stopTimer(&handlerTimer3);
    		if(flagDataReady){
    			sprintf(bufferData, "%d %d %d \n",(int) 0,(int) 0,(int) 0);
    			writeMsg(&handlerCommTerminal, bufferData);
    			writeMsg(&handlerCommTerminal, bufferData);
    			GPIO_WritePin(&handlerStateLED, SET);
    			flagDataReady = 0;
    			flagData = 0;
    		}
    	}
    }
}

// Timer encargado del StateLED
void Timer2_Callback(void){
	handlerStateLED.pGPIOx -> ODR ^= GPIO_ODR_OD5;		// Encendido y apagado StateLED
}

void Timer3_Callback(void){
	counterNData++;
	flagData = 1;
	i++;
}

void USART2_Callback(void){
	// Lectura de los datos recibidos
	rxData = returnData();
}

//void EXTI13_Callback(void){
//	userButtonFlag = 1;
//}
