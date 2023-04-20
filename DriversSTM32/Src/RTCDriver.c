/*
 * RTCDriver.c
 *
 *  Created on: Nov 1, 2022
 *      Author: jutoroa
 */


#include "RTCDriver.h"


void config_RTC(RTC_t *pRTC){

	// Activamos la señal de reloj para el PWR
	RCC -> APB1ENR |= RCC_APB1ENR_PWREN;

	// Desactivamos el modo Backup Protection
	PWR -> CR |= PWR_CR_DBP;

	// Activamos el LSI (LOW SPEED INTERNAL 32KHz)
	RCC -> CSR |= RCC_CSR_LSION;

	// Habilitamos la señal proveniente del LSI (LOW SPEED INTERNAL 32KHz)
	RCC ->BDCR |= RCC_BDCR_RTCSEL_1;

	// Habilitamos la compuerta AND para el RTC
	RCC -> BDCR |= RCC_BDCR_RTCEN;

	/* Iniciamos rutina de inicialización */

	// Desactivamos el Write Protection register con la clave
	RTC -> WPR = 0xCAU;
	RTC -> WPR = 0x53U;

	/* 1.0 Setiamos el bit INIT	en el ISR */
	RTC -> ISR |= RTC_ISR_INIT;

	/* 2.0 Ponemos en 1 el bit de INITF */
	RTC -> ISR |= RTC_ISR_INITF;

	/* 3.0 Configuramos el preescaler */

	RTC -> PRER |= RTC_PRER_PREDIV_A;
	RTC -> PRER |= (60 << RTC_PRER_PREDIV_S_Pos);

	/* 4.0 Cargamos los valores del calendario (DR - Date Register) y de la hora (TR - Time Register)*/

	// Escogemos el modo de 24 horas
	RTC -> CR |= (RTC_CR_FMT);
	RTC -> TR &= ~(RTC_TR_PM);

	// Escribimos las horas
	RTC -> TR |= (decToBCD(pRTC->hour)) << RTC_TR_HU_Pos;

	// Escribimos los minutos
	RTC -> TR |= (decToBCD(pRTC->minutes)) << RTC_TR_MNU_Pos;

	// Escribimos los segundos
	RTC -> TR |= (decToBCD(pRTC->seconds)) << RTC_TR_MNU_Pos;

	// Escribimos el dia
	RTC -> DR |= (decToBCD(pRTC->date)) << RTC_DR_DU_Pos;

	// Escribimos el mes
	RTC -> DR |= (decToBCD(pRTC->month)) << RTC_DR_MU_Pos;

	// Escribimos el año
	RTC -> DR |= (decToBCD(pRTC->year)) << RTC_DR_YU_Pos;

	// Escribimos el día de la semana
	RTC -> DR |= (pRTC->weekDay) << RTC_DR_WDU_Pos;

	/* 5.0 Salimos del modo de inicialización */
	RTC -> ISR &= ~RTC_ISR_INIT;

	/* 6.0 Activamos nuevamente el Write Protection */
	RTC -> WPR = 0xFFU;

}

// Función para convertir de numeros decimales a código BCD
uint8_t decToBCD(int val){
	return (uint8_t) ((val/10*16) + (val%10));
}

// Función para convertir de código BCD a numeros decimales

int BCDToDec(uint8_t val){
	return (int) ((val/16*10) + (val%16));
}
