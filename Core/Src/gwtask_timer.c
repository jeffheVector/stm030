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
	//����ʱ�����ţ���������
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
//			//���ģʽ���û�����ģʽ������ȫ����ģʽ
//			//ֻ������������룬ϵͳ��ʼ���У������˹���������
//			break;
//		case SYSRUN_MODE_WORK:
//			//����û���ʱ�䲻����������빤��ģʽ��
//			//��������ģʽ��ʱ���ж���Ҫ�����������
//			//���������жϻ��ѣ������յ����Ż���Զ�̿�������
//			break;
//		case SYSRUN_MODE_SLEEP:
//			//����ģʽ�£�һ��û�������ˣ����������ģʽ������ģʽֻ��һ�������ж�ʱ�䣬����ϵͳ���ÿ��ǲ���Ҫ������
//			//�����Ҫ������������裬����һϵ������
//			//��μ����Ҫ��ص��ж��������������GPRS�Ƿ��յ����ŵ�
//			break;
//	}
//	
//	if(SystemRunMode==SYSRUN_MODE_WORK)
//	{
//		//�����ǰû��������ô��������״̬
//		//SystemRunMode=SYSRUN_MODE_SLEEP;
//	}
//	else
//	{
//		//�����Ҫ�ɼ�����ô������һϵ������
//	}
}
