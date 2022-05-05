#include "systick.h"
#include "main.h"
static __IO uint32_t TimingDelay;
uint32_t g_GlobalTimeMilliseconds;
static uint32_t sysTicks=0;
/**
  * @brief  启动系统滴答定时器 SysTick
  * @param  无
  * @retval 无
  */
void SysTick_Init(void)
{
	/* SystemFrequency / 1000    1ms中断一次
	 * SystemFrequency / 100000	 10us中断一次
	 * SystemFrequency / 1000000 1us中断一次
	 */
	if (HAL_SYSTICK_Config(SystemCoreClock / 100000))
	{ 
		/* Capture error */ 
		while (1);
	}
	sysTicks=0;
}

/**
  * @brief   us延时程序,10us为一个单位
  * @param  
  *		@arg nTime: Delay_us( 1 ) 则实现的延时为 1 * 10us = 10us
  * @retval  无
  */
void Delay_us(__IO uint32_t nTime)
{ 
	TimingDelay = nTime;


	while(TimingDelay != 0);
}

void Delay_ms(__IO uint32_t nTime)
{
	Delay_us(nTime*100);
}

/**
  * @brief  获取节拍程序
  * @param  无
  * @retval 无
  * @attention  在 SysTick 中断函数 SysTick_Handler()调用
  */
void TimingDelay_Decrement(void)
{
	sysTicks++;
	if(sysTicks>=100)
	{
		g_GlobalTimeMilliseconds++;
		sysTicks=0;
	}
	if (TimingDelay != 0x00)
	{ 
		TimingDelay--;
	}
}
/*******************/
