/*
 * I2CxDriver.c
 *
 *  Created on: 18/05/2022
 *      Author: Juan Pablo
 */

#include "I2CxDriver.h"
#include "GPIOxDriver.h"

GPIO_Handler_t i2cPinSDA	= {0};

// Función de configuración para el I2C

void I2C_Config(I2C_Config_t *i2cConfig){

	/* 1. Activar la señal de reloj para el I2C y los PGIOs */

	if(i2cConfig -> i2cnumber == I2C_1){
		// Activación RCC para el I2C1
		RCC -> APB1RSTR |= RCC_APB1ENR_I2C1EN;
	}
	else if(i2cConfig -> i2cnumber == I2C_2){
		// Activación RCC para el I2C2
		RCC -> APB1RSTR |= RCC_APB1ENR_I2C2EN ;
	}
	else if(i2cConfig -> i2cnumber == I2C_3){
		// Activación RCC para el I2C3
		RCC -> APB1RSTR |= RCC_APB1ENR_I2C3EN;
	}

	/* 2. Limpiamos los registros del I2C */

	if(i2cConfig -> i2cnumber == I2C_1){
		// Limpiamos para I2C1
		I2C1 -> CR1 |= I2C_CR1_SWRST;
		I2C1 -> CR1 &= ~I2C_CR1_SWRST;

	}
	else if(i2cConfig -> i2cnumber == I2C_2){
		// Limpiamos para I2C2
		I2C2 -> CR1 |= I2C_CR1_SWRST;
		I2C2 -> CR1 &= ~I2C_CR1_SWRST;
	}
	else if(i2cConfig -> i2cnumber == I2C_3){
		// Limpiamos para I2C1
		I2C3 -> CR1 |= I2C_CR1_SWRST;
		I2C3 -> CR1 &= ~I2C_CR1_SWRST;
	}

	/* 3. Programación de la señal de reloj (peripheral input clock) para generar el timing correcto */
	// PLCK1 FRECUENCY in MHz

	if(i2cConfig -> i2cnumber == I2C_1){
		// Escogemos para el I2C1
		I2C1 -> CR2 |= (16 << 0);
	}else if(i2cConfig -> i2cnumber == I2C_2){
		// Escogemos para el I2C2
		I2C2 -> CR2 |= (i2cConfig -> frecuency);
	}
	else if(i2cConfig -> i2cnumber == I2C_3){
		// Escogemos para el I2C2
		I2C1 -> CR2 |= (i2cConfig -> frecuency);
	}

	/* 4. Configurar el Clock Control Register */

	if(i2cConfig -> i2cnumber == I2C_1){
		// Escogemos para el I2C1
		I2C1 -> CCR |= (80 << 0);
	}else if(i2cConfig -> i2cnumber == I2C_2){
		// Escogemos para el I2C2
		I2C1 -> CCR |= (80 << 0);
	}
	else if(i2cConfig -> i2cnumber == I2C_3){
		// Escogemos para el I2C2
		I2C1 -> CCR |= (80 << 0);
	}

	/* 5. Configurar el rising time register */

	if(i2cConfig -> i2cnumber == I2C_1){
		// Escogemos para el I2C1
		I2C1 -> TRISE = 17;
	}else if(i2cConfig -> i2cnumber == I2C_2){
		// Escogemos para el I2C2
		I2C1 -> CCR |= (80 << 0);
	}
	else if(i2cConfig -> i2cnumber == I2C_3){
		// Escogemos para el I2C2
		I2C1 -> CCR |= (80 << 0);
	}

	/* 6. Activar el I2C Peripheral */

	if(i2cConfig -> i2cnumber == I2C_1){
		// Escogemos para el I2C1
		I2C1 -> CR1 |= I2C_CR1_PE;
	}else if(i2cConfig -> i2cnumber == I2C_2){
		// Escogemos para el I2C2
		I2C1 -> CR1 |= I2C_CR1_PE;
	}
	else if(i2cConfig -> i2cnumber == I2C_3){
		// Escogemos para el I2C2
		I2C1 -> CR1 |= I2C_CR1_PE;
	}
}


void startI2C_1(void){

	/* 1. Empezar la condición de INICIO (START) */

	I2C1 -> CR1 |= I2C_CR1_START;
	while(!(I2C1 -> SR1 & (I2C_SR1_SB))){		// Se espera hasta que el SB Register sea puesto en SET
		__NOP();
	}
}

void writeI2C_1(uint8_t dataToSend){

	/* 1. Esperar al TXE (bit 7 in SR1) a set. Esto indica que el Data Register (DR) is empty */

	// Esperamos al TXE que se setee
	while(!(I2C1 -> SR1 & (I2C_SR1_TXE))){
		__NOP();
	}

	// Cargamos el dato
	I2C1 -> DR = dataToSend;

	// Esperamos a que el bit se mande
	while(!(I2C1 -> SR1 & (I2C_SR1_BTF))){
		__NOP();
	}
}

void addressI2C_1(uint8_t address){

	// Mandamos la dirección del Slave
	I2C1 -> DR = address;

	// Esperamos a que el ADDR cargue
	while(!(I2C1 -> SR1 & (I2C_SR1_ADDR))){
		__NOP();
	}
	// Leemos el SR1 y el SR2 para limpiar el ADDR bit
	//uint8_t temp = I2C1 -> SR1 | I2C1 -> SR2;
}
/*
movoid stopI2C_1(void){
	I2C1 -> CR1 |= I2C_CR1_STOP;
}*/

void writeMultiI2C_1(uint8_t *data,uint8_t size){
	/* 1. Esperar al TXE (bit 7 in SR1) a set. Esto indica que el Data Register (DR) is empty */

	// Esperamos al TXE que se setee
	while(!(I2C1 -> SR1 & (I2C_SR1_TXE))){
		__NOP();
	}

	while(size){
		while(!(I2C1 -> SR1 & (I2C_SR1_TXE))){
			__NOP();
		}
		I2C1 -> DR = (volatile uint32_t) * data++;	// Send data
		size --;
	}

	// Esperamos a que el bit se mande
	while(!(I2C1 -> SR1 & (I2C_SR1_BTF))){
		__NOP();
	}
}

// Configuración del GPIOx
void configI2CPin(I2C_Config_t *i2cConfig){

	// Escogemos el I2C_1
	if(i2cConfig -> i2cnumber == I2C_1){

		i2cPinSDA.pGPIOx								= GPIOB;
		i2cPinSDA.GPIO_PinConfig.GPIO_PinNumber			= i2cConfig -> pinNumber;
		i2cPinSDA.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
		i2cPinSDA.GPIO_PinConfig.GPIO_PinOPType			= GPIO_PUPDR_PULLUP;
		i2cPinSDA.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_OTYPE_OPENDRAIN;
		i2cPinSDA.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_HIGH;
		i2cPinSDA.GPIO_PinConfig.GPIO_PinAltFunMode		= AF4;

	}
	// Escogemos el I2C_2
	else if(i2cConfig -> i2cnumber == I2C_2){

		i2cPinSDA.pGPIOx								= GPIOB;
		i2cPinSDA.GPIO_PinConfig.GPIO_PinNumber			= i2cConfig -> pinNumber;
		i2cPinSDA.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
		i2cPinSDA.GPIO_PinConfig.GPIO_PinOPType			= GPIO_PUPDR_PULLUP;
		i2cPinSDA.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_OTYPE_OPENDRAIN;
		i2cPinSDA.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_HIGH;
		i2cPinSDA.GPIO_PinConfig.GPIO_PinAltFunMode		= AF4;
	}
	// Escogemos el I2C_3
	else if(i2cConfig -> i2cnumber == I2C_3){

		i2cPinSDA.pGPIOx								= GPIOC;
		i2cPinSDA.GPIO_PinConfig.GPIO_PinNumber			= i2cConfig -> pinNumber;
		i2cPinSDA.GPIO_PinConfig.GPIO_PinMode			= GPIO_MODE_OUT;
		i2cPinSDA.GPIO_PinConfig.GPIO_PinOPType			= GPIO_PUPDR_PULLUP;
		i2cPinSDA.GPIO_PinConfig.GPIO_PinPuPdControl	= GPIO_OTYPE_OPENDRAIN;
		i2cPinSDA.GPIO_PinConfig.GPIO_PinSpeed			= GPIO_OSPEED_HIGH;
		i2cPinSDA.GPIO_PinConfig.GPIO_PinAltFunMode		= AF4;
	}

	// Cargamos la configuración
	GPIO_Config(&i2cPinSDA);
}
