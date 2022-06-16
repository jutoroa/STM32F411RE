/*
 * OLEDDriver.c
 *
 *  Created on: Jun 5, 2022
 *      Author: jutoroa
 */

#include "OLEDDriver.h"

void OLED_CommandByte(I2C_Handler_t *ptrHandlerI2C, uint8_t dataToWrite){

	startI2C(ptrHandlerI2C);

	sendSlaveAddressWriteI2C(ptrHandlerI2C);

	// Se envía el control byte que dice que se enviará solamente un byte
	uint8_t memAddr = OLED_CONTROL_COMMAND_BYTE;

	sendMemoryAddressI2C(ptrHandlerI2C,memAddr);

	// Se envía el comando
	sendDataI2C(ptrHandlerI2C,dataToWrite);

	stopI2C(ptrHandlerI2C);
}

void OLED_DataByte(I2C_Handler_t *ptrHandlerI2C, char *dataToWrite){

	startI2C(ptrHandlerI2C);

	sendSlaveAddressWriteI2C(ptrHandlerI2C);

	// Se envía el control byte que dice que se enviará solamente un byte
	uint8_t memAddr = OLED_CONTROL_DATA_BYTE;

	sendMemoryAddressI2C(ptrHandlerI2C,memAddr);

	// Se envía el comando
	sendDataI2C(ptrHandlerI2C,*dataToWrite);		// Byte 1

	dataToWrite++;

	sendDataI2C(ptrHandlerI2C,*dataToWrite);		// Byte 2

	dataToWrite++;

	sendDataI2C(ptrHandlerI2C,*dataToWrite);		// Byte 3

	dataToWrite++;

	sendDataI2C(ptrHandlerI2C,*dataToWrite);		// Byte 4

	dataToWrite++;

	sendDataI2C(ptrHandlerI2C,*dataToWrite);		// Byte 5

	dataToWrite++;

	sendDataI2C(ptrHandlerI2C,*dataToWrite);		// Byte 6

	dataToWrite++;

	sendDataI2C(ptrHandlerI2C,*dataToWrite);		// Byte 7

	dataToWrite++;

	sendDataI2C(ptrHandlerI2C,*dataToWrite);		// Byte 8

	stopI2C(ptrHandlerI2C);
}

// Configuración para los pixeles por caracter

char charOledNull[8]  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
char charOledA[8] = {0x00, 0b11111100, 0b00010010, 0b00010010, 0b00010010, 0b11111100, 0x00, 0x00};// Escribir la letra A
char charOledB[8] = {0x00, 0b11111110, 0b10010010, 0b10010010, 0b10010010, 0b01101100, 0x00, 0x00};// Escribir la letra B
char charOledC[8] = {0x00, 0b01111100, 0b10000010, 0b10000010, 0b10000010, 0b01000100, 0x00, 0x00};// Escribir la letra C
char charOledD[8] = {0x00, 0b11111110, 0b10000010, 0b10000010, 0b10000010, 0b01111100, 0x00, 0x00};// Escribir la letra D
char charOledE[8] = {0x00, 0b11111110, 0b10010010, 0b10010010, 0b10010010, 0b10000010, 0x00, 0x00};// Escribir la letra E
char charOledF[8] = {0x00, 0b11111110, 0b00010010, 0b00010010, 0b00010010, 0b00000010, 0x00, 0x00};// Escribir la letra F
char charOledG[8] = {0x00, 0b01111100, 0b10000010, 0b10000010, 0b10100010, 0b01100100, 0x00, 0x00};// Escribir la letra G
char charOledH[8] = {0x00, 0b11111110, 0b00010000, 0b00010000, 0b00010000, 0b11111110, 0x00, 0x00};// Escribir la letra H
char charOledI[8] = {0x00, 0b10000010, 0b10000010, 0b11111110, 0b10000010, 0b10000010, 0x00, 0x00};// Escribir la letra I
char charOledJ[8] = {0x00, 0b01100000, 0b10000000, 0b10000000, 0b10000000, 0b01111110, 0x00, 0x00};// Escribir la letra J
char charOledK[8] = {0x00, 0b11111110, 0b00010000, 0b00101000, 0b01000100, 0b10000010, 0x00, 0x00};// Escribir la letra K
char charOledL[8] = {0x00, 0b11111110, 0b10000000, 0b10000000, 0b10000000, 0b00000000, 0x00, 0x00};// Escribir la letra L
char charOledM[8] = {0x00, 0b11111110, 0b00000100, 0b00001000, 0b00000100, 0b11111110, 0x00, 0x00};// Escribir la letra M
char charOledN[8] = {0x00, 0b11111110, 0b00001000, 0b00010000, 0b00100000, 0b11111110, 0x00, 0x00};// Escribir la letra N
char charOledO[8] = {0x00, 0b01111100, 0b10000010, 0b10000010, 0b10000010, 0b01111100, 0x00, 0x00};// Escribir la letra O
char charOledP[8] = {0x00, 0b11111110, 0b00010010, 0b00010010, 0b00010010, 0b00001100, 0x00, 0x00};// Escribir la letra P
char charOledQ[8] = {0x00, 0b01111100, 0b10000010, 0b10100010, 0b01000010, 0b10111100, 0x00, 0x00};// Escribir la letra Q
char charOledR[8] = {0x00, 0b11111110, 0b00010010, 0b00010010, 0b00010010, 0b11101100, 0x00, 0x00};// Escribir la letra R
char charOledS[8] = {0x00, 0b01001100, 0b10010010, 0b10010010, 0b10010010, 0b01100100, 0x00, 0x00};// Escribir la letra S
char charOledT[8] = {0x00, 0b00000010, 0b00000010, 0b11111110, 0b00000010, 0b00000010, 0x00, 0x00};// Escribir la letra T
char charOledU[8] = {0x00, 0b01111110, 0b10000000, 0b10000000, 0b10000000, 0b01111110, 0x00, 0x00};// Escribir la letra U
char charOledV[8] = {0x00, 0b00111110, 0b01000000, 0b10000000, 0b01000000, 0b00111110, 0x00, 0x00};// Escribir la letra V
char charOledW[8] = {0x00, 0b01111110, 0b10000000, 0b01100000, 0b10000000, 0b01111110, 0x00, 0x00};// Escribir la letra W
char charOledX[8] = {0x00, 0b11000110, 0b00101000, 0b00010000, 0b00101000, 0b11000110, 0x00, 0x00};// Escribir la letra X
char charOledY[8] = {0x00, 0b00000110, 0b00001000, 0b11110000, 0b00001000, 0b00000110, 0x00, 0x00};// Escribir la letra Y
char charOledZ[8] = {0x00, 0b11000010, 0b10100010, 0b10010010, 0b10001010, 0b10000110, 0x00, 0x00};// Escribir la letra Z
char charOled1[8] = {0x00, 0b00000000, 0b10000100, 0b11111110, 0b10000000, 0b00000000, 0x00, 0x00};					// 1/
char charOled2[8] = {0x00, 0b11000100, 0b10100010, 0b10010010, 0b10010010, 0b10001100, 0x00, 0x00};					// 2
char charOled3[8] = {0x00, 0b01000100, 0b10000010, 0b10010010, 0b10010010, 0b01101100, 0x00, 0x00};					// 3
char charOled4[8] = {0x00, 0b00110000, 0b00101000, 0b00100100, 0b00100010, 0b11111110, 0x00, 0x00};					// 4
char charOled5[8] = {0x00, 0b01001110, 0b10001010, 0b10001010, 0b10001010, 0b01110010, 0x00, 0x00};					// 5
char charOled6[8] = {0x00, 0b01111100, 0b10010010, 0b10010010, 0b10010010, 0b01100100, 0x00, 0x00};					// 6
char charOled7[8] = {0x00, 0b00000010, 0b11100010, 0b00010010, 0b00001010, 0b00000110, 0x00, 0x00};					// 7
char charOled8[8] = {0x00, 0b01101100, 0b10010010, 0b10010010, 0b10010010, 0b01101100, 0x00, 0x00};					// 8
char charOled9[8] = {0x00, 0b01001100, 0b10010010, 0b10010010, 0b10010010, 0b01111100, 0x00, 0x00};
char charOled0[8] = {0x00, 0b01111100, 0b10100010, 0b10010010, 0b10001010, 0b01111100, 0x00, 0x00};
char charOledPlus[8] 			= {0x00, 0x00, 0b00010000, 0b00111000, 0b00010000, 0x00, 0x00, 0x00};					// +
char charOledSus[8] 			= {0x00, 0x00, 0b00010000, 0b00010000, 0b00010000, 0x00, 0x00, 0x00};					// -
char charOledExclamation[8] 	= {0x00, 0x00, 0x00, 0b1011110, 0x00, 0x00, 0x00, 0x00};								// !
char charOledHeart[8] 			= {0x00, 0b00011000, 0b00111100, 0b01111000, 0b00111100, 0b00011000, 0x00, 0x00};		// <3
char charOledCube[8] 			= {0x00, 0b11111110, 0b11111110, 0b11111110, 0b11111110, 0b11111110, 0x00, 0x00};		// Cubo

char charOledPERIOD[8] 			= {0x00, 0x00, 0x00, 0x00, 0x00, 0b10000000, 0x00, 0x00};										// Punto
char charOledCOMMA[8] 			= {0x00, 0x00, 0x00, 0x00, 0b10000000, 0b01000000, 0x00, 0x00};									// Coma
char charOledCOLON[8] 			= {0x00, 0x00, 0x00, 0x00, 0b01001000, 0x00, 0x00, 0x00};										// Dos puntos
char charOledQUEST_MARK[8] 		= {0x00, 0b00000100, 0b00000010, 0b10100010, 0b00010010, 0b00001100, 0x00, 0x00};				// Signo de interrogación
char charOledSLASH[8]		 	= {0x00, 0b01000000, 0b00100000, 0b00010000, 0b00001000, 0b00000100, 0b00000010, 0x00};			// Signo slash
char charOledUNDERSCORE[8]		= {0x00, 0x00, 0b10000000, 0b10000000, 0b10000000, 0b10000000, 0x00, 0x00};						// Guion bajo
char charOledQUOTE[8]		 	= {0x00, 0x00, 0x00, 0b00000110, 0x00, 0b00000110, 0x00, 0x00};									// Comillas
char charOledMULTIPLICATION[8] 	= {0x00, 0x00, 0b00101000, 0b00010000, 0b00101000, 0x00, 0x00, 0x00};							// Escribir signo de multiplicación
char charOledDIVISION[8] 		= {0x00, 0b00010000, 0b00010000, 0b01010100, 0b00010000, 0b00010000, 0x00, 0x00};
char charOledEQUAL[8] 			= {0x00, 0x00, 0b00101000, 0b00101000, 0b00101000, 0x00, 0x00, 0x00};							// Escribir signo igual



/* Funciones para devolver los punteros a cada uno de los arreglos */

char* OLEDNull(void){return charOledNull;}
char* OLED_A(void){return charOledA;}
char* OLED_B(void){return charOledB;}
char* OLED_C(void){return charOledC;}
char* OLED_D(void){return charOledD;}
char* OLED_E(void){return charOledE;}
char* OLED_F(void){return charOledF;}
char* OLED_G(void){return charOledG;}
char* OLED_H(void){return charOledH;}
char* OLED_I(void){return charOledI;}
char* OLED_J(void){return charOledJ;}
char* OLED_K(void){return charOledK;}
char* OLED_L(void){return charOledL;}
char* OLED_M(void){return charOledM;}
char* OLED_N(void){return charOledN;}
char* OLED_O(void){return charOledO;}
char* OLED_P(void){return charOledP;}
char* OLED_Q(void){return charOledQ;}
char* OLED_R(void){return charOledR;}
char* OLED_S(void){return charOledS;}
char* OLED_T(void){return charOledT;}
char* OLED_U(void){return charOledU;}
char* OLED_V(void){return charOledV;}
char* OLED_W(void){return charOledW;}
char* OLED_X(void){return charOledX;}
char* OLED_Y(void){return charOledY;}
char* OLED_Z(void){return charOledZ;}
char* OLED_1(void){return charOled1;}
char* OLED_2(void){return charOled2;}
char* OLED_3(void){return charOled3;}
char* OLED_4(void){return charOled4;}
char* OLED_5(void){return charOled5;}
char* OLED_6(void){return charOled6;}
char* OLED_7(void){return charOled7;}
char* OLED_8(void){return charOled8;}
char* OLED_9(void){return charOled9;}
char* OLED_0(void){return charOled0;}
char* OLED_PLUS(void){return charOledPlus;}
char* OLED_SUS(void){return charOledSus;}
char* OLED_EXC(void){return charOledExclamation;}
char* OLED_HEART(void){return charOledHeart;}
char* OLED_CUBE(void){return charOledCube;}

char* OLED_PERIOD(void){return charOledPERIOD;}
char* OLED_COMMA(void){return charOledCOMMA;}
char* OLED_COLON(void){return charOledCOLON;}
char* OLED_QUEST(void){return charOledQUEST_MARK;}
char* OLED_SLASH(void){return charOledSLASH;}
char* OLED_UNDERSCORE(void){return charOledUNDERSCORE;}
char* OLED_QUOTE(void){return charOledQUOTE;}
char* OLED_MULTIPLICATION(void){return charOledMULTIPLICATION;}
char* OLED_DIVISION(void){return charOledDIVISION;}
char* OLED_EQUAL(void){return charOledEQUAL;}

/* Función para inicializar la pantalla OLED */
void OLED_Init(I2C_Handler_t *ptrHandlerI2C){

	OLED_CommandByte(ptrHandlerI2C, 0xAE);
	OLED_CommandByte(ptrHandlerI2C, 0xD5);
	OLED_CommandByte(ptrHandlerI2C, 0x80);
	OLED_CommandByte(ptrHandlerI2C, 0xA8);
	OLED_CommandByte(ptrHandlerI2C, 0x3F);
	OLED_CommandByte(ptrHandlerI2C, 0xD3);
	OLED_CommandByte(ptrHandlerI2C, 0x00);
	OLED_CommandByte(ptrHandlerI2C, 0x40);
	OLED_CommandByte(ptrHandlerI2C, 0xAD);
	OLED_CommandByte(ptrHandlerI2C, 0x8B);
	OLED_CommandByte(ptrHandlerI2C, 0xA1);
	OLED_CommandByte(ptrHandlerI2C, 0xC8);
	OLED_CommandByte(ptrHandlerI2C, 0xDA);
	OLED_CommandByte(ptrHandlerI2C, 0x12);
	OLED_CommandByte(ptrHandlerI2C, 0x81);
	OLED_CommandByte(ptrHandlerI2C, 0xCF);
	OLED_CommandByte(ptrHandlerI2C, 0xD9);
	OLED_CommandByte(ptrHandlerI2C, 0x22);
	OLED_CommandByte(ptrHandlerI2C, 0xDB);
	OLED_CommandByte(ptrHandlerI2C, 0x40);
	OLED_CommandByte(ptrHandlerI2C, 0x32);
	OLED_CommandByte(ptrHandlerI2C, 0xA6);
	OLED_CommandByte(ptrHandlerI2C, 0xAF);
	//delay(100);
	OLED_CommandByte(ptrHandlerI2C, 0x00);
	OLED_CommandByte(ptrHandlerI2C, 0x10);
	OLED_CommandByte(ptrHandlerI2C, 0x40);
}

/* Función para limpiar la pantalla */
void OLED_Clean(I2C_Handler_t *ptrHandlerI2C){
	char* pageClean[16] = {OLEDNull(),OLEDNull(),OLEDNull(),OLEDNull(),OLEDNull(),OLEDNull(),OLEDNull(),OLEDNull(),
			OLEDNull(),OLEDNull(),OLEDNull(),OLEDNull(),OLEDNull(),OLEDNull(),OLEDNull(),OLEDNull()};
	for(uint8_t j = 0; j < 8; j++){
		OLED_SETPage(ptrHandlerI2C, pageClean, j);
	}
}

// Función que escribe en una página, los elementos de un arreglo
void OLED_SETPage(I2C_Handler_t *ptrHandlerI2C, char* *dataPage, uint8_t numberPage){

	switch(numberPage){
	case(PAGE_0):
	{OLED_CommandByte(ptrHandlerI2C, 0xB0);
	 break;}
	case(PAGE_1):
	{OLED_CommandByte(ptrHandlerI2C, 0xB1);
	break;}
	case(PAGE_2):
	{OLED_CommandByte(ptrHandlerI2C, 0xB2);
	break;}
	case(PAGE_3):
	{OLED_CommandByte(ptrHandlerI2C, 0xB3);
	break;}
	case(PAGE_4):
	{OLED_CommandByte(ptrHandlerI2C, 0xB4);
	break;}
	case(PAGE_5):
	{OLED_CommandByte(ptrHandlerI2C, 0xB5);
	break;}
	case(PAGE_6):
	{OLED_CommandByte(ptrHandlerI2C, 0xB6);
	break;}
	case(PAGE_7):
	{OLED_CommandByte(ptrHandlerI2C, 0xB7);
	break;}
	default:
	{OLED_CommandByte(ptrHandlerI2C, 0xB0);
	break;}
	}
	OLED_CommandByte(ptrHandlerI2C, 0x02);
	OLED_CommandByte(ptrHandlerI2C, 0x10);

	for(uint16_t j = 0; j < 16; j++){
		OLED_DataByte(ptrHandlerI2C, *dataPage);
		dataPage++;
	}
}

// Función que convierte de ascci a la configuración para cada caracter
char* OLED_VarToChar(char character){
	switch(character){
	case(' '):
	{return OLEDNull();
	break;}
	case('A'):
	{return OLED_A();
	break;}
	case('B'):
	{return OLED_B();
	break;}
	case('C'):
	{return OLED_C();
	break;}
	case('D'):
	{return OLED_D();
	break;}
	case('E'):
	{return OLED_E();
	break;}
	case('F'):
	{return OLED_F();
	break;}
	case('G'):
	{return OLED_G();
	break;}
	case('H'):
	{return OLED_H();
	break;}
	case('I'):
	{return OLED_I();
	break;}
	case('J'):
	{return OLED_J();
	break;}
	case('K'):
	{return OLED_K();
	break;}
	case('L'):
	{return OLED_L();
	break;}
	case('M'):
	{return OLED_M();
	break;}
	case('N'):
	{return OLED_N();
	break;}
	case('O'):
	{return OLED_O();
	break;}
	case('P'):
	{return OLED_P();
	break;}
	case('Q'):
	{return OLED_Q();
	break;}
	case('R'):
	{return OLED_R();
	break;}
	case('S'):
	{return OLED_S();
	break;}
	case('T'):
	{return OLED_T();
	break;}
	case('U'):
	{return OLED_U();
	break;}
	case('V'):
	{return OLED_V();
	break;}
	case('W'):
	{return OLED_W();
	break;}
	case('X'):
	{return OLED_X();
	break;}
	case('Y'):
	{return OLED_Y();
	break;}
	case('Z'):
	{return OLED_Z();
	break;}
	case('1'):
	{return OLED_1();
	break;}
	case('2'):
	{return OLED_2();
	break;}
	case('3'):
	{return OLED_3();
	break;}
	case('4'):
	{return OLED_4();
	break;}
	case('5'):
	{return OLED_5();
	break;}
	case('6'):
	{return OLED_6();
	break;}
	case('7'):
	{return OLED_7();
	break;}
	case('8'):
	{return OLED_8();
	break;}
	case('9'):
	{return OLED_9();
	break;}
	case('0'):
	{return OLED_0();
	break;}
	case('+'):
	{return OLED_PLUS();
	break;}
	case('-'):
	{return OLED_SUS();
	break;}
	case('!'):
	{return OLED_EXC();
	break;}
	case('<'):
	{return OLED_HEART();
	break;}
	case(';'):
	{return OLED_CUBE();
	break;}
	case('.'):
	{return OLED_PERIOD();
	break;}
	case(','):
	{return OLED_COMMA();
	break;}
	case(':'):
	{return OLED_COLON();
	break;}
	case('?'):
	{return OLED_QUEST();
	break;}
	case('/'):
	{return OLED_SLASH();
	break;}
	case('_'):
	{return OLED_UNDERSCORE();
	break;}
	case('~'):
	{return OLED_QUOTE();
	break;}
	case('*'):
	{return OLED_MULTIPLICATION();
	break;}
	case('%'):
	{return OLED_DIVISION();
	break;}
	case('='):
	{return OLED_EQUAL();
	break;}
	default:
	{return OLEDNull();
	break;}
	}
}

// Función para imprimir un string en una página en específico
void OLED_FPrintPage(I2C_Handler_t *ptrHandlerI2C, char *ptrVarToPrint, uint8_t numberPage){
	switch(numberPage){
		case(PAGE_0):
		{OLED_CommandByte(ptrHandlerI2C, 0xB0);
		 break;}
		case(PAGE_1):
		{OLED_CommandByte(ptrHandlerI2C, 0xB1);
		break;}
		case(PAGE_2):
		{OLED_CommandByte(ptrHandlerI2C, 0xB2);
		break;}
		case(PAGE_3):
		{OLED_CommandByte(ptrHandlerI2C, 0xB3);
		break;}
		case(PAGE_4):
		{OLED_CommandByte(ptrHandlerI2C, 0xB4);
		break;}
		case(PAGE_5):
		{OLED_CommandByte(ptrHandlerI2C, 0xB5);
		break;}
		case(PAGE_6):
		{OLED_CommandByte(ptrHandlerI2C, 0xB6);
		break;}
		case(PAGE_7):
		{OLED_CommandByte(ptrHandlerI2C, 0xB7);
		break;}
		default:
		{OLED_CommandByte(ptrHandlerI2C, 0xB0);
		break;}
		}
		OLED_CommandByte(ptrHandlerI2C, 0x02);
		OLED_CommandByte(ptrHandlerI2C, 0x10);

		for(uint8_t j = 0; j < 16; j++){
			OLED_DataByte(ptrHandlerI2C, OLED_VarToChar(*ptrVarToPrint));
			ptrVarToPrint++;
		}
}

// Función para imprimir un string en toda la pantalla
void OLED_FPrint(I2C_Handler_t *ptrHandlerI2C, char *ptrVarToPrint){
	char* pageClean[16] = {OLEDNull(),OLEDNull(),OLEDNull(),OLEDNull(),OLEDNull(),OLEDNull(),OLEDNull(),OLEDNull(),
				OLEDNull(),OLEDNull(),OLEDNull(),OLEDNull(),OLEDNull(),OLEDNull(),OLEDNull(),OLEDNull()};

	OLED_SETPage(ptrHandlerI2C, pageClean, PAGE_0);
//	for(uint8_t j = 0; j < 16; j++){
//		ptrVarToPrint++;
//	}
	OLED_CommandByte(ptrHandlerI2C, 0xB1);
	OLED_CommandByte(ptrHandlerI2C, 0x02);
	OLED_CommandByte(ptrHandlerI2C, 0x10);
	for(uint8_t j = 16; j < 32; j++){
		OLED_DataByte(ptrHandlerI2C, OLED_VarToChar(*ptrVarToPrint));
		ptrVarToPrint++;
	}
	OLED_SETPage(ptrHandlerI2C, pageClean, PAGE_2);
//	for(uint8_t j = 32; j < 48; j++){
//
//	}
	OLED_CommandByte(ptrHandlerI2C, 0xB3);
	OLED_CommandByte(ptrHandlerI2C, 0x02);
	OLED_CommandByte(ptrHandlerI2C, 0x10);
	for(uint8_t j = 48 ; j < 64; j++){
		OLED_DataByte(ptrHandlerI2C, OLED_VarToChar(*ptrVarToPrint));
		ptrVarToPrint++;
	}
	OLED_SETPage(ptrHandlerI2C, pageClean, PAGE_4);
//		for(uint8_t j = 64; j < 80; j++){
//			ptrVarToPrint++;
//		}
	OLED_CommandByte(ptrHandlerI2C, 0xB5);
	OLED_CommandByte(ptrHandlerI2C, 0x02);
	OLED_CommandByte(ptrHandlerI2C, 0x10);
	for(uint8_t j = 80 ; j < 96; j++){
		OLED_DataByte(ptrHandlerI2C, OLED_VarToChar(*ptrVarToPrint));
		ptrVarToPrint++;
	}
	OLED_SETPage(ptrHandlerI2C, pageClean, PAGE_6);
//		for(uint8_t j = 96; j < 112; j++){
//			ptrVarToPrint++;
//		}
	OLED_CommandByte(ptrHandlerI2C, 0xB7);
	OLED_CommandByte(ptrHandlerI2C, 0x02);
	OLED_CommandByte(ptrHandlerI2C, 0x10);
	for(uint8_t j = 112 ; j < 128; j++){
		OLED_DataByte(ptrHandlerI2C, OLED_VarToChar(*ptrVarToPrint));
		ptrVarToPrint++;
	}

}
