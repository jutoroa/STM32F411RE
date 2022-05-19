/*
 * SysTickDriver.h
 *
 *  Created on: May 12, 2022
 *      Author: Juan Pablo
 */

#ifndef SYSTICKDRIVER_H_
#define SYSTICKDRIVER_H_

#include <stm32f4xx.h>

void config_SysTickMs(void);
uint64_t getTickMs(void);

#endif /* SYSTICKDRIVER_H_ */
