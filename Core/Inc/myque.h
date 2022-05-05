#ifndef __MYQUE_H
#define __MYQUE_H
#include "port_sys.h"
typedef void (* MYQUE_REMOVECALLBACK)(void *pNode);
typedef struct
{
	uint16_t bufferLen;
	uint16_t first,last;
	//uint16_t size;
	uint16_t dataSize;
	void *pBuffer;
}_myque;
void MyQue_FetchByte(_myque *pQue,u8 *destBuff,uint16_t *buffLen);
bool MyQue_TryGetByte(_myque *pQue,u8 *destBuff,uint16_t buffLen);
bool MyQue_Remove(_myque *pQue,uint16_t len,MYQUE_REMOVECALLBACK callback);
bool MyQue_AddByte(_myque *pQue,u8 byte);
void MyQue_Fetch(_myque *pQue,void *destBuff,uint16_t *buffLen);
bool MyQue_TryGet(_myque *pQue,void *destBuff,uint16_t buffLen);
bool MyQue_Add(_myque *pQue,void *pData);
void *MyQue_DataAt(_myque *pQue,uint16_t index);
bool MyQue_IsFull(_myque *pQue);
bool MyQue_IsEmpty(_myque *pQue);
u16 MyQue_Size(_myque *pQue);

_myque *MyQue_Init(uint16_t count,uint16_t dataSize);
#endif
