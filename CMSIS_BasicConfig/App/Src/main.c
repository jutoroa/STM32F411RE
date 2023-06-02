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

TIMER_Handler_t handlerTimer2 			= {0};	// Timer2

USART_Handler_t handlerCommTerminal		= {0};	// Usart para la terminal en USART 2

uint8_t 	rxData					= 0;				// Variable donde se guardarán los datos obtenidos por el RX

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
}

/* ----------------------------------------------------------------------
* Defines each of the tests performed
* ------------------------------------------------------------------- */
#define MAX_BLOCKSIZE   32
#define DELTA           (0.00001f)


/* ----------------------------------------------------------------------
* Test input data for Floating point sin_cos example for 32-blockSize
* Generated by the MATLAB randn() function
* ------------------------------------------------------------------- */

const float32_t testInput_f32[MAX_BLOCKSIZE] =
{
        -1.244916875853235400,  -4.793533929171324800,  0.360705030233248850,   0.827929644170887320,   -3.299532218312426900,  3.427441903227623800,   3.422401784294607700,   -0.108308165334010680,
        0.941943896490312180,   0.502609575000365850,   -0.537345278736373500,  2.088817392965764500,   -1.693168684143455700,  6.283185307179590700,   -0.392545884746175080,  0.327893095115825040,
        3.070147440456292300,   0.170611405884662230,   -0.275275082396073010,  -2.395492805446796300,  0.847311163536506600,   -3.845517018083148800,  2.055818378415868300,   4.672594161978930800,
        -1.990923030266425800,  2.469305197656249500,   3.609002606064021000,   -4.586736582331667500,  -4.147080139136136300,  1.643756718868359500,   -1.150866392366494800,  1.985805026477433800


};

const float32_t testRefOutput_f32 = 1.000000000;

/* ----------------------------------------------------------------------
* Declare Global variables
* ------------------------------------------------------------------- */
uint32_t blockSize = 32;
float32_t  testOutput;
float32_t  cosOutput;
float32_t  sinOutput;
float32_t  cosSquareOutput;
float32_t  sinSquareOutput;

/* ----------------------------------------------------------------------
* Max magnitude FFT Bin test
* ------------------------------------------------------------------- */

//arm_status status;

int main(void)
{
	char bufferData[128] = {0};

	initSystem();
	float32_t diff;
	uint32_t i;

        for(i=0; i< blockSize; i++){
        	cosOutput = arm_cos_f32(testInput_f32[i]);
            sinOutput = arm_sin_f32(testInput_f32[i]);

			arm_mult_f32(&cosOutput, &cosOutput, &cosSquareOutput, 1);
			arm_mult_f32(&sinOutput, &sinOutput, &sinSquareOutput, 1);

			arm_add_f32(&cosSquareOutput, &sinSquareOutput, &testOutput, 1);

                /* absolute value of difference between ref and test */
            diff = fabsf(testRefOutput_f32 - testOutput);

            /* Comparison of sin_cos value with reference */
            if(diff > DELTA)
            {
//                   status = ARM_MATH_TEST_FAILURE;
               	// Imprimimos los valores por consola
               	sprintf(bufferData, "ARM_MATH_TEST_FAILURE DIFF = %f \n\r",diff);
               	writeMsg(&handlerCommTerminal, bufferData);
            }
            else{
               	// Imprimimos los valores por consola
               	sprintf(bufferData, "ARM_MATH_TEST_SUCCESFULL DIFF = %f \n\r",diff);
               	writeMsg(&handlerCommTerminal, bufferData);
            }

//            if( status == ARM_MATH_TEST_FAILURE)
//            {
//               while(1);
//            }

    }

    while(1){

    }
}

// Timer encargado del StateLED
void Timer2_Callback(void){
	handlerStateLED.pGPIOx -> ODR ^= GPIO_ODR_OD5;		// Encendido y apagado StateLED
}

void USART2_Callback(void){
	// Lectura de los datos recibidos
	rxData = returnData();
}
