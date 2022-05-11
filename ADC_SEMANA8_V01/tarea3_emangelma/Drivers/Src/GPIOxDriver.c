/**
 ************************************************************************************************************************************************************
 * @file           : GPIOxDriver.c
 * @author         : Juan Pablo Toro Arenas y Emmanuel Ángel Maya
 * @brief          : Archivo .c periférico GPIOx
 ************************************************************************************************************************************************************
 * En este programa se escribe el código .c necesario para el periférico GPIOx
 * Contiene la configuración necesaria para trabajar con el registro GPIOx:
 * 	- Pin number
 * 	- Pin Mode
 * 	- Pin Speed
 * 	- Pin Pull up o pull down
 * 	- Pin Output type (Open drain o PUPD)
 * 	- Pin Alt function mode.
 ************************************************************************************************************************************************************
 */
#include "GPIOxDriver.h"

/**
Para cualquier periférico, hay varios pasos que siempre se deben seguir en un
orden estricto para poder que el sistema permita configurar el periférico X.
Lo primero y más importante es activar la señal del reloj principal hacia ese
elemento específico (relacionado con el periférico RCC), a esto llamaremos
simplemente "activar el periférico o activar la señal de reloj del periférico".
*/
void GPIO_Config (GPIO_Handler_t *pGPIOHandler){

	// Variable para hacer todo paso a paso.
	uint32_t auxConfig = 0;
	uint32_t auxPosition = 0;

	// 1) Activación del periférico
	// Se verifica para GPIOA.
	if (pGPIOHandler -> pGPIOx == GPIOA){

		// Escribimos 1 (SET) en la posición correspondiente al GPIOA.
		RCC -> AHB1ENR |= (RCC_AHB1ENR_GPIOAEN);
	}
	// Se verifica para GPIOB.
	else if (pGPIOHandler -> pGPIOx == GPIOB){

		// Escribimos 1 (SET) en la posición correspondiente al GPIOB.
		RCC -> AHB1ENR |= (RCC_AHB1ENR_GPIOBEN);
	}
	// Se verifica para GPIOC.
	else if (pGPIOHandler -> pGPIOx == GPIOC){

		// Se escribe 1 (SET) en la posición correspondiente al GPIOC.
		RCC -> AHB1ENR |= (RCC_AHB1ENR_GPIOCEN);
	}
	// Se verifica para GPIOD.
	else if (pGPIOHandler -> pGPIOx == GPIOD){

		// Se escribe 1 (SET) en la posición correspondiente al GPIOD.
		RCC -> AHB1ENR |= (RCC_AHB1ENR_GPIODEN);
	}
	// Se verifica para GPIOE.
	else if (pGPIOHandler -> pGPIOx == GPIOE){

		// Se escribe 1 (SET) en la posición correspondiente al GPIOE.
		RCC -> AHB1ENR |= (RCC_AHB1ENR_GPIOEEN);
	}
	// Se verifica para GPIOH.
	else if (pGPIOHandler -> pGPIOx == GPIOH){

		// Se escribe 1 (SET) en la posición correspondiente al GPIOH.
		RCC -> AHB1ENR |= (RCC_AHB1ENR_GPIOHEN);
	}

	// Después de activado, se puede comenzar a configurar.

	// 2) Configuración del registro GPIOx_MODER
	// Acá se está leyendo la config, moviendo "PinNumber" veces hacia la izquierda ese valor (shift left)
	// y todo eso se carga en la variable aux_Config.
	auxConfig = (pGPIOHandler -> GPIO_PinConfig.GPIO_PinMode << 2 * pGPIOHandler -> GPIO_PinConfig.GPIO_PinNumber);

	// Antes de cargar el nuevo valor, se limpian los bits específicos de ese registro (Se debe escribir 0b00)
	// para lo cual se aplica una máscara y una operación bitwise AND.
	pGPIOHandler -> pGPIOx -> MODER &= ~(0b11 << 2 * pGPIOHandler -> GPIO_PinConfig.GPIO_PinNumber);

	// Se carga a auxConfig en el registro MODER.
	pGPIOHandler -> pGPIOx -> MODER |= auxConfig;

	// 3) Se configura el registro GPIOx_OTYPER.
	// De nuevo, se lee y mueve el valor un número "PinNumber" de veces.
	auxConfig = (pGPIOHandler -> GPIO_PinConfig.GPIO_PinOPType << pGPIOHandler -> GPIO_PinConfig.GPIO_PinNumber);

	// Se limpia antes de cargar.
	pGPIOHandler -> pGPIOx -> OTYPER &= ~(SET << pGPIOHandler -> GPIO_PinConfig.GPIO_PinNumber);

	// Se carga el resultado sobre el registro adecuado.
	pGPIOHandler -> pGPIOx -> OTYPER |= auxConfig;

	// 4) Se configura ahora la velocidad.
	auxConfig = (pGPIOHandler -> GPIO_PinConfig.GPIO_PinSpeed << 2*pGPIOHandler -> GPIO_PinConfig.GPIO_PinNumber);

	// Se limpia la posición antes de cargar la nueva configuración.
	pGPIOHandler -> pGPIOx -> OSPEEDR &= ~(0b11 << 2*pGPIOHandler -> GPIO_PinConfig.GPIO_PinNumber);

	// Se carga el resultado sobre el registro adecuado.
	pGPIOHandler -> pGPIOx -> OSPEEDR |= auxConfig;

	// 5) Se configura si se desea pull-up, pull-down o flotante.
	auxConfig = (pGPIOHandler -> GPIO_PinConfig.GPIO_PinPuPdControl << 2*pGPIOHandler -> GPIO_PinConfig.GPIO_PinNumber);

	// Se limpia la posición antes de cargar la nueva configuración.
	pGPIOHandler -> pGPIOx -> PUPDR &= ~(0b11 << 2*pGPIOHandler -> GPIO_PinConfig.GPIO_PinNumber);

	// Se carga el resultado sobre el registro adecuado.
	pGPIOHandler -> pGPIOx -> PUPDR |= auxConfig;

	// Configuración funciones alternativas.
	if (pGPIOHandler -> GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_ALTFN){

		// Se selecciona primero si se debe utilizar el registro bajo (AFRL) o el alto (AFRH).
		if (pGPIOHandler -> GPIO_PinConfig.GPIO_PinNumber < 8){

			// Configuración del registro AFRL, que controla los pines del PIN_0 al PIN_7.
			auxPosition = 4 * pGPIOHandler -> GPIO_PinConfig.GPIO_PinNumber;

			// Se limpia primero la posición del registro que se desea escribir.
			pGPIOHandler -> pGPIOx -> AFR[0] &= ~(0b1111 << auxPosition);

			// Y se escribe el valor configurado en la posición seleccionada.
			pGPIOHandler -> pGPIOx -> AFR[0] |= (pGPIOHandler -> GPIO_PinConfig.GPIO_PinAltFunMode << auxPosition);
		}
		else{

			// Modificación del registro AFRH, que controla los pines del PIN_8 al PIN_15.
			auxPosition = 4 * (pGPIOHandler -> GPIO_PinConfig.GPIO_PinNumber -8);

			// Se limpia primero la posición del registro que se desea escribir.
			pGPIOHandler -> pGPIOx -> AFR[1] &= ~(0b1111 << auxPosition);

			// Y se escribe el valor configurado en la posición seleccionada.
			pGPIOHandler -> pGPIOx -> AFR[1] |= (pGPIOHandler -> GPIO_PinConfig.GPIO_PinAltFunMode << auxPosition);
		}
	}
} 			// Fin del GPIO_Config

/* Función utilizada para cambiar de estado el pin entregado en el handler, asignando
el valor entregado en la variable newState. */
void GPIO_WritePin (GPIO_Handler_t *pPinHandler, uint8_t newState){

	// Se limpia la posición que se desea usar.
	// pPinHandler -> pGPIOx -> ODR &= ~(SET << pPinHandler -> GPIO_PinConfig.GPIO_PinNumber);
	if(newState == SET){

		// Se trabajará con la parte baja del registro.
		pPinHandler -> pGPIOx -> BSRR |= (SET << pPinHandler -> GPIO_PinConfig.GPIO_PinNumber);
	}
	else{

		// Se trabajará con la parte alta del registro.
		pPinHandler -> pGPIOx -> BSRR |= (SET << (pPinHandler -> GPIO_PinConfig.GPIO_PinNumber + 16));
	}
}

/* Función para leer el estado de un pin específico. */
uint32_t GPIO_ReadPin (GPIO_Handler_t *pPinHandler){

	// Se crea una variable auxiliar que será retornada más tarde.
	uint32_t pinValue = 0;

	// Se carga el valor del registro IDR, desplazado a la derecha tantas veces como la ubicación
	// del pin específico.
	pinValue = (pPinHandler -> pGPIOx -> IDR >> pPinHandler -> GPIO_PinConfig.GPIO_PinNumber);
	pinValue &= 0b1;
	return pinValue;

}
