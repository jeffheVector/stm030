#include "myque.h"

bool inline MyQue_IsFull(_myque *pQue)
{
	u16 nextLast=pQue->last;
	nextLast++;
	if(nextLast>=pQue->bufferLen)
	{
		nextLast=0;
	}
	return pQue->first==nextLast;
}

bool inline MyQue_IsEmpty(_myque *pQue)
{
	return pQue->first==pQue->last;
}

_myque *MyQue_Init(uint16_t count,uint16_t dataSize)
{
	_myque *pQue=AppMalloc(sizeof(_myque));
	pQue->bufferLen=count;
	pQue->pBuffer=AppMalloc(count*dataSize);
	pQue->first=0;
	pQue->last=0;
	//pQue->size=0;
	pQue->dataSize=dataSize;
	return pQue;
}

bool MyQue_Add(_myque *pQue,void *pData)
{
	if(MyQue_IsFull(pQue))
	{
		return false;
	}
	u8 *pByte=(u8 *)pQue->pBuffer;
	void *pDataBuffer=pByte+pQue->last*pQue->dataSize;
	AppMemCpy(pDataBuffer,pData,pQue->dataSize);
	pQue->last++;
	if(pQue->last>=pQue->bufferLen)
	{
		pQue->last=0;
	}
	//PRINTF("ADD %X,last %d size %d\r\n",pDataBuffer,pQue->last,pQue->size);
	//pQue->size++;
	return true;
}

bool MyQue_AddByte(_myque *pQue,u8 byte)
{
	if(MyQue_IsFull(pQue))
	{
		return false;
	}
	
	((u8 *)pQue->pBuffer)[pQue->last]=byte;
	pQue->last++;
	if(pQue->last>=pQue->bufferLen)
	{
		pQue->last=0;
	}
	//pQue->size++;
	return true;
}

void MyQue_FetchByte(_myque *pQue,u8 *destBuff,uint16_t *buffLen)
{
//	if(*buffLen>pQue->size)
//	{
//		*buffLen=pQue->size;
//	}
	u8 *pByte=pQue->pBuffer;
	int32_t bindex=0;
	while(bindex < *buffLen)
	{
		if(MyQue_IsEmpty(pQue))
		{
			break;
		}
		destBuff[bindex]=pByte[pQue->first++];
		bindex++;
		//pos++;
		if(pQue->first>=pQue->bufferLen)
		{
			pQue->first=0;
		}
	}
	*buffLen=bindex;
	//pQue->size-=*buffLen;
	//pQue->first=pos;
}

void MyQue_Fetch(_myque *pQue,void *destBuff,uint16_t *buffLen)
{
//	if(*buffLen>pQue->size)
//	{
//		*buffLen=pQue->size;
//	}
	//u8 *pByte=pQue->pBuffer;
	u8 *pByte=(u8 *)pQue->pBuffer+pQue->first*pQue->dataSize;
	u8 *pDestByte=destBuff;
	int32_t bindex=0;
	while(bindex < *buffLen)
	{
		if(MyQue_IsEmpty(pQue))
		{
			break;
		}
		AppMemCpy(pDestByte,pByte,pQue->dataSize);//[pQue->first++];
		bindex++;
		pQue->first++;
		//pos++;
		if(pQue->first>=pQue->bufferLen)
		{
			pQue->first=0;
		}
		pDestByte+=pQue->dataSize;
		pByte=(u8 *)pQue->pBuffer+pQue->first*pQue->dataSize;
	}
	*buffLen=bindex;
	//pQue->first=pos;
}

u16 MyQue_Size(_myque *pQue)
{
	if(pQue->last>=pQue->first)
	{
		return pQue->last-pQue->first;
	}
	else
	{
		return pQue->last+pQue->bufferLen-pQue->first;
	}
}

bool MyQue_TryGetByte(_myque *pQue,u8 *destBuff,uint16_t buffLen)
{
	if(buffLen>MyQue_Size(pQue))
	{
		return false;
	}
	int32_t bindex=0;
	u8 *pByte=pQue->pBuffer;
	int32_t pos=pQue->first;
	
	while(bindex < buffLen)
	{
		destBuff[bindex]=pByte[pos++];
		bindex++;
		//pos++;
		if(pos>=pQue->bufferLen)
		{
			pos=0;
		}
	}
	return true;
}

void *MyQue_DataAt(_myque *pQue,uint16_t index)
{
	if(index>=MyQue_Size(pQue))
	{
		return NULL;
	}
	u8 *pByte=pQue->pBuffer;
	uint16_t posIndex=pQue->first+index;
	if(posIndex>=pQue->bufferLen)
	{
		posIndex-=pQue->bufferLen;
	}
	return pByte+posIndex*pQue->dataSize;
}

bool MyQue_TryGet(_myque *pQue,void *destBuff,uint16_t buffLen)
{
	if(buffLen>MyQue_Size(pQue))
	{
		return false;
	}
	int32_t bindex=0;
	//u8 *pByte=pQue->pBuffer;
	int32_t pos=pQue->first;
	u8 *pByte=(u8 *)pQue->pBuffer+pos*pQue->dataSize;
	u8 *pDestByte=destBuff;
	while(bindex < buffLen)
	{
		//destBuff[bindex]=pByte[pos++];
		AppMemCpy(pDestByte,pByte,pQue->dataSize);
		bindex++;
		pos++;
		if(pos>=pQue->bufferLen)
		{
			pos=0;
		}
		pDestByte+=pQue->dataSize;
		pByte=(u8 *)pQue->pBuffer+pos*pQue->dataSize;
	}
	return true;
}

bool MyQue_Remove(_myque *pQue,uint16_t len,MYQUE_REMOVECALLBACK callback)
{
	if(len>MyQue_Size(pQue))
	{
		return false;
	}
	if(callback!=NULL)
	{
		int i=0;
		u8 *pByte=(u8 *)pQue->pBuffer;
		while(i<len)
		{
			int index=pQue->first+i;
			if(index>=pQue->bufferLen)
			{
				index-=pQue->bufferLen;
			}
			u8 *pNode=pByte+index*pQue->dataSize;
			callback(pNode);
			i++;
		}
	}
	pQue->first+=len;
	if(pQue->first>=pQue->bufferLen)
	{
		pQue->first-=pQue->bufferLen;
	}
	//pQue->size-=len;
	return true;	
}
