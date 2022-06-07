/*
 * FPUDriver.c
 *
 *  Created on: Jun 6, 2022
 *      Author: jutoroa
 */


#include "FPUDriver.h"

void FPUInit(void){

	// Modificamos el System Control Block accediendo al Coprocessorr Access Control
	SCB -> CPACR |= FPU_ENABLE;

}
