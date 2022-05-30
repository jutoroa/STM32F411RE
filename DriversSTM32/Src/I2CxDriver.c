/*
 * I2CxDriver.c
 *
 *  Created on: 18/05/2022
 *      Author: Juan Pablo
 */

#include "I2CxDriver.h"
#include "GPIOxDriver.h"

GPIO_Handler_t i2cPinSDA	= {0};

/*
 * Recordar que se debe configurar los pines para el I2C (SDA y SCL),
 * para lo cual se necesita el modulo GPIO y los pines configurados en el modo Alternate Function.
 * Además, estos pines deben ser configurados como salidas open-drain
 * y con la resistencia en modo pull - up.
 * */

// Función de configuración para el I2C

void I2C_Config(I2C_Handler_t *ptrHandlerI2C){

	/* 1. Activar la señal de reloj para el I2C y los PGIOs */

	if(ptrHandlerI2C -> ptrI2Cx == I2C1){
		// Activación RCC para el I2C1
		RCC -> APB1RSTR |= RCC_APB1ENR_I2C1EN;
	}
	else if(ptrHandlerI2C -> ptrI2Cx == I2C2){
		// Activación RCC para el I2C2
		RCC -> APB1RSTR |= RCC_APB1ENR_I2C2EN ;
	}
	else if(ptrHandlerI2C -> ptrI2Cx == I2C3){
		// Activación RCC para el I2C3
		RCC -> APB1RSTR |= RCC_APB1ENR_I2C3EN;
	}

	/* 2. Limpiamos los registros del I2C. Reiniciamos el periferico
	 *  de forma que inicia en un estado conocido */

	ptrHandlerI2C -> ptrI2Cx -> CR1 |= I2C_CR1_SWRST;
	__NOP();
	ptrHandlerI2C -> ptrI2Cx -> CR1 &= ~I2C_CR1_SWRST;

	/* 3. Programación de la señal de reloj (peripheral input clock) para generar el timing correcto
	 * Indicamos cual es la velocidad del reloj principal, que es la señal utilizada por el periférico
	 * para generar la señal de reloj para el bus I2C
	 * */
	// PLCK1 FRECUENCY in MHz

	ptrHandlerI2C -> ptrI2Cx -> CR2 &= ~(0b111111 << I2C_CR2_FREQ_Pos);	// Borramos la configuración actual
	ptrHandlerI2C -> ptrI2Cx -> CR2 |= (MAIN_CLOCK_16_MHz_I2C << I2C_CR2_FREQ_Pos);

	/* 4. Configurar el Clock Control Register
	 * Configuramos el modo I2C en el que el sistema funciona
	 * En esta configuración también se incluye la velocidad del reloj
	 * y el tiempo máximo para el cambio de la señal (T-Rise).
	 * Todo comienza con los dos registros en 0
	 * */

	ptrHandlerI2C -> ptrI2Cx -> CCR = 0;
	ptrHandlerI2C -> ptrI2Cx -> TRISE = 0;

	if(ptrHandlerI2C -> modeI2C == I2C_MODE_SM){

		// Estamos en modo "standar" (SM Mode)
		// Seleccionamos el modo estandar
		ptrHandlerI2C -> ptrI2Cx -> CCR &= ~I2C_CCR_FS;

		// Configuramos el registro que se encarga de generar la señal del reloj
		ptrHandlerI2C -> ptrI2Cx -> CCR |= (I2C_MODE_SM_SPEED_100KHz << I2C_CCR_CCR_Pos);

		// Configuramos el registro que controla el tiempo T - Rise máximo
		ptrHandlerI2C -> ptrI2Cx -> TRISE |= I2C_MAX_RISE_TIME_SM;
	}
	else{
		// Estamos en modo "Fast" (FM Mode)
		// Seleccionamos el modo Fast
		ptrHandlerI2C -> ptrI2Cx -> CCR |= I2C_CCR_FS;

		// COnfiguramos el registro que se encarga de generar la señal del reloj
		ptrHandlerI2C -> ptrI2Cx -> CCR |= (I2C_MODE_FM_SPEED_400Khz << I2C_CCR_CCR_Pos);

		// Configuramos el registro que controla el tiempo T - Rise máximo
		ptrHandlerI2C -> ptrI2Cx -> TRISE |= I2C_MAX_RISE_TIME_FM;
	}

	/* 5. Activamos el I2C Peripheral (Módulo I2C).*/

	ptrHandlerI2C -> ptrI2Cx -> CR1 |= I2C_CR1_PE;
}

uint8_t I2C_readByte(I2C_Handler_t *ptrHandlerI2C, uint8_t memAddr){

	/* 0. Definimos una variable auxiliar */
	uint8_t auxByte = 0;
	(void) auxByte;	// Para no generar warning

	/* 1. Verificamos que la línea no está ocupada - bit "busy" en I2C_CR2 */
	while(ptrHandlerI2C -> ptrI2Cx -> SR2 & I2C_SR2_BUSY){
		__NOP();
	}

	/* 2. Generamos la señal "start" */
	ptrHandlerI2C -> ptrI2Cx -> CR1 |= I2C_CR1_START;

	/* 2a. Esperamos a que la bandera del evento "Start" se levante */
	/* Mientras esperamos, el valor de SB es 0, entonces la negación (!) es 1 */
	while(!(ptrHandlerI2C -> ptrI2Cx -> SR1 & I2C_SR1_SB)){
		__NOP();
	}

	/* 3. Enviamos la dirección del Slave y el bit que indica que deseamos escribir (0)
	 * (en el siguiente paso se envía la dirección de memoria que se desea leer */

	ptrHandlerI2C -> ptrI2Cx -> DR = (ptrHandlerI2C -> slaveAddress << 1)	| I2C_WRITE_DATA;

	 /* 3.1 Esperemos hasta que la bandera del evento "addr" se levante
	  * (esto nos indica que la dirección fue enviada satisfactoriamente */
	 while(!(ptrHandlerI2C -> ptrI2Cx -> SR1 & I2C_SR1_ADDR)){
		__NOP();
	 }

	 /* 3.2 Debemos limpiar la bandera de la recepción de ACK de la "addr", para lo cual
	  * debemos leer en secuencia primero el I2C_SR1 y luego I2C_SR2 */
	 auxByte = ptrHandlerI2C -> ptrI2Cx -> SR1;
	 auxByte = ptrHandlerI2C -> ptrI2Cx -> SR2;

	 /* 4. Enviamos la dirección de memoria que deseamos leer */
	 ptrHandlerI2C -> ptrI2Cx -> DR = memAddr;

	 /* 4.1 Esperamos hasta que el byte sea transmitido */
	 while(!(ptrHandlerI2C -> ptrI2Cx -> SR1 & I2C_SR1_TXE)){
		 __NOP();
	 }

	 /* 5. Debemos generar una señal de RESTART, o sea, enviar un nuevo START */
	 ptrHandlerI2C -> ptrI2Cx -> CR1 |= I2C_CR1_START;

	 /* 5.1 Esperamos a que la bandera del evento "start" se levante */
	 /* Mientras esperamos, el valor de SB es 0, entonces la negación (!) es 1*/
	 while(!(ptrHandlerI2C -> ptrI2Cx -> SR1 & I2C_SR1_SB)){
		 __NOP();
	 }

	 /* 6. Enviamos la dirección del Slave, pero ahora con la indicación de leer */
	 ptrHandlerI2C -> ptrI2Cx -> DR = (ptrHandlerI2C -> slaveAddress << 1) | I2C_READ_DATA;

	 /* 6.1 Esperamos hasat que la bandera del evento "addr" se levante
	  * (esto nos indica que la dirección fue enviada satisfactoriamente
	  */
	 while(!(ptrHandlerI2C -> ptrI2Cx -> SR1 & I2C_SR1_ADDR)){
		 __NOP();
	 }

	 /* 6.2 Debemos limpiar la bandera de la recepción de ACK de la "addr", para lo cual
	  * debemos leer en secuencia primero el I2C_SR1 y luego I2C_SR2 */

	 auxByte = ptrHandlerI2C -> ptrI2Cx -> SR1;
	 auxByte = ptrHandlerI2C -> ptrI2Cx -> SR2;

	 /* 7. Activamos la indicación para no ACK (indicación para el Slave de terminar)
	  * (Debemos escribir cero en la posición ACK del registro de control 1)
	  */
	 ptrHandlerI2C -> ptrI2Cx -> CR1 &= ~I2C_CR1_ACK;

	 /* 8. Genereamos la condición de stop */
	 ptrHandlerI2C -> ptrI2Cx -> CR1 |= I2C_CR1_STOP;

	 /* 9. Esperamos hasta que el byte entrante sea recibido */
	 while(!(ptrHandlerI2C -> ptrI2Cx -> SR1 & I2C_SR1_RXNE)){
		 __NOP();
	 }

	 ptrHandlerI2C -> dataI2C = ptrHandlerI2C -> ptrI2Cx -> DR;

	 return ptrHandlerI2C -> dataI2C;
}

void I2C_writeByte(I2C_Handler_t *ptrHandlerI2C, uint8_t memAddr, uint8_t dataToWrite){

	/* 0. Definimos una variable auxiliar */
	uint8_t auxByte = 0;
	(void) auxByte;

/* 1. Verificamos que la línea no está ocupada - bit "busy" en I2C_CR2 */
	while(ptrHandlerI2C -> ptrI2Cx -> SR2 & I2C_SR2_BUSY){
		__NOP();
	}

	/* 2. Generamos la señal "start" */
	ptrHandlerI2C -> ptrI2Cx -> CR1 |= I2C_CR1_START;

	/* 2a. Esperamos a que la bandera del evento "Start" se levante */
	/* Mientras esperamos, el valor de SB es 0, entonces la negación (!) es 1 */
	while(!(ptrHandlerI2C -> ptrI2Cx -> SR1 & I2C_SR1_SB)){
		__NOP();
	}

	/* 3. Enviamos la dirección del Slave y el bit que indica que deseamos escribir (0)
	 * (en el siguiente paso se envía la dirección de memoria que se desea leer */
	ptrHandlerI2C -> ptrI2Cx -> DR = (ptrHandlerI2C -> slaveAddress << 1)	| I2C_WRITE_DATA;

	 /* 3.1 Esperemos hasta que la bandera del evento "addr" se levante
	  * (esto nos indica que la dirección fue enviada satisfactoriamente */
	 while(!(ptrHandlerI2C -> ptrI2Cx -> SR1 & I2C_SR1_ADDR)){
		__NOP();
	 }

	 /* 3.2 Debemos limpiar la bandera de la recepción de ACK de la "addr", para lo cual
	  * debemos leer en secuencia primero el I2C_SR1 y luego I2C_SR2 */
	 auxByte = ptrHandlerI2C -> ptrI2Cx -> SR1;
	 auxByte = ptrHandlerI2C -> ptrI2Cx -> SR2;

	 /* 4. Enviamos la dirección de memoria que deseamos leer */
	 ptrHandlerI2C -> ptrI2Cx -> DR = memAddr;

	 /* 4.1 Esperamos hasta que el byte sea transmitido */
	 while(!(ptrHandlerI2C -> ptrI2Cx -> SR1 & I2C_SR1_TXE)){
		 __NOP();
	 }

	 /* 5. Crgamos el valor que deseamos escribir */

	 ptrHandlerI2C -> ptrI2Cx -> DR = dataToWrite;

	 /* 6. Esperamos hasta que el bit sea transmitido */

	 while(!(ptrHandlerI2C -> ptrI2Cx -> SR1 & I2C_SR1_BTF)){
	 		 __NOP();
	 	 }

	 /* 7. Generamos la condición de stop */
	 ptrHandlerI2C -> ptrI2Cx -> CR1 |= I2C_CR1_STOP;
}

//void startI2C_1(void){
//
//	/* 1. Empezar la condición de INICIO (START) */
//
//	I2C1 -> CR1 |= I2C_CR1_START;
//	while(!(I2C1 -> SR1 & (I2C_SR1_SB))){		// Se espera hasta que el SB Register sea puesto en SET
//		__NOP();
//	}
//}


//
//void writeI2C_1(uint8_t dataToSend){
//
//	/* 1. Esperar al TXE (bit 7 in SR1) a set. Esto indica que el Data Register (DR) is empty */
//
//	// Esperamos al TXE que se setee
//	while(!(I2C1 -> SR1 & (I2C_SR1_TXE))){
//		__NOP();
//	}
//
//	// Cargamos el dato
//	I2C1 -> DR = dataToSend;
//
//	// Esperamos a que el bit se mande
//	while(!(I2C1 -> SR1 & (I2C_SR1_BTF))){
//		__NOP();
//	}
//}
//
//void addressI2C_1(uint8_t address){
//
//	// Mandamos la dirección del Slave
//	I2C1 -> DR = address;
//
//	// Esperamos a que el ADDR cargue
//	while(!(I2C1 -> SR1 & (I2C_SR1_ADDR))){
//		__NOP();
//	}
//	// Leemos el SR1 y el SR2 para limpiar el ADDR bit
//	//uint8_t temp = I2C1 -> SR1 | I2C1 -> SR2;
//}
///*
//movoid stopI2C_1(void){
//	I2C1 -> CR1 |= I2C_CR1_STOP;
//}*/
//
//void writeMultiI2C_1(uint8_t *data,uint8_t size){
//	/* 1. Esperar al TXE (bit 7 in SR1) a set. Esto indica que el Data Register (DR) is empty */
//
//	// Esperamos al TXE que se setee
//	while(!(I2C1 -> SR1 & (I2C_SR1_TXE))){
//		__NOP();
//	}
//
//	while(size){
//		while(!(I2C1 -> SR1 & (I2C_SR1_TXE))){
//			__NOP();
//		}
//		I2C1 -> DR = (volatile uint32_t) * data++;	// Send data
//		size --;
//	}
//
//	// Esperamos a que el bit se mande
//	while(!(I2C1 -> SR1 & (I2C_SR1_BTF))){
//		__NOP();
//	}
//}
//*/

// Configuración del GPIOx
/*
void configI2CPin(I2C_Handler_t *ptrHandlerI2C){

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
*/
