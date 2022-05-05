#ifndef __GWTASK_TIMER_H
#define __GWTASK_TIMER_H
#include "port_sys.h"
//主要业务处理再这里
//定时任务
typedef void (* GWTASKTIMER_CALLBACK)(void);
typedef void (* GWTASKTIMER_ATTACHCALLBACK)(void * pAttach);

void GWTaskTimer_Init(void);
void GWTaskTimer_Deal(void);
void * GWTaskTimer_SetAttachTimer(u32 mm,bool once,void *pAttach,GWTASKTIMER_ATTACHCALLBACK callback);
void * GWTaskTimer_SetTimer(u32 mm,bool once,GWTASKTIMER_CALLBACK callback);
#endif
