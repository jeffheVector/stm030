#include "motor.h"
#include "gwtask_timer.h"
#ifdef MOTOR485
#define M1_HIGH_PORT GPIOB
#define M1_HIGH_PIN GPIO_PIN_1
#define M1_LOW_PORT GPIOB
#define M1_LOW_PIN GPIO_PIN_0
#define M2_HIGH_PORT GPIOA
#define M2_HIGH_PIN GPIO_PIN_7
#define M2_LOW_PORT GPIOA
#define M2_LOW_PIN GPIO_PIN_6
_motor_info motorInfos[MAXMOTOR_NUM];
#define MOTORSTOP_TIMEOUT 200

//延迟200毫秒后置彻底停止标志
static void DelaySet1StoppedStatus(void)
{
	motorInfos[0].status=MOTORSTATUS_STOPPED;
}

//延迟200毫秒后置彻底停止标志
static void DelaySet2StoppedStatus(void)
{
	motorInfos[1].status=MOTORSTATUS_STOPPED;
}

static void SetMotorStop(u8 motorIndex)
{
	SetPWMStop(motorIndex);
	if(motorIndex==0)
	{
		GWTaskTimer_SetTimer(MOTORSTOP_TIMEOUT,true,DelaySet1StoppedStatus);
	}
	else
	{
		GWTaskTimer_SetTimer(MOTORSTOP_TIMEOUT,true,DelaySet2StoppedStatus);
	}
}

static void ReportMotorInfo(void *pData)
{
	Send485(pData,MAX485PKG_LEN);
	AppFree(pData);
}

//static void MakeMotor485Data(u8 *pDataPackage,u8 funcCode,u8 index,u8 *data)
//{
//	u8 data485[4];
//	data485[0]=0;
//	data485[1]=index;
//	data485[2]=data[0];
//	data485[3]=data[1];
//	Make485Data(pDataPackage,funcCode,data485);
//}

static void Callback_StartMotor(u8 *data)
{
	u8 motorIndex=data[2];
	u8 resultInfo[4]={0};//=data+3;
	resultInfo[0]=motorIndex;
	u8 motorDir=data[3];
	resultInfo[1]=motorDir;
	resultInfo[2]=ERRORCODE_OK;
	if(motorInfos[motorIndex].status!=MOTORSTATUS_STOPPED)
	{
		resultInfo[2]=ERRORCODE_MOTOR_STARTMOTOR;
	}
	else
	{
		u8 timeOut=data[4];
		//去启动闸门
		if(motorDir==MOTORSTART_FORWARD)
		{
			//上升只有在不在上限位，且是停止状态下
			if(motorInfos[motorIndex].highState!=MOTORLIMIT_IDLE)
			{
				resultInfo[2]=ERRORCODE_MOTOR_HIGHLIMIT_REACH;
			}
			else
			{
				SetForward(motorInfos[motorIndex].pwmIndex,timeOut,100);
				motorInfos[motorIndex].status=MOTORSTATUS_FORWARD;
			}
		}
		else
		{
			if(motorInfos[motorIndex].lowState!=MOTORLIMIT_IDLE)
			{
				resultInfo[2]=ERRORCODE_MOTOR_LOWLIMIT_REACH;
			}
			else
			{
				SetBackward(motorInfos[motorIndex].pwmIndex,timeOut,100);
				motorInfos[motorIndex].status=MOTORSTATUS_BACKWARD;
			}
		}
	}
	u8 *pDataPackage=AppMalloc(MAX485PKG_LEN);
	Make485Data(pDataPackage,MOTORCMD485_REPORTSTARTMOTOR,resultInfo);
	//Make485Data(pDataPackage,MOTORCMD485_REPORTSTARTMOTOR,motorIndex,resultInfo);
	GWTaskTimer_SetAttachTimer(1,true,pDataPackage,ReportMotorInfo);
	
}

static void Callback_StopMotor(u8 *data)
{
	u8 motorIndex=data[2];
	u8 resultInfo[4]={0};//=data+3;
	resultInfo[0]=motorIndex;
	bool shouldStop=true;
	switch(motorInfos[motorIndex].status)
	{
		case MOTORSTATUS_STOPPED:
		case MOTORSTATUS_STOPPING:
			shouldStop=false;
			//resultInfo[2]=ERRORCODE_MOTOR_STOPMOTOR;
			break;
	}
	if(shouldStop)//resultInfo[2]==ERRORCODE_OK)
	{
		SetMotorStop(motorIndex);
		
	}
	u8 *pDataPackage=AppMalloc(MAX485PKG_LEN);
	Make485Data(pDataPackage,MOTORCMD485_REPORTSTOPMOTOR,resultInfo);
	GWTaskTimer_SetAttachTimer(0,true,pDataPackage,ReportMotorInfo);
//	u8 *pDataPackage=AppMalloc(MAX485PKG_LEN);
//	MakeMotor485Data(pDataPackage,MOTORCMD485_REPORTSTOPMOTOR,motorIndex,resultInfo);
//	GWTaskTimer_SetAttachTimer(0,true,pDataPackage,ReportMotorInfo);
}

static void Callback_QueryMotor(u8 *data)
{
	u8 motorIndex=data[2];
	//u8 *pRtnData=data+2;
	u8 resultInfo[4];//=data+3;
	resultInfo[0]=motorIndex;
	resultInfo[1]=motorInfos[motorIndex].status;
	resultInfo[2]=motorInfos[motorIndex].highState;
	resultInfo[3]=motorInfos[motorIndex].lowState;
	
	u8 *pDataPackage=AppMalloc(MAX485PKG_LEN);
	Make485Data(pDataPackage,MOTORCMD485_REPORTMOTORINFO,resultInfo);
	GWTaskTimer_SetAttachTimer(1,true,pDataPackage,ReportMotorInfo);
}
static void Timer_DealLimit1(void);
static void Timer_DealLimit2(void);
void Motor_Init(void)
{
	Register485Callback(localSubAddr,MOTORCMD485_STARTMOTOR,Callback_StartMotor);
	Register485Callback(localSubAddr,MOTORCMD485_STOPMOTOR,Callback_StopMotor);
	Register485Callback(localSubAddr,MOTORCMD485_QUERYMOTORINFO,Callback_QueryMotor);
	PWM_Init();
	motorInfos[0].highState=MOTORLIMIT_IDLE;
	motorInfos[0].lowState=MOTORLIMIT_IDLE;
	motorInfos[0].PORT_HIGH=M1_HIGH_PORT;
	motorInfos[0].PIN_HIGH=M1_HIGH_PIN;
	motorInfos[0].PORT_LOW=M1_LOW_PORT;
	motorInfos[0].PIN_LOW=M1_LOW_PIN;
	motorInfos[0].pwmIndex=0;
	motorInfos[0].status=MOTORSTATUS_STOPPED;
	motorInfos[0].highStateNums.readNum=0;
	motorInfos[0].highStateNums.idleNum=0;
	motorInfos[0].highStateNums.reachNum=0;
	motorInfos[0].lowStateNums.readNum=0;
	motorInfos[0].lowStateNums.idleNum=0;
	motorInfos[0].lowStateNums.reachNum=0;
	
	motorInfos[1].highState=MOTORLIMIT_IDLE;
	motorInfos[1].lowState=MOTORLIMIT_IDLE;
	motorInfos[1].PORT_HIGH=M2_HIGH_PORT;
	motorInfos[1].PIN_HIGH=M2_HIGH_PIN;
	motorInfos[1].PORT_LOW=M2_LOW_PORT;
	motorInfos[1].PIN_LOW=M2_LOW_PIN;
	motorInfos[1].pwmIndex=0;
	motorInfos[1].status=MOTORSTATUS_STOPPED;
	motorInfos[1].highStateNums.readNum=0;
	motorInfos[1].highStateNums.idleNum=0;
	motorInfos[1].highStateNums.reachNum=0;
	motorInfos[1].lowStateNums.readNum=0;
	motorInfos[1].lowStateNums.idleNum=0;
	motorInfos[1].lowStateNums.reachNum=0;
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Pin = motorInfos[0].PIN_HIGH;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(motorInfos[0].PORT_HIGH, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = motorInfos[0].PIN_LOW;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(motorInfos[0].PORT_LOW, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = motorInfos[1].PIN_HIGH;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(motorInfos[1].PORT_HIGH, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = motorInfos[1].PIN_LOW;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(motorInfos[1].PORT_LOW, &GPIO_InitStruct);
	Set1(motorInfos[0].PORT_HIGH, motorInfos[0].PIN_HIGH);//, GPIO_PIN_RESET);
	Set1(motorInfos[0].PORT_LOW, motorInfos[0].PIN_LOW);//, GPIO_PIN_RESET);
	Set1(motorInfos[1].PORT_HIGH, motorInfos[1].PIN_HIGH);//, GPIO_PIN_RESET);
	Set1(motorInfos[1].PORT_LOW, motorInfos[1].PIN_LOW);//, GPIO_PIN_RESET);
	
	
	GWTaskTimer_SetTimer(1,false,Timer_DealLimit1);
	//pIndex=AppMalloc(1);
	//*pIndex=1;
	GWTaskTimer_SetTimer(1,false,Timer_DealLimit2);
}
//需要去抖动
#define MAX_READSTATENUM 10
#define MAX_STABLENUM 8
static u8 CheckLimitState(_limit_statenums stateNums)
{
	if(stateNums.readNum>=MAX_READSTATENUM)
	{
		if(stateNums.idleNum>=MAX_STABLENUM)
		{
			return MOTORLIMIT_IDLE;
		}
		else if(stateNums.reachNum>=MAX_STABLENUM)
		{
			return MOTORLIMIT_REACH;
		}
		else
		{
			return MOTORLIMIT_NOISE;
		}
	}
	else
	{
		return MOTORLIMIT_WAITSTABLE;
	}
}

static void ClearLimitStateNums(_limit_statenums *pStateNums)
{
	pStateNums->idleNum=0;
	pStateNums->reachNum=0;
	pStateNums->readNum=0;
}

static void Timer_DealLimit(u8 index)
{
	//u8 index=*((u8 *)pIndex);
	motorInfos[index].highStateNums.readNum++;
	if(HAL_GPIO_ReadPin(motorInfos[index].PORT_HIGH, motorInfos[index].PIN_HIGH) == GPIO_PIN_SET)
	//if(HAL_GPIO_ReadPin(motorInfos[index].PORT_HIGH, motorInfos[index].PIN_HIGH) == GPIO_PIN_RESET)//PULLDOWN
	{
		motorInfos[index].highStateNums.reachNum++;
	}
	else
	{
		motorInfos[index].highStateNums.idleNum++;
	}
	switch(CheckLimitState(motorInfos[index].highStateNums))
	{
		case MOTORLIMIT_WAITSTABLE:
			break;
		case MOTORLIMIT_NOISE:
			ClearLimitStateNums(&motorInfos[index].highStateNums);
		break;
		case MOTORLIMIT_IDLE:
			if(motorInfos[index].highState!=MOTORLIMIT_IDLE)
			{
				#ifdef DEBUG
				PRINTF("MOTOR_%d:IDLE HIGH\r\n",index);
				#endif
				motorInfos[index].highState=MOTORLIMIT_IDLE;
			}
			ClearLimitStateNums(&motorInfos[index].highStateNums);
			break;
		case MOTORLIMIT_REACH:
			if(motorInfos[index].highState!=MOTORLIMIT_REACH)
			{
				#ifdef DEBUG
				PRINTF("MOTOR_%d:REACH HIGH\r\n",index);
				#endif
				motorInfos[index].highState=MOTORLIMIT_REACH;
				switch(motorInfos[index].status)
				{
					//case MOTORSTATUS_BACKWARD:
					case MOTORSTATUS_FORWARD:
						SetMotorStop(index);
						break;
				}
				
			}
			ClearLimitStateNums(&motorInfos[index].highStateNums);
			break;
	}
	motorInfos[index].lowStateNums.readNum++;
	if(HAL_GPIO_ReadPin(motorInfos[index].PORT_LOW, motorInfos[index].PIN_LOW) == GPIO_PIN_SET)
	//	if(HAL_GPIO_ReadPin(motorInfos[index].PORT_LOW, motorInfos[index].PIN_LOW) == GPIO_PIN_RESET)//pulldown
	{
		motorInfos[index].lowStateNums.reachNum++;
	}
	else
	{
		motorInfos[index].lowStateNums.idleNum++;
	}
	switch(CheckLimitState(motorInfos[index].lowStateNums))
	{
		case MOTORLIMIT_WAITSTABLE:
			break;
		case MOTORLIMIT_NOISE:
			ClearLimitStateNums(&motorInfos[index].lowStateNums);
		break;
		case MOTORLIMIT_IDLE:
			if(motorInfos[index].lowState!=MOTORLIMIT_IDLE)
			{
				PRINTF("MOTOR_%d:IDLE LOW\r\n",index);
				motorInfos[index].lowState=MOTORLIMIT_IDLE;
			}
			ClearLimitStateNums(&motorInfos[index].lowStateNums);
			break;
		case MOTORLIMIT_REACH:
			if(motorInfos[index].lowState!=MOTORLIMIT_REACH)
			{
				PRINTF("MOTOR_%d:REACH LOW\r\n",index);
				motorInfos[index].lowState=MOTORLIMIT_REACH;
				switch(motorInfos[index].status)
				{
					case MOTORSTATUS_BACKWARD:
					//case MOTORSTATUS_FORWARD:
						SetMotorStop(index);
						break;
				}
			}
			ClearLimitStateNums(&motorInfos[index].lowStateNums);
			break;
	}
}
static void Timer_DealLimit1(void)
{
	Timer_DealLimit(0);
}
static void Timer_DealLimit2(void)
{
	Timer_DealLimit(1);
}
//static void Timer_DealLimit(u8 index)
//{
//	//低电平表示常闭，没到限位
//	if(HAL_GPIO_ReadPin(motorInfos[index].PORT_HIGH, motorInfos[index].PIN_HIGH) == GPIO_PIN_SET)
//	{
//		//PRINTF("HUGH\r\n");
//		if(motorInfos[index].highState!=MOTORLIMIT_HIGH_IDLE)
//		{
//			//
//			//PRINTF("IDLE HIGH\r\n");
//			motorInfos[index].highState=MOTORLIMIT_HIGH_IDLE;
//		}
//		else
//		{
////			u8 *pDataPackage=AppMalloc(MAX485PKG_LEN);
////			//错误要上
////			u8 erroCode[2];
////			erroCode[0]=0;
////			erroCode[1]=ERRORCODE_MOTOR_HIGHLIMIT_IDLE;
////			MakeMotor485Data(pDataPackage,CMD485_REPORTGATEINFO,index,erroCode);
////			GWTaskTimer_SetAttachTimer(0,true,pDataPackage,ReportMotorInfo);
//		}
//	}
//	else
//	{
//		//PRINTF("LOW\r\n");
//		if(motorInfos[index].highState!=MOTORLIMIT_HIGH_REACH)
//		{
//			//PRINTF("REACH HIGH\r\n");
//			motorInfos[index].highState=MOTORLIMIT_HIGH_REACH;
//			//SetStop(index);
//		}
//		else
//		{
////			u8 *pDataPackage=AppMalloc(MAX485PKG_LEN);
////			//错误要上
////			u8 erroCode[2];
////			erroCode[0]=0;
////			erroCode[1]=ERRORCODE_MOTOR_HIGHLIMIT_REACH;
////			MakeMotor485Data(pDataPackage,CMD485_REPORTGATEINFO,index,erroCode);
////			GWTaskTimer_SetAttachTimer(0,true,pDataPackage,ReportMotorInfo);
//		}
//	}
//	if(HAL_GPIO_ReadPin(motorInfos[index].PORT_LOW, motorInfos[index].PIN_LOW) == GPIO_PIN_SET)
//	{
//		if(motorInfos[index].lowState!=MOTORLIMIT_LOW_IDLE)
//		{
//			//PRINTF("%d:IDLE LOW\r\n",index);
//			motorInfos[index].lowState=MOTORLIMIT_LOW_IDLE;
//		}
//		else
//		{
////			u8 *pDataPackage=AppMalloc(MAX485PKG_LEN);
////			//错误要上
////			u8 erroCode[2];
////			erroCode[0]=0;
////			erroCode[1]=ERRORCODE_MOTOR_LOWLIMIT_IDLE;
////			MakeMotor485Data(pDataPackage,CMD485_REPORTGATEINFO,index,erroCode);
////			GWTaskTimer_SetAttachTimer(0,true,pDataPackage,ReportMotorInfo);
//		}
//	}
//	else
//	{
//		if(motorInfos[index].lowState!=MOTORLIMIT_LOW_REACH)
//		{
//			//PRINTF("%d:REACH LOW\r\n",index);
//			motorInfos[index].lowState=MOTORLIMIT_LOW_REACH;
//			//SetStop(index);
//		}
//		else
//		{
////			u8 *pDataPackage=AppMalloc(MAX485PKG_LEN);
////			//错误要上
////			u8 erroCode[2];
////			erroCode[0]=0;
////			erroCode[1]=ERRORCODE_MOTOR_LOWLIMIT_REACH;
////			MakeMotor485Data(pDataPackage,CMD485_REPORTGATEINFO,index,erroCode);
////			GWTaskTimer_SetAttachTimer(0,true,pDataPackage,ReportMotorInfo);
//		}
//	}
//	if(motorInfos[index].lowState==MOTORLIMIT_LOW_REACH && motorInfos[index].highState==MOTORLIMIT_HIGH_REACH)
//	{
////		u8 *pDataPackage=AppMalloc(MAX485PKG_LEN);
////			//错误要上
////			u8 erroCode[2];
////			erroCode[0]=0;
////			erroCode[1]=ERRORCODE_MOTOR_HIGHLOWLIMIT_REACH;
////			MakeMotor485Data(pDataPackage,CMD485_REPORTGATEINFO,index,erroCode);
////			GWTaskTimer_SetAttachTimer(0,true,pDataPackage,ReportMotorInfo);
//	}
//}

void Motor_GetStatus(u8 motorIndex,char *buffer,u8 maxLen)
{
	char status[9]={0};
	char lowinfo[8]={0};
	char highinfo[8]={0};
	switch(motorInfos[motorIndex].status)
	{
		case MOTORSTATUS_STOPPED:
			sprintf(status,"STOPPED");
		break;
		case MOTORSTATUS_STOPPING:
			sprintf(status,"STOPPING");
		break;
		case MOTORSTATUS_FORWARD:
			sprintf(status,"FORWARD");
		break;
		case MOTORSTATUS_BACKWARD:
			sprintf(status,"BACKWARD");
		break;
		default:
			sprintf(status,"ERROR:%d",motorInfos[motorIndex].status);
		break;
	}
	switch(motorInfos[motorIndex].highState)
	{
		case MOTORLIMIT_IDLE:
			sprintf(highinfo,"IDLE");
		break;
		case MOTORLIMIT_REACH:
			sprintf(highinfo,"REACH");
		break;
		default:
			sprintf(highinfo,"ERR:%d",motorInfos[motorIndex].highState);
		break;
	}
	switch(motorInfos[motorIndex].lowState)
	{
		case MOTORLIMIT_IDLE:
			sprintf(lowinfo,"IDLE");
		break;
		case MOTORLIMIT_REACH:
			sprintf(lowinfo,"REACH");
		break;
		default:
			sprintf(lowinfo,"ERR:%d",motorInfos[motorIndex].lowState);
		break;
	}
	snprintf(buffer,maxLen,"MOTOR_%d:STATUS:%s,HIGH:%s,LOW:%s",
		motorIndex,status,highinfo,lowinfo);
}

/*
	限位查询
*/
//void Motor_Deal(void)
//{
//	//DealLimit(0);
//	DealLimit(1);
//}

bool Motor_StartForward(u8 motorIndex,u32 timeOut)
{
	if(motorInfos[motorIndex].status!=MOTORSTATUS_STOPPED)
	{
		return false;
	}
	motorInfos[motorIndex].status=MOTORSTATUS_FORWARD;
	SetForward(motorIndex,timeOut,100);
	return true;
}

bool Motor_StartBackward(u8 motorIndex,u32 timeOut)
{
	if(motorInfos[motorIndex].status!=MOTORSTATUS_STOPPED)
	{
		return false;
	}
	motorInfos[motorIndex].status=MOTORSTATUS_BACKWARD;
	SetBackward(motorIndex,timeOut,100);
	return true;
}

#endif
