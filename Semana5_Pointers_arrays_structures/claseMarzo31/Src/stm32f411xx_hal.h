/*
 * stm32f411xx_hal.h
 *
 *  Created on: Mar 31, 2022
 *      Author: if420_01
 */

#ifndef INC_STM32F411XX_HAL_H_
#define INC_STM32F411XX_HAL_H_

#include <stdint.h>
#include <stddef.h>

#define HSI_CLOCK_SPEED		16000000			// Value foe the main clock signal (HSI -> High Speed Internal)
#define HSE_CLOCK_SPEED		4000000				// Value foe the main clock signal (HSE -> High Speed External)

#define NOP()		asm("NOP")
#define __weak		__attribute__((weak))

/*
 * Base addres of flash and SRAM memories
 * Datasheet, Memory Map, Figura 14
 * (Remember, 1KByte = 1024 byteS)
 */
#define FLASH_BASE_ADDR			0x08000000U		// Esta es la memoria del programa, 512 KB
#define SRAM_BASE_ADDR			0x20000000U		// Esta es la memoria RAM, 128 KB.

/* Nota: Observar que existen unos registros específicos del Cortex M4 en la región 0xE0000000U
 * Los controladres de las interrupciones se encuentran allí, por ejemplo.
 * Esto se verá a su debido tiempo.
 */

/* Nota:
 * Ahora agregamos la dirección de memoria base para cada uno de los periféricos que posee el micro
 * En el "datasheet" del micro, Figura 14 (Memory Map) encontramos el mapa de los buses:
 * - APB1 (Advance Peripheral Bus)
 * - APB2
 * - AHB1 (Advance High-performance Bus)
 * - AHB2
 */

/**
 * AHBx and APBx Bus Peripherals base addresses
 */
#define APB1_BASE_ADDR			0x40000000U
#define APB2_BASE_ADDR			0x40010000U
#define AHB1_BASE_ADDR			0x40020000U
#define AHB2_BASE_ADDR			0x50000000U

/**
 * Y ahora debemos hacer lo mismo pero cada una de las posiciones de memoria de cada uno de los
 * periféricos descritos en la tabla 1 del manual de referencia del micro.
 * Observe que en dicha tabla está a su vez dividida en cuatro segmentos, cada uno correspondiente
 * a APB1, APB2, AHB1, AHB2.
 *
 * Comenzar de arriba hacia abajo como se muestra en la tabla. Inicia USB_OTG_FS (AHB2)
 *
 * NOTA: Solo lo vamos a hacer con los elementos que necesitamos en este ejercicio: RCC y GPIOx
 */

/* Posiciones de memoria para periféricos del AHB2 */
#define USB_OTG_FS_BASE_ADDR	(AHB2_BASE_ADDR + 0x0000U)

/* Posiciones de memoria para periféricos del AHB1
 * Observar que NO está completa */
#define RCC_BASE_ADDR			(AHB1_BASE_ADDR + 0x3800U)
#define GPIOH_BASE_ADDR			(AHB1_BASE_ADDR + 0x1C00U)
#define GPIOE_BASE_ADDR			(AHB1_BASE_ADDR + 0x1000U)
#define GPIOD_BASE_ADDR			(AHB1_BASE_ADDR + 0x0C00U)
#define GPIOC_BASE_ADDR			(AHB1_BASE_ADDR + 0x0800U)
#define GPIOB_BASE_ADDR			(AHB1_BASE_ADDR + 0x0400U)
#define GPIOA_BASE_ADDR			(AHB1_BASE_ADDR + 0x0000U)

/* Posiciones de memoria para periféricos del APB2
 * NOTA: NO los necesitamos en este ejercicio, NO copiar
 */

/**
 * Macros genéricos
 */
#define ENABLE				1
#define DISABLE				0
#define SET					ENABLE
#define CLEAR				DISABLE
#define RESET				DISABLE
#define GPIO_PIN_SET		SET
#define GPIO_PIN_RESET		RESET
#define FLAG_SET			SET
#define FLAG_RESET			RESET
#define I2C_WRITE			0
#define I2C_READ			1

/* +++====== 	INICIO de la descripción de los elementos que componen el periférico ======+++ */

/* Definición de la estructura de datos que representa a cada uno de los registros que componen el
 * periférico RCC.
 *
 * Debido a los temas que se van a manejar en el curso, solo se deben definir los bits de los registros:
 * 6.3.1 (RCC_CR) hasta el 6.3.12 (RCC_APB2ENR). 6.3.17 (RCC_BDCR) y 6.3.18 (RCC_CSR)
 *
 * NOTA: La posición de memoria (offset) debe encajar perfectamente con la posición de memoria indicada en la hoja de datos del equipo. Observe que los elementos "reserved" también están presentes allí.
 * */
 typedef struct
 {
	 volatile uint32_t CR;			// Clock Control Register					ADDR_OFFSET:	0x00
	 volatile uint32_t PLLCFGR;		// PLL configuration Register				ADDR_OFFSET:	0x04
	 volatile uint32_t CFGR;		// Clock Configuration Register				ADDR_OFFSET:	0x08
	 volatile uint32_t CIR;			// Clock Interrupt Register					ADDR_OFFSET:	0x0C
	 volatile uint32_t AHB1RSTR;	// AHB1 Peripheral Reset Register			ADDR_OFFSET:	0x10
	 volatile uint32_t AHB2RSTR;	// AHB2 Peripheral Reset Register			ADDR_OFFSET:	0x14
	 volatile uint32_t reserved0;	// reserved									ADDR_OFFSET:	0x18
	 volatile uint32_t reserved1;	// reserved									ADDR_OFFSET:	0x1C
	 volatile uint32_t APB1RSTR;	// APB1 Peripheral reset register			ADDR_OFFSET:	0x20
	 volatile uint32_t APB2RSTR;	// APB2 Peripheral reset register			ADDR_OFFSET:	0x24
	 volatile uint32_t reserved2;	// reserved									ADDR_OFFSET:	0x28
	 volatile uint32_t reserved3;	// reserved									ADDR_OFFSET:	0x2C
	 volatile uint32_t AHB1ENR;		// AHB1 Peripheral clock enable register	ADDR_OFFSET:	0x30
	 volatile uint32_t AHB2ENR;		// AHB2 Peripheral clock enable register	ADDR_OFFSET:	0x34
	 volatile uint32_t reserved4;	// reserved									ADDR_OFFSET:	0x38
	 volatile uint32_t reserved5;	// reserved									ADDR_OFFSET:	0x3C
	 volatile uint32_t APB1ENR;		// APB1 peripheral clock enable register	ADDR_OFFSET:	0x40
	 volatile uint32_t APB2ENR;		// APB2 peripheral clock enable register	ADDR_OFFSET:	0x44
	 volatile uint32_t reserved6;	// reserved									ADDR_OFFSET:	0x48
	 volatile uint32_t reserved7;	// reserved									ADDR_OFFSET:	0x4C
	 volatile uint32_t AHB1LPENR;	// AHB1 clock enable low power register		ADDR_OFFSET:	0x50
	 volatile uint32_t AHB2LPENR;	// AHB2 clock enable low power register		ADDR_OFFSET:	0x54
	 volatile uint32_t reserved8;	// reserved									ADDR_OFFSET:	0x58
	 volatile uint32_t reserved9;	// reserved									ADDR_OFFSET:	0x5C
	 volatile uint32_t APB1LPENR;	// APB1 clock enable low power register		ADDR_OFFSET:	0x60
	 volatile uint32_t APB2LPENR;	// APB2 clock enable low power register		ADDR_OFFSET:	0x64
	 volatile uint32_t reserved10;	// reserved									ADDR_OFFSET:	0x68
	 volatile uint32_t reserved11;	// reserved									ADDR_OFFSET:	0x6C
	 volatile uint32_t BDCR;		// Backup domain control register			ADDR_OFFSET:	0x70
	 volatile uint32_t CSR;			// Clock control & status register			ADDR_OFFSET:	0x74
	 volatile uint32_t reserved12;	// reserved									ADDR_OFFSET:	0x78
	 volatile uint32_t reserved13;	// reserved									ADDR_OFFSET:	0x7C
	 volatile uint32_t SSCGR;		// Spread Spectrum clock generation reg		ADDR_OFFSET:	0x80
	 volatile uint32_t PLLI2SCFGR;	// PLLI2S configuration register			ADDR_OFFSET:	0x84
	 volatile uint32_t reserved14;	// reserved									ADDR_OFFSET:	0x88
	 volatile uint32_t DCKCFGR;		// Dedicated clocks configuration reg		ADDR_OFFSET:	0x8C
 } RCC_RegDef_t;

 /*
  * Como se vio en la clase de introducción a las estructuras, hacemos un puntero a RCC_RegDef_t que
  * apunta a la posición exacta del periférico RCC, de forma que cada miembro de la estructura coincide
  * con cada uno de los SFR en la memoria del MCU. Esta acción la estamos haciendo en un MACRO, de forma
  * que el nuevo elemento "RCC" queda disponible para cada clase .c (archivo .c) que incluya este archivo
  * */
#define RCC			((RCC_RegDef_t *) RCC_BASE_ADDR)

 /* Descripción bit a bit de cada uno de los registros del que componen al periférico RCC
  * En este primer ejercicio muchos de ellos no son necesarios, se dejan sin implementar
  * NO son necesarios en este primer ejercicio:
  * 6.3.1 RCC_CR
  * 6.3.2 RCC_PLLCFGR
  * 6.3.3 RCC_CFGR
  * 6.3.4 RCC_CIR
  * 6.3.5 RCC_AHB1RSTR
  * 6.3.6 RCC_AHB2RSTR
  * 6.3.7 RCC_APB1RSTR
  * 6.3.8 RCC_APB2RSTR
	Este sí está implementado
6.3.10 RCC_AHB2ENR
6.3.11 RCC_APB1ENR
6.3.12 RCC_APB2ENR
6.3.17 RCC_BDCR
6.3.18 RCC_CSR
  */

/* 6.3.9 RCC_AHB1ENR */
#define RCC_AHB1ENR_GPIOA_EN		0
#define RCC_AHB1ENR_GPIOB_EN		1
#define RCC_AHB1ENR_GPIOC_EN		2
#define RCC_AHB1ENR_GPIOD_EN		3
#define RCC_AHB1ENR_GPIOE_EN		4
#define RCC_AHB1ENR_GPIOH_EN		7
#define RCC_AHB1ENR_CRCEN			12
#define RCC_AHB1ENR_DMA1_EN			21
#define RCC_AHB1ENR_DMA2_EN			22

/* ==== Fin de la descripción de los elementos que componen el periférico RCC ===== */

/* +++=== Inicio de la descripción de los elementos que componen el perfiérico GPIOx ====+++ */
/* Definición de la estructura de datos que representa a cada uno de los registros que componen el
periférico GPIO.
Debido a que el periférico GPIOx es muy simple, no es muy necesario crear la descripción bit a bit
de cada uno de los registros que componen dicho periférico, pero sí es necesario comprender qué
hace cada registro para poder cargar correctamente la configuración.
*/
typedef struct
{
	volatile uint32_t MODER;	// port mode register					ADDR_OFFSET: 0x00
	volatile uint32_t OTYPER;	// port output type register			ADDR_OFFSET: 0x04
	volatile uint32_t OSPEEDR;	// port output speed register			ADDR_OFFSET: 0x08
	volatile uint32_t PUPDR;	// port pull-up/down register			ADDR_OFFSET: 0x0C
	volatile uint32_t IDR;		// port input data register				ADDR_OFFSET: 0x10
	volatile uint32_t ODR;		// port output data register			ADDR_OFFSET: 0x14
	volatile uint32_t BSRR;		// port bit set/reset register			ADDR_OFFSET: 0x18
	volatile uint32_t LCKR;		// port configuration lock register		ADDR_OFFSET: 0x1C
	volatile uint32_t AFRL;		// alternate function low register		ADDR_OFFSET: 0x20
	volatile uint32_t AFRH;		// alternate function high register		ADDR_OFFSET: 0x24
} GPIOx_RegDef_t;

/* Al igual que con el RCC, creamos un puntero a la estructura que define GPIOx y debemos hacer
que cada GPIOx quede ubicado exactamente sobre la posición de memoria correcta.
Debido a que son varios periféricos GPIOx, es necesario hacer la definición para cada uno.

NOTA: Tener cuidado que cada elemento coincida con su respectiva dirección base.
* */
#define GPIOA		((GPIOx_RegDef_t *) GPIOA_BASE_ADDR)
#define GPIOB		((GPIOx_RegDef_t *) GPIOB_BASE_ADDR)
#define GPIOC		((GPIOx_RegDef_t *) GPIOC_BASE_ADDR)
#define GPIOD		((GPIOx_RegDef_t *) GPIOD_BASE_ADDR)
#define GPIOE		((GPIOx_RegDef_t *) GPIOE_BASE_ADDR)
#define GPIOH		((GPIOx_RegDef_t *) GPIOH_BASE_ADDR)

/* Valores estándar para las configuraciones */
/* 8.4.1 GPIOx_MODER (dos bit por cada PIN) */
#define GPIO_MODE_IN			0b00
#define GPIO_MODE_OUT			0b01
#define GPIO_MODE_ALTFN			0b10
#define GPIO_MODE_ANALOG		0b11

/* 8.4.2 GPIOx_OTYPER (un bit por PIN) */
#define GPIO_OTYPE_PUSHPULL		0b0
#define GPIO_OTYPE_OPENDRAIN	0b1

/* 8.4.3 GPIOx_OSPEEDR (dos bit por cada PIN) */
#define GPIO_OSPEED_LOW			0b00
#define GPIO_OSPEED_MEDIUM		0b01
#define GPIO_OSPEED_FAST		0b10
#define GPIO_OSPEED_HIGH		0b11

/* 8.4.4 GPIOx_PUPDR (dos bit por cada PIN) */
#define GPIO_PUPDR_NOTHING		0b00
#define GPIO_PUPDR_PULLUP		0b01
#define GPIO_PUPDR_PULLDOWN		0b10
#define GPIO_PUPDR_RESERVED		0b11

/* 8.4.5 GPIOx_IDR (un bit por PIN) - este es el registro para leer el estado de un PIN */

/* 8.4.6 GPIOx_ODR (un bit por PIN) - este es el registro para escribir el estado de un
PIN (1 o 0). Este registro puede ser escrito y leído desde el software, pero no garantiza
una escritura atómica, por lo cual es preferible utilizar el registro BSRR */

/* Definición de los nombres de los pines */
#define PIN_0		0
#define PIN_1		1
#define PIN_2		2
#define PIN_3		3
#define PIN_4		4
#define PIN_5		5
#define PIN_6		6
#define PIN_7		7
#define PIN_8		8
#define PIN_9		9
#define PIN_10		10
#define PIN_11		11
#define PIN_12		12
#define PIN_13		13
#define PIN_14		14
#define PIN_15		15

/* Definición de las funciones alternativas */
#define AF0		0b0000
#define AF1		0b0001
#define AF2		0b0010
#define AF3		0b0011
#define AF4		0b0100
#define AF5		0b0101
#define AF6		0b0110
#define AF7		0b0111
#define AF8		0b1000
#define AF9		0b1001
#define AF10	0b1010
#define AF11	0b1011
#define AF12	0b1100
#define AF13	0b1101
#define AF14	0b1110
#define AF15	0b1111

/*
typedef struct
{
	volatile uint32_t dummy;	// Dummy example register		ADDR_OFFSET:	0x00
} DUMMY_RegDef_t;
*/
/* No es necesario hacer nada más en este archivo para este primer proyecto */

#endif /* INC_STM32F411XX_HAL_H_ */
