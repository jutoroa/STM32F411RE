/*
 * EXTIxDriver.c
 *
 *  Created on: 12/05/2022
 *      Author: Juan Pablo
 */


#include "EXTIxDriver.h"
#include "GPIOxDriver.h"

GPIO_Handler_t handlerEXTIxPin = {0};

void EXTI_Config(EXTI_Config_t *extiConfig){

	/* 0. Activamos la señal del RCC al SYSCFG */

	RCC -> APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	/* 1. Configuramos el PinXY como entrada Digital */

	handlerEXTIxPin.pGPIOx = extiConfig -> port;
	handlerEXTIxPin.GPIO_PinConfig.GPIO_PinNumber = extiConfig -> pin;
	handlerEXTIxPin.GPIO_PinConfig.GPIO_PinPuPdControl = extiConfig -> PinControl;
	handlerEXTIxPin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;

	GPIO_Config(&handlerEXTIxPin);

	/* 2. Asignamos el Pin al EXTI */

	if((extiConfig -> pin)< 4){
		if(extiConfig -> port == GPIOA){
			SYSCFG -> EXTICR[0] |= (SYSCFG_EXTICR1_EXTI0_PA << (4 * (extiConfig -> pin)));			// Configuramos el pin PA0
		}
		else if(extiConfig -> port == GPIOB){
			SYSCFG -> EXTICR[0] |= (SYSCFG_EXTICR1_EXTI0_PB << (4 * (extiConfig -> pin)));			// Configuramos el pin PB0
		}
		else if(extiConfig -> port == GPIOC){
			SYSCFG -> EXTICR[0] |= (SYSCFG_EXTICR1_EXTI0_PC << (4 * (extiConfig -> pin)));			// Configuramos el pin PA0
		}
		else if(extiConfig -> port == GPIOD){
			SYSCFG -> EXTICR[0] |= (SYSCFG_EXTICR1_EXTI0_PD << (4 * (extiConfig -> pin)));			// Configuramos el pin PA0
		}
		else if(extiConfig -> port == GPIOE){
			SYSCFG -> EXTICR[0] |= (SYSCFG_EXTICR1_EXTI0_PE << (4 * (extiConfig -> pin)));			// Configuramos el pin PA0
		}
		else if(extiConfig -> port == GPIOH){
			SYSCFG -> EXTICR[0] |= (SYSCFG_EXTICR1_EXTI0_PH << (4 * (extiConfig -> pin)));			// Configuramos el pin PA0
		}
	}
	else if(((extiConfig -> pin) > 3) && ((extiConfig -> pin) < 8)){
		if(extiConfig -> port == GPIOA){
			SYSCFG -> EXTICR[1] |= (SYSCFG_EXTICR1_EXTI1_PA << (4 * (extiConfig -> pin)));			// Configuramos el pin PA0
		}
		else if(extiConfig -> port == GPIOB){
			SYSCFG -> EXTICR[1] |= (SYSCFG_EXTICR1_EXTI1_PB << (4 * (extiConfig -> pin)));			// Configuramos el pin PA0
		}
		else if(extiConfig -> port == GPIOC){
			SYSCFG -> EXTICR[1] |= (SYSCFG_EXTICR1_EXTI1_PC << (4 * (extiConfig -> pin)));			// Configuramos el pin PA0
		}
		else if(extiConfig -> port == GPIOD){
			SYSCFG -> EXTICR[1] |= (SYSCFG_EXTICR1_EXTI1_PD << (4 * (extiConfig -> pin)));			// Configuramos el pin PA0
		}
		else if(extiConfig -> port == GPIOE){
			SYSCFG -> EXTICR[1] |= (SYSCFG_EXTICR1_EXTI1_PE << (4 * (extiConfig -> pin)));			// Configuramos el pin PA0
		}
		else if(extiConfig -> port == GPIOH){
			SYSCFG -> EXTICR[1] |= (SYSCFG_EXTICR1_EXTI1_PH << (4 * (extiConfig -> pin)));			// Configuramos el pin PA0
		}
	}
	else if(((extiConfig -> pin) > 7) && ((extiConfig -> pin) < 12)){
		if(extiConfig -> port == GPIOA){
			SYSCFG -> EXTICR[2] |= (SYSCFG_EXTICR1_EXTI2_PA << (4 * (extiConfig -> pin)));			// Configuramos el pin PA0
		}
		else if(extiConfig -> port == GPIOB){
			SYSCFG -> EXTICR[2] |= (SYSCFG_EXTICR1_EXTI2_PB << (4 * (extiConfig -> pin)));			// Configuramos el pin PA0
		}
		else if(extiConfig -> port == GPIOC){
			SYSCFG -> EXTICR[2] |= (SYSCFG_EXTICR1_EXTI2_PC << (4 * (extiConfig -> pin)));			// Configuramos el pin PA0
		}
		else if(extiConfig -> port == GPIOD){
			SYSCFG -> EXTICR[2] |= (SYSCFG_EXTICR1_EXTI2_PD << (4 * (extiConfig -> pin)));			// Configuramos el pin PA0
		}
		else if(extiConfig -> port == GPIOE){
			SYSCFG -> EXTICR[2] |= (SYSCFG_EXTICR1_EXTI2_PE << (4 * (extiConfig -> pin)));			// Configuramos el pin PA0
		}
		else if(extiConfig -> port == GPIOH){
			SYSCFG -> EXTICR[2] |= (SYSCFG_EXTICR1_EXTI2_PH << (4 * (extiConfig -> pin)));			// Configuramos el pin PA0
		}
	}
	else if(((extiConfig -> pin) > 11) && ((extiConfig -> pin) < 16)){
		if(extiConfig -> port == GPIOA){
			SYSCFG -> EXTICR[3] |= (SYSCFG_EXTICR1_EXTI3_PA << (4 * (extiConfig -> pin)));			// Configuramos el pin PA0
		}
		else if(extiConfig -> port == GPIOB){
			SYSCFG -> EXTICR[3] |= (SYSCFG_EXTICR1_EXTI3_PB << (4 * (extiConfig -> pin)));			// Configuramos el pin PA0
		}
		else if(extiConfig -> port == GPIOC){
			SYSCFG -> EXTICR[3] |= (SYSCFG_EXTICR1_EXTI3_PC << (4 * (extiConfig -> pin)));			// Configuramos el pin PA0
		}
		else if(extiConfig -> port == GPIOD){
			SYSCFG -> EXTICR[3] |= (SYSCFG_EXTICR1_EXTI3_PD << (4 * (extiConfig -> pin)));			// Configuramos el pin PA0
		}
		else if(extiConfig -> port == GPIOE){
			SYSCFG -> EXTICR[3] |= (SYSCFG_EXTICR1_EXTI3_PE << (4 * (extiConfig -> pin)));			// Configuramos el pin PA0
		}
		else if(extiConfig -> port == GPIOH){
			SYSCFG -> EXTICR[3] |= (SYSCFG_EXTICR1_EXTI3_PH << (4 * (extiConfig -> pin)));			// Configuramos el pin PA0
		}
	}
	/* 3. Configuramos el trigger EXTI_RTSR*/

	// Activamos la configuración para flanco de subida
	if((extiConfig -> trigger) == TRIGGER_RISING){
		EXTI -> RTSR |= (EXTI_RTSR_TR0 << (extiConfig -> pin));
	}
	// Activamos la configuración para el flanco de bajada
	else if((extiConfig -> trigger) == TRIGGER_FALLING){
		EXTI -> FTSR |= (EXTI_FTSR_TR0 << (extiConfig -> pin));
	}
	// Activamos la configuración para los dos
	else{
		EXTI -> RTSR |= (EXTI_RTSR_TR0 << (extiConfig -> pin));
		EXTI -> RTSR |= (EXTI_FTSR_TR0 << (extiConfig -> pin));
	}

	/* 4. Configuramos el interrupt mask register */

	EXTI -> IMR |= (EXTI_IMR_MR0 << (extiConfig -> pin));

	/* 5. Desactivamos las interruociones globales */

	__disable_irq();

	/* 6. Matriculamos las interrupciones en el NVIC */

	switch(extiConfig->pin){
		case PIN_0 :{
			__NVIC_EnableIRQ(EXTI0_IRQn);
			break;
		}
		case PIN_1 :{
			__NVIC_EnableIRQ(EXTI1_IRQn);
			break;
		}
		case PIN_2 :{
			__NVIC_EnableIRQ(EXTI2_IRQn);
			break;
		}
		case PIN_3 :{
			__NVIC_EnableIRQ(EXTI3_IRQn);
			break;
		}
		case PIN_4 :{
			__NVIC_EnableIRQ(EXTI4_IRQn);
			break;
		}
		default:{
			break;
		}
		}

	if(((extiConfig -> pin) >= PIN_5) && ((extiConfig -> pin) <= PIN_9)) {
		__NVIC_EnableIRQ(EXTI9_5_IRQn);
	}

	if(((extiConfig -> pin) >= PIN_10) && ((extiConfig -> pin) <= PIN_15)) {
		__NVIC_EnableIRQ(EXTI15_10_IRQn);
	}

	/* 7. Activamos las interrupciones globales */

	__enable_irq();
}

void EXTI0_IRQHandler(void){
	// Bajamos la bandera
	EXTI -> PR |= (EXTI_PR_PR0);

	// Llamamos al callback
	EXTI0_Callback();
}

void EXTI1_IRQHandler(void){
	// Bajamos la bandera
	EXTI -> PR |= (EXTI_PR_PR1);

	// Llamamos al callback
	EXTI1_Callback();
}

void EXTI2_IRQHandler(void){
	// Bajamos la bandera
	EXTI -> PR |= (EXTI_PR_PR2);

	// Llamamos al callback
	EXTI2_Callback();
}

void EXTI3_IRQHandler(void){
	// Bajamos la bandera
	EXTI -> PR |= (EXTI_PR_PR3);

	// Llamamos al callback
	EXTI3_Callback();
}

void EXTI4_IRQHandler(void){
	// Bajamos la bandera
	EXTI -> PR |= (EXTI_PR_PR4);

	// Llamamos al callback
	EXTI4_Callback();
}

void EXTI9_5_IRQHandler(void){
	// Bajamos la bandera
	EXTI -> PR |= (EXTI_PR_PR0);

// Se verifica dónde se levantó la bandera
	// Se verifica para el PIN_5
	if(EXTI -> PR & (EXTI_PR_PR5)){

		// Se baja la bandera para el PIN_5
		EXTI -> PR |= EXTI_PR_PR5;

		// Se hace el llamado al a función que se ejecutará en el main.
		EXTI5_Callback();
	}

	// Se verifica para el PIN_6
	else if(EXTI -> PR & (EXTI_PR_PR6)){

		// Se baja la bandera para el PIN_6
		EXTI -> PR |= EXTI_PR_PR6;

		// Se hace el llamado al a función que se ejecutará en el main.
		EXTI6_Callback();
	}

	// Se verifica para el PIN_7
	else if(EXTI -> PR & (EXTI_PR_PR7)){

		// Se baja la bandera para el PIN_7
		EXTI -> PR |= EXTI_PR_PR7;

		// Se hace el llamado al a función que se ejecutará en el main.
		EXTI7_Callback();
	}

	// Se verifica para el PIN_8
	else if(EXTI -> PR & (EXTI_PR_PR8)){

		// Se baja la bandera para el PIN_8
		EXTI -> PR |= EXTI_PR_PR8;

		// Se hace el llamado al a función que se ejecutará en el main.
		EXTI8_Callback();
	}

	// Se verifica para el PIN_9
	else if(EXTI -> PR & (EXTI_PR_PR9)){

		// Se baja la bandera para el PIN_9
		EXTI -> PR |= EXTI_PR_PR9;

		// Se hace el llamado al a función que se ejecutará en el main.
		EXTI9_Callback();
	}
}


void EXTI15_10_IRQHandler(void){

	// Se verifica dónde se levantó la bandera
	// Se verifica para el PIN_10
	if(EXTI -> PR & (EXTI_PR_PR10)){

		// Se baja la bandera para el PIN_10
		EXTI -> PR |= EXTI_PR_PR10;

		// Se hace el llamado al a función que se ejecutará en el main.
		EXTI10_Callback();
	}

	// Se verifica para el PIN_11
	else if(EXTI -> PR & (EXTI_PR_PR11)){

		// Se baja la bandera para el PIN_11
		EXTI -> PR |= EXTI_PR_PR11;

		// Se hace el llamado al a función que se ejecutará en el main.
		EXTI11_Callback();
	}

	// Se verifica para el PIN_12
	else if(EXTI -> PR & (EXTI_PR_PR12)){

		// Se baja la bandera para el PIN_12
		EXTI -> PR |= EXTI_PR_PR12;

		// Se hace el llamado al a función que se ejecutará en el main.
		EXTI12_Callback();
	}

	// Se verifica para el PIN_13
	else if(EXTI -> PR & (EXTI_PR_PR13)){

		// Se baja la bandera para el PIN_13
		EXTI -> PR |= EXTI_PR_PR13;

		// Se hace el llamado al a función que se ejecutará en el main.
		EXTI13_Callback();
	}

	// Se verifica para el PIN_14
	else if(EXTI -> PR & (EXTI_PR_PR14)){

		// Se baja la bandera para el PIN_14
		EXTI -> PR |= EXTI_PR_PR14;

		// Se hace el llamado al a función que se ejecutará en el main.
		EXTI14_Callback();
	}

	// Se verifica para el PIN_15
	else if(EXTI -> PR & (EXTI_PR_PR15)){

		// Se baja la bandera para el PIN_15
		EXTI -> PR |= EXTI_PR_PR15;

		// Se hace el llamado al a función que se ejecutará en el main.
		EXTI15_Callback();
	}
}

/* Función débil, que debe ser sobreescrita en el main */
__attribute__((weak)) void EXTI0_Callback(void){
	__NOP();
}

__attribute__((weak)) void EXTI1_Callback(void){
	__NOP();
}

__attribute__((weak)) void EXTI2_Callback(void){
	__NOP();
}

__attribute__((weak)) void EXTI3_Callback(void){
	__NOP();
}

__attribute__((weak)) void EXTI4_Callback(void){
	__NOP();
}

__attribute__((weak)) void EXTI5_Callback(void){
	__NOP();
}

__attribute__((weak)) void EXTI6_Callback(void){
	__NOP();
}

__attribute__((weak)) void EXTI7_Callback(void){
	__NOP();
}

__attribute__((weak)) void EXTI8_Callback(void){
	__NOP();
}

__attribute__((weak)) void EXTI9_Callback(void){
	__NOP();
}

__attribute__((weak)) void EXTI10_Callback(void){
	__NOP();
}

__attribute__((weak)) void EXTI11_Callback(void){
	__NOP();
}

__attribute__((weak)) void EXTI12_Callback(void){
	__NOP();
}

__attribute__((weak)) void EXTI13_Callback(void){
	__NOP();
}

__attribute__((weak)) void EXTI14_Callback(void){
	__NOP();
}

__attribute__((weak)) void EXTI15_Callback(void){
	__NOP();
}


/*

void configEXTIPin(uint8_t EXTIPort, uint8_t EXTIPin, uint8_t PinPudControl){
	// Con esta función seleccionamos el Pin y el Puerto y lo configuramos.

	handlerEXTIxPin.pGPIOx = EXTIPort;
	handlerEXTIxPin.GPIO_PinConfig.GPIO_PinNumber = EXTIPin;
	handlerEXTIxPin.GPIO_PinConfig.GPIO_PinPuPdControl = PinPudControl;
	handlerEXTIxPin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
}

//configEXTIPin(extiConfig -> port,extiConfig -> pin,extiConfig -> PinControl);

*/
