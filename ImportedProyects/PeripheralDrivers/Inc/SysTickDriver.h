/*
 * SysTickDriver.h
 *
 *  Created on: May 2, 2022
 *      Author: namontoy
 */

#ifndef INC_SYSTICKDRIVER_H_
#define INC_SYSTICKDRIVER_H_

#include <stm32f4xx.h>

void config_SysTickMs(void);
uint64_t getTicksMs(void);

#endif /* INC_SYSTICKDRIVER_H_ */
