#ifndef __MYLIST_H
#define __MYLIST_H
#define MAX_LISTNUM 1000
#include "port_sys.h"
typedef struct
{
	void **ppItems;
	int count;
}_mylist;
typedef void *(MYLIST_FREENODE_CALLBACK)(void *);

void MyList_Free(_mylist *pList);
void MyList_Clear(_mylist *pList);
void MyList_AddNodePureData(_mylist *pList,void *pNode,size_t size);
void MyList_DeleteNode(_mylist *pList,void *pNode);
void MyList_DeleteNodeAt(_mylist *pList,int index);
int MyList_IndexOf(_mylist *pList,void *pNode);
void *MyList_GetNode(_mylist *pList,int index);
inline bool MyList_Empty(_mylist *pList)
{
	return pList->count==0;
}
_mylist * MyList_Create(void);
#endif
