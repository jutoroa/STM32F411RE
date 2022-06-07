/*
 * OLEDDriver.h
 *
 *  Created on: Jun 5, 2022
 *      Author: jutoroa
 */

#ifndef OLEDDRIVER_H_
#define OLEDDRIVER_H_

#include <stdbool.h>
#include "I2CxDriver.h"

/* Archivo que contendrá la configuración para controlar la pantalla OLED 1.3" con el controlador SH1106 */

#define OLED_CONTROL_COMMAND_BYTE 	0x00
#define OLED_CONTROL_DATA_BYTE 	  	0x40

char* OLEDNull(void);
char* OLED_A(void);
char* OLED_B(void);
char* OLED_C(void);
char* OLED_D(void);
char* OLED_E(void);
char* OLED_F(void);
char* OLED_G(void);
char* OLED_H(void);
char* OLED_I(void);
char* OLED_J(void);
char* OLED_K(void);
char* OLED_L(void);
char* OLED_M(void);
char* OLED_N(void);
char* OLED_O(void);
char* OLED_P(void);
char* OLED_Q(void);
char* OLED_R(void);
char* OLED_S(void);
char* OLED_T(void);
char* OLED_U(void);
char* OLED_V(void);
char* OLED_W(void);
char* OLED_X(void);
char* OLED_Y(void);
char* OLED_Z(void);
char* OLED_1(void);
char* OLED_2(void);
char* OLED_3(void);
char* OLED_4(void);
char* OLED_5(void);
char* OLED_6(void);
char* OLED_7(void);
char* OLED_8(void);
char* OLED_9(void);
char* OLED_0(void);
char* OLED_PLUS(void);
char* OLED_SUS(void);
char* OLED_EXC(void);
char* OLED_HEART(void);

#define PAGE_0 0
#define PAGE_1 1
#define PAGE_2 2
#define PAGE_3 3
#define PAGE_4 4
#define PAGE_5 5
#define PAGE_6 6
#define PAGE_7 7


void OLED_CommandByte(I2C_Handler_t *ptrHandlerI2C, uint8_t dataToWrite);

void OLED_DataByte(I2C_Handler_t *ptrHandlerI2C, char *dataToWrite);

void OLED_Init(I2C_Handler_t *ptrHandlerI2C);

void OLED_Clean(I2C_Handler_t *ptrHandlerI2C);

void OLED_SETPage(I2C_Handler_t *ptrHandlerI2C, char* *dataPage, uint8_t numberPage);

char* OLED_VarToChar(char character);

void OLED_FPrintPage(I2C_Handler_t *ptrHandlerI2C, char *ptrVarToPrint, uint8_t numberPage);

void OLED_FPrint(I2C_Handler_t *ptrHandlerI2C, char *varToPrint);

#endif /* OLEDDRIVER_H_ */
