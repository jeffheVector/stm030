#ifndef __PORT_SYS_H
#define __PORT_SYS_H
/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include "systick.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define AppMalloc malloc
#define AppMemCpy memcpy
#define AppMemSet memset
#define AppRealloc realloc
#define AppFree free
#define Set1(x,y) HAL_GPIO_WritePin(x,y,GPIO_PIN_SET)
#define Set0(x,y) HAL_GPIO_WritePin(x,y,GPIO_PIN_RESET)
extern uint32_t g_GlobalTimeMilliseconds;
#define APP_CORE_CURRENT_TICK g_GlobalTimeMilliseconds
#define APP_CORE_TICK_INTERVAL_MS(t1,t2) t2>=t1?t2-t1:UINT32_MAX-t1+t2

#define ERRORCODE_OK 0
#define ERRORCODE_LIST_DELNODEAT 1
#define ERRORCODE_LIST_DELNODE 2
#define ERRORCODE_SYSCONFIGCLOCK 3
#define ERRORCODE_INITUART 4
#define ERRORCODE_REGISTER485CALLBACK 5

#define ERRORCODE_MOTOR_HIGHLIMIT_IDLE 100
#define ERRORCODE_MOTOR_HIGHLIMIT_REACH 101
#define ERRORCODE_MOTOR_LOWLIMIT_IDLE 102
#define ERRORCODE_MOTOR_LOWLIMIT_REACH 103
#define ERRORCODE_MOTOR_HIGHLOWLIMIT_REACH 104
#define ERRORCODE_MOTOR_STARTMOTOR 105
#define ERRORCODE_MOTOR_STOPMOTOR 106

#define ERRORCODE_HDEP_CONNECTRELAY 170

#define ERRORCODE_LCD_INIT 130
void Error_Handler(u32 errorCode);
void PRINTF(char *format,...);
extern bool DebugLogBy485;
#endif
