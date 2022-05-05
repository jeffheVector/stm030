#include "mylist.h"


_mylist * MyList_Create()
{
	
	_mylist *pList=AppMalloc(sizeof(_mylist));
	pList->ppItems=NULL;
	pList->count=0;
	return pList;
	
}
void *MyList_GetNode(_mylist *pList,int index)
{
	return pList->ppItems[index];
}
int MyList_IndexOf(_mylist *pList,void *pNode)
{
	
	int index=-1;
	for(int i=0;i<pList->count;i++)
	{
		if(pList->ppItems[i]==pNode)
		{
			index=i;
			break;
		}
	}
	return index;
	
}
void MyList_DeleteNodeAt(_mylist *pList,int index)
{
	if(index<0 || index>=pList->count)
	{
		//assert(false);
		Error_Handler(ERRORCODE_LIST_DELNODEAT);
	}
	void *pNode=pList->ppItems[index];
	for(int i=index;i<pList->count-1;i++)
	{
		pList->ppItems[i]=pList->ppItems[i+1];
	}
	pList->count--;
	AppFree(pNode);
	u32 newSize=(pList->count)*sizeof(void *);
	if(newSize==0)
	{
		AppFree(pList->ppItems);
		pList->ppItems=NULL;
	}
	else
	{
		pList->ppItems=AppRealloc(pList->ppItems,newSize);
	}
}

void MyList_DeleteNode(_mylist *pList,void *pNode)
{
	
	int index=MyList_IndexOf(pList,pNode);
	if(index<0)
	{
		//PRINTF("ERROR MyList_DeleteNode\r\n");
		//assert(false);
		Error_Handler(ERRORCODE_LIST_DELNODE);
		//return;
	}
	MyList_DeleteNodeAt(pList,index);
	
}
void MyList_AddNodePureData(_mylist *pList,void *pNode,size_t size)
{
	
	u32 newSize=(pList->count+1)*sizeof(void *);
	if(pList->ppItems==NULL)
	{
		pList->ppItems=AppMalloc(newSize);
	}
	else
	{
		pList->ppItems=AppRealloc(pList->ppItems,newSize);
	}
	void *pNewNode=AppMalloc(size);
	AppMemCpy(pNewNode,pNode,size);
	pList->ppItems[pList->count]=pNewNode;
	pList->count++;
	
}
void MyList_Free(_mylist *pList)
{
	
	while(pList->count>0)
	{
		MyList_DeleteNode(pList,pList->ppItems[0]);
	}
	AppFree(pList);
	
}

void MyList_Clear(_mylist *pList)
{
	while(pList->count>0)
	{
		MyList_DeleteNode(pList,pList->ppItems[0]);
	}
}
