#include "myarray.h"

_myarray *MyArray_Create(u32 size)
{
	_myarray *pArr=AppMalloc(sizeof(_myarray));
	pArr->array=AppMalloc(size);
	pArr->dataSize=0;
	pArr->arraySize=size;
	return pArr;
}

void MyArray_Clear(_myarray *pArray)
{
	pArray->dataSize=0;
}

int MyArray_IndexOf(_myarray *pArray,u8 data)
{
	//int index=-1;
	for(int i=0;i<pArray->dataSize;i++)
	{
		if(pArray->array[i]==data)
		{
			return i;
		}
	}
	return -1;
}

u32 MyArray_FreeSize(_myarray *pArray)
{
	return pArray->arraySize-pArray->dataSize;
}

bool MyArray_Remove(_myarray *pArray,u32 delSize)
{
	if(delSize>pArray->dataSize)
	{
		return false;
	}
	u8 *pLastBuff=pArray->array+delSize;
	pArray->dataSize-=delSize;
	AppMemCpy(pArray->array,pLastBuff,pArray->dataSize);
	return true;
}

u8 *MyArray_FreePtrIndex(_myarray *pArray)
{
	return pArray->array+pArray->dataSize;
}

bool MyArray_IncreaseSize(_myarray *pArray,u32 incSize)
{
	if(pArray->dataSize+incSize>pArray->arraySize)
	{
		return false;
	}
	pArray->dataSize+=incSize;
	return true;
}

bool MyArray_AddBytes(_myarray *pArray,u8 *pBuff,u32 buffLen)
{
	if(pArray->dataSize+buffLen>pArray->arraySize)
	{
		return false;
	}
	AppMemCpy(pArray->array+pArray->dataSize,pBuff,buffLen);
	pArray->dataSize+=buffLen;
	return true;
}

void MyArray_Free(_myarray *pArray)
{
	AppFree(pArray->array);
	AppFree(pArray);
}
