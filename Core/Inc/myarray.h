#ifndef __MYARRAY_H
#define __MYARRAY_H
#include "port_sys.h"
typedef struct
{
	u8 *array;
	//u8 dataSize;
	u32 arraySize;
	u32 dataSize;
}_myarray;

_myarray *MyArray_Create(u32 size);
void MyArray_Clear(_myarray *pArray);
u32 MyArray_FreeSize(_myarray *pArray);
bool MyArray_Remove(_myarray *pArray,u32 delSize);
u8 *MyArray_FreePtrIndex(_myarray *pArray);
bool MyArray_IncreaseSize(_myarray *pArray,u32 incSize);
void MyArray_Free(_myarray *pArray);
int MyArray_IndexOf(_myarray *pArray,u8 data);
#endif
