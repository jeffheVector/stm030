#ifndef __PWM_H
#define __PWM_H
#ifdef MOTOR485
#include "port_sys.h"
/*
u32 pwmTick = 0;
u32 pwmFreq = 3333;
u32 dutyCycle = 20, highMaxValue;
u32 pwmMaxValue;
*/
typedef struct
{
	bool isHigh;
	u32 tickCurrent;
	u32 fulTimeOut;//毫秒
	u32 usedMs;//已经用了多长时间，毫秒计算
	u32 dutyCycle;
	u32 dutyMax;
	u32 highMaxValue;
	u32 maxTickCount;
	GPIO_TypeDef *PORT1,*PORT2,*PORTPWM;
	u16 PIN1,PIN2,PINPWM;
}_pwm_info;

//typedef struct
//{
//	//u8 dir;//方向
//	u32 timeOut;//毫秒
//	u32 usedMs;//已经用了多长时间，毫秒计算
//	u8 pwmIndex;
//}_pwmadjust;

void PWM_Init(void);
void PWM_Tick(void);
void SetForward(u8 index,u8 fullTimeOut,u32 dutyMax);
void SetBackward(u8 index,u8 fullTimeOut,u32 dutyMax);
void SetPWMStop(u8 index);
#endif
#endif
