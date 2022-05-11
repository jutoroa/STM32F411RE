/*
 * INITSystem.h
 *
 *  Created on: 28/04/2022
 *      Author: Juan Pablo
 */

#ifndef INC_INITSYSTEM_H_
#define INC_INITSYSTEM_H_

#include <stdio.h>

void delay(int time);
void initSystem(void);
void motorConfig(uint32_t nMotor);
void ledControl(void);

#endif /* INC_INITSYSTEM_H_ */
