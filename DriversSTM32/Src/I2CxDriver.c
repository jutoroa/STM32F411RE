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
		RCC -> APB1ENR |= RCC_APB1ENR_I2C1EN;
	}
	else if(ptrHandlerI2C -> ptrI2Cx == I2C2){
		// Activación RCC para el I2C2
		RCC -> APB1ENR |= RCC_APB1ENR_I2C2EN ;
	}
	else if(ptrHandlerI2C -> ptrI2Cx == I2C3){
		// Activación RCC para el I2C3
		RCC -> APB1ENR |= RCC_APB1ENR_I2C3EN;
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

	startI2C(ptrHandlerI2C);

	sendSlaveAddressWriteI2C(ptrHandlerI2C);

	sendMemoryAddressI2C(ptrHandlerI2C,memAddr);

	reStartI2C(ptrHandlerI2C);

	sendSlaveAddressReadI2C(ptrHandlerI2C);

	nACKI2C(ptrHandlerI2C);

	stopI2C(ptrHandlerI2C);

	uint8_t dataI2C = recibeDataI2C(ptrHandlerI2C);

	return dataI2C;
}

void I2C_writeByte(I2C_Handler_t *ptrHandlerI2C, uint8_t memAddr, uint8_t dataToWrite){

	startI2C(ptrHandlerI2C);

	sendSlaveAddressWriteI2C(ptrHandlerI2C);

	sendMemoryAddressI2C(ptrHandlerI2C,memAddr);

	sendDataI2C(ptrHandlerI2C,dataToWrite);

	stopI2C(ptrHandlerI2C);
}

void startI2C(I2C_Handler_t *ptrHandlerI2C){
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

}

void sendSlaveAddressWriteI2C(I2C_Handler_t *ptrHandlerI2C){
	/* 0. Definimos una variable auxiliar */
		uint8_t auxByte = 0;
		(void) auxByte;	// Para no generar warning

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
}

void ACKReception(I2C_Handler_t *ptrHandlerI2C){
	/* 0. Definimos una variable auxiliar */
		uint8_t auxByte = 0;
		(void) auxByte;	// Para no generar warning

 /* 3.1 Esperemos hasta que la bandera del evento "addr" se levante
	  * (esto nos indica que la dirección fue enviada satisfactoriamente */
	 while(!(ptrHandlerI2C -> ptrI2Cx -> SR1 & I2C_SR1_ADDR)){
		__NOP();
	 }

	 /* 3.2 Debemos limpiar la bandera de la recepción de ACK de la "addr", para lo cual
	  * debemos leer en secuencia primero el I2C_SR1 y luego I2C_SR2 */
	 auxByte = ptrHandlerI2C -> ptrI2Cx -> SR1;
	 auxByte = ptrHandlerI2C -> ptrI2Cx -> SR2;
}

void sendMemoryAddressI2C(I2C_Handler_t *ptrHandlerI2C, uint8_t memAddr){
	 /* 4. Enviamos la dirección de memoria que deseamos leer */
	 ptrHandlerI2C -> ptrI2Cx -> DR = memAddr;

	 /* 4.1 Esperamos hasta que el byte sea transmitido */
	 while(!(ptrHandlerI2C -> ptrI2Cx -> SR1 & I2C_SR1_TXE)){
		 __NOP();
	 }
}

void reStartI2C(I2C_Handler_t *ptrHandlerI2C){
	 /* 5. Debemos generar una señal de RESTART, o sea, enviar un nuevo START */
	 ptrHandlerI2C -> ptrI2Cx -> CR1 |= I2C_CR1_START;

	 /* 5.1 Esperamos a que la bandera del evento "start" se levante */
	 /* Mientras esperamos, el valor de SB es 0, entonces la negación (!) es 1*/
	 while(!(ptrHandlerI2C -> ptrI2Cx -> SR1 & I2C_SR1_SB)){
		 __NOP();
	 }
}

void sendSlaveAddressReadI2C(I2C_Handler_t *ptrHandlerI2C){
	/* 0. Definimos una variable auxiliar */
		uint8_t auxByte = 0;
		(void) auxByte;	// Para no generar warning

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
}

void nACKI2C(I2C_Handler_t *ptrHandlerI2C){
	 /* 7. Activamos la indicación para no ACK (indicación para el Slave de terminar)
	  * (Debemos escribir cero en la posición ACK del registro de control 1)
	  */
	 ptrHandlerI2C -> ptrI2Cx -> CR1 &= ~I2C_CR1_ACK;
}

void stopI2C(I2C_Handler_t *ptrHandlerI2C){
	 /* 8. Genereamos la condición de stop */
	 ptrHandlerI2C -> ptrI2Cx -> CR1 |= I2C_CR1_STOP;
}

uint8_t recibeDataI2C(I2C_Handler_t *ptrHandlerI2C){
	 /* 9. Esperamos hasta que el byte entrante sea recibido */
	 while(!(ptrHandlerI2C -> ptrI2Cx -> SR1 & I2C_SR1_RXNE)){
		 __NOP();
	 }

	 ptrHandlerI2C -> dataI2C = ptrHandlerI2C -> ptrI2Cx -> DR;

	 return ptrHandlerI2C -> dataI2C;
}

void sendDataI2C(I2C_Handler_t *ptrHandlerI2C, uint8_t dataToWrite){
	 /* 5. Cargamos el valor que deseamos escribir */

	 ptrHandlerI2C -> ptrI2Cx -> DR = dataToWrite;

	 /* 6. Esperamos hasta que el bit sea transmitido */

	 while(!(ptrHandlerI2C -> ptrI2Cx -> SR1 & I2C_SR1_BTF)){
			 __NOP();
		 }
}



