/*
 * mainTarea4.c
 *
 *  Created on: 4/05/2022
 *  Author: Juan Pablo
 *
 * Esta tarea pretende implementar un sumador de dos dígitos, en donde cada unidad se aumenta debido a una interrupción
 * del EXTI, la cuál podrá estar dada por un botón, o una fotocompuerta. Los resultados de la suma se pondrán observar
 * en un Display 7 segmentos.
 */


// *************** // INCLUDES // *************** //


#include "stm32f4xx.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#define LED_PIN 5
#define LED_PORT GPIOA

void delay(uint32_t delay_ms)
{
  uint32_t i;
  for (i = 0; i < (16000000 / 14000) * delay_ms; i++);
}

int main(void)
{
  // Enable clock for GPIOA peripheral
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

  // Configure LED pin as output
  LED_PORT->MODER &= ~(3 << (LED_PIN * 2));
  LED_PORT->MODER |= (1 << (LED_PIN * 2));
  LED_PORT->OTYPER &= ~(1 << LED_PIN);
  LED_PORT->OSPEEDR &= ~(3 << (LED_PIN * 2));
  LED_PORT->PUPDR &= ~(3 << (LED_PIN * 2));

  while (1)
  {
    LED_PORT->ODR ^= (1 << LED_PIN);
    delay(500);
  }
}
