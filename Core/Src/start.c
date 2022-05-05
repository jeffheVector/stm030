#include "start.h"
#include "485.h"
#ifdef START
#define SUB485ADDR_1 0X01//��һ��485�豸��ַ����Ҫ��ѯ���Ǹ�485
#define SUB485ADDR_2 0X02//Ҫ���͸����485����
#define FUNC485_1 0x03
//#define FUNC485_1 0x03

static void Callback_DealTest1(u8 *pData)
{
	PRINTF("Callback_DealTest1\r\n");
}

void Start_Init(void)
{
	//��ʼ��
	PRINTF("INIT START\r\n");
	Register485Callback(SUB485ADDR_1,FUNC485_1,Callback_DealTest1);
}

#endif
