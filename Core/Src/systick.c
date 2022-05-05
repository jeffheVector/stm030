#include "systick.h"
#include "main.h"
static __IO uint32_t TimingDelay;
uint32_t g_GlobalTimeMilliseconds;
static uint32_t sysTicks=0;
/**
  * @brief  ����ϵͳ�δ�ʱ�� SysTick
  * @param  ��
  * @retval ��
  */
void SysTick_Init(void)
{
	/* SystemFrequency / 1000    1ms�ж�һ��
	 * SystemFrequency / 100000	 10us�ж�һ��
	 * SystemFrequency / 1000000 1us�ж�һ��
	 */
	if (HAL_SYSTICK_Config(SystemCoreClock / 100000))
	{ 
		/* Capture error */ 
		while (1);
	}
	sysTicks=0;
}

/**
  * @brief   us��ʱ����,10usΪһ����λ
  * @param  
  *		@arg nTime: Delay_us( 1 ) ��ʵ�ֵ���ʱΪ 1 * 10us = 10us
  * @retval  ��
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
  * @brief  ��ȡ���ĳ���
  * @param  ��
  * @retval ��
  * @attention  �� SysTick �жϺ��� SysTick_Handler()����
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
