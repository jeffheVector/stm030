#ifndef __SYSTICK_H
#define __SYSTICK_H
#include "stm32f0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
void TimingDelay_Decrement(void);
void SysTick_Init(void);
void Delay_us(__IO uint32_t nTime);
void Delay_ms(__IO uint32_t nTime);
#endif
