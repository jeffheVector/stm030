#include "pwm.h"
#include "motor.h"
#include "gwtask_timer.h"
#ifdef MOTOR485

#define M1_PWM_PORT GPIOA
#define M1_PWM_PIN GPIO_PIN_4
#define M1_1_PORT GPIOA
#define M1_1_PIN GPIO_PIN_0
#define M1_2_PORT GPIOA
#define M1_2_PIN GPIO_PIN_1

#define M2_PWM_PORT GPIOA
#define M2_PWM_PIN GPIO_PIN_5
#define M2_1_PORT GPIOA
#define M2_1_PIN GPIO_PIN_2
#define M2_2_PORT GPIOA
#define M2_2_PIN GPIO_PIN_3
_pwm_info pwmInfos[MAXMOTOR_NUM];
#define PWM_FREQ 3000
static bool pwmInited=false;
//u32 pwmTick=0;
static void DealPWM1(void);
static void DealPWM2(void);
static void CalcPWMValue(u8 index,u32 freq)
{
	pwmInfos[index].maxTickCount = 100000 / freq;
    pwmInfos[index].highMaxValue = pwmInfos[index].dutyCycle * pwmInfos[index].maxTickCount / 100;
	#ifdef DEBUG
	//PRINTF("PWM:%d,FREQ:%d,MAXTICK:%d,MAXVALUE:%d,DUTY:%d\r\n",index,freq,pwmInfos[index].maxTickCount,pwmInfos[index].highMaxValue,pwmInfos[index].dutyCycle);
	#endif
}

//一个tick是10us,1秒等1000 00
void InitPWMCoef(void)
{
	AppMemSet(pwmInfos,0,sizeof(_pwm_info)*MAXMOTOR_NUM);
	pwmInfos[0].PIN1=M1_1_PIN;
	pwmInfos[0].PIN2=M1_2_PIN;
	pwmInfos[0].PORT1=M1_1_PORT;
	pwmInfos[0].PORT2=M1_2_PORT;
	pwmInfos[0].PINPWM=M1_PWM_PIN;
	pwmInfos[0].PORTPWM=M1_PWM_PORT;
	pwmInfos[0].tickCurrent=0;
	pwmInfos[0].isHigh=false;
	pwmInfos[0].dutyCycle=0;
	pwmInfos[0].dutyMax=0;
	CalcPWMValue(0,PWM_FREQ);
	
	pwmInfos[1].PIN1=M2_1_PIN;
	pwmInfos[1].PIN2=M2_2_PIN;
	pwmInfos[1].PORT1=M2_1_PORT;
	pwmInfos[1].PORT2=M2_2_PORT;
	pwmInfos[1].PINPWM=M2_PWM_PIN;
	pwmInfos[1].PORTPWM=M2_PWM_PORT;
	pwmInfos[1].tickCurrent=0;
	pwmInfos[1].isHigh=false;
	pwmInfos[1].dutyCycle=0;
	pwmInfos[1].dutyMax=0;
	CalcPWMValue(1,PWM_FREQ);
	//GWTaskTimer_SetTimer(10,false,DealPWM1);
	//GWTaskTimer_SetTimer(100,false,DealPWM2);
	SetPWMStop(0);
	SetPWMStop(1);
	
}
#define PWMWATCH_INERVALMS 100
//#define MAXDUTY 50
static void DealWatchMotorPWM(u8 index)
{
	if(pwmInfos[index].fulTimeOut==0)
	{
		return;
	}
	if(pwmInfos[index].usedMs>pwmInfos[index].fulTimeOut)
	{
		return;
	}
	pwmInfos[index].usedMs+=PWMWATCH_INERVALMS;
	u8 newDutyCycle=pwmInfos[index].usedMs*pwmInfos[index].dutyMax/pwmInfos[index].fulTimeOut;
	if(newDutyCycle>pwmInfos[index].dutyMax)
	{
		newDutyCycle=pwmInfos[index].dutyMax;
	}
	if(pwmInfos[index].dutyCycle!=newDutyCycle)
	{
		
		pwmInfos[index].dutyCycle=newDutyCycle;
		#ifdef DEBUG
		
//		PRINTF("PWM_%d:DUTY:%d%%,USED:%d,FULL:%d\r\n",index,pwmInfos[index].dutyCycle,
//			pwmInfos[index].usedMs,pwmInfos[index].fulTimeOut);
		if(pwmInfos[index].dutyCycle==pwmInfos[index].dutyMax)
		{
			PRINTF("PWM_%d:STARTEND\r\n",index);
		}
		#endif
		CalcPWMValue(index,PWM_FREQ);
	}
}
static void Timer_WatchMotor1PWM(void)
{
	DealWatchMotorPWM(0);
}
static void Timer_WatchMotor2PWM(void)
{
	DealWatchMotorPWM(1);
}
//static void StartPWMWatch(u8 index,u8 fullTimeOut)
//{
//	//监测定时器
//	pwmInfos[index].dutyCycle=0;
//	CalcPWMValue(index,PWM_FREQ);
//	_pwmadjust *pPWMAdjust=AppMalloc(sizeof(_pwmadjust));
//	pPWMAdjust->pwmIndex=index;
//	pPWMAdjust->timeOut=fullTimeOut*1000;
//	pPWMAdjust->usedMs=0;
//	GWTaskTimer_SetAttachTimer(PWMWATCH_INERVALMS,true,pPWMAdjust,Timer_WatchMotor);
//}
static void StartPWMWatch(u8 index,u8 fullTimeOut,u32 dutyMax)
{
	#ifdef DEBUG
		PRINTF("PWM_%d:START,FULL:%d\r\n",index,pwmInfos[index].fulTimeOut);
	#endif
	pwmInfos[index].dutyCycle=0;
	pwmInfos[index].usedMs=0;
	pwmInfos[index].fulTimeOut=fullTimeOut*1000;
	pwmInfos[index].dutyMax=dutyMax;
	CalcPWMValue(index,PWM_FREQ);
}

void SetForward(u8 index,u8 fullTimeOut,u32 dutyMax)
{
	#ifdef DEBUG
		//PRINTF("PWM_%d:FORWARD NOW\r\n",index);
	#endif
	Set1(pwmInfos[index].PORT1,pwmInfos[index].PIN1);
	Set0(pwmInfos[index].PORT2,pwmInfos[index].PIN2);
	StartPWMWatch(index,fullTimeOut,dutyMax);
}

void SetBackward(u8 index,u8 fullTimeOut,u32 dutyMax)
{
	#ifdef DEBUG
		//PRINTF("PWM_%d:BACKWARD NOW\r\n",index);
	#endif
	Set0(pwmInfos[index].PORT1,pwmInfos[index].PIN1);
	Set1(pwmInfos[index].PORT2,pwmInfos[index].PIN2);
	StartPWMWatch(index,fullTimeOut,dutyMax);
}

void SetPWMStop(u8 index)
{
	pwmInfos[index].usedMs=0;
	pwmInfos[index].fulTimeOut=0;
	pwmInfos[index].dutyCycle=0;
	CalcPWMValue(index,PWM_FREQ);
	Set0(pwmInfos[index].PORT1,pwmInfos[index].PIN1);
	Set0(pwmInfos[index].PORT2,pwmInfos[index].PIN2);
}

static void DealOnePWM(u8 index)
{
    if(pwmInfos[index].tickCurrent < pwmInfos[index].highMaxValue)
    {
        if(!pwmInfos[index].isHigh)
        {
			#ifdef DEBUG
			//PRINTF("PWM_%d HIGH\r\n",index);
			#endif
            HAL_GPIO_WritePin(pwmInfos[index].PORTPWM, pwmInfos[index].PINPWM, GPIO_PIN_SET);
            pwmInfos[index].isHigh = true;
        }
    }
    else
    {
		
		//HAL_GPIO_WritePin(pwmInfos[index].PORTPWM, pwmInfos[index].PINPWM, GPIO_PIN_RESET);
        if(pwmInfos[index].isHigh)
        {
			#ifdef DEBUG
			//PRINTF("PWM_%d LOW\r\n",index);
			#endif
			HAL_GPIO_WritePin(pwmInfos[index].PORTPWM, pwmInfos[index].PINPWM, GPIO_PIN_RESET);
            //HAL_GPIO_WritePin(M1_PWM_PORT, M1_PWM_PIN, GPIO_PIN_RESET);
            pwmInfos[index].isHigh = false;
        }
    }	
}

static void DealPWM1(void)
{
	DealOnePWM(0);
}

static void DealPWM2(void)
{
	DealOnePWM(1);
}

void PWM_Tick(void)
{
	if(!pwmInited)
		return;
    //pwmTick++;
	pwmInfos[0].tickCurrent++;
	if(pwmInfos[0].tickCurrent>=pwmInfos[0].maxTickCount)
	{
		pwmInfos[0].tickCurrent=0;
		
	}
	pwmInfos[1].tickCurrent++;
	if(pwmInfos[1].tickCurrent>=pwmInfos[1].maxTickCount)
	{
		pwmInfos[1].tickCurrent=0;
	}
	DealOnePWM(0);
	DealOnePWM(1);
    //PWM_Deal();
}

void PWM_Init(void)
{
    
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Pin = M1_PWM_PIN | M1_1_PIN | M1_2_PIN|M2_PWM_PIN | M2_1_PIN | M2_2_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(M1_PWM_PORT, &GPIO_InitStruct);
    //HAL_GPIO_WritePin(M1_PWM_PORT, M1_PWM_PIN, GPIO_PIN_RESET);
	//HAL_GPIO_WritePin(M2_PWM_PORT, M2_PWM_PIN, GPIO_PIN_RESET);
	Set0(M1_PWM_PORT,M1_PWM_PIN);
    Set0(M1_1_PORT,M1_1_PIN);// HAL_GPIO_WritePin(M1_1_PORT, M1_1_PIN, GPIO_PIN_RESET);
    Set0(M1_2_PORT,M1_2_PIN);
	Set0(M2_PWM_PORT,M2_PWM_PIN);
	Set0(M2_1_PORT,M2_1_PIN);
	Set0(M2_2_PORT,M2_2_PIN);
	//HAL_GPIO_WritePin(M1_2_PORT, M1_2_PIN, GPIO_PIN_RESET);
	InitPWMCoef();
	GWTaskTimer_SetTimer(PWMWATCH_INERVALMS,false,Timer_WatchMotor1PWM);
	GWTaskTimer_SetTimer(PWMWATCH_INERVALMS,false,Timer_WatchMotor2PWM);
	pwmInited=true;
}
#endif
