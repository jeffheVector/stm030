#include "gwtask_timer.h"
//#include "gwtask.h"
//#include "oper.h"
//#include "sysrun.h"
#include "mylist.h"

typedef struct
{
	u32 beginTick;
	bool once;
	u32 timeOutMS;
	void *pAttach;
	GWTASKTIMER_CALLBACK callback;
	GWTASKTIMER_ATTACHCALLBACK attachCallback;
}_timer;
_mylist *PTimerList;
//static _datetime dateTime_CurrentOld;
//_datetime dateTime_Current;
//static u32 runTimerCount=0;

void GWTaskTimer_Init()
{
	//AppGetCurrentDateTime(&dateTime_CurrentOld);
	//AppGetCurrentDateTime(&dateTime_Current);
	PTimerList=MyList_Create();
}

void * GWTaskTimer_SetAttachTimer(u32 ms,bool once,void *pAttach,GWTASKTIMER_ATTACHCALLBACK attachCallback)
{
	if(attachCallback==NULL)
	{
		return false;
	}
	_timer timer;
	timer.beginTick=APP_CORE_CURRENT_TICK;
	timer.attachCallback=attachCallback;
	timer.callback=NULL;
	timer.timeOutMS=ms;
	timer.once=once;
	timer.pAttach=pAttach;
	MyList_AddNodePureData(PTimerList,&timer,sizeof(timer));
	return MyList_GetNode(PTimerList,PTimerList->count-1);
}

void *GWTaskTimer_SetTimer(u32 ms,bool once,GWTASKTIMER_CALLBACK callback)
{
	if(callback==NULL)
	{
		return false;
	}
	//PRINTF("CCC\r\n");
	_timer timer;
	timer.beginTick=APP_CORE_CURRENT_TICK;
	timer.attachCallback=NULL;
	timer.callback=callback;
	timer.timeOutMS=ms;
	timer.once=once;
	timer.pAttach=NULL;
	MyList_AddNodePureData(PTimerList,&timer,sizeof(timer));
	//PRINTF("CCC\r\n");
	return MyList_GetNode(PTimerList,PTimerList->count-1);
}

void GWTaskTimer_Deal()
{
	//根据时间流逝，创建任务
	//AppGetCurrentDateTime(&dateTime_Current);
	int i=0;
	
	while(i<PTimerList->count)
	{
		u32 tickNow=APP_CORE_CURRENT_TICK;
		_timer *pTimer=MyList_GetNode(PTimerList,i);
		u32 timeOutMM=APP_CORE_TICK_INTERVAL_MS(pTimer->beginTick,tickNow);
		if(timeOutMM>pTimer->timeOutMS)
		{
			//PRINTF(" BEGIN %d NOW %d\r\n",pTimer->beginTick,tickNow);
			if(pTimer->pAttach!=NULL)
			{
				if(pTimer->attachCallback!=NULL)
				{
					pTimer->attachCallback(pTimer->pAttach);
				}
			}
			else if(pTimer->callback!=NULL)
			{
				pTimer->callback();
			}
			pTimer->beginTick=tickNow;
			if(pTimer->once)
			{
				MyList_DeleteNode(PTimerList,pTimer);
				//PRINTF("I:%d runTimerCount1 %d\r\n",i,PTimerList->count);
				continue;
			}
			
		}
		i++;
	}
//	if(PTimerList->count!=runTimerCount)
//	{
//		runTimerCount=PTimerList->count;
//		PRINTF("runTimerCount %d\r\n",runTimerCount);
//	}
//	
//	switch(SystemRunMode)
//	{
//		case SYSRUN_MODE_OPER:
//			//这个模式是用户操作模式，属于全性能模式
//			//只有两种情况进入，系统初始运行，或者人工按键唤醒
//			break;
//		case SYSRUN_MODE_WORK:
//			//如果用户长时间不操作，则进入工作模式；
//			//或者休眠模式定时器判断需要工作了则进入
//			//或者特殊中断唤醒，比如收到短信或者远程控制命令
//			break;
//		case SYSRUN_MODE_SLEEP:
//			//工作模式下，一旦没有任务了，则进入休眠模式，休眠模式只有一个任务判断时间，根据系统设置看是不是要工作了
//			//如果需要工作，则打开外设，创建一系列任务
//			//其次监控需要监控的中断数据情况，比如GPRS是否收到短信等
//			break;
//	}
//	
//	if(SystemRunMode==SYSRUN_MODE_WORK)
//	{
//		//如果当前没有任务，那么进入休眠状态
//		//SystemRunMode=SYSRUN_MODE_SLEEP;
//	}
//	else
//	{
//		//如果需要采集了那么，创建一系列任务
//	}
}
