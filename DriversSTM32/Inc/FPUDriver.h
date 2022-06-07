/*
 * FPUDriver.h
 *
 *  Created on: Jun 6, 2022
 *      Author: jutoroa
 */

#ifndef FPUDRIVER_H_
#define FPUDRIVER_H_

#include <stm32f4xx.h>

// Driver para inicializar la FPU

#define FPU_ENABLE		(0xF << 20);

void FPUInit(void);


#endif /* FPUDRIVER_H_ */
